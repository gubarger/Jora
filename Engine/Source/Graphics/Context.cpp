#include <Jora/Graphics/Context.hpp>

#include <Jora/Graphics/Window.hpp>

namespace jora
{

	vk::Result Context::Init() noexcept
	{
		if (const auto result = volkInitialize() != static_cast<VkResult>(vk::Result::eSuccess))
		{
			return static_cast<vk::Result>(result);
		}

		const auto [extCount, extStr] = GetWindowExtensions();

		const auto instanceResult = vkb::InstanceBuilder{}
			.set_engine_name("Jora Engine")
			.enable_validation_layers()
			.require_api_version(1, 3, 0)
			.use_default_debug_messenger()
			.enable_extensions(static_cast<size_t>(extCount), extStr)
			.build();
		if (!instanceResult)
		{
			return static_cast<vk::Result>(instanceResult.vk_result());
		}

		vkbInstance = instanceResult.value();

		volkLoadInstance(vkbInstance.instance);

		VkPhysicalDeviceVulkan13Features features13 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
			.synchronization2 = true,
			.dynamicRendering = true
		};

		const auto physicalDevice = vkb::PhysicalDeviceSelector{ vkbInstance }
			.set_required_features_13(features13)
			.set_minimum_version(1, 3)
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
	}

}