#ifndef MOTUSPLCICONTROL_H
#define MOTUSPLCICONTROL_H

#include <QObject>
#include "motusbaseplc.h"

class MotusPlcIControl : public QObject
{
    Q_OBJECT
public:
    explicit MotusPlcIControl(QObject *parent = nullptr);
    void initPara(MotusBasePlc *sMotusBasePlc,int sposition);
    bool getStatus(int index);
private:
    MotusBasePlc *mMotusBasePlc;
    int position;
signals:
public slots:
};

#endif // MOTUSPLCICONTROL_H
