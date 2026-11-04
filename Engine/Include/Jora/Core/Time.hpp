#pragma once

#include <Jora/Core/DefaultEngineDefines.hpp>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace jora
{

	using Seconds = std::chrono::seconds;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;

	using SteadyPoint = std::chrono::steady_clock::time_point;
	using SystemPoint = std::chrono::system_clock::time_point;
	using HighPoint = std::chrono::high_resolution_clock::time_point;

	const std::chrono::duration<double> FrameDuration(1.0 / Settings::TargetFps);

	/**
	 * @brief Включает или выключает таймер *(std::chrono::steady_clock::now())*.
	 * @return Конкретное время таймера.
	 */

	constexpr SteadyPoint SteadyTimerBlock() noexcept
	{
		return std::chrono::steady_clock::now();
	}

	/**
	 * @brief Включает или выключает таймер *(std::chrono::system_clock::now())*.
	 * @return Конкретное время таймера.
	 */

	constexpr SystemPoint SystemTimerBlock() noexcept
	{
		return std::chrono::system_clock::now();
	}

	/**
	 * @brief Включает или выключает таймер *(std::chrono::high_resolution_clock::now())*.
	 * @return Конкретное время таймера.
	 */

	constexpr HighPoint HighTimerBlock() noexcept
	{
		return std::chrono::high_resolution_clock::now();
	}

	/**
	 * @brief Форматирование локального времени на ПК.
	 * @param systemTime Конкретное системное время в установленной ОС зоне.
	 * @return Форматированное локальное время *(std::chrono::local_time)*.
	 */

	inline constexpr const auto SetFormatLocalSystemTime
	(const SystemPoint& systemTime) noexcept
	{
		return std::chrono::current_zone()->to_local(systemTime);
	}

	/**
	 * @brief Класс для обработки времени *(кадров, физики, задержек и т.д.)*.
	 */

	class GlobalTime
	{
	public:
		/**
		 * @brief Вычисление `deltaTime` в *секундах*.
		 */

		void BeginCountFrame()
		{
			currentTime_ = HighTimerBlock();

			elapsed_ = currentTime_ - previousTime_;

			previousTime_ = currentTime_;
			deltaTime_ = elapsed_.count();
		}

		/**
		 * @brief Подсчет реального *FPS*.
		 */

		void EndCountFrame()
		{
			frameCount_++;

			if (currentTime_ - lastFps_ >= 1s)
			{
				frameCount_ = 0;
				lastFps_ = currentTime_;
			}

			// Ограничение FPS.
			auto frameEndTime = HighTimerBlock();
			auto frameExecutionTime = std::chrono::duration_cast<Microseconds>(frameEndTime - currentTime_);

			if (frameExecutionTime < FrameDuration)
			{
				// Оставшееся время кадра
				std::this_thread::sleep_for(FrameDuration - frameExecutionTime);
			}
		}

		constexpr float DeltaTime() const noexcept
		{
			return deltaTime_;
		}

		constexpr HighPoint LastFPS() const noexcept
		{
			return lastFps_;
		}

		constexpr int32_t FrameCount() const noexcept
		{
			return frameCount_;
		}

		/**
		 * @brief Выводит в консоль информацию о времени.
		 */

		void View() const noexcept;

	private:
		std::chrono::duration<float> elapsed_;
		HighPoint currentTime_;
		HighPoint previousTime_;
		HighPoint lastFps_;
		int32_t frameCount_ = 0;
		float deltaTime_ = 0.0f;
	};

}