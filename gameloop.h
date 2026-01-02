#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "player.h"
#include "skillmodel.h"
#include "actionqueuemodel.h"
#include "world.h"
#include "mainwindow.h"
#include "logger.h"
#include "reservemodel.h"
#include "event.h"
#include "eventdialog.h"
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
    double timeScale = 50;
    //each tick is a second of game time
    unsigned int ticks = 0;

    Player player;
    World world;

    void processTriggeredEffects();
    void handleTriggeredEffect(const TriggerManager::TriggeredEffect& effect);

    std::vector<ActionButton*> actionButtons;
    SkillModel* skillModel = nullptr;
    ActionQueueModel* actionQueueModel = nullptr;
    ReserveModel* reserveModel = nullptr;

    void connectButtons();
    void addActionButton(std::shared_ptr<Action> action);
    void addActionButtons();

    void updateUI();

public:
    GameLoop(MainWindow* mainWindow, QObject *parent = nullptr);
    void startTimer();
public slots:
    void loop();
    void play();
    void pause();
    void skip();
};

#endif // GAMELOOP_H
