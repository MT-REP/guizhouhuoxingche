#ifndef MOTUSPLCOCONTROL_H
#define MOTUSPLCOCONTROL_H

#include <QObject>
#include "motusbaseplc.h"
class MotusPlcOControl : public QObject
{
    Q_OBJECT
public:
    explicit MotusPlcOControl(QObject *parent = nullptr);
    void initPara(MotusBasePlc *sMotusBasePlc,int sposition);
    bool getStatus(int index);
    void setValue(bool value,int index);
    bool setControl(bool value,int index);
private:
    MotusBasePlc *mMotusBasePlc;
    int position;
signals:
public slots:
};

#endif // MOTUSPLCOCONTROL_H
