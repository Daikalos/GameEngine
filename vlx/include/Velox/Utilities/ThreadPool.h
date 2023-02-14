#pragma once

#include <future>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>

namespace vlx
{
    class ThreadPool
    {
    public:
        ThreadPool()
        {
            m_threads.reserve(std::thread::hardware_concurrency());
            for (std::size_t i = 0; i < m_threads.size(); ++i)
                m_threads.push_back(std::thread(&ThreadPool::ThreadLoop, this));
        }

        ThreadPool(std::size_t size)
        {
            m_threads.reserve(size);
            for (std::size_t i = 0; i < m_threads.size(); ++i)
                m_threads.push_back(std::thread(&ThreadPool::ThreadLoop, this));
        }

        ~ThreadPool()
        {
            m_shutdown = true;
            m_cv.notify_all();
            for (std::thread& thread : m_threads)
                thread.join();
        }

        template<class F, class... Args>
        auto Enqueue(F&& f, Args&&... args)
        {
            using return_type = decltype(f(args...));

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), 
                std::forward<Args>(args)...));

            std::lock_guard<std::mutex> lock(m_mutex); // lock to synchronize

            // place task in queue
            m_tasks.emplace([task]()
                {
                    (*task)();
                });

            m_cv.notify_one(); // notify a thread that a task is available

            return task->get_future();
        }

    private:
        void ThreadLoop()
        {
            while (true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(m_mutex);

                    m_cv.wait(lock, [this]
                        {
                            return !m_tasks.empty() || m_shutdown;
                        });

                    if (m_shutdown)
                        break;

                    task = m_tasks.front();
                    m_tasks.pop();
                }

                task();
            }
        }

    private:
        std::vector<std::thread>            m_threads;
        std::queue<std::function<void()>>   m_tasks;
        std::condition_variable             m_cv;
        std::mutex                          m_mutex; // sync access to task queue
        bool                                m_shutdown{ false };
    };
}
