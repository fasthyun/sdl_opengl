#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gltf-loader.h"
#include <QTimer>
#include <QtDebug>

#include "./support/mesh.h"
#include "./support/material.h"

using namespace std;
vector<Meshf> meshes;
vector<Material> materials;
vector<Texture> textures;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    //format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    //ui->mywidget->setFormat(format);
    //QSurfaceFormat::setDefaultFormat(format);


    ui->setupUi(this);


    //const QOpenGLContext *context = ui->mywidget->context() ;

    //context;

    qDebug() << "OpenGl information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
    qDebug() << "                    RENDERDER:    " << (const char*)glGetString(GL_RENDERER);
    qDebug() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
    qDebug() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    qDebug() << "endstuff\n";


    //timer =new QTimer();
    //connect(timer, SIGNAL(timeout()), this, SLOT(on_timeout()));
    //timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    LoadGLTF("/home/hyun/box01.glb", 1, &meshes, &materials, &textures);
    ui->mywidget->setData(&meshes);
}

void MainWindow::on_timeout()
{

   fprintf(stderr,"timeout!\n");
   //ui->mywidget->update();   
   // printf("timeout!\n");
}
