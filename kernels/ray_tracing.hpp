#ifndef RAY_TRACING_HPP
#define RAY_TRACING_HPP

#include <string>

std::string kernel_src_main = R"(
__kernel void main(__global const int *out) {
  int id = get_global_id(0);
  out[id] = 2;
}
)";

#endif
