#ifndef THREADPOOL_V3_H
#define THREADPOOL_V3_H

#include <iostream>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace tp3
{
  class ThreadPool
  {
  public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    template <class F, class... Args>
    std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(
        F &&f, Args &&...args);

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
      std::cout << "Threadpool construction & Assigning a job to thread" << std::endl;
    }
  }

  ThreadPool::~ThreadPool()
  {
    stop_all = true;
    cv_job_q_.notify_all();
    for (auto &t : worker_threads_)
    {
      t.join();
      std::cout << "Joining a thread" << std::endl;
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

  template <class F, class... Args>
  std::future<typename std::result_of<F(Args...)>::type> ThreadPool::EnqueueJob(
      F &&f, Args &&...args)
  {
    if (stop_all)
    {
      throw std::runtime_error("ThreadPool 사용 중지됨");
    }
    using return_type = typename std::result_of<F(Args...)>::type;
    auto job = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> job_result_future = job->get_future();
    {
      std::lock_guard<std::mutex> lock(m_job_q_);
      jobs_.push([job]()
                 { (*job)(); });
    }
    cv_job_q_.notify_one();
    return job_result_future;
  }
}

#endif