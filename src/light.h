#ifndef LIGHT_H
#define LIGHT_HMATRIX_H

#include "vector.h"
#include <stdint.h>

typedef struct {
  vec3_t direction;
} light;

extern light global_light;

uint32_t apply_light_intensity_factor(uint32_t color, float factor);

#endif