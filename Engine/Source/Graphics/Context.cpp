#include <Jora/Graphics/Context.hpp>

#include <Jora/Graphics/Window.hpp>

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
		if (vkbSwapchain.swapchain != nullptr)
		{
			vkb::destroy_swapchain(vkbSwapchain);
		}

		if (vkbDevice.device != nullptr)
		{
			vkb::destroy_device(vkbDevice);
		}

		if (vkSurface != nullptr)
		{
			vkb::destroy_surface(vkbInstance, vkSurface);
		}

		if (vkbInstance.instance != nullptr)
		{
			vkb::destroy_instance(vkbInstance);
		}
	}

	vk::Result Context::Init(GLFWwindow* window) noexcept
	{
		if (volkInitialize() != VK_SUCCESS)
		{
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
			return static_cast<vk::Result>(instanceResult.vk_result());
		}

		vkbInstance = instanceResult.value();

		volkLoadInstance(vkbInstance.instance);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(static_cast<vk::Instance>(vkbInstance.instance));

		if (const auto surfaceResult = CreateWindowSurface(vkbInstance.instance, window, vkSurface);
			surfaceResult != vk::Result::eSuccess)
		{
			return surfaceResult;
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
			return static_cast<vk::Result>(physicalDevice.vk_result());
		}

		const auto deviceResult = vkb::DeviceBuilder{ physicalDevice.value() }.build();
		if (!deviceResult)
		{
			return static_cast<vk::Result>(deviceResult.vk_result());
		}

		vkbDevice = deviceResult.value();

		volkLoadDevice(vkbDevice.device);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(static_cast<vk::Device>(vkbDevice.device));

		return vk::Result::eSuccess;
	}

}