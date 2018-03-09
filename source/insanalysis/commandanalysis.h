#ifndef COMMANDANALYSIS_H
#define COMMANDANALYSIS_H

#include <QObject>
#include <vector>
#include <QStringList>

#include "source/globaldefine/mainStructure.h"

using namespace std;

class MainWindow;
class SerialDialog;
class NetWorkDialog;
class QString;
class QStringList;

class CommandAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit CommandAnalysis(QObject *parent = 0);
    ~CommandAnalysis();

    void getMainWindowPointer(MainWindow* p);
    void getNetWorkPointer(NetWorkDialog* p);
    void getSerialPointer(SerialDialog* p);

    void groupWarning(QString str);

    unsigned char protocalCheckDataForIns(unsigned char* instruction,int length);
    unsigned char protocalCheckDataFromDown(QByteArray data);
    bool checkParityFromDown(QByteArray data);

    void sendInsData(unsigned char* ins,int length);

private:
    void clearinfor();

signals:
    void digitalIdChanged(vector<int> Id);
    void analogIdChanged(vector<int> Id);
    void servoTypeChanged(unsigned char type);
    void inforChangedSignal(vector<pServoInfo> infor);
    void analogInforChangedSignal(vector<pServoInfo> infor);

    void fileListChangedSignal(QStringList data);
    void replyInsStateSignal(QString str);
    void createServoDebugScene();
    
public slots:
    void getDownServoCondition();
    void enableAllServo();
    void enableSomeServo(vector<int> Id);
    void disenableAllServo();
    void disenableSomeServo(vector<int> Id);

    void analogInformationSlot(unsigned char* p,int length);
    void saveSequenceSlot(vector<int> Id);
    void execSequenceSlot();
    void deleteMiniActionSlot(QString str);
    void execMiniActionSlot(QString str);
    void upLoadMiniActionSlot();
    void downLoadMiniActionSlot(QString fileName);

    void setServoIdSlot(vector<int> Id);
    void setServoType(unsigned char type);

    void getSomeServoPosSlot(vector<unsigned char> Id);

    void setParseData(QByteArray data);

public:
    unsigned char servoType;

private:
    MainWindow* zhMainWindow;
    SerialDialog* zhSerialDialog;
    NetWorkDialog* zhNetWorkDialog;

    //data-getFromDownServe
    QByteArray completeInsData;
    QByteArray insData;
    QByteArray headerData;

    vector<unsigned char> parseData;  //thinking

    //information for servo
    //pServoInfo inforTemp;
    vector<pServoInfo> infor;
    vector<int> digitalId;
    vector<int> analogId;

    int servoNum ;

    //information for file
    QStringList fileList;

    unsigned char lastServoType;
};

#endif // COMMANDANALYSIS_H
