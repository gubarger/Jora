#pragma once

#include <Jora/Core/DefaultEngineDefines.hpp>

#include <chrono>
#include <algorithm>
#include <source_location>

using namespace std::chrono_literals;

namespace jora
{
	/**
	 * @brief Базовый класс времени для программы.
	 * @tparam FloatType Выбранный тип рационального значения (например, `float`/`double`).
	 */

	template<std::floating_point FloatType = double>
	class Time
	{
	public:
		using Seconds = std::chrono::seconds;
		using Milliseconds = std::chrono::milliseconds;
		using Microseconds = std::chrono::microseconds;
		using Nanoseconds = std::chrono::nanoseconds;

		using Clock = std::chrono::steady_clock;
		using TimePoint = Clock::time_point;
		using Duration = std::chrono::duration<FloatType>;

		/**
		 * @brief Максимальный `DeltaTime` при ошибках.
		 */

		static constexpr Duration defaultMaxDelta = Duration{ FloatType{ 0.25 } };

		explicit Time(Duration maxDelta = defaultMaxDelta) noexcept : startTime_{ Clock::now() }
			, lastFrameTime_{ startTime_ }
			, maxDelta_{ maxDelta }
		{}

		/**
		 * @brief Расчет одного тика *(прохода)* времени.
		 */

		void Tick() noexcept
		{
			const auto currentTime = Clock::now();

			unscaledDelta_ = std::chrono::duration_cast<Duration>(currentTime - lastFrameTime_);
			lastFrameTime_ = currentTime;

			const auto clampedDelta = std::min(unscaledDelta_, maxDelta_);

			delta_ = isPaused_ ? Duration{ 0 } : clampedDelta * timeScale_;
			totalTime_ += delta_;

			Update(unscaledDelta_);
		}

		/**
		 * @brief Сброс счетчика *(lastFrameTime_)*.
		 * @note Обычно первый кадр начинается позже инициализации Window и Vulkan,
		 * поэтому функция перед циклом отрисовки.
		 */

		void Reset() noexcept
		{
			lastFrameTime_ = Clock::now();
		}

		/**
		 * @brief Для получения масштабируемого `DeltaTime` *(детерминированный шаг)*.
		 * @note Подходит для: анимаций, физики, таймеров.
		 * @return Выбранный тип рационального значения.
		 */

		[[nodiscard]] constexpr FloatType DeltaSeconds() const noexcept { return delta_.count(); }

		/**
		 * @brief Получение приватного `delta_` поля класса `Time`.
		 * @return *Delta* времени.
		 */

		[[nodiscard]] constexpr Duration Delta() const noexcept { return delta_; }

		/**
		 * @brief Получение не масштабируемого `DeltaTime` из приватного поля класса `Time`.
		 * @note Подходит для: UI систем, камер меню, отладки.
		 * @return Выбранный тип рационального значения.
		 */

		[[nodiscard]] constexpr FloatType UnscaledDeltaSeconds() const noexcept { return unscaledDelta_.count(); }

		/**
		 * @brief Получение общего времени работы.
		 * @warning Учитывает паузы и замедления.
		 * @return Выбранный тип рационального значения.
		 */

		[[nodiscard]] constexpr FloatType TotalSeconds() const noexcept { return totalTime_.count(); }

		/**
		 * @brief Реальное время работы.
		 * @note Подходит для: профилирования и ведения логов.
		 * @return Реальное время.
		 */

		[[nodiscard]] Duration RealUptime() const noexcept
		{
			return std::chrono::duration_cast<Duration>(Clock::now() - startTime_);
		}

		/**
		 * @brief Обновляет временной масштаб.
		 * @param scale Размер в `FloatType` *(Выбранный тип рационального значения)*.
		 */

		constexpr void SetTimeScale(FloatType scale) noexcept
		{
			timeScale_ = std::max(FloatType{ 0 }, scale);
		}

		/**
		 * @brief Выдает временной масштаб.
		 * @return Выбранный тип рационального значения.
		 */

		[[nodiscard]] constexpr FloatType TimeScale() const noexcept { return timeScale_; }

		/**
		 * @brief Изменяет значение остановки.
		 * @param paused Значение остановки *(bool)*.
		 */

		constexpr void SetPaused(const bool paused) noexcept { isPaused_ = paused; }

		/**
		 * @brief Выдает, остановлено ли время.
		 * @return `bool`.
		 */

		[[nodiscard]] constexpr bool IsPaused() const noexcept { return isPaused_; }

		/**
		 * @brief Получение реального значение кадров на данный момент из приватного поля класса `Time`.
		 * @return Выбранный тип рационального значения.
		 */

		[[nodiscard]] constexpr FloatType Fps() const noexcept { return currentFps_; }

		/**
		 * @brief Просчет кадрового времени в `ms` *(миллисекундах)*.
		 * @return Выбранный тип рационального значения.
		 */

		[[nodiscard]] constexpr FloatType FrameTimeMs() const noexcept
		{
			return unscaledDelta_.count() * FloatType { 1000 };
		}

	private:
		/**
		 * @brief Обновляет значения с учетом плавности *(каждые 0.5 секунды)*.
		 * @param dt Сырые посчитанные данные `DeltaTime`.
		 */

		void Update(Duration dt) noexcept
		{
			fpsAccumulator_ += dt;
			frameCounter_++;

			// Обновление FPS каждые пол секнды.
			if (fpsAccumulator_ >= Duration{ FloatType{0.5} })
			{
				currentFps_ = static_cast<FloatType>(frameCounter_) / fpsAccumulator_.count();
				fpsAccumulator_ = Duration{ 0 };
				frameCounter_ = 0;
			}
		}

	private:
		TimePoint startTime_; // Начало кадра.
		TimePoint lastFrameTime_; // Последний кадр.

		Duration maxDelta_{}; // Максимальное значение DeltaTime
		Duration delta_{}; // Базовый Delta, который считается.
		Duration unscaledDelta_{}; // Не смаштабируемый DeltaTime.
		Duration totalTime_{}; // Общее время (с учсетом остановок).

		FloatType timeScale_ = FloatType{ 1 }; // Масштаб времени.

		bool isPaused_ = false; // Флаг остановки времени обновлений.

		// Для расчета FPS
		Duration fpsAccumulator_{}; // Аккумулятор для просчета FPS.
		std::uint32_t frameCounter_ = 0; // Счетчик кадров.
		FloatType currentFps_ = FloatType{ 0 }; // Реальное значение кадров на данный момент.
	};

	/**
	 * @brief Класс для таймеров у объектов.
	 * @tparam FloatType Выбранный тип рационального значения (например, `float`/`double`).
	 */

	template<std::floating_point FloatType = double>
	class Cooldown
	{
	public:
		explicit constexpr Cooldown(FloatType durationSec) noexcept : duration_{ durationSec } {}

		constexpr void Update(FloatType dt) noexcept
		{
			if (remaining_ > FloatType{ 0 })
			{
				remaining_ = std::max(FloatType{ 0 }, remaining_ - dt);
			}
		}

		constexpr void Trigger() noexcept { remaining_ = duration_; }

		[[nodiscard]] constexpr bool IsReady() const noexcept { return remaining_ <= FloatType{ 0 }; }

		[[nodiscard]] constexpr FloatType Progress() const noexcept
		{
			return duration_ > FloatType{ 0 } ? FloatType{ 1 } - (remaining_ / duration_) : FloatType{ 1 };
		}

	private:
		FloatType duration_; // Продолжительность.
		FloatType remaining_ = FloatType{ 0 }; // Отставание.
	};

	/**
	 * @brief Структура для хранения информции времени и расположения.
	 */

	class ScopedTimer
	{
	public:
		explicit ScopedTimer(std::source_location loc = std::source_location::current()) noexcept
			: loc_{ loc }, start_{ std::chrono::steady_clock::now() } {}

		~ScopedTimer();

		ScopedTimer(const ScopedTimer&) = delete;
		ScopedTimer& operator=(const ScopedTimer&) = delete;

	private:
		std::source_location loc_; // Расположение.
		std::chrono::steady_clock::time_point start_;
	};
}