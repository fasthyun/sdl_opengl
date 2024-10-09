#ifndef MODEL_H
#define MODEL_H


#include "object.h"

//bool Import3DFromFile( const std::string &filename);
bool Import3DFromFile(const std::string filename, xObject *obj);


class model_object : public xObject{
public:
    //model_object(){};
    model_object(string  str);

    virtual void update(float dt=0);
    virtual void draw();
};

#endif // MODEL_H
