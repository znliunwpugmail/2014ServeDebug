#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include <QObject>

class QByteArray;
class QTimer;

class ReceiveData : public QObject
{
    Q_OBJECT
public:
    explicit ReceiveData(QObject *parent = 0);
    ~ReceiveData();
    
    void setParseData(QByteArray data); //prepare the data to parse

    //link with out
    QByteArray getheaderData();
    QByteArray getInsData();
    QByteArray getcompleteInsData();

signals:
    void readTimeout();
    void readFinished();

private:
    void groupSignals();
    
private slots:
    void clearInformation();
    void clearBufInformetion();

private:
    enum STATE
    {
        WAIT,
        READHEAD,
        READDATA
    }state;

    struct DataOut
    {
        QByteArray header;
        QByteArray insData;
        QByteArray allIns;
        void clear()
        {
            header.clear();
            insData.clear();
            allIns.clear();
        }
    } *pData;

    int timeOut;    // over time
    QByteArray currentHeader; //reading header
    int headAlreadyReadBytes; //already read bytes of header
    int headTargetReadBYtes;  //target read bytes of header
    QByteArray currentIntData; //reading ins
    int insAlreadyReadBytes; //already read bytes of ins
    int insTargetReadBytes; //target read bytes of ins
    QByteArray completeIns; //complete ins
    int checkBytes;  // the byte of check

    QTimer* overTime;
};

#endif // RECEIVEDATA_H
