#include "networkdialog.h"
#include "ui_networkdialog.h"

#include "source/communicate/receivedata/receivedata.h"

#include <QTcpSocket>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QString>
#include <QByteArray>

#include <QDebug>

NetWorkDialog::NetWorkDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWorkDialog)
{
    ui->setupUi(this);
    zhSocket = NULL;
    isconnect = false;
    zhReceiveData = NULL;

    initScene();
    controlTools();
    ipLineEdit->setText("192.168.0.100");
    portLineEdit->setText("8189");
    bool ok = false;
    connectButton->setDefault(&ok);
    setWindowTitle(QObject::tr("NetWork"));
}

NetWorkDialog::~NetWorkDialog()
{
    delete ui;
    delete zhReceiveData;
    zhReceiveData = NULL;
}

void NetWorkDialog::initScene()
{
    setWindowTitle(QObject::tr("Socket"));
    ipLabel = new QLabel(this);
    ipLineEdit = new QLineEdit(this);
    ipLabel->setText(QObject::tr("IpAdress"));
    QHBoxLayout* ipHLayout = new QHBoxLayout;
    ipHLayout->addWidget(ipLabel);
    ipHLayout->addWidget(ipLineEdit);

    portLabel = new QLabel(this);
    portLineEdit = new QLineEdit(this);
    portLabel->setText(QObject::tr("Port"));
    QHBoxLayout* portHLayout = new QHBoxLayout;
    portHLayout->addWidget(portLabel);
    portHLayout->addWidget(portLineEdit);

    connectButton = new QPushButton(this);
    disconnectButton = new QPushButton(this);
    connectButton->setText(QObject::tr("Connect"));
    disconnectButton->setText(QObject::tr("DisConnect"));
    QHBoxLayout* buttonHLayout = new QHBoxLayout;
    buttonHLayout->addWidget(connectButton);
    buttonHLayout->addWidget(disconnectButton);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addLayout(ipHLayout);
    vLayout->addLayout(portHLayout);
    vLayout->addLayout(buttonHLayout);

    setLayout(vLayout);
    groupSignals();
}

void NetWorkDialog::groupSignals()
{
    connect(connectButton,SIGNAL(clicked()),this,SLOT(connectButtonSlot()));
    connect(disconnectButton,SIGNAL(clicked()),this,SLOT(disconnectButtonSlot()));
    connect(this,SIGNAL(hasReadMsg()),this,SLOT(handlereadMsg()));
}

void NetWorkDialog::connectButtonSlot()
{
    IpString = ipLineEdit->text();
    portString = portLineEdit->text();
    bool ok = false;
    quint16 port = portString.toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("The Port is not correct!"));
        return;
    }
    if(!IpString.isEmpty()&&!portString.isEmpty())
    {
        if(NULL==zhSocket)
        {
            zhSocket = new QTcpSocket;
            connect(zhSocket,SIGNAL(disconnected()),this,SLOT(hasdisconnect()));
            connect(zhSocket,SIGNAL(connected()),this,SLOT(hasconnect()));
            connect(this,SIGNAL(isconnectChanged()),this,SLOT(controlTools()));
            connect(zhSocket,SIGNAL(readyRead()),this,SLOT(readMessage()));
        }
        zhSocket->connectToHost(IpString,port);
        if(!zhSocket->isOpen())
        {
            QMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("cannot connect!"));
        }
    }
}

void NetWorkDialog::disconnectButtonSlot()
{
    zhSocket->disconnectFromHost();
}

void NetWorkDialog::controlTools()
{
    connectButton->setDisabled(isconnect);
    disconnectButton->setDisabled(!isconnect);
    ipLineEdit->setDisabled(isconnect);
    portLineEdit->setDisabled(isconnect);
}

void NetWorkDialog::hasconnect()
{
    isconnect = true;
    emit getConnected();
    emit isconnectChanged();
}

void NetWorkDialog::hasdisconnect()
{
    isconnect = false;
    emit isconnectChanged();
}

void NetWorkDialog::sendMessage(QByteArray msg)
{
    zhSocket->write(msg);
    zhSocket->flush();
    emit hasSendIns(msg);
}

void NetWorkDialog::readMessage()
{
    readMsg.clear();
    readMsg = zhSocket->readAll();
    if(!readMsg.isEmpty())
    {
        emit hasReadMsg();
    }
}

bool NetWorkDialog::isConnected()
{
    if(zhSocket!=NULL)
    {
        return zhSocket->isOpen();
    }
    return false;
}

void NetWorkDialog::handlereadMsg()
{
    if(NULL==zhReceiveData)
    {
        zhReceiveData = new ReceiveData;
        connect(zhReceiveData,SIGNAL(readFinished()),this,SLOT(getInsFromReceiveData()));
    }
    zhReceiveData->setParseData(readMsg);
}

void NetWorkDialog::getInsFromReceiveData()
{
    if(NULL==zhReceiveData)
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("ReceiveData is NULL!"));
        return;
    }
    completeInsData.clear();
    headerData.clear();
    insData.clear();
    completeInsData = zhReceiveData->getcompleteInsData();
    headerData = zhReceiveData->getheaderData();
    insData = zhReceiveData->getInsData();
    emit hasReplyIns(completeInsData);
}

void NetWorkDialog::closeEvent(QCloseEvent *event)
{
    this->showMinimized();
    event->ignore();
}
