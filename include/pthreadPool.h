#ifndef PTHREADPOOL_H
#define PTHREADPOOL_H

#include <pthread.h>
#include <queue>

namespace ptp
{
  class ThreadPool
  {
  public:
    ThreadPool(int num_threads);
    ~ThreadPool();

    void submit_task(void (*task)(void *), void *arg);
    void wait();
    static void *worker(void *arg);

  private:
    int num_threads;
    std::vector<pthread_t> threads;
    std::queue<std::pair<void (*)(void *), void *>> tasks;
    pthread_mutex_t task_mutex;
    pthread_cond_t task_cond;
  };

  ThreadPool::ThreadPool(int num_threads)
  {
    this->num_threads = num_threads;
    threads.resize(num_threads);
    pthread_mutex_init(&task_mutex, NULL);
    pthread_cond_init(&task_cond, NULL);
    for (int i = 0; i < num_threads; i++)
    {
      pthread_create(&threads[i], NULL, worker, (void *)this);
    }
  }

  ThreadPool::~ThreadPool()
  {
    pthread_mutex_destroy(&task_mutex);
    pthread_cond_destroy(&task_cond);
    for (int i = 0; i < num_threads; i++)
    {
      pthread_join(threads[i], NULL);
    }
  }

  void ThreadPool::submit_task(void (*task)(void *), void *arg)
  {
    pthread_mutex_lock(&task_mutex);
    tasks.push(std::make_pair(task, arg));
    pthread_cond_signal(&task_cond);
    pthread_mutex_unlock(&task_mutex);
  }

  void ThreadPool::wait()
  {
    pthread_mutex_lock(&task_mutex);
    while (!tasks.empty())
    {
      pthread_cond_wait(&task_cond, &task_mutex);
    }
    for (int i = 0; i < num_threads; i++)
    {
      pthread_cond_signal(&task_cond);
    }
    pthread_mutex_unlock(&task_mutex);
  }

  void* ThreadPool::worker(void *arg)
  {
    ThreadPool *pool = (ThreadPool *)arg;
    while (true)
    {
      pthread_mutex_lock(&pool->task_mutex);
      while (pool->tasks.empty())
      {
        pthread_cond_wait(&pool->task_cond, &pool->task_mutex);
      }
      if (pool->tasks.empty())
      {
        pthread_mutex_unlock(&pool->task_mutex);
        break;
      }
      void (*task)(void *) = pool->tasks.front().first;
      void *task_arg = pool->tasks.front().second;
      pool->tasks.pop();
      pthread_mutex_unlock(&pool->task_mutex);
      task(task_arg);
    }
    pthread_exit(NULL);
  }
}

#endif