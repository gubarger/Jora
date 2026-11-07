#pragma once

namespace jora
{

	/**
	 * @brief Оператор для определения обновления кадров или FPS *(герцовки)*.
	 * @param monitorVal Желаемая частота обновления, которая будет поддерживаться
	 * *(зависит от обновления монитора)*.
	 * @return `long double`.
	 */

	constexpr long double operator"" Hz(unsigned long long int monitorVal) noexcept {
		return 1.0 / monitorVal;
	}

	/**
	 * @brief Структура для общих настроек программы.
	 * @note Возможно, будет перенесена из этого файла в отдельный `Settings.hpp`.
	 */

	struct Settings
	{

		static constexpr inline auto FixedDt = 60Hz;
	};

}