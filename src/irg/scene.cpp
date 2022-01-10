#include <irg/scene.hpp>

#include <cassert>

namespace irg {
  
  ::std::size_t scene::push_back(::irg::mesh_concept&& obj) {
    objects.emplace_back(::std::forward<::irg::mesh_concept>(obj));
    displayed.push_back(true);
    return objects.size() - 1;
  }

  ::irg::mesh_concept& scene::operator[](::std::size_t const idx) {
    return objects[idx];
  }
  
  void scene::toggle_visibility(::std::size_t const idx) {
    displayed[idx] = !displayed[idx];
  }
  
  void scene::draw() {
    auto view_matrix = viewer.view_matrix();
    auto proj_matrix =
      ::glm::perspective(::glm::radians(45.0), aspect_ratio, 0.1, 100.0);

    // no view matrix
    
    for (::std::size_t i = 0; i < objects.size(); ++i) {
      if (displayed[i]) {
        auto s = objects[i].shader();
        
        s->set_uniform_matrix("view", view_matrix);
        s->set_uniform_matrix("projection", proj_matrix);
        
        objects[i].draw();
      }
    }
  }
  
}
