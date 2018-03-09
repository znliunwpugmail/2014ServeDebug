#include "actionmanger.h"

#include "source/servodebug/SplineSample/myspline.h"
#include "source/servodebug/Graph/graphscene.h"
#include "source/insanalysis/communicateDefine.h"
#include "source/globaldefine/upDefine.h"

#include <QTableWidget>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <QDebug>

ActionManger::ActionManger(QTableWidget *parent) :
    QObject(parent)
{
    zhScene = NULL;
    servoType = 0x00;
    servoNumber = 0;
    timeSpace = 20;
    intSpace =  1;
    zhTableWidget = parent;

    groupSignals();
}

ActionManger::~ActionManger()
{
    delete [] zhActionChannelInfo;
}

void ActionManger::groupWarning(QString str)
{
    QMessageBox::warning(NULL,QObject::tr("Wrong"),str);
}

quint8 ActionManger::checkSum(int length,quint8 instruction[])
{
    quint8 checksum=0;
    for(int i=2;i<length;i++)
    {
        checksum+=instruction[i];
    }
    return  ~(checksum%255) ;
}

int ActionManger::combineInstruction(InstrFrameInfo InstrInfo,quint8 * Instr)
{
    if(servoType == DigitalServoType)
    {
        Instr[0] = 0xff;
        Instr[1] = 0xff;
        Instr[2] = 0xfe;
        Instr[4] = 0x83;
        Instr[5] = 0x1e;
        Instr[6] = 0x04;
        quint8 length = 0;   //ins length

        for(int i=0; i<InstrInfo.Size; i++)
        {
            memcpy (&Instr[7+i*5],&(InstrInfo.Id[i]),1);
            memcpy (&Instr[8+i*5],&(InstrInfo.Position[i]),2);
            memcpy (&Instr[10+i*5],&(InstrInfo.Speed[i]),2);
        }
        length = (quint8)(4+InstrInfo.Size*5);
        memcpy (&Instr[3],&length,1);
        Instr[7+InstrInfo.Size*5] = checkSum (7+InstrInfo.Size*5,Instr);

        return 7+InstrInfo.Size*5+1;
    }
    else if(servoType==AnalogServoType)
    {
        Instr[0] = 0xfe;
        Instr[1] = 0xfe;
        for(int i=0; i<(int)InstrInfo.Size; i++)
        {
            memcpy (&Instr[2+i*3],&(InstrInfo.Id[i]),1);
            memcpy (&Instr[3+i*3],&(InstrInfo.Position[i]),2);
        }
        return 2 + InstrInfo.Size*3;
    }
    else if(servoType==MixtureServoType)
    {
        Instr[0] = 0xff;
        Instr[1] = 0xff;
        Instr[2] = 0xfe;
        Instr[4] = 0x83;
        Instr[5] = 0x1e;
        Instr[6] = 0x04;
        quint8 length = 0;/*每帧指令中的长度信息*/

        for(int i=0; i<InstrInfo.Size; i++)
        {
            memcpy (&Instr[7+i*5],&(InstrInfo.Id[i]),1);
            memcpy (&Instr[8+i*5],&(InstrInfo.Position[i]),2);
            memcpy (&Instr[10+i*5],&(InstrInfo.Speed[i]),2);
        }
        length = (quint8)(4+InstrInfo.Size*5);
        memcpy(&Instr[3],&length,1);
        Instr[7+InstrInfo.Size*5] = checkSum (7+InstrInfo.Size*5,Instr);

        int length1 = 7+InstrInfo.Size*5;
        Instr[length1+1] = 0xfe;
        Instr[length1+2] = 0xfe;
        for(int i=0; i<InstrInfo.Size; i++)
        {
            memcpy (&Instr[length1+3+i*3],&(InstrInfo.Id[i]),1);
            memcpy (&Instr[length1+4+i*3],&(InstrInfo.Position[i]),2);
        }
        return 7+InstrInfo.Size*5+1;
    }
    else
    {
        groupWarning(QObject::tr("Servo_select_kind is Wrong!"));
        return 0;
    }
}

void ActionManger::setServoIdVector()
{
    servoIdVector.clear();
    switch(servoType)
    {
    case DigitalServoType:
        servoIdVector = digitalId;
        break;

    case AnalogServoType:
        servoIdVector = analogId;
        break;

    case MixtureServoType:
        for(int i=0;i<(int)digitalId.size();i++)
        {
            servoIdVector.push_back(digitalId.at(i));
        }
        for(int i=0;i<(int)analogId.size();i++)
        {
            servoIdVector.push_back(analogId.at(i));
        }
        break;

    default:
        groupWarning(QObject::tr("without reason wrong!"));
        break;
    }
    emit createSecneSignal();
}

void ActionManger::setTimeSpaceSlot(int time)
{
    timeSpace = time;
    intSpace =  time/BaseSpace;
}

void ActionManger::getDigitalIdSlot(vector<int> Id)
{
    digitalId.clear();
    digitalId = Id;
    servoNumber = (int)Id.size();
    initAcitonChannelInfo(Id);
    emit servoIdChanged();
}

void ActionManger::getAnalogIdSlot(vector<int> Id)
{
    analogId.clear();
    analogId = Id;
    servoNumber = (int)Id.size();
    initAcitonChannelInfo(Id);
    emit servoIdChanged();
}

void ActionManger::getServoTypeSlot(unsigned char type)
{
    servoType = type;
    if(servoType==AnalogServoType)
    {
        emit analogDebugStart();
    }
    else if(servoType==MixtureServoType)
    {
        emit mixtureDebugStart();
    }
}

void ActionManger::refreshSlot()
{
    refreshKeyInfor();
    refreshInterpInfor();

    for(int i=0; i<(int)servoIdVector.size(); i++)
    {
        // not using id is better
        //int id = servoIdVector.at(i);
        //ActionManage strat from zero.

        if(zhScene==NULL)
        {
            return;
        }

        zhScene->setKeyFrameData(zhActionChannelInfo[i].id,&(zhActionChannelInfo[i].keyPoint));   //keyFrame
        if(zhActionChannelInfo[i].keyPoint.size()>=3)
        {
            zhScene->setInterpFrameData(zhActionChannelInfo[i].id,&(zhActionChannelInfo[i].InterPoint));//interp data
        }
        else
        {
            PointList tempPoint;
            tempPoint.clear();
            zhScene->setInterpFrameData(zhActionChannelInfo[i].id,&tempPoint);
        }
    }
}

void ActionManger::refreshKeyInfor()
{
    int row = zhTableWidget->rowCount();
    qDebug()<<tr("row=")<<row;

    clearAcitonChannelInfo(servoNumber);

    Point tempPoint;
    for(int i=0; i<row; i++)
    {
        tempPoint.x = zhTableWidget->item(i,servoIdVector.size()-1)->text().toInt();
        for(int j=0; j<(int)servoIdVector.size()-1; j++)
        {
            qDebug()<<tr("servoIdVector.size()=")<<servoIdVector.size();
            //int id = servoIdVector.at(j);

            tempPoint.y = zhTableWidget->item(i,j)->text().toInt();
            //tempPoint.y = zhTableWidget->item(i,j)->text().toInt();
            zhActionChannelInfo[j].keyPoint.push_back(tempPoint);
        }
    }
}

void ActionManger::refreshInterpInfor()
{
    for(int i=0; i<(int)servoIdVector.size(); i++)
    {
        spline.Clear();
        int size = zhActionChannelInfo[i].keyPoint.size();
        for(int j=0; j<size; j++)
        {
            spline.AddSamplePoint(zhActionChannelInfo[i].keyPoint[j].x,
                    zhActionChannelInfo[i].keyPoint[j].y);
            spline.Calculation();
        }

        if(size>0)
        {
            size = (int)(zhActionChannelInfo[i].keyPoint[size-1].x/BaseSpace);
        }
        double y,s,last_y=512;
        for(int j=0; j<size; j++)
        {
            spline.GetCal_S(j*BaseSpace,y);
            Point pointTemp;
            pointTemp.x = j*BaseSpace;
            pointTemp.y = y;
            zhActionChannelInfo[i].InterPoint.push_back(pointTemp);
            s = SPEEDPARAM*(fabs(y-last_y)/((double)timeSpace));
            if(s>1023)
            {
                s=1023;
            }
            pointTemp.y = s;
            zhActionChannelInfo[i].speedPoint.push_back(pointTemp);
            last_y = y;
        }
    }
}

void ActionManger::saveFileSlot(QString str1,QString str2)
{
    saveKeyPoint(str1);
    saveMiniAction(str2);
}

void ActionManger::saveKeyPoint(QString str)
{
    QFile saveFile(str);
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        groupWarning(QObject::tr("file fail to open!"));
        return ;
    }

    QTextStream csvStream(&saveFile);
    csvStream.setIntegerBase(10);

    int frameNum = zhActionChannelInfo[0].keyPoint.size();
    int servoNum = servoIdVector.size();

    if(!frameNum||!servoNum)
    {
        groupWarning(QObject::tr("servo num or frame num is zero!"));
        return ;
    }

    csvStream<<servoType<<"\r\n";
    for(int i=0;i<servoNum;i++)
    {
        QString temp;
        temp = temp.setNum(zhActionChannelInfo[i].id,10);
        temp.append(QObject::tr("-Servo"));
        if(i == 0)
        {
            csvStream << temp;
        }
        else
        {
            csvStream << "," << temp;
        }
    }
    csvStream << "," << QObject::tr("Time") << "\r\n";
    for(int i=0; i<frameNum; i++)
    {
        for(int j=0; j<servoNum; j++)
        {
            if(j == 0)
            {
                csvStream << zhActionChannelInfo[j].keyPoint[i].y;
            }
            else
            {
                csvStream << "," << zhActionChannelInfo[j].keyPoint[i].y;
            }
        }
        csvStream<< "," << zhActionChannelInfo[0].keyPoint[i].x <<"\r\n";
    }

    saveFile.flush ();
    saveFile.close ();
}

void ActionManger::saveMiniAction(QString str)
{
    QFile saveFile(str);
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        groupWarning(QObject::tr("file fail to open!"));
        return;
    }

    QByteArray bWriteData;
    bWriteData.clear ();

    quint8  id[ServoNum];
    quint16 position[ServoNum];
    quint16 speed[ServoNum];

    InstrFrameInfo InstrInfo;
    InstrInfo.Id = id;
    InstrInfo.Position = position;
    InstrInfo.Speed = speed;

    int servoNum = servoIdVector.size();
    int frameNum = zhActionChannelInfo[0].InterPoint.size();

    if(!frameNum)
    {
        groupWarning(QObject::tr("frame num is zero!"));
        return ;
    }

    if(!servoNum)
    {
        groupWarning(QObject::tr("servo num is zero!"));
        return ;
    }
    if(servoType == DigitalServoType)
    {
        bWriteData.clear();
        quint16 InstrSize = 7+servoNum*5+1;
        bWriteData.append(timeSpace);
        bWriteData.append(((quint8*)(&InstrSize))[0]);
        bWriteData.append(((quint8*)(&InstrSize))[1]);
        saveFile.write(bWriteData);
        bWriteData.clear();

        InstrInfo.Size = servoNum;
        int length = 0;
        quint8 Instr[InstrSize_Max];
        for(int i=0; i<frameNum; i+=intSpace)
        {
            for(int j=0; j<servoNum; j++)
            {
                InstrInfo.Id[j] = (quint8)(zhActionChannelInfo[j].id);
                InstrInfo.Position[j] = (quint16)(zhActionChannelInfo[j].InterPoint[i].y);
                InstrInfo.Speed[j] = (quint16)(zhActionChannelInfo[j].speedPoint[i].y);
            }
            length = combineInstruction(InstrInfo,Instr);
            bWriteData.clear();
            for(int j=0; j<length; j++)
            {
                bWriteData.append(Instr[j]);
            }
            saveFile.write(bWriteData);
        }
    }
    else if(servoType==AnalogServoType)
    {
        bWriteData.clear();
        quint16 InstrSize = 2+servoNum*3;
        bWriteData.append(timeSpace);
        bWriteData.append(((quint8 *)(&InstrSize))[0]);
        bWriteData.append(((quint8 *)(&InstrSize))[1]);
        saveFile.write(bWriteData);
        bWriteData.clear();

        InstrInfo.Size = servoNum;   //number of analogservo
        int length = 0;
        quint8 Instr[InstrSize_Max];
        for(int i=0; i<frameNum; i+=intSpace)
        {
            for(int j=0; j<servoNum; j++)
            {
                InstrInfo.Id[j] = (quint8)(zhActionChannelInfo[j].id);
                InstrInfo.Position[j] = (quint16)(zhActionChannelInfo[j].InterPoint[i].y);
                InstrInfo.Speed[j] = (quint16)(zhActionChannelInfo[j].speedPoint[i].y);
            }
            length = combineInstruction(InstrInfo,Instr);
            bWriteData.clear();
            for(int j=0; j<length; j++)
            {
                bWriteData.append(Instr[j]);
            }
            saveFile.write(bWriteData);
        }
    }
    else if(servoType==MixtureServoType)
    {
        bWriteData.clear();
        quint16 InstrSize = 2+servoNum*3+7+servoNum*5+1;
        bWriteData.append(timeSpace);
        bWriteData.append(((quint8 *)(&InstrSize))[0]);
        bWriteData.append(((quint8 *)(&InstrSize))[1]);
        saveFile.write(bWriteData);
        bWriteData.clear();
        quint8 analogNum = (quint8)digitalId.size();
        bWriteData.append(analogNum);
        saveFile.write(bWriteData);
        saveFile.write("\n");
        bWriteData.clear();

        InstrInfo.Size = servoNum;
        int length = 0;
        quint8 Instr[InstrSize_Max];
        for(int i=0; i<frameNum; i+=intSpace)
        {
            for(int j=0; j<servoNum; j++)
            {
                InstrInfo.Id[j] = (quint8)(zhActionChannelInfo[servoIdVector.at(j)-1].id);
                InstrInfo.Position[j] = (quint16)(zhActionChannelInfo[servoIdVector.at(j)-1].InterPoint[i].y);
                InstrInfo.Speed[j] = (quint16)(zhActionChannelInfo[servoIdVector.at(j)-1].speedPoint[i].y);
            }
            length = combineInstruction(InstrInfo,Instr);
            bWriteData.clear();
            for(int j=0; j<length; j++)
            {
                bWriteData.append(Instr[j]);
            }
            saveFile.write(bWriteData);
        }
    }
    else
    {
        groupWarning(QObject::tr("no reason wrong!"));
        return ;
    }
}

void ActionManger::initAcitonChannelInfo(vector<int> Id)
{
    int num = (int)Id.size();
    zhActionChannelInfo = new AcitonChannelInfo[num];
    for(int i=0;i<num;i++)
    {
        zhActionChannelInfo[i].id = Id.at(i);
    }
}

void ActionManger::clearAcitonChannelInfo(int num)
{
    for(int i=0;i<num;i++)
    {
        zhActionChannelInfo[i].clear();
    }
}

void ActionManger::groupSignals()
{
    connect(this,SIGNAL(servoIdChanged()),this,SLOT(setServoIdVector()),Qt::UniqueConnection);
    connect(this,SIGNAL(createSecneSignal()),this,SLOT(refreshSlot()),Qt::UniqueConnection);
}

void ActionManger::getzhScenePointer(GraphScene* p)
{
    zhScene = p;
}

void ActionManger::getzhTableWidgetPointer(QTableWidget* p)
{
    zhTableWidget = p;
}
