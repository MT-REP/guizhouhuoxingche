#ifndef MOTUSSINGLECONTROL_H
#define MOTUSSINGLECONTROL_H

#include <QObject>
#include "motusbaseplc.h"

class MotusSingleControl : public QObject
{
    Q_OBJECT
public:
    explicit MotusSingleControl(QObject *parent = nullptr);
    void initPara(MotusBasePlc *sMotusBasePlc,int sinposition,int soutposition);
    bool setControl(bool value,int index);
    bool getIStatus(int index);
    bool getOStatus(int index);
    void setValue(bool value,int index);
    bool action(int index);
private:
    MotusBasePlc *mMotusBasePlc;
    int inposition;
    int outposition;
signals:

public slots:
};

#endif // MOTUSSINGLECONTROL_H
