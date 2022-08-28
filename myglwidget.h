#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include "./support/mesh.h"
#include "object.h"

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();
    void setData(vector<Meshf> *_meshes);
    void update_objects();
    void draw_grid();
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
public slots:
    // slots for xyz-rotation slider
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void on_timeout();
signals:
    // signaling rotation from mouse movement
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

private:
    void draw();    
    QTimer *timer;
    qint64 time_prev;
    qint64 time_fps;

    int fps;
    int fps_count;

    int xRot;
    int yRot;
    int zRot;

    QPoint lastPos;

    vector<Meshf> *meshes;
    vector<xObject* > objects;    
    xObject *d_camera;
};

#endif // MYGLWIDGET_H
