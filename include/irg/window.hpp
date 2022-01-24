#pragma once

#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <irg/obmodel.hpp>

namespace irg {

  namespace window_event_types {
    using on_size_change = ::std::function<ob::action(int const, int const)>;
  }

  class window_events : public ob::observer<window_event_types::on_size_change>
  {
   public:
    static void buffer_size_callback(::GLFWwindow*, int const w, int const h);   
  };

  window_events extern w_events;

}