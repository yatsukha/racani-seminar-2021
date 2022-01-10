#pragma once

#include <iostream>
#include <glm/glm.hpp>

namespace irg {

  template<::glm::length_t S, typename T, ::glm::qualifier P>
  ::std::ostream& operator<<(::std::ostream& out, 
                             ::glm::vec<S, T, P> const& v) {
    for (::glm::length_t i = 0; i < v.length(); ++i) {
      out << v[i];
      if (i != v.length() - 1)
        out << " ";
    }
    return out;
  }

  template<::glm::length_t R, ::glm::length_t C, typename T, ::glm::qualifier P>
  ::std::ostream& operator<<(::std::ostream& out,
                             ::glm::mat<R, C, T, P> const& m) {
    for (::glm::length_t c = 0; c < m.length(); ++c) {
      for (::glm::length_t r = 0; r < m[c].length(); ++r)
        out << m[r][c] << " ";
      out << "\n";
    }
    return out;
  }

  template<typename T, ::glm::qualifier P>
  T vcos(::glm::vec<3, T, P> const& a, ::glm::vec<3, T, P> const& b) noexcept {
    return ::glm::dot(a, b) / (::glm::length(a) * ::glm::length(b));
  }

  template<typename T, ::glm::qualifier P>
  T sproj(::glm::vec<3, T, P> const& a, ::glm::vec<3, T, P> const& b) noexcept {
    return ::glm::dot(a, b) / ::glm::length(b);
  }

  template<typename T, ::glm::qualifier P>
  ::glm::vec<3, T, P> vproj(::glm::vec<3, T, P> const& a, 
                            ::glm::vec<3, T, P> const& b) noexcept {
    return ::glm::normalize(b) * sproj(a, b);
  }

}