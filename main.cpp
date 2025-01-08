//don't need ! #include <SDL2/SDL_opengl.h>
#include "stable.h"
#include "object.h"
#include "console.h"
#include "xmath.h"
//#include "font.h"
#include "texture.h"
#include "Shader.h"
#include "model.h"

#include <cstdlib> // rand()
//#include <complex>

///#includ "misc.h"

//#include "nukclear_pre.h"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_gl3.h"
#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

using namespace std;
vector<xObject* > objects; 
camera *d_camera;
console *d_console;
bool quit;

//The window we'll be rendering to
SDL_Window* sdl_window = nullptr;
SDL_GLContext glContext; //OpenGL context

/* GUI */
struct nk_context *ctx;
// struct nk_colorf bg;
int flag_nk=1;

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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //works , no legacy!
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // maybe better
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); //fail
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Request a color buffer with 8-bits per RGBA channel
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // required
    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Create an application window with the following settings:
    sdl_window = SDL_CreateWindow(
        "An SDL2 window for Development",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        width,                               // width, in pixels 640x480
        height,                               // height, in pixels
        SDL_WINDOW_OPENGL            // flags - see below
                //|SDL_WINDOW_RESIZABLE
    );

    // Check that the window was successfully created
    if (sdl_window == nullptr) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }

    glContext = SDL_GL_CreateContext(sdl_window);

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
    //SDL_SetWindowGrab(window,SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
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

    printf("init_GL() finish...\n");
    return success;
}


void nk_drawMatrix4f(struct nk_context *_ctx, string _label, float *mat)
{
    char str[128];
    nk_layout_row_dynamic(ctx, 10, 1); // ctx, height, cols
    nk_label(_ctx, _label.c_str() , NK_TEXT_LEFT);
    for(int i=0;i < 13;i+=4)
    {
        sprintf(str,"%8.4f,%8.4f,%8.4f,%8.4f",
            mat[i+0],mat[i+1],mat[i+2],mat[i+3]);
            //mat[4],mat[5],mat[6],mat[7],
            //mat[8],mat[9],mat[10],mat[11],
            // mat[12],mat[13],mat[14],mat[15]);
        nk_layout_row_dynamic(ctx, 10, 1); // ctx, height, cols
        nk_label(_ctx, str, NK_TEXT_LEFT);
    }
}

void nk_draw_vector3f(struct nk_context *_ctx, string _label, float v[3])
{
    char str[128];
    sprintf(str,"%s : %8.4f,%8.4f,%8.4f",_label.c_str(),v[0],v[1],v[2]);
    nk_layout_row_dynamic(ctx, 10, 1); // ctx, height, cols
    nk_label(_ctx, str, NK_TEXT_LEFT);
}

void test_print_nk_tree_object(xObject *obj)
{
    if(nk_tree_push_hashed(ctx, NK_TREE_NODE , obj->name.c_str(), NK_MINIMIZED, obj->uuid.c_str(), obj->uuid.length() ,0))
    {
        nk_draw_vector3f(ctx,"force",obj->force);
        //nk_layout_row_dynamic(ctx, 10, 1);
        nk_drawMatrix4f(ctx,"model_mat",obj->model_mat);
        //nk_label(ctx, "Label aligned centered", NK_TEXT_CENTERED);

        for ( size_t i=0 ; i < obj->children.size(); i++)
        {
            xObject *c_obj = obj->children[i];
            //if(nk_tree_push(ctx, NK_TREE_NODE, obj->name.c_str() , NK_MINIMIZED))
            if(nk_tree_push_hashed(ctx, NK_TREE_NODE , c_obj->name.c_str(), NK_MINIMIZED, c_obj->uuid.c_str(), c_obj->uuid.length() ,0))
            {
                //nk_layout_row_dynamic(ctx, 10, 1);
                nk_drawMatrix4f(ctx,"model_mat",obj->model_mat);
                nk_tree_pop(ctx);
            }
        }
        nk_tree_pop(ctx);
    }
}

#include <glm/glm.hpp> // vec2, vec3, mat4, radians
#include <glm/ext.hpp> // perspective, translate, rotate
void main_loop()
{
    //float  time_test=0;
    float  time_fps=0;
    int  fps_count=0;
    int  fps=0;

    Uint32 prevTime;
    Uint32 _now;
    prevTime=SDL_GetTicks(); // init
    float dt=0;

    char str1[30];
    char str2[30];

    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    if (flag_nk){
        ctx = nk_sdl_init(sdl_window);
        struct nk_font_atlas *atlas;
        nk_sdl_font_stash_begin(&atlas);
        /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
        /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
        /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
        /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
        /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
        /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
        nk_sdl_font_stash_end();
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        /*nk_style_set_font(ctx, &roboto->handle);*/
    }


    //SDL_GL_MakeCurrent(window,glContext);    
    while( !quit )
    {
        _now = SDL_GetTicks();
        dt = ( _now - prevTime)/1000.0;
        prevTime= _now;
        //time_test+=dt;
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

        float FoV = 65. ;
        // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
        // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar?
        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        // Far clipping plane. Keep as little as possible.
        glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f );

        glm::mat4 viewMatrix ;
        /* = glm::lookAt(
            glm::make_vec3(d_camera->pos), // the position of your camera, in world space
            glm::make_vec3(d_camera->forward),   // where you want to look at, in world space
            glm::make_vec3(d_camera->up)        // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
        ); */
        // TODO:
        viewMatrix = lookAt_with_glm(d_camera->pos, d_camera->forward , d_camera->up);
        glm::mat4 tmp_mat4 = projectionMatrix * viewMatrix;
        //memcpy(proj_m, glm::value_ptr(tmp_mat4), sizeof( proj_m ));

        for ( size_t i=0 ; i < objects.size(); i++)
        {
            xObject *obj = objects[i];

            obj->update(dt);  //update objects
            if (obj->shader != nullptr)
            {
                obj->shader->SetActive();
                GLint location=glGetUniformLocation(obj->shader->mShaderProgram, "projView"); // testing!
                if (location>=0)
                    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(tmp_mat4)); // ( location, count,  transpose, float *value )

                obj->draw(); //temp
            }
        }

        // Draw top layer : FPS etc
        /*
          glViewport(0,0,width,height);
          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          glMatrixMode(GL_MODELVIEW);
          glLoadIdentity();
        */

        glDisable(GL_DEPTH_TEST);
        if (flag_nk){
            /* Input */
            SDL_Event evt;
            nk_input_begin(ctx);
            while (SDL_PollEvent(&evt)) {
                if (evt.type == SDL_QUIT)
                {
                    quit = true;  // global
                    break;
                }
                if( evt.type == SDL_KEYDOWN)
                {
                    int key=evt.key.keysym.sym;
                    //handleKeys( e.text.text[ 0 ], x, y );
                    if (key == SDLK_ESCAPE)
                    {
                        camera *_camera= (camera *) findObject( "camera_viewer");
                        if(_camera)
                            _camera->d_focus=true;
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    }
                    if (key == SDLK_F4)
                    {
                        quit = true;  // global
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    }

                }
                nk_sdl_handle_event(&evt);
            }
            nk_input_end(ctx);

            static nk_flags window_flags =NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE;

            /* nk Window ? */
            if (nk_begin(ctx, "console", nk_rect(10, 10, 300, 130), window_flags ))
            {                
                nk_layout_row_static(ctx, 30, 80, 1);
                if (nk_button_label(ctx, "button"))    printf("button pressed!\n");
                nk_layout_row_dynamic(ctx, 20, 1);
                //if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
                //if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
                //nk_layout_row_dynamic(ctx, 22, 1);
                //nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
                sprintf(str2, "FPS: %d", fps);
                nk_label(ctx, str2, NK_TEXT_LEFT);
                //nk_layout_row_dynamic(ctx, 20, 1);

                if (nk_tree_push(ctx, NK_TREE_TAB, "Objects", NK_MAXIMIZED)) {
                    //nk_layout_row_dynamic(ctx, 30, 2);
                    for ( size_t i=0 ; i < objects.size(); i++)
                    {
                        xObject *obj = objects[i];
                        test_print_nk_tree_object(obj);
                    }
                    nk_tree_pop(ctx);
                }
            }
            nk_end(ctx);
            //nk_window_is_closed(ctx, "console");
            /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
             * with blending, scissor, face culling, depth test and viewport and
             * defaults everything back into a default state.
             * Make sure to either a.) save and restore or b.) reset your own state after
             * rendering the UI. */
            nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
        }
        // printf("dt=%d \n",dt);


        SDL_GL_SwapWindow( sdl_window );
    } // while

}

void init_object()
{
    //texture *_tex = new texture("font-map.bmp"); // 16x6
    //_tex = new texture("font-map-mtl.png"); // 16x6
    //_tex = new texture("check.bmp"); //
    //_tex = new texture("font-map.png"); // 16x6

    xObject *obj;
    //obj=new xObject();
    //objects.push_back(obj);

    obj = new grid();
    obj->name="grid";
    objects.push_back(obj);

    d_camera = new camera();
    set(d_camera->pos,0,3,7);
    set(d_camera->up,0,1,0); //
    set(d_camera->forward,0,0,-1); //
    objects.push_back(d_camera);

    obj=new console();
    obj->name="console";
    objects.push_back(obj);  

    obj=new CollisionDetector();
    objects.push_back(obj);

    //obj=new texture_manager();
    //objects.push_back(obj);

    //time_fps=0;
    //call_count=0;
    //time_call=0;
    printf("init_object()\n");
}

/*
void init_shader()
{
    shader=new Shader();
    shader->Load("./shader/basic_vertex.glsl","./shader/basic_fragment.glsl");
}*/


int main(int argc, char *argv[])
{
    cout << "SDL(OpenGL) program() !" << endl;
    cout << "sizeof(complex<float>) = "  << sizeof(std::complex<float>) << "bytes" << endl;
    cout << "sizeof(float) = "  << sizeof(float) << "bytes" << endl;
    init_SDL();
    init_GL();


    //init_shader();
    //init_font();
    //init_font_freetype();
    init_object(); 
    init_models();
    main_loop();
    return 0;
}

