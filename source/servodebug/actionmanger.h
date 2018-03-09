#ifndef ACTIONMANGER_H
#define ACTIONMANGER_H

#include <QObject>

#include "source/globaldefine/mainStructure.h"
#include "source/servodebug/SplineSample/myspline.h"

class QTableWidget;
class MySpline;
class GraphScene;
class QString;

class ActionManger : public QObject
{
    Q_OBJECT
public:
    explicit ActionManger(QTableWidget *parent = 0);
    ~ActionManger();

    void refreshKeyInfor();
    void refreshInterpInfor();
    void groupSignals();
    
private:
    void saveKeyPoint(QString str);
    void saveMiniAction(QString str);

    void groupWarning(QString str);
    int combineInstruction(InstrFrameInfo InstrInfo,quint8 * Instr);
    quint8 checkSum(int length,quint8 instruction[]);

    void initAcitonChannelInfo(vector<int> Id);
    void clearAcitonChannelInfo(int num);

signals:
    void servoIdChanged();
    void analogDebugStart();
    void mixtureDebugStart();
    void createSecneSignal();
    
public slots:
    void setTimeSpaceSlot(int time);
    void saveFileSlot(QString str1,QString str2);
    void refreshSlot();

    void getDigitalIdSlot(vector<int> Id);
    void getAnalogIdSlot(vector<int> Id);
    void getServoTypeSlot(unsigned char type);

    void setServoIdVector();
    void getzhScenePointer(GraphScene* p);
    void getzhTableWidgetPointer(QTableWidget* p);

private:
    MySpline spline;
    GraphScene* zhScene;
    QTableWidget* zhTableWidget;
    AcitonChannelInfo* zhActionChannelInfo;

    quint8 timeSpace;
    int intSpace;

    vector<int> servoIdVector;
    vector<int> analogId;
    vector<int> digitalId;
    unsigned char servoType;
    int servoNumber;
};

#endif // ACTIONMANGER_H
