#include <irg/keyboard.hpp>

namespace irg {

  keyboard_events k_events;

  void keyboard_events::callback(GLFWwindow*, int key, int, int action, int) {
    if (action != GLFW_PRESS && action != GLFW_RELEASE)
      return;

    bool released = action == GLFW_RELEASE;

    auto iter = k_events.listeners.begin();
    while (iter != k_events.listeners.end())
      if ((*iter)(key, released) == ob::action::detach)
        iter = k_events.listeners.erase(iter);
      else
        ++iter;
  }

}