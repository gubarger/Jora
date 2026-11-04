#pragma once

#include <queue>
#include <thread>
#include <future>
#include <vector>
#include <atomic>
#include <algorithm>
// #include <latch>

namespace jora
{

	/**
	 * @brief Приоритет задачи для выполнения.
	 */

	enum class TaskPriority : uint32_t
	{
		High = 2, // Выполняется в первую очередь.
		Low = 1, // Всегда выполняется в последнюю очередь.
	};

	/**
	 * @brief Базовый пул потоков, где постоянно живут "работники" задач.
	 */

	class BasicThreadPool
	{
	public:
		/**
		 * @brief Тип задачи.
		 */

		using Task = std::move_only_function<void()>;

		/**
		 * @brief Конструктор создает количество потоков по умолчанию.
		 * @param threads Кол-во потоков (по умолчанию число логических процессов).
		 */

		explicit BasicThreadPool(size_t threads = std::max(1u, std::thread::hardware_concurrency()));

		~BasicThreadPool();

		BasicThreadPool(const BasicThreadPool&) = delete;
		BasicThreadPool& operator=(const BasicThreadPool&) = delete;

		/**
		 * @warning Потоки держат this, поэтому перемещать нельзя
		 */

		BasicThreadPool(BasicThreadPool&&) = delete;
		BasicThreadPool& operator=(BasicThreadPool&&) = delete;

		/**
		 * @brief Функция добовления задачи в очередь.
		 * @tparam Func *Тип* добовляемой функции.
		 * @tparam ...Args *Типы* добовляемых аргументов
		 * @param priority Приоритет задачи.
		 * @param func Функция.
		 * @param ...args Аргументы `func`.
		 * @return Объект ассинхронно выполненной операции (`std::future`).
		 */

		template<class _Func, class... _Args>
		[[nodiscard]] auto Sumbit(TaskPriority priority, _Func&& func, _Args&&... args) ->
			std::future<std::invoke_result_t<std::decay_t<_Func>, std::decay_t<_Args>...>>
		{
			using Result = std::invoke_result_t<std::decay_t<_Func>, std::decay_t<_Args>...>;

			std::packaged_task<Result()> task(
				[func = std::forward<_Func>(func), ... args = std::forward<_Args>(args)]() mutable -> Result
				{
					return std::invoke(std::move(func), std::move(args)...);
				}
			);

			auto future = task.get_future();

			Enqueue(priority, Task(std::move(task)));

			return future;
		}

		/**
		 * @brief Задача без `std::future`.
		 * @param priority Приоритет задачи.
		 * @param task Задача.
		 */

		void Post(TaskPriority priority, Task task) { Enqueue(priority, std::move(task)); }

	private:
		/**
		 * @brief Добовляет в очереди (`high_, low_`) задачу.
		 * @param priority Приоритет задачи.
		 * @param task Задача.
		 */

		void Enqueue(TaskPriority priority, Task task);

		/**
		 * @brief Добовляет в очереди (`high_, low_`) массив задач.
		 * @param priority Приоритет задач.
		 * @param tasks Массив задач.
		 */

		void EnqueuePack(TaskPriority priority, std::vector<Task> tasks);

		/**
		 * @brief Функция с бесконечно работающими потоками.
		 */

		void Run();

	private:
		std::mutex mutex_;
		std::condition_variable conditionVariable_;
		bool stop_ = false;

		std::deque<Task> high_, low_; // Две очереди с разными задачами по приоритетам.
		std::vector<std::jthread> workers_; // Должен быть последним в объявлении полей.
	};

}