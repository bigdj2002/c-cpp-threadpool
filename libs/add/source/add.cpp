#include "add.h"

int work(int t, int id)
{
  printf("%d start \n", id);
  std::this_thread::sleep_for(std::chrono::seconds(t));
  printf("%d end after %ds\n", id, t);
  return t + id;
}