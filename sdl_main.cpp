#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h> //
#include "object.h"
#include "zmath.h"

using namespace std;
vector<xObject* > objects; 
camera *d_camera;


Uint32 prevTime,now;
bool quit;
//Starts up SDL, creates window, and initializes OpenGL
int init_sdl();
//Initializes matrices and clear color
bool initGL();
//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* window = NULL;
//OpenGL context
SDL_GLContext glContext;


extern int key_foward;
extern int key_backward;
extern int key_side_right;
extern int key_side_left;

int width=1024;
int height=768;

int init_sdl()
{
    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL2 window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        width,                               // width, in pixels 640x480
        height,                               // height, in pixels
        SDL_WINDOW_OPENGL |SDL_WINDOW_RESIZABLE             // flags - see below
    );

    // Check that the window was successfully created
    if (window == nullptr) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    glContext = SDL_GL_CreateContext(window);
    /* auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /if(renderer == nullptr)
	return -1;

     SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
     SDL_RenderClear(renderer);
     SDL_RenderPresent(renderer);
     */
    // The window is open: could enter program loop here (see SDL_PollEvent())

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
    glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true
            );
            */
    // Close and destroy the renderer
    //SDL_DestroyRenderer(renderer);
    //
    key_foward=SDLK_w;
    key_backward=SDLK_s;
    key_side_right=SDLK_d;
    key_side_left=SDLK_a;
    prevTime=SDL_GetTicks(); // init
    return 0;
}

bool initGL()
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
        printf( "Error initializing OpenGL! %d\n",  error  );
        success = false;
    }

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %d\n",  error  );
        success = false;
	}
  //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );
    
    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %d\n",  error  );
        success = false;
    }
    
    return success;
}


float  time_test=0;
void sdl_loop()
{
    //SDL_GL_MakeCurrent(window,glContext);
	//Event handler
	SDL_Event e;
    float dt;
	//While application is running
	while( !quit )
    {
        now=SDL_GetTicks();
        dt = (now -prevTime)/1000.0;
        prevTime=now;
        time_test+=dt;
        if (time_test > 1)
        {
            printf("dt=%f \n",dt);
            time_test=0;
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
        glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-1,1,-1,1,1,5000*3);
        LookAt(d_camera->pos, d_camera->forward , d_camera->up);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        for ( size_t i=0 ; i < objects.size(); i++)
        {
            objects[i]->draw();
        }
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
    
    //time_fps=0;
    //call_count=0;
    //time_call=0;
}

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;

    init_sdl();
    initGL();
    init_object(); 
    sdl_loop();
    return 0;
}
