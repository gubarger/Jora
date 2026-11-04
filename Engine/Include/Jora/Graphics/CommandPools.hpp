#include <Jora/Graphics/DefaultVulkan.hpp>

namespace jora
{

	/**
	 * @brief Класс для коммандных пулов.
	 */

	class CommandPools
	{
	public:
		/**
		 * @brief Инициализирует коммандные пулы.
		 * @param device GPU *(VkDevice)*.
		 * @param queueFamilyIndex Семейство очереди *(uint32_t)*.
		 * @param framesInFlight Проходы кадра *(uint32_t)*
		 * @param slots Конкретные слоты *(uint32_t)*.
		 * @return Результат инициализации *(vk::Result)*.
		 */

		vk::Result Init(vk::Device device,
			uint32_t queueFamilyIndex,
			uint32_t framesInFlight,
			uint32_t slots);

		/**
		 * @brief Уничтожает коммандные пулы.
		 */

		void Destroy() noexcept;

		/**
		 * @brief Начинает записывать в коммандные буфферы.
		 * @param frame Конкретный кадр.
		 * @return Результат пересоздания *(vk::Result)*.
		 */

		vk::Result Begin(uint32_t frame);

		/**
		 * @brief Создает конкретный коммандный буффер.
		 * @param frame Конкретный кадр.
		 * @param slot Конкретный слот.
		 * @param level Уровень коммандного буффера *(дефолт: vk::CommandBufferLevel::ePrimary)*
		 * @return Перезаписанный или созданный коммандный буффер.
		 */

		vk::CommandBuffer Acquire(uint32_t frame, uint32_t slot,
			vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

	private:
		/**
		 * @brief Структура слота.
		 */

		struct Slot
		{
			vk::CommandPool pool;
			std::vector<vk::CommandBuffer> buffers[2]; // [0] primary, [1] secondary

			size_t used[2] = {};
		};

	private:
		/**
		 * @brief Отдает конкретный слот.
		 * @param frame Конкретный кадр.
		 * @param slot Конкретный слот.
		 * @return Ссылку на конкретный слот в кадре *(Slot)*.
		 */

		Slot& At(uint32_t frame, uint32_t slot) { return slots_[frame * slotsPerFrame_ + slot]; }

	private:
		vk::Device device_;
		uint32_t slotsPerFrame_ = 0; // Слотов на кадр.

		std::vector<Slot> slots_; // Слоты.
	};

}