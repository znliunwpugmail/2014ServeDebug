#include "armmanage.h"
#include "ui_armmanage.h"

#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QDir>
#include <QModelIndex>
#include <QFileDialog>
#include <QInputDialog>

#include <QDebug>

ArmManage::ArmManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArmManage)
{
    ui->setupUi(this);

    miniActionMaxId = 0;

    refreshFileList();
    setWindowTitle(QObject::tr("FileManage"));
    setMinimumSize(QSize(380,400));
}

ArmManage::~ArmManage()
{
    delete ui;
}

void ArmManage::refreshFileList()
{
    QDir dir;
    currentPath = dir.absolutePath();

    for(int i=ui->tableWidget_DigitalActionList->rowCount()-1; i>=0; i--)
    {
        ui->tableWidget_DigitalActionList->removeRow(i);
    }

    QStringList Filter;
    Filter.append(QString("*.csv"));
    QStringList FileList = QDir(currentPath+"/KeyPoint").entryList(Filter);
    if(FileList.size()==0)
    {
        groupWarning(QObject::tr("not keypoint file!"));
        return;
    }

    for(int i=0; i<FileList.size(); i++)
    {
        ui->tableWidget_DigitalActionList->insertRow(i);
        ui->tableWidget_DigitalActionList->setItem(i,0,new QTableWidgetItem(FileList.at(i)));
    }
}

void ArmManage::groupWarning(QString str)
{
    QMessageBox::warning(this,QObject::tr("Wrong"),str);
}

bool ArmManage::produceFileName(QString& filename)
{
    if(!filename.isEmpty())
    {
        filename.remove(".csv");
        filename = QString::number(miniActionMaxId+1)+"_"+filename+"_";
        miniActionMaxId++;
        return true;
    }
    return false;
}

void ArmManage::on_pushButton_EditMiniActionList_clicked()
{
    QDir dir;
    currentPath = dir.absolutePath();
    QString path =QFileDialog::getSaveFileName(this,
                                               tr("Save file!"),
                                               currentPath+"/KeyPoint",
                                               tr("File(*.csv)"));
    if(path.isEmpty())
    {
        groupWarning(QObject::tr("path is empty!"));
        return;
    }
    int pos = path.lastIndexOf("/");
    QString filename = path.mid(pos);
    filename.remove(0,1);
    produceFileName(filename);
    path = currentPath+"/KeyPoint/"+filename+"KeyFrames.csv";
    QString path2(currentPath+"/MiniAction/"+filename+"MiniAction.bin");

    emit saveFileSignal(path,path2);
    groupWarning(QObject::tr("File has saved!"));
    refreshFileList();
}

void ArmManage::on_pushButton_RenameActionFile_clicked()
{
    /*
     *1.first check is selected a item
     *2.then check input file name is OK !
     */
    if(ui->tableWidget_DigitalActionList->currentRow()<0)
    {
        groupWarning(QObject::tr("Without choosing the file!"));
        return;
    }
    int currentRow = ui->tableWidget_DigitalActionList->currentRow();

    QString actionName = currentPath+"/KeyPoint/"+
            ui->tableWidget_DigitalActionList->item(currentRow,0)->text();
    currentEditFile= currentPath+"/KeyPoint/"+currentEditFile;
    if(actionName == currentEditFile)
    {
        groupWarning(QObject::tr("The file is Editing!"));
        return;
    }

    bool ok;
    QString newName = QInputDialog::getText(this, tr("Change file name"),
                                            tr("New file name"), QLineEdit::Normal,
                                            QDir::home().dirName(), &ok);
    QString tempFile = ui->tableWidget_DigitalActionList->item(currentRow,0)->text();
    tempFile.remove("KeyFrames.csv");
    QString binFile = currentPath+"/MiniAction/"+tempFile+"MiniAction.bin";
    if (ok && !newName.isEmpty())
    {
        QFile file(actionName);
        QFile file2(binFile);
        if(file.open(QIODevice::ReadWrite)&&file2.open(QIODevice::ReadWrite))
        {
            int pos = actionName.lastIndexOf("/");
            int pos2 = binFile.lastIndexOf("/");
            QString binName = binFile.mid(pos2);
            QString filename = actionName.mid(pos);
            filename.remove(0,1);
            binName.remove(0,1);
            QStringList binList = binName.split("_");
            QStringList strList = filename.split("_");
            if(strList.size()<3||binList.size()<3)
            {
                groupWarning(QObject::tr("The file name is wrong!"));
                return;
            }
            binName = currentPath+"/MiniAction/"+binList.at(0)+"_"+newName+"_"+binList.at(2);
            newName = currentPath+"/KeyPoint/"+strList.at(0)+"_"+newName+"_"+strList.at(2);
            file.rename(newName);
            file2.rename(binName);
            file.close();
            file2.close();
            return;
        }
        else
        {
            groupWarning(QObject::tr("The file is not open!"));
            return;
        }
    }
}

void ArmManage::on_pushButton_Cover_clicked()
{
    if(ui->tableWidget_DigitalActionList->currentRow()<0)
    {
        groupWarning(QObject::tr("Without choosing the file!"));
        return;
    }

    emit coverDataSignal();
    // keyframe actionmanage
    int row = ui->tableWidget_DigitalActionList->currentRow();
    QString fileName = ui->tableWidget_DigitalActionList->item(row,0)->text();
    QString path1 = currentPath+"/KeyPoint/"+fileName;
    fileName.remove("KeyFrames.csv");
    QString path2 = currentPath+"/MiniAction/"+fileName+"MiniAction.bin";
    emit saveFileSignal(path1,path2);
    refreshFileList();
}

void ArmManage::on_pushButton_DeleteMiniAction_clicked()
{
    int row = ui->tableWidget_DigitalActionList->currentRow();
    if(row<0)
    {
        groupWarning(QObject::tr("Nothing is choosed!"));
        return;
    }

    QString fileName = ui->tableWidget_DigitalActionList->item(row,0)->text();
    if(fileName==currentEditFile)
    {
        groupWarning(QObject::tr("The file is editing!"));
        return;
    }

    QMessageBox msgBox;
    msgBox.setText(tr("Are you sure to Delete \"") + fileName + tr(" \" ?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch(ret)
    {
    case QMessageBox::Ok:
    {
        ui->tableWidget_DigitalActionList->removeRow(row);
        QDir dir1(currentPath+"/KeyPoint");
        dir1.remove(fileName);
        QDir dir2(currentPath+"/MiniAction");
        fileName = fileName.remove("KeyFrames.csv")+"MiniAction.bin";
        dir2.remove(fileName);
        break;
    }

    case QMessageBox::Cancel:
        break;
    }
}

void ArmManage::on_pushButton_DownSelectMiniAction_clicked()
{
    int currentRow = ui->tableWidget_DigitalActionList->currentRow();
    if(currentRow<0)
    {
        groupWarning(QObject::tr("Without choosing the file!"));
        return;
    }

    QString fileName = ui->tableWidget_DigitalActionList->item(currentRow,0)->text();
    fileName.remove("KeyFrames.csv");
    fileName = currentPath + "/MiniAction/" + fileName+"MiniAction.bin";
    emit downloadMiniActionSignal(fileName);
}

void ArmManage::on_pushButton_RefreshList_clicked()
{
    refreshFileList();
}

void ArmManage::on_pushButton_PlaySpeed_clicked()
{
    int timespace = QInputDialog::getInt(this, tr("set time space"),
                                         tr("time space:"),20);
    if(timespace/20>12)
    {
        groupWarning(QObject::tr("space time is too long!"));
        return;
    }
    if(timespace/20<1)
    {
        groupWarning(QObject::tr("space time is too short!"));
        return;
    }

    timespace = (timespace/20)*20;
    emit timeSpaceChanged(timespace);
}

void ArmManage::on_pushButton_UpLoadMiniAction_clicked()
{
    emit uploadMiniActionSignal();
}

void ArmManage::on_pushButton_PlayCurrentActionFrame_clicked()
{
    int currentRow = ui->tableWidget_UploadActionList->currentRow();
    if(currentRow<0)
    {
        groupWarning(QObject::tr("Without choosing file!"));
        return;
    }

    QString actionName;
    actionName =  ui->tableWidget_UploadActionList->item(currentRow,0)->text();
    emit playMiniActionSignal(actionName);
}

void ArmManage::on_pushButton_DeleteLoadAction_clicked()
{
    int currentRow = ui->tableWidget_UploadActionList->currentRow();
    if(currentRow<0)
    {
        groupWarning(QObject::tr("Without choosing the file!"));
        return;
    }

    QString actionName;
    actionName =  ui->tableWidget_UploadActionList->item(currentRow,0)->text();

    QMessageBox msgBox;
    msgBox.setText(tr("Are you sure to Delete \"") + actionName + tr(" \" ?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch(ret)
    {
    case QMessageBox::Ok:
        emit deleteMiniActionSignal(actionName);
        break;

    case QMessageBox::Cancel:
        break;
    }
}

void ArmManage::on_pushButton_ExecuteSequence_clicked()
{
    emit executeSequenceSignal();
}

void ArmManage::on_pushButton_DownSequence_clicked()
{
    vector<int> id;
    QString sequence = QInputDialog::getText(this,tr("file id split with space"),tr("put in:"));
    if(sequence.isEmpty())
    {
        groupWarning(QObject::tr("sequence is empty!"));
        return;
    }
    QStringList test = sequence.split(" ",QString::SkipEmptyParts);
    for(int i=0;i<test.size();i++)
    {
        id.push_back(test[i].toInt());
    }
    emit downSequenceSignal(id);
}

void ArmManage::on_tableWidget_DigitalActionList_doubleClicked(const QModelIndex &index)
{
    index;
    int row = ui->tableWidget_DigitalActionList->currentRow();
    QString filePath = currentPath+"/KeyPoint/"+
            ui->tableWidget_DigitalActionList->item(row,0)->text();

    currentEditFile = ui->tableWidget_DigitalActionList->item(row,0)->text();
    emit openFileSignal(filePath);
    groupWarning(tr("open file successfully!"));
}

void ArmManage::getUpLoadFileNameSlot(QStringList data)
{
    int rowCount = ui->tableWidget_UploadActionList->rowCount();
    for(int i=rowCount-1; i>=0; i--)
    {
        ui->tableWidget_UploadActionList->removeRow(i);
    }
    for(int i=0; i<(int)data.size(); i++)
    {
        ui->tableWidget_UploadActionList->insertRow(i);
        ui->tableWidget_UploadActionList->setItem(i,0,new QTableWidgetItem(data.at(i)));
    }
}

void ArmManage::closeEvent(QCloseEvent *event)
{
    showMinimized();
    event->ignore();
}
