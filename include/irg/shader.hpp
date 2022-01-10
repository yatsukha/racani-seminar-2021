#pragma once

#include <string>
#include <fstream>
#include <streambuf>
#include <cstring>
#include <array>
#include <memory>
#include <csignal>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <irg/common.hpp>
#include <irg/primitive.hpp>
#include <irg/ownership.hpp>

namespace irg {

  class shader {
    shared_ownership<unsigned> _id;

   public:
    unsigned type;

    shader(char const* file, int const type)
      : type(type)
      , _id(deffer_ownership(
          new unsigned{glCreateShader(type)}, 
          [](auto* ptr) {
            glDeleteShader(*ptr);
          }
        ))
    {
      ::std::ifstream f(file);
      if (!f.is_open())
        ::std::cerr << "Error while opening file: ",
        ::irg::terminate(file);

      ::std::string source(
        (::std::istreambuf_iterator<char>(f)),
        ::std::istreambuf_iterator<char>()
      );

      const char* chars = source.c_str();

      glShaderSource(*_id, 1, &chars, nullptr);
      glCompileShader(*_id);

      int success;
      ::std::array<char, 512> log;

      glGetShaderiv(*_id, GL_COMPILE_STATUS, &success);
      if (!success)
        glGetShaderInfoLog(*_id, log.max_size(), nullptr, log.data()),
        ::std::cerr << file << ": " << "\n",
        ::irg::terminate(log.data());
    }

    unsigned id() const noexcept {
      return *_id;
    }
  };

  class shader_program {
    shared_ownership<unsigned> id;

   public:
    shader_program(shader const& vertex, shader const& fragment) 
      : id(
          deffer_ownership(
            new unsigned{glCreateProgram()}, 
            [](auto* ptr) {
              glDeleteProgram(*ptr);
            }
          )
        )
    {
      glAttachShader(*id, vertex.id());
      glAttachShader(*id, fragment.id());

      glLinkProgram(*id);

      int success;
      ::std::array<char, 512> log;

      glGetProgramiv(*id, GL_LINK_STATUS, &success);
      if (!success)
        glGetProgramInfoLog(*id, log.max_size(), nullptr, log.data()),
        ::irg::terminate(log.data());
    }

    shader_program& activate() noexcept {
      glUseProgram(*id);
      return *this;
    }

    shader_program* operator->() noexcept {
      return &activate();
    }
    
    void set_uniform_float(char const* uniform_name, float const f) {
      glUniform1f(glGetUniformLocation(*id, uniform_name), f);
    }
    
    void transform_uniform_float(char const* uniform_name,
                                 ::std::function<float(float)> transform) {
      auto location = glGetUniformLocation(*id, uniform_name);
      float val;
      glGetUniformfv(*id, location, &val);
      glUniform1f(location, transform(val));
    }
    

    void set_uniform_int(char const* uniform_name, int const i) {
      glUniform1i(glGetUniformLocation(*id, uniform_name), i);
    }

    void set_uniform_color(char const* uniform_name, color const& c) {
      glUniform3f(glGetUniformLocation(*id, uniform_name), c.r, c.g, c.b);
    }

    void set_uniform_vec3(char const* uniform_name, ::glm::vec3 const &v) {
      glUniform3fv(
        glGetUniformLocation(*id, uniform_name), 
        1, ::glm::value_ptr(v)
      );
    }

    ::glm::mat4 get_uniform_matrix(char const* uniform_name) {
      ::std::unique_ptr<float[]> mat(new float[16]);
      glGetUniformfv(*id, glGetUniformLocation(*id, uniform_name), mat.get());

      return ::glm::make_mat4(mat.get());
    }

    void set_uniform_matrix(char const* uniform_name, ::glm::mat4 const& m) {
      glUniformMatrix4fv(
        glGetUniformLocation(*id, uniform_name), 
        1, GL_FALSE, ::glm::value_ptr(m)
      );
    }

    void transform_matrix(char const* uniform_name, ::glm::mat4 const& m) {
      set_uniform_matrix(uniform_name, m * get_uniform_matrix(uniform_name));
    }
    
  };

}