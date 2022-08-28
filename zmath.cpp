#include <cmath>
#include <qopengl.h>

void normalize(float v[3])
{
    float r;
    r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    if (r == 0.0) return;
    v[0] /= r;
    v[1] /= r;
    v[2] /= r;
}
/*
** Make m an identity matrix
*/
void gluMakeIdentityf(float m[16])
{
    // m[16] == m[4][4] same? (hyun)
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

void cross(float v1[3], float v2[3], float result[3])
{
    result[0] = v1[1]*v2[2] - v1[2]*v2[1];
    result[1] = v1[2]*v2[0] - v1[0]*v2[2];
    result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

void add(float v1[3], float v2[3], float result[3])
{
    result[0] = v1[0] + v2[0];
    result[1] = v1[1] + v2[1];
    result[2] = v1[2] + v2[2];
}

void sub(float v1[3], float v2[3], float result[3])
{
    result[0] = v1[0] - v2[0];
    result[1] = v1[1] - v2[1];
    result[2] = v1[2] - v2[2];
}
void set(float v[3], float x,float y, float z)
{
    v[0]=x;
    v[1]=y;
    v[2]=z;
}

void gluLookAt(float eye[3], float center[3], float up[3])
{
    // 이해 10%
    float forward[3], side[3];
    float m[4][4];

    sub(center, eye, forward);

    normalize(forward);

    /* Side = forward x up */
    cross(forward, up, side);
    normalize(side);
    // 왜 다시계산하는가? (hyun) --> up 크기가 1이 아닐수 있음

    /* Recompute up as: up = side x forward */
    cross(side, forward, up);

    gluMakeIdentityf(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixf(&m[0][0]);
    glTranslatef(-eye[0], -eye[1], -eye[2]);
}
