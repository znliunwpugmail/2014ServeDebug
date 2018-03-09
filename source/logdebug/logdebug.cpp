#include "logdebug.h"
#include "ui_logdebug.h"

#include <QFile>
#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QGridLayout>
#include <QString>
#include <QByteArray>
#include <QMessageBox>

LogDebug::LogDebug(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogDebug)
{
    ui->setupUi(this);
    pTime = NULL;

    initScene();

    pTime = new QDateTime;
    file.setFileName("logDebug.txt");
    if(!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this,QObject::tr("Wrong"),QObject::tr("logdebugfile fail to open!"));
    }
    setWindowTitle(QObject::tr("LogRecord"));
}

LogDebug::~LogDebug()
{
    delete ui;
}

void LogDebug::initScene()
{
    connectLabel = new QLabel(this);
    connectLabel->setText(QObject::tr("connect type"));
    connectLineEdit = new QLineEdit(this);

    digitalLabel = new QLabel(this);
    digitalLabel->setText(QObject::tr("digital servo number:"));
    digitalLineEdit = new QLineEdit(this);

    analogLabel = new QLabel(this);
    analogLabel->setText(QObject::tr("analog servo number:"));
    analogLineEdit = new QLineEdit(this);

    lastSendInsLabel = new QLabel(this);
    lastSendInsLabel->setText(QObject::tr("last send ins data:"));
    lastSendInsLineEdit = new QLineEdit(this);

    lastReplyInsLabel = new QLabel(this);
    lastReplyInsLabel->setText(QObject::tr("last reply ins data:"));
    lastReplyInsLineEdit = new QLineEdit(this);

    stateLabel = new QLabel(this);
    stateLabel->setText(QObject::tr("reply state:"));
    stateLineEdit = new QLineEdit(this);

    recordTextEdit = new QTextEdit(this);

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(connectLabel,0,0);
    gridLayout->addWidget(connectLineEdit,0,1);
    gridLayout->addWidget(digitalLabel,1,0);
    gridLayout->addWidget(digitalLineEdit,1,1);
    gridLayout->addWidget(analogLabel,2,0);
    gridLayout->addWidget(analogLineEdit,2,1);
    gridLayout->addWidget(lastSendInsLabel,3,0);
    gridLayout->addWidget(lastSendInsLineEdit,3,1);
    gridLayout->addWidget(lastReplyInsLabel,4,0);
    gridLayout->addWidget(lastReplyInsLineEdit,4,1);
    gridLayout->addWidget(stateLabel,5,0);
    gridLayout->addWidget(stateLineEdit,5,1);
    gridLayout->addWidget(recordTextEdit,6,0,1,2);

    setLayout(gridLayout);
}

void LogDebug::setSendIns(QByteArray ins)
{
    QString msg;
    quint8 it = 0;
    if(ins.size() <= 0)
        return ;
    for(int i=0;i<(int)ins.size();i++)
    {
        it = (quint8)ins.at(i);
        msg += QString::number(it);
        msg += tr(" ");
    }
    if(msg.isEmpty())
        return ;
    lastSendInsLineEdit->setText(msg);
    setSendMsg(ins);
}

void LogDebug::setSendIns(QString ins)
{
    lastSendInsLineEdit->setText(ins);
}

void LogDebug::setSendIns(vector<unsigned char> ins)
{
    QString msg;
    quint8 it = 0;
    if(ins.size() <= 0)
    {
        return ;
    }
    for(int i=0;i<(int)ins.size();i++)
    {
        it = (quint8)ins.at(i);
        msg += QString::number(it);
        msg += tr(" ");
    }
    if(msg.isEmpty())
    {
        return ;
    }
    lastSendInsLineEdit->setText(msg);
}

void LogDebug::setSendMsg(QByteArray msg)
{
    QString test = pTime->currentDateTime().toString(Qt::TextDate);
    test += QObject::tr("SendMessage:\n");
    quint8 it = 0;
    test += tr(" ");
    for(int i=0;i<(int)msg.size();i++)
    {
        it = (quint8)msg.at(i);
        test += QString::number(it);
        test += tr(" ");
    }
    test += tr("\n");
    recordTextEdit->insertPlainText(test);
    file.write(test.toAscii(),test.length());
}

void LogDebug::setReplyIns(QString ins)
{
    lastReplyInsLineEdit->setText(ins);
}

void LogDebug::setReplyIns(QByteArray ins)
{
    QString data;
    quint8 it = 0;
    if(ins.size() <= 0)
    {
        return ;
    }
    for(int i=0;i<(int)ins.size();i++)
    {
        it = (quint8)ins.at(i);
        data += QString::number(it);
        data += tr(" ");
    }
    if(data.isEmpty())
    {
        return ;
    }
    lastReplyInsLineEdit->setText(data);
    setReplyMsg(ins);
}

void LogDebug::setReplyIns(vector<unsigned char> ins)
{
    QString data;
    quint8 it = 0;
    if(ins.size() <= 0)
    {
        return ;
    }
    for(int i=0;i<(int)ins.size();i++)
    {
        it = (quint8)ins.at(i);
        data += QString::number(it);
        data += tr(" ");
    }
    if(data.isEmpty())
    {
        return ;
    }
    lastReplyInsLineEdit->setText(data);
}

void LogDebug::setReplyMsg(QByteArray msg)
{
    QString test = pTime->currentDateTime().toString(Qt::TextDate);
    test += QObject::tr("ReplyMessage:\n");
    quint8 it = 0;
    test += tr(" ");
    for(int i=0;i<(int)msg.size();i++)
    {
        it = (quint8)msg.at(i);
        test += QString::number(it);
        test += tr(" ");
    }
    test += tr("\n");
    recordTextEdit->insertPlainText(test);
    file.write(test.toAscii(),test.length());
}

void LogDebug::setconnectState(QString str)
{
    connectLineEdit->setText(str);
}

void LogDebug::setDigitalNumber(vector<int> Id)
{
    digitalLineEdit->setText(QObject::tr("%1").arg((int)Id.size()));
}

void LogDebug::setAnalogNumber(vector<int> Id)
{
    analogLineEdit->setText(QObject::tr("%1").arg((int)Id.size()));
}

void LogDebug::setReplyInsStateSlot(QString str)
{
    stateLineEdit->setText(str);
}

void LogDebug::closeEvent(QCloseEvent *event)
{
    this->showMinimized();
    event->ignore();
}
