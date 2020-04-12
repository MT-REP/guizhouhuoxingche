#include "motusbaseplc.h"
#include <QTextStream>
#include <QFile>
#include <QStringList>
#include <QMessageBox>
#include <QDebug>
MotusBasePlc::MotusBasePlc(QObject *parent) : QObject(parent),mMotusSocket(this)
{
    isInit=true;
    //读取IP信息
    isInit=readPlcIpFile("plcIp.txt",mIpMessageList);
    if(!isInit)
    {
        QMessageBox::information(NULL,"友情提示","文件读取失败");
        return;
    }
    //读取PLC的IO信息
    isInit=readPlcIpFile("plcIO0.txt",mPlcMessage);
    if(!isInit)
    {
        QMessageBox::information(NULL,"友情提示","文件读取失败");
        return;
    }
    mPlcMessage.mIpMessage=mIpMessageList.at(0);
}

//参数初始化
void MotusBasePlc::initPara()
{
    if(!isInit)
        return;
    //设置网络信息
    mMotusSocket.setRemoteIpAndPort(mPlcMessage.mIpMessage.ip,mPlcMessage.mIpMessage.port,0);
    isInit=mMotusSocket.initSocket(10050);
    if(!isInit)
    {
        QMessageBox::information(NULL,"友情提示","PLC网络初始化失败");
        return;
    }
    else
    {
        //连接信号槽
        connect(&mMotusSocket, SIGNAL(sendDataSign(char *,int ,QHostAddress)),this, SLOT(recvDataSign(char *,int ,QHostAddress)));
    }
}

//接收信号槽函数
void MotusBasePlc::recvDataSign(char *data,int lenght,QHostAddress recvRemoteaddr)
{
    char recvData=0;
    QString strIP;
    strIP=recvRemoteaddr.toString().remove("::ffff:");
    //qDebug()<<strIP;
    //qDebug()<<mPlcMessage[i].mIpMessage.ip;
    //ip值相比较
    if(strIP.compare(mPlcMessage.mIpMessage.ip)==0)
    {
        //qDebug()<<1<<i<<mPlcMessage[i].totalLength<<lenght;
        if(lenght==(mPlcMessage.totalLength+2))
        {
            //异或值相比较
            for(int j=0;j<lenght-1;j++)
            {
               recvData^=data[j];
            }
            //数据处理
            if(recvData==data[lenght-1])
            {
                //qDebug()<<2;
                dealData(data);
            }
        }
    }
}

//数据处理
void MotusBasePlc::dealData(char *data)
{
      //qDebug()<<index;
      //qDebug()<<mPlcMessage[0].totalLength<<mPlcMessage[0].outLenght;
      //总数据
      memcpy(mPlcMessage.data,&data[1],mPlcMessage.totalLength);
      //for(int i=0;i<mPlcMessage[index].totalLength;i++)
      //{
      //   qDebug()<<i<<(int)mPlcMessage[index].data[i]<<endl;
      //}
      //输入IN
      memcpy(mPlcMessage.inData,mPlcMessage.data,mPlcMessage.totalLength-mPlcMessage.outLenght);
      //for(int i=0;i<mPlcMessage[index].totalLength-mPlcMessage[index].outLenght;i++)
      //{
      //   qDebug()<<i<<(int)mPlcMessage[index].inData[i]<<endl;
      //}
      //输出OUT
      memcpy(mPlcMessage.outData,
             &mPlcMessage.data[mPlcMessage.totalLength-mPlcMessage.outLenght],
             mPlcMessage.outLenght);
      //for(int i=0;i<mPlcMessage[index].outLenght;i++)
      //{
      //   qDebug()<<i<<(int)mPlcMessage[index].outData[i]<<endl;
      //}
      mPlcMessage.recvCount=0;
      mPlcMessage.connect=true;
      //循环处理
      for(int i=0;i<mPlcMessage.mPlcIOMessage.size();i++)
      {
        unsigned char check=0;
        if(i<mPlcMessage.inNum)
        {
            check=mPlcMessage.inData[(mPlcMessage.mPlcIOMessage.at(i).position)/8];
            check=check>>(mPlcMessage.mPlcIOMessage.at(i).position%8);
            check&=0x01;
            if(check==mPlcMessage.mPlcIOMessage.at(i).volid)
            {
                mPlcMessage.valueData[i]=true;
            }
            else
            {
                mPlcMessage.valueData[i]=false;
            }
            //qDebug()<<i<<mPlcMessage[index].valueData[i]<<endl;
        }
        else
        {
            check=mPlcMessage.outData[(mPlcMessage.mPlcIOMessage.at(i).position)/8];
            check=check>>(mPlcMessage.mPlcIOMessage.at(i).position%8);
            check&=0x01;
            if(check==mPlcMessage.mPlcIOMessage.at(i).volid)
            {
                mPlcMessage.valueData[i]=true;
            }
            else
            {
                mPlcMessage.valueData[i]=false;
            }
            //qDebug()<<i<<mPlcMessage[index].valueData[i]<<endl;
        }
    }
}

//得到输入输出
bool MotusBasePlc::getPlcIOStatus(bool *data,int &lenght)
{
    if(isInit&&data!=NULL)
    {
        lenght=mPlcMessage.valueLenght;
        memcpy(data,mPlcMessage.valueData,lenght);
        return true;
    }
    return false;
}

//得到PLC的状态
bool MotusBasePlc::getPlcConnectStatus()
{
    mPlcMessage.recvCount++;
    if(mPlcMessage.recvCount>=30)
    {
        mPlcMessage.recvCount=30;
        mPlcMessage.connect=false;
    }
    return mPlcMessage.connect;
}

//得到给定PLC的状态
bool MotusBasePlc::getPlcIo(bool *data,int *postion,int length)
{
    if(!mPlcMessage.connect||!isInit)
    {
        return false;
    }
    if(data==NULL||postion==NULL)
    {
        return false;
    }
    for(int i=0;i<length;i++)
    {
        data[i]=mPlcMessage.valueData[postion[i]];
    }
    return true;
}

//写PLC的状态
bool MotusBasePlc::writePlcIo(bool *data,int *postion,int length)
{
    if(!mPlcMessage.connect||!isInit)
    {
        return false;
    }
    if(data==NULL||postion==NULL)
    {
        return false;
    }
    char writedata[10]={0x55};
    writedata[0]=0x55;
    int  writeLen=mPlcMessage.outLenght+2;
    for(int i=0;i<mPlcMessage.outLenght;i++)
    {
        writedata[1+i]=mPlcMessage.outData[i];
        //qDebug()<<i<<(int)writedata[1+i]<<endl;
    }
    unsigned char check=0;
    int realPositon=0;
    for(int i=0;i<length;i++)
    {
        realPositon=mPlcMessage.mPlcIOMessage.at(postion[i]).position;
        if(data[i])//判断
        {
            check=0x1<<(realPositon%8);//移位运算
            writedata[1+(realPositon/8)]=writedata[1+(realPositon/8)]|(check);
        }
        else
        {
            check=0x1<<(realPositon%8);//移位运算
            writedata[1+(realPositon/8)]=writedata[1+(realPositon/8)]&(~check);
        }
    }
    check=0;
    for(int i=0;i<writeLen-1;i++)
    {
        check^=writedata[i];
        if(i>=1)
        {
            mPlcMessage.outData[i-1]=writedata[i];
        }
        //qDebug()<<i<<(unsigned char)writedata[i]<<endl;
    }
    writedata[writeLen-1]=check;
    mMotusSocket.sendData(writedata,writeLen,0);
    return true;
}

//读取PLC的IP信息*
bool MotusBasePlc::readPlcIpFile(QString filename,QList<IpMessage>&mIpMessageList)
{
    //QDir dir;
    //qDebug()<<dir.currentPath();
    QString str="config\\";
    str+=filename;
    QFile file;
    file.setFileName(str);
    if(file.exists())
    {
        file.open(QIODevice::ReadOnly);
        QTextStream out(&file);
        while(!out.atEnd())
        {
            QString  tempstr;
            QStringList  tempstrlist;
            IpMessage tempIpMessage;
            tempstr=out.readLine();
            tempstrlist=tempstr.split (",");
            if(tempstrlist.size()!=3)
            {
               file.close();
               QMessageBox::information(NULL,"友情提示","文件格式不正确");
               return false;
            }
            tempIpMessage.ip=tempstrlist.at(0);
            tempIpMessage.port=tempstrlist.at(1).toUInt();
            mIpMessageList.append(tempIpMessage);
        }
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

//读取PLC的IO信息
bool MotusBasePlc::readPlcIpFile(QString filename,PlcMessage &mPlcMessage)
{
    //QDir dir;
    //qDebug()<<dir.currentPath();
    QString str="config\\";
    str+=filename;
    QFile file;
    file.setFileName(str);
    if(file.exists())
    {
        file.open(QIODevice::ReadOnly);
        QTextStream out(&file);
        QString  tempstr;
        QStringList  tempstrlist;
        tempstr=out.readLine();
        tempstrlist=tempstr.split (",");
        mPlcMessage.totalLength=tempstrlist.at(0).toUInt();
        //qDebug()<<mPlcMessage.totalLength;
        tempstr=out.readLine();
        tempstrlist=tempstr.split (",");
        mPlcMessage.outLenght=tempstrlist.at(0).toUInt();
        //qDebug()<<mPlcMessage.outLenght;
        tempstr=out.readLine();
        tempstrlist=tempstr.split (",");
        mPlcMessage.inNum=tempstrlist.at(0).toUInt();
        //qDebug()<<mPlcMessage.inNum;
        while(!out.atEnd())
        {
            QString  tempstr;
            QStringList  tempstrlist;
            PlcIOMessage tempPlcIOMessage;
            tempstr=out.readLine();
            tempstrlist=tempstr.split (",");
            if(tempstrlist.size()!=4)
            {
               file.close();
               QMessageBox::information(NULL,"友情提示","文件格式不正确");
               return false;
            }
            tempPlcIOMessage.index=tempstrlist.at(0).toUInt();
            tempPlcIOMessage.position=tempstrlist.at(1).toUInt();
            tempPlcIOMessage.volid=tempstrlist.at(2).toUInt();
            //qDebug()<<tempPlcIOMessage.index<<tempPlcIOMessage.position<<tempPlcIOMessage.volid;
            mPlcMessage.mPlcIOMessage.append(tempPlcIOMessage);
        }
        mPlcMessage.valueLenght=mPlcMessage.mPlcIOMessage.size();
        //qDebug()<<mPlcMessage.valueLenght;
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

