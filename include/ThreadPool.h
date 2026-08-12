#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    // Create a pool with a specific number of background workers
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Push a generic function into the queue for a worker to execute
    void EnqueueTask(std::function<void()> task);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};