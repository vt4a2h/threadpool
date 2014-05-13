#include "threadpool.h"

namespace threads {

  ThreadPool::ThreadPool(size_t max_threads_count)
    : done_(false)
    , threads_count_(max_threads_count ? max_threads_count : std::thread::hardware_concurrency())
    , joiner_(threads_)
  {
    try {
      for (size_t i = 0; i < threads_count_; ++i)
        threads_.push_back(std::thread(&ThreadPool::task_runner, this));
    } catch (...) {
      done_ = true;
      throw;
    }
  }

  ThreadPool::~ThreadPool()
  {
    done_ = true;
  }

  void ThreadPool::task_runner()
  {
    while(!done_) {
      FunctionWrapper task;

      if (tasks_queue_.TryPop(task))
        task();
      else
        std::this_thread::yield();
    }
  }

}
