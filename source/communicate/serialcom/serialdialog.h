#ifndef SERIALDIALOG_H
#define SERIALDIALOG_H

#include <QWidget>
#include <QCloseEvent>

#include "source/communicate/serialcom/qextserialbase.h"
#include "source/communicate/serialcom/qextserialport.h"
#include "source/communicate/serialcom/win_qextserialport.h"

class QComboBox;
class QLabel;
class QPushButton;
class QString;
class QByteArray;

class ReceiveData;

namespace Ui {
class SerialDialog;
}

class SerialDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit SerialDialog(QWidget *parent = 0);
    ~SerialDialog();

    void sendMessage(QByteArray msg);
    bool isConnected();

private:
    void initScene();
    void groupSignals();

    void setzhPortSettings();

    void closeEvent(QCloseEvent *event);

signals:
    void zhComIsOpen();
    void zhComIsClose();

    void comIsOpenChanged();
    void hasReadMsg();

    void hasSendIns(QByteArray ins);
    void hasReplyIns(QByteArray ins);

private slots:
    void openCom();
    void closeCom();

    void hasOpen();
    void hasClose();

    //enable tools
    void controlTools();

    void readMessage();
    void handleMessage();

    void getInsFromReceiveData(); //get complete ins data

    //out Data
public:
    QByteArray completeInsData;
    QByteArray insData;
    QByteArray headerData;
    
private:
    Ui::SerialDialog *ui;

    //scene tools--ui
    QLabel* comLabel;
    QComboBox* comComboBox;
    QLabel* paudLabel;
    QComboBox* paudComboBox;
    QLabel* dataLabel;
    QComboBox* dataComboBox;
    QLabel* stopLabel;
    QComboBox* stopComboBox;
    QLabel* checkLabel;
    QComboBox* checkComboBox;

    QPushButton* openSerialButton;
    QPushButton* closeSerialButton;

    //serialCom settings
    Win_QextSerialPort* zhCom;
    PortSettings zhPortSettings;
    QString portName;

    //control tools state
    bool comIsOpen;

    //read message
    QByteArray readMsg;

    //connect to out
    ReceiveData* zhReceiveData;
};

#endif // SERIALDIALOG_H
