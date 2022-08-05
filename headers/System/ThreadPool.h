#pragma once

#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <queue>
#include <mutex>
#include <future>
#include <iostream>

#define threadPool ThreadPool::instance()

class ThreadPool {
	using TaskType = std::function<void()>;

	std::condition_variable m_tasksAvailable;
	std::vector<std::thread> m_vThreads;
	std::queue<TaskType> m_qTaskQueue;
	std::mutex m_queueMutex;
	std::atomic<bool> m_bDone{ false };

	template<typename F>
	auto make_task(F&& func, std::shared_ptr<std::promise<decltype(func())>>&& promise) noexcept -> typename std::enable_if_t<std::is_same_v<decltype(func()), void>, std::function<void()>>
	{
		auto task = [promise = std::move(promise), func = std::forward<F>(func)]() mutable {
			func();
			promise->set_value();
		};
		return task;
	}

	template<typename F>
	auto make_task(F&& func, std::shared_ptr<std::promise<decltype(func())>>&& promise) noexcept -> typename std::enable_if_t<!std::is_same_v<decltype(func()), void>, std::function<void()>>
	{
		auto task = [promise = std::move(promise), func = std::forward<F>(func)]() mutable {
			promise->set_value(func());
		};
		return task;
	}

public:
	static ThreadPool& instance() noexcept;

	ThreadPool();
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;
	~ThreadPool();

	template<typename F>
	auto queue(F&& func) noexcept -> std::future<decltype(func())> {
		using ReturnType = decltype(func());
		auto promise = std::make_shared<std::promise<ReturnType>>();
		auto fut = promise->get_future();
		auto task = make_task(std::forward<F>(func), std::move(promise));
		{
			std::scoped_lock<std::mutex> lck{ m_queueMutex };
			m_qTaskQueue.emplace(std::move(task));
		}
		m_tasksAvailable.notify_one();
		return fut;
	}

	void shutDown() noexcept;
};