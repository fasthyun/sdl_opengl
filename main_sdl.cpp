#include <iostream>
#include <vector>
#include <string>
//#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <GL/glew.h>
#include "object.h"
#include "console.h"
#include "zmath.h"
#include "font.h"
#include "texture.h"

//#include "Shader.h"
using namespace std;
vector<xObject* > objects; 
camera *d_camera;
console *d_console;
Uint32 prevTime,now;

bool quit;
//Starts up SDL, creates window, and initializes OpenGL
int init_sdl();
//Initializes matrices and clear color
bool init_GL();
//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* window = NULL;
//OpenGL context
SDL_GLContext glContext;
GLuint vshader,fshader;
GLuint program;
//Shader *shader;

extern int key_foward;
extern int key_backward;
extern int key_side_right;
extern int key_side_left;

int width=1024;
int height=768;

int init_sdl()
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
    {
            SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
            return false;
    }
    // Set OpenGL attributes
    // Use the core OpenGL profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //works
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
        "An SDL2 window",                  // window title
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

    SDL_GL_SetSwapInterval(0); // works !! no VSYNC
    key_foward=SDLK_w;
    key_backward=SDLK_s;
    key_side_right=SDLK_d;
    key_side_left=SDLK_a;
    prevTime=SDL_GetTicks(); // init
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
    
    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        //printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        //printf( "Error initializing OpenGL! %d\n",  error  );
        success = false;
    }

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
       // printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
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
    
    return success;
}


float  time_test=0;
float  time_fps=0;
int  fps_count=0;
int  fps=0;
void main_loop()
{
    //SDL_GL_MakeCurrent(window,glContext);
    //Event handler
    SDL_Event e;
    float dt=0;

    //While application is running
    while( !quit )
    {
        now=SDL_GetTicks();
        dt = (now -prevTime)/1000.0;
        prevTime=now;
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
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            //Handle keypress with current mouse position
            else if( e.type == SDL_KEYDOWN)
            {
                int key=e.key.keysym.sym;
                //int scancode=e.key.keysym.scancode;
                /* SDLK_UP , SDLK_W */
                //SDL_GetMouseState( &x, &y );
                //handleKeys( e.text.text[ 0 ], x, y );

                if (key == SDLK_ESCAPE)
                    quit=true;
                
                d_camera->on_key_pressed(key);
                printf("keycode=%d \n",key);
            }
            else if( e.type == SDL_KEYUP)
            {
                int key=e.key.keysym.sym;
                d_camera->on_key_released(key);
            }
            else if( e.type == SDL_MOUSEMOTION)
            {
                int dx=e.motion.xrel;
                int dy=e.motion.yrel;
                if (e.motion.state==SDL_BUTTON_LMASK)
                    d_camera->on_mouse_moved(dx,dy);
            }   
        }
        //update_objects
        for ( size_t i=0 ; i < objects.size(); i++)
        {
            objects[i]->update(dt);
        }

        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);                
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        //glFrontFace(GL_CCW);
        glEnableClientState(GL_VERTEX_ARRAY);
        //glEnable(GL_CULL_FACE);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();        
        // draw console? or push pop?
        glFrustum(-1,1,-1,1,1,5000*3);
        LookAt(d_camera->pos, d_camera->forward , d_camera->up);
        glMatrixMode(GL_MODELVIEW);
        //shader->SetActive();
        glLoadIdentity();
        for ( size_t i=0 ; i < objects.size(); i++)
        {
            objects[i]->draw();
        }

        // temp
        glViewport(0,0,width,height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        render_text("fps= " + std::to_string(fps)  ,-1,0.95);
        // printf("dt=%d \n",dt);
        SDL_GL_SwapWindow( window );             
    }

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

    obj= new xObject();
    obj->load_gltf("/home/hyun/works/qt_opengl/data/DamagedHelmet.gltf");
    objects.push_back(obj);

    //time_fps=0;
    //call_count=0;
    //time_call=0;
}
void init_shader()
{
  //  shader=new Shader();
  //  shader->Load("./Basic.vert","./Basic.frag");

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
    cout << sizeof(std::complex<float>) << endl;
    init_sdl();
    init_GL();
    //init_shader();
    init_font();
    init_font_freetype();
    init_object(); 
    main_loop();
    return 0;
}
