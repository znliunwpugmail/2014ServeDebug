#ifndef MAINSTRUCTURE_H
#define MAINSTRUCTURE_H

using namespace std;

#include <vector>
#include <QList>

struct Point
{
    double x;
    double y;
    Point()
    {
        x=0;
        y=0;
    }
};

typedef vector<Point> PointList;

struct InstrFrameInfo
{
    quint8* Id;
    quint16* Position;
    quint16*Speed;
    int Size;
};

struct AcitonChannelInfo
{
    int id;
    vector<Point> keyPoint;
    vector<Point> InterPoint;
    vector<Point> speedPoint;
    void clear()
    {
        keyPoint.clear();
        InterPoint.clear();
        speedPoint.clear();
    }
};

typedef struct ServoInfo
{
    int servoId;
    int currentPos;
    int currentSpeed;
} *pServoInfo;



#endif // MAINSTRUCTURE_H
