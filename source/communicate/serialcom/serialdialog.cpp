#include "serialdialog.h"
#include "ui_serialdialog.h"

#include "source/communicate/receivedata/receivedata.h"

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QGridLayout>
#include <QDebug>

SerialDialog::SerialDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialDialog)
{
    ui->setupUi(this);
    zhCom = NULL;
    comIsOpen = false;
    zhReceiveData = NULL;

    initScene();
    controlTools();
    setWindowTitle(QObject::tr("Sericom"));
}

SerialDialog::~SerialDialog()
{
    delete ui;
    delete zhReceiveData;
    zhReceiveData = NULL;
}

void SerialDialog::initScene()
{
    comLabel = new QLabel(this);
    comLabel->setText(QObject::tr("Com"));
    comComboBox = new QComboBox(this);
    QStringList str;
    str<<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"
      <<"COM7"<<"COM8"<<"COM9"<<"COM10"<<"COM11"<<"COM12";
    comComboBox->addItems(str);

    paudLabel = new QLabel(this);
    paudLabel->setText(QObject::tr("Paud"));
    paudComboBox = new QComboBox(this);
    str.clear();
    str<<"9600"<<"19200"<<"38400"<<"57600"<<"115200";
    paudComboBox->addItems(str);

    dataLabel = new QLabel(this);
    dataLabel->setText(QObject::tr("Data"));
    dataComboBox = new QComboBox(this);
    str.clear();
    str<<"8"<<"7";
    dataComboBox->addItems(str);

    stopLabel = new QLabel(this);
    stopLabel->setText(QObject::tr("Stop"));
    stopComboBox = new QComboBox(this);
    str.clear();
    str<<"1"<<"2";
    stopComboBox->addItems(str);

    checkLabel = new QLabel(this);
    checkLabel->setText(QObject::tr("Check"));
    checkComboBox = new QComboBox(this);
    str.clear();
    str<<"NULL"<<"EVEN"<<"ODD";
    checkComboBox->addItems(str);

    openSerialButton = new QPushButton(this);
    openSerialButton->setText(QObject::tr("OpenSerialCom"));
    closeSerialButton = new QPushButton(this);
    closeSerialButton->setText(QObject::tr("CloseSerialCom"));

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(comLabel,0,0);
    gridLayout->addWidget(comComboBox,0,1);
    gridLayout->addWidget(paudLabel,1,0);
    gridLayout->addWidget(paudComboBox,1,1);
    gridLayout->addWidget(dataLabel,2,0);
    gridLayout->addWidget(dataComboBox,2,1);
    gridLayout->addWidget(stopLabel,3,0);
    gridLayout->addWidget(stopComboBox,3,1);
    gridLayout->addWidget(checkLabel,4,0);
    gridLayout->addWidget(checkComboBox,4,1);
    gridLayout->addWidget(openSerialButton,5,0);
    gridLayout->addWidget(closeSerialButton,5,1);

    setLayout(gridLayout);

    groupSignals();
}

void SerialDialog::groupSignals()
{
    connect(openSerialButton,SIGNAL(clicked()),this,SLOT(openCom()),Qt::UniqueConnection);
    connect(closeSerialButton,SIGNAL(clicked()),this,SLOT(closeCom()),Qt::UniqueConnection);
    connect(this,SIGNAL(zhComIsOpen()),this,SLOT(hasOpen()),Qt::UniqueConnection);
    connect(this,SIGNAL(zhComIsClose()),this,SLOT(hasClose()),Qt::UniqueConnection);
    connect(this,SIGNAL(comIsOpenChanged()),this,SLOT(controlTools()),Qt::UniqueConnection);
    connect(this,SIGNAL(hasReadMsg()),this,SLOT(handleMessage()),Qt::UniqueConnection);
}

bool SerialDialog::isConnected()
{
    if(zhCom!=NULL)
    {
        return zhCom->isOpen();
    }
    return false;
}

void SerialDialog::sendMessage(QByteArray msg)
{
    zhCom->write(msg);
    zhCom->flush();
    emit hasSendIns(msg);
}

void SerialDialog::openCom()
{
    setzhPortSettings();
    if(NULL==zhCom)
    {
        zhCom = new Win_QextSerialPort(portName,QextSerialBase::EventDriven);
    }
    zhCom->open(QIODevice::ReadWrite);
    zhCom->setBaudRate(zhPortSettings.BaudRate);
    zhCom->setDataBits(zhPortSettings.DataBits);
    zhCom->setParity(zhPortSettings.Parity);
    zhCom->setStopBits(zhPortSettings.StopBits);
    zhCom->setFlowControl(zhPortSettings.FlowControl);
    zhCom->setTimeout(zhPortSettings.Timeout_Millisec);
    if(zhCom->isOpen())
    {
        connect(zhCom,SIGNAL(readyRead()),this,SLOT(readMessage()),Qt::UniqueConnection);
        emit zhComIsOpen();
    }
    else
    {
        delete zhCom;
        zhCom = NULL;
        QMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("cannot open!"));
    }
}

void SerialDialog::closeCom()
{
    zhCom->close();
    if(!zhCom->isOpen())
    {
        delete zhCom;
        zhCom = NULL;
        emit zhComIsClose();
    }
}

void SerialDialog::setzhPortSettings()
{
    portName = comComboBox->currentText();

    if(paudComboBox->currentText()==QObject::tr("9600"))
    {
        zhPortSettings.BaudRate = BAUD9600;
    }
    else if(paudComboBox->currentText()==QObject::tr("19200"))
    {
        zhPortSettings.BaudRate = BAUD19200;
    }
    else if(paudComboBox->currentText()==QObject::tr("38400"))
    {
        zhPortSettings.BaudRate = BAUD38400;
    }
    else if(paudComboBox->currentText()==QObject::tr("57600"))
    {
        zhPortSettings.BaudRate = BAUD57600;
    }
    else if(paudComboBox->currentText()==QObject::tr("115200"))
    {
        zhPortSettings.BaudRate = BAUD115200;
    }
    else
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("There is a fault!"));
        return;
    }

    if(dataComboBox->currentText()==QObject::tr("8"))
    {
        zhPortSettings.DataBits = DATA_8;
    }
    else if(dataComboBox->currentText()==QObject::tr("7"))
    {
        zhPortSettings.DataBits = DATA_7;
    }
    else
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("There is a fault!"));
        return;
    }

    if(stopComboBox->currentText()==QObject::tr("1"))
    {
        zhPortSettings.StopBits = STOP_1;
    }
    else if(stopComboBox->currentText()==QObject::tr("2"))
    {
        zhPortSettings.StopBits = STOP_2;
    }
    else
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("There is a fault!"));
        return;
    }

    if(checkComboBox->currentText()==QObject::tr("NULL"))
    {
        zhPortSettings.Parity = PAR_NONE;
    }
    else if(checkComboBox->currentText()==QObject::tr("EVEN"))
    {
        zhPortSettings.Parity = PAR_EVEN;
    }
    else if(checkComboBox->currentText()==QObject::tr("ODD"))
    {
        zhPortSettings.Parity = PAR_ODD;
    }
    else
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("There is a fault!"));
        return;
    }

    zhPortSettings.FlowControl = FLOW_OFF;
    zhPortSettings.Timeout_Millisec = 10;
}

void SerialDialog::hasOpen()
{
    comIsOpen = true;
    emit comIsOpenChanged();
}

void SerialDialog::hasClose()
{
    comIsOpen = false;
    emit comIsOpenChanged();
}

void SerialDialog::controlTools()
{
    comComboBox->setDisabled(comIsOpen);
    paudComboBox->setDisabled(comIsOpen);
    dataComboBox->setDisabled(comIsOpen);
    stopComboBox->setDisabled(comIsOpen);
    checkComboBox->setDisabled(comIsOpen);
    openSerialButton->setDisabled(comIsOpen);
    closeSerialButton->setDisabled(!comIsOpen);
}

void SerialDialog::readMessage()
{
    readMsg.clear();
    readMsg = zhCom->readAll();
    if(!readMsg.isEmpty())
    {
        emit hasReadMsg();
    }
}

void SerialDialog::handleMessage()
{
    if(NULL==zhReceiveData)
    {
        zhReceiveData = new ReceiveData;
        connect(zhReceiveData,SIGNAL(readFinished()),this,SLOT(getInsFromReceiveData()),Qt::UniqueConnection);
    }
    zhReceiveData->setParseData(readMsg);
}

void SerialDialog::getInsFromReceiveData()
{
    if(NULL==zhReceiveData)
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),tr("ReceiveData is NULL!"));
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

void SerialDialog::closeEvent(QCloseEvent *event)
{
    this->showMinimized();
    event->ignore();
}
