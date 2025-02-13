#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
  float u;
  float v;
} text2_t;

extern uint32_t *mesh_texture;
extern int texture_width;
extern int texture_height;

extern const uint8_t REDBRICK_TEXTURE[];

#endif