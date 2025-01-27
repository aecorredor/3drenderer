#include "light.h"

light global_light = {.direction = {.x = 0, .y = 0, .z = 1}};

uint32_t apply_light_intensity_factor(uint32_t original_color,
                                      float percetange_factor) {

  if (percetange_factor < 0)
    percetange_factor = 0;

  if (percetange_factor > 1) {
    percetange_factor = 1;
  }

  uint32_t a = original_color & 0xFF000000;
  uint32_t r = (original_color & 0x00FF0000) * percetange_factor;
  uint32_t g = (original_color & 0x0000FF00) * percetange_factor;
  uint32_t b = (original_color & 0x000000FF) * percetange_factor;

  uint32_t new_color =
      a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

  return new_color;
}