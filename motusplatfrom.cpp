#include "motusplatfrom.h"
#include <QDebug>
//构造函数
MotusPlatfrom::MotusPlatfrom(QObject *parent) : QObject(parent)
{
    recvCount=0;          //接收计数
}

//初始化函数
void MotusPlatfrom::initPara()
{
    mMotusSocket.setRemoteIpAndPort("192.168.0.125",5000,0);
    mMotusSocket.initSocket(10000);
    connect(&mMotusSocket, SIGNAL(sendDataSign(char*,int,QHostAddress)),this, SLOT(recvDataSign(char*,int,QHostAddress)));
}

//接收数据信号槽函数
void MotusPlatfrom::recvDataSign(char *data,int lenght,QHostAddress recvRemoteaddr)
{
    static QString  strIP="192.168.0.125";
    QString recvIP;
    recvIP=recvRemoteaddr.toString().remove("::ffff:");
    //qDebug()<<recvRemoteaddr.toString();
    if(recvIP.compare(strIP)==0&&lenght==sizeof(DataToHost))
    {
        memcpy(&mDataToHost,data,lenght);
        recvCount=0;
    }
}

//得到平台状态
void MotusPlatfrom::getPlatfromStatus(int &status)
{
    recvCount++;
    if(recvCount>=100)
    {
        recvCount=100;
        status=255;
    }
    else
    {
        status=mDataToHost.DOFStatus;
    }
}

//得到是否通讯正常
bool MotusPlatfrom::getConnect()
{
    if(recvCount>=100)
        return false;
    return true;
}

//发送命令
void MotusPlatfrom::sendPlatfromCmd(int cmd)
{
    mDataToDOF.Cmd=cmd;
    for(int i=0;i<6;i++)
    {
        mDataToDOF.DOFs[i]=0.f;
    }
    mMotusSocket.sendData((char *)&mDataToDOF,sizeof(DataToDOF),0);
}

//发送姿态
void MotusPlatfrom::sendPlatfromAttu(float attu[6])
{
    mDataToDOF.Cmd=9;
    for(int i=0;i<6;i++)
    {
        mDataToDOF.DOFs[i]=attu[i];
    }
    mMotusSocket.sendData((char *)&mDataToDOF,sizeof(DataToDOF),0);
}






