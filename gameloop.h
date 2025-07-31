#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "player.h"
#include "world.h"
#include "mainwindow.h"
#include <QObject>
#include <QTimer>
#include <QDebug>

//Should handle timing of updates, play, pause, fastforward,
class GameLoop : public QObject
{
    Q_OBJECT
private:
    MainWindow* mainWindow;
    QTimer *timer = new QTimer(this);
    bool paused;
    //smallest usable unit of time
    unsigned int ticks = 0;
    Player player;
    std::vector<ActionButton*> actionButtons;

    void connectButtons();
    void addActionButton(std::shared_ptr<Action> action);

public:
    GameLoop(MainWindow* mainWindow, QObject *parent = nullptr);
    void startTimer();
public slots:
    void loop();
    void play();
    void pause();
};

#endif // GAMELOOP_H
