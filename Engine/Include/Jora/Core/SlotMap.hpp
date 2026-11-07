#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <expected>
#include <compare>

namespace jora
{

	/**
	 * @brief Типизированный хендл.
	 */

	template<typename Tag>
	struct Handle
	{
		std::uint32_t index = 0;
		std::uint32_t generation = 0; // 0 = невалидный хендл.

		auto operator<=>(const Handle&) const = default;

		/**
		 * @brief Функиця возвращает валиден ли хэндл.
		 * @return `bool`.
		 */

		[[nodiscard]] constexpr bool IsValid() const noexcept { return generation != 0; }
	};

	/**
	 * @brief Перечисление ошибочных кодов в слотах.
	 */

	enum class SlotError { OutOfBounds, StaleHandle };

	/**
	 * @brief Структура хранимых слотов.
	 * @tparam T Тип данных.
	 * @tparam Tag Тип индексатора.
	 */

	template<typename T, typename Tag>
	class SlotMap
	{
	public:
		using Id = Handle<Tag>;

		/**
		 * @brief Создаёт объект на месте и возвращает его хендл.
		 */

		template<typename... Args>
		Id Emplace(Args&&... args)
		{
			std::uint32_t index;

			if (!free_.empty())
			{
				index = free_.back(); // Переиспользует освободившуюся ячейку.
				free_.pop_back();
			}
			else
			{
				index = static_cast<std::uint32_t>(slots_.size());
				slots_.emplace_back(); // Новая пустая ячейка в конце (generation = 1).
			}

			Slot& slot = slots_[index];

			slot.data.emplace(std::forward<Args>(args)...);

			return Id{ index, slot.generation };
		}

		/**
		 * @brief Удаляет объект. Возвращает его владельцу, чтобы тот освободил ресурсы.
		 * @return `std::nullopt`, если хендл устарел или неверен.
		 */

		std::optional<T> Erase(Id id)
		{
			if (!IsAlive(id)) return std::nullopt;

			Slot& slot = slots_[id.index];
			std::optional<T> result = std::move(slot.data);

			slot.data.reset(); // Данные существуют, поэтому явный сброс.

			Bump(slot);

			free_.push_back(id.index);

			return result;
		}

		[[nodiscard]] bool IsAlive(Id id) const noexcept
		{
			return id.index < slots_.size() && slots_[id.index].data.has_value()
				&& slots_[id.index].generation == id.generation;
		}

		[[nodiscard]] std::expected<T*, SlotError> Get(Id id) noexcept
		{
			if (id.index >= slots_.size())
			{
				return std::unexpected(SlotError::OutOfBounds);
			}

			Slot& slot = slots_[id.index];

			if (!slot.data || slot.generation != id.generation)
			{
				return std::unexpected(SlotError::StaleHandle);
			}

			return &*slot.data;
		}

		/**
		 * @brief Отдаёт все живые объекты функции `f(T&&)` и освобождает ячейки.
		 * @note Нужно для завершения работы.
		 */

		template<typename Function>
		void Drain(Function&& function)
		{
			for (std::uint32_t i = 0; i < slots_.size(); ++i)
			{
				Slot& slot = slots_[i];

				if (!slot.data)
				{
					continue;
				}

				function(std::move(*slot.data));

				slot.data.reset();

				Bump(slot);

				free_.push_back(i);
			}
		}

	private:
		struct Slot
		{
			std::optional<T> data; // Хранящиеся данные.
			std::uint32_t generation = 1; // 0 зарезервирован (невалидный).
		};

		static void Bump(Slot& slot) noexcept
		{
			if (++slot.generation == 0) slot.generation = 1; // Защита от переполнения.
		}

		std::vector<Slot> slots_; // Массив слотов.
		std::vector<std::uint32_t> free_; // Free-list из индексов для id.
	};

}