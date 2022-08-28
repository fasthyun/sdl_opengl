#include "object.h"
#include <chrono>
#include "zmath.h"
// ...

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
    set(pos,0,0,0);
    set(up,0,1,0); //
    set(dir,0,0,1); //
    set(force,0,0,0);
    make_circle();
}

void xObject::update()
{

}

void xObject::make_circle(float size)
{
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

void make_cylinder(float size=1.0)
{
    float len=3;
    glLineWidth(3);
    glColor3f(1.0, 0, 0);
    glBegin(GL_POLYGON);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);

    glColor3f(0.0, 1.0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,len,0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,len);
    glEnd();
    glLineWidth(1);
}

void xObject::draw_dir_up()
{
    float len=3;
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3fv(pos);
    glVertex3fv(up);
    //glColor3f(0.0, 1.0, 0);
    glVertex3fv(pos);
    glVertex3fv(dir);
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

void xObject::draw()
{
    //draw_axis();
    draw_dir_up();
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_TRIANGLES);
    for(size_t i=0; i< triangles.size();i++)
    {
        triangle *tri= triangles.at(i);
        //vector<float> &vertices = (m->vertices);
        //vector<unsigned int> &faces = (m->faces);

        //    glNormal3fv(Ng); // normal for geometry
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

void camera::update()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glViewport(width(),height());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
