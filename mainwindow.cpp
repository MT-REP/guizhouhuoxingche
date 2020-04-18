#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "motustimer.h"
#include <QMessageBox>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //播放主页
    mMotusOperationView=new MotusOperationView(this);
    connect(this,SIGNAL(sendPlay(bool)),mMotusOperationView,SLOT(recvPlay(bool)));
    connect(mMotusOperationView,SIGNAL(txtDataChange(QList<M_MovieData>&,QString)),this,SLOT(txtDataChange(QList<M_MovieData>&,QString)));
    connect(mMotusOperationView,SIGNAL(sendOperationCmd(int)),this,SLOT(recvOperationCmd(int)));
    connect(this,SIGNAL(setOperateViewText(QString)),mMotusOperationView,SLOT(recvOperateViewText(QString)));
    mMotusOperationView->init();
    //状态监测
    mMotusaFlatformStatus=new MotusaFlatformStatus(this);
    //手动控制
    mMotusHand=new MotusHand(this);
    actionValue=false;
    connect(mMotusHand,SIGNAL(sendSafetyBeltAble(bool)),this,SLOT(recvSafetyBeltAble(bool)));
    connect(mMotusHand,SIGNAL(sendMainWindoewCmd(int,int,bool)),this,SLOT(recvMainWindoewCmd(int,int,bool)));
    connect(this,SIGNAL(sendHandPermissin(bool)),mMotusHand,SLOT(recvHandPermissin(bool)));
    //错误记录
    mMotusRunLog=new MotusRunLog(this);
    //用户管理
    mMotusUserManagement=new MotusUserManagement(this);
    //入栈
    ui->m_stackwidget->addWidget(mMotusOperationView);
    ui->m_stackwidget->addWidget(mMotusaFlatformStatus);
    ui->m_stackwidget->addWidget(mMotusHand);
    ui->m_stackwidget->addWidget(mMotusRunLog);
    ui->m_stackwidget->addWidget(mMotusUserManagement);

    currentMainCmd=Automatic;
    currentSonCmd=NullCmd;
    pathStep=0;
    viewId=0;
    playMovieCount=0;
    shutDownBit=false;
    safetyBeltAble=true;
    platfromStatus=255;
    //复位标志位
    for(int i=0;i<10;i++)
    {
        resetBit[i]=false;
    }
    //定时器初始化
    mMotusTimer=new MotusTimer(10,this);
    //连接信号槽
    connect(mMotusTimer,SIGNAL(timeout()),this,SLOT(masterClock()));

    mMotusPlatfrom.initPara();
    mMotusBasePlc.initPara();
    ////////////////////////主plc输入输出///////////////////////////////////////
    MotusPlcOControl  runHostOut;      //待客指示灯
    MotusPlcOControl  resetHostOut;    //复位指示灯
    MotusPlcOControl  faultHostOut;    //故障指示灯
    MotusPlcOControl  lightHostOut;    //照明
    MotusPlcOControl  fanHostOut;      //风扇
    scramHostIn.initPara(&mMotusBasePlc,0);
    runHostIn.initPara(&mMotusBasePlc,1);
    resetHostIn.initPara(&mMotusBasePlc,2);
    errorHostIn.initPara(&mMotusBasePlc,3);
    safetyBelt1.initPara(&mMotusBasePlc,4);
    safetyBelt2.initPara(&mMotusBasePlc,5);
    safetyBelt3.initPara(&mMotusBasePlc,6);
    safetyBelt4.initPara(&mMotusBasePlc,7);
    safetyBelt5.initPara(&mMotusBasePlc,8);
    safetyBelt6.initPara(&mMotusBasePlc,9);
    safetyBelt7.initPara(&mMotusBasePlc,10);
    safetyBelt8.initPara(&mMotusBasePlc,11);
    safetyBelt9.initPara(&mMotusBasePlc,12);
    carDoor1.initPara(&mMotusBasePlc,13);
    carDoor2.initPara(&mMotusBasePlc,14);
    platfromLow.initPara(&mMotusBasePlc,15);

    runHostOut.initPara(&mMotusBasePlc,16);
    resetHostOut.initPara(&mMotusBasePlc,17);
    faultHostOut.initPara(&mMotusBasePlc,18);
    lightHostOut.initPara(&mMotusBasePlc,19);
    fanHostOut.initPara(&mMotusBasePlc,20);
    /////////////////////////////////////////////////////////////////////
    //int width=QApplication::desktop()->width();
    //int height=QApplication::desktop()->height();
    //this->setMinimumSize(width,height);
    //setWindowFlags (Qt::FramelessWindowHint);

    //播放界面切换
    on_playButton_clicked();

    ui->centralWidget->setLayout(ui->mainGridLayout);

    //mQTcpSocket.connectToHost("192.168.1.7",23);
    //if (!mQTcpSocket.waitForConnected(1000))
    //QMessageBox::information(this,"友情提示","未连接到视频播放器");
    mMotusPlayer.initPara();

    timeOpen=false;
    timeOpen=mMotusKeepTime.readLog();  //时间限制类
    //启动定时器
    mMotusTimer->start();
    //登录显示界面
    mMotusUserManagement->showLoginDialog();

}

//析构函数
MainWindow::~MainWindow()
{
    delete ui;
}

//接收传过来的参数
void MainWindow::txtDataChange(QList<M_MovieData>&movieData,QString movieorder)
{
    playMovieData=movieData;
    movieSque=movieorder;
}


//主时钟
void MainWindow::masterClock(void)
{
    static unsigned long timeCount=0;
    static bool plcStatus=false;
    timeCount++;
    mMotusPlatfrom.getPlatfromStatus(platfromStatus);
    //得到PLC的状态
    if(timeCount%3==0)
    {
        plcStatus=mMotusBasePlc.getPlcConnectStatus();
    }
    switch(currentMainCmd)
    {
        case Automatic:
        {
            //复位按键
            if(pathStep==3||pathStep==4||pathStep==5||pathStep==6||pathStep==7)
            {
               if(plcStatus&&resetHostIn.getStatus())
               {
                   for(int i=0;i<10;i++)
                   {
                      resetBit[i]=true;
                   }
                   errorMessage="";
                   pathStep=1;
                   playMovieCount=0;
               }
            }
            //故障信息处理
            if(pathStep==1||pathStep==2||pathStep==4||pathStep==5||pathStep==6||pathStep==8)
            {
                 if(plcStatus&&scramHostIn.getStatus())
                 {
                     pathStep=7;
                     errorMessage.sprintf("%d%s",0,"急停按键按下");
                     mMotusRunLog->addStringLog(errorMessage);
                 }
                 //驱动器错误
                 if(plcStatus&&errorHostIn.getStatus())
                 {
                     pathStep=7;
                     errorMessage.sprintf("%d%s",0,"驱动器故障");
                     mMotusRunLog->addStringLog(errorMessage);
                 }
                 break;
            }
            //初始默认值
            if(pathStep==0)
            {
                //可以更改手动
                emit sendHandPermissin(true);
                emit sendPlay(false);
            }
            //自检处理  关闭所有输出
            if(pathStep==1&&plcStatus&&carDoor1.getStatus()&&carDoor2.getStatus())
            {
                static int steppath1=0;
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[1]){  steppath1=0;  resetBit[1]=false; }
                if(mMotusOperaterStatus.playbit)
                {
                    //const char VideoStopCmd[6] = {0x53,0x54,0x4F,0x50,0x0D,0x0A};
                    //mQTcpSocket.write(VideoStopCmd,6);
                    char VideoStopCmd[5] = "stop";
                    mMotusPlayer.sendData(VideoStopCmd,4);
                    mMotusOperaterStatus.playbit=false;
                    playMovieCount=0;
                }
                //////////////////////////主PLC程序控制//////////////////////
                if(!timeOpen)
                {
                    stepMessage.sprintf("%d%s",0,"系统需要更新!!!");
                    break;
                }
                //熄灭故障指示灯
                if(steppath1==0&&faultHostOut.setControl(false))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"熄灭复位指示灯");
                    break;
                }
                else if(steppath1==0&&faultHostOut.getStatus())
                {
                    stepMessage.sprintf("%d%s",steppath1,"熄灭故障指示灯");
                    break;
                }
                //熄灭复位指示灯
                if(steppath1==1&&resetHostOut.setControl(false))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"熄灭待客指示灯");
                    break;
                }
                //熄灭待客指示灯
                if(steppath1==2&&runHostOut.setControl(false))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"车内照明开");
                    break;
                }
                //车内照明开
                if(steppath1==3&&lightHostOut.setControl(true))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"风扇关");
                    break;
                }
                //风扇关
                if(steppath1==4&&fanHostOut.setControl(false))
                {
                    steppath1=0;
                    pathStep=2;
                    stepMessage.sprintf("%d%s",steppath1,"平台回底");
                    break;
                }
            }
            else if(pathStep==1&&
                    (!plcStatus||!carDoor1.getStatus()||!carDoor2.getStatus()))
            {
                pathStep=7;
                if(!plcStatus)
                {
                    errorMessage.sprintf("%d%s",1,"PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                }
                if(!carDoor1.getStatus())
                {
                    errorMessage.sprintf("%d%s",1,"车门1未关闭");
                    mMotusRunLog->addStringLog(errorMessage);
                }
                if(!carDoor2.getStatus())
                {
                    errorMessage.sprintf("%d%s",1,"车门2未关闭");
                    mMotusRunLog->addStringLog(errorMessage);
                }
                break;
            }
            //准备处理
            if(pathStep==2)
            {
                static int steppath2=0;
                emit sendHandPermissin(false);            //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[2]) { steppath2=0;  resetBit[2]=false; }
                //平台回底
                if(steppath2==0)
                {
                    //平台是否连接 plc是否连接 平台是否可用
                    if(mMotusPlatfrom.getConnect()&&plcStatus&&carDoor1.getStatus()&&carDoor2.getStatus())
                    {
                        //底位限位亮 目的
                        if((platfromStatus==1||platfromStatus==13)&&platfromLow.getStatus())
                        {
                            steppath2 += 1;
                            stepMessage.sprintf("%d%s",steppath2,"复位指示灯点亮");
                            break;
                        }
                        //平台中位回底
                        if(platfromStatus==3||platfromStatus==15) mMotusPlatfrom.sendPlatfromCmd(7);
                        //平台顶位回中
                        if(platfromStatus==11) mMotusPlatfrom.sendPlatfromCmd(7);
                        //平台运动回底
                        if(platfromStatus==4) mMotusPlatfrom.sendPlatfromCmd(7);
                        //平台初始化寻低
                        if(platfromStatus==55) mMotusPlatfrom.sendPlatfromCmd(4);
                    }
                    else if(!mMotusPlatfrom.getConnect()||!plcStatus||!carDoor1.getStatus()||!carDoor2.getStatus())
                    {
                        pathStep=7;
                        if(!mMotusPlatfrom.getConnect())
                        {
                            errorMessage.sprintf("%d%s",2,"平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!plcStatus)
                        {
                            errorMessage.sprintf("%d%s",2,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!carDoor1.getStatus())
                        {
                            errorMessage.sprintf("%d%s",2,"车门1未关闭");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!carDoor2.getStatus())
                        {
                            errorMessage.sprintf("%d%s",2,"车门2未关闭");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        break;
                    }
                }
                //点亮复位指示灯
                if(steppath2==1&&resetHostOut.setControl(true))
                {
                    steppath2+=1;
                    stepMessage.sprintf("%d%s",steppath2,"点亮待客指示灯");
                    break;
                }
                else if(steppath2==1&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",2,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //点亮待客指示灯
                if(steppath2==2&&runHostOut.setControl(true))
                {
                    steppath2=0;
                    pathStep=3;
                    stepMessage.sprintf("%d%s",steppath2,"按下确认按键");
                    break;
                }
                else if(steppath2==2&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",2,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
            }
            //待客处理
            if(pathStep==3)
            {
                bool retBool=false;   //返回结果
                emit sendHandPermissin(true); //允许手动操作
                emit sendPlay(false);
                if(!safetyBeltAble)
                   retBool=true;
                else
                    retBool=safetyBelt1.getStatus()&&safetyBelt2.getStatus()&&safetyBelt3.getStatus()
                            &&safetyBelt4.getStatus()&&safetyBelt5.getStatus()&&safetyBelt6.getStatus()
                            &&safetyBelt7.getStatus()&&safetyBelt8.getStatus()&&safetyBelt9.getStatus();
                if(plcStatus&&runHostIn.getStatus()&&retBool&&carDoor1.getStatus()&&carDoor2.getStatus())
                {
                    pathStep=4;
                    stepMessage.sprintf("%d%s",0,"待客指示灯熄灭");
                    break;
                }
                else if(plcStatus&&runHostIn.getStatus()&&(!retBool||!carDoor1.getStatus()||!carDoor2.getStatus()))
                {
                    if(!retBool)
                    {
                        errorMessage.sprintf("%d%s",3,"安全带未系好");
                    }
                    if(!carDoor1.getStatus())
                    {
                        errorMessage.sprintf("%d%s",3,"车门1未关闭");
                    }
                    if(!carDoor2.getStatus())
                    {
                        errorMessage.sprintf("%d%s",3,"车门2未关闭");
                    }
                    break;
                }
            }
            //运动准备
            if(pathStep==4)
            {
                static int steppath4=0;
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[4]){  steppath4=0;  resetBit[4]=false; }                
                //熄灭待客指示灯
                if(steppath4==0&&plcStatus&&runHostOut.setControl(false))
                {
                    steppath4+=1;
                    stepMessage.sprintf("%d%s",steppath4,"影片载入");
                    break;
                }
                else if(steppath4==0&&plcStatus&&runHostOut.getStatus())
                {
                    stepMessage.sprintf("%d%s",steppath4,"熄灭待客指示灯");
                    break;
                }
                else if(steppath4==0&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",4,"PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //影片载入
                if(steppath4==1)
                {
                    //LOAD 001.XML
                    char mdata[4] = {'l','o','a','d'};
                    //int  sque=movieSque.toInt();
                    //mdata[5]=0x30+sque/100;
                    //mdata[6]=0x30+(sque%100)/10;
                    //mdata[7]=0x30+sque%10;
                    //mQTcpSocket.write(mdata,14);
                    mMotusPlayer.sendData(mdata,4);
                    steppath4+=1;
                    stepMessage.sprintf("%d%s",steppath4,"平台回中");
                    break;
                }
                //平台运动中位
                if(steppath4==2)
                {
                    //平台是否连接 plc是否连接 平台是否可用
                    if(mMotusPlatfrom.getConnect()&&plcStatus&&carDoor1.getStatus()&&carDoor2.getStatus())
                    {
                        //平台回中位 护栏下降
                        if(platfromStatus==3||platfromStatus==15)
                        {
                            steppath4+=1;
                            stepMessage.sprintf("%d%s",steppath4,"车内风扇关");
                            break;
                        }
                        //平台回中
                        if(platfromStatus==1||platfromStatus==13)
                            mMotusPlatfrom.sendPlatfromCmd(6);

                    }
                    else if(!mMotusPlatfrom.getConnect()||!plcStatus||!carDoor1.getStatus()||!carDoor2.getStatus())
                    {
                        pathStep=7;
                        if(!mMotusPlatfrom.getConnect())
                        {
                            errorMessage.sprintf("%d%s",4,"平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!plcStatus)
                        {
                            errorMessage.sprintf("%d%s",4,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!carDoor1.getStatus())
                        {
                            errorMessage.sprintf("%d%s",4,"车门1未关闭");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!carDoor2.getStatus())
                        {
                            errorMessage.sprintf("%d%s",4,"车门2未关闭");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        break;
                    }
                }          
                //风扇开
                if(steppath4==3&&fanHostOut.setControl(true))
                {
                    steppath4 += 1;
                    stepMessage.sprintf("%d%s",steppath4,"车内照明关");
                    break;
                }
                else if(steppath4==3&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",4,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //车内照明关
                if(steppath4==4&&lightHostOut.setControl(false))
                {
                    steppath4+=1;
                    stepMessage.sprintf("%d%s",steppath4,"影片播放信号发送");
                    break;
                }
                else if(steppath4==4&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",4,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //影片播放信号发送
                if(steppath4==5)
                {
                    //const char VideoPlayCmd[6] = {0x50,0x4C,0x41,0x59,0x0D,0x0A};
                    //mQTcpSocket.write(VideoPlayCmd,6);
                    char VideoPlayCmd[4] = {'p','l','a','y'};
                    mMotusPlayer.sendData(VideoPlayCmd,4);
                    stepMessage.sprintf("%d%s",steppath4,"等待视频播放");
                    steppath4 += 1;
                    break;
                }
                //等待视频播放
                if(steppath4==6)
                {
                    QString str=mMotusPlayer.getData();
                    if(str.compare("playing")==0)
                    {
                        mMotusOperaterStatus.playbit=true;
                        steppath4=0;
                        stepMessage.sprintf("%d%s",steppath4,"平台运动");
                        pathStep=5;
                    }
                    break;
                }

            }
            //平台运动
            if(pathStep==5)
            {
                static int steppath5=0;         //步骤标志位
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(true);
                //复位初始化
                if(resetBit[5]){  steppath5=0;  resetBit[5]=false; }
                //平台运动
                if(steppath5==0)
                {
                    float data[6]={0.f,0.f,0.f,0.f,0.f,0.f};
                    //平台是否连接 plc是否连接 平台是否可用
                    if(mMotusPlatfrom.getConnect()&&plcStatus&&carDoor1.getStatus()&&carDoor2.getStatus())
                    {
                        //////////////////////////////////////////////////////////////////////////////////////
                        //平台回中位 护栏下降
                        if(platfromStatus==3||platfromStatus==15||platfromStatus==4)
                        {
                            if(playMovieCount<playMovieData.size())
                            {
                               data[0]=playMovieData[playMovieCount].atu1;
                               data[1]=playMovieData[playMovieCount].atu2;
                               data[2]=playMovieData[playMovieCount].atu3;
                               data[3]=playMovieData[playMovieCount].atu4;
                               data[4]=playMovieData[playMovieCount].atu5;
                               data[5]=playMovieData[playMovieCount].atu6;
                               mMotusPlatfrom.sendPlatfromAttu(data);
                            }
                        }
                        ////////////////////////////////////////////////////////////////////////////////////////
                    }
                    else if(!mMotusPlatfrom.getConnect()||!plcStatus||!carDoor1.getStatus()||!carDoor2.getStatus())
                    {
                        pathStep=7;
                        if(!mMotusPlatfrom.getConnect())
                        {
                            errorMessage.sprintf("%d%s",5,"平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!plcStatus)
                        {
                            errorMessage.sprintf("%d%s",5,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!carDoor1.getStatus())
                        {
                            errorMessage.sprintf("%d%s",5,"车门1未关闭");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!carDoor2.getStatus())
                        {
                            errorMessage.sprintf("%d%s",5,"车门2未关闭");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        break;
                    }
                    playMovieCount++;
                    if(playMovieCount>=playMovieData.size())
                    {
                        playMovieCount=0;
                        emit sendPlay(false);
                        mMotusOperaterStatus.playbit=false;
                        steppath5+=1;
                        stepMessage.sprintf("%d%s",steppath5,"风扇关");
                    }
                    break;
                }
                //风扇关
                if(steppath5==1&&plcStatus&&fanHostOut.setControl(false))
                {
                    steppath5=0;
                    pathStep=6;
                    stepMessage.sprintf("%d%s",steppath5,"平台回底");
                    break;
                }
                else if(steppath5==1&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",5,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
            }
            //恢复处理
            if(pathStep==6)
            {
                static int steppath6=0;                   //步骤标志位
                emit sendHandPermissin(false);            //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[6]){  steppath6=0;  resetBit[6]=false; }
                //平台回底
                if(steppath6==0)
                {
                    //平台是否连接 plc是否连接 平台是否可用
                    if(mMotusPlatfrom.getConnect()&&plcStatus&&carDoor1.getStatus()&&carDoor2.getStatus())
                    {
                        //平台回中位
                        if(platfromStatus==3||platfromStatus==15||platfromStatus==4)
                           mMotusPlatfrom.sendPlatfromCmd(7);
                        //平台回中
                        if(platfromStatus==1||platfromStatus==13)
                        {
                            steppath6+=1;
                            stepMessage.sprintf("%d%s",steppath6,"车内照明开");
                            break;
                        }

                    }
                    else if(!mMotusPlatfrom.getConnect()||!plcStatus||!carDoor1.getStatus()||!carDoor2.getStatus())
                    {
                        pathStep=7;
                        if(!mMotusPlatfrom.getConnect())
                        {
                            errorMessage.sprintf("%d%s",6,"平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!plcStatus)
                        {
                            errorMessage.sprintf("%d%s",6,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!carDoor1.getStatus())
                        {
                            errorMessage.sprintf("%d%s",6,"车门1未关闭");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        if(!carDoor2.getStatus())
                        {
                            errorMessage.sprintf("%d%s",6,"车门2未关闭");
                            mMotusRunLog->addStringLog(errorMessage);
                        }
                        break;
                    }
                }
                //车内照明开
                if(steppath6==1&&plcStatus&&lightHostOut.setControl(true))
                {
                    steppath6+=1;
                    stepMessage.sprintf("%d%s",steppath6,"待客指示灯点亮");
                    break;
                }
                else if(steppath6==1&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",6,"PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //待客指示灯点亮
                if(steppath6==2&&plcStatus&&runHostOut.setControl(true))
                {
                    steppath6=0;
                    pathStep=3;
                    stepMessage.sprintf("%d%s",steppath6,"按下确认按键");
                    break;
                }
                else if(steppath6==2&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",6,"PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
            }
            //故障处理
            if(pathStep==7)
            {
                static int steppath7=0;
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[7]){steppath7=0;resetBit[7]=false;}
                if(mMotusOperaterStatus.playbit)
                {
                    //const char VideoStopCmd[6] = {0x53,0x54,0x4F,0x50,0x0D ,0x0A };
                    //mQTcpSocket.write(VideoStopCmd,6);
                    char VideoStopCmd[4] = {'s','t','o','p'};
                    mMotusPlayer.sendData(VideoStopCmd,4);
                    //mQTcpSocket.write(VideoStopCmd,6);
                    mMotusOperaterStatus.playbit=false;
                    playMovieCount=0;
                }
                //////////////////////////主PLC程序控制//////////////////////
                //点亮故障指示灯
                if(steppath7==0&&plcStatus&&faultHostOut.setControl(true))
                {                    
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭复位指示灯");
                    break;
                }
                else if(steppath7==0&&plcStatus&&!faultHostOut.getStatus())
                {
                    stepMessage.sprintf("%d%s",steppath7,"点亮故障指示灯");
                    break;
                }
                else if(steppath7==0&&!plcStatus)
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭复位指示灯");
                    break;
                }
                //熄灭复位指示灯
                if(steppath7==1&&plcStatus&&resetHostOut.setControl(false))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭待客指示灯");
                    break;
                }
                else if(steppath7==1&&!plcStatus)
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭待客指示灯");
                    break;
                }
                //熄灭待客指示灯
                if(steppath7==2&&plcStatus&&runHostOut.setControl(false))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"车内照明开");
                    break;
                }
                else if(steppath7==2&&!plcStatus)
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"车内照明开");
                    break;
                }
                //车内照明开
                if(steppath7==3&&plcStatus&&lightHostOut.setControl(true))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"车内风扇关");
                    break;
                }
                else if(steppath7==3&&!plcStatus)
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"车内风扇关");
                    break;
                }
                //风扇关
                if(steppath7==4&&plcStatus&&fanHostOut.setControl(false))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"车内风扇关");
                    break;
                }
                else if(steppath7==4&&!plcStatus)
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"车内风扇关");
                    break;
                }
            }
            //关机处理
            if(pathStep==8)
            {
                static int steppath8=0;
                emit sendHandPermissin(false);            //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[8]){steppath8=0;resetBit[8]=false;}
                //////////////////////////主PLC程序控制//////////////////////
                //熄灭复位指示灯
                if(steppath8==0&&plcStatus&&resetHostOut.setControl(false))
                {
                    steppath8+=1;
                    stepMessage.sprintf("%d%s",steppath8,"熄灭待客指示灯");
                    break;
                }
                else if(steppath8==0&&plcStatus&&resetHostOut.getStatus())
                {
                    stepMessage.sprintf("%d%s",steppath8,"熄灭复位指示灯");
                    break;
                }
                else if(steppath8==0&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",8,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //熄灭待客指示灯
                if(steppath8==1&&plcStatus&&runHostOut.setControl(false))
                {
                    steppath8+=1;
                    stepMessage.sprintf("%d%s",steppath8,"照明关");
                    break;
                }
                else if(steppath8==1&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",8,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //照明关
                if(steppath8==2&&plcStatus&&lightHostOut.setControl(true))
                {
                    steppath8=0;
                    stepMessage.sprintf("%d%s",steppath8,"关机结束");
                    break;
                }
                else if(steppath8==2&&!plcStatus)
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",8,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }

            }
        }
        case HandMove:
        {
            switch (currentSonCmd)
            {
                //冷风特效
                case ColdWind:
                     {
                        if(plcStatus)
                        {
                           fanHostOut.setValue(actionValue);
                        }
                        break;
                     }
                //座椅照明
                case SeatLight:
                     {
                        if(plcStatus)
                        {
                           lightHostOut.setValue(actionValue);
                        }
                        break;
                     }
                //平台升
                case PlatfromUp:
                     {
                        if(mMotusPlatfrom.getConnect())
                        {
                            mMotusPlatfrom.sendPlatfromCmd(6);
                        }
                        break;
                     }
                //平台降
                case PlatfromDown:
                     {
                        if(mMotusPlatfrom.getConnect())
                        {
                            mMotusPlatfrom.sendPlatfromCmd(7);
                        }
                        break;
                     }
                default:
                    break;
            }
            break;
        }
    }
    if(timeCount%50==0)
    {
        switch(viewId)
        {
            case 0:
            {
              bool  valuedata[36]={false};
              bool status[100]={false};
              int lenght=0;
              if(mMotusBasePlc.getPlcIOStatus(status,lenght))
              {
                  valuedata[0]=status[13];
                  valuedata[1]=status[4];
                  valuedata[2]=status[5];
                  valuedata[3]=status[6];
                  valuedata[4]=status[14];
                  valuedata[5]=status[7];
                  valuedata[6]=status[8];
                  valuedata[7]=status[9];
                  valuedata[8]=status[10];
                  valuedata[9]=status[11];
                  valuedata[10]=status[12];
              }
              mMotusOperationView->setViewStatus(valuedata);
              mMotusOperationView->updateOperationStatus(pathStep, stepMessage,errorMessage, playMovieCount);
              break;
            }
            case 1:
            {
                bool status[100]={false};
                int lenght=0;
                mMotusaFlatformStatus->viewPlatfromStatus(platfromStatus);
                mMotusaFlatformStatus->viewPlcStatus(plcStatus);
                if(mMotusBasePlc.getPlcIOStatus(status,lenght))
                {
                    mMotusaFlatformStatus->viewMainPlcStatus(status,lenght);
                }
                break;
            }
        }
    }
}

void MainWindow::recvdata(QString strName, QString strPassword)
{
     QString  str;
     str=strName;
     str=strPassword;
}

//播放按键
void MainWindow::on_playButton_clicked()
{
    ui->m_stackwidget->setCurrentWidget(mMotusOperationView);
    viewId=0;
    setWindowTitle("播放主页");
}

//接收控制指令
void MainWindow::recvOperationCmd(int cmd)
{
    switch (cmd)
    {
        //开机指令
        case StartUp:
             {
                 if(pathStep==0)
                 {
                    pathStep=1;
                    emit setOperateViewText("关机");
                 }
                 break;
             }
        //关机指令
        case PowerOff:
             {
                 if(pathStep==3)
                 {
                   pathStep=8;
                   emit setOperateViewText("开机");
                 }
                 break;
             }
        //复位指令
        case Restoration:
             {
                 //复位操作
                 if(pathStep==3||pathStep==4||pathStep==5||pathStep==6||pathStep==7)
                 {
                   for(int i=0;i<9;i++)
                   {
                      resetBit[i]=true;
                   }
                   playMovieCount=0;
                   pathStep=1;
                   errorMessage="";
                 }
                 break;
             }
        //运行指令
        case Operation:
            {
                if(pathStep==3)
                {
                  pathStep=4;
                }
                break;
            }
        //默认
        default:
            break;
    }
}

//状态按键
void MainWindow::on_stateButton_clicked()
{
    ui->m_stackwidget->setCurrentWidget(mMotusaFlatformStatus);
    viewId=1;
    setWindowTitle("状态主页");
}


//手动按键
void MainWindow::on_handButton_clicked()
{
    ui->m_stackwidget->setCurrentWidget(mMotusHand);
    viewId=2;
    setWindowTitle("手动主页");
}

//安全带是否可用
void MainWindow::recvSafetyBeltAble(bool able)
{
    safetyBeltAble=able;
}

//接收主窗口的命令
void MainWindow::recvMainWindoewCmd(int cmd,int ma,bool ret)
{
    //qDebug()<<num<<cmd<<main;
    if(ma==0)
    {
        //主循环命令
        currentMainCmd=cmd;
    }
    else if(ma==1)
    {
        //当前子命令
        currentSonCmd=cmd;
        //输出
        actionValue=ret;
    }
}

//错误按键
void MainWindow::on_errorCheckButton_clicked()
{
    ui->m_stackwidget->setCurrentWidget(mMotusRunLog);
     viewId=3;
    setWindowTitle("记录主页");
}

//用户按键
void MainWindow::on_userButton_clicked()
{
    ui->m_stackwidget->setCurrentWidget(mMotusUserManagement);
    viewId=4;
    setWindowTitle("用户主页");
}

//关闭消息
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!shutDownBit)
    {
       event->ignore();
    }
}

//退出按键
void MainWindow::on_exitButton_clicked()
{
      QMessageBox::StandardButton reply;
      reply = QMessageBox::warning(this, "警告", "是否关机并关闭软件",QMessageBox::Yes | QMessageBox::No);
      if(reply==QMessageBox::Yes)
      {

      }
      else
      {
          return;
      }
      shutDownBit=true;
      this->close();
      HANDLE hToken;
      TOKEN_PRIVILEGES tkp;
      //获取进程标志
      if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
      return ;
      //获取关机特权的LUID
      LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
      tkp.PrivilegeCount = 1;
      tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      //获取这个进程的关机特权
      AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
      if (GetLastError() != ERROR_SUCCESS) return ;
      // 强制关闭计算机
      if( !ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
          return ;
}
