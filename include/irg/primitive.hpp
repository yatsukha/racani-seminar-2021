#pragma once

#include <iostream> // TODO: delet
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace irg {

  using unit = float;

  struct color {
    unit r;
    unit g;
    unit b;
  };

  namespace colors {
    extern color red, green, blue, white;
  }

  struct point {
    unit x;
    unit y;

    friend ::std::ostream& operator<<(::std::ostream& out, point const& p) {
      out << "(" << p.x << ", " << p.y << ")";
      return out;
    }

    friend void swap(point& a, point& b) noexcept {
      using ::std::swap;
      swap(a.x, b.x);
      swap(a.y, b.y);
    }

    friend bool operator==(point const& a, point const& b) noexcept {
      return a.x == b.x && a.y == b.y;
    }
  };

  inline point normalize(point const& p, point const& dim) noexcept {
    return {p.x / dim.x * 2.0f - 1.0f, -p.y / dim.y * 2.0f + 1.0f};
  }

  inline point scale(point const& p, point const& dim) noexcept {
    return {(p.x + 1.0f) / 2.0f * dim.x, (p.y - 1.0f) / 2.0f * dim.y * -1.0f};
  }

  template<typename Point>
  inline ::glm::vec3 homogenous(Point&& p) noexcept {
    return ::glm::vec3{p.x, p.y, 1.0f};
  }

  template<typename Vec3>
  inline point non_homogenous(Vec3&& v) noexcept {
    return point{v.x, v.y};
  }

  struct line_segment {
    point start;
    point end;
  };

}