#include "motusplcocontrol.h"

MotusPlcOControl::MotusPlcOControl(QObject *parent) : QObject(parent)
{
    mMotusBasePlc=NULL;
    position=0;
}

//初始化函数
void MotusPlcOControl::initPara(MotusBasePlc *sMotusBasePlc,int sposition)
{
    mMotusBasePlc=sMotusBasePlc;
    position=sposition;
}

//得到状态
bool MotusPlcOControl::getStatus()
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&position,1))
    {
        return ret;
    }
    return ret;
}

//设置数值
bool MotusPlcOControl::setControl(bool value)
{
    if(getStatus()!=value)
    {
        setValue(value);
        return false;
    }
    return true;
}

//设置数值
void MotusPlcOControl::setValue(bool value)
{
    if(mMotusBasePlc==NULL)
        return;
    mMotusBasePlc->writePlcIo(&value,&position,1);
}
