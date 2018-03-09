#include "serverdebug.h"
#include "ui_serverdebug.h"

#include "source/servodebug/Graph/graphscene.h"
#include "source/insanalysis/communicateDefine.h"

#include <QGraphicsView>
#include <QPushButton>
#include <QGridLayout>
#include <QInputDialog>
#include <QMessageBox>

ServerDebug::ServerDebug(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerDebug)
{
    ui->setupUi(this);
    toolsState = false;
    zhScene = NULL;

    initScene();
    controlTools();
    setWindowTitle(QObject::tr("ServoDebug"));
}

ServerDebug::~ServerDebug()
{
    delete ui;
}

void ServerDebug::initScene()
{
    graphicsView = new QGraphicsView(this);
    requestButton = new QPushButton(this);
    requestButton->setText(QObject::tr("pingAllServo"));
    enableAllButton = new QPushButton(this);
    enableAllButton->setText(QObject::tr("enableAllServo"));
    disenableAllButton = new QPushButton(this);
    disenableAllButton->setText(QObject::tr("disEnableAllServo"));
    enableSomeButton = new QPushButton(this);
    enableSomeButton->setText(QObject::tr("enableSomeButton"));
    disenableSomeButton = new QPushButton(this);
    disenableSomeButton->setText(QObject::tr("disEnableSomeButton"));

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(graphicsView,0,0,4,3);
    gridLayout->addWidget(requestButton,4,0,2,1);
    gridLayout->addWidget(enableAllButton,4,1);
    gridLayout->addWidget(disenableAllButton,4,2);
    gridLayout->addWidget(enableSomeButton,5,1);
    gridLayout->addWidget(disenableSomeButton,5,2);

    setLayout(gridLayout);

    groupSignals();
}

void ServerDebug::initGraphScene()
{
    if(zhScene)
    {
        delete zhScene;
        zhScene = NULL;
        graphicsView->clearFocus();
    }
    zhScene = new GraphScene(this);
    graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    graphicsView->setScene(zhScene);
    emit zhSceneSignals(zhScene);
    emit zhSceneCreated(zhScene);
}

void ServerDebug::groupSignals()
{
    connect(requestButton,SIGNAL(clicked()),this,SLOT(requestButtonSlot()),Qt::UniqueConnection);
    connect(enableAllButton,SIGNAL(clicked()),this,SLOT(enableAllButtonSlot()),Qt::UniqueConnection);
    connect(disenableAllButton,SIGNAL(clicked()),this,SLOT(disenableAllButtonSlot()),Qt::UniqueConnection);
    connect(enableSomeButton,SIGNAL(clicked()),this,SLOT(enableSomeButtonSlot()),Qt::UniqueConnection);
    connect(disenableSomeButton,SIGNAL(clicked()),this,SLOT(disenableSomeButtonSlot()),Qt::UniqueConnection);
}

void ServerDebug::requestButtonSlot()
{
    toolsState = false;
    controlTools();
    initGraphScene();
    emit createKeyPosWidget();
    emit requestInformation();
}

void ServerDebug::enableAllButtonSlot()
{
    emit enableAllInformation();
}

void ServerDebug::disenableAllButtonSlot()
{
    emit disenableAllInformation();
}

void ServerDebug::enableSomeButtonSlot()
{
    vector<int> Id;
    QString Sequence = QInputDialog::getText(this,tr("servoId(split with space)"),tr("put in:"));
    if(Sequence.isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("servoId is empty!"));
        return;
    }
    QStringList test = Sequence.split(" ",QString::SkipEmptyParts);
    for(int i=0;i<(int)test.size();i++)
    {
        Id.push_back(test.at(i).toInt());
    }
    emit enableSomeInformation(Id);
}

void ServerDebug::disenableSomeButtonSlot()
{
    vector<int> Id;
    QString Sequence = QInputDialog::getText(this,tr("servoId(split with space)"),tr("put in:"));
    if(Sequence.isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("servoId is empty!"));
        return;
    }
    QStringList test = Sequence.split(" ",QString::SkipEmptyParts);
    for(int i=0;i<(int)test.size();i++)
    {
        Id.push_back(test.at(i).toInt());
    }
    emit disenableSomeInformation(Id);
}

void ServerDebug::controlTools()
{
    requestButton->setDisabled(toolsState);
    enableAllButton->setDisabled(!toolsState);
    disenableAllButton->setDisabled(!toolsState);
    enableSomeButton->setDisabled(!toolsState);
    disenableSomeButton->setDisabled(!toolsState);
}

void ServerDebug::controlButtonState(unsigned char type)
{
    if(type==DigitalServoType)
    {
        toolsState = true;
        controlTools();
        requestButton->setDisabled(!toolsState);
    }
}

void ServerDebug::closeEvent(QCloseEvent *event)
{
    showMinimized();
    event->ignore();
}
