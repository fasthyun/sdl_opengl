#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <string>
#include "object.h"

/*
 *  Qt나 다른 플렛폼에 들어갈수있게 ..할수 있을듯...
 */

void makeTexture(SDL_Surface *);

class texture
{
public:
    texture(std::string file);
    texture(int width, int height);
    texture( SDL_Surface *surface);
    SDL_Surface* getSurface(int width, int height);
    SDL_Surface* LoadImage(std::string file);
    void makeTexture(SDL_Surface *);
    GLuint  getTextureName();
    int d_width,d_height;
    GLuint d_tex_glname;
    string d_filename;
};

class texture_object : public xObject{        
public:
    texture_object();
    texture_object(char *texture_filename);
    //void make_glVertexArray();
    virtual void update(float dt=0);
    virtual void draw();
};

class model_object : public texture_object{
public:
    //model_object(){};
    model_object(char *str);
    model_object(string  str);

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
#endif // TEXTURE_H

