#ifndef ELEMENT3D_H
#define ELEMENT3D_H


#include <vector>

using namespace std;

struct vertex {
    float v[3];
    float tu,tv;
};


struct triangle {
    int  v[3]; // index of verts
};
void make_circle(vector<vertex*> &verts, vector<triangle*> &tris, float size);

#endif // ELEMENT3D_H
