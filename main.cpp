#include "kernels/generate_initial_eye_rays.hpp"
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

  /* Build and launch kernel: generate initial eye rays */

  // Create and build the program
  cl::Program program(context, kernelSource);
  program.build({device});

  // Input data
  std::vector<float> a = {0.123f, 0.456f, 0.789f, 0.101f};
  std::vector<float> b(4, 0.0f);

  // Create buffers
  cl::Buffer bufferA(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * a.size(), a.data());
  cl::Buffer bufferB(context, CL_MEM_WRITE_ONLY, sizeof(float) * b.size());

  // Create the kernel and set the arguments
  cl::Kernel kernel(program, "test");
  kernel.setArg(0, bufferA);
  kernel.setArg(1, bufferB);

  // Execute the kernel
  cl::NDRange globalSize(a.size());
  queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);

  // Read the result back to host memory
  queue.finish();
  queue.enqueueReadBuffer(bufferA, CL_TRUE, 0, sizeof(float) * b.size(), b.data());

  // Output the results
  for (size_t i = 0; i < b.size(); ++i) {
    std::cout << "b[" << i << "] = " << b[i] << std::endl;
  }

  return EXIT_SUCCESS;
}
