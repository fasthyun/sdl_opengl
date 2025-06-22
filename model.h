#ifndef MODEL_H
#define MODEL_H


//#include "object.h"

//bool Import3DFromFile( const std::string &filename);
bool Import3DFromFile(const std::string filename, xObject *obj);
void init_models();
void clear_models();
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



#endif // MODEL_H
