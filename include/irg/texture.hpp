#pragma once

#include <irg/ownership.hpp>

namespace irg {

  class texture {
    shared_ownership<unsigned> id;
    
    void init();

   public:
    texture() {};
    texture(char const* path);
    texture(unsigned char const* data, int w, int h);
    void use();
  };

}