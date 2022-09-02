#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gltf-loader.h"
#include <QTimer>

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
    ui->setupUi(this);
    //timer =new QTimer();
    //connect(timer,SIGNAL(timeout()),this,SLOT(on_timeout()));
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
