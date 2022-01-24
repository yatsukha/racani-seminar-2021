#pragma once

#include <utility>
#include <iostream>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace irg {

  class on_scope_exit final {
   public:
    using callable = ::std::function<void(void)>;
    on_scope_exit(callable&& c)
      : c(::std::forward<callable>(c)) 
      {}
    ~on_scope_exit() { c(); }
   private:
    callable c;
  };

  void terminate(char const* err);

  void assert_no_error();

  on_scope_exit init(int const major_version = 3, int const minor_version = 3);

  ::GLFWwindow* create_window(int const width = 800, int const height = 600);

  void window_loop(::GLFWwindow* window, ::std::function<void(void)> render);

  void bind_events(::GLFWwindow* window);

}