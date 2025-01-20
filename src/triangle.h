#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"
#include <stdint.h>

typedef struct {
  int a;
  int b;
  int c;
  uint32_t color;
} face_t;

typedef struct {
  vec2_t points[3];
  uint32_t color;
  float avg_depth;
} triangle_t;

void draw_filled_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);

#endif
