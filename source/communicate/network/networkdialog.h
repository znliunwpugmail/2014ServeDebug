#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include <QWidget>
#include <QCloseEvent>
#include <QtNetWork/QTcpSocket>

class QTcpSocket;
class QLabel;
class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QString;
class QByteArray;

class ReceiveData;

namespace Ui {
class NetWorkDialog;
}

class NetWorkDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit NetWorkDialog(QWidget *parent = 0);
    ~NetWorkDialog();

    void sendMessage(QByteArray msg);
    bool isConnected();
    
private:
    void initScene();
    void groupSignals();

    void closeEvent(QCloseEvent *event);

private slots:
    void connectButtonSlot();
    void disconnectButtonSlot();
    void hasconnect();
    void hasdisconnect();

    //enable tools
    void controlTools();

    void readMessage();
    void handlereadMsg();

    void getInsFromReceiveData(); //get complete Ins data

signals:
    void isconnectChanged();
    void hasReadMsg();

    void getConnected();
    void hasSendIns(QByteArray ins);
    void hasReplyIns(QByteArray ins);

    //out data
public:
    QByteArray completeInsData;
    QByteArray insData;
    QByteArray headerData;

private:
    Ui::NetWorkDialog *ui;
    QTcpSocket* zhSocket;

    //scene tools--ui
    QLabel* ipLabel;
    QLineEdit* ipLineEdit;
    QLabel* portLabel;
    QLineEdit* portLineEdit;
    QPushButton* connectButton;
    QPushButton* disconnectButton;

    //socket information
    QString IpString;
    QString portString;

    //control tools state
    bool isconnect;

    //read message
    QByteArray readMsg;

    //connect out
    ReceiveData* zhReceiveData;
};

#endif // NETWORKDIALOG_H
