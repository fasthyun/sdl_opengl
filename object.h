#ifndef OBJECT_H
#define OBJECT_H
#include "Shader.h"
#include "element3d.h"
#include "./support/mesh.h"
#include "./support/material.h"

/* TODO:
   * naming to xObject.h
*/

//typedef vector<vertex*> vec_vertex;

#define TYPE_STATIC         0x01
#define TYPE_DYNAMIC        0x02
#define TYPE_CUBE           0x05
#define TYPE_SPHERE         0x06
#define TYPE_ANTIGRAVITY    0b00000001  // weight
#define TYPE_GROUND         0xF0
#define TYPE_SKY            0xF1
#define TYPE_PARTICLE       0xF2
#define TYPE_CONTROLLER     0xF3


using namespace  std;

class xObject {

public:
    xObject();
    ~xObject();
    virtual void update(float dt=0);
    virtual void draw();
    void set_parent(xObject *p){ parent=p;}
    void set_shader(Shader *s){ shader=s;}
    void set_texture(GLuint name){ texname=name;}
    void draw_meshes();
    void draw_axis();
    void draw_dir_up();
    void load_gltf(string name); // remove
    void make_glVertexArray();
    void make_radius(); // tmp
    vector<vertex>  vertexes; // verts
    vector<triangle> triangles; // tris
    vector<MeshFloat> meshes;
    ///vector<Material> materials;
    ///vector<xTexture> xtextures;

    // variables
    xObject *parent;
    float pos[3];
    float forward[3]; //
    float up[3];
    float force[3];
    float weight = 0.0 ;  // kg
    float model_mat[16];
    uint64_t prev_time;
    Shader *shader;

    // OpenGL element
    GLuint VBO, VAO, EBO;
    GLuint texname;
    vector<xObject*> children;
    string name;
    string uuid;
    int d_type;
    bool xobject_found=false;
    string path; // tmp
    float radius; // tmp
};

class camera : public xObject{

public:
    camera();
    virtual void update(float dt=0);
    void on_key_pressed(uint key);
    void on_key_released(uint key);
    void on_mouse_moved(int dx,int dy);    
    bool d_focus;
    // temporary
    uint key_forward;
    uint key_backward;
    uint key_side_right;
    uint key_side_left;
    xObject *d_ball;
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



class CollisionDetector : public xObject{
    GLuint buffer;
    GLuint vertex_array;
    int grid_n;
public:
    CollisionDetector();
    virtual void update(float dt=0);
    virtual void draw(){}
};


class particle : public xObject{
    GLuint buffer;
    GLuint vertex_array;
    int d_size;
public:
    particle();
    virtual void update(float dt=0);
    virtual void draw();
};

xObject* findObject(string _name);



#endif // OBJECT_H
