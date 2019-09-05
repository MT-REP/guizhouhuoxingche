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
    isInit=readPlcIpFile("plcIO0.txt",mPlcMessage[0]);
    if(!isInit)
    {
        QMessageBox::information(NULL,"友情提示","文件读取失败");
        return;
    }
    //读取PLC的IO信息
    isInit=readPlcIpFile("plcIO1.txt",mPlcMessage[1]);
    if(!isInit)
    {
        QMessageBox::information(NULL,"友情提示","文件读取失败");
        return;
    }
    //qDebug()<<mPlcMessage[1].mPlcIOMessage.size();
    for(int i=0;i<7;i++)
    {
        mPlcMessage[i].index=i;
        //赋值给PLC信息的网络信息
        mPlcMessage[i].mIpMessage=mIpMessageList.at(i);
        if(i>=2)
        {
            mPlcMessage[i].mPlcIOMessage=mPlcMessage[1].mPlcIOMessage;
            mPlcMessage[i].totalLength=mPlcMessage[1].totalLength;
            mPlcMessage[i].inNum=mPlcMessage[1].inNum;
            mPlcMessage[i].outLenght=mPlcMessage[1].outLenght;
            mPlcMessage[i].valueLenght=mPlcMessage[1].valueLenght;
            //qDebug()<<mPlcMessage[i].mPlcIOMessage.size();
        }
    }
}

//参数初始化
void MotusBasePlc::initPara()
{
    if(!isInit)
        return;
    for(int i=0;i<7;i++)
    {
        //设置网络信息
        mMotusSocket.setRemoteIpAndPort(mPlcMessage[i].mIpMessage.ip,mPlcMessage[i].mIpMessage.port,mPlcMessage[i].index);
    }
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
    for(int i=0;i<7;i++)
    {
        //qDebug()<<strIP;
        //qDebug()<<mPlcMessage[i].mIpMessage.ip;
        //ip值相比较
        if(strIP.compare(mPlcMessage[i].mIpMessage.ip)==0)
        {
            //qDebug()<<1<<i<<mPlcMessage[i].totalLength<<lenght;
            if(lenght==(mPlcMessage[i].totalLength+2))
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
                    dealData(data,i);
                    break;
                }
            }
        }
    }
}

//数据处理
void MotusBasePlc::dealData(char *data,int index)
{
      //qDebug()<<index;
      //qDebug()<<mPlcMessage[0].totalLength<<mPlcMessage[0].outLenght;
      //总数据
      memcpy(mPlcMessage[index].data,&data[1],mPlcMessage[index].totalLength);
      //for(int i=0;i<mPlcMessage[index].totalLength;i++)
      //{
      //   qDebug()<<i<<(int)mPlcMessage[index].data[i]<<endl;
      //}
      //输入IN
      memcpy(mPlcMessage[index].inData,mPlcMessage[index].data,mPlcMessage[index].totalLength-mPlcMessage[index].outLenght);
      //for(int i=0;i<mPlcMessage[index].totalLength-mPlcMessage[index].outLenght;i++)
      //{
      //   qDebug()<<i<<(int)mPlcMessage[index].inData[i]<<endl;
      //}
      //输出OUT
      memcpy(mPlcMessage[index].outData,
             &mPlcMessage[index].data[mPlcMessage[index].totalLength-mPlcMessage[index].outLenght],
             mPlcMessage[index].outLenght);
      //for(int i=0;i<mPlcMessage[index].outLenght;i++)
      //{
      //   qDebug()<<i<<(int)mPlcMessage[index].outData[i]<<endl;
      //}
      mPlcMessage[index].recvCount=0;
      mPlcMessage[index].connect=true;
      //循环处理
      for(int i=0;i<mPlcMessage[index].mPlcIOMessage.size();i++)
      {
        unsigned char check=0;
        if(i<mPlcMessage[index].inNum)
        {
            check=mPlcMessage[index].inData[(mPlcMessage[index].mPlcIOMessage.at(i).position)/8];
            check=check>>(mPlcMessage[index].mPlcIOMessage.at(i).position%8);
            check&=0x01;
            if(check==mPlcMessage[index].mPlcIOMessage.at(i).volid)
            {
                mPlcMessage[index].valueData[i]=true;
            }
            else
            {
                mPlcMessage[index].valueData[i]=false;
            }
            //qDebug()<<i<<mPlcMessage[index].valueData[i]<<endl;
        }
        else
        {
            check=mPlcMessage[index].outData[(mPlcMessage[index].mPlcIOMessage.at(i).position)/8];
            check=check>>(mPlcMessage[index].mPlcIOMessage.at(i).position%8);
            check&=0x01;
            if(check==mPlcMessage[index].mPlcIOMessage.at(i).volid)
            {
                mPlcMessage[index].valueData[i]=true;
            }
            else
            {
                mPlcMessage[index].valueData[i]=false;
            }
            //qDebug()<<i<<mPlcMessage[index].valueData[i]<<endl;
        }
    }
}

//得到输入输出
bool MotusBasePlc::getPlcIOStatus(bool *data,int &lenght,int index)
{
    if(isInit&&data!=NULL)
    {
        lenght=mPlcMessage[index].valueLenght;
        memcpy(data,mPlcMessage[index].valueData,lenght);
        return true;
    }
    return false;
}

//得到PLC的状态
bool MotusBasePlc::getPlcConnectStatus(int index)
{
    mPlcMessage[index].recvCount++;
    if(mPlcMessage[index].recvCount>=30)
    {
        mPlcMessage[index].recvCount=30;
        mPlcMessage[index].connect=false;
    }
    return mPlcMessage[index].connect;
}

//得到给定PLC的状态
bool MotusBasePlc::getPlcIo(bool *data,int *postion,int length,int index)
{
    if(!mPlcMessage[index].connect||!isInit)
    {
        return false;
    }
    if(data==NULL||postion==NULL)
    {
        return false;
    }
    for(int i=0;i<length;i++)
    {
        data[i]=mPlcMessage[index].valueData[postion[i]];
    }
    return true;
}

//写PLC的状态
bool MotusBasePlc::writePlcIo(bool *data,int *postion,int length,int index)
{
    if(!mPlcMessage[index].connect||!isInit)
    {
        return false;
    }
    if(data==NULL||postion==NULL)
    {
        return false;
    }
    char writedata[10]={0x55};
    writedata[0]=0x55;
    int  writeLen=mPlcMessage[index].outLenght+2;
    for(int i=0;i<mPlcMessage[index].outLenght;i++)
    {
        writedata[1+i]=mPlcMessage[index].outData[i];
        //qDebug()<<i<<(int)writedata[1+i]<<endl;
    }
    unsigned char check=0;
    int realPositon=0;
    for(int i=0;i<length;i++)
    {
        realPositon=mPlcMessage[index].mPlcIOMessage.at(postion[i]).position;
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
            mPlcMessage[index].outData[i-1]=writedata[i];
        }
        //qDebug()<<i<<(unsigned char)writedata[i]<<endl;
    }
    writedata[writeLen-1]=check;
    mMotusSocket.sendData(writedata,writeLen,index);
    return true;
}

//读取用户数据
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
    }else
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
    }else
    {
        return false;
    }
}

