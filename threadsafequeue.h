#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace threads {

  template <class T>
  class ThreadsafeQueue
  {
    public:
      ThreadsafeQueue();
      ThreadsafeQueue(const ThreadsafeQueue &) = delete;
      ThreadsafeQueue &operator =(const ThreadsafeQueue &) = delete;

      std::shared_ptr<T> TryPop();
      bool TryPop(T &value);

      std::shared_ptr<T> WaitAndPop();
      void WaitAndPop(T &value);

      void Push(T value);
      bool IsEmpty();

    private:
      struct Node {
          std::shared_ptr<T> data_;
          std::unique_ptr<Node> next_;
      };

      Node *get_tail();
      std::unique_ptr<Node> pop_head();
      std::unique_lock<std::mutex> wait_for_data();
      std::unique_ptr<Node> wait_pop_head();
      std::unique_ptr<Node> wait_pop_head(T &value);

      std::unique_ptr<Node> try_pop_head();
      std::unique_ptr<Node> try_pop_head(T &value);

      std::mutex head_mutex_;
      std::mutex tail_mutex_;
      std::unique_ptr<Node> head_;
      Node *tail_;
      std::condition_variable data_cond_;
  };

  template <class T>
  ThreadsafeQueue<T>::ThreadsafeQueue()
    : head_(new Node)
    , tail_(head_.get())
  {
  }

  template <class T>
  std::shared_ptr<T> ThreadsafeQueue<T>::TryPop()
  {
    std::unique_ptr<Node> old_head = try_pop_head();
    return (old_head ? old_head->data_ : std::shared_ptr<T>());
  }

  template <class T>
  bool ThreadsafeQueue<T>::TryPop(T &value)
  {
    std::unique_ptr<Node> const old_head = try_pop_head(value);
    return old_head.operator bool();
  }

  template <class T>
  std::shared_ptr<T> ThreadsafeQueue<T>::WaitAndPop()
  {
    std::unique_ptr<Node> const old_head = wait_pop_head();
    return old_head->data_;
  }

  template <class T>
  void ThreadsafeQueue<T>::WaitAndPop(T &value)
  {
    std::unique_ptr<Node> const old_head = wait_pop_head(value);
  }

  template <class T>
  void ThreadsafeQueue<T>::Push(T value)
  {
    std::shared_ptr<T> new_data(std::make_shared<T>(std::move(value)));

    std::unique_ptr<Node> p(new Node);
    {
      std::lock_guard<std::mutex> tail_lock(tail_mutex_);
      tail_->data_ = new_data;
      Node * const new_tail = p.get();
      tail_->next_ = std::move(p);
      tail_ = new_tail;
    }

    data_cond_.notify_one();
  }

  template <class T>
  bool ThreadsafeQueue<T>::IsEmpty()
  {
    std::lock_guard<std::mutex> head_lock(head_mutex_);
    return (head_.get() == get_tail());
  }

  template <class T>
  std::unique_ptr<typename ThreadsafeQueue<T>::Node> ThreadsafeQueue<T>::pop_head()
  {
    std::unique_ptr<Node> old_head = std::move(head_);
    head_ = std::move(old_head->next_);
    return old_head;
  }

  template <class T>
  std::unique_lock<std::mutex> ThreadsafeQueue<T>::wait_for_data()
  {
    std::unique_lock<std::mutex> head_lock(head_mutex_);
    data_cond_.wait(head_lock, [&]{ return head_.get() != get_tail(); });
    return std::move(head_lock);
  }

  template <class T>
  std::unique_ptr<typename ThreadsafeQueue<T>::Node> ThreadsafeQueue<T>::wait_pop_head()
  {
    std::unique_lock<std::mutex> head_lock(wait_for_data());
    return pop_head();
  }

  template <class T>
  std::unique_ptr<typename ThreadsafeQueue<T>::Node> ThreadsafeQueue<T>::wait_pop_head(T &value)
  {
    std::unique_lock<std::mutex> head_lock(wait_for_data());
    value = std::move(*head_->data_);
    return pop_head();
  }

  template <class T>
  std::unique_ptr<typename ThreadsafeQueue<T>::Node> ThreadsafeQueue<T>::try_pop_head()
  {
    std::lock_guard<std::mutex> head_lock(head_mutex_);

    if (head_.get() == get_tail()) return std::unique_ptr<Node>();

    return pop_head();
  }

  template <class T>
  std::unique_ptr<typename ThreadsafeQueue<T>::Node> ThreadsafeQueue<T>::try_pop_head(T &value)
  {
    std::lock_guard<std::mutex> head_lock(head_mutex_);

    if (head_.get() == get_tail()) return std::unique_ptr<Node>();

    value = std::move(*head_->data_);
    return pop_head();
  }

  template <class T>
  typename ThreadsafeQueue<T>::Node *ThreadsafeQueue<T>::get_tail()
  {
    std::lock_guard<std::mutex> tail_lock(tail_mutex_);
    return tail_;
  }

}

#endif // THREADSAFEQUEUE_H
