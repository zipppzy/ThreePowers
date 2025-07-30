#include "mainwindow.h"
#include "gameloop.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    GameLoop gameLoop{&mainWindow};
    return a.exec();
}
