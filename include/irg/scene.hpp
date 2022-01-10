#pragma once

#include <vector>

#include <irg/camera.hpp>
#include <irg/generic_mesh.hpp>

namespace irg {
  
  class scene {
   private:
    ::std::vector<::irg::mesh_concept> objects;
    ::std::vector<bool> displayed;
   public:
    ::irg::camera viewer = {};
    
    double aspect_ratio = 1.0;

    scene() noexcept = default;
    
    ::std::size_t push_back(::irg::mesh_concept&& obj);
    ::irg::mesh_concept& operator[](::std::size_t const idx);
    
    void toggle_visibility(::std::size_t const idx);
    void draw();
    
    auto begin() {
      return objects.begin();
    }
    
    auto end() {
      return objects.end();
    }
  };
  
}
