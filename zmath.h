#ifndef ZMATH_H
#define ZMATH_H

//#include <cmath>

void normalize(float v[3]);
void gluMakeIdentityf(float m[16]);
void cross(float v1[3], float v2[3], float result[3]);
void add(float v1[3], float v2[3], float result[3]);
void sub(float v1[3], float v2[3], float result[3]);
void set(float v[3], float x,float y, float z);
void gluLookAt(float eye[3], float center[3], float up[3]);

#endif // ZMATH_H
