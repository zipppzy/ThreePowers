#include "gameloop.h"


GameLoop::GameLoop(QObject *parent):QObject(parent)
{
    Action::loadActionDatabase("action_database.toml");
    World world;
    world.addLocation("Village", 0);

    std::shared_ptr<Location> startingLocation = world.findLocation(1).value_or(nullptr);

    auto maybeAction = Action::checkActionDatabaseDatabase("Meditate");
    if(maybeAction.has_value()){
        Action action = maybeAction->get();
        startingLocation->addAction(action);
    }

    player = Player(0, 100000, startingLocation);
    player.startAction(startingLocation->getActions()[0]);
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
        if(ticks%10){
            player.tick();
        }
        ticks++;
        qDebug()<<ticks;
    }
}
