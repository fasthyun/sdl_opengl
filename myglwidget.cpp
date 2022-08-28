
#include <QtWidgets>
#include <QtOpenGL>
#include "object.h"
#include "myglwidget.h"
#include "zmath.h"

MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    meshes=NULL; // temp
    xRot = 0;
    yRot = 0;
    zRot = 0;        
    timer =new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timeout()));
    timer->start(0);
    time_prev=QDateTime::currentMSecsSinceEpoch();
    fps=0;
    fps_count=0;
    xObject *x = new xObject();
    objects.push_back(x);    
    d_camera = new camera();
    objects.push_back(d_camera);
}

MyGLWidget::~MyGLWidget()
{

}

void MyGLWidget::setData(vector<Meshf> *_meshes)
{
    meshes=_meshes;
    update();
}

QSize MyGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MyGLWidget::sizeHint() const
{
    return QSize(600, 600);
}

static void qNormalizeAngle(int &angle)
{
    // ????
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360)
        angle -= 360 * 16;
}

void MyGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::update_objects()
{
    for ( size_t i=0 ; i < objects.size(); i++)
    {
        objects[i]->update();
    }

}

void MyGLWidget::on_timeout()
{
    qint64 _now = QDateTime::currentMSecsSinceEpoch();
    qint64 dt=_now - time_prev;
    time_prev=_now;
    time_fps+=dt;

    //update();
    update_objects();
    updateGL();
}

void MyGLWidget::initializeGL()
{
    //qglClearColor(Qt::black);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    //glEnable(GL_CULL);
    glShadeModel(GL_SMOOTH);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    static GLfloat lightPosition[4] = { 0, 50, 80, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void MyGLWidget::paintGL()
{
    fps_count++;

    if (time_fps>1000)
    {
        fps=fps_count;
        time_fps=0;
        fps_count=0;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);


    draw_grid();
    draw();
    for ( size_t i=0 ; i < objects.size(); i++)
    {
        objects[i]->draw();
    }

    glColor3f(1,1,1);
    renderText(15,23,"FPS:" + QString::number(fps));

}

void MyGLWidget::resizeGL(int width, int height)
{
    //int side = qMin(width, height);
    // glViewport((width - side) / 2, (height - side) / 2, side, side);
    glViewport(0,0,width,height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-1,1,-1,1,1,5000*3);
    //glOrtho(-2, +2, -2, +2, 1.0, 15.0);
    //gluLookAt(pos,)

    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }

    lastPos = event->pos();
}

void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()){
    case  Qt::Key_Up :
        break;
    case  Qt::Key_Left :
    break;
    case  Qt::Key_Right :
    break;
    case  Qt::Key_Down :
    break;
    }
}

void MyGLWidget::draw_grid()
{
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

void MyGLWidget::draw()
{
    if (meshes==NULL) return; // hmm...

    //printf("mesesh->size()= %ld \n", meshes->size());
    for(size_t i=0; i< meshes->size();i++)
    {
        Meshf *mesh= &(meshes->at(i));        
        //vector<float> &vertices = (m->vertices);
        //vector<unsigned int> &faces = (m->faces);
        int face_n=mesh->faces.size()/3;
        qglColor(Qt::red);
        glBegin(GL_TRIANGLES);
        for (int face_idx=0 ; face_idx<face_n; face_idx++)
        {
            float v0[3], v1[3], v2[3], Ng[3], Ns[3], u, v;
            u=1;
            v=1;
            mesh->GetVerticeNormal(v0,v1,v2,Ng,Ns,face_idx,u,v);
            glNormal3fv(Ng); // normal for geometry
            glVertex3fv(v0);
            glVertex3fv(v1);
            glVertex3fv(v2);
        }
        glEnd();
    }
}
