#ifndef CONSOLE_H
#define CONSOLE_H


#include "object.h"
#include "font.h"

class console : public xObject{

public:
    console();
    virtual void update(float dt=0);
    virtual void draw();
    //void print
    void put(char *);
};
#endif // CONSOLE_H
