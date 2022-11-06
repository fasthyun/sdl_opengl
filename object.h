#ifndef OBJECT_H
#define OBJECT_H
#include <vector>
#include "element3d.h"
#include "./support/mesh.h"
#include "./support/material.h"

/* TODO:
   * naming to xObject , object_x , x_object, object3d.h
*/

typedef vector<vertex*> vec_vertex;

using namespace  std;

class xObject {
public:
    xObject();
    virtual void update(float dt=0);
    virtual void draw();
    void draw_meshes();

    void draw_axis();
    void draw_dir_up();

    void load_gltf(string name);

    vec_vertex  vertexes; // verts
    vector<triangle*> triangles; // tris
    vector<MeshFloat> meshes;
    vector<Material> materials;
    vector<Texture> xtextures;

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
