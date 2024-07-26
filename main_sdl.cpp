#include <iostream>
#include <vector>
#include <string>
//#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//don't need ! #include <SDL2/SDL_opengl.h>
#include <GL/glew.h>
#include "object.h"
#include "console.h"
#include "zmath.h"
#include "font.h"
#include "texture.h"
#include "Shader.h"
#include "model.h"
//#define GLT_IMPORTS
//#define GLT_IMPLEMENTATION //?????
#include "gltext.h"         /* https://github.com/vallentin/glText */
//#include "misc.h"

using namespace std;
vector<xObject* > objects; 
camera *d_camera;
console *d_console;
bool quit;

bool init_SDL();  //Starts up SDL, creates window, and initializes OpenGL
bool init_GL();  //Initializes matrices and clear color

//The window we'll be rendering to
SDL_Window* window = NULL;
SDL_GLContext glContext; //OpenGL context

int width=1024;
int height=768;

bool init_SDL()
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
    {
            SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
            return false;
    }
    // Set OpenGL attributes
    // Use the core OpenGL profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //works
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // maybe better
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); //fail
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // Request a color buffer with 8-bits per RGBA channel
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL2 window for Development",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        width,                               // width, in pixels 640x480
        height,                               // height, in pixels
        SDL_WINDOW_OPENGL            // flags - see below
                //|SDL_WINDOW_RESIZABLE
    );

    // Check that the window was successfully created
    if (window == nullptr) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }

    glContext = SDL_GL_CreateContext(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
            SDL_Log("Failed to initialize GLEW.");
            return false;
    }

    // Check OpenGL properties

    //gladLoadGLLoader(SDL_GL_GetProcAddress);
    /*
        Vendor graphic card: NVIDIA Corporation
        Renderer: GeForce GT 635/PCIe/SSE2
        Version GL: 4.6.0 NVIDIA 388.13
        Version GLSL: 4.60 NVIDIA
    */
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version GL:  %s\n", glGetString(GL_VERSION));
    printf("Version GLSL:  %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Enable the debug callback over gl4.3
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);    
    //glDebugMessageCallback(GLDebugMessageCallback, NULL);
    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);

    // Close and destroy the renderer
    //SDL_DestroyRenderer(renderer);

    SDL_GL_SetSwapInterval(0); // works !! no VSYNC wait


    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    }

    printf("SDL(OpenGL) loaded\n");
    return true;
}

bool init_GL()
{
    bool success = true;
    GLenum error = GL_NO_ERROR;

    glViewport(0,0,width,height);
    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();    
    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        printf( "Error initializing OpenGL! %d\n", error  );
        success = false;
    }

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
       // printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        printf( "Error initializing OpenGL! %d\n",  error  );
        success = false;
    }   

    if (!gltInit()) // glText font init!
    {
        printf("ERROR : Failed to initialize glText\n");
        success = false;
    }

    printf("init_GL() finish...\n");
    return success;
}


float  time_test=0;
float  time_fps=0;
int  fps_count=0;
int  fps=0;


void main_loop()
{
    Uint32 prevTime;
    Uint32 _now;
    prevTime=SDL_GetTicks(); // init
    float dt=0;

    GLTtext *text1 = gltCreateText();
    GLTtext *text2 = gltCreateText();
    GLTtext *text3 = gltCreateText();
    GLTtext *text4 = gltCreateText();

    char str1[30];
    char str2[30];

    float proj_m[16];
    float model_m[16];

    //SDL_GL_MakeCurrent(window,glContext);    
    while( !quit ) //While application is running
    {
        _now = SDL_GetTicks();
        dt = ( _now - prevTime)/1000.0;
        prevTime= _now;
        time_test+=dt;
        time_fps+=dt;
        fps_count++;

        if (time_fps > 1)
        {
            ///printf("dt=%f \n",dt);
            fps = fps_count;
            time_fps = 0;
            fps_count=0;
        }

        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);                
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glFrontFace(GL_CCW);
        //glEnable(GL_CULL_FACE);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // draw console? or push pop?
        glFrustum(-0.5,0.5,-0.5,0.5,0.5,500*3);
        LookAt(d_camera->pos, d_camera->forward , d_camera->up);
        /* gluLookAt(d_camera->pos[0],d_camera->pos[1],d_camera->pos[2],
                  d_camera->pos[0]+d_camera->forward[0]*3,d_camera->pos[1]+d_camera->forward[1]*3 ,
                  d_camera->pos[2]+d_camera->forward[2]*3,
                  d_camera->up[0],d_camera->up[1],d_camera->up[2]); */

        glGetFloatv(GL_PROJECTION_MATRIX, proj_m);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        for ( size_t i=0 ; i < objects.size(); i++)
        {
            xObject *obj = objects[i];
            //glLoadIdentity();
            //glGetFloatv(GL_MODELVIEW_MATRIX, model_m);
            //translate(model_m, obj->pos[0], obj->pos[1], obj->pos[2] );

            if (obj->shader != NULL)
            {
                obj->shader->SetActive();
                GLint location=glGetUniformLocation(obj->shader->mShaderProgram, "projView");
                if (location>=0)
                    glUniformMatrix4fv(location, 1, GL_FALSE, proj_m); // GLint location,GLsizei count, GLboolean transpose, const GLfloat *value

                /*location=glGetUniformLocation(obj->shader->mShaderProgram, "modelView");
                //sprintf(str1, "location: %d", location);
                if (location>=0)
                    glUniformMatrix4fv(location, 1, GL_FALSE, model_m); */
            }
            obj->update(dt);  //update objects
            obj->draw();
        }

        // Draw top layer : FPS etc
        /* glViewport(0,0,width,height);
        //glMatrixMode(GL_PROJECTION);
        //glLoadIdentity();
        //glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();
         glDisable(GL_DEPTH_TEST);
        */

        if(true)
        {
            //render_text("fps= " + std::to_string(fps)  ,-1,0.95);
            gltBeginDraw();
            /*
            gltSetText(text1, "Hello World!");
            gltColor(1.0f, 1.0f, 1.0f, 1.0f);
            gltDrawText2D(text1, 0.0f, 0.0f, 1.0f); // x=0.0, y=0.0, scale=1.0
            gltDrawText2DAligned(text1,(GLfloat)(10),(GLfloat)(20),
                                 3.0f,  GLT_CENTER, GLT_CENTER);
            */            
            sprintf(str2, "FPS: %d", fps);
            gltSetText(text2, str2);
            gltDrawText2DAligned(text2, 10.0f, 20, 1.0f, GLT_LEFT, GLT_BOTTOM);

            gltSetText(text1, str1);
            gltDrawText2DAligned(text1, 10.0f, 40, 1.0f, GLT_LEFT, GLT_BOTTOM);

            gltDrawMatrix4f(text3 ,proj_m , 10, 60);
            gltEndDraw();
        }
        // printf("dt=%d \n",dt);
        SDL_GL_SwapWindow( window );
        //SDL_WarpMouseInWindow(window,width/2,height/2);
    }
    //gltDeleteText(text1);
    //gltDeleteText(text2);
    //gltTerminate();
}

void init_object()
{
    texture *_tex = new texture("font-map.bmp"); // 16x6
    _tex = new texture("font-map-mtl.png"); // 16x6
    _tex = new texture("check.bmp"); //
    _tex = new texture("font-map.png"); // 16x6

    xObject *obj;
    //obj=new xObject();
    //objects.push_back(obj);

    obj = new grid();
    objects.push_back(obj);

    d_camera = new camera();
    set(d_camera->pos,0,3,7);
    set(d_camera->up,0,1,0); //
    set(d_camera->forward,0,0,-1); //
    objects.push_back(d_camera);

    obj=new console();
    objects.push_back(obj);

    xObject *texobj;//=new texture_object("check.bmp");
    //set(texobj->pos,-2,0,0);
    //objects.push_back(texobj);

    //texobj=new texture_object("font-map-mtl.png");
    //set(texobj->pos,2,0,0);
    //objects.push_back(texobj);

    //texobj=new texture_object("font-map.png");
    //set(texobj->pos,4,0,0);
    //objects.push_back(texobj);

    xObject *model_obj=new model_object((char*)"");
    set(model_obj->pos,0,10,0);
    objects.push_back(model_obj);


  //  obj=new texture_manager();
  //  objects.push_back(obj);

    //obj= new xObject();
    //obj->load_gltf("/home/hyun/works/sdl_opengl/data/DamagedHelmet.gltf");
    //objects.push_back(obj);
    //time_fps=0;
    //call_count=0;
    //time_call=0;
    printf("init_obect())\n");
}

void init_shader()
{
    /*
    shader=new Shader();
    shader->Load("./shader/basic_vertex.glsl","./shader/basic_fragment.glsl");
    */
}

#include <complex>

int main(int argc, char *argv[])
{
    cout << "SDL(OpenGL) program() !" << endl;
    cout << "sizeof(complex<float>) = "  << sizeof(std::complex<float>) << "bytes" << endl;
    init_SDL();
    init_GL();
    //init_shader();
    //init_font();
    //init_font_freetype();
    init_object(); 
    main_loop();
    return 0;
}

