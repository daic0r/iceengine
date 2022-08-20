#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <exception>
#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <deque>
#include <atomic>
#include <System/task.h>
#include <iostream>
#include <optional>

class thread_pool {
   using task_t = task;

   std::size_t m_nNumThreads{};
   std::size_t m_nNumQueues{};
   std::vector<std::thread> m_vThreads;
   std::vector<std::atomic<std::deque<task_t>*>> m_vQueues;
   std::atomic<bool> m_bDone{ false };
   std::mutex m_readyMtx;
   std::condition_variable m_ready;
   std::atomic<std::size_t> m_nReady{};

public:
   thread_pool(std::size_t numThreads = 0);
   thread_pool(const thread_pool&) = delete;
   thread_pool& operator=(const thread_pool&) = delete;
   thread_pool(thread_pool&&) = delete;
   thread_pool& operator=(thread_pool&&) = delete;
   ~thread_pool();

   template<typename Callable>
   auto async(Callable&& callable) -> std::future<std::invoke_result_t<Callable>> {
      using return_t = std::invoke_result_t<Callable>;
      auto promise = std::promise<return_t>{};
      auto future = promise.get_future();
      if constexpr (std::is_same_v<void, return_t>)
         queue([p=std::move(promise), f=std::forward<Callable>(callable)]() mutable {
            try {
               f();
            }
            catch (...) {
               p.set_exception(std::current_exception());
               return;
            }
            p.set_value();
         });
      else
         queue([p=std::move(promise), f=std::forward<Callable>(callable)]() mutable {
            try {
               p.set_value(f());
            }
            catch (...) {
               p.set_exception(std::current_exception());
            }
         });
      return future;
   }

   std::optional<task_t> try_pop(std::size_t nIdx);
   bool data_ready() const noexcept;

private:
   template<typename Task>
   void queue(Task&& task) {
      static std::size_t nIdx{};
      bool bDone{};
      while (!bDone) {
         auto& slot = m_vQueues.at(nIdx);
         auto pQueue = slot.load(std::memory_order_acquire);
         if (pQueue) {
            if (slot.compare_exchange_strong(pQueue, nullptr, std::memory_order_acq_rel, std::memory_order_relaxed)) {
               pQueue->emplace_front(std::forward<Task>(task));
               slot.store(pQueue, std::memory_order_release);
               m_nReady.fetch_add(1, std::memory_order_release);
               m_ready.notify_all();
               bDone = true;
            }
         }
         nIdx = (nIdx + 1) % m_nNumQueues;
      }
   }
};


#endif
