#include "motussocket.h"
#include <QMessageBox>
MotusSocket::MotusSocket(QObject *parent) : QObject(parent)
{
    isInit=false;
    for(int i=0;i<MaxNum;i++)
    {
        remoteaddr[i]=NULL;
        remotePort[i]=10000;
    }

}

//网络初始化
bool MotusSocket::initSocket(int bindPort)
{
      //创建UdpSocket
      motusUdpServer = new QUdpSocket(this);
      //绑定端口号
      if(motusUdpServer->bind(QHostAddress::Any, bindPort))
      {
          //网络数据接收
          connect(motusUdpServer, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
          //初始化完成
          isInit=true;
          return true;
      }
      else
      {
          QMessageBox box;
          box.setText("绑定IP出现\r\n网络错误");
          box.exec();
      }
      return false;
}

//设置远程IP和端口
void MotusSocket::setRemoteIpAndPort(QString remoteIP,int remotePort,int index)
{
    if(index>=MaxNum)
        return;
    if(remoteaddr[index]==NULL)
    {
        remoteaddr[index]=new QHostAddress();
        remoteaddr[index]->setAddress(remoteIP);
    }
    else
    {
        remoteaddr[index]->setAddress(remoteIP);
    }
    this->remotePort[index]=remotePort;
}

//接收数据
void MotusSocket::readPendingDatagrams()
{
     while(motusUdpServer->hasPendingDatagrams())//判断是否有待接收数据
     {
         QByteArray data;
         data.resize(motusUdpServer->pendingDatagramSize());
         motusUdpServer->readDatagram(data.data(),data.size(),&recvRemoteaddr);//读取数据
         emit sendDataSign(data.data(),data.size(),recvRemoteaddr);
     }
}

//发送数据
void MotusSocket::sendData(char data[],int length,int index)
{
    if(index>=MaxNum)
        return;
    if(remoteaddr[index]!=NULL&&isInit)
    {
        //发送数据
        motusUdpServer->writeDatagram(data,length,*(remoteaddr[index]),remotePort[index]);
    }
}

//关闭Socket
void MotusSocket::closeSocket()
{
    if(isInit)
    {
        motusUdpServer->close();
    }
    for(int i=0;i<MaxNum;i++)
    {
        if(remoteaddr[i]!=NULL)
        {
            remoteaddr[i]->clear();
        }
    }
}

//析构函数
MotusSocket::~MotusSocket()
{
    //关闭Socket
    closeSocket();
}




