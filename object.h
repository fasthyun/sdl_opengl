#ifndef OBJECT_H
#define OBJECT_H
#include "Shader.h"
#include "element3d.h"
#include "texture.h"
#include <glm/glm.hpp>
#include "./support/mesh.h"
#include "./support/material.h"


//class texture;
//class Material;

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
    xObject(string _name);
    xObject(xObject *);
    xObject(xObject *&);
    xObject(const xObject &);

    //xObject *createObject(){return new xObject("None");}; // need???
    virtual ~xObject();  // virtual why?
    virtual void update(float dt=0);
    virtual void draw();
    void copy(xObject *);
    void copyModel(xObject *);
    void set_parent(xObject *p){ parent=p;}
    void set_shader(Shader *s){ shader=s;}
    void set_texture(GLuint name){ texname=name;}
    void draw_meshes();
    void draw_axis();
    void make_axis();
    void update_model_matrix();
    void draw_dir_up();    
    void make_glVertexArray();    
    void make_radius(); // tmp
    void check_collision();
    vector<vertex>  vertexes; // verts
    vector<triangle> triangles; // tris
    vector<MeshFloat> meshes; //
    Material    *material; // testing: Material only 1
    ///vector<Material> materials;
    ///vector<xTexture> xtextures;

    // variables
    xObject *parent;
    //float pos[3];  // translate
    glm::vec3 position;
    float forward[3]; //
    float up[3];
    float force[3];
    glm::vec3 new_forward,new_up,new_force;
    float weight = 0.0 ;  // kg
    float movement_speed=0.0;
    float pitch = 0.0; // x , angle
    float yaw = 0.0; // y , angle
    float roll = 0.0; // z
    float scale[3] = {1.0,1.0,1.0}; // scale
    float life = 100;
    ///float model_mat[16];
    glm::mat4 model; // new matrix
    uint64_t prev_time;
    Shader *shader;

    // OpenGL element
    GLuint VBO, VAO=0, EBO;
    GLuint VBO_axis=0, VAO_axis=0;
    GLuint texname = 0;
    vector<xObject*> children;
    string name;
    string uuid;
    int d_type;
    bool xobject_found=false;
    string path; // tmp
    float radius; // tmp
    bool flag_axis_on=false;
    bool flag_shader_global=false;
    bool flag_gravity=false;
};

class camera : public xObject{

public:
    camera();
    virtual void update(float dt=0);
    void updateCameraVectors();
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


class objLight : public xObject{
    GLuint buffer;
    GLuint vertex_array;
    int d_size;
public:
    objLight();
    virtual void update(float dt=0);
    virtual void draw();
};

class cube : public xObject
{

public:
    cube();
    float rotate_angle_speed;
    virtual void update(float dt=0);
    virtual void draw();
    float time_x,hz;
};


xObject* findObject(string _name);


//#include "object.h"

class texture_object : public xObject{
public:
    texture_object();
    texture_object(char *texture_filename);
    //void make_glVertexArray();
    virtual void update(float dt=0);
    virtual void draw();
};


class texture_manager : public xObject{

    unsigned int VBO, VAO, EBO;
public:
    texture_manager();
    virtual void update(float dt=0);
    virtual void draw();
    static GLuint get_glname(string filename);
    static GLuint load_texture(string filename);
    void render_texture(GLuint texname,float x,float y,float z,float size) ;
};




#endif // OBJECT_H

