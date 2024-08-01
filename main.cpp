#include "kernels/ray_tracing.hpp"

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/cl.hpp>

#include <iostream>
#include <vector>

// Return codes
#define EXIT_SUCCESS 0
#define EXIT_OPENCL_SETUP_FAILURE 1
#define EXIT_OPENCL_BUILD_KERNEL_FAILURE 2
#define EXIT_SETUP_SDL2_FAILURE 3

#define WIDTH 4
#define HEIGHT 3

int main(int argc, char** argv) {

  /* Setup OpenCL */

  // Get all platforms (drivers), e.g., NVIDIA, AMD, Intel, etc.
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  // Get a GPU device from the first platform
  std::vector<cl::Device> devices;
  platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
  cl::Device device = devices[0];

  // Create a context and queue
  cl::Context context(device);
  cl::CommandQueue queue(context, device);

  /* Build and launch ray tracing kernel */

  // Create and build the program
  cl::Program program(context, kernel_src_main);
  program.build({device});

  // Input data
  float time = 12.34f;
  cl_float3 cameraPosition = {0.0f, 0.0f, 0.0f};
  cl_float3 cameraDirection = {0.0f, 0.0f, 1.0f};

  // Create buffers
  cl::Buffer bufferTime(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float), &time);
  cl::Buffer bufferCameraPosition(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float3), &cameraPosition);
  cl::Buffer bufferCameraDirection(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float3), &cameraDirection);
  cl::Buffer bufferEyeRayOrigins(context, CL_MEM_READ_WRITE, sizeof(cl_float3) * WIDTH * HEIGHT);
  cl::Buffer bufferEyeRayDirections(context, CL_MEM_READ_WRITE, sizeof(cl_float3) * WIDTH * HEIGHT);

  // Create the kernel and set the arguments
  cl::Kernel kernel(program, "rayTracing");
  kernel.setArg(0, bufferTime);
  kernel.setArg(1, bufferCameraPosition);
  kernel.setArg(2, bufferCameraDirection);
  kernel.setArg(3, bufferEyeRayOrigins);
  kernel.setArg(4, bufferEyeRayDirections);

  // Execute the kernel
  cl::NDRange globalSize(WIDTH, HEIGHT);
  queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);

  // Output data
  cl_float3 eyeRayOrigins[WIDTH * HEIGHT];
  cl_float3 eyeRayDirections[WIDTH * HEIGHT];

  // Read the result back to host memory
  queue.finish();
  queue.enqueueReadBuffer(bufferEyeRayOrigins, CL_TRUE, 0, sizeof(cl_float3) * WIDTH * HEIGHT, eyeRayOrigins);
  queue.enqueueReadBuffer(bufferEyeRayDirections, CL_TRUE, 0, sizeof(cl_float3) * WIDTH * HEIGHT, eyeRayDirections);

  // Output the results
  for (int i = 0; i < HEIGHT * WIDTH; i++) {
    std::cout << eyeRayOrigins[i].x << " " << eyeRayOrigins[i].y << " " << eyeRayOrigins[i].z << " | "
    << eyeRayDirections[i].x << " " << eyeRayDirections[i].y << " " << eyeRayDirections[i].z << std::endl;
  }

  return EXIT_SUCCESS;
}
