#include "motusplayer.h"

MotusPlayer::MotusPlayer(QObject *parent) : QObject(parent)
{
    recvStr="";
}

//初始化函数
void MotusPlayer::initPara()
{
    mMotusSocket.setRemoteIpAndPort("192.168.0.126",10000,0);
    mMotusSocket.initSocket(20001);
    connect(&mMotusSocket, SIGNAL(sendDataSign(char*,int,QHostAddress)),this, SLOT(recvDataSign(char*,int,QHostAddress)));
}

//接收数据信号槽函数
void MotusPlayer::recvDataSign(char *data,int lenght,QHostAddress recvRemoteaddr)
{
    static QString  strIP="192.168.0.126";
    char recvData[100]={0};
    QString recvIP;
    recvIP=recvRemoteaddr.toString().remove("::ffff:");
    //qDebug()<<recvRemoteaddr.toString();
    if(recvIP.compare(strIP)==0)
    {
        memcpy(recvData,data,lenght);
        recvStr=recvData;
    }
}

//得到数据
QString MotusPlayer::getData()
{
    return recvStr;
}

//发送数据
void MotusPlayer::sendData(char *data,int lenght)
{
    mMotusSocket.sendData(data,lenght,0);
}

