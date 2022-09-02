#ifndef ZMATH_H
#define ZMATH_H

//#include <cmath>
// xmath x_math, mathx,
class x_vector {
    float v[3];
};

void normalize(float v[3]);
void gluMakeIdentityf(float m[16]);
void cross(float v1[3], float v2[3], float result[3]);

void set(float v[3], float x,float y, float z);
void set(float src[3], float dst[3]);
void add(float v1[3], float v2[3], float result[3]);
void sub(float v1[3], float v2[3], float result[3]);
void multiply(float v[3],float c, float result[3]);
void multiply(float m[4][4], float v[3],float result[3]);
void LookAt(float eye[3], float center[3], float up[3]);

void quat_rotate(float r[3], float angle,float v[3],float result[3]);

#endif // ZMATH_H
