#include "mesh.h"
#include "array.h"
#include "vector.h"
#include <stdio.h>
#include <string.h>

mesh_t mesh = {.vertices = NULL,
               .faces = NULL,
               .rotation = {0, 0, 0},
               .translation = {0, 0, 0},
               .scale = {1.0, 1.0, 1.0}};

vec3_t cube_vertices[CUBE_VERTICES_COUNT] = {
    {.x = -1, .y = -1, .z = -1}, {.x = -1, .y = 1, .z = -1},
    {.x = 1, .y = 1, .z = -1},   {.x = 1, .y = -1, .z = -1},
    {.x = 1, .y = 1, .z = 1},    {.x = 1, .y = -1, .z = 1},
    {.x = -1, .y = 1, .z = 1},   {.x = -1, .y = -1, .z = 1},
};

face_t cube_faces[CUBE_FACE_COUNT] = {
    // front - blue
    {.a = 1, .b = 2, .c = 3, .color = 0x0000FF},
    {.a = 1, .b = 3, .c = 4, .color = 0x0000FF},
    // right - yellow
    {.a = 4, .b = 3, .c = 5, .color = 0xFFFF00},
    {.a = 4, .b = 5, .c = 6, .color = 0xFFFF00},
    // back - red
    {.a = 6, .b = 5, .c = 7, .color = 0xFF0000},
    {.a = 6, .b = 7, .c = 8, .color = 0xFF0000},
    // left - green
    {.a = 8, .b = 7, .c = 2, .color = 0x00FF00},
    {.a = 8, .b = 2, .c = 1, .color = 0x00FF00},
    // top - purple
    {.a = 2, .b = 7, .c = 5, .color = 0x800080},
    {.a = 2, .b = 5, .c = 3, .color = 0x800080},
    // bottom - orange
    {.a = 6, .b = 8, .c = 1, .color = 0xFFA500},
    {.a = 6, .b = 1, .c = 4, .color = 0xFFA500},
};

void load_cube_mesh(void) {
  for (int i = 0; i < CUBE_VERTICES_COUNT; i++) {
    array_push(mesh.vertices, cube_vertices[i]);
  }

  for (int i = 0; i < CUBE_FACE_COUNT; i++) {
    array_push(mesh.faces, cube_faces[i]);
  }
}

void load_obj_file_data(char *filename) {
  FILE *file;
  file = fopen(filename, "r");
  char line[1024];

  while (fgets(line, 1024, file)) {
    // Vertex info
    if (strncmp(line, "v ", 2) == 0) {
      vec3_t vertex;
      sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      array_push(mesh.vertices, vertex);
    }

    // Face info
    if (strncmp(line, "f ", 2) == 0) {
      int vertex_indices[3];
      int texture_indices[3];
      int normal_indices[3];

      sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0],
             &texture_indices[0], &normal_indices[0], &vertex_indices[1],
             &texture_indices[1], &normal_indices[1], &vertex_indices[2],
             &texture_indices[2], &normal_indices[2]);

      face_t face = {
          .a = vertex_indices[0],
          .b = vertex_indices[1],
          .c = vertex_indices[2],
      };
      array_push(mesh.faces, face);
    }
  }
}
