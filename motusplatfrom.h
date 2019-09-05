#ifndef MOTUSPLATFROM_H
#define MOTUSPLATFROM_H

#include <QObject>
#include "motussocket.h"


typedef struct DataToHostStruct
{
    unsigned char nCheckID;
    unsigned char nDOFStatus;
    unsigned char nRev0;
    unsigned char nRev1;

    float attitude[6];			//实际姿态
    float para[6];				//错误代码
    float motor_code[6];		//电机码值
    DataToHostStruct()
    {
        nCheckID=55;
        nDOFStatus=255;
        nRev0=0;
        nRev1=0;
        for(int i=0;i<6;i++)
        {
            attitude[i]=0.f;			//实际姿态
            para[i]=0.f;				//错误代码
            motor_code[i]=0.f;		//电机码值
        }
    }
}DataToHost;


typedef struct DataToDOFStruct
{
    unsigned char nCheckID;			//标识位
    unsigned char nCmd;				//状态指令
    unsigned char nAct;				//特效指令
    unsigned char nReserved;	        //保留

    float DOFs[6];			//{横摇，纵倾，航向，前向，侧向，升降}
    float Vxyz[3];			//{前向，侧向，升降}，向右为正，向下为正（速度）
    float Axyz[3];			//...（加速度）
    DataToDOFStruct()
    {
        nCheckID=55;
        nCmd=0;
        nAct=0;
        nReserved=0;
        for(int i=0;i<6;i++)
        {
            if(i<3)
            {
                Vxyz[i]=0.f;
                Axyz[i]=0.f;
            }
            DOFs[i]=0.f;
        }

    }
}DataToDOF;



class MotusPlatfrom : public QObject
{
    Q_OBJECT
public:
    explicit MotusPlatfrom(QObject *parent = nullptr);
    void initPara();
private:
    MotusSocket mMotusSocket;       //网络接收对象
    DataToHost  mDataToHost[MaxNum];//接收数据结构体
    int recvCount[MaxNum];          //接收计数
    DataToDOF  mDataToDOF;          //发送数据结构体
public:
    void getPlatfromStatus(int status[6]);//得到平台状态
    bool getConnect(int index); //得到平台是否连接正常
    void sendPlatfromCmd(int cmd,int index);//发送命令
    void sendPlatfromAttu(float attu[6],int index);//发送姿态
signals:
public slots:
    void recvDataSign(char *data,int lenght,QHostAddress recvRemoteaddr);
};

#endif // MOTUSPLATFROM_H
