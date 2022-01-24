#pragma once

#include <vector>

namespace irg::ob {

  enum action {
    remain,
    detach
  };

  template<typename Listener>
  class observer {
   protected:
    ::std::vector<Listener> listeners;

   public:
    void add_listener(Listener l) {
      listeners.push_back(l);
    }

    virtual ~observer() = default;
  };

}