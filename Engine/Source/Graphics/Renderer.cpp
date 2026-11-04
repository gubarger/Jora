#include <Jora/Graphics/Renderer.hpp>
#include <Jora/Graphics/Context.hpp>
#include <Jora/Core/Logger.hpp>

namespace jora
{

	Renderer::~Renderer()
	{
		context_.vkDevice.waitIdle();
		commandPools_.Destroy();
	}

	vk::Result Renderer::Init() noexcept
	{
		return commandPools_.Init(
			context_.vkDevice,
			context_.graphicsFamily,
			Context::maxFrames,
			1); // слотов на кадр: пока что лишь одна задача (один поток).
	}

	void Renderer::Record()
	{
		std::ignore = context_.vkDevice.waitForFences(1, &context_.inFlights[context_.currentFrame],
			vk::True,
			UINT64_MAX);

		uint32_t imageIndex = 0;

		auto acquireNextImage = context_.vkDevice.acquireNextImageKHR(context_.vkSwapchain,
			UINT64_MAX,
			context_.imageAvailables[context_.currentFrame],
			nullptr,
			&imageIndex);

		if (acquireNextImage == vk::Result::eErrorOutOfDateKHR)
		{
			return;
		}

		// eSuboptimalKHR рисует.

		std::ignore = context_.vkDevice.resetFences(1, &context_.inFlights[context_.currentFrame]);

		commandPools_.Begin(context_.currentFrame);

		vk::CommandBuffer commandBuffer = commandPools_.Acquire(context_.currentFrame, 0,
			vk::CommandBufferLevel::ePrimary);

		/*
		* Тестовый временный код для проверки работоспособности.
		*/

		std::ignore = commandBuffer.begin(
			vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		// Начало кадра.

		vk::Image image = context_.vkImages[imageIndex];
		vk::ImageSubresourceRange imageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

		// eUndefined в eTransferDstOptimal (временная очистка).
		vk::ImageMemoryBarrier toClear({}, vk::AccessFlagBits::eTransferWrite,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, imageSubresourceRange);

		commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
			{}, {}, {}, toClear);

		vk::ClearColorValue color(std::array<float, 4>{0.1f, 0.2f, 0.4f, 1.0f});

		commandBuffer.clearColorImage(image, vk::ImageLayout::eTransferDstOptimal, color, imageSubresourceRange);

		// eTransferDstOptimal в ePresentSrcKHR (временный показ).
		vk::ImageMemoryBarrier toPresent(vk::AccessFlagBits::eTransferWrite, {},
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, imageSubresourceRange);

		commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
			{}, {}, {}, toPresent);

		// Конец кадра.
		std::ignore = commandBuffer.end();

		vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eTransfer;

		vk::SubmitInfo submit(
			1, &context_.imageAvailables[context_.currentFrame], &waitStage,
			1, &commandBuffer,
			1, &context_.renderFinisheds[imageIndex]);

		vk::Queue graphics{ context_.queues.graphics };

		std::ignore = graphics.submit(1, &submit, context_.inFlights[context_.currentFrame]);

		vk::PresentInfoKHR present(1, &context_.renderFinisheds[imageIndex], 1, &context_.vkSwapchain, &imageIndex);

		vk::Queue presentQueue{ context_.queues.present };

		auto presentFrame = presentQueue.presentKHR(present);

		// Заглушка пересоздания VkSwapchainKHR: eErrorOutOfDateKHR или eSuboptimalKHR.

		context_.currentFrame = (context_.currentFrame + 1) % Context::maxFrames; // Следующий кадр.
	}

}