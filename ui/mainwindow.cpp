#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sectionsContainer = new QWidget();
    sectionsLayout = new QVBoxLayout(sectionsContainer);
    sectionsLayout->setAlignment(Qt::AlignTop);
    sectionsContainer->setLayout(sectionsLayout);
    ui->actionScrollArea->setWidget(sectionsContainer);
    ui->actionScrollArea->setWidgetResizable(true);
    ui->actionScrollArea->setWidgetResizable(true);

    this->messageLogWidget = new MessageLog(this);

    auto* layout = new QVBoxLayout(ui->messageLogContainer);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(messageLogWidget, 0);

    auto* researchTabPage = new QWidget();
    ui->tabWidget->addTab(researchTabPage, "Research");
    auto* researchTabLayout = new QVBoxLayout(researchTabPage);
    researchTabLayout->setContentsMargins(0, 0, 0, 0);
    researchTab = new ResearchTabWidget(researchTabPage);
    researchTabLayout->addWidget(researchTab);


    ui->tabWidget->setCurrentIndex(0);
    this->showMaximized();
}

MainWindow::~MainWindow(){
    delete ui;
}

QPushButton* MainWindow::getPauseButton() const{
    return ui->pauseButton;
}

QPushButton* MainWindow::getPlayButton() const{
    return ui->playButton;
}

QPushButton* MainWindow::getSkipButton() const{
    return ui->skipButton;
}

void MainWindow::addActionButton(ActionButton* btn, const QString& section){
    if (!actionSections.contains(section)) {
        auto* s = new CollapsibleActionSection(section);
        sectionsLayout->addWidget(s);
        actionSections[section] = s;
    }
    actionSections[section]->addButton(btn);
}

void MainWindow::clearActionButtons() {
    for (auto* section : actionSections) {
        section->setParent(nullptr);
        delete section;
    }
    actionSections.clear();
}

void MainWindow::logMessage(std::string& message){
    this->messageLogWidget->addMessage(QString::fromStdString(message));
}

void MainWindow::updateTime(unsigned int ticks){
    this->ui->time->setText(QString("%1y %2d %3h %4m %5s")
                               .arg(ticks / (86400 * 365))
                               .arg((ticks / 86400) % 365)
                               .arg((ticks / 3600) % 24)
                               .arg((ticks / 60) % 60)
                               .arg(ticks % 60));
}

void MainWindow::setupSkillView(SkillModel* skillModel){
    QVBoxLayout* layout = new QVBoxLayout(ui->skillTab);

    this->skillView = new QListView(ui->skillTab);
    this->skillView->setModel(skillModel);
    this->skillView->setItemDelegate(new SkillDelegate(this->skillView));
    this->skillView->setViewMode(QListView::IconMode);
    this->skillView->setGridSize(QSize(200, 80));
    this->skillView->setSpacing(10);

    layout->addWidget(this->skillView);
}

void MainWindow::setupActionQueueView(ActionQueueModel* actionQueueModel, ActionQueueDelegate* actionQueueDelegate){
    this->actionQueueView = new QListView(this);

    connect(actionQueueDelegate, &ActionQueueDelegate::hoverChanged, actionQueueView, QOverload<>::of(&QListView::update));

    actionQueueView->setModel(actionQueueModel);
    actionQueueView->setItemDelegate(actionQueueDelegate);
    actionQueueView->setMouseTracking(true);
    actionQueueView->setSelectionMode(QAbstractItemView::NoSelection);
    actionQueueView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    actionQueueView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    actionQueueView->setMaximumWidth(300);

    ui->leftVerticalLayout->addWidget(actionQueueView);
}

void MainWindow::setupReserveView(ReserveModel* reserveModel){

    this->reserveView = new QListView(this);
    this->reserveView->setModel(reserveModel);
    this->reserveView->setItemDelegate(new ReserveDelegate(this->reserveView));
    this->reserveView->setViewMode(QListView::ListMode);
    this->reserveView->setSpacing(5);

    ui->rightVerticalLayout->layout()->addWidget(this->reserveView);
}

void MainWindow::setupInventoryView(InventoryModel* inventoryModel){
    this->inventoryView = new QTableView(ui->inventoryTab);

    this->inventoryView->setModel(inventoryModel);
    this->inventoryView->setItemDelegate(new InventoryDelegate(this->inventoryView));

    // Enable sorting
    this->inventoryView->setSortingEnabled(true);
    this->inventoryView->sortByColumn(0, Qt::AscendingOrder);

    // Configure table appearance
    this->inventoryView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->inventoryView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->inventoryView->setAlternatingRowColors(false);
    this->inventoryView->verticalHeader()->setVisible(false);
    this->inventoryView->horizontalHeader()->setStretchLastSection(true);

    // Set column widths
    this->inventoryView->setColumnWidth(0, 200); // Name
    this->inventoryView->setColumnWidth(1, 80);  // Count
    this->inventoryView->setColumnWidth(2, 120); // Weight
    this->inventoryView->setColumnWidth(3, 120); // Volume

    QVBoxLayout* layout = new QVBoxLayout(ui->inventoryTab);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this->inventoryView);
}

void MainWindow::setupResearchTab(const std::unordered_map<std::string, ResearchTopic>* topics){
    if(researchTab) {
        researchTab->setTopics(topics);
    }
}

void MainWindow::refreshResearchTab(){
    if(researchTab){
        researchTab->refresh();
    }
}

void MainWindow::addResearchActionButton(ActionButton* btn){
    if(researchTab){
        researchTab->addActionButton(btn);
    }
}

void MainWindow::clearResearchActionButtons() {
    if(researchTab){
        researchTab->clearActionButtons();
    }
}

