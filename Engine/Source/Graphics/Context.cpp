#include <Jora/Graphics/Context.hpp>

#include <Jora/Graphics/Window.hpp>

#include <Jora/Core/Logger.hpp>

#include <GLFW/glfw3.h>

namespace
{

	/**
	 * @brief Создает через функцию библиотеки GLFW VkSurface.
	 * @param window Созданное окно.
	 * @return Результат выполнения glfwCreateWindowSurface(...)
	 */

	vk::Result CreateWindowSurface(VkInstance instance, GLFWwindow* window, VkSurfaceKHR& surface)
	{
		return static_cast<vk::Result>(glfwCreateWindowSurface(instance, window, nullptr, &surface));
	}

}

namespace jora
{

	Context::~Context()
	{
		vkbSwapchain.destroy_image_views(vkImageViews);
		MessageLogging::Message("Destroy VkSwapchainKHR image views from memory.", MessageType::Success);

		if (vkbSwapchain.swapchain != nullptr)
		{
			vkb::destroy_swapchain(vkbSwapchain);
			MessageLogging::Message("Destroy VkSwapchainKHR from memory.", MessageType::Success);
		}

		if (vkbDevice.device != nullptr)
		{
			vkb::destroy_device(vkbDevice);
			MessageLogging::Message("Destroy VkDevice from memory.", MessageType::Success);
		}

		if (vkSurface != nullptr)
		{
			vkb::destroy_surface(vkbInstance, vkSurface);
			MessageLogging::Message("Destroy VkSurface from memory.", MessageType::Success);
		}

		if (vkbInstance.instance != nullptr)
		{
			vkb::destroy_instance(vkbInstance);
			MessageLogging::Message("Destroy VkInstance from memory.", MessageType::Success);
		}
	}

	vk::Result Context::Init(GLFWwindow* window) noexcept
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			MessageLogging::Message("Failed initialize Volk.", MessageType::Error);
			return vk::Result::eErrorInitializationFailed;
		}

		const auto [extCount, extStr] = GetWindowExtensions();

		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		const auto instanceResult = vkb::InstanceBuilder{}
			.set_engine_name("Jora Engine")
			.enable_validation_layers()
			.require_api_version(1, 3, 0)
			.use_default_debug_messenger()
			.enable_extensions(extCount, extStr)
			.build();
		if (!instanceResult)
		{
			MessageLogging::Message("Failed create VkInstance.", MessageType::Error);
			return static_cast<vk::Result>(instanceResult.vk_result());
		}

		vkbInstance = instanceResult.value();

		volkLoadInstance(vkbInstance.instance);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(static_cast<vk::Instance>(vkbInstance.instance));

		if (CreateWindowSurface(vkbInstance.instance, window, vkSurface) != vk::Result::eSuccess)
		{
			MessageLogging::Message("Failed create VkSurface.", MessageType::Error);
			return vk::Result::eErrorInitializationFailed;
		}

		VkPhysicalDeviceVulkan13Features features13 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
			.synchronization2 = true,
			.dynamicRendering = true
		};

		const auto physicalDevice = vkb::PhysicalDeviceSelector{ vkbInstance }
			.set_required_features_13(features13)
			.set_minimum_version(1, 3)
			.set_surface(vkSurface)
			.select();
		if (!physicalDevice)
		{
			MessageLogging::Message("Failed select current GPU.", MessageType::Error);
			return static_cast<vk::Result>(physicalDevice.vk_result());
		}

		const auto deviceResult = vkb::DeviceBuilder{ physicalDevice.value() }.build();
		if (!deviceResult)
		{
			MessageLogging::Message("Failed create VkDevice.", MessageType::Error);
			return static_cast<vk::Result>(deviceResult.vk_result());
		}

		vkbDevice = deviceResult.value();

		volkLoadDevice(vkbDevice.device);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(static_cast<vk::Device>(vkbDevice.device));

		const auto swapchainResult = vkb::SwapchainBuilder{ vkbDevice }
			.set_desired_min_image_count(2)
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			.build();
		if (!swapchainResult)
		{
			MessageLogging::Message("Failed create VkSwapchainKHR.", MessageType::Error);
			return static_cast<vk::Result>(swapchainResult.vk_result());
		}

		vkbSwapchain = swapchainResult.value();

		MessageLogging::Message(MessageLogging::ConstructMessage(
			{ "Swapchain image count = ", std::to_string(vkbSwapchain.image_count) })
			, MessageType::Default);

		const auto imagesResult = vkbSwapchain.get_images();
		if (!imagesResult)
		{
			MessageLogging::Message("Failed get raw images from VkSwapchainKHR.", MessageType::Error);
			return static_cast<vk::Result>(imagesResult.vk_result());
		}

		vkImages = imagesResult.value();

		const auto imageViewsResult = vkbSwapchain.get_image_views();
		if (!imageViewsResult)
		{
			MessageLogging::Message("Failed get image views from VkSwapchainKHR.", MessageType::Error);
			return static_cast<vk::Result>(imageViewsResult.vk_result());
		}

		vkImageViews = imageViewsResult.value();

		const auto graphicsQueueResult = vkbDevice.get_queue(vkb::QueueType::graphics);
		if (!graphicsQueueResult)
		{
			MessageLogging::Message("Failed get graphics queue from VkDevice (GPU).", MessageType::Error);
			return static_cast<vk::Result>(graphicsQueueResult.vk_result());
		}

		queues.graphics = graphicsQueueResult.value();

		const auto graphicsFamilyResult = vkbDevice.get_queue_index(vkb::QueueType::graphics);
		if (!graphicsFamilyResult)
		{
			MessageLogging::Message("Failed get graphics queue family from VkDevice (GPU).", MessageType::Error);
			return static_cast<vk::Result>(graphicsFamilyResult.vk_result());
		}

		queues.graphicsFamily = graphicsFamilyResult.value();

		const auto presentQueueResult = vkbDevice.get_queue(vkb::QueueType::present);
		if (!presentQueueResult)
		{
			MessageLogging::Message("Failed get present queue from VkDevice (GPU).", MessageType::Error);
			return static_cast<vk::Result>(presentQueueResult.vk_result());
		}

		queues.present = presentQueueResult.value();

		const auto presentFamilyResult = vkbDevice.get_queue_index(vkb::QueueType::present);
		if (!presentFamilyResult)
		{
			MessageLogging::Message("Failed get present queue family from VkDevice (GPU).", MessageType::Error);
			return static_cast<vk::Result>(presentFamilyResult.vk_result());
		}

		queues.presentFamily = presentFamilyResult.value();

		return vk::Result::eSuccess;
	}

}