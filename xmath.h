#ifndef ZMATH_H
#define ZMATH_H

//#include <cmath>
// TODO:
// * naming... xmath x_math, mathx, math3d.h
//

class x_vector {
    float v[3];
};

void print(float mat[16]);

float vect3f_distance(float v1[3], float v2[3]);

void mat4x4_translate(float *dest, float x, float y, float z);
void copy(float dest[], float src[]);
void loadIdentity(float mat[16]);

void normalize(float v[3]);
void gluMakeIdentityf(float m[16]);
void cross(float v1[3], float v2[3], float result[3]);

void set(float v[3], float x,float y, float z);
void set(float dst[3], float src[3]);
void mat4x4_set(float dst[16], float src[16]);

void add(float v1[3], float v2[3], float result[3]);
void sub(float v1[3], float v2[3], float result[3]);
void multiply(float v[3],float c, float result[3]);
void multiply(float m[4][4], float v[3],float result[3]);
void mat4x4_mult(float dest[16], float *matA, float *matB);
void LookAt(float eye[3], float center[3], float up[3]);

/* matrix */
void load(float dst[16],float src[16]);

void quat_rotate(float r[3], float angle,float v[3],float result[3]);

#endif // ZMATH_H
