#ifndef ANALOGDATA_H
#define ANALOGDATA_H

#include <QWidget>
#include <vector>
#include <QCloseEvent>

using namespace std;

#include "source/insanalysis/communicateDefine.h"
#include "source/globaldefine/mainStructure.h"

class QSlider;
class QSpinBox;
class QLabel;
class QString;

namespace Ui {
class AnalogData;
}

class AnalogData : public QWidget
{
    Q_OBJECT
    
public:
    explicit AnalogData(QWidget *parent = 0);
    ~AnalogData();

    void clearSceneTools();
    void groupWarning(QString str);

    //move the slider to the target position
    void moveSliderToTargetPosition(vector<ServoInfo> data);

    //read the now all the pos from the enable slider
    vector<pServoInfo> readPosfromSlider();

    void closeEvent(QCloseEvent *event);

public slots:
    void setSendData();
    void initScene(vector<int> Id);
    void getInforFromSliderSlot();
    void gotoSlot(vector<ServoInfo> data);

    void setSliderToTargetPosition(vector<pServoInfo> posInfor);

signals:
    void readySendMessage(unsigned char* p,int length);
    void hasGetInforSlot(vector<pServoInfo> posInfor);
    
private:
    Ui::AnalogData *ui;

    //trends group
    QLabel* label;
    QSlider* slider;
    QSpinBox* spinBox;
    vector<QLabel*> labelVector;
    vector<QSlider*> sliderVector;
    vector<QSpinBox*> spinBoxVector;

    int analogNum;
    vector<int> analogId;

    bool isInit;
};

#endif // ANALOGDATA_H
