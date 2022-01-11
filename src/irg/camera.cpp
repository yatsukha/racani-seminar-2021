#include <irg/camera.hpp>

namespace irg {

  void camera::update() noexcept {
    auto direction = ::glm::vec4{position - target, 1.0};
    auto rotate_around = 
    [&direction](auto& object, auto&& angle, auto&& mask) {
      object +=
        ::glm::vec3(
            ::glm::angleAxis(
              ::glm::radians(angle), 
              ::glm::vec3{mask, 1.0} * ::glm::vec3{-1.0, 1.0, 0.0}
            ) 
            * direction - direction
        );
    };

    auto zoom =
    [&direction](auto& object, auto&& sensitivity, auto&& mask) {
      object += mask * sensitivity * ::glm::vec3(direction);
    };

    auto y_inversed = [](auto&& obj) {
      return ::glm::vec2{obj[0], -obj[1]};
    };

    rotate_around(position, sensitivity[0], y_inversed(position_mask));
    rotate_around(target, sensitivity[1], y_inversed(target_mask));

    zoom(position, zoom_sensitivity[0], -zoom_mask[0]);
    zoom(target, zoom_sensitivity[1], zoom_mask[1]);
  }

  ::glm::mat4 camera::view_matrix() noexcept {
    this->update();
    return ::glm::lookAt(position, target, {0.0, 1.0, 0.0}); 
  }
  
  ::irg::keyboard_event_type::on_press standard_camera_controler(camera& camera) {
    return [&camera](auto&& key, auto&& released){
      auto adjust = [&](auto&& n) { return released ? -n : n; };
        
      if (key == GLFW_KEY_A)
        camera.position_mask[1] += adjust(1.0);
      else if (key == GLFW_KEY_D)
        camera.position_mask[1] += adjust(-1.0);
      else if (key == GLFW_KEY_W)
        camera.position_mask[0] += adjust(1.0);
      else if (key == GLFW_KEY_S)
        camera.position_mask[0] += adjust(-1.0);
      else if (key == GLFW_KEY_I)
        camera.zoom_mask[0] += adjust(1.0);
      else if (key == GLFW_KEY_O)
        camera.zoom_mask[0] += adjust(-1.0);
      else if (key == GLFW_KEY_LEFT)
        camera.target_mask[1] += adjust(1.0);
      else if (key == GLFW_KEY_RIGHT)
        camera.target_mask[1] += adjust(-1.0);
      else if (key == GLFW_KEY_UP)
        camera.target_mask[0] += adjust(1.0);
      else if (key == GLFW_KEY_DOWN)
        camera.target_mask[0] += adjust(-1.0);
      else if (key == GLFW_KEY_J)
        camera.zoom_mask[1] += adjust(1.0);
      else if (key == GLFW_KEY_K)
        camera.zoom_mask[1] += adjust(-1.0);
      
      return ob::action::remain;
    };
  }

  namespace bezier {

    bezier_curve compute_from(control_points const cp) {
      return [cp = ::std::move(cp)](float t) -> ::glm::vec3 {
        ::glm::vec3 ret{0.0, 0.0, 0.0};

        auto const sz  = cp.size();
        auto const szf = ::std::tgamma(sz);

        for (::std::size_t i = 0; i < sz; ++i)
          ret += 
            static_cast<float>(
              (szf / (::std::tgamma(i + 1) * ::std::tgamma(sz - i)))
                * ::std::pow(t, i) * ::std::pow(1.0f - t, sz - i - 1)) 
            * cp[i];

        return ret;
      };
    }

  }

}
