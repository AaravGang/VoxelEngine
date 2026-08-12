#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads)
    : stop(false) {
    // Spin up the workers
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    // Lock the queue so this thread can safely check for work
                    std::unique_lock<std::mutex> lock(this->queueMutex);

                    // Put the thread to sleep until a task arrives OR the pool is stopped
                    this->condition.wait(lock,
                                         [this] { return this->stop || !this->tasks.empty(); });

                    // If the pool is stopping and the queue is empty, exit the thread
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }

                    // Grab the task from the front of the queue
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                } // Lock is automatically released here when it goes out of scope

                // Execute the task outside of the lock so other threads aren't blocked!
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    // Wake up all sleeping threads so they see the stop signal and exit cleanly
    condition.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::EnqueueTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop)
            return;
        tasks.push(std::move(task));
    }
    // Wake up exactly one sleeping thread to handle this new task
    condition.notify_one();
}