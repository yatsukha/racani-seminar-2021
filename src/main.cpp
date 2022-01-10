#include "irg/obmodel.hpp"
#include <iostream>

#include <irg/common.hpp>
#include <irg/mesh.hpp>
#include <irg/shader.hpp>
#include <irg/keyboard.hpp>
#include <irg/window.hpp>
#include <irg/texture.hpp>
#include <irg/camera.hpp>
#include <irg/scene.hpp>

int main() {
  auto  guard  = ::irg::init();
  auto* window = ::irg::create_window(1000, 1000);

  ::irg::bind_events(window);

  ::irg::shader_program shader{
    {"data/shaders/vert.glsl", GL_VERTEX_SHADER},
    {"data/shaders/frag.glsl", GL_FRAGMENT_SHADER},
  };

  ::glm::vec3 camera{0.0f, 0.0f, -5.0f};

  shader.activate();
  shader.set_uniform_vec3("resolution", {1000.f, 1000.f, 0.f});
  shader.set_uniform_vec3("light_position", {2000.f, -5000.f, 300.f});
  shader.set_uniform_vec3("camera", camera);
  float power = 8.0;
  float power_delta = 1.0005;
  shader.set_uniform_float("power", power);

  ::irg::k_events.add_listener([&power_delta](auto key, bool released) {
    if (released) {
      return ::irg::ob::remain;
    }
    auto constexpr static delta = 0.0001;
    if (key == GLFW_KEY_I) {
      power_delta += delta;
      ::std::cout << power_delta << "\n";
    } else if (key == GLFW_KEY_O) {
      power_delta -= delta;
      ::std::cout << power_delta << "\n";
    }
    return ::irg::ob::remain;
  });

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
    shader.activate();
    if (::std::abs(power_delta) > 1e-6) {
      power *= power_delta;
      shader.set_uniform_float("power", power);
      ::std::cout << "p: " << power << "\n";
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ::irg::assert_no_error();
  });

}
