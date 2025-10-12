#include "gameloop.h"


GameLoop::GameLoop(MainWindow *mainWindow, QObject *parent):
    QObject(parent)
    , mainWindow(mainWindow)
{
    this->connectButtons();

    Action::loadActionDatabase("action_database.toml");
    Skill::loadSkillDatabase("skills_database.toml");
    World::loadLocationDatabase("location_database.toml");

    World world;
    world.loadWorldState("world_state.toml");

    std::shared_ptr<Location> startingLocation = world.findLocation(1).value_or(nullptr);

    player = Player(0, 100000, startingLocation);
    player.applySkillEffectsCurrentLocation();
    for(auto& action : player.getCurrentLocation()->getActions()){
        this->addActionButton(action);
    }
    startTimer();
}

void GameLoop::connectButtons(){
    connect(mainWindow->getPauseButton(),&QPushButton::clicked, this , &GameLoop::pause);
    connect(mainWindow->getPlayButton(), &QPushButton::clicked, this , &GameLoop::play);
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
