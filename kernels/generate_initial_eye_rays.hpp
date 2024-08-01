#ifndef GENERATE_INITIAL_EYE_RAYS_HPP
#define GENERATE_INITIAL_EYE_RAYS_HPP

#include <string>

const std::string kernel_src_generateEyeRayDirections = R"(
__constant float PI = 3.14159265359f;
__constant float3 WORLD_UP = (float3)(0.0f, 1.0f, 0.0f);

__constant int WIDTH = 800;
__constant int HEIGHT = 600;
__constant float FOV = 90.0f;
__constant APERTURE = 0.0f;
__constant DEPTH_OF_FIELD = 1.0f;

__kernel void generateEyeRays(
    __constant float time,
    __global const float3 *cameraPosition,
    __global const float3 *cameraDirection,
    __global float3 *eyeRayOrigins,
    __global float3 *eyeRayDirections
  ) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    // Generate initial seed
    float rand = dot(float2(x, y), float2(12.9898f, 78.233f)) + time;

    //* Generate eye ray origin *//

    // No aperture
    if (APERTURE == 0.0f) {
      eyeRayOrigins[y * WIDTH + x] = cameraPosition[0];
    }

    // With aperture
    else {
      // Generate a random point on a sphere
      random(&rand);
      float theta = rand * 2 * PI;
      random(&rand);
      float phi = rand * PI;
      float3 randomPointOnSphere = float3(
        APERTURE * cos(theta) * sin(phi),
        APERTURE * sin(theta) * sin(phi),
        APERTURE * cos(phi)
      );

      // Project the point onto the camera plane
      float magnitudeNormal = sqrt(cameraDirection[0].x * cameraDirection[0].x + cameraDirection[0].y * cameraDirection[0].y + cameraDirection[0].z * cameraDirection[0].z);
      float distanceToPlane = dot(cameraDirection[0], randomPointOnSphere) / magnitudeNormal;
      float3 thisEyeRayOrigin = randomPointOnSphere - distanceToPlane * cameraDirection[0] / magnitudeNormal;

      // Displace the eye ray direction from (0,0,0) to the camera position
      eyeRayOrigins[y * WIDTH + x] = thisEyeRayOrigin + cameraPosition[0];
    }

    //* Generate eye ray direction *//

    // Calculate ndc vectors
    float y_scale = DEPTH_OF_FIELD * tan(FOV * PI / 2.0f);
    float x_scale = y_scale * WIDTH / HEIGHT;

    float3 cameraRight = normalize(cross(WORLD_UP, cameraDirection[0]));
    float3 cameraUp = normalize(cross(cameraDirection[0], cameraRight));

    float3 x_ndc = cameraRight * x_scale;
    float3 y_ndc = cameraUp * y_scale;

    // Generate focal point
    random(&rand);
    float x_offset = 2 * (x + rand) / WIDTH - 1;
    random(&rand);
    float y_offset = 2 * (y + rand) / HEIGHT - 1;

    float3 eyeRayFocalPoint = cameraPosition[0] + DEPTH_OF_FIELD * cameraDirection[0] + x_offset * x_ndc + y_offset * y_ndc;

    // Calculate eye ray direction
    eyeRayDirections[y * WIDTH + x] = normalize(eyeRayFocusPoint - eyeRayOrigins[y * WIDTH + x]);

}
)";

#endif
