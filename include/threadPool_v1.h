#ifndef THREADPOOL_V1_H
#define THREADPOOL_V1_H

#include <iostream>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace tp1
{
  class ThreadPool
  {
  public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    void EnqueueJob(std::function<void()> job);

  private:
    size_t num_threads_;
    std::vector<std::thread> worker_threads_;
    std::queue<std::function<void()>> jobs_;

    std::condition_variable cv_job_q_;
    std::mutex m_job_q_;

    bool stop_all;

    void WorkerThread();
  };

  ThreadPool::ThreadPool(size_t num_threads)
      : num_threads_(num_threads), stop_all(false)
  {
    worker_threads_.reserve(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i)
    {
      worker_threads_.emplace_back([this]()
                                   { this->WorkerThread(); });
    }
  }

  ThreadPool::~ThreadPool()
  {
    stop_all = true;
    cv_job_q_.notify_all();

    for (auto &t : worker_threads_)
    {
      t.join();
    }
  }

  void ThreadPool::WorkerThread()
  {
    while (true)
    {
      std::unique_lock<std::mutex> lock(m_job_q_);
      cv_job_q_.wait(lock, [this]()
                     { return !this->jobs_.empty() || stop_all; });
      if (stop_all && this->jobs_.empty())
      {
        return;
      }

      std::function<void()> job = std::move(jobs_.front());
      jobs_.pop();
      lock.unlock();

      job();
    }
  }

  void ThreadPool::EnqueueJob(std::function<void()> job)
  {
    if (stop_all)
    {
      throw std::runtime_error("ThreadPool 사용 중지됨");
    }
    {
      std::lock_guard<std::mutex> lock(m_job_q_);
      jobs_.push(std::move(job));
    }
    cv_job_q_.notify_one();
  }
}

/**
 * \author: bigdj2002@naver.com
 * \brief: Use below code with threadPool_v1.h in main.cpp
 */

// int main()
// {
//   ThreadPool::ThreadPool pool(3);
//   for (int i = 0; i < 10; i++)
//   {
//     pool.EnqueueJob([i]()
//                     { work(i % 3 + 1, i); });
//   }
// }

#endif