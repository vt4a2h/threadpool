#include "functionwrapper.h"

namespace threads {

  FunctionWrapper::FunctionWrapper(FunctionWrapper &&other)
    : impl_(std::move(other.impl_))
  {
  }

  FunctionWrapper &FunctionWrapper::operator =(FunctionWrapper &&other)
  {
    impl_ = std::move(other.impl_);
    return *this;
  }

  void FunctionWrapper::operator ()()
  {
    impl_->call();
  }

}
