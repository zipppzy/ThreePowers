#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , nextActionButtonCoords{0,0}
{
    ui->setupUi(this);

    this->showMaximized();

    buttonContainer = new QWidget();
    gridLayout = new QGridLayout(buttonContainer);

    buttonContainer->setLayout(gridLayout);
    ui->actionScrollArea->setWidget(buttonContainer);
    ui->actionScrollArea->setWidgetResizable(true);

    // connect(ui->pauseButton, SIGNAL(clicked()), &gameLoop , SLOT(pause()));
    // connect(ui->playButton, SIGNAL(clicked()), &gameLoop , SLOT(play()));
}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::addActionButton(std::shared_ptr<Action> action){
    ActionButton *btn = new ActionButton(action);
    gridLayout->addWidget(btn, nextActionButtonCoords.second, nextActionButtonCoords.first);

    if(nextActionButtonCoords.first >= MAX_ACTION_BUTTON_COLS){
        nextActionButtonCoords.first = 0;
        nextActionButtonCoords.second++;
    }else{
        nextActionButtonCoords.first++;
    }
}

