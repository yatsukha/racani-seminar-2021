#version 330 core

uniform vec3 resolution;
uniform vec3 light_position;
uniform float power;

float mandelbub_de(vec3 pos) {
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
  const float Bailout = 100.0;
  const int Iterations = 20;
	for (int i = 0; i < Iterations ; i++) {
		r = length(z);
		if (r>Bailout) break;
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
		dr =  pow( r, power-1.0)*power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,power);
		theta = theta*power;
		phi = phi*power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
    //z = zr*vec3( cos(theta)*cos(phi), cos(theta)*sin(phi), sin(theta));
		z+=pos;
	}
	return 0.5*log(r)*r/dr;
}

float sierpinski_de(vec3 z) {
  const vec3 Offset = vec3(1.5, 1.5, 1.5); 
  const float Scale = 0.2;
  const int Iterations = 8;
  float r;
  int n = 0;
  while (n < Iterations) {
     if(z.x+z.y<0) z.xy = -z.yx; // fold 1
     if(z.x+z.z<0) z.xz = -z.zx; // fold 2
     if(z.y+z.z<0) z.zy = -z.yz; // fold 3	
     z = z*Scale - Offset*(Scale-1.0);
     n++;
  }
  return (length(z) ) * pow(Scale, -float(n));
}

float distance_from_sphere(in vec3 p, in vec3 c, float r) {
  return length(p - c) - r;
}

float distance_from_scene(in vec3 p) {
  return mandelbub_de(p);
  //return distance_from_fractal(p);
  //const vec3 c = vec3(50.0, 50.0, 20.0);
  //return 
  //  distance_from_sphere(mod(p + 0.5 * c, c) - 0.5 * c, vec3(0.0), 5.0);
  //return min(
  //  distance_from_sphere(mod(p + 0.5 * c, c), vec3(0.0), 5.0), 
  //  distance_from_sphere(p, vec3(2.0, 10.0, 10.0), 5.0)
  //);
}

vec3 calculate_normal(in vec3 p) {
  const float small_step = 0.0001;
  vec3 rv = vec3(0.0);

  for (int i = 0; i < 3; ++i) {
    vec3 step = vec3(0.0);
    step[i] = small_step;
    rv[i] = distance_from_scene(p + step) - distance_from_scene(p - step);
  }

  return normalize(rv);
}

struct march_result {
  vec3 position;
  int steps;
  float distance;
};

const int NUMBER_OF_STEPS = 32;

// ray origin => the starting point
// ray direction => direction of the ray
march_result ray_march(in vec3 ro, in vec3 rd) {
  float distance_traveled = 0.0;

  //const float MINIMUM_HIT_DISTANCE = 0.01;
  const float MINIMUM_HIT_DISTANCE = 0.0001;
  const float MAXIMUM_TRACE_DISTANCE = 100.0;

  for (int i = 0; i < NUMBER_OF_STEPS; ++i) {
    vec3 current_position = ro + distance_traveled * rd;

    float closest = distance_from_scene(current_position);
    if (closest < MINIMUM_HIT_DISTANCE) {
      return march_result(current_position, i + 1, distance_traveled);
    }

    distance_traveled += closest;
    if (distance_traveled > MAXIMUM_TRACE_DISTANCE) {
      break;
    }
  }

  return march_result(
    ro + distance_traveled * rd, 
    NUMBER_OF_STEPS, 
    -1.0
  );
}

void main() {
  vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - vec2(1.0, 1.0);
  float ratio = float(resolution.x) / resolution.y;
  uv.x *= ratio;
  vec3 camera_position = vec3(0.0, 0.0, -2.0);
  vec3 ro = camera_position;
  vec3 rd = vec3(uv, 1.0); // 1.0 acts as a fov, sort of

  march_result mr = ray_march(ro, rd);

  if (mr.distance > 0.0) {
    vec3 normal = calculate_normal(mr.position);
    vec3 to_light = normalize(mr.position - light_position);
    float diffuse = max(0.0, dot(normal, to_light));
    //gl_FragColor = vec4(1.0);
    gl_FragColor = vec4(vec3(min(1.0, 1.1 -  float(mr.steps) / NUMBER_OF_STEPS)), 1.0);
  } else {
    gl_FragColor = vec4(vec3(0.0), 1.0);
  }
}
