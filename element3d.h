#ifndef ELEMENT3D_H
#define ELEMENT3D_H

//#include <vector>
//#include <cmath>

using namespace std;

struct vertex {
    float v[3];
    float tu,tv;
    float r,g,b;
    int type;
};

//#define vertex_set(vert,x,y,z,_tu,_tv) vert->v[0]=x,vert->v[1]=y,vert->v[2]=z,vert->tu=_tu,vert->tv=_tv;

struct triangle {
    int  v[3]; // index of verts
};
void make_circle(vector<vertex*> &verts, vector<triangle*> &tris, float size);
void make_cube(vector<vertex> &verts, vector<triangle> &tris, float size);
void vertex_set(vertex &vert,float x,float y, float z,float tu, float tv);
void triangle_set(triangle &tri,int i, int j, int k );

#endif // ELEMENT3D_H
