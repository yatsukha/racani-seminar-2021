#include <cstdio>
#include <iostream>

#include <irg/common.hpp>
#include <irg/shader.hpp>
#include <irg/keyboard.hpp>
#include <irg/window.hpp>
#include <irg/camera.hpp>

int main(int const argc, char const* const* argv) {
  if (argc != 2) {
    ::irg::terminate(
      "Expected one command line argument: <fragment shader path>\n"
      "See 'data/shaders' folder of this repository.");
  }

  auto const initial_width = 400;
  auto const initial_height = 400;
  auto  guard  = ::irg::init();
  auto* window = ::irg::create_window(initial_width, initial_height);

  ::irg::bind_events(window);

  ::irg::shader_program shader{
    {"#version 330 core\n"
     "layout (location = 0) in vec2 pos;\n"
     "void main(){ gl_Position = vec4(pos, 0.0, 1.0); }", 
     GL_VERTEX_SHADER},
    ::irg::shader::from_file(argv[1], GL_FRAGMENT_SHADER)
  };

  ::irg::camera camera{{0, 0, -2}, {0, 0, 0}};
  ::irg::k_events.add_listener(::irg::standard_camera_controler(camera));

  shader.activate();
  shader.set_uniform_vec3("resolution", {
    static_cast<float>(initial_width), 
    static_cast<float>(initial_height),
    0.f,
  });

  int iterations = 8;
  int max_steps = 64;
  float min_distance = 0.001;
  shader.set_uniform_int("iterations", iterations);
  shader.set_uniform_int("max_steps", max_steps);
  shader.set_uniform_float("min_distance", min_distance);

  auto const update_camera = [&camera, &shader]{
    camera.update();
    shader.set_uniform_vec3("camera_position", camera.position);
    shader.set_uniform_vec3("camera_target", camera.target);
  };

  update_camera();

  


  float power = 4.0;
  float power_delta = 1.0005;
  shader.set_uniform_float("power", power);

  ::irg::k_events.add_listener([&](auto key, bool released) {
    if (released) {
      return ::irg::ob::remain;
    }
    auto constexpr static delta = 0.0001;
    if (key == GLFW_KEY_0) {
      power_delta = 1.0;
      ::std::cout << "power_delta: " << power_delta << "\n";
    } else if (key == GLFW_KEY_1) {
      power_delta += delta;
      ::std::cout << "power_delta: " << power_delta << "\n";
    } else if (key == GLFW_KEY_2) {
      power_delta -= delta;
      ::std::cout << "power_delta: " << power_delta << "\n";
    } else if (key == GLFW_KEY_3) {
      shader.set_uniform_int("iterations", ++iterations);
      ::std::cout << "iterations: " << iterations << "\n";
    } else if (key == GLFW_KEY_4) {
      shader.set_uniform_int("iterations", --iterations);
      ::std::cout << "iterations: " << iterations << "\n";
    } else if (key == GLFW_KEY_5) {
      shader.set_uniform_int("max_steps", max_steps *= 2);
      ::std::cout << "max steps: " << max_steps << "\n";
    } else if (key == GLFW_KEY_6) {
      max_steps /= 2;
      if (!max_steps) max_steps = 1;
      shader.set_uniform_int("max_steps", max_steps);
      ::std::cout << "max steps: " << max_steps << "\n";
    } else if (key == GLFW_KEY_7) {
      shader.set_uniform_float("min_distance", min_distance *= 10.0);
      ::std::cout << "min_distance: " << min_distance << "\n";
    } else if (key == GLFW_KEY_8) {
      shader.set_uniform_float("min_distance", min_distance /= 10.0);
      ::std::cout << "min_distance: " << min_distance << "\n";
    }
    return ::irg::ob::remain;
  });

  ::std::cout 
    << "3D fractals with Ray Marching by https://github.com/yatsukha/" << "\n\n"
    << "Use WASD to rotate camera around target, IO to zoom in/out." << "\n"
    << "Use arrow keys to move the camera target, JK to zoom in/out." << "\n"
    << "0 to reset power increase/decrease for Mandelbulb." << "\n"
    << "1/2 to increase/decrease power change for Mandelbulb." << "\n"
    << "3/4 to increase/decrease iteration count for fractals." << "\n"
    << "5/6 to increase/decrease the max number of ray march steps." << "\n"
    << "7/8 to increase/decrease minimum distance required for a hit."
    << ::std::endl;
    

  ::irg::w_events.add_listener([&shader](auto const w, auto const h) {
    shader.activate();
    shader.set_uniform_vec3(
      "resolution",
      {static_cast<float>(w), static_cast<float>(h), 0.f}
    );
    return ::irg::ob::remain;
  });

  ::std::vector<float> fullscreen_quad{
     1.0f,  1.0f,
     1.0f, -1.0f,
    -1.0f, -1.0f,
    -1.0f,  1.0f,
  };

  ::std::vector<unsigned> indices{
    0, 1, 3,
    1, 2, 3,
  };

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(
    GL_ARRAY_BUFFER, 
    sizeof(fullscreen_quad[0]) * fullscreen_quad.size(),
    fullscreen_quad.data(),
    GL_STATIC_DRAW
  );
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    sizeof(indices[0]) * indices.size(),
    indices.data(),
    GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  glEnable(GL_DEPTH_TEST);

  ::irg::window_loop(window, [&]{
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    update_camera();
    if (::std::abs(power_delta - 1.0) > 1e-6) {
      power *= power_delta;
      shader.set_uniform_float("power", power);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ::irg::assert_no_error();
  });

}
