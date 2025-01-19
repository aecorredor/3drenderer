#include "triangle.h"
#include "display.h"

void float_swap(float *a, float *b) {
  float temp = *a;
  *a = *b;
  *b = temp;
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bot_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(p1.x - p0.x) / (p1.y - p0.y);
  float inv_slope_2 = (float)(p2.x - p0.x) / (p2.y - p0.y);

  float x_start = p0.x;
  float x_end = p0.x;

  // Loop all scanlines from top to bottom
  for (int y = p0.y; y <= p2.y; y++) {
    vec2_t start = {.x = x_start, .y = y};
    vec2_t end = {.x = x_end, .y = y};
    draw_line(start, end, color);

    x_start += inv_slope_1;
    x_end += inv_slope_2;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(p2.x - p0.x) / (p2.y - p0.y);
  float inv_slope_2 = (float)(p2.x - p1.x) / (p2.y - p1.y);

  float x_start = p2.x;
  float x_end = p2.x;

  // Loop all scanlines from top to bottom
  for (int y = p2.y; y >= p0.y; y--) {
    vec2_t start = {.x = x_start, .y = y};
    vec2_t end = {.x = x_end, .y = y};
    draw_line(start, end, color);

    x_start -= inv_slope_1;
    x_end -= inv_slope_2;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(Mx,My)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color) {
  // We need to sort vertices by ascending y-coordinate (y0 < y1 < y2)
  if (p0.y > p1.y) {
    float_swap(&p0.y, &p1.y);
    float_swap(&p0.x, &p1.x);
  }

  if (p1.y > p2.y) {
    float_swap(&p1.y, &p2.y);
    float_swap(&p1.x, &p2.x);
  }

  if (p0.y > p1.y) {
    float_swap(&p0.y, &p1.y);
    float_swap(&p0.x, &p1.x);
  }

  if (p1.y == p2.y) {
    fill_flat_bot_triangle(p0, p1, p2, color);
  } else if (p0.y == p1.y) {
    fill_flat_top_triangle(p0, p1, p2, color);
  } else {
    // Calculate thew new vertex (mx, my) using triangle similarity
    float my = p1.y;
    float mx = (((p2.x - p0.x) * (p1.y - p0.y)) / (p2.y - p0.y)) + p0.x;

    vec2_t m = {.x = mx, .y = my};

    fill_flat_bot_triangle(p0, p1, m, color);
    fill_flat_top_triangle(p1, m, p2, color);
  }
}
