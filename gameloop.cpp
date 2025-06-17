#include "gameloop.h"


GameLoop::GameLoop(QObject *parent):QObject(parent)
{
    Action::loadActionDatabase("action_database.toml");
    World world;
    world.addLocation("Village", 0);
    auto maybeAction = Action::checkActionDatabaseDatabase("Meditate");
    std::shared_ptr<Action> actionPtr;
    if(maybeAction.has_value()){
        actionPtr = std::make_shared<Action>(maybeAction->get());
    }
    std::shared_ptr<Location> startingLocation = world.findLocation(1).value_or(nullptr);
    startingLocation->addAction(actionPtr);
    player = Player(0, 100000, startingLocation.get());
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
