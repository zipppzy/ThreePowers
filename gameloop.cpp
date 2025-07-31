#include "gameloop.h"


GameLoop::GameLoop(MainWindow *mainWindow, QObject *parent):
    QObject(parent)
    , mainWindow(mainWindow)
{
    Action::loadActionDatabase("action_database.toml");
    Skill::loadSkillDatabase("skills_database.toml");
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
    this->addActionButton(startingLocation->getActions()[0]);
    startTimer();
}

void GameLoop::addActionButton(std::shared_ptr<Action> action){
    ActionButton* btn = new ActionButton(action);
    connect(btn, &ActionButton::tryStartAction, this, [this, action](){
        this->player.startAction(action);
    });
    this->actionButtons.push_back(btn);
    mainWindow->addActionButton(btn);
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
        if(ticks%100 == 0){
            player.tick();
            for(ActionButton* btn : this->actionButtons){
                btn->updateProgress();
            }
        }
        ticks++;
        qDebug()<<ticks;
    }
}
