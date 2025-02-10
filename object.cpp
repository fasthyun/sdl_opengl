#include "stable.h"
#include "object.h"
#include "xmath.h"

#include <cstddef> // offsetof()

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.  to_string()

#include <glm/gtc/type_ptr.hpp> // make_mat4()
#include <glm/gtc/matrix_transform.hpp> // rotate()
//#include <glm/gtx/transform.hpp> //rotate

using namespace std::chrono;
// 이걸 왜 작성했지????
milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch() );

uint64_t epoch_now() { // timeSinceEpochMillisec
    // 좀 느릴거 같은뎅..(hyun) 이걸 왜 만들었지?
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

extern vector<xObject* > objects;
xObject* findObject(string _name)
{
    for ( size_t i=0 ; i < objects.size(); i++)
    {
        xObject *obj = objects[i];
        if (obj->name == _name)
            return obj;
    }
    return nullptr;
}

xObject::xObject()
{
    boost::uuids::uuid _uuid = boost::uuids::random_generator()(); // get uuid
    uuid=to_string(_uuid);
    //std::cout << "uuid=" << uuid << std::endl;

    parent=nullptr;
    shader=nullptr; // ***
    VBO=0;
    VAO=0;
    EBO=0;
    //shader=new Shader();
    //shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");
    //meshes=NULL;
    //set(pos,0,0,0);
    position.x=0;position.y=0;position.z=0;
    set(up ,0,1,0);
    set(forward,0,0,1);
    set(force,0,0,0);
    //make_circle();
    // loadIdentity(model_mat);
    model = glm::mat4(1);
    new_force = glm::vec3(0);
    name="None";
    make_axis();
}


xObject::xObject(const xObject &obj)
{
    uuid=obj.uuid;
    parent=obj.parent;
}

void xObject::copyModel(xObject *obj) // 임시 생성!
{
    model = obj->model;
    VBO=obj->VBO;
    VAO=obj->VAO;
    EBO=obj->EBO;
    material=obj->material;
    vertexes=obj->vertexes;
    triangles=obj->triangles;
    texname=obj->texname;
    children=obj->children;
}

void xObject::copy(xObject *obj) // 임시 생성!
{
///  uuid=obj->uuid;
    name=obj->name;

    parent=obj->parent;
    shader=obj->shader; // ***

    position=obj->position;
    //set(up ,0,1,0);
    //set(forward,0,0,1);
    //set(force,0,0,0);    
    new_force = obj->new_force;
    new_forward = obj->new_forward;
    new_up = obj->new_up;

    copyModel(obj);
    make_axis();
    make_glVertexArray();
}

xObject::xObject(xObject *obj)
{
    xObject::copy(obj);
}

xObject::xObject(xObject *&obj)
{
    uuid=obj->uuid;
    parent=obj->parent;

}


xObject::~xObject(){
    if (parent !=nullptr)
    {
        delete parent;
        parent = nullptr;
    }
    if (shader !=nullptr)
    {
        delete shader;
        shader = nullptr;
    }
}



void xObject::update(float dt /* ms seconds */)
{
    float v[3];
    glm::vec3 force(0);
    //multiply(force, dt, v);
    //add(pos, v, pos);

    if (flag_gravity)
    {
        force += glm::vec3(0,-980,0);
    }

    force+=new_force;

    position += force * dt;


}


void xObject::draw_dir_up()
{
    float len=3;
    float t[3];
}


void xObject::make_axis()
{
    float axies[] = {
        0.0f,0.0f,0.0f,  1,0,0,1,   // x line red
        100.0f,0.0f,0.0f,  1,0,0,1,   // x line red
        0.0f,0.0f,0.0f,  0,1,0,1,   // y line gree
        0.0f,100.0f,0.0f,  0,1,0,1,   // y line gree
        0.0f,0.0f,0.0f,  0,0,1,1,   // z line  blue
        0.0f,0.0f,100.0f,  0,0,1,1,   // z line  blue
        };

    glGenVertexArrays(1, &VAO_axis);
    glGenBuffers(1, &VBO_axis);

    glBindVertexArray(VAO_axis);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axies), axies, GL_STATIC_DRAW);

    // 이렇게도 값 전달이 된다. 띄엄띄엄
    glEnableVertexAttribArray(0); //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)0); // position[3]   // (idx, size, type, ? , stride-size, offset)
    glEnableVertexAttribArray(2); //
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)(3 * sizeof(float))); // RGBA

    glBindVertexArray(0); // break
}
// TODO: name

void xObject::make_glVertexArray()
{
    if (triangles.size()==0)
        return;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertexes.size()*sizeof(vertex), vertexes.data(), GL_STATIC_DRAW);
    ///printf("sizeof(vertex)= %ld vertexs.size=%ld data=%p\n", sizeof(vertex), vertexes.size(), vertexes.data());
    //printf(" why??? %f %f %f %f %f\n", vertexes.data()[0].v[0],vertexes.data()[0].v[1],vertexes.data()[0].v[2],vertexes.data()[0].v[3], vertexes.data()[4]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size()*sizeof(triangle) ,triangles.data(), GL_STATIC_DRAW);

    // VBO버퍼 구성 설명
    glEnableVertexAttribArray(0); //  first input to vertex-shader?
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex,v)); // pos[3]  (idx, size, type, ? , stride-size, offset)
    glEnableVertexAttribArray(1); //  second input to vertex-shader?
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex,tu)); // tu,tv    // 3*4 = 12, 16 ok...
    glEnableVertexAttribArray(2); //  third input to vertex-shader?
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex,r)); // RGBA
    glEnableVertexAttribArray(3); //  fourth input to vertex-shader?
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex,normal)); // normal[3]
    //glVertexAttribPointer(4, 1, GL_FLOAT,GL_FALSE, sizeof(vertex), (void*)offsetof(vertex,type)); // type : 1 color 0 texture
    glVertexAttribIPointer(4, 1, GL_INT, sizeof(vertex), (void*)offsetof(vertex,type)); // type : 1 color 0 texture works!!when intger!!!
    glEnableVertexAttribArray(4); //  fifth input to vertex-shader?

    //DEL printf("offsetof(v)=%ld offsetof(tu)=%ld offsetof(r)=%ld offsetof(normal)=%ld offsetof(type)=%ld \n",offsetof(vertex,v),offsetof(vertex,tv),offsetof(vertex,r),offsetof(vertex,normal),offsetof(vertex,type));
    glBindVertexArray(0); // break
}


void xObject::make_radius()
{
    // bound-box-radius , temporaliry
    vertex *t;
    vertex orig={0,0,0,0,0,0,0,0,0,0,0,0,0};
    float r=0,max_r=0;
    for ( int i =0 ;i < vertexes.size() ; i++)
    {
        t = &vertexes[i];
        r = vect3f_distance(t->v, orig.v);
        if ( r > max_r )
            radius=r;
    }
}

void xObject::check_collision()
{

    xObject *other;
    for ( size_t i=0 ; i < objects.size(); i++)
    {
        other = objects[i];
        if (this != other )
        {


        }
    }
}


void xObject::draw_axis()
{
    /* https://stackoverflow.com/questions/60440682/drawing-a-line-in-modern-opengl
     */
    if (flag_axis_on && VAO_axis>0)
    {
        glDisable(GL_DEPTH_TEST);    // Enable depth buffering
        shader->setMat4("model",model); //
        glLineWidth(3);
        glBindVertexArray(VAO_axis);
        //glVertexAttrib3f(vertColor_loc, 1.0f, 1.0f, 0.2f);
        glVertexAttribI1i(4, 2); //glVertexAttrib1f(4, 2);
        glDrawArrays(GL_LINES, 0, 6); // why 6?
    }

}


void xObject::draw_meshes()
{
    // will not work ! (legacy)
}


void xObject::update_model_matrix()
{
        model = glm::mat4(1.0); // identity
        //model=glm::rotate(model, roll , glm::vec3(0,0,1));//z
        //model=glm::rotate(model, yaw , glm::vec3(0,1,0)); //y
        //model=glm::rotate(model, pitch , glm::vec3(1,0,0)); //x
        //model=glm::scale(model, glm::vec3(scale[0],scale[1],scale[2]));
        glm::mat4 Mi = glm::mat4(1.0f);
        //glm::mat4 RotationMatrix = glm::mat4(1.0);
        glm::mat4 RotationMatrix = glm::rotate(Mi, glm::radians(yaw), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 TranslationMatrix = glm::translate(model, position);
        glm::mat4 ScaleMatrix = glm::mat4(1.0);
        /* TransformedVector = TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVertex; */
        model= TranslationMatrix * RotationMatrix * ScaleMatrix; // works!!!
}

#include <glm/gtc/matrix_transform.hpp>
void xObject::draw()
{    
    //draw_dir_up();
    //draw_meshes();
    //mat4x4_translate(model_mat, pos[0], pos[1], pos[2]);

    //glm::mat4  m1(1.0);
    //glm::mat4  m2(1.0);
    //glm::translate(_m1, glm::vec3(pos[0], pos[1], pos[2]));
    //m1[3].x =pos[0];    m1[3].y =pos[1];    m1[3].z =pos[2];
    //m1[3].x =pos[0];    m1[3].y =pos[1];    m1[3].z =pos[2];

    update_model_matrix();
    glm::mat4 _m1(1);
    float _m[16];
    if (parent)
    {
        loadIdentity(_m);
        //set(_m,parent->model_m);
        //mat4x4_mult(_m, parent->model_mat, model_mat);
        _m1=parent->model * _m1;

    }
    else _m1 = model;
    //else mat4x4_set(_m, model_mat); // copy

    //glm::mat4 _m1 = glm::make_mat4(_m);

    if (texname > 0)
        glBindTexture(GL_TEXTURE_2D, texname);

    if (VAO>0)
    {
        glEnable(GL_DEPTH_TEST);    // Enable depth buffering
        shader->setMat4("model",_m1);
        /* GLint location;
          location = glGetUniformLocation(shader->mProgram, "model");
          if (location >= 0)
            glUniformMatrix4fv(location, 1, GL_FALSE, _m); // ( location, count,  transpose, float *value )

            See OpenGL 4.6 API Compatibility Profile Specification; 7.10 Samplers; page 154:
            Samplers are special uniforms used in the OpenGL Shading Language to identify
            the texture object used for each texture lookup. The value of a sampler
            indicates the texture image unit being accessed.
            Setting a sampler’s value to i selects texture image unit number i.
        */

        GLint location = glGetUniformLocation(shader->mProgram, "ourTexture");
        if(location >=0) glUniform1i(location, 0); // 0: GL_TEXTURE0 works!!! 0값이 전달될때 sampler2D로 변환되는것 같다.
        location = glGetUniformLocation(shader->mProgram, "material.emission");
        if(location >=0) glUniform3fv (location, 1, material->emission );

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, triangles.size() *3 /* index count so x3 */ , GL_UNSIGNED_INT, 0); //( mode, count, index_data_type, void * indices);
        /* same as
           for(i =0 ;i < len ; i++)
              glArrayElement(i);
           --- or ---
           glDrawArrays(GL_TRIANGLES, 0, vertexes.size());
        */
    }


   draw_axis();

   for ( size_t i=0 ; i < children.size(); i++)
    {
        // Parent shader is active
        xObject *obj = children[i];
        obj->draw();
       // printf("obj.children()=%d  %d\n",i,obj->VAO);
    }
}

camera::camera(): xObject() // init
{
    //set(pos,0,5,20);
    position.x=0;
    position.y=500;
    position.z=900;

    set(up,0,1,0); //
    set(forward,0,0,-1); //
    set(force,0,0,0);

    key_forward =SDLK_w;
    key_backward=SDLK_s;
    key_side_right=SDLK_d;
    key_side_left=SDLK_a;

    d_ball=nullptr;
    d_focus=true;
    name="camera_viewer";
}

extern bool quit;
void camera::update(float dt)
{
    SDL_Event e; //Event handler

    xObject::update(dt);

    if(d_focus==false)
        return;

    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
        int key=e.key.keysym.sym;

        switch(e.type)
        {
            case SDL_QUIT:
                //quit = true;  // global
                d_focus=false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
                break;
            case SDL_MOUSEBUTTONDOWN:
                printf("clicked! \n");
                if (d_ball==nullptr)
                    d_ball=findObject("ball"); // how lagg ?
                if (d_ball!=nullptr){
                    //set(d_ball->pos, pos);
                }
                break;
            default :;

        }

        //Handle keypress with current mouse position
        if( e.type == SDL_KEYDOWN)
        {            
            //int scancode=e.key.keysym.scancode;
            /* SDLK_UP , SDLK_W */
            //handleKeys( e.text.text[ 0 ], x, y );
            if (key == SDLK_ESCAPE)
            {
               // quit=true;
                //quit = true;  // global
                d_focus=false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
                break;
            }
            if (key == SDLK_F4)
            {
                quit = true;  // global
                d_focus=false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
                break;
            }
            on_key_pressed(key);
            // printf("keycode=%d \n",key);
        }
        if( e.type == SDL_KEYUP)
        {            
            on_key_released(key);
        }
        if( e.type == SDL_MOUSEMOTION)
        {
            //SDL_GetMouseState( &x, &y );
            int dx=e.motion.xrel;
            int dy=e.motion.yrel;

            on_mouse_moved(dx,dy);            
            if (e.motion.state==SDL_BUTTON_LMASK)
            {
               // printf("dddddd!\n");
            }
        }

    }

}

void camera::on_key_pressed(uint key)
{
    /*
     right_side,
     혹은 dir 성분만  zero해야하는거 아닐까?
    */
    float right[3],t[3];
    //fprintf(stderr,"camera.onkey\n");
    float speed=500;
    glm::vec3 v1;
    if(key==key_forward) //w
    {
        //multiply(forward,speed,force);
        new_force=new_forward*speed;
    }
    else if(key==key_backward) //s
    {
        //multiply(forward,-speed,force);
        new_force=new_forward* -speed;
    } else
    if(key==key_side_right) //d
    {
        //cross(forward,up,right);
        //multiply(right,speed,force);
        v1=glm::cross(new_forward,new_up);
        new_force=v1* speed;

    } else
    if(key==key_side_left) // a
    {        
        //cross(forward,up,right);
        //multiply(right,-speed,force);
        v1=glm::cross(new_forward,new_up);
        new_force=v1* -speed;
    } else
       printf("pressed key %d \n", key);

    ///fprintf(stderr," pressed! %f %f %f \n",force[0],force[1],force[2]);
    //fprintf(stderr," pressed! %f %f %f \n",forward[0],forward[1],forward[2]);
    //fprintf(stderr," pressed! pos %f %f %f \n",pos[0],pos[1],pos[2]);
}


void camera::on_key_released(uint key)
{

    if(key==key_forward or key==key_backward
            or key==key_side_right or key==key_side_left) //s) //s) //w
    {
        //multiply(forward,0.0,force); //
        new_force=glm::vec3(0);
        //fprintf(stderr,"release! %f %f %f \n",force[0],force[1],force[2]);
    }    

    if(key==SDLK_1) //
        set(force,0,0,0);
}
void camera::updateCameraVectors()
{

}
void camera::on_mouse_moved(int dx, int dy)
{
    //dx=0;
    float angle= -1*dx*M_PI/360.0;

    ///printf("angle=%f \n",angle);
    /*
     * float v[3];
    quat_rotate(up,angle,forward,v);
    normalize(v);
    set(forward,v);

    float side[3];
    cross(forward,up,side);
    quat_rotate(side,angle,forward,v);
    normalize(v);
    set(forward,v);
    */

    glm::quat q;
    glm::vec3 v1;
    new_up=glm::make_vec3(up);
    q=glm::angleAxis(angle, new_up);
    v1 = q * glm::make_vec3(forward);
    new_forward=glm::normalize(v1);

    set(forward, glm::value_ptr(new_forward)); // **

    angle=-2*dy*M_PI/360.0;
    ///printf("angle=%f \n",angle);
    glm::vec3 right=glm::cross(new_forward, new_up);
    q=glm::angleAxis(angle, right);
    v1 = q * new_forward;
    new_forward=glm::normalize(v1);
    //v1 = q * glm::make_vec3(new_up); // not work!!!
    set(forward, glm::value_ptr(new_forward)); // **

    v1=glm::cross(right,new_forward);
    //new_up=glm::normalize(v1);
    //set(up,glm::value_ptr(new_up));
}

grid::grid()
{
    grid_n=100;
    shader=new Shader();
    shader->Load("./shader/grid_vertex.glsl","./shader/grid_fragment.glsl");

    float *vertexData = (float*)malloc(sizeof(float)*2000*4*2);
    if (vertexData==nullptr)
    {
        SDL_Log("ERROR: Faile to malloc !!!!");
        return;
    }
    int i=0;
    for(int x=-500 ; x < 500; x++)
    {
        vertexData[i++]=x;  // x
        vertexData[i++]=0;  // y
        vertexData[i++]=500;// z
        vertexData[i++]=1.0;
        vertexData[i++]=x;
        vertexData[i++]=0;
        vertexData[i++]=-500;
        vertexData[i++]=1.0;
    }

    for(int z=-500 ; z < 500; z++)
    {
        vertexData[i++]=-500;  // x
        vertexData[i++]=0;  // y
        vertexData[i++]=z;// z
        vertexData[i++]=1.0;
        vertexData[i++]=500;
        vertexData[i++]=0;
        vertexData[i++]=z;
        vertexData[i++]=1.0;
    }
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &buffer);

    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 2000*8* sizeof(float)/* bytes */, vertexData, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float) /* bytes */, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    free(vertexData);
}


void grid::draw()
{
    //TODO: object
    //draw_axis();
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_LINES, 0, 4000);
    //glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
    //glDisableVertexAttribArray(0);    
}

void grid::update(float dt)
{

}


extern vector<xObject* > objects;


void apply_gravity(xObject *obj)
{
    //obj->
    //set(obj->force,0,-9.8,0);
    if(obj->d_type!=TYPE_GROUND)
        obj->force[1]=-9.8;
}

CollisionDetector::CollisionDetector()
{
    d_type=TYPE_CONTROLLER;
}

void CollisionDetector::update(float dt)
{

    for ( size_t i=0 ; i < objects.size(); i++)
    {
        xObject *obj = objects[i];
        float new_pos[3];
        //apply_gravity(obj);
        if(obj->d_type==TYPE_CONTROLLER) continue;
        if(obj->d_type==TYPE_GROUND)
        {
            continue;
        }
        if(obj->d_type==TYPE_SPHERE)
        {
            float v[3];
            multiply(obj->force, dt*0.1, v);
            ////add(obj->pos, v, obj->pos);
        }

        xObject *other;
        for ( size_t j=0 ; j < objects.size(); j++)
        {
            other = objects[i];
            if (obj != other )
            {


            }
        }
    }
}

particle::particle():xObject()
{
    name="particle";
    d_size=2000;

    Texture *_tex = new Texture("particle.png"); //
    texname=_tex->getTextureName();

    shader=new Shader();
    shader->Load("./shader/point_vertex.glsl","./shader/point_fragment.glsl");

    float *vertexData = (float*)malloc(sizeof(float)*3*d_size);

    if (vertexData==nullptr)
    {
        SDL_Log("ERROR: Fail to malloc() !!!!");
        return;
    }

    for ( int i=0 ; i < d_size ;)
    {
        float x,y,z;
        x=rand()%5000 - 1000;
        y=rand()%5000 - 0;
        z=rand()%5000 - 1000;
        vertexData[i++]=x;  // x
        vertexData[i++]=y;  // y
        vertexData[i++]=z;  // z
      //  printf("x=%f  y=%f  z=%f \n",x,y,z);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * d_size /* bytes */, vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // input to vertex-shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3 /* bytes */, (void*)0);

    //glEnableClientState(GL_VERTEX_ARRAY);
    //glVertexPointer(3, GL_FLOAT,sizeof(float) * 3 /* bytes */, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    glBindVertexArray(0); // unbind

    free(vertexData);


    flag_axis_on=true;
    make_axis();//
}

void particle::draw()
{
    update_model_matrix();

    float _m[16];
    if (parent)
    {
        loadIdentity(_m);
        //set(_m,parent->model_m);
        //mat4x4_mult(_m, parent->model_mat, model_mat);
    }
    //else mat4x4_set(_m, model_mat); // copy
    if (1)
    {
      //  model = glm::mat4(1.0); // identity
    }

    //glm::mat4 _m1(1);

    if (texname > 0)
    {
       // glBindTexture(GL_TEXTURE_2D, texname);
       // glEnable(GL_POINT_SPRITE);
       // glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    }

    if (VAO>0)
    {
        GLint location;
        //shader->setMat4("model",_m1);
        location = glGetUniformLocation(shader->mProgram, "modelView");
        if (location >= 0)
            glUniformMatrix4fv(location, 1, GL_FALSE, _m); // ( location, count,  transpose, float *value )

        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointSize(3);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, d_size); // 0~2000
        //glDrawArrays(GL_LINES, 0, d_size/2);
        glBindVertexArray(0); // break
        //glDisableVertexAttribArray(0);
        glDisable(GL_POINT_SPRITE);
    }
    draw_axis();
}

void particle::update(float dt)
{

}

objLight::objLight():xObject()
{
    name="light";
    //d_size=2000;
    //shader=new Shader();
    //shader->Load("./shader/point_vertex.glsl","./shader/point_fragment.glsl");
    //shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");
    flag_axis_on=true;
}

void objLight::draw()
{
    xObject::draw();
}

void objLight::update(float dt)
{

}

cube::cube():xObject()
{
    rotate_angle_speed=30;
    flag_axis_on=true;
}

void cube::update(float dt)
{
    yaw += rotate_angle_speed * dt; // degree to radian
}

void cube::draw()
{
    xObject::draw();
}


extern std::vector<Texture *> cached_textures;

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
    for ( size_t i=0 ; i < cached_textures.size(); i++)
    {
        if(cached_textures[i]->d_filename==filename)
        {
            texname=cached_textures[i]->getTextureName();
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
    Texture *_tex = new Texture(basename);
    if (_tex->getTextureName()>0)
        cached_textures.push_back(_tex);
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

