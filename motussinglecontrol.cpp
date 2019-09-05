#include "motussinglecontrol.h"

MotusSingleControl::MotusSingleControl(QObject *parent) : QObject(parent)
{
    mMotusBasePlc=NULL;
    inposition=0;
    outposition=0;
}

//初始化参数
void MotusSingleControl::initPara(MotusBasePlc *sMotusBasePlc,int sinposition,int soutposition)
{
    mMotusBasePlc=sMotusBasePlc;
    inposition=sinposition;
    outposition=soutposition;
}

//得到I点状态
bool MotusSingleControl::getIStatus(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&inposition,1,index))
    {
        return ret;
    }
    return ret;
}

//得到O点状态
bool MotusSingleControl::getOStatus(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&outposition,1,index))
    {
        return ret;
    }
    return ret;
}

//设置数值
void MotusSingleControl::setValue(bool value,int index)
{
    if(mMotusBasePlc==NULL)
        return;
    mMotusBasePlc->writePlcIo(&value,&outposition,1,index);
}


//设置数值
bool MotusSingleControl::setControl(bool value,int index)
{
    if(getOStatus(index)!=value)
    {
        setValue(value,index);
        return false;
    }
    return true;
}

//实现动作
bool MotusSingleControl::action(int index)
{
    if(getIStatus(index)&&!getOStatus(index))
        return true;
    //信号无就输出
    if(!getIStatus(index))
    {
        setValue(true,index);
        return false;
    }
    //信号有检查输出
    if(getIStatus(index)&&getOStatus(index))
    {
        setValue(false,index);
        return false;
    }
}

