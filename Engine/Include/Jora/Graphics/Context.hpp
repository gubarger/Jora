#pragma once

#include <Jora/Graphics/DefaultVulkan.hpp>

#include <optional>

namespace jora
{

	class Context
	{
	public:
		vkb::Instance vkbInstance;
		vkb::Device vkbDevice;
		vkb::Swapchain vkbSwapchain;

		/**
		 * @brief Структура для очередей.
		 */

		struct Queues
		{
			std::optional<vkb::QueueType> graphics = std::nullopt;
			std::optional<vkb::QueueType> present = std::nullopt;
		} queues;

		vk::detail::DispatchLoaderDynamic dynamicDispatcher = nullptr;

		/**
		 * @brief Инициализация основных хендлов.
		 * @return vk::Result код.
		 */

		vk::Result Init() noexcept;
	};

}