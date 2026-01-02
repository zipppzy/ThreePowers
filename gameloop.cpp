#include "gameloop.h"


GameLoop::GameLoop(MainWindow *mainWindow, QObject *parent):
    QObject(parent)
    , mainWindow(mainWindow)
{
    this->connectButtons();

    Action::loadActionDatabase("config/databases/action_database.toml");
    Skill::loadSkillDatabase("config/databases/skills_database.toml");
    World::loadLocationDatabase("config/databases/location_database.toml");
    Event::loadEventDatabase("config/databases/event_database.toml");

    world.loadWorldState("config/world_state.toml");

    std::shared_ptr<Location> startingLocation = world.findLocation(2).value_or(nullptr);

    player = Player(0, 100000, startingLocation);

    player.addNewReserve(Reserve{"Vigor", 10.0});
    player.findReserve("Vigor").value()->addRegen(.1);

    if(auto maybeAction = Action::checkActionDatabaseDatabase("Rest")){
        player.addGlobalAction(std::make_shared<Action>(maybeAction.value()));
    }
    //player.setRestAction(player.getGlobalActions()[0]);
    this->addActionButtons();

    this->skillModel = new SkillModel(this);
    this->skillModel->setSkillSource(&player.getSkills());
    mainWindow->setupSkillView(this->skillModel);

    this->reserveModel = new ReserveModel(this);
    this->reserveModel->setReserveSource(&player.getReserves());
    mainWindow->setupReserveView(this->reserveModel);

    this->actionQueueModel = new ActionQueueModel(&player.getActionQueue(), this);
    ActionQueueDelegate* actionQueueDelegate = new ActionQueueDelegate(this);
    connect(actionQueueDelegate, &ActionQueueDelegate::removeRequested, this, [this](int row) {
        player.removeActionFromQueue(row, 1);
        actionQueueModel->refresh();
    });
    connect(actionQueueDelegate, &ActionQueueDelegate::moveUpRequested, this, [this](int row) {
        player.moveUpActionInQueue(row);
        actionQueueModel->refresh();
    });
    mainWindow->setupActionQueueView(this->actionQueueModel, actionQueueDelegate);

    startTimer();
}

void GameLoop::processTriggeredEffects(){
    while(auto effect = player.triggerManager.getNextEffect()){
        handleTriggeredEffect(*effect);
    }
}

void GameLoop::handleTriggeredEffect(const TriggerManager::TriggeredEffect& effect){
    if(effect.effectType == "log"){
        Logger::log(effect.effectData);
    }else if(effect.effectType == "event"){
        if(auto maybeEvent = Event::checkEventDatabase(effect.effectData)){
            EventDialog* dialog = new EventDialog(maybeEvent->get(), this->mainWindow);
            dialog->exec();
        }
    }
}

void GameLoop::connectButtons(){
    connect(mainWindow->getPauseButton(),&QPushButton::clicked, this , &GameLoop::pause);
    connect(mainWindow->getPlayButton(), &QPushButton::clicked, this , &GameLoop::play);
    connect(mainWindow->getSkipButton(), &QPushButton::clicked, this, &GameLoop::skip);
}

void GameLoop::addActionButton(std::shared_ptr<Action> action){
    ActionButton* btn = new ActionButton(action);
    connect(btn, &ActionButton::tryStartAction, this, [this, action](){
        this->player.addActionToQueue(action, 1);
        this->play();
    });
    connect(btn, &ActionButton::tryStartMultipleActions, this, [this, action](int count){
        this->player.addActionToQueue(action, count);
        this->play();
    });
    connect(btn, &ActionButton::setRestAction, this, [this](std::shared_ptr<Action> action){
        this->player.setRestAction(action);
    });
    this->actionButtons.push_back(btn);
    mainWindow->addActionButton(btn);
}

void GameLoop::addActionButtons(){
    for(const auto& action : this->player.getGlobalActions()){
        this->addActionButton(action);
    }
    for(const auto& action : this->player.getCurrentLocation()->getActions()){
        this->addActionButton(action);
    }
}

void GameLoop::updateUI(){
    for(ActionButton* btn : this->actionButtons){
        btn->updateProgress();
    }

    if(player.movedLocation){
        this->mainWindow->clearActionButtons();
        this->actionButtons.clear();
        this->addActionButtons();
        player.movedLocation = false;
    }

    this->actionQueueModel->refresh();
    this->reserveModel->refreshAll();

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
    this->mainWindow->updateTime(this->ticks);

    while(!Logger::logMessages.empty()){
        this->mainWindow->logMessage(Logger::logMessages.front());
        Logger::logMessages.pop_front();
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
    elapsedTimer.restart();
}

void GameLoop::pause()
{
    paused = true;
}

void GameLoop::skip(){
    this->player.fastForwardQueue();
}

void GameLoop::loop()
{
    if(paused){
        this->updateUI();
        return;
    }

    double realDeltaTime = elapsedTimer.elapsed() / 1000.0; //time in seconds
    int numTicks = static_cast<int>(std::floor(realDeltaTime*timeScale));   //number of ticks to simulate this loop
    if(numTicks <= 0) return;

    player.tick(numTicks);

    this->processTriggeredEffects();

    if(player.getActionQueue().empty()){
        this->pause();
    }
    ticks += numTicks;
    this->updateUI();
    elapsedTimer.restart();
}
