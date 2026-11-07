#pragma once

#include <Jora/Graphics/DefaultVulkan.hpp>
#include <Jora/Core/SlotMap.hpp>

#include <mutex>

namespace jora
{

	using BufferId = Handle<struct BufferTag>;

	struct AllocatedBuffer
	{
		vk::Buffer buffer = nullptr; // Буффер.
		VmaAllocation allocation = nullptr; // Выделенный блок в памяти.
		VmaAllocationInfo info{}; // Информация о выделенном блоке.
	};

	class BufferManager
	{
	public:
		explicit BufferManager(VmaAllocator allocator) noexcept : allocator_(allocator) {}

		~BufferManager() { Release(); }

		BufferManager(const BufferManager&) = delete;
		BufferManager& operator=(const BufferManager&) = delete;

		BufferManager(BufferManager&&) noexcept = delete;
		BufferManager& operator=(BufferManager&&) noexcept = delete;

		std::expected<BufferId, vk::Result> Create(vk::DeviceSize size, vk::BufferUsageFlags usage,
			VmaAllocationCreateFlags flags = 0)
		{
			VkBufferCreateInfo bufferInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = size,
				.usage = static_cast<VkBufferUsageFlags>(usage),
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE
			};

			VmaAllocationCreateInfo allocInfo{ .flags = flags, .usage = VMA_MEMORY_USAGE_AUTO };

			VkBuffer rawBuffer = nullptr;
			AllocatedBuffer result;

			if (const VkResult createBufferResult = vmaCreateBuffer(allocator_,
				&bufferInfo,
				&allocInfo,
				&rawBuffer,
				&result.allocation,
				&result.info);
				createBufferResult != VK_SUCCESS)
			{
				return std::unexpected(static_cast<vk::Result>(createBufferResult));
			}

			result.buffer = vk::Buffer{ rawBuffer };

			std::lock_guard lock(mutex_);
			return buffers_.Emplace(std::move(result));
		}

		bool Destroy(BufferId id) noexcept
		{
			std::optional<AllocatedBuffer> removed;
			{
				std::lock_guard lock(mutex_);
				removed = buffers_.Erase(id);
			}

			if (!removed)
			{
				return false; // уже удалён или хендл чужой
			}

			vmaDestroyBuffer(allocator_, static_cast<VkBuffer>(removed->buffer), removed->allocation);

			return true;
		}

		std::optional<vk::Buffer> GetHandle(BufferId id)
		{
			std::lock_guard lock(mutex_);

			auto result = buffers_.Get(id);

			if (!result)
			{
				return std::nullopt;
			}

			return (*result)->buffer;
		}

	private:
		void Release() noexcept
		{
			buffers_.Drain([this](AllocatedBuffer&& b)
				{
					vmaDestroyBuffer(allocator_, static_cast<VkBuffer>(b.buffer), b.allocation);
				});
		}

		VmaAllocator allocator_; // VMA аллокатор.

		std::mutex mutex_; // Защищает buffers_.
		SlotMap<AllocatedBuffer, BufferTag> buffers_; // Хранимые буфферы по индексу.
	};

}