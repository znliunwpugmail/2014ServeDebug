#include "keyframewidget.h"
#include "ui_keyframewidget.h"

#include "source/insanalysis/communicateDefine.h"
#include "source/globaldefine/upDefine.h"

#include <QTableWidget>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFile>

#include <QDebug>

KeyFrameWidget::KeyFrameWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyFrameWidget)
{
    ui->setupUi(this);

    isInit = false;
    servoType = 0;
    lastServoType = -1;
    layout = NULL;
    keyPosTableWIdget =  NULL;
    initScene();
    setWindowTitle(QObject::tr("KeyFrame"));
}

KeyFrameWidget::~KeyFrameWidget()
{
    delete ui;
}

QTableWidget* KeyFrameWidget::setTableWidgetPointer()
{
    return keyPosTableWIdget;
}

void KeyFrameWidget::initScene()
{
    keyPosTableWIdget = new QTableWidget(this);
    layout = new QHBoxLayout(this);
    layout->addWidget(keyPosTableWIdget);
    setLayout(layout);
    //emit createKeyPosTableWidget(keyPosTableWIdget);
}

void KeyFrameWidget::groupSignals()
{

}

void KeyFrameWidget::groupWarning(QString str)
{
    QMessageBox::warning(this,QObject::tr("Wrong"),str);
}

void KeyFrameWidget::openKeyPointSlot(QString str)
{
    QFile openFile(str);
    if(!openFile.open(QIODevice::ReadOnly))
    {
        groupWarning(QObject::tr("Fail to the file!"));
        return;
    }

    for(int i=keyPosTableWIdget->rowCount()-1;i>=0;i--)
    {
        keyPosTableWIdget->removeRow(i);
    }

    fileData.clear();
    fileData.append(openFile.readAll());
    openFile.close();

    initServoInformation();

    emit openKeyPointSignal();
}

void KeyFrameWidget::initServoInformation()
{
    if(!fileData.isEmpty())
    {
        //relate to servoType in a line
        QStringList Instrlist = fileData.split("\r\n");
        QString tempStr = Instrlist.at(0);
        /*
         *read first column to ininit servoType ,ID_digital ,ID_analog,Pos_servo
         *file first coloumn is : 0(1(2))
         *data block
         */
        QStringList tempList = tempStr.split(" ");
        QString strData;
        strData.append(tempList.at(0));
        bool ok = false;
        servoType = (unsigned char)strData.toInt(&ok);
        qDebug()<<tr("servotype=")<<servoType;
        if(!ok)
        {
            groupWarning(QObject::tr("file data is wrong!"));
            return;
        }
        if(servoType!=lastServoType)
        {
            lastServoType = servoType;
            emit servoTypeChanged(servoType);
        }
        switch(strData.toInt())
        {
        qDebug()<<tr("strData=")<<strData;
        case DigitalServoType:
            if(tempList.size()>1)
            {
                groupWarning(QObject::tr("fileData is Wrong!"));
                return;
            }
            servoType = (unsigned char)DigitalServoType;
            buildServoInformation();
            break;
        case AnalogServoType:
            if(tempList.size()>1)
            {
                groupWarning(QObject::tr("fileData is Wrong!"));
                return ;
            }
            servoType = (unsigned char)AnalogServoType;
            buildServoInformation();
            break;
        case MixtureServoType:
        {
            if(tempList.size()<2)
            {
                groupWarning(QObject::tr("fileData is Wrong!"));
                return ;
            }
            servoType = (unsigned char)MixtureServoType;
            strData.clear();
            strData.append(tempList.at(1));
            digitalId.clear();
            int number = strData.toInt();
            strData.clear();
            for(int i=0;i<number;i++)
            {
                strData.append(tempList.at(2+i));
                digitalId.push_back(strData.toInt());
            }
            strData.clear();
            strData.append(tempList.at(2+number));
            analogId.clear();
            int length = number;
            number = strData.toInt();
            strData.clear();
            for(int i=0;i<number;i++)
            {
                strData.append(tempList.at(2+length+1+i));
                analogId.push_back(strData.toInt());
            }
            buildServoInformation();
            break;
        }
        default:
            groupWarning(QObject::tr("Wrong with the servotype!"));
            break;
        }
    }
    else
    {
        groupWarning(QObject::tr("File is not Open!"));
    }
}


void KeyFrameWidget::buildServoInformation()
{
    servoIdVector.clear();
    if(!fileData.isEmpty())
    {
        QStringList instrlist = fileData.split("\r\n");
        if(instrlist.size()==1)
        {
            groupWarning(QObject::tr("The Data form is Wrong!"));
            return;
        }
        QStringList baseInfoList;
        baseInfoList = instrlist[1].split(",");
        for(int i=0;i<(int)baseInfoList.size()-1;i++)
        {
            QString temp = baseInfoList.at(i);
            temp = temp.remove(QObject::tr("-Servo"));
            int temp_id = temp.toInt();
            servoIdVector.push_back(temp_id);
        }
        emit servoIdVectorChanged(servoIdVector);

        initTableWidget();
        for(int i=0; i<(int)instrlist.size(); i++)
        {
            baseInfoList = instrlist[i].split(",");
            int size = baseInfoList.size();
            if(size==1)
            {
                continue;
            }
            keyPosTableWIdget->insertRow(i-2);
            for(int j=0; j<size-1; j++)
            {
                keyPosTableWIdget->setItem(i-2,j,new QTableWidgetItem(baseInfoList.at(j)));
            }
            keyPosTableWIdget->setItem(i-2,0,new QTableWidgetItem(baseInfoList.at(size-1)));
        }
        return;
    }
    else
    {
        groupWarning(QObject::tr("The fileData is ampty!"));
        return;
    }
}

void KeyFrameWidget::KeyFrame_InsertKeyFrame(int TimePos)
{
    SetNextWorkInfo(InsertType,TimePos);
    if(servoType == DigitalServoType)
    {
        vector<unsigned char> ins;
        ins.push_back(0xfe);
        emit getSomeServoPosSignal(ins);
    }
    else if(servoType == AnalogServoType)
    {
        emit getInforFromSliderSignal();
        ServoDataToBuffer(analogPos);
        Work();
    }
    else if(servoType==MixtureServoType)
    {
        emit getInforFromSliderSignal();
        ServoDataToBuffer(analogPos);
        Work();
        vector<unsigned char> ins;
        ins.push_back(0xfe);
        emit getSomeServoPosSignal(ins);
    }
    else
    {
        groupWarning(QObject::tr("servoType is Wrong!"));
    }
}

void KeyFrameWidget::KeyFrame_Update_Load(int TimePos)
{
    SetNextWorkInfo(UpdateLoadType,TimePos);
    Delete();
    KeyFrame_InsertKeyFrame(TimePos);
}

void KeyFrameWidget::KeyFrame_Update_Move(int OldTimePos,int NewTimePos)
{
    SetNextWorkInfo(UpdateMoveType,OldTimePos,NewTimePos);
    Work();
}

void KeyFrameWidget::KeyFrame_Update_Modify(int ServoId, int TimePos, int newData)
{
    SetUpdateInfo(ServoId,TimePos,newData);
    Work();
}

void KeyFrameWidget::KeyFrame_DeleteraKeyFrame(int TimePos)
{
    SetNextWorkInfo(DeleteType,TimePos);
    Work();
}

void KeyFrameWidget::KeyFrame_Goto(int TimePos)
{
    int index = this->FindTimePos(TimePos);
    int column = keyPosTableWIdget->columnCount();
    vector<ServoInfo> ServoDataList;
    ServoInfo tempServoData;
    tempServoData.currentPos = BaseSpeed;

    for(int i=0; i<column-1; i++)
    {
        tempServoData.servoId = i+1;
        tempServoData.currentPos = keyPosTableWIdget->item(index,i+1)->text().toInt();
        ServoDataList.push_back(tempServoData);
    }

    //int servoType, int &length, int *ID, int *pos, int *speed
    if(servoType==AnalogServoType||servoType==MixtureServoType)
    {
        emit gotoSignal(ServoDataList);
        emit getInforFromSliderSignal();
        unsigned char enableInstruction[100] = {'\0'};
        enableInstruction[0] = 0xff;
        enableInstruction[3] = servoType;
        enableInstruction[4] = RequestPacket;
        enableInstruction[5] = MoveServoPosition;
        for(int i=0;i<(int)servoIdVector.size();i++)
        {
            memcpy(&enableInstruction[6+i*3],&(ServoDataList[i].servoId),1);
            memcpy(&enableInstruction[7+i*3],&(ServoDataList[i].currentPos),2);
        }
        int length=(int)ServoDataList.size()*3+6;
        memcpy(&enableInstruction[1],&length,2);
        emit readySendMessage(enableInstruction,length-1);
    }
    else if(servoType==DigitalServoType)
    {
        unsigned char enableInstruction[100] = {'\0'};
        enableInstruction[0] = 0xff;
        enableInstruction[3] = servoType;
        enableInstruction[4] = RequestPacket;
        enableInstruction[5] = MoveServoPosition;
        for(int i=0;i<(int)servoIdVector.size();i++)
        {
            memcpy(&enableInstruction[6+i*3],&(ServoDataList[i].servoId),1);
            memcpy(&enableInstruction[7+i*3],&(ServoDataList[i].currentPos),2);
        }
        int length=(int)ServoDataList.size()*3+6;
        memcpy(&enableInstruction[1],&length,2);
        emit readySendMessage(enableInstruction,length-1);
        //drop check -1
    }
    return;
}

bool KeyFrameWidget::SetNextWorkInfo(int Type,int TimePos ,int NewTimePos)
{
    NextWorkType = Type;
    if(WorkTimePos>=0)
    {
        WorkTimePos = SelectTimePos = TimePos; //may be selected
    }
    else
    {
        WorkTimePos = TimePos;
    }
    UpdateTimePos = NewTimePos;
    return true;
}

void KeyFrameWidget::getInforFromSliderSlot(vector<pServoInfo> data)
{
    analogPos.clear();
    analogPos = data;
}

void KeyFrameWidget::ServoDataToBuffer(vector<pServoInfo>& ServoData)
{
    ServoDataBuffer.clear();
    for(int i=0;i<(int)servoIdVector.size();i++)
    {
        ServoDataBuffer.push_back(*ServoData.at(i));
    }
    ServoData.clear();
}

bool KeyFrameWidget::Work()
{
    bool result = true;
    switch(NextWorkType)
    {
    case NotWork:
        return true;
    case InsertType:
        result = Insert();
        break;
    case DeleteType:
        result = Delete();
        break;
    case UpdateMoveType:
        result = Update_Move();
        break;
    case UpdateLoadType:
        result = Update_Load();
        break;
    case SelectType:
        result = Select();
        break;
    }

    if(NextWorkType!=SelectType)
    {
        emit setUpdateInfoSignal();
    }
    NextWorkType = NotWork;
    return result;
}

bool KeyFrameWidget::Insert()
{
    if(WorkTimePos<0)
    {
        int oldTimePos;
        for(int i=0; i<keyPosTableWIdget->rowCount(); i++)
        {
            oldTimePos = keyPosTableWIdget->item(i,0)->text().toInt();
            keyPosTableWIdget->item(i,0)->setText(QString("%1").arg(oldTimePos-WorkTimePos));
        }
        WorkTimePos=0;
    }

    int index = FindTimePos(WorkTimePos);
    keyPosTableWIdget->insertRow(index);
    keyPosTableWIdget->setItem(index,0,new QTableWidgetItem(QString("%1").arg(WorkTimePos)));

    for(int i=1; i<=(int)ServoDataBuffer.size(); i++)
    {
        keyPosTableWIdget->setItem(index,i,
                                   new QTableWidgetItem(QString("%1").arg(ServoDataBuffer.at(i-1).currentPos)));
    }
    ServoDataBuffer.clear();
    return true;
}

bool KeyFrameWidget::Update_Move()
{
    int index = FindTimePos(WorkTimePos);
    keyPosTableWIdget->item(index,0)->setText(QString("%1").arg(UpdateTimePos));
    SortTableWidget();
    return true;
}

bool KeyFrameWidget::Update_Load()
{
    Delete();
    Insert();
    return true;
}

bool KeyFrameWidget::Delete()
{
    int index = FindTimePos(WorkTimePos);
    keyPosTableWIdget->removeRow(index);
    return true;
}

bool KeyFrameWidget::Select()
{
    return true;
}

int KeyFrameWidget::FindTimePos(int TimePos)
{
    int CurrentTimePos,i;
    for(i=0; i<(int)keyPosTableWIdget->rowCount(); i++)
    {
        CurrentTimePos = keyPosTableWIdget->item(i,0)->text().toInt();
        if(TimePos>CurrentTimePos)
        {
            continue;
        }
        break;
    }
    return i;
}

void KeyFrameWidget::ClearWorkInfo()
{
    NextWorkType = 0;
    WorkTimePos = 0;
    UpdateTimePos = 0;
}

void KeyFrameWidget::SortTableWidget()
{
    int row = keyPosTableWIdget->rowCount();
    int size = (int)servoIdVector.size();
    QList<QList<QString > > ItemList;
    QList<QString > tempList;
    for(int i=0; i<row; i++)
    {
        tempList.clear();
        tempList.append(keyPosTableWIdget->item(i,0)->text());
        for(int j=0; j<size; j++)
        {
            tempList.append(keyPosTableWIdget->item(i,j)->text());
        }
        ItemList.append(tempList);
    }

    for(int i=row-1; i>=0; i--)
    {
        keyPosTableWIdget->removeRow(i);
    }

    for(int i=0; i<row; i++)
    {
        for(int j=i+1; j<row; j++)
        {
            if(ItemList[i].at(0).toInt()>ItemList[j].at(0).toInt())
            {
                ItemList.swap(i,j);
            }
        }
    }

    for(int i=0; i<row; i++)
    {
        keyPosTableWIdget->insertRow(i);
        keyPosTableWIdget->setItem(i,0,new QTableWidgetItem(ItemList[i].at(0)));
        for(int j=0; j<size; j++)
        {
            keyPosTableWIdget->setItem(i,servoIdVector.at(j),
                                       new QTableWidgetItem(ItemList[i].at(j+1)));
        }
    }
}

void KeyFrameWidget::SetUpdateInfo(int ServoId,int TimePos,int NewPosition)
{
    NextWorkType = NotWork;
    int index = FindTimePos(TimePos);
    keyPosTableWIdget->setItem(index,ServoId,new QTableWidgetItem(QString("%1").arg(NewPosition)));
    emit setUpdateInfoSignal();
}

void KeyFrameWidget::initTableWidget()
{
//    if(isInit)
//    {
//        return;
//    }
//    isInit = true;

    keyPosTableWIdget->setColumnCount(servoIdVector.size()+1);
    QStringList header;
    header<<"Time";
    for(int i=0;i<(int)servoIdVector.size();i++)
    {
        QString str;
        str = str.setNum(servoIdVector.at(i),10);
        header <<str;
    }
    keyPosTableWIdget->setHorizontalHeaderLabels(header);
    keyPosTableWIdget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    keyPosTableWIdget->setSelectionBehavior (QAbstractItemView::SelectItems);
}

void KeyFrameWidget::setServoIdVectorSlot(vector<int> Id)
{
    if(servoIdVector==Id)
    {
        return;
    }
    servoIdVector.clear();
    servoIdVector = Id;

    //the signal relate to CommandAnalysis
    //change each other if the servoId is changed
    emit servoIdVectorChanged(Id);
    initTableWidget();
}

void KeyFrameWidget::setDigitalPosSlot(vector<pServoInfo> data)
{
    ServoDataToBuffer(data);
    Work();
}

void KeyFrameWidget::setServoTypeSlot(unsigned char type)
{
    servoType = type;
}

void KeyFrameWidget::closeEvent(QCloseEvent *event)
{
    showMinimized();
    event->ignore();
}
