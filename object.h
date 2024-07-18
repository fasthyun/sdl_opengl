#ifndef OBJECT_H
#define OBJECT_H
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "Shader.h"
#include "element3d.h"
#include "./support/mesh.h"
#include "./support/material.h"

/* TODO:
   * naming to xObject , object_x , x_object, object3d.h
*/

//typedef vector<vertex*> vec_vertex;

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
    void make_glVertexArray();
    vector<vertex>  vertexes; // verts
    vector<triangle> triangles; // tris
    vector<MeshFloat> meshes;
    vector<Material> materials;
    vector<Texture> xtextures;

    // just
    float pos[3];
    float forward[3]; //
    float up[3];
    float force[3];
    float weight;
    float mat[16];
    uint64_t prev_time;
    Shader *shader;

    GLuint VBO, VAO, EBO;
    GLuint texname;
    vector<xObject> children;
};

class camera : public xObject{

public:
    camera();
    virtual void update(float dt=0);
    void on_key_pressed(uint key);
    void on_key_released(uint key);
    void on_mouse_moved(int dx,int dy);    

    // temporary
    uint key_forward;
    uint key_backward;
    uint key_side_right;
    uint key_side_left;
};


class grid : public xObject{
    GLuint buffer;
    GLuint vertex_array;
    int grid_n;
public:
    grid();
    virtual void update(float dt=0);
    virtual void draw();
};






#endif // OBJECT_H
