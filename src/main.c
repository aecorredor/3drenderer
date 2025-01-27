#include "array.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <stdio.h>

bool is_running = false;

vec3_t camera_position = {.x = 0, .y = 0, .z = 0};
mat4_t proj_matrix;

triangle_t *triangles_to_render = NULL;

int previous_frame_time = 0;

void setup(void) {
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  // the same as 180/3, or 60deg.
  float fov = M_PI / 3.0;
  float aspect = (float)window_height / window_width;
  float znear = 0.1;
  float zfar = 100.0;
  proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

  load_obj_file_data("./src/assets/f22.obj");
  // load_cube_mesh();
}

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    is_running = false;
    break;
  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {
    case SDLK_ESCAPE:
      is_running = false;
      break;
    case SDLK_1:
      render_method = RENDER_WIRE_VERTEX;
      break;
    case SDLK_2:
      render_method = RENDER_WIRE;
      break;
    case SDLK_3:
      render_method = RENDER_FILL_TRIANGLE;
      break;
    case SDLK_4:
      render_method = RENDER_FILL_TRIANGLE_WIRE;
      break;
    case SDLK_c:
      cull_method = CULL_BACKFACE;
      break;
    case SDLK_d:
      cull_method = CULL_NONE;
      break;
    }
  }
}

void set_vec3_vertices(vec3_t *vertices, vec4_t transformed_vertices[3]) {
  vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*      A    */
  vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*    /  \   */
  vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /*   C -- B  */

  vertices[0] = vector_a;
  vertices[1] = vector_b;
  vertices[2] = vector_c;
}

vec3_t get_normal(vec3_t transformed_vertices[3]) {
  vec3_t vector_a = transformed_vertices[0];
  vec3_t vector_b = transformed_vertices[1];
  vec3_t vector_c = transformed_vertices[2];

  // Get the vectors of two sides of the triangle pointing
  // to the camera.
  vec3_t vector_ab = vec3_sub(vector_b, vector_a);
  vec3_t vector_ac = vec3_sub(vector_c, vector_a);
  vec3_normalize(&vector_ab);
  vec3_normalize(&vector_ac);

  // Compute the face normal, which is just the cross product
  // to find perpendicular vector - left handed coordinate system.
  vec3_t normal = vec3_cross(vector_ab, vector_ac);
  vec3_normalize(&normal);

  return normal;
}

bool should_backface_cull(vec3_t transformed_vertices[3]) {
  vec3_t vector_a = transformed_vertices[0];

  // Compute the face normal, which is just the cross product
  // to find perpendicular vector - left handed coordinate system.
  vec3_t normal = get_normal(transformed_vertices);

  // Find the vector between a point in the triangle (face) and the
  // camera origin.
  vec3_t camera_ray = vec3_sub(camera_position, vector_a);

  // Calculate how aligned the camera ray is with the face normal.
  // (using dot product).
  float face_camera_alignment = vec3_dot(normal, camera_ray);

  return face_camera_alignment < 0;
}

void update(void) {
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  triangles_to_render = NULL;

  // Change the mesh scale/rotation values per animation frame.
  // mesh.rotation.x += 0.005;
  // mesh.rotation.y += 0.005;
  // mesh.rotation.z += 0.005;
  // mesh.scale.x += 0.002;
  // mesh.scale.y += 0.001;
  // mesh.translation.x += 0.01;
  // Translate vertex away from the camera.
  mesh.translation.z = 5;

  // Loop all triangle faces of our mesh.
  for (int i = 0; i < array_length(mesh.faces); i++) {
    face_t mesh_face = mesh.faces[i];
    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    triangle_t projected_triangle;

    mat4_t scale_matrix =
        mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(
        mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    vec4_t transformed_vertices[3];

    // Loop all three vertices of the current face and apply
    // transformations.
    for (int j = 0; j < 3; j++) {

      // scale, rotate, and then translate, in that exclusive order.
      mat4_t world_matrix = mat4_identity();
      world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
      world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

      vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
      transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
      transformed_vertices[j] = transformed_vertex;
    }

    vec3_t vec3_transformed_vertices[3];

    set_vec3_vertices(vec3_transformed_vertices, transformed_vertices);

    if (cull_method == CULL_BACKFACE &&
        should_backface_cull(vec3_transformed_vertices)) {
      // Skip rendering the face.
      continue;
    }

    float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z +
                       transformed_vertices[2].z) /
                      3.0;

    vec3_t normal = get_normal(vec3_transformed_vertices);
    float light_intensity_factor = -vec3_dot(normal, global_light.direction);

    for (int j = 0; j < 3; j++) {
      vec2_t projected_point = vec2_from_vec4(
          mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]));

      float width_factor = window_width / 2.0;
      float height_factor = window_height / 2.0;

      // Scale into view
      projected_point.x *= width_factor;
      projected_point.y *= height_factor;

      // Account for flipped y-axis in models vs screen buffer.
      projected_point.y *= -1;

      // Translate points to the middle of the screen.
      projected_point.x += width_factor;
      projected_point.y += height_factor;

      projected_triangle.points[j] = projected_point;
      projected_triangle.color =
          apply_light_intensity_factor(mesh_face.color, light_intensity_factor);
      projected_triangle.avg_depth = avg_depth;
    }

    // Save the projected triangle in the array of triangles to render.
    array_push(triangles_to_render, projected_triangle);
  }

  // Simple bubble sort:
  //  Initial array: [2️⃣, 5️⃣, 1️⃣, 4️⃣]

  // Step-by-step sorting:

  // i=0:
  //   j=0: [2️⃣, 5️⃣, 1️⃣, 4️⃣] Compare 2-2 (i with i): No swap
  //   j=1: [2️⃣, 5️⃣, 1️⃣, 4️⃣] Compare 2-5 (i with j): Swap! ➡️ [5️⃣, 2️⃣, 1️⃣, 4️⃣]
  //   j=2: [5️⃣, 2️⃣, 1️⃣, 4️⃣] Compare 5-1 (i with j): No swap
  //   j=3: [5️⃣, 2️⃣, 1️⃣, 4️⃣] Compare 5-4 (i with j): No swap

  // i=1:
  //   j=1: [5️⃣, 2️⃣, 1️⃣, 4️⃣] Compare 2-2 (i with i): No swap
  //   j=2: [5️⃣, 2️⃣, 1️⃣, 4️⃣] Compare 2-1 (i with j): No swap
  //   j=3: [5️⃣, 2️⃣, 1️⃣, 4️⃣] Compare 2-4 (i with j): Swap! ➡️ [5️⃣, 4️⃣, 1️⃣, 2️⃣]

  // i=2:
  //   j=2: [5️⃣, 4️⃣, 1️⃣, 2️⃣] Compare 1-1 (i with i): No swap
  //   j=3: [5️⃣, 4️⃣, 1️⃣, 2️⃣] Compare 1-2 (i with j): Swap! ➡️ [5️⃣, 4️⃣, 2️⃣, 1️⃣]

  // i=3:
  //   j=3: [5️⃣, 4️⃣, 2️⃣, 1️⃣] Compare 1-1 (i with i): No swap

  // Final sorted array: [5️⃣, 4️⃣, 2️⃣, 1️⃣]
  int mesh_face_count = array_length(triangles_to_render);
  for (int i = 0; i < mesh_face_count; i++) {
    for (int j = i; j < mesh_face_count; j++) {
      triangle_t curr = triangles_to_render[i];
      triangle_t next = triangles_to_render[j];

      // Higher depth go first so they are rendered in the back.
      if (curr.avg_depth < next.avg_depth) {
        triangles_to_render[i] = next;
        triangles_to_render[j] = curr;
      }
    }
  }
}

void render(void) {
  draw_grid();

  // Loop all the projected triangles and render them.
  int mesh_face_count = array_length(triangles_to_render);

  for (int i = 0; i < mesh_face_count; i++) {
    triangle_t triangle = triangles_to_render[i];

    switch (render_method) {
    case RENDER_WIRE_VERTEX:
      // Draw all vertices of the triangle.
      draw_rect(triangle.points[0], 6, 6, 0xFF0000);
      draw_rect(triangle.points[1], 6, 6, 0xFF0000);
      draw_rect(triangle.points[2], 6, 6, 0xFF0000);
      draw_triangle(triangle.points[0], triangle.points[1], triangle.points[2],
                    0xFFFFFF);
      break;
    case RENDER_WIRE:
      draw_triangle(triangle.points[0], triangle.points[1], triangle.points[2],
                    0xFFFFFF);
      break;
    case RENDER_FILL_TRIANGLE:
      draw_filled_triangle(triangle.points[0], triangle.points[1],
                           triangle.points[2], triangle.color);
      break;
    case RENDER_FILL_TRIANGLE_WIRE:
      draw_filled_triangle(triangle.points[0], triangle.points[1],
                           triangle.points[2], triangle.color);
      draw_triangle(triangle.points[0], triangle.points[1], triangle.points[2],
                    0xFFFFFF);
      break;
    }
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
  render_method = RENDER_WIRE;
  cull_method = CULL_BACKFACE;

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
