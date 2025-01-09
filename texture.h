#ifndef TEXTURE_H
#define TEXTURE_H

#include "stable.h"

/*
 *  Qt나 다른 플렛폼에 들어갈수있게 ..할수 있을듯...
 */

//namespace std;

using namespace std;

void makeTexture(SDL_Surface *);

class Texture
{
public:
    Texture(std::string file);
    Texture(int width, int height);
    Texture( SDL_Surface *surface);
    SDL_Surface* getSurface(int width, int height);
    SDL_Surface* LoadImage(std::string file);
    void makeTexture(SDL_Surface *);
    GLuint  getTextureName();
    int d_width,d_height;
    GLuint d_tex_glname;
    string d_filename;
};

class Material
{
public:
    Material(string _name);
    string name;
    float diffuse[4];
    float specular[4];
    float ambient[4];
    float emission[4];
    float shininess, strength;
    Texture *texture;
    GLuint  getTextureName();
};

#endif // TEXTURE_H

