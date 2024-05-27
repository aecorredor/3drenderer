#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "vector.h"

#define MESH_VERTICES_COUNT 8
extern vec3_t mesh_vertices[MESH_VERTICES_COUNT];

// 6 faces, 2 triangles each
#define MESH_FACE_COUNT (6 * 2)
extern face_t mesh_faces[MESH_FACE_COUNT];
#endif
