#include "gameloop.h"

GameLoop::GameLoop(QObject *parent):QObject(parent)
{
    startTimer();
}

void GameLoop::startTimer()
{
    QObject::connect(timer, &QTimer::timeout, this, &GameLoop::loop);
    timer->start(1);
}

void GameLoop::play()
{
    paused = false;
}

void GameLoop::pause()
{
    paused = true;
}

void GameLoop::loop()
{
    if(!paused)
    {
        ticks++;
        qDebug()<<ticks;
    }
}
