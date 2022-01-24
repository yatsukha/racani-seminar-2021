#include <irg/window.hpp>

namespace irg {

  window_events w_events;

  void window_events::buffer_size_callback(::GLFWwindow*, int const w, 
                                           int const h) {
    auto iter = w_events.listeners.begin();
    while (iter != w_events.listeners.end())
      if ((*iter)(w, h) == ob::action::detach)
        iter = w_events.listeners.erase(iter);
      else
        ++iter;
  }

}