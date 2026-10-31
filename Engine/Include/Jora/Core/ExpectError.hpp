#pragma once

#include <Jora/Core/Logger.hpp>

#include <expected>

namespace jora
{

	/**
	 * @brief Простая обертка над std::expected.
	 *
	 * @tparam Result	Тип, который представляет возврат *успешного* значения.
	 * @tparam Error	Тип, который представляет возврат *ошибочного* значения (уведомление об ошибке).
	 */

	template<typename Result, typename Error>
	class ExpectErrorWrapper
	{
	public:

		/**
		 * @brief Конструктор для создания *успешных* типов.
		 * @tparam ...Args Аргументы *любого типа* (например, std::vector).
		 * @param ...args Вставляемые аргументы в конструктор.
		 *
		 * Пример:
		 * @code{.cpp}
		 * ExpectErrorWrapper<std::vector<int>, std::string> successBox(std::in_place, 5, 52);
		 * successBox.ViewStatus(); // Успех
		 *
		 * ExpectErrorWrapper<std::vector<int>, std::string> errorBox(std::unexpect, "Not found shader file");
		 * errorBox.ViewStatus(); // Ошибка
		 * @endcode
		 */

		template<typename... Args>
		explicit ExpectErrorWrapper(std::in_place_t, const Args&... args)
			: expection_(std::in_place, args...) {}

		template<typename... Args>
		explicit ExpectErrorWrapper(std::unexpect_t, const Args&... args)
			: expection_(std::unexpect, args...) {}

		/**
		* @brief Выводит информацию о статусе и проверяет.
		*/

		inline constexpr bool ViewStatus() const noexcept
		{
			if (expection_.has_value())
			{
				MessageLogging::QuickMessage("Values ​​are present.");
				return true;
			}
			else
			{
				MessageLogging::QuickMessage("Values ​​are missing, or an error has occurred.");
				return false;
			}
		}

	private:
		std::expected<Result, Error> expection_;
	};

}