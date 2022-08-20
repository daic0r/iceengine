#include <System/ThreadPool.h>
#include <iostream>

ThreadPool::ThreadPool(std::size_t nNumThreads)
{
	const size_t n = nNumThreads == 0 ? std::thread::hardware_concurrency() - 1 : nNumThreads;
	m_vThreads.reserve(n);
	for (size_t i = 0; i < n; ++i) {
		m_vThreads.emplace_back([this, i]() {
			while (true) {
				TaskType task;
				{
					std::unique_lock<std::mutex> lck{ m_queueMutex };
					m_tasksAvailable.wait(lck, [this]() { return m_qTaskQueue.size() > 0 || m_bDone.load(std::memory_order_acquire); });
					if (m_bDone.load(std::memory_order_acquire))
						break;
					task = std::move(m_qTaskQueue.front());
					m_qTaskQueue.pop();
				}
				task();
			}
		});
	}
	std::cout << "Thread pool up." << std::endl;
}

ThreadPool::~ThreadPool()
{
	shutDown();
}

void ThreadPool::shutDown() noexcept
{
	m_bDone.store(true, std::memory_order_release);
	m_tasksAvailable.notify_all();
	for (auto& t : m_vThreads) {
		t.join();
	}
	std::cout << "Thread pool shut down." << std::endl;
}