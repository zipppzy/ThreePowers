#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QTableView>
#include <QHeaderView>
#include <qgridlayout.h>
#include "actionbutton.h"
#include "actionqueuemodel.h"
#include "inventorymodel.h"
#include "inventorydelegate.h"
#include "messagelog.h"
#include "skillmodel.h"
#include "skilldelegate.h"
#include "actionqueuedelegate.h"
#include "reservemodel.h"
#include "reservedelegate.h"

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
    QPushButton* getSkipButton() const;

    void updateTime(unsigned int ticks);

    void addActionButton(ActionButton* btn);
    void clearActionButtons();

    void logMessage(std::string& message);

    void setupSkillView(SkillModel* skillModel);
    void setupActionQueueView(ActionQueueModel* actionQueueModel, ActionQueueDelegate *actionQueueDelegate);
    void setupReserveView(ReserveModel* reserveModel);
    void setupInventoryView(InventoryModel* inventoryModel);

private:
    Ui::MainWindow *ui;

    QWidget* buttonContainer = nullptr;
    QGridLayout* gridLayout = nullptr;
    MessageLog* messageLogWidget = nullptr;

    std::pair<int, int> nextActionButtonCoords;  //position in gridLayout (x,y)

    QListView* skillView = nullptr;
    QListView* actionQueueView = nullptr;
    QListView* reserveView = nullptr;
    QTableView* inventoryView = nullptr;

};
#endif // MAINWINDOW_H
