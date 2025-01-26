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
    // 좀 느릴거 같은뎅..(hyun)
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
    boost::uuids::uuid _uuid = boost::uuids::random_generator()();
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
    name="None";
    make_axis();
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
    glm::vec3 force1;
    //multiply(force, dt, v);
    //add(pos, v, pos);
    position+=force1*dt;
}


void xObject::draw_dir_up()
{
    float len=3;
    float t[3];
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);
    ///glVertex3fv(pos);
    ////add(pos,up,t);
    glVertex3fv(t);
    glColor3f(0.0, 1.0, 0);
    ////glVertex3fv(pos);
    ///add(pos,forward,t);
    glVertex3fv(t);
    glEnd();
    glLineWidth(1);
}

///#include "gltf-loader.h"
void xObject::load_gltf(string name)
{
   /// LoadGLTF(name, 1, &meshes, &materials, &xtextures);
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

    glEnableVertexAttribArray(0); //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)0); // position[3]   // (idx, size, type, ? , stride-size, offset)
    glEnableVertexAttribArray(2); //
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)(3 * sizeof(float))); // RGBA

    glBindVertexArray(0); // break
}
// TODO: name

void xObject::make_glVertexArray()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertexes.size()*sizeof(vertex), vertexes.data(), GL_STATIC_DRAW);
    printf("sizeof(vertex)= %ld vertexs.size=%ld data=%p\n", sizeof(vertex), vertexes.size(), vertexes.data());
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


void xObject::draw_axis()
{
    /* https://stackoverflow.com/questions/60440682/drawing-a-line-in-modern-opengl
     */
    // this will not work ! old_opengl
    float len=3;


    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1.0, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(len,0,0);

    glColor3f(0.0, 1.0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,len,0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,len);
    glEnd();
    glLineWidth(1);
}


void xObject::draw_meshes()
{
    // will not work ! (legacy)
    // if (meshes==NULL) return; // hmm...
    //   TODO: vertexbuffer
    // printf("mesesh->size()= %ld \n", meshes->size());
    for(size_t i=0; i< meshes.size();i++)
    {
        MeshFloat *mesh= &(meshes.at(i));
        //vector<float> &vertices = (m->vertices);
        //vector<unsigned int> &faces = (m->faces);
        int face_n=mesh->faces.size()/3;
        glColor3f(0.6,0.6,0.6);
        glBegin(GL_TRIANGLES);
        for (int face_idx=0 ; face_idx < face_n; face_idx++)
        {
            float v0[3], v1[3], v2[3], Ng[3], Ns[3], u, v;
            u=1; v=1;
            mesh->GetVerticeNormal(v0,v1,v2,Ng,Ns,face_idx,u,v);
            glNormal3fv(Ng); // normal for geometry
            glVertex3fv(v0);
            glVertex3fv(v1);
            glVertex3fv(v2);
        }
        glEnd();
    }
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

    if (1)
    {
        model = glm::mat4(1.0); // identity
        //model=glm::rotate(model, roll , glm::vec3(0,0,1));//z
        //model=glm::rotate(model, yaw , glm::vec3(0,1,0)); //y
        //model=glm::rotate(model, pitch , glm::vec3(1,0,0)); //x
        //model=glm::scale(model, glm::vec3(scale[0],scale[1],scale[2]));
        glm::mat4 Mi = glm::mat4(1.0f);
        glm::mat4 RotationMatrix = glm::rotate(Mi, glm::radians(yaw), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 TranslationMatrix = glm::translate(model, position);
        glm::mat4 ScaleMatrix = glm::mat4(1.0);
        //glm::mat4 RotationMatrix = glm::mat4(1.0);
        /* TransformedVector = TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector; */
        model= TranslationMatrix * RotationMatrix * ScaleMatrix; // works!!!
    }
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
        */
        /* location = glGetUniformLocation(shader->mShaderProgram, "ourTexture");
        if (location >=0) glUniform1i(location, texname); */

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, triangles.size() *3 /* index count so x3 */ , GL_UNSIGNED_INT, 0); //( mode, count, index_data_type, void * indices);
        /* same as
           for(i =0 ;i < len ; i++)
              glArrayElement(i);
           --- or ---
           glDrawArrays(GL_TRIANGLES, 0, vertexes.size());
        */
    }

    if (flag_axis_on && VAO_axis>0)
    {
        glDisable(GL_DEPTH_TEST);    // Enable depth buffering
        shader->setMat4("model",_m1); //
        glLineWidth(3);
        glBindVertexArray(VAO_axis);
        //glVertexAttrib3f(vertColor_loc, 1.0f, 1.0f, 0.2f);
        glVertexAttribI1i(4, 2); //glVertexAttrib1f(4, 2);
        glDrawArrays(GL_LINES, 0, 6); // why 6?
    }

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
    position.y=5;
    position.z=20;

    set(up,0,1,0); //
    set(forward,0,0,-1); //
    set(force,0,0,0);

    key_forward=SDLK_w;
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
                if (d_ball!=nullptr);
                    //set(d_ball->pos, pos);
                break;
            default :;

        }

        //Handle keypress with current mouse position
        if( e.type == SDL_KEYDOWN)
        {            
            //int scancode=e.key.keysym.scancode;
            /* SDLK_UP , SDLK_W */
            //SDL_GetMouseState( &x, &y );
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
        else if( e.type == SDL_KEYUP)
        {            
            on_key_released(key);
        }
        else if( e.type == SDL_MOUSEMOTION)
        {
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
    float speed=5;
    if(key==key_forward) //w
    {
        multiply(forward,speed,force);
    }
    else if(key==key_backward) //s
    {
        multiply(forward,-speed,force);
    } else
    if(key==key_side_right) //d
    {
        cross(forward,up,right);
        multiply(right,speed,force);
    } else
    if(key==key_side_left) // a
    {
        //float right[3];
        cross(forward,up,right);
        multiply(right,-speed,force);
    } else
       printf("pressed key %d \n", key);

    fprintf(stderr," pressed! %f %f %f \n",force[0],force[1],force[2]);
    //fprintf(stderr," pressed! %f %f %f \n",forward[0],forward[1],forward[2]);
    //fprintf(stderr," pressed! pos %f %f %f \n",pos[0],pos[1],pos[2]);
}


void camera::on_key_released(uint key)
{
    if(key==key_forward) //w
    {
        multiply(forward,0.0,force); //
        //fprintf(stderr,"release! %f %f %f \n",force[0],force[1],force[2]);
    }
    if(key==key_backward) //s
        multiply(forward,0.0,force);

    if(key==key_side_right) //d
        set(force,0,0,0);

    if(key==key_side_left) // a
        set(force,0,0,0);

    if(key==key_side_left) // a
        set(force,0,0,0);

    if(key==SDLK_1) //
        set(force,0,0,0);
}

void camera::on_mouse_moved(int dx, int dy)
{
    float angle=-1*M_PI*dx/360.0;
    float v[3];
    quat_rotate(up,angle,forward,v);
    normalize(v);
    set(forward,v);

    angle=-1*M_PI*dy/360.0;
    float side[3];
    cross(forward,up,side);
    quat_rotate(side,angle,forward,v);
    normalize(v);
    set(forward,v);
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

particle::particle()
{
    name="particle";
    d_size=2000;
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
        x=rand()%60 - 30;
        y=rand()%60 - 30;
        z=rand()%60 - 30;
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
}

void particle::draw()
{
    //draw_meshes();
    ///mat4x4_translate(model_mat, pos[0], pos[1], pos[2]);
    float _m[16];
    if (parent)
    {
        loadIdentity(_m);
        //set(_m,parent->model_m);
        //mat4x4_mult(_m, parent->model_mat, model_mat);
    }
    //else mat4x4_set(_m, model_mat); // copy

    if (texname > 0)
        glBindTexture(GL_TEXTURE_2D, texname);

    if (VAO>0)
    {
        GLint location;
        location = glGetUniformLocation(shader->mProgram, "modelView");
        if (location >= 0)
            glUniformMatrix4fv(location, 1, GL_FALSE, _m); // ( location, count,  transpose, float *value )

        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointSize(3);
        //draw_axis();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, d_size); // 0~2000
        //glDrawArrays(GL_LINES, 0, d_size/2);
        glBindVertexArray(0); // break
        //glDisableVertexAttribArray(0);
    }
}

void particle::update(float dt)
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


extern std::vector<Texture *> textures;

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
    Texture *_tex = new Texture(basename);
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

