#include "commandanalysis.h"

#include "mainwindow.h"
#include "source/communicate/network/networkdialog.h"
#include "source/communicate/serialcom/serialdialog.h"
#include "source/insanalysis/communicateDefine.h"
#include "source/globaldefine/upDefine.h"

#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QDebug>

CommandAnalysis::CommandAnalysis(QObject *parent) :
    QObject(parent)
{
    zhMainWindow = NULL;
    zhSerialDialog = NULL;
    zhNetWorkDialog = NULL;
    servoNum = 0;
    servoType = 0;          //digital by default
    lastServoType = -1;      // record the last servotype
    //connect(this,SIGNAL(servoTypeChanged(unsigned char)),this,SLOT(setServoType(unsigned char)));
}

CommandAnalysis::~CommandAnalysis()
{

}

void CommandAnalysis::getMainWindowPointer(MainWindow* p)
{
    zhMainWindow = p;
}

void CommandAnalysis::getNetWorkPointer(NetWorkDialog* p)
{
    zhNetWorkDialog = p;

    completeInsData.clear();
    insData.clear();
    headerData.clear();
    completeInsData = zhNetWorkDialog->completeInsData;
    insData = zhNetWorkDialog->insData;
    headerData = zhNetWorkDialog->headerData;
}

void CommandAnalysis::getSerialPointer(SerialDialog* p)
{
    zhSerialDialog = p;

    completeInsData.clear();
    insData.clear();
    headerData.clear();
    completeInsData = zhSerialDialog->completeInsData;
    insData = zhSerialDialog->insData;
    headerData = zhSerialDialog->headerData;
    parseData.clear();
}

//legnth low-first
void CommandAnalysis::setParseData(QByteArray data)
{
    bool currect = checkParityFromDown(data);
    if(!currect)
    {
        groupWarning(QObject::tr("checkParity is wrong!"));
        return;
    }
    servoType = (unsigned char)data.at(3);
    if(servoType!=lastServoType)
    {
        lastServoType = servoType;
        emit servoTypeChanged(servoType);
    }
    int length = 0;
    length = (unsigned char)data.at(1)+(unsigned char)data.at(2)*256;
    int insDataLength = length-6;
    int pos = 0;

    switch((unsigned char)data.at(5))
    {
    case OK:
        emit replyInsStateSignal(QObject::tr("Ok"));
        break;

    case ResServoPosInfo:
    {
        //only for digital servo
        clearinfor();
        if(insDataLength%3!=0)
        {
            groupWarning(QObject::tr("The InsDataLength is Wrong!"));
            return;
        }
        if(servoNum!=insDataLength/3)
        {
            groupWarning("ServoNum is Wrong!");
            return;
        }
        pServoInfo inforTemp = new ServoInfo[insDataLength/3];
        for(int i=0;i<(int)insDataLength/3;i++)
        {
            (inforTemp+i)->servoId = (int)((unsigned char)data.at(6+i*3));
            pos = (unsigned char)data.at(7+i*3)+(unsigned char)data.at(8+i*3)*256;
            (inforTemp+i)->currentPos = pos;
            infor.push_back((inforTemp+i));
        }
        emit inforChangedSignal(infor);
        emit replyInsStateSignal(QObject::tr("ResServoPosInfo"));
        //   delete [] inforTemp;
        break;
    }

    case ConnectServo:
        emit createServoDebugScene();
        if(servoType == DigitalServoType)
        {
            digitalId.clear();
            for(int i=0;i<insDataLength;i++)
            {
                digitalId.push_back((int)((unsigned char)data.at(6+i)));
            }
            servoNum = (int)digitalId.size();
            emit digitalIdChanged(digitalId);
        }
        else if(servoType == AnalogServoType)
        {
            if(insDataLength%3!=0)
            {
                groupWarning(QObject::tr("The InsDataLength is Wrong!"));
                return;
            }
            clearinfor();
            analogId.clear();
            pServoInfo inforTemp = new ServoInfo[insDataLength/3];
            for(int i=0;i<insDataLength/3;i++)
            {
                analogId.push_back((int)(unsigned char)data.at(6+i*3));
                (inforTemp+i)->servoId = (int)(unsigned char)data.at(6+i*3);
                pos = (unsigned char)data.at(7+i*3)+(unsigned char)data.at(8+i*3)*256;
                (inforTemp+i)->currentPos = pos;
                infor.push_back((inforTemp+i));
            }
            servoNum = (int)analogId.size();
            emit analogIdChanged(analogId);
            emit analogInforChangedSignal(infor);
            //delete [] inforTemp;
        }
        else if(servoType==MixtureServoType)
        {
            digitalId.clear();
            analogId.clear();
            clearinfor();
            for(int i=0;i<(int)(unsigned char)data.at(6);i++)
            {
                digitalId.push_back((int)(unsigned char)data.at(7+i));
            }
            insDataLength -= (1+(int)(unsigned char)data.at(6));

            emit digitalIdChanged(digitalId);

            if(insDataLength%3!=0)
            {
                groupWarning(QObject::tr("The InsDataLength is WOrng!"));
                return;
            }
            pServoInfo inforTemp = new ServoInfo[insDataLength/3];
            for(int i=0;i<insDataLength/3;i++)
            {
                analogId.push_back((int)((unsigned char)data.at(7+(int)data.at(6)+i*3)));
                (inforTemp+i)->servoId = (int)(unsigned char)data.at(7+(int)data.at(6)+i*3);
                pos = (unsigned char)data.at(7+i*3)+(unsigned char)data.at(8+i*3)*256;
                (inforTemp+i)->currentPos = pos;
                infor.push_back((inforTemp+i));
            }
            emit analogIdChanged(analogId);
            emit analogInforChangedSignal(infor);
            //delete [] inforTemp;
        }
        emit replyInsStateSignal(QObject::tr("ConnectServo"));
        ///////ServoDebug
        break;

    case CheckError:
        emit replyInsStateSignal(QObject::tr("CheckError"));
        groupWarning(QObject::tr("Check Error!"));
        break;

    case NoSuchType:
        emit replyInsStateSignal(QObject::tr("NoSuchType"));
        groupWarning(QObject::tr("parse instruction is NoSuchType error!"));
        break;

    case NoSuchServo:
        emit replyInsStateSignal(QObject::tr("NoSuchServo"));
        groupWarning(QObject::tr("parse instruction is NoSuchServo error!"));
        break;

    case NoSuchInsType:
        emit replyInsStateSignal(QObject::tr("NoSuchInsType"));
        groupWarning(QObject::tr("parse instruction is NoSuchInstructionType error!"));
        break;

    case NoSuchFile:
        emit replyInsStateSignal(QObject::tr("NoSuchFile"));
        groupWarning(QObject::tr("parse instruction is NoSuchFile error!"));
        break;

    case ResSensorData:
        emit replyInsStateSignal(QObject::tr("ResSensorData"));
        //////////////waiting
        break;

    case UpLoadActionsNameType:
    {
        if(insDataLength>=FileNum-1)
        {
            groupWarning(QObject::tr("FileNum is too short!"));
            return;
        }
        fileList.clear();
        QString str;
        for(int i=6;i<(int)data.size()-1;i++)
        {
            str.push_back(data.at(i));
        }
        fileList = str.split(" ");
        emit fileListChangedSignal(fileList);
        emit replyInsStateSignal(QObject::tr("UpLoadActionsNameType"));
        break;
    }

    case Error:
        emit replyInsStateSignal(QObject::tr("Error"));
        groupWarning(QObject::tr("parse instruction is Error_NoSuchFile error!"));
        break;

    default:
        emit replyInsStateSignal(QObject::tr("Without reason"));
        groupWarning(QObject::tr("Without reason!"));
        break;
    }
    return;
}

unsigned char CommandAnalysis::protocalCheckDataFromDown(QByteArray data)
{
    unsigned char check = 0;
    for(int i=1;i<(int)data.size()-1;i++)
    {
        check += (unsigned char)data.at(i);
    }
    return ~check ;
}

unsigned char CommandAnalysis::protocalCheckDataForIns(unsigned char* instruction,int length)
{
    unsigned char check=0;
    for(int i=1;i<length;i++)
        check += instruction[i];
    return ~(check);
}

bool CommandAnalysis::checkParityFromDown(QByteArray data)
{
    return ((unsigned char)data.at((int)data.size()-1)==protocalCheckDataFromDown(data));
}

void CommandAnalysis::groupWarning(QString str)
{
    QMessageBox::warning(zhMainWindow,tr("Wrong!"),str);
    return;
}

void CommandAnalysis::clearinfor()
{
    if(infor.empty())
    {
        return;
    }
    for(int i=0;i<(int)infor.size();i++)
    {
        if(infor.at(i)!=NULL)
        {
            //delete infor.at(i);
            //infor.at(i) = NULL;
        }
    }
    infor.clear();
}

void CommandAnalysis::sendInsData(unsigned char* ins,int length)
{
    if(length <= 0)
    {
        groupWarning(QObject::tr("Ins is below zero!"));
        return;
    }

    QByteArray sendByte;
    for(int i=0;i<length;i++)
    {
        sendByte.append(ins[i]);
    }

    if(zhSerialDialog != NULL)
    {
        bool test = zhSerialDialog->isConnected();
        if(test == true)
        {
            zhSerialDialog->sendMessage(sendByte);
            return ;
        }
        groupWarning(QObject::tr("The SerialCom is not open!"));
    }

    if(zhNetWorkDialog != NULL)
    {
        bool test = zhNetWorkDialog->isConnected();
        if(test == true)
        {
            zhNetWorkDialog->sendMessage(sendByte);
            return ;
        }
        groupWarning(QObject::tr("The Network is not work!"));
    }
    groupWarning(QObject::tr("Disconnected!"));
}

void CommandAnalysis::getDownServoCondition()
{
    unsigned char getID[100]={'\0'};
    int length=0;
    length = 6;
    getID[0]=0xff;
    memcpy(&getID[1],&length,2);
    getID[3] = (unsigned char)servoType;
    getID[4]=RequestPacket;
    getID[5]=GetServoConnection;
    getID[6] = protocalCheckDataForIns(getID,length);
    sendInsData(getID,length+1);
}

void CommandAnalysis::enableAllServo()
{
    unsigned char getID[8]={'\0'};
    int length=0;
    length = 7;
    getID[0]=0xff;
    memcpy(&getID[1],&length,2);
    getID[3] = (unsigned char)servoType;
    getID[4]=RequestPacket;
    getID[5]=EnableServo;
    getID[6]=0xfe;
    getID[7] = protocalCheckDataForIns(getID,length);
    sendInsData(getID,length+1);
}

void CommandAnalysis::enableSomeServo(vector<int> Id)
{
    if(Id.empty())
    {
        groupWarning(QObject::tr("servoId is empty!"));
        return;
    }
    unsigned char enableInstruction[100]={'\0'};
    enableInstruction[0]=0xff;
    enableInstruction[3] = (unsigned char)servoType;
    enableInstruction[4]=RequestPacket;
    enableInstruction[5]=EnableServo;
    int length=0;
    length = 6+(int)Id.size();
    memcpy(&enableInstruction[1],&length,2);
    for(int i=0;i<(int)Id.size();i++)
    {
        memcpy(&enableInstruction[6+i],&Id[i],1);
    }
    enableInstruction[length] = protocalCheckDataForIns(enableInstruction,length);
    sendInsData(enableInstruction,length+1);
}

void CommandAnalysis::disenableAllServo()
{
    unsigned char getID[8]={'\0'};
    int length=0;
    length = 7;
    getID[0]=0xff;
    memcpy(&getID[1],&length,2);
    getID[3] = (unsigned char)servoType;
    getID[4]=RequestPacket;
    getID[5]=DisableServo;
    getID[6]=0xfe;
    getID[7] = protocalCheckDataForIns(getID,length);
    sendInsData(getID,length+1);
}

void CommandAnalysis::disenableSomeServo(vector<int> Id)
{
    if(Id.empty())
    {
        groupWarning(QObject::tr("servoId is empty!"));
        return;
    }
    unsigned char enableInstruction[100]={'\0'};
    enableInstruction[0]=0xff;
    enableInstruction[3] = (unsigned char)servoType;
    enableInstruction[4]=RequestPacket;
    enableInstruction[5]=DisableServo;
    int length=0;
    length = 6+(int)Id.size();
    memcpy(&enableInstruction[1],&length,2);
    for(int i=0;i<(int)Id.size();i++)
    {
        memcpy(&enableInstruction[6+i],&Id[i],1);
    }
    enableInstruction[length] = protocalCheckDataForIns(enableInstruction,length);
    sendInsData(enableInstruction,length+1);
}

void CommandAnalysis::analogInformationSlot(unsigned char* p,int length)
{
    p[length+1] = protocalCheckDataForIns(p,length+1);
    sendInsData(p,length+2);
}

void CommandAnalysis::saveSequenceSlot(vector<int> Id)
{
    unsigned char* ins = new unsigned char[1024];
    ins[0] = 0xff;
    int length = 0;
    ins[3] = (unsigned char)servoType;
    ins[4] = RequestPacket;
    ins[5] = DownSequence;

    for(int i=0;i<(int)Id.size();i++)
    {
        memcpy(&ins[6+i],&Id[i],1);
    }

    length = 6+Id.size();
    memcpy(&ins[1],&length,2);
    ins[length] = protocalCheckDataForIns(ins,length);
    sendInsData(ins,length+1);
    delete [] ins;
}

void CommandAnalysis::execSequenceSlot()
{
    unsigned char* ins = new unsigned char[100];
    ins[0] = 0xff;
    int length = 6;
    ins[3] = (unsigned char)servoType;
    ins[4] = RequestPacket;
    ins[5] = ExectueSequence;
    memcpy(&ins[1],&length,2);
    ins[6] = protocalCheckDataForIns(ins,length);
    sendInsData(ins,length+1);
    delete [] ins ;
}

void CommandAnalysis::deleteMiniActionSlot(QString str)
{
    unsigned char* ins = new unsigned char[1024*10];
    ins[0]= 0xff;
    ins[3] = (unsigned char)servoType;
    ins[4]= RequestPacket;
    ins[5]= DeleteMiniAction;

    int total = 0;
    memcpy (&ins[6+total],str.toAscii().data(),str.size());
    total += str.size ();

    int length = 0;
    length += (6+total);
    memcpy (&ins[1],&length,2);
    ins[length] = protocalCheckDataForIns(ins,length);
    sendInsData(ins,length+1);
    delete [] ins;
}

void CommandAnalysis::execMiniActionSlot(QString str)
{
    unsigned char* ins = new unsigned char[1024];
    ins[0]=0xff;
    int length = 6;
    ins[3] = (unsigned char)servoType;
    ins[4]=RequestPacket;
    ins[5]=ExcuteMiniAction;
    memcpy(&ins[6],str.toAscii().data(),str.size ());
    length = 6+str.size();
    memcpy (&ins[1],&length,2);
    ins[length] = protocalCheckDataForIns(ins,length);
    sendInsData(ins,length+1);
    delete [] ins;
}

void CommandAnalysis::upLoadMiniActionSlot()
{
    unsigned char* ins = new unsigned char[128];
    ins[0] = 0xff;
    int length = 6;
    memcpy(&ins[1],&length,2);
    ins[3] = (unsigned char)servoType;
    ins[4] = RequestPacket;
    ins[5] = UploadFileName;
    ins[6] = protocalCheckDataForIns(ins,length);
    sendInsData(ins,length+1);
    delete [] ins;
}

void CommandAnalysis::downLoadMiniActionSlot(QString fileName)
{
    /*fileName is absulate path*/
    unsigned char* instruction = new unsigned char[1024*50];

    instruction[0]=0xff;
    instruction[3] = (unsigned char)servoType;
    instruction[4]=RequestPacket;
    instruction[5]=DownloadMiniAction;

    QString str;
    str = fileName.mid (fileName.lastIndexOf("/")+1);
    int fileNameLength = str.length();
    memcpy (&instruction[6],&fileNameLength,1);
    memcpy (&instruction[7],str.toAscii().data(),fileNameLength);

    QFile readFile(fileName.toAscii().data());
    if(!readFile.open(QIODevice::ReadOnly))
    {
        groupWarning(QObject::tr("file fail to open!"));
        return ;
    }
    int num = 0;
    int length = 0;
    if(readFile.isOpen())
    {
        QByteArray test =  readFile.readAll();
        num = test.size();
        for(int i=0;i<(int)test.size();i++)
        {
            instruction[7+fileNameLength+i] = static_cast<unsigned char>(test[i]);
        }
        length = 7+fileNameLength+num;
        memcpy (&instruction[1],&length,2);
        instruction[length] = protocalCheckDataForIns(instruction,length);
        sendInsData(instruction,length+1);
        readFile.close();
        delete [] instruction;
    }
    else
    {
        groupWarning(QObject::tr("Can't Find the instruction File,Please Check"));
        delete []instruction;
    }
}

void CommandAnalysis::setServoIdSlot(vector<int> Id)
{
    if(servoType==DigitalServoType)
    {
        if(digitalId==Id)
        {
            return;
        }
        digitalId.clear();
        digitalId = Id;
        emit digitalIdChanged(Id);
    }
    else if(servoType==AnalogServoType)
    {
        if(analogId==Id)
        {
            return;
        }
        analogId.clear();
        analogId = Id;
        emit analogIdChanged(Id);
    }
    else if(servoType==MixtureServoType)
    {
        //wait
    }
    else
    {
        groupWarning(QObject::tr("servoType is wrong!"));
    }
}

void CommandAnalysis::setServoType(unsigned char type)
{
    servoType = type;
    emit servoTypeChanged(type);
}

void CommandAnalysis::getSomeServoPosSlot(vector<unsigned char> Id)
{
    unsigned char getPos[100]={'\0'};
    int length=6+(int)Id.size();
    getPos[0]=0xff;
    memcpy(&getPos[1],&length,2);
    getPos[3] = (unsigned char)servoType;
    getPos[4]=RequestPacket;
    getPos[5]=GetServoPosition;

    for(int i=0;i<(int)Id.size();i++)
    {
        memcpy(&getPos[6+i],&Id[i],1);
    }

    int currentPos=6+(int)Id.size();
    getPos[currentPos] = protocalCheckDataForIns(getPos,length);
    sendInsData(getPos,length+1);
}
