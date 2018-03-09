#ifndef KEYFRAMEWIDGET_H
#define KEYFRAMEWIDGET_H

#include <QWidget>
#include <vector>
#include <QCloseEvent>

#include "source/globaldefine/mainStructure.h"
#include "source/globaldefine/upDefine.h"

using namespace std;

class QTableWidget;
class QString;
class QHBoxLayout;

namespace Ui {
class KeyFrameWidget;
}

class KeyFrameWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit KeyFrameWidget(QWidget *parent = 0);
    ~KeyFrameWidget();

    QTableWidget* setTableWidgetPointer();

    bool SetNextWorkInfo(int Type = InsertType,int TimePos=0,int NewTimePos=0);
    void ServoDataToBuffer(vector<pServoInfo>& ServoData);

    bool Work();
    int FindTimePos(int TimePos);
    void SetUpdateInfo(int ServoId,int TimePos,int NewPosition);
    void initTableWidget();
    void initScene();
    void closeEvent(QCloseEvent *event);

protected:
    bool Insert();
    bool Update_Move();
    bool Update_Load();
    bool Delete();
    bool Select();
    void ClearWorkInfo();
    void SortTableWidget();

private:
    void groupSignals();
    void groupWarning(QString str);

    void initServoInformation();
    void buildServoInformation();

signals:
    void openKeyPointSignal();
    void servoTypeChanged(unsigned char type);
    void servoIdVectorChanged(vector<int> Id);
    void getSomeServoPosSignal(vector<unsigned char> Id);
    void getInforFromSliderSignal();
    void setUpdateInfoSignal();
    void gotoSignal(vector<ServoInfo> data);
    void readySendMessage(unsigned char* ins,int length);
    void createKeyPosTableWidget(QTableWidget* p);

public slots:
    void openKeyPointSlot(QString str);

    void KeyFrame_InsertKeyFrame(int TimePos); //get pos
    void KeyFrame_Update_Load(int TimePos);    //move to left or right
    void KeyFrame_Update_Move(int OldTimePos,int NewTimePos);  //move up or down
    void KeyFrame_Update_Modify(int ServoId, int TimePos, int newData);
    void KeyFrame_DeleteraKeyFrame(int TimePos);
    void KeyFrame_Goto(int TimePos);

    void getInforFromSliderSlot(vector<pServoInfo> data);
    void setServoIdVectorSlot(vector<int> Id);
    void setDigitalPosSlot(vector<pServoInfo> data);
    void setServoTypeSlot(unsigned char type);
    
private:
    Ui::KeyFrameWidget *ui;

    //tools--ui
    QTableWidget* keyPosTableWIdget;
    QHBoxLayout* layout;

    QString fileData;
    unsigned char servoType;
    unsigned char lastServoType;
    vector<int> analogId;
    vector<int> digitalId;
    vector<int> servoIdVector;

    vector<pServoInfo> analogPos;
    vector<ServoInfo> ServoDataBuffer;

    int NextWorkType;
    int WorkTimePos;
    int UpdateTimePos;
    int SelectTimePos;

    bool isInit;
};

#endif // KEYFRAMEWIDGET_H
