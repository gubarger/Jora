#pragma once

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

		vk::Device vkDevice = nullptr;
		vk::SwapchainKHR vkSwapchain = nullptr;

		Context() = default;

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;

		Context(Context&&) noexcept = default;
		Context& operator=(Context&&) noexcept = default;

		~Context();

		/**
		 * @brief Структура для очередей.
		 */

		struct Queues
		{
			VkQueue graphics = nullptr;
			VkQueue present = nullptr;
		} queues;

		uint32_t graphicsFamily = 0;
		uint32_t presentFamily = 0;

		std::vector<VkImage> vkImages;
		std::vector<VkImageView> vkImageViews;

		static constexpr uint32_t maxFrames = 2;
		uint32_t currentFrame = 0;

		std::vector<vk::Fence> inFlights; // maxFrames
		std::vector<vk::Semaphore> imageAvailables; // maxFrames
		std::vector<vk::Semaphore> renderFinisheds; // vkImages.size();

		vk::Result Init(GLFWwindow* window) noexcept;
	};

}