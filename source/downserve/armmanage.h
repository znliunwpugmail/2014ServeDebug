#ifndef ARMMANAGE_H
#define ARMMANAGE_H

#include <QWidget>
#include <vector>
#include <QCloseEvent>

using namespace std;

class QString;
class QStringList;
class QModelIndex;

namespace Ui {
class ArmManage;
}

class ArmManage : public QWidget
{
    Q_OBJECT

public:
    explicit ArmManage(QWidget *parent = 0);
    ~ArmManage();

    void refreshFileList();
    void closeEvent(QCloseEvent *event);

public slots:
    void getUpLoadFileNameSlot(QStringList data);

private:
    void groupWarning(QString str);
    bool produceFileName(QString& filename);

signals:
    void openFileSignal(QString str);
    void deleteMiniActionSignal(QString str);
    void saveFileSignal(QString str1,QString str2);
    void coverDataSignal();
    void downloadMiniActionSignal(QString str);
    void uploadMiniActionSignal();
    void playMiniActionSignal(QString str);
    void executeSequenceSignal();
    void downSequenceSignal(vector<int> Id);
    void timeSpaceChanged(int time);

private slots:
    void on_pushButton_EditMiniActionList_clicked();
    void on_pushButton_RenameActionFile_clicked();
    void on_pushButton_Cover_clicked();
    void on_pushButton_DeleteMiniAction_clicked();
    void on_pushButton_DownSelectMiniAction_clicked();
    void on_pushButton_RefreshList_clicked();
    void on_pushButton_PlaySpeed_clicked();
    void on_pushButton_UpLoadMiniAction_clicked();
    void on_pushButton_PlayCurrentActionFrame_clicked();
    void on_pushButton_DeleteLoadAction_clicked();
    void on_pushButton_ExecuteSequence_clicked();
    void on_pushButton_DownSequence_clicked();
    void on_tableWidget_DigitalActionList_doubleClicked(const QModelIndex &index);

private:
    Ui::ArmManage *ui;

    //file information
    QString currentPath;
    int miniActionMaxId;

    //upload file name
    QStringList fileName;
    QString currentEditFile;

};

#endif // ARMMANAGE_H
