#pragma once

#include <Jora/Graphics/DefaultVulkan.hpp>
#include <Jora/Core/SlotMap.hpp>

#include <mutex>

namespace jora
{

	/**
	 * @brief Хэндл на пустую структуру `ImageTag` *(id изображения)*
	 */

	using ImageId = Handle<struct ImageTag>;

	/**
	 * @brief Структура информации о создаваемом изображении *(VkImage)*.
	 */

	struct AllocatedImage
	{
		vk::Image image = nullptr; // Изображение.
		vk::ImageView view = nullptr; // Отображение изображения.
		VmaAllocation allocation = nullptr; // Выделенный блок в памяти.
		VmaAllocationInfo info{}; // Информация о выделенном блоке.
		vk::Extent3D extent{};
		vk::Format format = vk::Format::eUndefined;
		uint32_t mipLevels = 1;
	};

	/**
	 * @brief Структура для хранения базовых полей при создании `VkImage`.
	 */

	struct ImageDesc
	{
		vk::Extent3D extent{ 1, 1, 1 };
		vk::Format format = vk::Format::eR8G8B8A8Srgb;
		vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
		vk::ImageType type = vk::ImageType::e2D;
		uint32_t mipLevels = 1;
		uint32_t arrayLayers = 1;
		vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1;
		vk::ImageCreateFlags flags{};
		vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor;
		VmaAllocationCreateFlags allocFlags = 0;
	};

	/**
	 * @brief Считает число мип-уровней *(mipLevels)*.
	 * @param width Ширина.
	 * @param height Длинна.
	 * @return Минимальное количество бит, которые необходимы.
	 */

	constexpr uint32_t MipLevels(const uint32_t width, const uint32_t height) noexcept
	{
		return std::bit_width(std::max(width, height));
	}

	/**
	 * @brief Базовый менеджер изображений *(VkImage, VkImageView)*.
	 */

	class ImageManager
	{
	public:
		ImageManager(vk::Device device, VmaAllocator allocator) noexcept
			: device_(device), allocator_(allocator) {}

		~ImageManager() { Release(); }

		ImageManager(const ImageManager&) = delete;
		ImageManager& operator=(const ImageManager&) = delete;

		ImageManager(ImageManager&&) noexcept = delete;
		ImageManager& operator=(ImageManager&&) noexcept = delete;

		std::expected<ImageId, vk::Result> Create(const ImageDesc& desc)
		{
			VkImageCreateInfo info{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.flags = static_cast<VkImageCreateFlags>(desc.flags),
				.imageType = static_cast<VkImageType>(desc.type),
				.format = static_cast<VkFormat>(desc.format),
				.extent = { desc.extent.width, desc.extent.height, desc.extent.depth },
				.mipLevels = desc.mipLevels,
				.arrayLayers = desc.arrayLayers,
				.samples = static_cast<VkSampleCountFlagBits>(desc.samples),
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = static_cast<VkImageUsageFlags>(desc.usage),
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE, // Пока что нет нескольких семейств очередей.
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED };

			VmaAllocationCreateInfo alloc{ .flags = desc.allocFlags, .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE };

			AllocatedImage result;
			VkImage raw = nullptr;

			if (const VkResult createImageResult = vmaCreateImage(allocator_, &info, &alloc, &raw, &result.allocation, &result.info);
				createImageResult != VK_SUCCESS)
			{
				return std::unexpected(static_cast<vk::Result>(createImageResult));
			}

			result.image = vk::Image{ raw };

			vk::ImageViewCreateInfo viewInfo({}, result.image, ViewTypeFor(desc), desc.format, {},
				vk::ImageSubresourceRange(desc.aspect, 0, desc.mipLevels, 0, desc.arrayLayers));

			auto view = device_.createImageView(viewInfo);

			if (view.result != vk::Result::eSuccess)
			{
				vmaDestroyImage(allocator_, raw, result.allocation); // Откат

				return std::unexpected(view.result);
			}

			result.view = view.value;
			result.extent = desc.extent;
			result.format = desc.format;
			result.mipLevels = desc.mipLevels;

			std::lock_guard lock(mutex_);
			return images_.Emplace(std::move(result));
		}

		bool Destroy(ImageId id) noexcept
		{
			std::optional<AllocatedImage> removed;
			{
				std::lock_guard lock(mutex_);
				removed = images_.Erase(id);
			}

			if (!removed)
			{
				return false;
			}

			DestroyRaw(*removed);

			return true;
		}

		std::optional<std::pair<vk::Image, vk::ImageView>> GetHandle(ImageId id)
		{
			std::lock_guard lock(mutex_);

			auto result = images_.Get(id);

			if (!result)
			{
				return std::nullopt;
			}

			return { { (*result)->image, (*result)->view } };
		}

	private:
		static vk::ImageViewType ViewTypeFor(const ImageDesc& desc) noexcept
		{
			if (desc.flags & vk::ImageCreateFlagBits::eCubeCompatible)
			{
				return vk::ImageViewType::eCube;
			}

			switch (desc.type)
			{
			case vk::ImageType::e1D: return desc.arrayLayers > 1 ? vk::ImageViewType::e1DArray : vk::ImageViewType::e1D;
			case vk::ImageType::e3D: return vk::ImageViewType::e3D;

			default: return desc.arrayLayers > 1 ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D;
			}
		}

		void DestroyRaw(AllocatedImage& i) noexcept
		{
			device_.destroyImageView(i.view);
			vmaDestroyImage(allocator_, static_cast<VkImage>(i.image), i.allocation);
		}

		void Release() noexcept { images_.Drain([this](AllocatedImage&& i) { DestroyRaw(i); }); }

	private:
		vk::Device device_;
		VmaAllocator allocator_;

		std::mutex mutex_; // Защищает images_.
		SlotMap<AllocatedImage, ImageTag> images_;
	};

}