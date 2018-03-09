#ifndef LOGDEBUG_H
#define LOGDEBUG_H

#include <QWidget>
#include <QCloseEvent>
#include <vector>

using namespace std;

class QFile;
class QDateTime;
class QLabel;
class QLineEdit;
class QTextEdit;
class QString;
class QByteArray;

namespace Ui {
class LogDebug;
}

class LogDebug : public QWidget
{
    Q_OBJECT
    
public:
    explicit LogDebug(QWidget *parent = 0);
    ~LogDebug();

public slots:
    void setSendIns(QByteArray ins);
    void setSendIns(QString ins);
    void setSendIns(vector<unsigned char> ins);
    void setSendMsg(QByteArray mag);
    void setReplyIns(QString ins);
    void setReplyIns(QByteArray ins);
    void setReplyIns(vector<unsigned char> ins);
    void setReplyMsg(QByteArray msg);
    void setconnectState(QString str);
    void setDigitalNumber(vector<int> Id);
    void setAnalogNumber(vector<int> Id);
    void setReplyInsStateSlot(QString str);

private:
    void closeEvent(QCloseEvent *event);

    void initScene();

private:
    Ui::LogDebug *ui;
    QFile file;
    QDateTime* pTime;

    //scene tools--ui
    QLabel* connectLabel;
    QLineEdit* connectLineEdit;
    QLabel* digitalLabel;
    QLineEdit* digitalLineEdit;
    QLabel* analogLabel;
    QLineEdit* analogLineEdit;
    QLabel* lastSendInsLabel;
    QLineEdit* lastSendInsLineEdit;
    QLabel* lastReplyInsLabel;
    QLineEdit* lastReplyInsLineEdit;
    QLabel* stateLabel;
    QLineEdit* stateLineEdit;

    QTextEdit* recordTextEdit;
};

#endif // LOGDEBUG_H
