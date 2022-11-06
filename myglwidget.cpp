#include "zmath.h"
#include "font.h"
#include "texture.h"
#include "myglwidget.h"
//#include <GL/glu.h>
#include <QDateTime>
#include <QtWidgets>

extern int key_foward;
extern int key_backward;
extern int key_side_right;
extern int key_side_left;

MyGLWidget::MyGLWidget(QWidget *parent)
    : QOpenGLWidget( parent)
{
    meshes=NULL; // temp

    timer =new QTimer();
    setFocusPolicy(Qt::StrongFocus);
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timeout()));
    timer->start(0);
    time_prev=QDateTime::currentMSecsSinceEpoch();
    fps=0;
    fps_count=0;

    xObject *obj = new xObject();
    objects.push_back(obj);

    obj = new grid();
    objects.push_back(obj);

    d_camera = new camera();
    set(d_camera->pos,0,3,7);
    set(d_camera->up,0,1,0); //
    set(d_camera->forward,0,0,-1); //
    objects.push_back(d_camera);
    time_fps=0;

    call_count=0;
    time_call=0;
    /// makeCurrent();

    font_init2(); // move to console.h ???
     key_foward=Qt::Key_W;
    key_backward=Qt::Key_S;
    key_side_right=Qt::Key_D;
    key_side_left=Qt::Key_A;
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

}


void MyGLWidget::on_timeout()
{
    qint64 _now = QDateTime::currentMSecsSinceEpoch();
    float  dt= (_now - time_prev)/1000.0;
    time_prev=_now;
    time_fps +=dt;
    time_call +=dt;

    call_count++;
    if (time_call>1.0)
    {
        fprintf(stderr,"call=%d\n",call_count);
        call_count=0;
        time_call-=1.0;
    }

    // update_objects
    for ( size_t i=0 ; i < objects.size(); i++)
    {
        objects[i]->update(dt);
    }
    update();
}

void MyGLWidget::initializeGL()
{
    //qglClearColor(Qt::black);
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
    //glShadeModel(GL_SMOOTH);

    static GLfloat lightPosition[4] = { 0, 80, 80, 2.0 };
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    printf("OpenGL loaded\n");
    //gladLoadGLLoader(SDL_GL_GetProcAddress);
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    printf("Context valid: %d \n" , context()->isValid());
    printf("Really used OpenGl:  %d.%d \n", context()->format().majorVersion(),
             context()->format().minorVersion());

}

void MyGLWidget::paintGL()
{
    fps_count++;
    if (time_fps>1.0)
    {
        fps=fps_count;
        time_fps=time_fps - 1.0;
        fps_count=0;
        fprintf(stderr,"fps:%d\n",fps);
    }

    glEnable(GL_TEXTURE_2D);
    glFrontFace(GL_CCW);
    //glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1,1,-1,1,1,5000*3);
    LookAt(d_camera->pos, d_camera->forward , d_camera->up);
    /*
     float target[3];
     add(d_camera->pos,d_camera->forward,target);
     gluLookAt(d_camera->pos[0],d_camera->pos[1],d_camera->pos[2],
            target[0],target[1],target[2], d_camera->up[0],d_camera->up[1],d_camera->up[2]);
    */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // fprintf(stderr,"pos %f %f %f \n",pos[0],pos[1],pos[2]);
    // fprintf(stderr,"dir %f %f %f \n",dir[0],dir[1],dir[2]);

    render_text_texture();

    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //draw();
    //glDisable(GL_LIGHTING);

    for ( size_t i=0 ; i < objects.size(); i++)
    {
       glLoadIdentity();
       objects[i]->draw();
    }
    //glColor3f(1,1,1);
    //renderText(15,23,"FPS:" + QString::number(fps));
}

void MyGLWidget::resizeGL(int width, int height)
{
    //int side = qMin(width, height);
    // glViewport((width - side) / 2, (height - side) / 2, side, side);
    printf("resize \n");
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 5000*3);
    //glOrtho(-2, +2, -2, +2, 1.0, 15.0);
    glMatrixMode(GL_MODELVIEW);    
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    //event->
   // lastPos = event->pos();

}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{

    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    d_camera->on_mouse_moved(dx,dy);
    /*
    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    */
    lastPos = event->pos();
}

void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    d_camera->on_key_pressed(e->key());
    switch(e->key()){
    case  Qt::Key_Up :
        fprintf(stderr,"up\n");
        break;
    case  Qt::Key_Left :
    break;
    case  Qt::Key_Right :
    break;
    case  Qt::Key_Down :
    break;
    }
}

void MyGLWidget::keyReleaseEvent(QKeyEvent *e)
{
     d_camera->on_key_released(e->key());
}


void MyGLWidget::draw()
{

}
