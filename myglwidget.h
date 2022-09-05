#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

// #include <QGLWidget> // old
#include <QOpenGLWidget>
#include <QTimer>
#include "object.h"

class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();
    void setData(vector<Meshf> *_meshes);

signals:

public slots:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
public slots:
    // slots for xyz-rotation slider
    void setXRotation(int angle);

    void on_timeout();
signals:
    // signaling rotation from mouse movement
    void xRotationChanged(int angle);

private:
    void draw();    
    QTimer *timer;
    qint64 time_prev;
    float time_fps;
    float time_call;

    int fps;
    int fps_count;
    int call_count;

    QPoint lastPos;

    vector<Meshf> *meshes;
    vector<xObject* > objects;    
    camera *d_camera;
};

#endif // MYGLWIDGET_H
