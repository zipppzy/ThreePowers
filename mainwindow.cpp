#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "skilldelegate.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , nextActionButtonCoords{0,0}
{
    ui->setupUi(this);

    this->showMaximized();

    buttonContainer = new QWidget();
    gridLayout = new QGridLayout(buttonContainer);
    gridLayout->setAlignment(Qt::AlignTop);

    buttonContainer->setLayout(gridLayout);
    ui->actionScrollArea->setWidget(buttonContainer);
    ui->actionScrollArea->setWidgetResizable(true);

    // connect(ui->pauseButton, SIGNAL(clicked()), &gameLoop , SLOT(pause()));
    // connect(ui->playButton, SIGNAL(clicked()), &gameLoop , SLOT(play()));
}

MainWindow::~MainWindow(){
    delete ui;
}

QPushButton* MainWindow::getPauseButton() const{
    return ui->pauseButton;
}

QPushButton* MainWindow::getPlayButton() const{
    return ui->playButton;
}

void MainWindow::addActionButton(ActionButton* btn){
    gridLayout->addWidget(btn, nextActionButtonCoords.second, nextActionButtonCoords.first);

    if(nextActionButtonCoords.first >= MAX_ACTION_BUTTON_COLS){
        nextActionButtonCoords.first = 0;
        nextActionButtonCoords.second++;
    }else{
        nextActionButtonCoords.first++;
    }
}

void MainWindow::updateTime(unsigned int ticks){
    this->ui->time->setText(QString("%1y %2d %3h %4m %5s")
                               .arg(ticks / (86400 * 365))
                               .arg((ticks / 86400) % 365)
                               .arg((ticks / 3600) % 24)
                               .arg((ticks / 60) % 60)
                               .arg(ticks % 60));
}

void MainWindow::setupSkillView(SkillModel* skillModel){
    QVBoxLayout* layout = new QVBoxLayout(ui->skillTab);

    this->skillView = new QListView(ui->skillTab);
    this->skillView->setModel(skillModel);
    this->skillView->setItemDelegate(new SkillDelegate(this->skillView));
    this->skillView->setViewMode(QListView::IconMode);
    this->skillView->setGridSize(QSize(200, 80));
    this->skillView->setSpacing(10);

    layout->addWidget(this->skillView);
}

