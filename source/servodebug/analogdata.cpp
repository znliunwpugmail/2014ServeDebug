#include "analogdata.h"
#include "ui_analogdata.h"

#include "source/globaldefine/upDefine.h"
#include "source/insanalysis/communicateDefine.h"

#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QString>

#include <QDebug>

AnalogData::AnalogData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalogData)
{
    ui->setupUi(this);
    analogNum = 0;
    isInit = false;
    setMinimumSize(QSize(200,300));
    setWindowTitle(QObject::tr("AnalogDebug"));
}

AnalogData::~AnalogData()
{
    delete ui;
}

void AnalogData::initScene(vector<int> Id)
{
    if(isInit)
    {
        return;
    }
    isInit = true;
    //col is 10 by default
    int num = (int)Id.size();
    analogNum = num;
    analogId.clear();
    analogId = Id;
    int col = 0;
    int line = 0;
    int remainder = 0;
    QGridLayout* gridLayout = new QGridLayout(this);
    for(int i=0;i<num;i++)
    {
        label = new QLabel(this);
        label->setText(QObject::tr("%1").arg(Id.at(i)));
        labelVector.push_back(label);
        slider = new QSlider(Qt::Vertical);
        sliderVector.push_back(slider);
        spinBox = new QSpinBox(this);
        spinBoxVector.push_back(spinBox);

        line = i/10;
        col = 4*(i/10);
        remainder = i%10;
        gridLayout->addWidget(label,col,remainder);
        gridLayout->addWidget(spinBox,col+1,remainder);
        gridLayout->addWidget(slider,col+2,remainder);

        slider->setRange(0,Range);
        spinBox->setRange(0,Range);

        connect(slider,SIGNAL(sliderReleased()),this,SLOT(setSendData()),Qt::UniqueConnection);
        connect(slider,SIGNAL(valueChanged(int)),spinBox,SLOT(setValue(int)),Qt::UniqueConnection);
        connect(spinBox,SIGNAL(valueChanged(int)),slider,SLOT(setValue(int)),Qt::UniqueConnection);
    }
    setLayout(gridLayout);
    show();
}

void AnalogData::clearSceneTools()
{

}

void AnalogData::groupWarning(QString str)
{
    QMessageBox::warning(this,tr("Wrong"),str);
}

void AnalogData::moveSliderToTargetPosition(vector<ServoInfo> data)
{
    if(analogNum==0)
    {
        groupWarning(QObject::tr("analogNum is zero!"));
        return;
    }

    QSlider* sliderTemp = NULL;
    for(int i=0;i<(int)data.size();i++)
    {
        sliderTemp = sliderVector.at(i);
        sliderTemp->setValue(data.at(i).currentPos);
    }
}

void AnalogData::setSliderToTargetPosition(vector<pServoInfo> posInfor)
{
    pServoInfo pStr = NULL;
    QSlider* sliderTemp = NULL;
    for(int i=0;i<(int)posInfor.size();i++)
    {
        pStr = posInfor.at(i);
        sliderTemp = sliderVector.at(i);
        sliderTemp->setValue(pStr->currentPos);
    }
}

vector<pServoInfo> AnalogData::readPosfromSlider()
{
    vector<pServoInfo> infor;
    pServoInfo pStr = NULL;
    QSpinBox* spinBoxTemp = NULL;
    for(int i=0;i<analogNum;i++)
    {
        pStr = new ServoInfo;
        spinBoxTemp = spinBoxVector.at(i);
        pStr->servoId = analogId.at(i);
        pStr->currentPos = spinBoxTemp->text().toInt();
        pStr->currentSpeed = 10;
        infor.push_back(pStr);
    }
    emit hasGetInforSlot(infor);
    return infor;
}

void AnalogData::setSendData()
{
    vector<pServoInfo> id_analog;
    id_analog = readPosfromSlider();
    unsigned char enableInstruction[100] = {'\0'};
    enableInstruction[0] = 0xff;
    enableInstruction[3] = AnalogServoType;
    enableInstruction[4] = RequestPacket;
    enableInstruction[5] = MoveServoPosition;
    for(int i=0;i<(int)id_analog.size();i++)
    {
        memcpy(&enableInstruction[6+i*3],&id_analog[i]->servoId,1);
        memcpy(&enableInstruction[7+i*3],&id_analog[i]->currentPos,2);
    }
    int length=(int)id_analog.size()*3+6;
    memcpy(&enableInstruction[1],&length,2);
    emit readySendMessage(enableInstruction,length-1);
}

void AnalogData::getInforFromSliderSlot()
{
    readPosfromSlider();
}

void AnalogData::gotoSlot(vector<ServoInfo> data)
{
    moveSliderToTargetPosition(data);
}

void AnalogData::closeEvent(QCloseEvent *event)
{
    showMinimized();
    event->ignore();
}
