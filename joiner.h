#ifndef JOINER_H
#define JOINER_H

#include <vector>
#include <thread>

namespace threads {

  typedef std::vector<std::thread> Threads;

  class Joiner
  {
    public:
      Joiner(Threads &threads);
      ~Joiner();

    private:
      Threads &threads_;
  };

}

#endif // JOINER_H
