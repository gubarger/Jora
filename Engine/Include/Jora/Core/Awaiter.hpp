#pragma once

#include <coroutine>

/**
 * Возможна цикличная зависимость (при подключении): непонятно как избавиться.
 * Возможно, будет перенесено все в *ThreadPool.hpp*.
 */

#include <Jora/Core/ThreadPool.hpp>

namespace jora
{

	/**
	 * @brief Корутина-плонировщик для асинхронных задач в `BasicThreadPool` *(см BasicThreadPool)*
	 */

	struct Schedule
	{
		BasicThreadPool& pool;
		TaskPriority priority = TaskPriority::Low;

		/**
		 * @brief Всегда приостонавливает.
		 * @return Всегда `false`
		 */

		bool AwaitReady() const noexcept { return false; }

		/**
		 * @brief Рабочий *(см. BasicThreadPool)* продолжает корутину.
		 * @param handle Ручка, чтобы корутина продолжала с указанной позиции *(отсюда)*.
		 */

		void AwaitSuspend(std::coroutine_handle<> handle) const
		{
			pool.Post(priority, [handle] { handle.resume(); });
		}

		void AwaitResume() const noexcept { /* Нет логики */ }
	};

	/**
	 * @brief Базовая структура корутины.
	 */

	struct Detached
	{
		/**
		 * @brief Стандартный тип корутины.
		 */

		struct promise_type
		{
			Detached get_return_object() noexcept { return {}; }

			/**
			 * @brief Функция для старта.
			 * @return Базовый конструктор.
			 */

			std::suspend_never initial_suspend() noexcept { return {}; }

			/**
			 * @brief Функция для освобождения *(делает это сам)*.
			 * @return Базовый конструктор.
			 */

			std::suspend_never final_suspend() noexcept { return {}; }

			void return_void() noexcept {}

			void unhandled_exception() noexcept { std::terminate(); }
		};
	};

}