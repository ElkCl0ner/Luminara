#ifndef GENERATE_EYE_RAY_HPP
#define GENERATE_EYE_RAY_HPP

#include <string>

const std::string kernel_src_generateEyeRay = R"(
__constant float PI = 3.14159265359f;
__constant float3 WORLD_UP = (float3)(0.0f, 1.0f, 0.0f);

__constant float FOV = 90.0f;
__constant float APERTURE = 0.5f;
__constant float DEPTH_OF_FIELD = 1.0f;

void generateEyeRays(
  __private float *seed,
  __constant float3 *cameraPosition,
  __constant float3 *cameraDirection,
  __global float3 *eyeRayOrigins,
  __global float3 *eyeRayDirections
) {
  int x = get_global_id(0);
  int y = get_global_id(1);

  int width = get_global_size(0);
  int height = get_global_size(1);

  // Calculate camera right and up vectors
  float3 cameraRight = normalize(cross(WORLD_UP, cameraDirection[0]));
  float3 cameraUp = normalize(cross(cameraDirection[0], cameraRight));

  //* Generate eye ray origin *//

  // No aperture
  if (APERTURE == 0.0f) {
    eyeRayOrigins[y * width + x] = cameraPosition[0];
  }

  // With aperture
  else {
    float theta = 2 * PI * random(seed);
    float r = APERTURE * sqrt(random(seed));
    float3 aperturePoint = (float3)(r * cos(theta), 0.0f, r * sin(theta)); // generated a point on the disk with normal (0, 1, 0)

    // No need to transform the aperture point (cameraDirection is (0, 1, 0))
    if (all(cameraDirection[0] == WORLD_UP)) {
      eyeRayOrigins[y * width + x] = cameraPosition[0] + aperturePoint;

    // Transform the aperture point
    } else {
      eyeRayOrigins[y * width + x] = cameraPosition[0] + aperturePoint.x * cameraRight + aperturePoint.z * cameraUp;
    }
  }

  //* Generate eye ray direction *//

  // Calculate ndc vectors
  float y_scale = DEPTH_OF_FIELD * tan(FOV * PI / 360.0f);
  float x_scale = y_scale * width / height;

  float3 x_ndc = cameraRight * x_scale;
  float3 y_ndc = cameraUp * y_scale;

  // Generate focal point
  float x_offset = 2 * (x + random(seed)) / width - 1;
  float y_offset = 2 * (y + random(seed)) / height - 1;

  float3 eyeRayFocalPoint = cameraPosition[0] + DEPTH_OF_FIELD * cameraDirection[0] + x_offset * x_ndc + y_offset * y_ndc;

  // Calculate eye ray direction
  eyeRayDirections[y * width + x] = normalize(eyeRayFocalPoint - eyeRayOrigins[y * width + x]);

}
)";

#endif
