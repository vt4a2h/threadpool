#ifndef FUNCTIONWRAPPER_H
#define FUNCTIONWRAPPER_H

#include <utility>
#include <memory>

namespace threads {

  class FunctionWrapper
  {
    public:
      FunctionWrapper() = default;

      template <class F>
      FunctionWrapper(F &&f);
      FunctionWrapper(FunctionWrapper &&other);
      FunctionWrapper &operator =(FunctionWrapper &&other);

      void operator ()();

      FunctionWrapper(const FunctionWrapper &) = delete;
      FunctionWrapper(FunctionWrapper &) = delete;
      FunctionWrapper &operator =(const FunctionWrapper &) = delete;

    private:
      class BaseImpl
      {
        public:
          virtual void call() = 0;
          virtual ~BaseImpl() {}
      };

      template <class Function>
      class TypeImpl : public BaseImpl
      {
        public:
          TypeImpl(Function &&f) : f_(std::move(f)) {}
          void call() override { f_(); }

        private:
          Function f_;
      };

      std::unique_ptr<BaseImpl> impl_;
  };

  template <class F>
  FunctionWrapper::FunctionWrapper(F &&f)
    : impl_(new TypeImpl<F>(std::move(f)))
  {
  }

}

#endif // FUNCTIONWRAPPER_H
