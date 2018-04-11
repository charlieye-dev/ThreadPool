#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread_pool.h"

static void *task_callback(void *data) {
  T_TASK_ *task;
  T_POOL_ *pool = (T_POOL_ *)data;

  while(1) {
    pthread_mutex_lock(&pool->lock);
    while(!pool->queue && pool->active) {
      pthread_cond_wait(&pool->cond, &pool->lock);
    }
    if(!pool->active) {
      pthread_mutex_unlock(&pool->lock);
      pthread_exit(NULL);
    }

    task = pool->queue;
    pool->queue = pool->queue->next;
    pthread_mutex_unlock(&pool->lock);

    task->task(task->data);
  }

  return;
}

ThreadPool::ThreadPool(int num = DEFAULT_NUM) {
  pool = (T_POOL_ *)malloc(sizeof(T_POOL_));
  if(pool == NULL) {
    printf("failed to create pool.\n");
  }

  pool->max_num = num;
  pool->active = false;
  pool->queue = NULL;

  pthread_mutex_init(&pool->lock, NULL);
  pthread_cond_init(&pool->cond, NULL);

  std::vector<pthread_t> thread_num(num);
  pool->thread = thread_num;
}

ThreadPool::~ThreadPool() {
  int i = 0;

  pool->active = false;

  pthread_mutex_lock(&pool->lock);
  pthread_cond_broadcast(&pool->cond);
  pthread_mutex_unlock(&pool->lock);

  for(i; i < pool->max_num; i++) {
    pthread_join(pool->thread[i], NULL);
  }

  pthread_mutex_destroy(&pool->lock);
  pthread_cond_destroy(&pool->cond);

  pool->thread.clear();
  free(pool);
}

bool ThreadPool::CreateThread() {
  int i = 0;
  int ret;

  pool->active = true;

  for(i; i < pool->max_num; i++) {
    ret = pthread_create(&pool->thread[i], NULL, task_callback, pool);
    if(ret != 0) {
      printf("failed to create thread %d.\n", i);
      return false;
    }
  }

  return true;
}

bool ThreadPool::AddTask(void *(*task)(void *data), void *data) {
  T_TASK_ *work, *member;

  if(task == NULL) {
    printf("No task add.\n");
    return false;
  }

  work = (T_TASK_ *)malloc(sizeof(T_TASK_));
  if(work == NULL) {
    printf("failed to malloc memery.\n");
    return false;
  }
  work->task = task;
  work->data = data;
  work->next = NULL;

  pthread_mutex_lock(&pool->lock);
  member = pool->queue;
  if(!member) {
    pool->queue = work;
  } else {
    while(member->next) {
      member = member->next;
    }
    member->next = work;
  }
  pthread_cond_signal(&pool->cond);
  pthread_mutex_unlock(&pool->lock);

  return true;
}
std::vector<pthread_t> ThreadPool::GetThreadId() {
  return pool->thread;
}
