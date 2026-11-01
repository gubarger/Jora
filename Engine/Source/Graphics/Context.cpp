#include <Jora/Graphics/Context.hpp>

#include <Jora/Graphics/Window.hpp>

#include <Jora/Core/Logger.hpp>

#include <GLFW/glfw3.h>

namespace
{

	/**
	 * @brief Глобальный вывод Vulkan Validation Layers
	 * @param messageSeverity Уровень сообщения.
	 * @param messageType Тип сообщения.
	 * @param pCallbackData Возвращаемые данные для вывода.
	 * @param pUserData Сохраняемые параметры для пользователя ***(не используется)***.
	 * @return VK_FALSE (0U)
	 */

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = jora::ConstructMessage({"Vulkan (Validation Layer) \n\t-- Error: \t", pCallbackData->pMessage}),
					.type = jora::MessageType::Debug });
		}
		else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = jora::ConstructMessage({"Vulkan (Validation Layer) \n\t-- Warning: \t", pCallbackData->pMessage}),
					.type = jora::MessageType::Debug });
		}
		else {
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = jora::ConstructMessage({"Vulkan (Validation Layer) \n\t-- Info: \t", pCallbackData->pMessage}),
					.type = jora::MessageType::Debug });
		}

		return VK_FALSE;
	}

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
		if (!vkImageViews.empty())
		{
			vkbSwapchain.destroy_image_views(vkImageViews);
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "VkImageViews destroyed success!",
					.type = jora::MessageType::Debug });
		}

		if (vkbSwapchain.swapchain != nullptr)
		{
			vkb::destroy_swapchain(vkbSwapchain);
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "VkSwapchainKHR destroyed success!",
					.type = jora::MessageType::Debug });
		}

		if (vkbDevice.device != nullptr)
		{
			vkb::destroy_device(vkbDevice);
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "VkDevice destroyed success!",
					.type = jora::MessageType::Debug });
		}

		if (vkSurface != nullptr)
		{
			vkb::destroy_surface(vkbInstance, vkSurface);
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "VkSurfaceKHR destroyed success!",
					.type = jora::MessageType::Debug });
		}

		if (vkbInstance.instance != nullptr)
		{
			vkb::destroy_instance(vkbInstance);
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "VkInstance destroyed success!",
					.type = jora::MessageType::Debug });
		}
	}

	vk::Result Context::Init(GLFWwindow* window) noexcept
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Error init Volk!",
					.type = jora::MessageType::Highest });
			return vk::Result::eErrorInitializationFailed;
		}

		const auto [extCount, extStr] = GetWindowExtensions();

		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		const auto instanceResult = vkb::InstanceBuilder{}
			.set_engine_name("Jora Engine")
			.enable_validation_layers()
			.require_api_version(1, 3, 0)
			.use_default_debug_messenger()
			.set_debug_callback(VulkanDebugCallback)
			.enable_extensions(extCount, extStr)
			.build();
		if (!instanceResult)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Error create VkInstance!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(instanceResult.vk_result());
		}

		vkbInstance = instanceResult.value();

		volkLoadInstance(vkbInstance.instance);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(static_cast<vk::Instance>(vkbInstance.instance));

		if (CreateWindowSurface(vkbInstance.instance, window, vkSurface) != vk::Result::eSuccess)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Error create VkSurfaceKHR!",
					.type = jora::MessageType::Highest });
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
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Failed to get or select current GPU!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(physicalDevice.vk_result());
		}

		const auto deviceResult = vkb::DeviceBuilder{ physicalDevice.value() }.build();
		if (!deviceResult)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Error create VkDevice!",
					.type = jora::MessageType::Highest });
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
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Error create VkSwapchainKHR!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(swapchainResult.vk_result());
		}

		vkbSwapchain = swapchainResult.value();



		const auto imagesResult = vkbSwapchain.get_images();
		if (!imagesResult)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Failed to get VkImage!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(imagesResult.vk_result());
		}

		vkImages = imagesResult.value();

		const auto imageViewsResult = vkbSwapchain.get_image_views();
		if (!imageViewsResult)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Failed to get VkImageViews!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(imageViewsResult.vk_result());
		}

		vkImageViews = imageViewsResult.value();

		const auto graphicsQueueResult = vkbDevice.get_queue(vkb::QueueType::graphics);
		if (!graphicsQueueResult)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Failed to get queue for graphics queues!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(graphicsQueueResult.vk_result());
		}

		queues.graphics = graphicsQueueResult.value();

		const auto graphicsFamilyResult = vkbDevice.get_queue_index(vkb::QueueType::graphics);
		if (!graphicsFamilyResult)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Failed to get queue family for graphics queues!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(graphicsFamilyResult.vk_result());
		}

		queues.graphicsFamily = graphicsFamilyResult.value();

		const auto presentQueueResult = vkbDevice.get_queue(vkb::QueueType::present);
		if (!presentQueueResult)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Failed to get queue for present queues!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(presentQueueResult.vk_result());
		}

		queues.present = presentQueueResult.value();

		const auto presentFamilyResult = vkbDevice.get_queue_index(vkb::QueueType::present);
		if (!presentFamilyResult)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Failed to get queue family for present queues!",
					.type = jora::MessageType::Highest });
			return static_cast<vk::Result>(presentFamilyResult.vk_result());
		}

		queues.presentFamily = presentFamilyResult.value();

		return vk::Result::eSuccess;
	}

}