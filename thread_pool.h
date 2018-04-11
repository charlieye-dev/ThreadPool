#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <mutex>
#include <vector>
#include <pthread.h>

#define DEFAULT_NUM 0

typedef struct thread_task {
  void (*task)(void *data);
  void *data;
  struct thread_task *next;
} T_TASK_;

typedef struct thread_pool {
  bool active;        // if thread pool active.
  int max_num;       // max thread number.
  std::vector<pthread_t> thread;  // thread id array.
  T_TASK_ *queue;
  pthread_mutex_t lock;
  pthread_cond_t cond;
} T_POOL_;

class ThreadPool {
public:
  ThreadPool(int num = DEFAULT_NUM);

  ~ThreadPool();

  /* Create thread */
  bool CreateThread();

  /* Add task to thread */
  bool AddTask(void *(*task)(void *data), void *data);

  /* Get thread id array */
  std::vector<pthread_t> GetThreadId();

private:
  T_POOL_ *pool;
};

#endif
