#include "threadPool_v3.h"
#include "add.h"

int main()
{
  ThreadPool pool(3);
  std::vector<std::future<int>> futures;
  for (int i = 0; i < 10; i++)
  {
    futures.emplace_back(pool.EnqueueJob(work, i % 3 + 1, i));
  }
  for (auto &f : futures)
  {
    printf("result : %d \n", f.get());
  }
}