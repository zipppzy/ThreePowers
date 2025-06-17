#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "player.h"
#include "world.h"
#include <QObject>
#include <QTimer>
#include <QDebug>

//Should handle timing of updates, play, pause, fastforward,
class GameLoop : public QObject
{
    Q_OBJECT
private:
    QTimer *timer = new QTimer(this);
    bool paused;
    //smallest usable unit of time
    unsigned int ticks = 0;
    Player player;
public:
    GameLoop(QObject *parent = nullptr);
    void startTimer();
public slots:
    void loop();
    void play();
    void pause();
};

#endif // GAMELOOP_H
