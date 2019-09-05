#ifndef MOTUSBASEPLC_H
#define MOTUSBASEPLC_H

#include <QObject>
#include <QString>
#include "motussocket.h"
#include <QList>


//PLC的IP信息结构体
typedef struct IpMessageStruct{
    QString ip;   //IP信息
    int port;     //端口
    IpMessageStruct()
    {
        port=0;     //端口
    }
}IpMessage;

//PLC的IO信息结构体
typedef struct PlcIOMessageStruct{
    int index;    //索引
    int position; //位置
    int volid;    //有效
    PlcIOMessageStruct()
    {
        index=0;    //索引
        position=0; //位置
        volid=0;    //有效
    }
}PlcIOMessage;

//PLC的总信息
typedef struct PlcMessageStruct{
    int index;
    IpMessage mIpMessage;
    char data[15];
    int  totalLength;
    char inData[10];
    int  inNum;
    char outData[10];
    int  outLenght;
    bool valueData[100];
    int  valueLenght;
    int  recvCount;
    bool connect;
    QList<PlcIOMessage>mPlcIOMessage;
    PlcMessageStruct()
    {
        index=0;
        inNum=0;
        totalLength=0;
        outLenght=0;
        valueLenght=0;
        recvCount=0;
        connect=false;
        for(int i=0;i<100;i++)
        {
            if(i<10)
            {
                inData[i]=0;
                outData[i]=0;
            }
            valueData[i]=false;
        }
    }
}PlcMessage;


class MotusBasePlc : public QObject
{
    Q_OBJECT
public:
    explicit MotusBasePlc(QObject *parent = nullptr);
    void initPara();//初始化参数
    bool getPlcIOStatus(bool *data,int &lenght,int index);//得到PLC的IO状态
    bool getPlcConnectStatus(int index);//得到PLC的连接状态
    bool getPlcIo(bool *data,int *postion,int length,int index);//得到给定PLC的状态
    bool writePlcIo(bool *data,int *postion,int length,int index);//写PLC的状态
private:
    MotusSocket mMotusSocket;       //网络接收对象
    QList<IpMessage>mIpMessageList; //PLC的IP信息
    PlcMessage mPlcMessage[7];      //PLC的全部信息
    bool isInit;
    //数据处理
    void dealData(char *data,int index);
    //读取PLC的IP信息
    bool readPlcIpFile(QString filename,QList<IpMessage>&mIpMessageList);
    //读取PLC的IO信息
    bool readPlcIpFile(QString filename,PlcMessage &mPlcMessage);
signals:

public slots:
   void recvDataSign(char *data,int lenght,QHostAddress recvRemoteaddr);
};

#endif // MOTUSBASEPLC_H
