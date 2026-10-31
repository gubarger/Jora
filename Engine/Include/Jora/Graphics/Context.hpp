#pragma once

#include <Jora/Graphics/DefaultVulkan.hpp>

#include <optional>

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
			std::optional<vkb::QueueType> graphics = std::nullopt;
			std::optional<vkb::QueueType> present = std::nullopt;
		} queues;

		/**
		 * @brief Инициализация основных хендлов.
		 * @return vk::Result код.
		 */

		vk::Result Init(GLFWwindow* window) noexcept;
	};

}