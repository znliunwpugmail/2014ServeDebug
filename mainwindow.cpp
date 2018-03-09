#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "source/communicate/network/networkdialog.h"
#include "source/communicate/serialcom/serialdialog.h"
#include "source/insanalysis/commandanalysis.h"
#include "source/logdebug/logdebug.h"
#include "source/servodebug/serverdebug.h"
#include "source/servodebug/analogdata.h"
#include "source/servodebug/keyframewidget.h"
#include "source/downserve/armmanage.h"
#include "source/servodebug/actionmanger.h"
#include "source/servodebug/Graph/graphscene.h"

#include <QToolBar>
#include <QToolButton>
#include <QMdiArea>
#include <QMessageBox>
#include <QLabel>
#include <QString>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    zhNetWork = NULL;
    zhSerial = NULL;
    zhCommandAnalysis = NULL;
    zhLogDebug = NULL;
    zhServerDebug = NULL;
    zhKeyFrameWidget = NULL;
    zhAnalogData = NULL;
    zhArmManage = NULL;
    zhActionManger = NULL;

    mainMdiArea = new QMdiArea(this);
    mainMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mainMdiArea);

    initToolBar();
    initButtonsState();
    initLogDebug();

    stateLabel = new QLabel(this);
    this->statusBar()->addWidget(stateLabel);
    stateLabel->setText(QObject::tr("Ready!"));
    setWindowTitle(QObject::tr("2013SevoDebug"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete zhCommandAnalysis;
    delete zhActionManger;
}

void MainWindow::initToolBar()
{
    mainToolBar = new QToolBar(this);

    serialButton = new QToolButton(this);
    serialButton->setIcon(QIcon(":/MainOperator-Image/serialConfig.jpg"));
    serialButton->setText(tr("SerialCom"));
    serialButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    networkButton = new QToolButton(this);
    networkButton->setIcon(QIcon(":/MainOperator-Image/networkConfig.jpg"));
    networkButton->setText(tr("NetWork"));
    networkButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    servoDebugButton = new QToolButton(this);
    servoDebugButton->setIcon(QIcon(":/MainOperator-Image/servo.jpg"));
    servoDebugButton->setText(tr("ServoDebug"));
    servoDebugButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    logrecordButton = new QToolButton(this);
    logrecordButton->setIcon(QIcon(":/MainOperator-Image/logRecord.jpg"));
    logrecordButton->setText(tr("Log"));
    logrecordButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    downserveButton = new QToolButton(this);
    downserveButton->setIcon(QIcon(":/MainOperator-Image/ARM6410.jpg"));
    downserveButton->setText(tr("DownArm"));
    downserveButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    exitButton = new QToolButton(this);
    exitButton->setIcon(QIcon(":/MainOperator-Image/Exit.jpg"));
    exitButton->setText(tr("Quit"));
    exitButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    mainToolBar->addWidget(serialButton);
    mainToolBar->addWidget(networkButton);
    mainToolBar->addWidget(servoDebugButton);
    mainToolBar->addWidget(downserveButton);
    mainToolBar->addWidget(logrecordButton);
    mainToolBar->addWidget(exitButton);

    addToolBar(mainToolBar);
    groupSignals();
}

void MainWindow::groupSignals()
{
    connect(serialButton,SIGNAL(clicked()),this,SLOT(serialButtonSlot()),Qt::UniqueConnection);
    connect(networkButton,SIGNAL(clicked()),this,SLOT(networkButtonSlot()),Qt::UniqueConnection);
    connect(servoDebugButton,SIGNAL(clicked()),this,SLOT(servoDebugButtonSlot()),Qt::UniqueConnection);
    connect(downserveButton,SIGNAL(clicked()),this,SLOT(downserveButtonSlot()),Qt::UniqueConnection);
    connect(logrecordButton,SIGNAL(clicked()),this,SLOT(logrecordButtonSlot()),Qt::UniqueConnection);
    connect(exitButton,SIGNAL(clicked()),this,SLOT(exitButtonSlot()),Qt::UniqueConnection);
}

void MainWindow::createMainWindowScene()
{

}

void MainWindow::serialButtonSlot()
{
    initSerialDialog();
}

void MainWindow::networkButtonSlot()
{
    initNetWorkDialog();
}

void MainWindow::servoDebugButtonSlot()
{
    initServerDebug();
    downserveButton->setEnabled(true);
    servoDebugButton->setDisabled(true);
}

void MainWindow::downserveButtonSlot()
{
    initArmManage();
    downserveButton->setDisabled(true);
}

void MainWindow::logrecordButtonSlot()
{
    initLogDebug();
    logrecordButton->setDisabled(true);
}

void MainWindow::exitButtonSlot()
{
    this->close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(QObject::tr("Exit the Program"));
    msgBox.setText(QObject::tr("You are exiting the Program"));
    msgBox.setInformativeText(QObject::tr("Are you sure to Exit the Program?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int buttonTemp = msgBox.exec();
    if(buttonTemp==QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::initNetWorkDialog()
{
    if(NULL==zhNetWork)
    {
        zhNetWork = new NetWorkDialog(this);
        mainMdiArea->addSubWindow(zhNetWork);
        zhNetWork->show();
        zhNetWork->setAttribute(Qt::WA_DeleteOnClose);
        networkButton->setDisabled(true);
        serialButton->setDisabled(true);
        connect(zhNetWork,SIGNAL(getConnected()),this,SLOT(createCommandAnalysis()));
        connect(zhNetWork,SIGNAL(hasSendIns(QByteArray)),zhLogDebug,SLOT(setSendIns(QByteArray)));
        connect(zhNetWork,SIGNAL(hasReplyIns(QByteArray)),zhLogDebug,SLOT(setReplyIns(QByteArray)));
        emit startConnect(QObject::tr("NetWork"));
    }
}

void MainWindow::initSerialDialog()
{
    if(NULL==zhSerial)
    {
        zhSerial = new SerialDialog(this);
        mainMdiArea->addSubWindow(zhSerial);
        zhSerial->show();
        zhSerial->setAttribute(Qt::WA_DeleteOnClose);
        networkButton->setDisabled(true);
        serialButton->setDisabled(true);
        connect(zhSerial,SIGNAL(zhComIsOpen()),this,SLOT(createCommandAnalysis()));
        connect(zhSerial,SIGNAL(hasSendIns(QByteArray)),zhLogDebug,SLOT(setSendIns(QByteArray)));
        connect(zhSerial,SIGNAL(hasReplyIns(QByteArray)),zhLogDebug,SLOT(setReplyIns(QByteArray)));
        emit startConnect(QObject::tr("SerialCom"));
    }
}

void MainWindow::createCommandAnalysis()
{
    initCommandAnalysis();
    servoDebugButton->setEnabled(true);
}

void MainWindow::createAnalogData()
{
    initAnalogData();
}

void MainWindow::initCommandAnalysis()
{
    if(NULL==zhCommandAnalysis)
    {
        zhCommandAnalysis = new CommandAnalysis;
        zhCommandAnalysis->getMainWindowPointer(this);
        if(zhNetWork!=NULL)
        {
            zhCommandAnalysis->getNetWorkPointer(zhNetWork);
            connect(zhNetWork,SIGNAL(hasReplyIns(QByteArray)),zhCommandAnalysis,SLOT(setParseData(QByteArray)));
        }
        else if(zhSerial!=NULL)
        {
            zhCommandAnalysis->getSerialPointer(zhSerial);
            connect(zhSerial,SIGNAL(hasReplyIns(QByteArray)),zhCommandAnalysis,SLOT(setParseData(QByteArray)));
        }
        else
        {
            QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("Communicate is NULL!"));
            return;
        }
        connect(zhCommandAnalysis,SIGNAL(digitalIdChanged(vector<int>)),zhLogDebug,SLOT(setDigitalNumber(vector<int>)),Qt::UniqueConnection);
        connect(zhCommandAnalysis,SIGNAL(analogIdChanged(vector<int>)),zhLogDebug,SLOT(setAnalogNumber(vector<int>)),Qt::UniqueConnection);
        connect(zhCommandAnalysis,SIGNAL(servoTypeChanged(unsigned char)),this,SLOT(setServoTypeState(unsigned char)),Qt::UniqueConnection);
        connect(zhCommandAnalysis,SIGNAL(replyInsStateSignal(QString)),zhLogDebug,SLOT(setReplyInsStateSlot(QString)),Qt::UniqueConnection);
    }
}

void MainWindow::initLogDebug()
{
    if(NULL==zhLogDebug)
    {
        zhLogDebug = new LogDebug(this);
        mainMdiArea->addSubWindow(zhLogDebug);
        zhLogDebug->show();
        connect(this,SIGNAL(startConnect(QString)),zhLogDebug,SLOT(setconnectState(QString)));
        zhLogDebug->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void MainWindow::initServerDebug()
{
    if(NULL==zhServerDebug)
    {
        zhServerDebug = new ServerDebug(this);
        mainMdiArea->addSubWindow(zhServerDebug);
        zhServerDebug->show();

        //connect(zhCommandAnalysis,SIGNAL(createServoDebugScene()),zhServerDebug,SLOT(initGraphScene()));
        connect(zhServerDebug,SIGNAL(requestInformation()),zhCommandAnalysis,SLOT(getDownServoCondition()));
        connect(zhServerDebug,SIGNAL(enableAllInformation()),zhCommandAnalysis,SLOT(enableAllServo()));
        connect(zhServerDebug,SIGNAL(disenableAllInformation()),zhCommandAnalysis,SLOT(disenableAllServo()));
        connect(zhServerDebug,SIGNAL(enableSomeInformation(vector<int>)),zhCommandAnalysis,SLOT(enableSomeServo(vector<int>)));
        connect(zhServerDebug,SIGNAL(disenableSomeInformation(vector<int>)),zhCommandAnalysis,SLOT(disenableSomeServo(vector<int>)));
        connect(zhServerDebug,SIGNAL(zhSceneSignals(GraphScene*)),this,SLOT(groupzhSceneSignals(GraphScene*)));
        connect(zhCommandAnalysis,SIGNAL(servoTypeChanged(unsigned char)),zhServerDebug,SLOT(controlButtonState(unsigned char)));
        zhServerDebug->setAttribute(Qt::WA_DeleteOnClose);
    }
    initKeyFrameWidget();
    //initscene when request servo information
    //zhServerDebug->initGraphScene();
}

void MainWindow::initKeyFrameWidget()
{
    if(NULL==zhKeyFrameWidget)
    {
        zhKeyFrameWidget = new KeyFrameWidget(this);
        mainMdiArea->addSubWindow(zhKeyFrameWidget);
        zhKeyFrameWidget->show();
        zhKeyFrameWidget->setAttribute(Qt::WA_DeleteOnClose);
        //connect(zhServerDebug,SIGNAL(createKeyPosWidget()),zhKeyFrameWidget,SLOT(initScene()));
    }
    initActionManger();
    connect(zhKeyFrameWidget,SIGNAL(servoTypeChanged(unsigned char)),zhCommandAnalysis,SLOT(setServoType(unsigned char)),Qt::UniqueConnection);
    connect(zhKeyFrameWidget,SIGNAL(servoIdVectorChanged(vector<int>)),zhCommandAnalysis,SLOT(setServoIdSlot(vector<int>)),Qt::UniqueConnection);
    connect(zhKeyFrameWidget,SIGNAL(getSomeServoPosSignal(vector<unsigned char>)),zhCommandAnalysis,SLOT(getSomeServoPosSlot(vector<unsigned char>)),Qt::UniqueConnection);
    connect(zhKeyFrameWidget,SIGNAL(readySendMessage(unsigned char*,int)),zhCommandAnalysis,SLOT(analogInformationSlot(unsigned char*,int)),Qt::UniqueConnection);
    connect(zhCommandAnalysis,SIGNAL(digitalIdChanged(vector<int>)),zhKeyFrameWidget,SLOT(setServoIdVectorSlot(vector<int>)),Qt::UniqueConnection);
    connect(zhCommandAnalysis,SIGNAL(analogIdChanged(vector<int>)),zhKeyFrameWidget,SLOT(setServoIdVectorSlot(vector<int>)),Qt::UniqueConnection);
    connect(zhCommandAnalysis,SIGNAL(inforChangedSignal(vector<pServoInfo>)),zhKeyFrameWidget,SLOT(setDigitalPosSlot(vector<pServoInfo>)),Qt::UniqueConnection);
    connect(zhCommandAnalysis,SIGNAL(servoTypeChanged(unsigned char)),zhKeyFrameWidget,SLOT(setServoTypeSlot(unsigned char)),Qt::UniqueConnection);
}

void MainWindow::initAnalogData()
{
    if(NULL==zhAnalogData)
    {
        zhAnalogData = new AnalogData(this);
        mainMdiArea->addSubWindow(zhAnalogData);
        zhAnalogData->show();
        zhAnalogData->setAttribute(Qt::WA_DeleteOnClose);
        connect(zhAnalogData,SIGNAL(readySendMessage(unsigned char*,int)),zhCommandAnalysis,SLOT(analogInformationSlot(unsigned char*,int)));
        connect(zhCommandAnalysis,SIGNAL(analogIdChanged(vector<int>)),zhAnalogData,SLOT(initScene(vector<int>)));
        connect(zhKeyFrameWidget,SIGNAL(getInforFromSliderSignal()),zhAnalogData,SLOT(getInforFromSliderSlot()));
        connect(zhAnalogData,SIGNAL(hasGetInforSlot(vector<pServoInfo>)),zhKeyFrameWidget,SLOT(getInforFromSliderSlot(vector<pServoInfo>)));
        connect(zhKeyFrameWidget,SIGNAL(gotoSignal(vector<ServoInfo>)),zhAnalogData,SLOT(gotoSlot(vector<ServoInfo>)));
        connect(zhCommandAnalysis,SIGNAL(analogInforChangedSignal(vector<pServoInfo>)),zhAnalogData,SLOT(setSliderToTargetPosition(vector<pServoInfo>)));
    }
}

void MainWindow::initActionManger()
{
    if(NULL==zhActionManger)
    {
        zhActionManger = new ActionManger(zhKeyFrameWidget->setTableWidgetPointer());
        //connect(zhKeyFrameWidget,SIGNAL(createKeyPosTableWidget(QTableWidget*)),zhActionManger,SLOT(getzhTableWidgetPointer(QTableWidget*)));
        connect(zhCommandAnalysis,SIGNAL(digitalIdChanged(vector<int>)),zhActionManger,SLOT(getDigitalIdSlot(vector<int>)));
        connect(zhCommandAnalysis,SIGNAL(analogIdChanged(vector<int>)),zhActionManger,SLOT(getAnalogIdSlot(vector<int>)));
        connect(zhCommandAnalysis,SIGNAL(servoTypeChanged(unsigned char)),zhActionManger,SLOT(getServoTypeSlot(unsigned char)));
        connect(zhKeyFrameWidget,SIGNAL(openKeyPointSignal()),zhActionManger,SLOT(refreshSlot()));
        connect(zhServerDebug,SIGNAL(zhSceneCreated(GraphScene*)),zhActionManger,SLOT(getzhScenePointer(GraphScene*)));
        connect(zhKeyFrameWidget,SIGNAL(setUpdateInfoSignal()),zhActionManger,SLOT(refreshSlot()));
        connect(zhActionManger,SIGNAL(analogDebugStart()),this,SLOT(createAnalogData()));
        connect(zhActionManger,SIGNAL(mixtureDebugStart()),this,SLOT(createAnalogData()));
    }
}

void MainWindow::initArmManage()
{
    if(NULL==zhArmManage)
    {
        zhArmManage = new ArmManage(this);
        mainMdiArea->addSubWindow(zhArmManage);
        zhArmManage->show();
        zhArmManage->setAttribute(Qt::WA_DeleteOnClose);
    }
    initKeyFrameWidget();
    connect(zhArmManage,SIGNAL(openFileSignal(QString)),zhKeyFrameWidget,SLOT(openKeyPointSlot(QString)),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(downSequenceSignal(vector<int>)),zhCommandAnalysis,SLOT(saveSequenceSlot(vector<int>)),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(executeSequenceSignal()),zhCommandAnalysis,SLOT(execSequenceSlot()),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(deleteMiniActionSignal(QString)),zhCommandAnalysis,SLOT(deleteMiniActionSlot(QString)),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(playMiniActionSignal(QString)),zhCommandAnalysis,SLOT(execMiniActionSlot(QString)),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(uploadMiniActionSignal()),zhCommandAnalysis,SLOT(upLoadMiniActionSlot()),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(downloadMiniActionSignal(QString)),zhCommandAnalysis,SLOT(downLoadMiniActionSlot(QString)),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(timeSpaceChanged(int)),zhActionManger,SLOT(setTimeSpaceSlot(int)),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(saveFileSignal(QString,QString)),zhActionManger,SLOT(saveFileSlot(QString,QString)),Qt::UniqueConnection);
    connect(zhArmManage,SIGNAL(coverDataSignal()),zhActionManger,SLOT(refreshSlot()),Qt::UniqueConnection);
    connect(zhCommandAnalysis,SIGNAL(fileListChangedSignal(QStringList)),zhArmManage,SLOT(getUpLoadFileNameSlot(QStringList)),Qt::UniqueConnection);
}

void MainWindow::groupzhSceneSignals(GraphScene* p)
{
    if(NULL==zhKeyFrameWidget)
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("KeyFrameWidget is NULL!"));
        return;
    }
    connect(p,SIGNAL(keyFrameDelete(int)),zhKeyFrameWidget,SLOT(KeyFrame_DeleteraKeyFrame(int)),Qt::UniqueConnection);
    connect(p,SIGNAL(keyFrameGoto(int)),zhKeyFrameWidget,SLOT(KeyFrame_Goto(int)),Qt::UniqueConnection);
    connect(p,SIGNAL(keyFrameLoad(int)),zhKeyFrameWidget,SLOT(KeyFrame_Update_Load(int)),Qt::UniqueConnection);
    connect(p,SIGNAL(keyFrameModify(int,int,int)),zhKeyFrameWidget,SLOT(KeyFrame_Update_Modify(int,int,int)),Qt::UniqueConnection);
    connect(p,SIGNAL(keyFrameMove(int,int)),zhKeyFrameWidget,SLOT(KeyFrame_Update_Move(int,int)),Qt::UniqueConnection);
    connect(p,SIGNAL(insertKeyFrame(int)),zhKeyFrameWidget,SLOT(KeyFrame_InsertKeyFrame(int)),Qt::UniqueConnection);
}

void MainWindow::initButtonsState()
{
    servoDebugButton->setDisabled(true);
    downserveButton->setDisabled(true);
    logrecordButton->setDisabled(true);
}

void MainWindow::setServoTypeState(unsigned char type)
{
    stateLabel->setText(QObject::tr("ServoType:%1").arg((int)type));
}
