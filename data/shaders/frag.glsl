#version 330 core

precision highp float;

uniform vec3 resolution;
uniform vec3 camera_position;
uniform vec3 camera_target;

uniform int iterations;
uniform float power;
uniform float min_distance;
uniform int max_steps;

float mandelbulb_de(vec3 pos) {
  const float Bailout = 256.0;
  vec3 z = pos;
  float dr = 1.0;
  float r = 0.0;
  for (int i = 0; i < iterations ; i++) {
    r = length(z);

    if (r > Bailout) break;
    
    float theta = acos(z.z/r);
    float phi = atan(z.y,z.x);
    dr = pow(r, power - 1.0) * power * dr + 1.0;
    
    float zr = pow(r, power);
    theta = theta * power;
    phi = phi * power;
    
    z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
    //z = zr * vec3(cos(theta) * cos(phi), cos(theta) * sin(phi), sin(theta));
    z += pos;
  }
  return 0.5 * log(r) * r / dr;
}

float sierpinski_de(vec3 z) {
  const vec3 Offset = vec3(1, 1, 1); 
  const float Scale = 2.0;
  float r;
  int n = 0;
  while (n < iterations) {
    if (z.x + z.y < 0) z.xy = -z.yx; // fold 1
    if (z.x + z.z < 0) z.xz = -z.zx; // fold 2
    if (z.y + z.z < 0) z.zy = -z.yz; // fold 3	
    z = z * Scale - Offset * (Scale - 1.0);
    n++;
  }
  return length(z) * pow(Scale, -float(n));
}

float distance_from_sphere(in vec3 p, in vec3 c, float r) {
  return max(0.0, length(p - c) - r);
}

float balls_de(in vec3 p) {
  const vec3 c = vec3(5.0, 2.0, 2.0);
  return 
    distance_from_sphere(mod(p + 0.5 * c, c) - 0.5 * c, vec3(0.0), 0.5);
}

struct march_result {
  vec3 position;
  int steps;
  float distance;
};

// ray origin => the starting point
// ray direction => direction of the ray
march_result ray_march(in vec3 ro, in vec3 rd) {
  float distance_traveled = 0.0;

  const float MAXIMUM_TRACE_DISTANCE = 100.0;

  for (int i = 0; i < max_steps; ++i) {
    vec3 current_position = ro + distance_traveled * rd;
    
    vec4 color;
    float closest = mandelbulb_de(current_position);
    if (closest < min_distance) {
      return march_result(current_position, i + 1, distance_traveled);
    }

    distance_traveled += closest;
    if (distance_traveled > MAXIMUM_TRACE_DISTANCE) {
      break;
    }
  }

  return march_result(
    ro + distance_traveled * rd,
    max_steps, 
    -1.0
  );
}

vec3 rotateAxis(vec3 p, vec3 axis, float angle) {
  return mix(dot(axis, p) * axis, p, cos(angle)) 
    + cross(axis, p) * sin(angle);
}

void main() {
  vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - vec2(1.0, 1.0);
  uv.x *= float(resolution.x) / resolution.y; // aspect ratio
  
  const vec3 up = vec3(0.0, 1.0, 0.0);

  float angle = acos(dot(up.xy, uv) / (length(up.xy) * length(uv)));
  
  if (uv.x < 0) {
    angle *= -1;
  }

  vec3 cam_vec = camera_target - camera_position;

  vec3 rd = normalize(cam_vec) 
    + rotateAxis(up, normalize(cam_vec), angle) * length(uv);
  rd *= 0.5;
  march_result mr = ray_march(camera_position, rd);

  if (mr.distance > 0.0) {
    float ratio = min(1.0, 1.2 - float(mr.steps) / max_steps);
    float ratio2 = ratio * ratio;
    gl_FragColor = vec4(ratio, ratio2, 1.0 - ratio2 * ratio, 1.0);
  } else {
    gl_FragColor = vec4(vec3(0.0), 1.0);
  }
}
