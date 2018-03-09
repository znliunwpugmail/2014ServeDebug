#ifndef SERVERDEBUG_H
#define SERVERDEBUG_H

#include <QWidget>
#include <vector>
#include <QCloseEvent>

using namespace std;

class QGraphicsView;
class QPushButton;
class GraphScene;

namespace Ui {
class ServerDebug;
}

class ServerDebug : public QWidget
{
    Q_OBJECT
    
public:
    explicit ServerDebug(QWidget *parent = 0);
    ~ServerDebug();

    void initGraphScene();
    void closeEvent(QCloseEvent *event);

private:
    void initScene();
    void groupSignals();

    //enable tools
    void controlTools();

private slots:
    void requestButtonSlot();
    void enableAllButtonSlot();
    void disenableAllButtonSlot();
    void enableSomeButtonSlot();
    void disenableSomeButtonSlot();

    void controlButtonState(unsigned char type);

signals:
    void requestInformation();
    void enableAllInformation();
    void disenableAllInformation();
    void enableSomeInformation(vector<int> Id);
    void disenableSomeInformation(vector<int> Id);

    void zhSceneCreated(GraphScene* p);
    void zhSceneSignals(GraphScene* p);
    void createKeyPosWidget();
    
private:
    Ui::ServerDebug *ui;

    //scene tools--ui
    QGraphicsView* graphicsView;
    QPushButton* requestButton;
    QPushButton* enableAllButton;
    QPushButton* disenableAllButton;
    QPushButton* enableSomeButton;
    QPushButton* disenableSomeButton;

    bool toolsState;

    GraphScene* zhScene;
};

#endif // SERVERDEBUG_H
