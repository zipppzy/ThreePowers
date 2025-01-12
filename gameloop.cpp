#include "gameloop.h"

GameLoop::GameLoop(QObject *parent):QObject(parent)
{
    player = Player(0, 100000, new Location("Village"), new Action());
    Skill::loadSkillDatabase("skills_database.toml");
    qDebug()<<Skill::checkSkillDatabase("Concentration").name;
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
