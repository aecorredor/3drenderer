#include "matrix.h"
#include <math.h>

mat4_t mat4_identity() {
  mat4_t m = {.m = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};

  return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
  mat4_t m = mat4_identity();
  m.m[0][0] = sx;
  m.m[1][1] = sy;
  m.m[2][2] = sz;

  return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
  mat4_t m = mat4_identity();
  m.m[0][3] = tx;
  m.m[1][3] = ty;
  m.m[2][3] = tz;

  return m;
}

mat4_t mat4_make_rotation_x(float angle) {
  mat4_t m = mat4_identity();
  float cosine = cos(angle);
  float sine = sin(angle);

  m.m[1][1] = cosine;
  m.m[1][2] = -sine;
  m.m[2][1] = sine;
  m.m[2][2] = cosine;

  return m;
}

mat4_t mat4_make_rotation_y(float angle) {
  mat4_t m = mat4_identity();
  float cosine = cos(angle);
  float sine = sin(angle);

  m.m[0][0] = cosine;
  m.m[0][2] = sine;
  m.m[2][0] = -sine;
  m.m[2][2] = cosine;

  return m;
}

mat4_t mat4_make_rotation_z(float angle) {
  mat4_t m = mat4_identity();
  float cosine = cos(angle);
  float sine = sin(angle);

  m.m[0][0] = cosine;
  m.m[0][1] = -sine;
  m.m[1][0] = sine;
  m.m[1][1] = cosine;

  return m;
}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar) {
  mat4_t m = {{{0}}};
  m.m[0][0] = aspect * (1 / tan(fov / 2));
  m.m[1][1] = 1 / tan(fov / 2);
  m.m[2][2] = zfar / (zfar - znear);
  m.m[2][3] = (-zfar * znear) / (zfar - znear);
  m.m[3][2] = 1.0;

  return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
  vec4_t result;
  result.x =
      m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
  result.y =
      m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
  result.z =
      m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
  result.w =
      m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

  return result;
}

mat4_t mat4_mul_mat4(mat4_t ma, mat4_t mb) {
  mat4_t m;

  // [0, 0, 0, 0] * [0, 0, 0, 0]
  // [0, 0, 0, 0]   [0, 0, 0, 0]
  // [0, 0, 0, 0]   [0, 0, 0, 0]
  // [0, 0, 0, 0]   [0, 0, 0, 0]
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m.m[i][j] = ma.m[i][0] * mb.m[0][j] + ma.m[i][1] * mb.m[1][j] +
                  ma.m[i][2] * mb.m[2][j] + ma.m[i][3] * mb.m[3][j];
    }
  }

  return m;
}

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v) {
  vec4_t result = mat4_mul_vec4(mat_proj, v);

  // perform perspective divide with original z-value that is now stored in w.
  if (result.w != 0.0) {
    result.x /= result.w;
    result.y /= result.w;
    result.z /= result.w;
  }

  return result;
}
