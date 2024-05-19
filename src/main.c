#include "display.h"
#include "vector.h"
#include <stdio.h>

bool is_running = false;

const int CUBE_POINT_COUNT = 9 * 9 * 9;
vec3_t cube_points[CUBE_POINT_COUNT];
vec2_t projected_points[CUBE_POINT_COUNT];
float fov_factor = 128;

void setup(void) {
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
}

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    is_running = false;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE)
      is_running = false;
    break;
  }
}

vec2_t project(vec3_t point) {
  vec2_t projected_point = {.x = (fov_factor * point.x),
                            .y = (fov_factor * point.y)};
  return projected_point;
}

void update(void) {
  for (int i = 0; i < CUBE_POINT_COUNT; i++) {
    vec3_t point = cube_points[i];
    vec2_t projected_point = project(point);
    projected_points[i] = projected_point;
  }
}

void render(void) {
  draw_grid();

  for (int i = 0; i < CUBE_POINT_COUNT; i++) {
    vec2_t projected_point = projected_points[i];
    draw_rect(projected_point.x + (window_width / 2.0f),
              projected_point.y + (window_height / 2.0f), 4, 4, 0xFFFFFF00);
  }

  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

int main(void) {
  is_running = initialize_window();

  setup();

  int point_count = 0;

  for (float x = -1; x <= 1; x += 0.25) {
    for (float y = -1; y <= 1; y += 0.25) {
      for (float z = -1; z <= 1; z += 0.25) {
        vec3_t new_point = {.x = x, .y = y, .z = z};
        cube_points[point_count++] = new_point;
      }
    }
  }

  while (is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();
  return 0;
}
