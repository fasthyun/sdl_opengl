#include <iostream>
#include <vector>
#include <string>
//#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
//don't need ! #include <SDL2/SDL_opengl.h>
#include <GL/glew.h>
#include "object.h"
#include "console.h"
#include "zmath.h"
#include "font.h"
#include "texture.h"


#define GLT_IMPLEMENTATION
#include "gltext.h" /* https://github.com/vallentin/glText */

#include "Shader.h"
using namespace std;
vector<xObject* > objects; 
camera *d_camera;
console *d_console;
bool quit;

int init_SDL();  //Starts up SDL, creates window, and initializes OpenGL
bool init_GL();  //Initializes matrices and clear color
void close(); //Frees media and shuts down SDL

//The window we'll be rendering to
SDL_Window* window = NULL;
//OpenGL context
SDL_GLContext glContext;
GLuint vshader,fshader;
GLuint program;
Shader *shader;

int width=1024;
int height=768;

int init_SDL()
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
    {
            SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
            return false;
    }
    // Set OpenGL attributes
    // Use the core OpenGL profile
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //works
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    // Specify version 3.3
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
        return 1;
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
    printf("OpenGL loaded\n");
    //gladLoadGLLoader(SDL_GL_GetProcAddress);
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    /*
    // Enable the debug callback over gl4.3
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(this->openglCallbackFunction, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
    */
    // Close and destroy the renderer
    //SDL_DestroyRenderer(renderer);

    SDL_GL_SetSwapInterval(0); // works !! no VSYNC wait


    return 0;
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
        printf( "Error initializing OpenGL! %d\n",  error  );
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

    if (!gltInit())
    {
        printf("Failed to initialize glText\n");
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

    //SDL_GL_MakeCurrent(window,glContext);
    //While application is running
    while( !quit )
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
            fps=fps_count;
            time_fps=0;
            fps_count=0;
        }

        //glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);                
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        //glFrontFace(GL_CCW);
        //shader->SetActive();
        //glEnableClientState(GL_VERTEX_ARRAY);
        /// glEnable(GL_CULL_FACE);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // draw console? or push pop?
        glFrustum(-0.5,0.5,-0.5,0.5,0.5,5000*3);
        LookAt(d_camera->pos, d_camera->forward , d_camera->up);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        for ( size_t i=0 ; i < objects.size(); i++)
        {
            objects[i]->update(dt);  //update objects
            objects[i]->draw();
        }

        // Draw top layer : FPS etc
        glViewport(0,0,width,height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);

        if(0)
        {
            char str[30];
            //gltSetText(text1, "Hello World!");

            GLTtext *text1 = gltCreateText();
            GLTtext *text2 = gltCreateText();

            //render_text("fps= " + std::to_string(fps)  ,-1,0.95);
            gltBeginDraw();
            //gltColor(1.0f, 1.0f, 1.0f, 1.0f);
            //gltDrawText2D(text1, 0.0f, 0.0f, 1.0f); // x=0.0, y=0.0, scale=1.0

            //gltDrawText2DAligned(text1,(GLfloat)(10),(GLfloat)(20),
            //                     3.0f,  GLT_CENTER, GLT_CENTER);
            sprintf(str, "FPS: %d", fps);
            gltSetText(text2, str);
            gltDrawText2DAligned(text2, 1.0f, 30, 1.0f, GLT_LEFT, GLT_BOTTOM);
            gltEndDraw();
        }
        // printf("dt=%d \n",dt);
        SDL_GL_SwapWindow( window );             
    }
    //gltDeleteText(text1);
    //gltDeleteText(text2);
    //gltTerminate();
}

void init_object()
{
    xObject *obj=new xObject();
    objects.push_back(obj);

    obj = new grid();
    objects.push_back(obj);

    d_camera = new camera();
    set(d_camera->pos,0,3,7);
    set(d_camera->up,0,1,0); //
    set(d_camera->forward,0,0,-1); //
    objects.push_back(d_camera);

    obj=new console();
    objects.push_back(obj);

    obj=new texture_manager();
    objects.push_back(obj);

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
    shader=new Shader();
    shader->Load("./shader/basic_vertex.glsl","./shader/basic_fragment.glsl");

    /*
    program=glCreateProgram();
    //glAttachShader(program​, vshader​);
    //char *vs = NULL,*fs = NULL,*fs2 = NULL;
    //vshader = glCreateShader(GL_VERTEX_SHADER);
    //fshader = glCreateShader(GL_FRAGMENT_SHADER);

    shader->CompileShader("./Basic.vert", GL_VERTEX_SHADER, vshader);
    shader->CompileShader("./Basic.frag", GL_FRAGMENT_SHADER, fshader);

    //const char * vv = vs;
    //const char * ff = fs;

    //glShaderSource(vshader, 1, &vv,NULL);
    //glShaderSource(fshader, 1, &ff,NULL);

    //glCompileShader(vshader);
    //glCompileShader(fshader);


    glAttachShader(program,vshader);
    glAttachShader(program,fshader);

    glLinkProgram(program);
    glUseProgram(program);
    */

}
#include <complex>

int main(int argc, char *argv[])
{
    cout << "SDL test program() !" << endl;
    cout << "sizeof(complex<float>) ="  << sizeof(std::complex<float>) << "bytes" << endl;
    init_SDL();
    init_GL();
    init_shader();
    //texture *_tex = new texture("font-map.bmp"); // 16x6
    texture *_tex2 = new texture("check.bmp"); // 16x6
    //init_font();
    //init_font_freetype();
    init_object(); 
    main_loop();
    return 0;
}
