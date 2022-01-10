#include <irg/mouse.hpp>

#include <type_traits>

namespace irg {

  template<typename Callable, typename Arg>
  auto safe_invoke(Callable const& c, Arg const& a) {
    if (c)
      return c(a);

    if constexpr (!::std::is_same_v<void, decltype(c(::std::declval<Arg>()))>)
      return decltype(c(::std::declval<Arg>())){};
  }

  simple_mouse_events sm_events;

  void simple_mouse_events::move_callback(GLFWwindow* w, double xpos, 
                                          double ypos) {
    if (sm_events.listeners.empty())
      return;

    int width, height;
    ::glfwGetWindowSize(w, &width, &height);

    auto point = normalize(
      {static_cast<float>(xpos), static_cast<float>(ypos)}, 
      {static_cast<float>(width), static_cast<float>(height)});

    auto iter = sm_events.listeners.begin();

    while (iter != sm_events.listeners.end())
      if (safe_invoke(iter->moved, point) == ob::action::detach)
        iter = sm_events.listeners.erase(iter);
      else
        ++iter;
  }

  void simple_mouse_events::click_callback(GLFWwindow* w, int btn, int type, 
                                            int) {
    if (btn != GLFW_MOUSE_BUTTON_LEFT || type != GLFW_PRESS 
        || sm_events.listeners.empty())
      return;

    double xpos, ypos;
    ::glfwGetCursorPos(w, &xpos, &ypos);
    
    int width, height;
    ::glfwGetWindowSize(w, &width, &height);

    auto point = normalize(
      {static_cast<float>(xpos), static_cast<float>(ypos)}, 
      {static_cast<float>(width), static_cast<float>(height)});

    auto iter = sm_events.listeners.begin();

    while (iter != sm_events.listeners.end())
      if (safe_invoke(iter->clicked, point) == ob::action::detach)
        iter = sm_events.listeners.erase(iter);
      else
        ++iter;
  }

  // TODO: rewrite to use simple mouse events
  mouse_events m_events;

  void mouse_events::move_callback(GLFWwindow* w, double xpos, double ypos) {
    if (!m_events.tracking) 
      return;

    int width, height;
    ::glfwGetWindowSize(w, &width, &height);

    m_events.ls.end = normalize(
      {static_cast<float>(xpos), static_cast<float>(ypos)}, 
      {static_cast<float>(width), static_cast<float>(height)});

    for (auto& listener : m_events.listeners)
      safe_invoke(listener.moved, m_events.ls);
  }

  void mouse_events::click_callback(GLFWwindow* w, int btn, int type, int) {
    if (btn != GLFW_MOUSE_BUTTON_LEFT || type != GLFW_PRESS)
      return;

    double xpos, ypos;
    ::glfwGetCursorPos(w, &xpos, &ypos);
    
    int width, height;
    ::glfwGetWindowSize(w, &width, &height);

    point p = normalize(
      {static_cast<float>(xpos), static_cast<float>(ypos)}, 
      {static_cast<float>(width), static_cast<float>(height)});

    if (!m_events.tracking) {
      m_events.ls.start = p;

      for (auto& listener : m_events.listeners)
        safe_invoke(listener.started, p);

      m_events.tracking = true;
    } else {
      m_events.ls.end = p;

      bool detached = false;
      auto iter = m_events.listeners.begin();
      while (iter != m_events.listeners.end())
        if (safe_invoke(iter->finalized, m_events.ls) == ob::action::detach)
          detached = true,
          iter = m_events.listeners.erase(iter);
        else
          ++iter;

      if (!detached) {
        m_events.ls.start = m_events.ls.end;
      } else {
        m_events.tracking = false;
      }
    }
  }

}