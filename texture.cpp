#include "stable.h"
#include "texture.h"

std::vector<texture *> textures;



/*
SDL_Texture *loadTexture(const char *file){
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = IMG_Load(file);
    if (surface == NULL){
        printf("fail to read %s\n", file);
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL){
        printf("unable to create texture.\n");
    }
    SDL_FreeSurface(surface);

    return texture;
} */




SDL_Surface* texture::LoadImage(std::string file)
{
    SDL_Surface *surface = nullptr;
    //surface = SDL_LoadBMP(file.c_str());
    surface = IMG_Load(file.c_str());

    if (surface != nullptr)
    {
        //texture = SDL_CreateTextureFromSurface(renderer, loadedImage); //hmmm
        //SDL_FreeSurface(loadedImage);
        return surface;
    }
    else
    {
        std::cout << SDL_GetError() << std::endl;
        return 0 ;
    }
}

#include <boost/algorithm/string/replace.hpp> // replace_all()
#include <boost/filesystem.hpp>
//std::string base_filename = path.substr(path.find_last_of("/\\") + 1)

texture::texture(std::string _path)
{
    d_tex_glname = -1;    

    //boost::replace_all(_path, "\\", "/");
    //boost::replace_all(_path, "//", "/");
    string  _filename=boost::filesystem::path(_path).filename().string();  // basename!

    auto path="./texture/"+_filename;
    std::cout << "texture(): " << path  << "\n";
    SDL_Surface *surface = IMG_Load(path.c_str()); //SDL_Surface *surface=LoadImage(path);
    if (surface == NULL) {
        std::cout << "Error: fail read " << path  << "\n";
        return;
    }
    makeTexture(surface);
    SDL_FreeSurface(surface);  // free surface !
    //textures.push_back(this);
    d_filename=_filename;
}

texture::texture(int width, int height)
{
    d_tex_glname = -1;
    // not a file
    SDL_Surface*surface = getSurface(width,height);
    makeTexture(surface);
    //textures.push_back(this);
    SDL_FreeSurface(surface);
}

texture::texture(SDL_Surface *surface)
{
    d_tex_glname=-1;
    // not a file
    makeTexture(surface);
    //textures.push_back(this);
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
           return 0;
    }
    return surface;
}

GLuint texture::getTextureName()
{
    return d_tex_glname;
}

void texture::makeTexture(SDL_Surface *surface)
{
    /*
       * samples :
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Binding of texture name
        glBindTexture(GL_TEXTURE_2D, textureIds[i]);

        // Texture specification
        glTexImage2D(GL_TEXTURE_2D, 0, n, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);// Texture specification.

        // we also want to be able to deal with odd texture dimensions
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
        glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
        glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
        glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
    */

    d_width = surface->w;
    d_height = surface->h;

    int _bytes = surface->format->BytesPerPixel;
    glGenTextures(1, &d_tex_glname);
    glBindTexture(GL_TEXTURE_2D, d_tex_glname);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // power of 2 !!, need in AMD!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repeat ok!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // We will use linear interpolation for minifying filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // We will use linear interpolation for magnification filter

    if(_bytes == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, d_width, d_height, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    }
    else if(_bytes == 4)
    {
        SDL_Surface *output;
        if (surface->format->format==SDL_PIXELFORMAT_RGBA32)
        {
            output=surface;
            //std::cout << "\n";
        }
        else{
            // SDL_PIXELFORMAT_BGRA32, SDL_PIXELFORMAT_ARGB32, SDL_PIXELFORMAT_ABGR32
            // SDL_PIXELFORMAT_RGBA8888
            std::cout << "  ===> Not RGBA32: " <<  SDL_GetPixelFormatName(surface->format->format)  ;
            SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            output = SDL_ConvertSurface(surface, format, 0);
            std::cout << " ===> result RGBA32 ? :" << (output->format->format==SDL_PIXELFORMAT_RGBA32) << endl;
            SDL_FreeFormat(format); // remove a leak!
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, d_width, d_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, output->pixels);
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); //?
    glGenerateMipmap(GL_TEXTURE_2D); // works???

    //printf("\n ===> bytes=%d format=%s  width=%d height=%d \n",_bytes,SDL_GetPixelFormatName(surface->format->format),d_width,d_height)  ;
    glBindTexture(GL_TEXTURE_2D, 0); //ok!
}


texture_manager::texture_manager()
{
    shader=new Shader();
    shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");

}

void texture_manager::update(float dt)
{

}


// TODO
void texture_manager::render_texture(GLuint texname,float x,float y,float z,float size)
{

}

GLuint texture_manager::get_glname(string filename)
{
    GLuint texname = 0 ;
    // cout << "1. get_glname => "<< filename << "," << to_string(texname) << "\n";
    for ( size_t i=0 ; i < textures.size(); i++)
    {
        if(textures[i]->d_filename==filename)
        {
            texname=textures[i]->getTextureName();
            cout << " texname==>" << texname << "\n";
            break;
        }
    }
    // cout << "2. get_glname => "<< filename << "," << to_string(texname)  << "\n";
    if (texname == 0)
    {
        texname=load_texture(filename);
    }

    return texname;
}

GLuint texture_manager::load_texture(string path)
{
    std::string basename = path.substr(path.find_last_of("/\\") + 1);

    cout << "load_texture "<< basename << "\n;";
    texture *_tex = new texture(basename);
    if (_tex->getTextureName()>0)
        textures.push_back(_tex);
    return _tex->getTextureName();
}


void texture_manager::draw()
{
   /* for ( size_t i=0 ; i < 1; i++)
    {
        //render_texture(texname,i*5,0,0,5);
        GLuint texname=textures[i]->getTextureName();
        glBindTexture(GL_TEXTURE_2D, texname);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    */
}


texture_object::texture_object(): xObject()
{

}

texture_object::texture_object(char *filename): xObject()
{
    shader=new Shader();
    shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");
    make_cube(vertexes,triangles,1);
    make_glVertexArray(); // works!!!    
    texname=texture_manager::get_glname(filename);
    /// printf("VAO=%d, texname = %d , sizeof(vertex)= %d, data=%x\n",VAO, texname, sizeof(vertex), vertexes.data());
}


void texture_object::update(float dt)
{

}

void texture_object::draw()
{
    xObject::draw();
}

