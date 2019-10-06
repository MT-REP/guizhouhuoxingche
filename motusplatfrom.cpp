#include "motusplatfrom.h"
#include <QDebug>
//构造函数
MotusPlatfrom::MotusPlatfrom(QObject *parent) : QObject(parent)
{
    for(int i=0;i<MaxNum;i++)
    {
        recvCount[i]=0;          //接收计数
    }
}

//初始化函数
void MotusPlatfrom::initPara()
{
    mMotusSocket.setRemoteIpAndPort("192.168.1.121",5000,0);
    mMotusSocket.setRemoteIpAndPort("192.168.1.122",5000,1);
    mMotusSocket.setRemoteIpAndPort("192.168.1.123",5000,2);
    mMotusSocket.setRemoteIpAndPort("192.168.1.124",5000,3);
    mMotusSocket.setRemoteIpAndPort("192.168.1.125",5000,4);
    mMotusSocket.setRemoteIpAndPort("192.168.1.126",5000,5);
    mMotusSocket.initSocket(10000);
    connect(&mMotusSocket, SIGNAL(sendDataSign(char*,int,QHostAddress)),this, SLOT(recvDataSign(char*,int,QHostAddress)));
}

//接收数据信号槽函数
void MotusPlatfrom::recvDataSign(char *data,int lenght,QHostAddress recvRemoteaddr)
{
    static QString  strIP[6]={"192.168.1.121","192.168.1.122","192.168.1.123","192.168.1.124","192.168.1.125","192.168.1.126"};
    QString recvIP;
    recvIP=recvRemoteaddr.toString().remove("::ffff:");
    for(int i=0;i<6;i++)
    {
        //qDebug()<<recvRemoteaddr.toString();
        if(recvIP.compare(strIP[i])==0&&lenght==sizeof(DataToHost))
        {
            memcpy(&mDataToHost[i],data,lenght);
            recvCount[i]=0;
            break;
        }
    }
}

//得到平台状态
void MotusPlatfrom::getPlatfromStatus(int status[6])
{
    for(int i=0;i<6;i++)
    {
        recvCount[i]++;
        if(recvCount[i]>=100)
        {
            recvCount[i]=100;
            status[i]=255;
        }
        else
        {
            status[i]=mDataToHost[i].nDOFStatus;
        }

    }
}

//得到是否通讯正常
bool MotusPlatfrom::getConnect(int index)
{
    if(recvCount[index]>=100)
        return false;
    return true;
}

//发送命令
void MotusPlatfrom::sendPlatfromCmd(int cmd,int index)
{
    mDataToDOF.nCmd=cmd;
    for(int i=0;i<6;i++)
    {
        mDataToDOF.DOFs[i]=0.f;
    }
    mMotusSocket.sendData((char *)&mDataToDOF,sizeof(DataToDOF),index);
}

//发送姿态
void MotusPlatfrom::sendPlatfromAttu(float attu[6],int index)
{
    mDataToDOF.nCmd=9;
    for(int i=0;i<6;i++)
    {
        mDataToDOF.DOFs[i]=attu[i];
    }
    mMotusSocket.sendData((char *)&mDataToDOF,sizeof(DataToDOF),index);
}






