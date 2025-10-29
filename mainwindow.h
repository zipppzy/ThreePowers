#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <qgridlayout.h>
#include "actionbutton.h"
#include "skillmodel.h"

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

    QPushButton* getPauseButton() const;
    QPushButton* getPlayButton() const;

    void updateTime(unsigned int ticks);

    void addActionButton(ActionButton* btn);

    void setupSkillView(SkillModel* skillModel);

private:
    Ui::MainWindow *ui;

    QWidget* buttonContainer = nullptr;
    QGridLayout* gridLayout = nullptr;

    std::pair<int, int> nextActionButtonCoords;  //position in gridLayout (x,y)

    QListView* skillView = nullptr;

};
#endif // MAINWINDOW_H
