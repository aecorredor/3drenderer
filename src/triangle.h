#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "texture.h"
#include "vector.h"
#include <stdint.h>

typedef struct {
  int a;
  int b;
  int c;
  text2_t a_uv;
  text2_t b_uv;
  text2_t c_uv;
  uint32_t color;
} face_t;

typedef struct {
  vec2_t points[3];
  text2_t texcoords[3];
  uint32_t color;
  float avg_depth;
} triangle_t;

void draw_filled_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);
void draw_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);
void draw_textured_triangle(vec2_t p0, vec2_t p1, vec2_t p2, text2_t a_uv,
                            text2_t b_uv, text2_t c_uv, uint32_t *texture);

#endif
