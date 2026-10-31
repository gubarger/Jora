#pragma once

#include <Jora/Graphics/DefaultVulkan.hpp>

struct GLFWwindow;

namespace jora
{

	class Context
	{
	public:
		vkb::Instance vkbInstance;
		vkb::Device vkbDevice;
		vkb::Swapchain vkbSwapchain;

		VkSurfaceKHR vkSurface = nullptr;

		~Context();

		/**
		 * @brief Структура для очередей.
		 */

		struct Queues
		{
			VkQueue graphics = nullptr;
			VkQueue present = nullptr;

			uint32_t graphicsFamily = 0;
			uint32_t presentFamily = 0;
		} queues;

		std::vector<VkImage> vkImages;
		std::vector<VkImageView> vkImageViews;

		/**
		 * @brief Инициализация основных хендлов.
		 * @return vk::Result код.
		 */

		vk::Result Init(GLFWwindow* window) noexcept;
	};

}