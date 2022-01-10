#pragma once

#include <iostream>
#include <optional>
#include <algorithm>
#include <functional>

#include <glm/glm.hpp>

#include <irg/common.hpp>
#include <irg/mouse.hpp>
#include <irg/primitive.hpp>
#include <irg/shader.hpp>
#include <irg/keyboard.hpp>

namespace irg {

  class shape {
    shader_program mutable sp;
    color c = colors::white;

   protected:
    bool locked = false;

   public:
    shape(shader_program const sp): sp(sp) {
      k_events.add_listener([this](int const key, auto) {
        if (!locked)
          c = key == GLFW_KEY_R // oh god what have i done
            ? colors::red
            : key == GLFW_KEY_B
            ? colors::blue
            : key == GLFW_KEY_G
            ? colors::green
            : key == GLFW_KEY_W
            ? colors::white
            : c;
        
        return locked ? ob::action::detach : ob::action::remain;
      });
    }

    virtual void draw() const {
      sp.activate();
      sp.set_uniform_color("u_color", c);
    };

    virtual ~shape() = default;
  };

  class line : public shape {
    unsigned VAO;
    unsigned VBO;

    void update_buffer(line_segment const& ls);

   public:
    line(point const start, shader_program const sp, mouse_events& me);

    line(line const&) = delete;
    line& operator=(line const&) = delete;

    void draw() const override;

    ~line();
  };

  class bresenham_line : public shape {
   private:
    ::GLFWwindow* w;
    line_segment ls;

    line_segment adjust(line_segment ls) noexcept {
      ls.start.y -= 0.02;
      ls.end.y   -= 0.02;
      return ls;
    }

   public:
    bresenham_line(::GLFWwindow* w, point const start, shader_program const sp,
                   mouse_events& me);

    void draw() const override;
  };

  template<int N>
  class polygon : public shape {
    static_assert(N > 2, "Polygon should have atleast 3 vertices");

    ::std::vector<point> vertices;

   public:
    polygon(point const start, shader_program const sp, mouse_events& me)
      : vertices({start, start}), shape(sp) {
      me.add_listener({
        {},
        [this](auto const& ls) {
          vertices.back() = ls.end;
        },
        [this](auto const& ls) {
          vertices.back() = ls.end;

          if (vertices.size() == N) {
            shape::locked = true;
            return ob::action::detach;
          } else {
            vertices.push_back(vertices.back());
            return ob::action::remain;
          }
        }
      });
    }
    
    polygon(::std::vector<point> const& vertices, shader_program const sp)
      : shape(sp), vertices(vertices.begin(), vertices.begin() + N) {}

    void draw() const override {
      shape::draw();

      unsigned VAO, VBO;

      glGenVertexArrays(1, &VAO);
      glBindVertexArray(VAO);
      glGenBuffers(1, &VBO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      on_scope_exit guard([VAO, VBO]{
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
      });

      glBufferData(
        GL_ARRAY_BUFFER, sizeof(point) * vertices.size(),
        reinterpret_cast<float const*>(vertices.data()), GL_STATIC_DRAW);

      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(point), nullptr);
      glEnableVertexAttribArray(0);

      auto sz = vertices.size();
      glDrawArrays(sz < 3 || (vertices[1] == vertices[2]) ? GL_LINES : GL_TRIANGLE_FAN, 0, sz);
    }
  };

  class scanline_polygon : public shape {
    ::GLFWwindow* w;
    ::std::vector<::glm::vec3> vertices;
    bool filled = false;
    float delta_multiplier = 1.0f;

    struct edge {
      point y_cords;
      ::std::function<float(float const)> line;
    };

    ::std::vector<edge> edges[2] {
      ::std::vector<edge>{},
      ::std::vector<edge>{}
    };

    void finalize();

   public:
    scanline_polygon(point const& p, shader_program const sp, mouse_events& me,
                     keyboard_events& ke, ::GLFWwindow* w);

    bool is_final() { return shape::locked; }
    bool is_inside(point const& p);

    void set_filled(bool const filled) { this->filled = filled; }

    void draw() const override;

  };

}