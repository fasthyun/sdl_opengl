#include "object.h"
#include <chrono>
#include "zmath.h"
#include <iostream>
#include <GL/glu.h>
// ...

// temporary
int key_foward;
int key_backward;
int key_side_right;
int key_side_left;

using namespace std::chrono;
milliseconds ms = duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
);

uint64_t epoch_now() { // timeSinceEpochMillisec
    // 좀 느릴거 같은뎅..(hyun)
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

xObject::xObject()
{
    meshes=NULL;
    set(pos,0,0,0);
    set(up,0,1,0); //
    set(forward,0,0,1); //
    set(force,0,0,0);
    //make_circle();
}

void xObject::update(float dt)
{
    float t[3];
    multiply(force, dt, t);
    add(pos, t, pos);
}

void xObject::make_circle(float size)
{
    // move to element3d.cpp  3d_element, element_3d
    // blender로 아니면 이렇게 ... 둘다 필요?

    int n=50;
    float dt=2*M_PI/n;
    float x,y;
    vertex *vert=new vertex(); //center (0,0,0)
    vert->v[0]=0;
    vert->v[1]=0;
    vert->v[2]=0;
    vertexes.push_back(vert); // center
    for (int i=0;i<n;i++)
    {
         vertex *vert=new vertex();
         x=sin(dt*i);
         y=cos(dt*i);
         vert->v[0]=x;
         vert->v[1]=y;
         vert->v[2]=0;
         vertexes.push_back(vert);
    }

    for (int i=0;i< n;i++)
    {
        triangle * tri = new triangle();
        tri->v[0]=0;
        tri->v[1]=i;
        if (i+1 >= n)
            tri->v[2]=1;
        else
            tri->v[2]=i+1;
        triangles.push_back(tri);
    }
}

void xObject::draw_dir_up()
{
    float len=3;
    float t[3];
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3fv(pos);
    add(pos,up,t);
    glVertex3fv(t);
    //glColor3f(0.0, 1.0, 0);
    glVertex3fv(pos);
    add(pos,forward,t);
    glVertex3fv(t);
    glEnd();
    glLineWidth(1);
}

void xObject::draw_axis()
{
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
    if (meshes==NULL) return; // hmm...

    //printf("mesesh->size()= %ld \n", meshes->size());
    for(size_t i=0; i< meshes->size();i++)
    {
        Meshf *mesh= &(meshes->at(i));
        //vector<float> &vertices = (m->vertices);
        //vector<unsigned int> &faces = (m->faces);
        int face_n=mesh->faces.size()/3;
        glColor3f(0.6,0.6,0.6);
        glBegin(GL_TRIANGLES);
        for (int face_idx=0 ; face_idx<face_n; face_idx++)
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

void xObject::draw()
{
    draw_axis();
    draw_dir_up();
    draw_meshes();
    return;
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_TRIANGLES);
    for(size_t i=0; i< triangles.size();i++)
    {
        triangle *tri= triangles.at(i);
        //vector<float> &vertices = (m->vertices);
        //vector<unsigned int> &faces = (m->faces);
        //glNormal3fv(Ng); // normal for geometry
        int idx;
        idx=tri->v[0];
        glVertex3fv(vertexes[idx]->v);
        idx=tri->v[1];
        glVertex3fv(vertexes[idx]->v);
        idx=tri->v[2];
        glVertex3fv(vertexes[idx]->v);
    }
    glEnd();
}

void xObject::on_key_pressed(uint key)
{

}

void xObject::on_key_released(uint key)
{

}

void xObject::on_mouse_moved(int dx, int dy)
{

}

camera::camera(): xObject()
{
    set(pos,0,5,20);
    set(up,0,1,0); //
    set(forward,0,0,-1); //
    set(force,0,0,0);
}

void camera::update(float dt)
{
    xObject::update(dt);
}

void camera::on_key_pressed(uint key)
{
    /*
     *  right_side,
     *  혹은 dir 성분만  zero해야하는거 아닐까?
     *
     */
    float right[3],t[3];
    //fprintf(stderr,"camera.onkey\n");
    float speed=5;
    if(key==key_foward) //w
    {
        multiply(forward,speed,force);
    }
    if(key==key_backward) //s
        multiply(forward,-speed,force);

    if(key==key_side_right) //d
    {
        cross(forward,up,right);
        multiply(right,speed,force);
    }
    if(key==key_side_left) // a
    {
        //float right[3];
        cross(forward,up,right);
        multiply(right,-speed,force);

    }
    //fprintf(stderr," pressed! %f %f %f \n",force[0],force[1],force[2]);
    //fprintf(stderr," pressed! %f %f %f \n",forward[0],forward[1],forward[2]);
    fprintf(stderr," pressed! pos %f %f %f \n",pos[0],pos[1],pos[2]);
}

void camera::on_key_released(uint key)
{
    if(key==key_foward) //w
    {
        multiply(forward,0,force); //
        //fprintf(stderr,"release! %f %f %f \n",force[0],force[1],force[2]);
    }
     if(key==key_backward) //s
        multiply(forward,0,force);

   if(key==key_side_right) //d
        set(force,0,0,0);
    if(key==key_side_left) // a
        set(force,0,0,0);

}

void camera::on_mouse_moved(int dx, int dy)
{
    float angle=-1*M_PI*dx/360.0;
    float v[3];
    quat_rotate(up,angle,forward,v);
    normalize(v);
    set(v,forward);

    angle=-1*M_PI*dy/360.0;
    float side[3];
    cross(forward,up,side);
    quat_rotate(side,angle,forward,v);
    normalize(v);
    set(v,forward);
}

grid::grid()
{

}
void grid::draw()
{
    //TODO: object
    draw_axis();
    glColor4f(0.3,0.3,0.3,1);
    glBegin(GL_LINES);
    for(int x=-500 ; x < 500; x++)
    {
        glVertex3f(x,0,-500);
        glVertex3f(x,0,500);
    }
    for(int z=-500 ; z < 500; z++)
    {
        glVertex3f(-500,0,z);
        glVertex3f(500,0,z);
    }
    glEnd();
}


void grid::update(float dt)
{

}
