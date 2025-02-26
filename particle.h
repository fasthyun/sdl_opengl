#ifndef PARTICLE_H
#define PARTICLE_H

#include "object.h"

class particle2 : public xObject{
    GLuint buffer;
    GLuint vertex_array;
    int d_size;
    struct_particle_vertex  *motes;

public:
    particle2();
    void init_particles();
    virtual void update(float dt=0);
    virtual void draw();
    void update_VBO();
};

#endif // PARTICLE_H
