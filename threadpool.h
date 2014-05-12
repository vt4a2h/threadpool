#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <future>

#include "threadsafequeue.h"
#include "functionwrapper.h"
#include "joiner.h"

namespace threads {

  class ThreadPool
  {
    public:
      ThreadPool(size_t threads_count);
      ~ThreadPool();

      template <class F>
      std::future<typename std::result_of<F()>::type> AddTask(F f);

    private:
      void task_runner();

      std::atomic_bool done_;
      std::atomic_uint threads_count_;
      ThreadsafeQueue<FunctionWrapper> tasks_queue_;
      Threads threads_;
      Joiner  joiner_;
  };

  template <class F>
  std::future<typename std::result_of<F()>::type> ThreadPool::AddTask(F f)
  {
    typedef typename std::result_of<F()>::type result_type;

    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type>          result(task.get_future());

    tasks_queue_.Push(std::move(task));
    return result;
  }

}

#endif // THREADPOOL_H
