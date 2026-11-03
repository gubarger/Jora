#include <Jora/Core/ThreadPool.hpp>

namespace jora
{

	BasicThreadPool::BasicThreadPool(size_t threads)
	{
		workers_.reserve(threads);
		for (size_t i = 0; i < threads; ++i)
		{
			workers_.emplace_back([this] { Run(); }); // Поток запускается и заходит в ожидание задач.
		}
	}

	BasicThreadPool::~BasicThreadPool()
	{
		{
			std::lock_guard lock(mutex_);
			stop_ = true;
		}

		conditionVariable_.notify_all();
	}

	void BasicThreadPool::Enqueue(TaskPriority priority, Task task)
	{
		{
			std::lock_guard lock(mutex_);
			(priority == TaskPriority::High ? high_ : low_).push_back(std::move(task));
		}

		conditionVariable_.notify_one();
	}

	void BasicThreadPool::EnqueuePack(TaskPriority priority, std::vector<Task> tasks)
	{
		{
			std::lock_guard lock(mutex_);
			for (auto& task : tasks)
			{
				(priority == TaskPriority::High ? high_ : low_).push_back(std::move(task));
			}
		}

		conditionVariable_.notify_all();
	}

	void BasicThreadPool::Run()
	{
		while (true)
		{
			Task task;
			{
				std::unique_lock lock(mutex_);

				conditionVariable_.wait(lock, [this] {
					return stop_ || !high_.empty() || !low_.empty();
					}); // Поток ждет задачу.

				if (high_.empty() && low_.empty())
				{
					return; // Поток завершил работу
				}

				auto& queue = !high_.empty() ? high_ : low_;

				task = std::move(queue.front()); // Поток получил задачу по приоритету.

				queue.pop_front();
			}

			task(); // Поток выполняет задачу.
		}
	}

}