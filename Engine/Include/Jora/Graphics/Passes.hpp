#pragma once

#include <Jora/Graphics/DefaultVulkan.hpp>

/**
 * @brief Файл для легких проходов `Render Graph`.
 * @note На данный момент это лишь заготовка для будущего: `Pass` *({прочитать[],записать[], лямбда})*
 * и цикла, который будет сравнивать прошлое и следующее использование каждого ресурса.
 * @details Здесь временно находятся *функции-одиночки* для реализаций легких переходов.
 */

namespace jora
{

	void TranslationImage(vk::CommandBuffer commandBuffer, vk::Image image, vk::ImageAspectFlags aspect,
		vk::ImageLayout fromLayout	/* Из старого */,
		vk::ImageLayout toLayout	/* В новый */,
		vk::PipelineStageFlags2 sourceStage			/* Запись данных, до барьера */,
		vk::AccessFlags2 sourceAccess				/* Запись данных, до барьера */,
		vk::PipelineStageFlags2 destinationStage	/* Чтение данных, после барьера */,
		vk::AccessFlags2 destinationAccess			/* Чтение данных, после барьера */) noexcept
	{
		vk::ImageMemoryBarrier2 barrier{};

		// Source (src).
		barrier.srcStageMask = sourceStage;
		barrier.srcAccessMask = sourceAccess;

		// Destination (dst).
		barrier.dstStageMask = destinationStage;
		barrier.dstAccessMask = destinationAccess;

		barrier.oldLayout = fromLayout;
		barrier.newLayout = toLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;

		barrier.subresourceRange = vk::ImageSubresourceRange(aspect,
			0u, vk::RemainingMipLevels,
			0u, vk::RemainingArrayLayers);

		vk::DependencyInfo dependency{};

		dependency.imageMemoryBarrierCount = 1;
		dependency.pImageMemoryBarriers = &barrier;

		commandBuffer.pipelineBarrier2(dependency);
	}

}