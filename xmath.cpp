/*
 *
 * it's need to change zmath.h to something better !
 *
 * -O2 -std=gnu++11
 g++ -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-unused-value -Wwrite-strings -Wno-unused-but-set-variable -Wunused-function -O2 -std=gnu++11 -Wall -Wextra -D_REENTRANT -fPIC -DQT_NO_DEBUG

g++  -D_REENTRANT -fPIC -O2 -std=gnu++11 xmath.h
 *
 */
#include "xmath.h"

//#include "test.h"

/*
 *  TODO:
 *  xmath
 */
float IDENTITY_MATRIX[]={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

void print(float mat[16])
{
  printf("%8.4f,%8.4f,%8.4f,%8.4f\n"
            "%8.4f,%8.4f,%8.4f,%8.4f\n"
            "%8.4f,%8.4f,%8.4f,%8.4f\n"
            "%8.4f,%8.4f,%8.4f,%8.4f\n",
        mat[0],mat[1],mat[2],mat[3],
        mat[4],mat[5],mat[6],mat[7],
        mat[8],mat[9],mat[10],mat[11],
        mat[12],mat[13],mat[14],mat[15]);
}


void load(float dst[16], float src[16])
{
    for(int i=0; i<16;i++)
        dst[i]=src[i];
}

void loadIdentity(float mat[16])
{
    for(int i=0; i<16;i++)
        mat[i]=IDENTITY_MATRIX[i];
}

void copy(float dest[], float src[])
{
    int c;
    for(c = 0; c < 16; c++)
        dest[c] = src[c];
}

// mat4x4_translate(
void mat4x4_translate(float *dest, float x, float y, float z)
{
       /* dest[0] = 1;  dest[4] = 0;  dest[8] = 0; */ dest[12]= x;
       /* dest[1] = 0;  dest[5] = 1;  dest[9] = 0; */ dest[13]= y;
       /* dest[2] = 0;  dest[6] = 0;  dest[10]= 1; */ dest[14]= z;
      /*  dest[3] = 0;  dest[7] = 0;  dest[11]= 0; */ dest[15]= 1;
}

void rotMatX(float *dest, float a)
{
    copy(dest, IDENTITY_MATRIX);

    dest[5] = cos(a);
    dest[6] = -sin(a);
    dest[9] = sin(a);
    dest[10]= cos(a);
}

void rotMatY(float *dest, float a)
{
    copy(dest, IDENTITY_MATRIX);

    dest[0] = cos(a);
    dest[2] = sin(a);
    dest[8] = -sin(a);
    dest[10]= cos(a);
}

void rotMatZ(float * dest, float a)
{
    copy(dest, IDENTITY_MATRIX);

    dest[0] = cos(a);
    dest[1] = sin(a);
    dest[4] = -sin(a);
    dest[5]= cos(a);
}

float vect3f_distance(float v1[3], float v2[3])
{
    float v[3];
    float r;
    sub(v1,v2,v);
    r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    return r;
}

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
void makeIdentityf(float m[16])
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
    // TODO: macro
    v[0]=x;
    v[1]=y;
    v[2]=z;
}

void set4f(float v[4], float x,float y, float z, float u)
{
    // TODO: macro
    v[0]=x;
    v[1]=y;
    v[2]=z;
    v[3]=u;
}
// vec4_set
void set4f(float dst[4], float src[4])
{
    // TODO: macro
    dst[0]=src[0];
    dst[1]=src[1];
    dst[2]=src[2];
    dst[3]=src[3];
}

// vec3_set
void set(float dst[3], float src[3])
{
    // TODO: macro
    dst[0]=src[0];
    dst[1]=src[1];
    dst[2]=src[2];
}


void mat4x4_set(float dst[16], float src[16])
{
    // TODO: macro
    for (int i=0 ; i<16; i++)
        dst[i]=src[i];
}

void multiply(float v[3], float c,float result[3])
{
    result[0]= v[0]*c;
    result[1]= v[1]*c;
    result[2]= v[2]*c;
}

// mat4x4_mult
void multiply(float m[4][4], float v[3],float result[3])
{
    result[0]=m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];
    result[1]=m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];
    result[2]=m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2];
    //result[3]= 0 + 0 + 0 + 1;
}

void mat4x4_mult(float *matDest, float *matA, float *matB) // column
{
    float dest[16];
    int c, d, e;

    for(c = 0; c < 4; c++)
        for(d = 0; d < 4; d++)
            for(e = 0, dest[c + d * 4] = 0; e < 4; e++)
                dest[c + d * 4] += matA[c + e * 4] * matB[e + d * 4];

    copy(matDest, dest);
}

void quat_rotate(float r[3], float angle,float v[3],float result[3])
{
    // works well,
    /*
     * 쿼터니언을 이용한 회전,  기준 벡터 r, 그리고 회전각 angle, 입력 vertex, 결과  result
     * 사용중:
     * object.cpp ---> void camera::on_mouse_moved(int dx, int dy)
     */

    float x = r[0] * sin(angle/2);
    float y = r[1] * sin(angle/2);
    float z = r[2] * sin(angle/2);
    float w = cos(angle/2);

    float m[4][4];
    makeIdentityf(&m[0][0]);

    m[0][0] = 1-2*y*y -2*z*z;
    m[1][0] = 2*x*y + 2*w*z;
    m[2][0] = 2*x*z - 2*w*y;

    m[0][1] = 2*x*y - 2*w*z;
    m[1][1] = 1-2*x*x -2*z*z;
    m[2][1] = 2*y*z + 2*w*x;

    m[0][2] = 2*x*z + 2*w*y;
    m[1][2] = 2*y*z - 2*w*x;
    m[2][2] = 1-2*x*x -2*y*y;
    m[3][3] = 1;
    multiply(m,v,result);
}

void LookAt(float eye_pos[3], float forward[3], float up[3])
{
    // gluLookAt 복제품 :
    // works well, 이해 10%
    float side[3],new_up[3];
    float m[4][4];

    //sub(center, eye, forward);
    //normalize(forward);
    /* Side = forward x up */
    cross(forward, up, side); // ok
    normalize(side);
    // up을 왜 다시 계산하는가?   up 크기가 1이 아닐수 있음?? (hyun)

    /* Recompute up as: up = side x forward */
    cross(side, forward, new_up);

    makeIdentityf(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = new_up[0];
    m[1][1] = new_up[1];
    m[2][1] = new_up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixf(&m[0][0]);
    glTranslatef(-eye_pos[0], -eye_pos[1], -eye_pos[2]);
}



/*
 mat<4, 4, T, Q> lookAtRH(vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up)
{    
        vec<3, T, Q> const f(normalize(center - eye));
        vec<3, T, Q> const s(normalize(cross(f, up)));
        vec<3, T, Q> const u(cross(s, f));

        mat<4, 4, T, Q> Result(1);
        Result[0][0] = s.x;
        Result[1][0] = s.y;
        Result[2][0] = s.z;
        Result[0][1] = u.x;
        Result[1][1] = u.y;
        Result[2][1] = u.z;
        Result[0][2] =-f.x;
        Result[1][2] =-f.y;
        Result[2][2] =-f.z;
        Result[3][0] =-dot(s, eye);
        Result[3][1] =-dot(u, eye);
        Result[3][2] = dot(f, eye);
}
*/
float vect3f_dot(float v1[3], float v2[3])
{
    float r;
    r = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] ;
    return r;
}


glm::mat4 lookAt_with_glm(float eye_pos[3], float forward[3], float up[3])
{
    float side[3],new_up[3];
    //float m[4][4];
    glm::mat4 m(1);

    //sub(center, eye, forward);
    normalize(forward);
    normalize(up);
    /* Side = forward x up */
    cross(forward, up, side); // ok
    normalize(side);
    // up을 왜 다시 계산하는가?   up 크기가 1이 아닐수 있음?? (hyun)
    /* Recompute up as: up = side x forward */
    cross(side, forward, new_up);

    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = new_up[0];
    m[1][1] = new_up[1];
    m[2][1] = new_up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

  //  glTranslatef(-eye_pos[0], -eye_pos[1], -eye_pos[2]);    
    m[3][0] = - vect3f_dot(side, eye_pos);
    m[3][1] = - vect3f_dot(new_up, eye_pos);
    m[3][2] =   vect3f_dot(forward, eye_pos);
    return m;
}

/* https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

*/
glm::vec3  ray_intersects_triangle( const glm::vec3 &ray_origin, const glm::vec3 &ray_vector,
                                    glm::vec3 a,glm::vec3 b,glm::vec3 c)
    //const triangle3& triangle) // c++17
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    glm::vec3 edge1 = b - a;
    glm::vec3 edge2 = c - a;
    glm::vec3 ray_cross_e2 = glm::cross(ray_vector, edge2);
    float det = glm::dot(edge1, ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return {};    // This ray is parallel to this triangle.

    float inv_det = 1.0 / det;
    glm::vec3 s = ray_origin - a;
    float u = inv_det * dot(s, ray_cross_e2);

    if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u-1) > epsilon))
        return {};

    glm::vec3 s_cross_e1 = glm::cross(s, edge1);
    float v = inv_det * dot(ray_vector, s_cross_e1);

    if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon))
        return {};

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = inv_det * dot(edge2, s_cross_e1);

    if (t > epsilon) // ray intersection
    {
        return  glm::vec3(ray_origin + ray_vector * t);
    }
    else // This means that there is a line intersection but not a ray intersection.
        return glm::vec3();
}

/*
bool intersect_triangle( Ray R,  glm::vec3 A, glm::vec3 B, glm::vec3 C, float t,
                        float u,float v,glm::vec3 N)
{
   glm::vec3 E1 = B-A;
   glm::vec3 E2 = C-A;
   glm::vec3 N = glm::cross(E1,E2);
   float det = -glm::dot(R.Dir, N);
   float invdet = 1.0/det;
   vec3 AO  = R.Origin - A;
   vec3 DAO = glm::cross(AO, R.Dir);
   u =  glm::dot(E2,DAO) * invdet;
   v = -glm::dot(E1,DAO) * invdet;
   t =  glm::dot(AO,N)  * invdet;
   return (det >= 1e-6 && t >= 0.0 && u >= 0.0 && v >= 0.0 && (u+v) <= 1.0);
} */
