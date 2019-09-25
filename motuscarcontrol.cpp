#include "motuscarcontrol.h"
#include <QDebug>
//构造函数
MotusCarControl::MotusCarControl(QObject *parent) : QObject(parent)
{
    //////////输入//////////////
    inposition[0]=10;
    inposition[1]=11;
    inposition[2]=12;
    inposition[3]=13;
    inposition[4]=14;
    inposition[5]=15;
    //////////输出//////////////
    outposition[0]=40;
    outposition[1]=41;
    outposition[2]=42;
    outposition[3]=43;
    outposition[4]=44;
    outposition[5]=48;
    ////////////////////////////
}

//初始化函数
void MotusCarControl::initPara(MotusBasePlc *sMotusBasePlc)
{
    if(sMotusBasePlc==NULL)
        return;
    mMotusBasePlc=sMotusBasePlc;
}

//得到极限位
bool MotusCarControl::getLimitStatus(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&inposition[0],1,index))
    {
        return ret;
    }
    return ret;
}

//得到前进停止位
bool MotusCarControl::getFrontStopStatus(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&inposition[1],1,index))
    {
        return ret;
    }
    return ret;
}

//得到后退停止位
bool MotusCarControl::getBackStopStatus(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&inposition[2],1,index))
    {
        return ret;
    }
    return ret;
}

//得到前进减速位
bool MotusCarControl::getFrontLowSpeed(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&inposition[3],1,index))
    {
        return ret;
    }
    return ret;
}

//得到后退减速位
bool MotusCarControl::getBackLowSpeed(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&inposition[4],1,index))
    {
        return ret;
    }
    return ret;
}

//得到气动抱闸
bool MotusCarControl::getGasBrake(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&inposition[5],1,index))
    {
        return ret;
    }
    return ret;
}

//得到前进输出
bool MotusCarControl::getFrontControl(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&outposition[0],1,index))
    {
        return ret;
    }
    return ret;
}

//得到后退输出
bool MotusCarControl::getBackControl(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&outposition[1],1,index))
    {
        return ret;
    }
    return ret;
}

//得到大车高速
bool MotusCarControl::getHighSpeedControl(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&outposition[2],1,index))
    {
        return ret;
    }
    return ret;
}

//得到大车低速
bool MotusCarControl::getLowSpeedControl(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&outposition[3],1,index))
    {
        return ret;
    }
    return ret;
}

//得到气动气刹
bool MotusCarControl::getGasBrakeControl(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&outposition[4],1,index))
    {
        return ret;
    }
    return ret;
}

//得到气动气刹关
bool MotusCarControl::getGasBrakeCloseControl(int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    bool ret=false;
    if(mMotusBasePlc->getPlcIo(&ret,&outposition[5],1,index))
    {
        return ret;
    }
    return ret;
}

//设置前进输出
bool MotusCarControl::setFrontControl(bool value,int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    if(mMotusBasePlc->writePlcIo(&value,&outposition[0],1,index))
    {
        return true;
    }
    return false;
}

//设置后退输出
bool MotusCarControl::setBackControl(bool value,int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    if(mMotusBasePlc->writePlcIo(&value,&outposition[1],1,index))
    {
        return true;
    }
    return false;
}

//设置大车高速
bool MotusCarControl::setHighSpeedControl(bool value,int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    if(mMotusBasePlc->writePlcIo(&value,&outposition[2],1,index))
    {
        return true;
    }
    return false;
}

//设置大车低速
bool MotusCarControl::setLowSpeedControl(bool value,int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    if(mMotusBasePlc->writePlcIo(&value,&outposition[3],1,index))
    {
        return true;
    }
    return false;
}

//设置气动气刹
bool MotusCarControl::setGasBrakeControl(bool value,int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    if(mMotusBasePlc->writePlcIo(&value,&outposition[4],1,index))
    {
        return true;
    }
    return false;
}

//设置气动气刹关
bool MotusCarControl::setGasBrakeCloseControl(bool value,int index)
{
    if(mMotusBasePlc==NULL)
        return false;
    if(mMotusBasePlc->writePlcIo(&value,&outposition[5],1,index))
    {
        return true;
    }
    return false;
}

//清除输出
bool MotusCarControl::clearOutput(int index)
{
    //前进关闭
    if(getFrontControl(index))
    {
        setFrontControl(false,index);
        return false;
    }
    //后退关闭
    if(getBackControl(index))
    {
        setBackControl(false,index);
        return false;
    }
    //高速关闭
    if(getHighSpeedControl(index))
    {
        setHighSpeedControl(false,index);
        return false;
    }
    //低速关闭
    if(getLowSpeedControl(index))
    {
        setLowSpeedControl(false,index);
        return false;
    }
    //气刹输出
    if(getGasBrakeControl(index))
    {
        setGasBrakeControl(false,index);
        return false;
    }
    //气刹输出
    if(!getGasBrakeCloseControl(index))
    {
        setGasBrakeCloseControl(true,index);
        return false;
    }
    return true;
}


