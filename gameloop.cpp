#include "gameloop.h"


GameLoop::GameLoop(MainWindow *mainWindow, QObject *parent):
    QObject(parent)
    , mainWindow(mainWindow)
{
    this->connectButtons();

    Action::loadActionDatabase("action_database.toml");
    Skill::loadSkillDatabase("skills_database.toml");
    World::loadLocationDatabase("location_database.toml");

    world.loadWorldState("world_state.toml");

    std::shared_ptr<Location> startingLocation = world.findLocation(1).value_or(nullptr);

    player = Player(0, 100000, startingLocation);

    for(auto& action : player.getCurrentLocation()->getActions()){
        this->addActionButton(action);
    }

    this->skillModel = new SkillModel(this);
    this->skillModel->setSkillSource(&player.getSkills());
    mainWindow->setupSkillView(this->skillModel);

    player.addSkillXp("Concentration", 215);

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

void GameLoop::updateUI(){
    for(ActionButton* btn : this->actionButtons){
        btn->updateProgress();
    }
    if(!player.updatedSkillIndexes.empty()){
        for(int i : player.updatedSkillIndexes){
            emit this->skillModel->dataChanged(
                skillModel->index(i,0),
                skillModel->index(i,0),
                {SkillModel::XpRole, SkillModel::LevelRole});
        }
        player.updatedSkillIndexes.clear();
    }
    if(!player.newSkillIndexes.empty()){
        for(int i : player.newSkillIndexes){
            skillModel->onSkillAdded(i);
        }
        player.newSkillIndexes.clear();
    }
}

void GameLoop::startTimer()
{
    QObject::connect(timer, &QTimer::timeout, this, &GameLoop::loop);
    // should be approximately 100 FPS
    timer->start(10);
    elapsedTimer.start();
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
    if(paused) return;

    double realDeltaTime = elapsedTimer.elapsed() / 1000.0; //time in seconds
    int numTicks = static_cast<int>(std::floor(realDeltaTime*timeScale));   //number of ticks to simulate this loop
    if(numTicks <= 0) return;

    player.tick(numTicks);
    this->updateUI();
    elapsedTimer.restart();
}
