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
#include "motuskeeptime.h"
#include <QTcpSocket>
#include <QCloseEvent>
#include "motusplayer.h"
//#include <QToolBar> //
//#include <QAction>  //
class MotusTimer;

// const char VideoPlayCmd[6] = {0x50,0x4C,0x41,0x59,0x0D,0x0A};	                  //PLAY\r\n
// const char VideoStopCmd[6] = {0x53,0x54,0x4F,0x50,0x0D ,0x0A };                    //STOP\r\n
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
enum PlatfromHandCmd{NullCmd=0,PlatfromUp=1,PlatfromDown=2,SeatLight=3,ColdWind=4};
typedef struct OperaterStatus
{
    bool liftbelt[6]; //安全带
    bool platerror[6];//平台错误
    bool freerror[6]; //变频器错误
    bool playbit;
    OperaterStatus()
    {
        for(int i=0;i<6;i++)
        {
            liftbelt[i]=false;
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
//    QToolBar *toolbar;
//    QAction *act[6];
    MotusSocket mMotusSocket;
    QString stepMessage;     //步骤信息
    QString errorMessage;    //错误信息
    bool shutDownBit;        //关机标志位
    bool resetBit[10];       //复位标志位
    int pathStep;            //步骤
    int viewId;              //窗口ID
    int currentMainCmd;      //主指令
    int currentSonCmd;       //子命令
    bool safetyBeltAble;     //是否禁用安全带
    bool saftBeltBit[6];     //座椅标志位
    int platfromStatus;      //平台状态
    //////////////////////////////////////////
    QList<M_MovieData>playMovieData;
    int playMovieCount;
    QString movieSque;
    ///////////////////////////////////////////
    MotusTimer *mMotusTimer;
    MotusOperationView  *mMotusOperationView;
    MotusaFlatformStatus  *mMotusaFlatformStatus;
    MotusHand *mMotusHand;
    bool actionValue;
    MotusRunLog  *mMotusRunLog;
    MotusUserManagement  *mMotusUserManagement;
    //QTcpSocket mQTcpSocket;
    MotusPlayer mMotusPlayer;
    MotusPlatfrom mMotusPlatfrom;  //平台对象
    MotusBasePlc mMotusBasePlc;    //平台基本plc类
    MotusKeepTime mMotusKeepTime;  //时间限制类
    bool timeOpen;
    MotusOperaterStatus mMotusOperaterStatus;//操作中间状态结构体

    /////////////////////主PLC拥有IO点//////////////////////////
    MotusPlcIControl  scramHostIn;     //急停按键
    MotusPlcIControl  runHostIn;       //运行按键
    MotusPlcIControl  resetHostIn;     //复位按键
    MotusPlcIControl  errorHostIn;     //驱动器故障
    MotusPlcIControl  safetyBelt1;     //安全带1
    MotusPlcIControl  safetyBelt2;     //安全带2
    MotusPlcIControl  safetyBelt3;     //安全带3
    MotusPlcIControl  safetyBelt4;     //安全带4
    MotusPlcIControl  safetyBelt5;     //安全带5
    MotusPlcIControl  safetyBelt6;     //安全带6
    MotusPlcIControl  safetyBelt7;     //安全带7
    MotusPlcIControl  safetyBelt8;     //安全带8
    MotusPlcIControl  safetyBelt9;     //安全带9
    MotusPlcIControl  carDoor1;        //舱门1
    MotusPlcIControl  carDoor2;        //舱门2
    MotusPlcIControl  platfromLow;     //平台寻底信号
    MotusPlcOControl  runHostOut;      //待客指示灯
    MotusPlcOControl  resetHostOut;    //复位指示灯
    MotusPlcOControl  faultHostOut;    //故障指示灯
    MotusPlcOControl  lightHostOut;    //照明
    MotusPlcOControl  fanHostOut;      //风扇
    ////////////////////////////////////////////////////
    void closeEvent(QCloseEvent *event);
signals:
    void setOperateViewText(QString text);
    void sendHandPermissin(bool able);
    void sendPlay(bool isplay);
private:
    void buttonView(int view);
public slots:
    void recvSafetyBeltAble(bool able);
    void recvOperationCmd(int cmd);
    void recvMainWindoewCmd(int cmd,int ma,bool ret);
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
