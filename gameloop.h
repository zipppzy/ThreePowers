#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "player.h"
#include "skillmodel.h"
#include "world.h"
#include "mainwindow.h"
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>

//Inbetween for UI and gameLogic
//Should handle timing of updates, play, pause, fastforward
class GameLoop : public QObject
{
    Q_OBJECT
private:
    MainWindow* mainWindow;
    QTimer* timer = new QTimer(this);
    QElapsedTimer elapsedTimer;
    bool paused = false;

    //ticks per real life second
    double timeScale = 50.0;
    //each tick is a second of game time
    unsigned int ticks = 0;

    Player player;
    World world;

    std::vector<ActionButton*> actionButtons;
    SkillModel* skillModel;

    void connectButtons();
    void addActionButton(std::shared_ptr<Action> action);

    void updateUI();

public:
    GameLoop(MainWindow* mainWindow, QObject *parent = nullptr);
    void startTimer();
public slots:
    void loop();
    void play();
    void pause();
};

#endif // GAMELOOP_H
