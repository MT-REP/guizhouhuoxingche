#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "motusoperationview.h"
#include "motusaflatformstatus.h"
#include "motususermanagement.h"
#include "motushand.h"
#include "motusrunlog.h"
#include "motusplatfrom.h"
#include "motusbaseplc.h"
#include "motusplcicontrol.h"
#include "motusplcocontrol.h"
#include "motussinglecontrol.h"
#include "motuscarcontrol.h"
#include <QTcpSocket>
//#include "motussocket.h"

class MotusTimer;

// const char VideoPlayCmd[6] = {0x50,0x4C,0x41,0x59,0x0D,0x0A};	                  //PLAY\r\n
// const char VideoStopCmd[6] = {0x53,0x54,0x4F,0x50,0x0D ,0x0A };                   //STOP\r\n
// const char VideoRewindCmd[8] = {0x52,0x45 ,0x57 ,0x49 ,0x4E ,0x44, 0x0D ,0x0A };   //REWIND\r\n
// const char VideoLoadCmd[14] = {0x4C,0x4F,0x41,0x44,0x20,0x30,0x30,0x31,0x2E,0x58,0x4D,0x4C,0x0D ,0x0A };//LOAD 001.:ML\r\n


namespace Ui {
class MainWindow;
}

//主控命令枚举
enum OperationCode{StartUp=1,PowerOff=2,Restoration=3,Operation=4};

//平台主指令
enum PlatfromCode{NullCode=0,Automatic=1,HandMove=2};

//平台子指令
enum PlatfromHandCmd{NullCmd=0,
                     GuardBarUp=2,GuardBarDown=3,WaittingCustomer=4,CarFront=6,CarBack=7,
                     PlatfromUp=8, PlatfromDown=9,ColdWind=10,SprayWater=11,SeatLight=12,
                     OutputClear=13,SmokeEffect=14,StrobeEffect=15,HubbleEffect=16,};
typedef struct OperaterStatus
{
    bool liftbelt[6]; //安全带
    bool makesure[6]; //确认按键
    bool slowbit[6];  //行走低速标志位
    bool platerror[6];//平台错误
    bool freerror[6]; //变频器错误
    bool playbit;
    OperaterStatus()
    {
        for(int i=0;i<6;i++)
        {
            liftbelt[i]=false;
            makesure[i]=false;
            slowbit[i]=false;
            platerror[i]=false;
            freerror[i]=false;
            playbit=false;
        }
    }
}MotusOperaterStatus;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    MotusSocket mMotusSocket;
    QString stepMessage;     //步骤信息
    QString errorMessage;    //错误信息
    bool resetBit[10];       //复位标志位
    int pathStep;            //步骤
    int viewId;              //窗口ID
    int currentMainCmd;      //主指令
    int currentSonCmd;       //子命令
    int currentHandPlatfrom; //当前平台
    bool platfromAble[6];    //是否禁用平台
    bool saftBeltBit[6];     //座椅标志位
    int platfromStatus[6];   //平台状态
    //////////////////////////////////////////
    QList<M_MovieData>playMovieData;
    int playMovieCount;
    QString movieSque;
    ///////////////////////////////////////////
    MotusTimer *mMotusTimer;
    MotusOperationView  *mMotusOperationView;
    MotusaFlatformStatus  *mMotusaFlatformStatus;
    int plcIndex;                  //PLC的索引
    MotusHand *mMotusHand;
    bool actionValue;              //
    MotusRunLog  *mMotusRunLog;
    MotusUserManagement  *mMotusUserManagement;
    QTcpSocket mQTcpSocket;
    MotusPlatfrom mMotusPlatfrom;  //平台对象
    MotusBasePlc mMotusBasePlc;    //平台基本plc类
    MotusOperaterStatus mMotusOperaterStatus;//操作中间状态结构体

    /////////////////////主PLC拥有IO点//////////////////////////
    MotusPlcIControl  scramHostIn;     //急停按键
    MotusPlcIControl  renewHostIn;     //恢复按键
    MotusPlcIControl  resetHostIn;     //复位按键
    MotusPlcIControl  continueHostIn;  //继续按键
    MotusPlcIControl  airHighHostIn;   //空压机气压高
    MotusPlcIControl  airLowHostIn;    //空压机气压低
    MotusPlcOControl  faultHostOut;    //故障指示灯
    MotusPlcOControl  renewHostOut;    //恢复指示灯
    MotusPlcOControl  resetHostOut;    //复位指示灯
    MotusPlcOControl  continueHostOut; //继续指示灯
    MotusPlcOControl  waitHostOut;     //待客指示灯
    MotusPlcOControl  smokeHostOut;    //烟雾特效
    MotusPlcOControl  strobeHostOut;   //频闪特效
    MotusPlcOControl  hubbleHostOut;   //泡泡特效
    ////////////////////////////////////////////////////

    /////////////////////从PLCIO点//////////////////////////
    MotusPlcIControl  scramFromIn;     //急停按键
    MotusPlcIControl  msureFormIn;     //确认按键
    MotusPlcIControl  resetFromIn;     //复位按键
    MotusPlcIControl  handFromIn;      //手动按键
    MotusPlcIControl  pleverDownFromIn;//压杆下限位
    MotusPlcIControl  pleverUpFromIn;  //压杆上限位
    MotusPlcIControl  pleverLockFromIn;//压杆锁限位
    MotusPlcIControl  railUpFromIn;    //护栏上限位
    MotusPlcIControl  railDownFromIn;  //护栏下限位
    MotusCarControl   mCarControl;     //小车控制
    MotusPlcIControl  freChangeFaultFromIn;//变频器故障
    MotusPlcIControl  driverFaultFromIn;   //驱动器故障
    MotusPlcIControl  cylinderFromIn;   //电动缸信号
    MotusPlcIControl  saftBelt1FromIn;  //1#安全带
    MotusPlcIControl  saftBelt2FromIn;  //2#安全带
    MotusPlcIControl  saftBelt3FromIn;  //3#安全带
    MotusPlcIControl  saftBelt4FromIn;  //4#安全带
    MotusPlcIControl  railReverFromIn;  //护栏翻转


    MotusPlcOControl  faultFromOut;       //故障指示灯
    MotusPlcOControl  mSureFromOut;       //确认指示灯
    MotusPlcOControl  resetFromOut;       //复位指示灯
    MotusPlcOControl  handFromOut;        //手动指示灯
    MotusPlcOControl  waitFromOut;        //待客指示灯
    MotusPlcOControl  railUpFromOut;      //护栏上输出
    MotusPlcOControl  railDownFromOut;    //护栏下输出
    MotusPlcOControl  pleverLockFromOut;  //压杆锁
    MotusPlcOControl  coolWindFromOut;    //冷风特效
    MotusPlcOControl  waterSprayFromOut;  //喷水特效
    MotusPlcOControl  seatLightFromOut;   //座椅照明
    ////////////////////////////////////////////////////
signals:
    void setOperateViewText(QString text);
    void sendHandPermissin(bool able);
    void sendPlay(bool isplay);
public slots:
    void recvPlatfromAble(int num ,bool able);
    void recvOperationCmd(int cmd);
    void recvPlatfromStatus(int index);
    void recvMainWindoewCmd(int num,int cmd,int ma,bool ret);
    void masterClock(void);//主时钟
    void txtDataChange(QList<M_MovieData>&movieData,QString movieorder);
private slots:
    void recvdata(QString strName, QString strPassword);
    void on_playButton_clicked();
    void on_stateButton_clicked();
    void on_handButton_clicked();
    void on_errorCheckButton_clicked();
    void on_userButton_clicked();
    void on_exitButton_clicked();
};

#endif // MAINWINDOW_H
