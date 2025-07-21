#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gameLoop(this)
{
    ui->setupUi(this);

    connect(ui->pauseButton, SIGNAL(clicked()), &gameLoop , SLOT(pause()));
    connect(ui->playButton, SIGNAL(clicked()), &gameLoop , SLOT(play()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


