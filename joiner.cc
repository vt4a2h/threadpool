#include "joiner.h"

namespace threads {

  Joiner::Joiner(Threads &threads)
    : threads_(threads)
  {
  }

  Joiner::~Joiner()
  {
    JoinAll();
  }

  void Joiner::JoinAll()
  {
    for (auto &thread : threads_)
      if (thread.joinable()) thread.join();
  }

}
