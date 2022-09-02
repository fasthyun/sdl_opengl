#include "element3d.h"
#include <cmath>

void make_cube(vector<vertex*> &verts, vector<triangle*> &tris, float size)
{
    // move to element3d.cpp  3d_element, element_3d
    // blender로 아니면 이렇게 ... 둘다 필요?

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

void make_cylinder(vector<vertex*> &verts, vector<triangle*> &tris, float size)
{

}

void make_cone(vector<vertex*> &verts, vector<triangle*> &tris, float size)
{

}
