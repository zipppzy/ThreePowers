#include "gameloop.h"

GameLoop::GameLoop(QObject *parent):QObject(parent)
{
    Action::loadActionDatabase("action_database.toml");
    Location startingLocation = Location("Village", nullptr);
    auto maybeAction = Action::checkActionDatabaseDatabase("Meditate");
    std::shared_ptr<Action> actionPtr;
    if(maybeAction.has_value()){
        actionPtr = std::make_shared<Action>(maybeAction->get());
    }
    startingLocation.addAction(actionPtr);
    player = Player(0, 100000, startingLocation);
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
