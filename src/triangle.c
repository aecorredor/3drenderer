#include "triangle.h"
#include "display.h"
#include "texture.h"
#include <math.h>

void vec2_swap(vec2_t *a, vec2_t *b) {
  vec2_t temp = *a;
  *a = *b;
  *b = temp;
}

void vec4_swap(vec4_t *a, vec4_t *b) {
  vec4_t temp = *a;
  *a = *b;
  *b = temp;
}

void text2_swap(text2_t *a, text2_t *b) {
  text2_t temp = *a;
  *a = *b;
  *b = temp;
}

void int_swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

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

// TODO: this method is not working for the texture part for some reason.
// Maybe try it together with the top triangle method.
void fill_flat_bot_triangle_by_pixel(int x0, int y0, int x1, int y1, int x2,
                                     int y2) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
  float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

  float x_start = x0;
  float x_end = x0;

  // Loop all scanlines from top to bottom, then draw pixel
  // by pixel through the X axis.
  for (int y = y0; y <= y2; y++) {

    if (x_end < x_start) {
      float_swap(&x_end, &x_start);
    }

    for (int x = x_start; x < x_end; x++) {
      draw_pixel(x, y, 0xFFFF00FF);
    }

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
// Return the barycentric weights alpha, beta, and gamma for a point P
///////////////////////////////////////////////////////////////////////////////
//
//              B
//            / | \
//           /  |  \
//          /  (p)  \
//         /  /   \  \
//        / /      \  \
//       A------------C
///////////////////////////////////////////////////////////////////////////////
vec3_t barycentric_coordinates(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
  vec2_t ac = vec2_sub(c, a);
  vec2_t ab = vec2_sub(b, a);
  vec2_t ap = vec2_sub(p, a);
  vec2_t pc = vec2_sub(c, p);
  vec2_t pb = vec2_sub(b, p);

  float parallelogram_abc_area = vec2_cross(ac, ab);
  float alpha = vec2_cross(pc, pb) / parallelogram_abc_area;
  float beta = vec2_cross(ac, ap) / parallelogram_abc_area;
  float gamma = 1 - alpha - beta;

  return (vec3_t){alpha, beta, gamma};
}

void draw_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color) {
  draw_line(p0, p1, color);
  draw_line(p1, p2, color);
  draw_line(p2, p0, color);
}

void draw_texel(int x, int y, uint32_t *texture, vec4_t point_a, vec4_t point_b,
                vec4_t point_c, text2_t uv_a, text2_t uv_b, text2_t uv_c) {
  vec2_t p = {.x = x, .y = y};
  vec2_t p0_2d = vec2_from_vec4(point_a);
  vec2_t p1_2d = vec2_from_vec4(point_b);
  vec2_t p2_2d = vec2_from_vec4(point_c);
  vec3_t weights = barycentric_coordinates(p0_2d, p1_2d, p2_2d, p);

  float alpha = weights.x;
  float beta = weights.y;
  float gamma = weights.z;

  // Interpolated values for reciprocal of w to get linear interpolation for
  // perspective-correction.
  float interpolated_u = alpha * (uv_a.u / point_a.w) +
                         beta * (uv_b.u / point_b.w) +
                         gamma * (uv_c.u / point_c.w);
  float interpolated_v = alpha * (uv_a.v / point_a.w) +
                         beta * (uv_b.v / point_b.w) +
                         gamma * (uv_c.v / point_c.w);
  float interpolated_reciprocal_w = alpha * (1 / point_a.w) +
                                    beta * (1 / point_b.w) +
                                    gamma * (1 / point_c.w);

  // Now we can divide the interpolated u and v by the interpolated reciprocal w
  // to get the perspective-corrected u and v. ACK: preferring readability over
  // performance here; i.e. lots of divisions. Specifically, the recriprocal
  // divisions for the triangle vertices could be lifted up (outside the loop),
  // since they are constant for the triangle.
  interpolated_u /= interpolated_reciprocal_w;
  interpolated_v /= interpolated_reciprocal_w;

  // We take the modulo of the texture width and height to ensure that the
  // text_x and text_y are within the texture bounds; i.e. we're dealing with a
  // discrete raster of a triangle, but went with integer values for the
  // vertices, so we could definitely end up with a pixel point that's outside
  // of our perfect triangle raster.
  int text_x = abs((int)(interpolated_u * texture_width)) % texture_width;
  int text_y = abs((int)(interpolated_v * texture_height)) % texture_height;

  draw_pixel(x, y, texture[texture_width * text_y + text_x]);
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

  fill_flat_bot_triangle(p0.x, p0.y, p1.x, p1.y, mx, my, color);
  fill_flat_top_triangle(p1.x, p1.y, mx, my, p2.x, p2.y, color);
}

void draw_textured_triangle(vec4_t p0, vec4_t p1, vec4_t p2, text2_t p0_uv,
                            text2_t p1_uv, text2_t p2_uv, uint32_t *texture) {
  // We need to sort vertices by ascending y-coordinate (y0 < y1 < y2)
  if (p0.y > p1.y) {
    vec4_swap(&p0, &p1);
    text2_swap(&p0_uv, &p1_uv);
  }

  if (p1.y > p2.y) {
    vec4_swap(&p1, &p2);
    text2_swap(&p1_uv, &p2_uv);
  }

  if (p0.y > p1.y) {
    vec4_swap(&p0, &p1);
    text2_swap(&p0_uv, &p1_uv);
  }

  p0_uv.v = 1.0 - p0_uv.v;
  p1_uv.v = 1.0 - p1_uv.v;
  p2_uv.v = 1.0 - p2_uv.v;

  // Render flat-bottom (top) triangle.
  float inv_slope_1 = 0;
  float inv_slope_2 = 0;

  if (p1.y - p0.y != 0) {
    inv_slope_1 = (p1.x - p0.x) / fabs(p1.y - p0.y);
  }

  if (p2.y - p0.y != 0) {
    inv_slope_2 = (p2.x - p0.x) / fabs(p2.y - p0.y);
  }

  if (p1.y - p0.y != 0) {
    for (int y = p0.y; y <= p1.y; y++) {
      int x_start = p1.x + (y - p1.y) * inv_slope_1;
      int x_end = p0.x + (y - p0.y) * inv_slope_2;

      if (x_end < x_start) {
        int_swap(&x_end, &x_start);
      }

      for (int x = x_start; x < x_end; x++) {
        draw_texel(x, y, texture, p0, p1, p2, p0_uv, p1_uv, p2_uv);
      }
    }
  }

  // Render flat-top (bottom) triangle.
  inv_slope_1 = 0;
  inv_slope_2 = 0;

  if (p2.y - p1.y != 0) {
    inv_slope_1 = (p2.x - p1.x) / fabs(p2.y - p1.y);
  }

  if (p2.y - p0.y != 0) {
    inv_slope_2 = (p2.x - p0.x) / fabs(p2.y - p0.y);
  }

  if (p2.y - p1.y != 0) {
    for (int y = p1.y; y <= p2.y; y++) {
      int x_start = p1.x + (y - p1.y) * inv_slope_1;
      int x_end = p0.x + (y - p0.y) * inv_slope_2;

      if (x_end < x_start) {
        int_swap(&x_end, &x_start);
      }

      for (int x = x_start; x < x_end; x++) {
        // draw_pixel(x, y, (x % 2 == 0 && y % 2 == 0 ? 0xFFFF00FF :
        // 0xFF000000));
        draw_texel(x, y, texture, p0, p1, p2, p0_uv, p1_uv, p2_uv);
      }
    }
  }
}
