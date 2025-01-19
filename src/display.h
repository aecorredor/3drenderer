#ifndef DISPLAY_H
#define DISPLAY_H

#include "vector.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texture;
extern uint32_t *color_buffer;
extern int window_width;
extern int window_height;

bool initialize_window(void);
void draw_pixel(int x, int y, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_window(void);
void draw_grid(void);
void draw_line(vec2_t p0, vec2_t p1, uint32_t color);
void draw_rect(vec2_t p, int width, int height, uint32_t color);
void draw_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);

#endif