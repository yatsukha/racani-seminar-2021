#pragma once

#include <vector>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <irg/keyboard.hpp>

namespace irg {

  class camera {
   public:
    ::glm::vec3 position = {0.0, 0.0, 2.0};
    ::glm::vec3 target   = {0.0, 0.0, 0.0};

    ::glm::vec2 sensitivity = {1.0, 0.5};

    ::glm::vec2 position_mask = {0.0, 0.0};
    ::glm::vec2 target_mask   = {0.0, 0.0};

    ::glm::vec2 zoom_sensitivity = {0.02, 0.02};
    ::glm::vec2 zoom_mask        = {0.0, 0.0};

    camera() = default;
    camera(::glm::vec3 const& position, ::glm::vec3 const& target)
      : position(position), target(target) {}
    
    ::glm::mat4 view_matrix() noexcept;
    void update() noexcept;
  };

  ::irg::keyboard_event_type::on_press standard_camera_controler(camera& c);

  namespace bezier {

    using control_points = ::std::vector<::glm::vec3>;
    using bezier_curve   = ::std::function<::glm::vec3(float)>;

    bezier_curve compute_from(control_points const cp);

  }

}
