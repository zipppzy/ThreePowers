#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qgridlayout.h>
#include "actionbutton.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define MAX_ACTION_BUTTON_COLS 5

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addActionButton(std::shared_ptr<Action> action);



private:
    Ui::MainWindow *ui;

    QWidget *buttonContainer = nullptr;
    QGridLayout *gridLayout = nullptr;

    std::pair<int, int> nextActionButtonCoords;  //position in gridLayout (x,y)

};
#endif // MAINWINDOW_H
