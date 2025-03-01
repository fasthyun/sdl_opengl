#ifndef PARTICLE_H
#define PARTICLE_H

#include "object.h"

struct struct_particle_vertex {
    glm::vec3 position;
    glm::vec3 force;
    float life;
    float force_life;
    float size;
    float total_life;
};

class particle_base : public xObject{
    GLuint buffer;
    GLuint vertex_array;
    int d_size;
    float *vertexData;
    struct_particle_vertex  *motes;

public:
    particle_base();
    void init_particles();
    virtual void update(float dt=0);
    virtual void draw();
    void update_VBO();
};

class particle_spark : public particle_base {
    GLuint buffer;
    GLuint vertex_array;
    int d_size;
    struct_particle_vertex  *motes;

public:
    particle_spark();
    void init_particles();
    virtual void update(float dt=0);
    virtual void draw();
    void update_VBO();
};




#endif // PARTICLE_H
