#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <string>

//*seed = fract(sin(seed)*43758.5453123);
std::string kernel_src_random = R"(
__kernel void random(__private float *seed) {
  *seed = 0.90f;
}
)";

#endif
