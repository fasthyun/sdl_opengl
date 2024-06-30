#include <vector>
#include "texture.h"
#include <iostream>
#include <SDL2/SDL_opengl.h>
// #include <SDL2/SDL_image.h>

std::vector<texture *> textures;

texture::texture(std::string file)
{
    d_texname=-1;
    // Temporary
    if (file=="")
    {
        return;
    }
    auto path="/home/hyun/works/sdl_opengl/texture/"+file;
    std::cout << "texture(): " << path;
    SDL_Surface *surface=LoadImage(path);
    if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }
    makeTexture(surface);
    SDL_FreeSurface(surface);  // free surface !
    textures.push_back(this);
}

texture::texture(int width, int height)
{
    d_texname=-1;
    // not a file
    SDL_Surface*surface=getSurface(width,height);
    makeTexture(surface);
    textures.push_back(this);
    SDL_FreeSurface(surface);
}


texture::texture( SDL_Surface *surface)
{
    d_texname=-1;
    // not a file
    makeTexture(surface);
    textures.push_back(this);
}

SDL_Surface* texture::getSurface(int width, int height)
{
    SDL_Surface *surface;
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
/* or using the default masks for the depth: */
    surface = SDL_CreateRGBSurface(0, width, height, 32,
                                   rmask, gmask, bmask, amask);

    //surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (surface == NULL) {
           SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
           exit(1);
    }
    return surface;
}

SDL_Surface* texture::LoadImage(std::string file)
{
    SDL_Surface *loadedImage = nullptr;    
    loadedImage = SDL_LoadBMP(file.c_str());
    /* imgBuff = stbi_load("/home/hyun/works/qt_opengl/font-map.png", &w, &h, &comp, 0);
       if(imgBuff == nullptr){
        throw(std::string("Failed to load texture"));
        return;
    }*/
    if (loadedImage != nullptr)
    {
        //texture = SDL_CreateTextureFromSurface(renderer, loadedImage); //hmmm
        //SDL_FreeSurface(loadedImage);
        return loadedImage;
    }
    else
    {
        std::cout << SDL_GetError() << std::endl;
        return 0 ;
    }
}

GLuint texture::getTextureName()
{
    return d_texname;
}

void texture::makeTexture(SDL_Surface *surface)
{
    /*
    samples :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glGenerateMipmap(GL_TEXTURE_2D);
    */
    d_width=surface->w;
    d_height=surface->h;
    int _bytes = surface->format->BytesPerPixel;
    glGenTextures(1, &d_texname);
    glBindTexture(GL_TEXTURE_2D, d_texname);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // ???
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(_bytes == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, d_width, d_height, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    else if(_bytes == 4)
    {
        SDL_Surface *output;
        if (surface->format->format==SDL_PIXELFORMAT_RGBA32)
        {
            output=surface;
        }
        else{
            // SDL_PIXELFORMAT_BGRA32, SDL_PIXELFORMAT_ARGB32, SDL_PIXELFORMAT_ABGR32

            std::cout << "  ===> Not RGBA32: " <<  SDL_GetPixelFormatName(surface->format->format)  ;

            SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            output = SDL_ConvertSurface(surface, format, 0);            
            std::cout << " ===> result RGBA? :" << (output->format->format==SDL_PIXELFORMAT_RGBA32) << endl;
            SDL_FreeFormat(format); // remove a leak!
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, d_width, d_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    }
    glBindTexture(GL_TEXTURE_2D, 0); //ok!
}


// TODO
void render_texture(GLuint texname,float x,float y,float z,float size) {
    glBindTexture(GL_TEXTURE_2D, texname);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(x, y , z);
    glTexCoord2f(0.0, 1.0); glVertex3f(x, y + size, z);
    glTexCoord2f(1.0, 1.0); glVertex3f(x+size, y+size, z);
    glTexCoord2f(1.0, 0.0); glVertex3f(x+size, y, z);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0); //!!!
}

texture_manager::texture_manager()
{

}

void texture_manager::update(float dt)
{

}

void texture_manager::draw()
{
    for ( size_t i=0 ; i < textures.size(); i++)
    {
        GLuint texname=textures[i]->getTextureName();
        render_texture(texname,i*5,0,0,5);
    }
}
