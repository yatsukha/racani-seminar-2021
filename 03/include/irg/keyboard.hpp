#pragma once

#include <vector>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <irg/obmodel.hpp>

namespace irg {

  namespace keyboard_event_type {
    using on_press = ::std::function<ob::action(int const, bool)>;
  }

  class keyboard_events : public ob::observer<keyboard_event_type::on_press> {
   public:
    void static callback(GLFWwindow*, int, int, int, int);
  };

  extern keyboard_events k_events;

}
