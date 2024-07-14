#include "element3d.h"
#include <cmath>

void make_cube(vector<vertex> &verts, vector<triangle> &tris, float size)
{
    // move to element3d.cpp  3d_element, element_3d
    // blender로  아니면 이렇게 ... 둘 다 필요?
    vertex vert;
    vertex_set(vert,-1.f, 1.f, 1.f,  0.,1.);  verts.push_back(vert); // 0  upper left
    vertex_set(vert, 1.f, 1.f, 1.f,  1.0,1.0);  verts.push_back(vert); // 1  upper right
    vertex_set(vert, 1.f,-1.f, 1.f,  1.f,0.);  verts.push_back(vert); // 2  down right
    vertex_set(vert,-1.f,-1.f, 1.f,  0,0);  verts.push_back(vert); // 3  down left

    //vertex_set(vert, 1.f, 1.f, 1.f,  1.,1.);  verts.push_back(vert); // 1  upper right
    //vertex_set(vert, 1.f,-1.f, 1.f,  1.,0);  verts.push_back(vert); // 2  down right

    triangle tri;
    triangle_set(tri,0,1,3); tris.push_back(tri);
    triangle_set(tri,1,2,3); tris.push_back(tri);
}

void make_circle(vector<vertex*> &verts, vector<triangle*> &tris, float size)
{
    // move to element3d.cpp  3d_element, element_3d
    // blender로 아니면 이렇게 ... 둘다 필요?

    int n=50;
    float dt=2*M_PI/n;
    float x,y;
    vertex *vert=new vertex(); //center (0,0,0)
    vert->v[0]=0;
    vert->v[1]=0;
    vert->v[2]=0;
    verts.push_back(vert); // center
    for (int i=0;i<n;i++)
    {
         vertex *vert=new vertex();
         x=sin(dt*i);
         y=cos(dt*i);
         vert->v[0]=x;
         vert->v[1]=y;
         vert->v[2]=0;
         verts.push_back(vert);
    }

    for (int i=0;i< n;i++)
    {
        triangle * tri = new triangle();
        tri->v[0]=0;
        tri->v[1]=i;
        if (i+1 >= n)
            tri->v[2]=1;
        else
            tri->v[2]=i+1;
        tris.push_back(tri);
    }
}

void make_cylinder(float size=1.0)
{
    /*
    float len=3;
    glLineWidth(3);
    glColor3f(1.0, 0, 0);
    glBegin(GL_POLYGON);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);

    glColor3f(0.0, 1.0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,len,0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,len);
    glEnd();
    glLineWidth(1); */
}

void make_cylinder(vector<vertex*> &verts, vector<triangle*> &tris, float size)
{

}

void make_cone(vector<vertex*> &verts, vector<triangle*> &tris, float size)
{

}

void vertex_set(vertex &vert, float x, float y, float z, float _tu, float _tv)
{
    vert.v[0]=x;
    vert.v[1]=y;
    vert.v[2]=z;
    vert.tu=_tu;
    vert.tv=_tv;
}

void triangle_set(triangle &tri, int i, int j, int k)
{
    tri.v[0]=i;
    tri.v[1]=j;
    tri.v[2]=k;
}
