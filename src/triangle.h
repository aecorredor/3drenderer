#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <stdint.h>

#include "vector.h"

typedef struct {
  int a;
  int b;
  int c;
} face_t;

typedef struct {
  vec2_t points[3];
} triangle_t;

void float_swap(float *a, float *b);

void fill_flat_bot_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);

void fill_flat_top_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);

void draw_filled_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);

#endif
