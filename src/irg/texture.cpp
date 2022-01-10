#include <irg/texture.hpp>

#include <irg/common.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

namespace irg {
  
  namespace detail {
    
    void bind_texture(unsigned char const* data, int w, int h) {
      glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    
  }
  
  void texture::init() {
    id = deffer_ownership(new unsigned{}, [](unsigned* ptr) -> void {
      glDeleteTextures(1, ptr);
    });
    
    glGenTextures(1, id.get());
    glBindTexture(GL_TEXTURE_2D, *id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  
  texture::texture(unsigned char const* data, int w, int h) {
    init();
    
    detail::bind_texture(data, w, h);
  }

  texture::texture(char const* file) {
    init();
    
    int w, h, nc;
    unsigned char* data = stbi_load(file, &w, &h, &nc, 0);
    on_scope_exit data_guard{[&data]{
      stbi_image_free(reinterpret_cast<void*>(data));
    }};

    if (!data)
      terminate("Error while loading texture.");

    detail::bind_texture(data, w, h);
  }

  void texture::use() {
    if (id)
      glBindTexture(GL_TEXTURE_2D, *id);
  }

}