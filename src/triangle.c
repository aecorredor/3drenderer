#include "triangle.h"
#include "display.h"

void int_swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

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

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color) {
  // TODO: implement
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          uint32_t color) {
  // We need to sort vertices by ascending y-coordinate (y0 < y1 < y2)
  if (y0 > y1) {
    int_swap(&y0, &y1);
    int_swap(&x0, &x1);
  }

  if (y1 > y2) {
    int_swap(&y1, &y2);
    int_swap(&x1, &x2);
  }

  if (y0 > y1) {
    int_swap(&y0, &y1);
    int_swap(&x0, &x1);
  }

  // Calculate thew new vertex (mx, my) using triangle similarity
  int my = y1;
  int mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

  fill_flat_bot_triangle(x0, y0, x1, y1, mx, my, color);
  fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
}
