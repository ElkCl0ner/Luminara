#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <string>

std::string kernel_src_random = R"(
float random(float *seed) {
  *seed = sin(*seed)*43758.5453123;
  *seed = *seed - floor(*seed);
  return *seed;
}
)";

#endif
