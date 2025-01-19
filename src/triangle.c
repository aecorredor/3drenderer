#include "triangle.h"
#include "display.h"

void vec2_swap(vec2_t *a, vec2_t *b) {
  vec2_t temp = *a;
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
void fill_flat_bot_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
  float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

  float x_start = x0;
  float x_end = x0;

  // Loop all scanlines from top to bottom
  for (int y = y0; y <= y2; y++) {
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
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
  float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

  float x_start = x2;
  float x_end = x2;

  // Loop all scanlines from bottom to top
  for (int y = y2; y >= y0; y--) {
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
    vec2_swap(&p0, &p1);
  }

  if (p1.y > p2.y) {
    vec2_swap(&p1, &p2);
  }

  if (p0.y > p1.y) {
    vec2_swap(&p0, &p1);
  }

  if (p1.y == p2.y) {
    fill_flat_bot_triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
    return;
  }

  if (p0.y == p1.y) {
    fill_flat_top_triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
    return;
  }

  // Calculate thew new vertex (mx, my) using triangle similarity
  float my = p1.y;
  float mx = (((p2.x - p0.x) * (p1.y - p0.y)) / (p2.y - p0.y)) + p0.x;

  vec2_t m = {.x = mx, .y = my};

  fill_flat_bot_triangle(p0.x, p0.y, p1.x, p1.y, m.x, m.y, color);
  fill_flat_top_triangle(p1.x, p1.y, m.x, m.y, p2.x, p2.y, color);
}
