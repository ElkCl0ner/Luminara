#ifndef RAY_TRACING_HPP
#define RAY_TRACING_HPP

#include "random.hpp"
#include "generate_eye_ray.hpp"

#include <string>

std::string kernel_src_main =
kernel_src_random +
kernel_src_generateEyeRay +
R"(
__kernel void rayTracing(
  __constant float *time,
  __constant float3 *cameraPosition,
  __constant float3 *cameraDirection,
  __global float3 *eyeRayOrigins,
  __global float3 *eyeRayDirections
) {
  int x = get_global_id(0);
  int y = get_global_id(1);

  int width = get_global_size(0);
  int height = get_global_size(1);

  float seed = dot((float2)((float)x/width, (float)y/height), (float2)(12.9898,78.233)) + *time;

  generateEyeRays(&seed, cameraPosition, cameraDirection, eyeRayOrigins, eyeRayDirections);
}
)";

#endif
