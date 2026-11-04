#include <Jora/Graphics/CommandPools.hpp>

#include <cassert>

namespace jora
{

	vk::Result CommandPools::Init(vk::Device device,
		uint32_t queueFamilyIndex,
		uint32_t framesInFlight,
		uint32_t slots)
	{
		assert(framesInFlight > 0 && slots > 0);

		device_ = device;
		slotsPerFrame_ = slots;

		slots_.resize(size_t(framesInFlight) * slots);

		vk::CommandPoolCreateInfo info({}, queueFamilyIndex);

		for (auto& slot : slots_)
		{
			auto result = device_.createCommandPool(info);

			if (result.result != vk::Result::eSuccess)
			{
				return result.result;
			}

			slot.pool = result.value;
		}

		return vk::Result::eSuccess;
	}


	void CommandPools::Destroy() noexcept
	{
		for (auto& slot : slots_)
		{
			if (slot.pool)
			{
				device_.destroyCommandPool(slot.pool);
			}
		}

		slots_.clear();
	}

	vk::Result CommandPools::Begin(uint32_t frame)
	{
		for (uint32_t i = 0; i < slotsPerFrame_; ++i)
		{
			auto& slot = At(frame, i);
			if (auto result = device_.resetCommandPool(slot.pool); result != vk::Result::eSuccess)
			{
				return result;
			}

			slot.used[0] = slot.used[1] = 0; // Можно записывать заново.
		}

		return vk::Result::eSuccess;
	}

	vk::CommandBuffer CommandPools::Acquire(uint32_t frame, uint32_t slot,
		vk::CommandBufferLevel level)
	{
		auto& currentSlot = At(frame, slot);
		const int key = (level == vk::CommandBufferLevel::ePrimary) ? 0 : 1;

		if (currentSlot.used[key] == currentSlot.buffers[key].size()) // Свободных нет - выделяется новый.
		{
			vk::CommandBufferAllocateInfo allocateInfo(currentSlot.pool, level, 1);

			auto resultAllocate = device_.allocateCommandBuffers(allocateInfo);

			if (resultAllocate.result != vk::Result::eSuccess)
			{
				return nullptr;
			}

			currentSlot.buffers[key].push_back(resultAllocate.value[0]);
		}

		return currentSlot.buffers[key][currentSlot.used[key]++];
	}

}