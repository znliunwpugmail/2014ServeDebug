#include "receivedata.h"

#include <QTimer>
#include <QByteArray>

ReceiveData::ReceiveData(QObject *parent) :
    QObject(parent)
{
    state = WAIT;
    pData = new DataOut;
    timeOut = 3000;
    overTime = NULL;
    headAlreadyReadBytes = 0;
    headTargetReadBYtes = 4;
    currentHeader.resize(headTargetReadBYtes);
    insAlreadyReadBytes = 0;
    checkBytes = 1;

    groupSignals();
}

ReceiveData::~ReceiveData()
{
    currentHeader.clear();
    currentIntData.clear();
    completeIns.clear();
    delete pData;
    pData = NULL;
}

//header length ServoType Type
//ins insType insData check
void ReceiveData::setParseData(QByteArray data)
{
    for(int i=0;i<(int)data.size();i++)
    {
        unsigned char byte = static_cast<unsigned char>(data.at(i));

        switch(state)
        {
        case WAIT:
            if(byte == 0xFF)
            {
                headAlreadyReadBytes = 0;
                state = READHEAD;

                currentHeader.clear();;
                currentIntData.clear();
                completeIns.clear();
            }
            continue;
        case READHEAD:
            if(overTime==NULL)
            {
                overTime = new QTimer;
            }
            else
            {
                delete overTime;
                emit readTimeout();
                overTime = new QTimer;
            }

            overTime->start(timeOut);
            connect(overTime,SIGNAL(timeout()),this,SLOT(clearInformation()),Qt::UniqueConnection);
            connect(overTime,SIGNAL(timeout()),this,SLOT(clearBufInformetion()),Qt::UniqueConnection);

            currentHeader[headAlreadyReadBytes] = byte;
            headAlreadyReadBytes++;

            if(headAlreadyReadBytes==headTargetReadBYtes)
            {
                headAlreadyReadBytes = 0;
                int length = 0;
                unsigned char ins[2];
                ins[0] = (unsigned char)currentHeader.at(0);
                ins[1] = (unsigned char)currentHeader.at(1);
                memcpy(&length,ins,2);

                if(length>0)
                {
                    insTargetReadBytes = length - 4;
                    insAlreadyReadBytes = 0;
                    state = READDATA;
                }
                else
                {
                    state = WAIT;
                }
            }
            continue;
        case READDATA:
            currentIntData[insAlreadyReadBytes++] = byte;
            if(currentIntData.size()==insTargetReadBytes)
            {
                insAlreadyReadBytes = 0;
                insTargetReadBytes = 0;

                completeIns.clear();
                unsigned char temp = 0xFF;
                completeIns.append(temp);
                completeIns.append(currentHeader);
                completeIns.append(currentIntData);

                pData->clear();
                pData->header.append(currentHeader);
                pData->insData.append(currentIntData);
                pData->allIns.append(completeIns);

                currentHeader.clear();
                currentIntData.clear();
                completeIns.clear();

                state = WAIT;
                emit readFinished();
                if(overTime!=0)
                {
                    emit readTimeout();
                    overTime->stop();
                    delete overTime;
                    overTime = NULL;
                }
            }
            break;
        }
    }
}

void ReceiveData::clearInformation()
{
    state = WAIT;
    emit readTimeout();
}

void ReceiveData::groupSignals()
{
    //connect(this,SIGNAL(readFinished()),this,SLOT());
}

void ReceiveData::clearBufInformetion()
{
    currentHeader.clear();
    currentIntData.clear();
    completeIns.clear();
}

QByteArray ReceiveData::getheaderData()
{
    return pData->header;
}

QByteArray ReceiveData::getInsData()
{
    return pData->insData;
}

QByteArray ReceiveData::getcompleteInsData()
{
    return pData->allIns;
}
