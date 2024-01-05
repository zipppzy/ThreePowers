#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "gameloop.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GameLoop *gameLoop = new GameLoop(this);

    connect(ui->pauseButton, SIGNAL(clicked()), gameLoop , SLOT(pause()));
    connect(ui->playButton, SIGNAL(clicked()), gameLoop , SLOT(play()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


