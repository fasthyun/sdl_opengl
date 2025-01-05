#ifndef MODEL_H
#define MODEL_H


//#include "object.h"

//bool Import3DFromFile( const std::string &filename);
bool Import3DFromFile(const std::string filename, xObject *obj);
void init_models();

class model_object : public xObject{
public:
    //model_object(){};
    model_object(string  _path);

    virtual void update(float dt=0);
    virtual void draw();
};

class shader_object : public xObject{
public:
    //shader_object(){};
    shader_object(string _type);
    virtual void update(float dt=0);
    virtual void draw();
};


class material
{
public:
    material(string _name);
    string name;
    float diffuse[4];
    float specular[4];
    float ambient[4];
    float emission[4];
    float shininess, strength;
    texture textures[5];
};


#endif // MODEL_H
