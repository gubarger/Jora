#pragma once

#include <string>
#include <queue>
#include <optional>

namespace jora
{

	/**
	 * @brief Перечисление типов ошибок для вывода информации.
	 * @details Перечисление идет сверху вниз ради использования priority_queue.
	 */

	enum class MessageType : int32_t
	{
		Highest = 3, // Суда идут ошибки и серьезные баги (активно в движке).
		Default = 2, // Info (другое название). Можно выставлять в Debug, если нужно именно там.
		/* Информация об отдельных
		 * Success: сюда идет информация о успешном завершении
		 * (Если это отладочная информация, то лучше выставить значение для Debug)
		 *
		 * Vulkan: Сюда идет информация от Vulkan API (незначительная, не отладочная)
		 */
		Average = 1, // Сюда идут предупреждения и прочие мелкие проблемы (активно в движке). 
		// Можно выставлять в Debug, если нужно именно там.
		Debug = 0 // Сюда идет отладочная информация и Vulkan Validation Layers (не активно в движке).
	};

	/**
	 * @brief Объект блока информации о сообщении в логе.
	 */

	struct MessageBox
	{
		std::optional<std::string_view> message;
		std::optional<MessageType> type;

		auto operator<=>(const MessageBox& other) const = default;
	};

	/**
	 * @brief Преобразует MessageType в временную строку.
	 * @param type Тип преобразуемого вывода ошибки.
	 * @return `lvalue` временную строку.
	 */

	std::pair<std::ostream&, std::string_view> MessageTypeToString(const MessageType type);

	/**
	 * @brief Конструирует сообщение из initializer_list string_view.
	 * @warning Может приводить к потере производительности.
	 * @param messages Сообщения, которые необходимо соеденить.
	 * @return Соедененную string строку в памяти.
	 */

	std::string ConstructMessage(const std::initializer_list<std::string_view> messages);

	/**
	 * @brief Класс для разделения потока вывода на стадии:
	 * 1. Debug (если есть) - выводимая информация при отладке;
	 * 2. Important - выводимая информация движка (Error, Warning);
	 * 3. External - остальная информации (Default, Success);
	 *
	 * @details Класс хранит приоритетную очередь для асинхронного и отложенного вывода логов (см. MessageType и MessageBox).
	 * MessageSplittingManager является блоком управления сообщений.
	 * Есть глобальный логгер движка - им управляет MessageSplittingManager.
	 * priority_queue<MessageBox> в свою очередь предоставляет лишь окошко: оно не управляет данными, а групперует их.
	 * Сообщения в преоритетной очереди очищаются каждый раз, когда блок вывода или задачи заканчивает жизнь (задача завершается).
	 * При этом последние данные, которые имеют высший приоритет, остаются в логах до полного удаления
	 * (завершения конкретного MessageSplittingManager).
	 */

	class MessageSplittingManager
	{
	public:
		/**
		 * @brief Добовляет в приоритетную очередь блок информации.
		 * @param messageInfo Блок выводимой информации.
		 */

		inline void AddMessageInQueue(const MessageBox messageInfo)
		{
			queue_.push(messageInfo);
		}

		/**
		 * @brief Отдает последнюю информацию в приоритетной очереди (head).
		 * @note Если очередь пуста, то возвращается структура с *"пустыми"* элементами *(std::nullopt)*.
		 * @warning Удаляет все, кроме последнего элемента.
		 * @return Блок информации.
		 */

		inline MessageBox LastInfo()
		{
			if (!queue_.empty())
			{
				return { std::nullopt, std::nullopt };
			}

			while (queue_.size() > 1)
			{
				queue_.pop();
			}

			return queue_.top();
		}

		/**
		 * @brief Уничтожает приоритетную очередь для данного логгера.
		 */

		inline void Destroy()
		{
			if (!queue_.empty()) return;

			while (queue_.size() != 0)
			{
				queue_.pop();
			}
		}

		/**
		 * @brief Созраняет информацию лога в приоритетную очередь.
		 * @param messageInfo Информация о логе.
		 */

		void Message(const MessageBox messageInfo);

	private:
		std::priority_queue<MessageBox> queue_; //< Приоритетная очередь, хранящяя список сообщений.
	};

	inline MessageSplittingManager globalLogger;
}