#ifndef OBJECT_H
#define OBJECT_H
#include <vector>
#include <QObject>
#include <QtOpenGL>
#include "element3d.h"

// xObject , object_x , x_object
using namespace  std;


class xObject {
public:
    xObject();
    virtual void update(float dt=0);
    virtual void draw();

    void make_circle(float size=1.0);
    void draw_axis();
    void draw_dir_up();

    vector<vertex*> vertexes; // verts
    vector<triangle*> triangles; // tris
    virtual void on_key_pressed(uint key);
    virtual void on_key_released(uint key);
    virtual void on_mouse_moved(int dx,int dy);

    // just
    float pos[3];
    float forward[3]; //
    float up[3];
    float force[3];
    float weight;
    uint64_t prev_time;
};

class camera : public xObject{

public:
    camera();
    virtual void update(float dt=0);
    void on_key_pressed(uint key);
    void on_key_released(uint key);
    void on_mouse_moved(int dx,int dy);
};


class grid : public xObject{

public:
    grid();
    virtual void update(float dt=0);
    virtual void draw();
};


#endif // OBJECT_H
