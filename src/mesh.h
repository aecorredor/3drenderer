#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "vector.h"

#define CUBE_VERTICES_COUNT 8
extern vec3_t cube_vertices[CUBE_VERTICES_COUNT];

// 6 faces, 2 triangles each
#define CUBE_FACE_COUNT (6 * 2)
extern face_t cube_faces[CUBE_FACE_COUNT];

// Define struct for dynamic size meshes.
typedef struct {
  vec3_t *vertices;
  face_t *faces;
  vec3_t rotation;
} mesh_t;

// Using this global variable to store a global mesh during
// our program's lifetime.
extern mesh_t mesh;

void load_cube_mesh(void);

#endif
