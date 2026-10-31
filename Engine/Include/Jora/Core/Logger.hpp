#pragma once

#include <string>

namespace jora
{

	/**
	 * @brief Перечисление типов ошибок для вывода информации.
	 */

	enum class MessageType : int32_t
	{
		Default = 1000,
		Success = 0,
		Error = 1,
		Warning = -1
	};

	/**
	 * @brief Преобразует MessageType в временную строку.
	 * @param type Тип преобразуемого вывода ошибки.
	 * @return `lvalue` временную строку.
	 */

	std::pair<std::ostream&, std::string_view> MessageTypeToString(const MessageType type);

	/**
	 * @brief Класс для вывода логирования в консоль и файлы.
	 */

	class MessageLogging
	{
	public:
		/**
		 * @brief Удаление операторов копирования (сообщения для логов могут лишь перемещаться).
		 */

		MessageLogging(const MessageLogging&) = delete;
		MessageLogging operator=(const MessageLogging&) = delete;

		/**
		 * @brief Выводит сообщение с типом лога.
		 * @param message Выводимое сообщение.
		 * @param type Тип выводимого сообщения.
		 */

		static void Message(const std::string_view message, const MessageType type);
		static 	std::string ConstructMessage(const std::initializer_list<std::string_view> messages);
	};

}