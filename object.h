#ifndef OBJECT_H
#define OBJECT_H
#include <vector>
#include <QObject>
#include <QtOpenGL>


using namespace  std;
struct vertex {
    float v[3];
    float tu,tv;
};


struct triangle {
    int  v[3]; // index of verts
};

class xObject {
public:
    xObject();
    void update();    
    void draw();
    void on_key_pressed(uint key);
    void make_circle(float size=1.0);
    void draw_axis();
    void draw_dir_up();

    vector<vertex*> vertexes; // verts
    vector<triangle*> triangles; // tris

private: // just
    float pos[3];
    float dir[3];
    float up[3];
    float force[3];
    float weight;
    uint64_t prev_time;
};

class camera : public xObject{
    void update();
};

#endif // OBJECT_H
