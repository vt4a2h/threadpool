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
      ThreadPool(size_t max_threads_count);
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

  /*!
   * \brief Method for adding task to the one free thread.
   *        If you want to add task with parameters, you should use std::bind.
   *        Example:
   *          pool.AddTask(std::bind(sum, 1, 2));
   *
   * \param f function or object with operator().
   * \return std::future object with calculation result.
   *         For get result use std::future::get.
   *         Example:
   *          auto fut = pool.AddTask(std::bind(sum, 1, 2));
   *          auto s = fut.get(); // s == 3
   */
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
