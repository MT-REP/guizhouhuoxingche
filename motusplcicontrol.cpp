#include "motusplcicontrol.h"

//构造函数
MotusPlcIControl::MotusPlcIControl(QObject *parent) : QObject(parent)
{
    mMotusBasePlc=NULL;
    position=0;
}

//初始化函数
void MotusPlcIControl::initPara(MotusBasePlc *sMotusBasePlc,int sposition)
{
    mMotusBasePlc=sMotusBasePlc;
    position=sposition;
}

//得到状态
bool MotusPlcIControl::getStatus()
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
