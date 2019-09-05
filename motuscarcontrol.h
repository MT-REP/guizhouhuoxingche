#ifndef MOTUSCARCONTROL_H
#define MOTUSCARCONTROL_H

#include <QObject>
#include "motusbaseplc.h"

class MotusCarControl : public QObject
{
    Q_OBJECT
public:
    explicit MotusCarControl(QObject *parent = nullptr);
    void initPara(MotusBasePlc *sMotusBasePlc);
    bool getLimitStatus(int index);
    bool getFrontStopStatus(int index);
    bool getBackStopStatus(int index);
    bool getFrontLowSpeed(int index);
    bool getBackLowSpeed(int index);
    bool getGasBrake(int index);
    bool getFrontControl(int index);
    bool getBackControl(int index);
    bool getHighSpeedControl(int index);
    bool getLowSpeedControl(int index);
    bool getGasBrakeControl(int index);
    bool setFrontControl(bool value,int index);
    bool setBackControl(bool value,int index);
    bool setHighSpeedControl(bool value,int index);
    bool setLowSpeedControl(bool value,int index);
    bool setGasBrakeControl(bool value,int index);
    bool setCarFront(int index);
    bool setCarBack(int index);
    bool clearOutput(int index);
private:
    MotusBasePlc *mMotusBasePlc;
    int inposition[6];
    int outposition[5];
signals:
public slots:
};

#endif // MOTUSCARCONTROL_H
