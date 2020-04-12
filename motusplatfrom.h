#ifndef MOTUSPLATFROM_H
#define MOTUSPLATFROM_H

#include <QObject>
#include "motussocket.h"

//typedef struct DataToHostStruct
//{
//    unsigned char nCheckID;
//    unsigned char nDOFStatus;
//    unsigned char nRev0;
//    unsigned char nRev1;
//    float attitude[6];			//实际姿态
//    float para[6];				//错误代码
//    float motor_code[6];		//电机码值
//    DataToHostStruct()
//    {
//        nCheckID=55;
//        nDOFStatus=255;
//        nRev0=0;
//        nRev1=0;
//        for(int i=0;i<6;i++)
//        {
//            attitude[i]=0.f;			//实际姿态
//            para[i]=0.f;				//错误代码
//            motor_code[i]=0.f;		//电机码值
//        }
//    }
//}DataToHost;

typedef struct DataToHost			//总共108字节
{
    unsigned char CheckID;			//标识位 固定为55
    unsigned char DOFStatus;		//下位机状态
    unsigned char DI;				//数字输入
    unsigned char Rev1;			    //保留位
    float Attitudes[6];	            //实际姿态
    float ErrorCode[6];	            //错误代码
    float MotorCode[6];	            //电机码值
    float Tor[6];			        //电机力矩
    unsigned int Version;			//下位机固件版本
    unsigned int Time;			    //时间戳
    DataToHost()
    {
        CheckID=55;			//标识位 固定为55
        DOFStatus=255;		//下位机状态
        DI=0;				//数字输入
        Rev1=0;			    //保留位
        Version=0;			//下位机固件版本
        Time=0;			    //时间戳
        for(int i=0;i<6;i++)
        {
            Attitudes[i]=0.f;	 //实际姿态
            ErrorCode[i]=0.f;	 //错误代码
            MotorCode[i]=0.f;	 //电机码值
            Tor[i]=0.f;			 //电机力矩
        }
    }
}DataToHost;

//typedef struct DataToDOFStruct
//{
//    unsigned char nCheckID;			//标识位
//    unsigned char nCmd;				//状态指令
//    unsigned char nAct;				//特效指令
//    unsigned char nReserved;	        //保留
//    float DOFs[6];			//{横摇，纵倾，航向，前向，侧向，升降}
//    float Vxyz[3];			//{前向，侧向，升降}，向右为正，向下为正（速度）
//    float Axyz[3];			//...（加速度）
//    DataToDOFStruct()
//    {
//        nCheckID=55;
//        nCmd=0;
//        nAct=0;
//        nReserved=0;
//        for(int i=0;i<6;i++)
//        {
//            if(i<3)
//            {
//                Vxyz[i]=0.f;
//                Axyz[i]=0.f;
//            }
//            DOFs[i]=0.f;
//        }
//    }
//}DataToDOF;

typedef struct DataToDOF		//总共180字节
{
    unsigned char CheckID;		//标识位 固定为55
    unsigned char Cmd;		    //状态指令
    unsigned char SubCmd;		//子状态指令
    unsigned char FileNum;		//本地文件运行文件编号
    unsigned char DO;			//数字量输出
    unsigned char CyChoose;	    //运动缸选择，按位置1
    short JogSpeed;	    //手动模式运行速度
    float DOFs[6];	    //随动模式姿态：纵倾，横摇，航向，横移，前冲，升降
    float Vxyz[3];	    //洗出算法三轴角速度
    float Axyz[3];	    //洗出算法三轴位移加速度
    float Amp[6];		//指令运行正弦幅值
    float Fre[6];		//指令运行正弦频率
    float Pha[6];		//指令运行正弦相位
    float Pos[6];		//指令运行单步目标位置
    float Spd[6];		//指令运行单步运行速度
    unsigned int Time;		    //时间戳
    DataToDOF()
    {
        CheckID=55;
        Cmd=0;
        SubCmd=0;
        FileNum=0;
        for(int i=0;i<6;i++)
        {
            if(i<3)
            {
                Vxyz[i]=0.f;    //洗出算法三轴角速度
                Axyz[i]=0.f;	//洗出算法三轴位移加速度
            }
            DOFs[i]=0.f;	//随动模式姿态：纵倾，横摇，航向，横移，前冲，升降
            Amp[i]=0.f;		//指令运行正弦幅值
            Fre[i]=0.f;		//指令运行正弦频率
            Pha[i]=0.f;		//指令运行正弦相位
            Pos[i]=0.f;		//指令运行单步目标位置
            Spd[i]=0.f;		//指令运行单步运行速度
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
    DataToHost  mDataToHost;        //接收数据结构体
    int recvCount;                  //接收计数
    DataToDOF  mDataToDOF;          //发送数据结构体
public:
    void getPlatfromStatus(int &status);//得到平台状态
    bool getConnect(); //得到平台是否连接正常
    void sendPlatfromCmd(int cmd);//发送命令
    void sendPlatfromAttu(float attu[6]);//发送姿态
signals:
public slots:
    void recvDataSign(char *data,int lenght,QHostAddress recvRemoteaddr);
};

#endif // MOTUSPLATFROM_H
