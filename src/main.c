#include "array.h"
#include "display.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <stdio.h>

bool is_running = false;

vec3_t camera_position = {.x = 0, .y = 0, .z = -5};

triangle_t *triangles_to_render = NULL;

float fov_factor = 640;

int previous_frame_time = 0;

void setup(void) {
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  // load_cube_mesh();
  load_obj_file_data("./src/assets/f22.obj");
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
  vec2_t projected_point = {.x = (fov_factor * point.x) / point.z,
                            .y = (fov_factor * point.y) / point.z};
  return projected_point;
}

void update(void) {
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  triangles_to_render = NULL;

  mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.01;
  mesh.rotation.z += 0.01;

  // Loop all triangle faces of our mesh.
  for (int i = 0; i < array_length(mesh.faces); i++) {
    face_t mesh_face = mesh.faces[i];
    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    // Loop all three vertices of the current face and apply
    // transformations.

    triangle_t projected_triangle;

    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];
      transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

      // Translate vertex away from the camera.
      transformed_vertex.z -= camera_position.z;

      vec2_t projected_point = project(transformed_vertex);

      // Scale and translate projected points to the middle of the screen.
      projected_point.x += (window_width / 2.0f);
      projected_point.y += (window_height / 2.0f);

      projected_triangle.points[j] = projected_point;
    }

    // Save the projected triangle in the array of triangles to render.
    array_push(triangles_to_render, projected_triangle)
  }
}

void render(void) {
  draw_grid();

  // Loop all the projected triangles and render them.
  int mesh_face_count = array_length(triangles_to_render);
  for (int i = 0; i < mesh_face_count; i++) {
    triangle_t triangle = triangles_to_render[i];
    // Draw all vertices of the triangle.
    draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
    draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
    draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

    // Draw line between vertices.
    draw_triangle(triangle.points[0], triangle.points[1], triangle.points[2],
                  0xFFFFFF00);
  }

  array_free(triangles_to_render);
  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

void free_resources(void) {
  free(color_buffer);
  array_free(mesh.vertices);
  array_free(mesh.faces);
}

int main(void) {
  is_running = initialize_window();

  setup();

  while (is_running) {
    process_input();
    update();
    render();
  }

  free_resources();
  destroy_window();
  return 0;
}
