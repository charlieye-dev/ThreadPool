#include <iostream>
#include <unistd.h>

#include "thread_pool.h"

static void *func(void *data) {
  printf("thread %d\n", int(data));
  sleep(1);
  return NULL;
}

int main() {
  int i = 0;
  ThreadPool *test = new ThreadPool(5);

  test->CreateThread();

  std::vector<pthread_t> id(test->GetThreadId());
  for(i; i < id.size(); i++)
    printf("%ld\n", id[i]);

  for(i = 0; i < 10; i++)
    test->AddTask(func, (void *)i);

  sleep(3);
  delete test;

  return 0;
}
