#pragma once

#include <irg/generic_mesh.hpp>

namespace irg {
  
  // change mesh implementation without breaking change
  using mesh = generic_mesh<vertex_policies::simple_vertex_policy>;

}