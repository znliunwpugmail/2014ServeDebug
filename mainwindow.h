#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

class QToolBar;
class QToolButton;
class QMdiArea;
class QLabel;
class NetWorkDialog;
class SerialDialog;
class CommandAnalysis;
class LogDebug;
class ServerDebug;
class AnalogData;
class KeyFrameWidget;
class ArmManage;
class ActionManger;
class GraphScene;

class QString;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initToolBar(); //Each toolBar has its parents,we do not need to delete it.
    void createMainWindowScene();
    void groupSignals();

    //handle various events
    void closeEvent(QCloseEvent *event);

    //init children class
    void initNetWorkDialog();
    void initSerialDialog();
    void initCommandAnalysis();
    void initLogDebug();
    void initServerDebug();
    void initKeyFrameWidget();
    void initAnalogData();
    void initActionManger();
    void initArmManage();
    void initButtonsState();

private slots:
    void serialButtonSlot();
    void networkButtonSlot();
    void servoDebugButtonSlot();
    void downserveButtonSlot();
    void logrecordButtonSlot();
    void exitButtonSlot();

    void createCommandAnalysis();
    void createAnalogData();

    void groupzhSceneSignals(GraphScene* p);
    void setServoTypeState(unsigned char type);

signals:
    void startConnect(QString str);

    //connect out
public:
    //children class
    NetWorkDialog* zhNetWork;
    SerialDialog* zhSerial;
    CommandAnalysis* zhCommandAnalysis;
    ServerDebug* zhServerDebug;
    KeyFrameWidget* zhKeyFrameWidget;
    AnalogData* zhAnalogData;
    ArmManage* zhArmManage;
    ActionManger* zhActionManger;

private:
    Ui::MainWindow *ui;
    QMdiArea* mainMdiArea;

    //MainWindow Scene Tools
    QToolBar* mainToolBar;
    QToolButton* serialButton;
    QToolButton* networkButton;
    QToolButton* servoDebugButton;
    QToolButton* logrecordButton;
    QToolButton* downserveButton;
    QToolButton* exitButton;

    //show State
    QLabel* stateLabel;

    //children class
    LogDebug* zhLogDebug;
};

#endif // MAINWINDOW_H
