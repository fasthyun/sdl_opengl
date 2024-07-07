#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <string>
#include "object.h"

/*
 *  Qt나 다른 플렛폼에 들어갈수있게 ..할수 있을듯...
 */

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
    GLuint d_texname;
};

class texture_manager : public xObject{

    unsigned int VBO, VAO, EBO;
public:
    texture_manager();    
    virtual void update(float dt=0);
    virtual void draw();
    void render_texture(GLuint texname,float x,float y,float z,float size) ;
};
#endif // TEXTURE_H

