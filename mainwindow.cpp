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
    plcIndex=1;
    mMotusaFlatformStatus=new MotusaFlatformStatus(this);
    connect(mMotusaFlatformStatus,SIGNAL(platfromSwitch(int)),this,SLOT(recvPlatfromStatus(int)));
    //手动控制
    mMotusHand=new MotusHand(this);
    actionValue=false;
    connect(mMotusHand,SIGNAL(sendPlatfromAble(int,bool)),this,SLOT(recvPlatfromAble(int ,bool)));
    connect(mMotusHand,SIGNAL(sendMainWindoewCmd(int,int,int,bool)),this,SLOT(recvMainWindoewCmd(int,int,int,bool)));
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
    currentHandPlatfrom=0;
    playMovieCount=0;
    shutDownBit=false;
    for(int i=0;i<6;i++)
    {
        platfromAble[i]=true;
        platfromStatus[i]=255;
    }
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
    scramHostIn.initPara(&mMotusBasePlc,0);     //急停按键
    renewHostIn.initPara(&mMotusBasePlc,1);     //恢复按键
    resetHostIn.initPara(&mMotusBasePlc,2);     //复位按键
    continueHostIn.initPara(&mMotusBasePlc,3);  //继续按键
    airHighHostIn.initPara(&mMotusBasePlc,4);   //空压机气压高
    airLowHostIn.initPara(&mMotusBasePlc,5);    //空压机气压低
    faultHostOut.initPara(&mMotusBasePlc,6);    //故障指示灯
    renewHostOut.initPara(&mMotusBasePlc,7);    //恢复指示灯
    resetHostOut.initPara(&mMotusBasePlc,8);    //复位指示灯
    continueHostOut.initPara(&mMotusBasePlc,9); //继续指示灯
    waitHostOut.initPara(&mMotusBasePlc,10);     //待客指示灯
    smokeHostOut.initPara(&mMotusBasePlc,11);    //烟雾特效
    strobeHostOut.initPara(&mMotusBasePlc,12);   //频闪特效
    hubbleHostOut.initPara(&mMotusBasePlc,13);   //泡泡特效
    /////////////////////////////////////////////////////////////////////
    /////////////////////////从PLC输入输出////////////////////////////////////
    scramFromIn.initPara(&mMotusBasePlc,0);      //急停按键
    msureFormIn.initPara(&mMotusBasePlc,1);      //确认按键
    resetFromIn.initPara(&mMotusBasePlc,2);      //复位按键
    handFromIn.initPara(&mMotusBasePlc,3);       //手动按键
    pleverDownFromIn.initPara(&mMotusBasePlc,4); //压杆下限位
    pleverUpFromIn.initPara(&mMotusBasePlc,5);   //压杆上限位
    pleverLockFromIn.initPara(&mMotusBasePlc,6); //压杆锁限位
    mCarControl.initPara(&mMotusBasePlc);        //小车控制
    freChangeFaultFromIn.initPara(&mMotusBasePlc,16); //变频器故障
    driverFaultFromIn.initPara(&mMotusBasePlc,17); //驱动器故障
    cylinderFromIn.initPara(&mMotusBasePlc,20);    //电动缸信号
    saftBelt1FromIn.initPara(&mMotusBasePlc,21);   //1#安全带
    saftBelt2FromIn.initPara(&mMotusBasePlc,22);   //2#安全带
    saftBelt3FromIn.initPara(&mMotusBasePlc,23);   //3#安全带
    saftBelt4FromIn.initPara(&mMotusBasePlc,24);   //4#安全带
    railUpFromIn.initPara(&mMotusBasePlc,7);       //护栏上限位
    railDownFromIn.initPara(&mMotusBasePlc,8);     //护栏下限位
    railReverFromIn.initPara(&mMotusBasePlc,25);   //护栏翻转
    faultFromOut.initPara(&mMotusBasePlc,29);     //故障指示灯
    mSureFromOut.initPara(&mMotusBasePlc,30);     //确认指示灯
    resetFromOut.initPara(&mMotusBasePlc,31);     //复位指示灯
    handFromOut.initPara(&mMotusBasePlc,32);      //手动指示灯
    waitFromOut.initPara(&mMotusBasePlc,33);      //待客指示灯

    pleverLockFromOut.initPara(&mMotusBasePlc,36);  //压杆锁
    railUpFromOut.initPara(&mMotusBasePlc,37);      //护栏上输出
    railDownFromOut.initPara(&mMotusBasePlc,38);    //护栏下输出
    coolWindFromOut.initPara(&mMotusBasePlc,45);     //冷风特效
    waterSprayFromOut.initPara(&mMotusBasePlc,46);   //喷水特效
    seatLightFromOut.initPara(&mMotusBasePlc,47);    //座椅照明
    /////////////////////////////////////////////////////////////////////
    //int width=QApplication::desktop()->width();
    //int height=QApplication::desktop()->height();
    //this->setMinimumSize(width,height);
    //setWindowFlags (Qt::FramelessWindowHint);

    //播放界面切换
    on_playButton_clicked();

    ui->centralWidget->setLayout(ui->mainGridLayout);

    mQTcpSocket.connectToHost("192.168.1.7",23);
    if (!mQTcpSocket.waitForConnected(1000))
    QMessageBox::information(this,"友情提示","未连接到视频播放器");

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
    static bool plcStatus[7];
    timeCount++;
    mMotusPlatfrom.getPlatfromStatus(platfromStatus);
    //得到PLC的状态
    if(timeCount%3==0)
    {
        for(int i=0;i<7;i++)
        {
            plcStatus[i]=mMotusBasePlc.getPlcConnectStatus(i);
        }
    }
    switch(currentMainCmd)
    {
        case Automatic:
        {
            //复位按键
            if(pathStep==3||pathStep==4||pathStep==5||pathStep==6||pathStep==7)
            {
               if(plcStatus[0]&&resetHostIn.getStatus(0))
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
                 int i=0;
                 if(plcStatus[0]&&scramHostIn.getStatus(0))
                 {
                     pathStep=7;
                     errorMessage.sprintf("%d%s",0,"急停按键按下");
                     mMotusRunLog->addStringLog(errorMessage);
                 }
                 //平台可以后退   ----
                 for(i=0;i<6;i++)
                 {
                     mMotusOperaterStatus.platerror[i]=false;
                     if(platfromAble[i]&&(platfromStatus[i]==119))
                     {
                         if(pathStep==5||pathStep==6)
                         {
                             mMotusOperaterStatus.platerror[i]=true;
                             errorMessage.sprintf("%d%s",i+1,"平台驱动器错误");
                         }
                         else
                         {
                             pathStep=7;
                             errorMessage.sprintf("%d%s",i+1,"平台驱动器错误");
                             mMotusRunLog->addStringLog(errorMessage);
                             break;
                         }
                     }
                 }
                 //驱动器错误     ----
                 for(i=0;i<6;i++)
                 {
                     if(platfromAble[i]&&plcStatus[i+1]&&driverFaultFromIn.getStatus(i+1))
                     {
                         if(pathStep==5||pathStep==6)
                         {
                             mMotusOperaterStatus.platerror[i]=true;
                             if(driverFaultFromIn.getStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"驱动器故障");
                         }
                         else
                         {
                             pathStep=7;
                             if(driverFaultFromIn.getStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"驱动器故障");
                             mMotusRunLog->addStringLog(errorMessage);
                             break;
                         }
                     }
                 }
                 //变频器故障     ----
                 for(i=0;i<6;i++)
                 {
                     mMotusOperaterStatus.freerror[i]=false;
                     if(platfromAble[i]&&plcStatus[i+1]
                       &&(freChangeFaultFromIn.getStatus(i+1)||scramFromIn.getStatus(i+1)))
                     {
                         if(pathStep==5||pathStep==6)
                         {
                             mMotusOperaterStatus.freerror[i]=true;
                             if(freChangeFaultFromIn.getStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"变频器故障");
                             if(scramFromIn.getStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"急停按下");
                         }
                         else
                         {
                             pathStep=7;
                             if(freChangeFaultFromIn.getStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"变频器故障");
                             mMotusRunLog->addStringLog(errorMessage);
                             if(scramFromIn.getStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"急停按下");
                             mMotusRunLog->addStringLog(errorMessage);
                             break;
                         }
                     }
                 }
            }
            //初始默认值
            if(pathStep==0)
            {
                //可以更改手动
                emit sendHandPermissin(true);
                emit sendPlay(false);
            }
            //自检处理  关闭所有输出
            if(pathStep==1&&plcStatus[0])
            {
                static int steppath1=0;
                int i=0;                  //循环计数
                bool retBool=false;       //返回结果
                int retCount=0;           //结果个数
                int totalCount=0;         //统计个数
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[1]){  steppath1=0;  resetBit[1]=false; }
                if(mMotusOperaterStatus.playbit)
                {
                    const char VideoStopCmd[6] = {0x53,0x54,0x4F,0x50,0x0D,0x0A};
                    mQTcpSocket.write(VideoStopCmd,6);
                    mMotusOperaterStatus.playbit=false;
                    playMovieCount=0;
                }
                //////////////////////////主PLC程序控制//////////////////////
                if(airLowHostIn.getStatus(0))
                {
                    stepMessage.sprintf("%d%s",0,"气压不足请检查气压设备");
                    break;
                }
                if(!timeOpen)
                {
                    stepMessage.sprintf("%d%s",0,"系统需要更新!!!");
                    break;
                }
                //熄灭故障指示灯
                if(steppath1==0&&faultHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"熄灭恢复指示灯");
                    break;
                }
                else if(steppath1==0&&faultHostOut.getStatus(0))
                {
                    stepMessage.sprintf("%d%s",steppath1,"熄灭故障指示灯");
                    break;
                }
                //熄灭恢复指示灯
                if(steppath1==1&&renewHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"熄灭复位指示灯");
                    break;
                }
                //熄灭复位指示灯
                if(steppath1==2&&resetHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"熄灭继续指示灯");
                    break;
                }
                //熄灭继续指示灯
                if(steppath1==3&&continueHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"熄灭待客指示灯");
                    break;
                }
                //熄灭待客指示灯
                if(steppath1==4&&waitHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"烟雾特效关");
                    break;
                }
                //烟雾特效关
                if(steppath1==5&&smokeHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"频闪特效关");
                    break;
                }
                //频闪特效关
                if(steppath1==6&&strobeHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"泡泡特效关");
                    break;
                }
                //泡泡特效关
                if(steppath1==7&&hubbleHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"熄灭故障指示灯");
                    break;
                }
                //////////////////////////主PLC控制结束//////////////////////////
                //////////////////////////从PLC控制/////////////////////////////
                //熄灭故障指示灯
                if(steppath1==8)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=faultFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath1+=1;
                        stepMessage.sprintf("%d%s",steppath1,"熄灭确认指示灯");
                    }
                    break;
                }
                //熄灭确认指示灯
                if(steppath1==9)
                {
                    retCount=0;  totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=mSureFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath1+=1;
                        stepMessage.sprintf("%d%s",steppath1,"熄灭待客指示灯");
                    }
                    break;
                }
                //熄灭待客指示灯
                if(steppath1==10)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=waitFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath1+=1;
                        stepMessage.sprintf("%d%s",steppath1,"冷风特效关");
                    }
                    break;
                }
                //冷风特效关
                if(steppath1==11)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=coolWindFromOut.setControl(false,i+1);
                            if(retBool)  retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath1+=1;
                        stepMessage.sprintf("%d%s",steppath1,"喷水特效关");
                    }
                    break;
                }
                //喷水特效关
                if(steppath1==12)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=waterSprayFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath1+=1;
                        stepMessage.sprintf("%d%s",steppath1,"护栏上输出关");
                    }
                    break;
                }
                //护栏上输出关
                if(steppath1==13)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=railUpFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath1+=1;
                        stepMessage.sprintf("%d%s",steppath1,"护栏下输出关");
                    }
                    break;
                }
                //护栏下输出关
                if(steppath1==14)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=railDownFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath1+=1;
                        stepMessage.sprintf("%d%s",steppath1,"小车输出关");
                    }
                    break;
                }
                //小车输出关
                if(steppath1==15)
                {
                    retCount=0;totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=mCarControl.clearOutput(i+1);
                            if(retBool)retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        stepMessage.sprintf("%d%s",steppath1,"平台回底");
                        steppath1=0;
                        pathStep=2;
                    }
                    break;
                }
                //////////////////////////从PLC控制/////////////////////////////
            }
            else if(pathStep==1&&!plcStatus[0])
            {
                pathStep=7;
                errorMessage.sprintf("%d%s",0,"号PLC通讯异常");
                mMotusRunLog->addStringLog(errorMessage);
                break;
            }
            //准备处理
            if(pathStep==2)
            {
                static int steppath2=0;
                int i=0;                                  //循环计数
                bool retBool=false;                       //返回结果
                int retCount=0;                           //结果个数
                int totalCount=0;                         //统计个数
                emit sendHandPermissin(false);            //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[2]) { steppath2=0;  resetBit[2]=false; }
                //平台回底
                if(steppath2==0)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //底位限位亮 目的
                            if((platfromStatus[i]==1||platfromStatus[i]==13)&&cylinderFromIn.getStatus(i+1))  retBool=true;
                            //平台中位回底
                            if(platfromStatus[i]==3||platfromStatus[i]==15) mMotusPlatfrom.sendPlatfromCmd(7,i);
                            //平台顶位回中
                            if(platfromStatus[i]==11) mMotusPlatfrom.sendPlatfromCmd(7,i);
                            //平台运动回底
                            if(platfromStatus[i]==4) mMotusPlatfrom.sendPlatfromCmd(7,i);
                            //平台初始化寻低
                            if(platfromStatus[i]==55) mMotusPlatfrom.sendPlatfromCmd(4,i);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"号平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"护栏下降");
                    }
                    break;
                }
                //护栏下降
                if(steppath2==1)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //后退停止位
                            if(!mCarControl.getBackStopStatus(i+1))
                            {
                                //护栏降完成
                                if(railDownFromIn.getStatus(i+1)&&!railDownFromOut.getStatus(i+1)) retBool=true;
                                //护栏降
                                if(!railDownFromIn.getStatus(i+1))  railDownFromOut.setControl(true,i+1);
                                //护栏降输出关闭
                                if(railDownFromIn.getStatus(i+1)&&railDownFromOut.getStatus(i+1)) railDownFromOut.setControl(false,i+1);
                            }
                            else
                            {
                                retBool=true;
                            }
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"平台后退");
                    }
                    break;
                }
                //平台后退
                if(steppath2==2)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //小车触碰到极限位 进行报错处理
                            if(mCarControl.getLimitStatus(i+1))
                            {
                                pathStep=7;
                                errorMessage.sprintf("%d%s",i+1,"小车触碰到极限位");
                                mMotusRunLog->addStringLog(errorMessage);
                                break;
                            }                            
                            //判断小车是否到位
                            if(mCarControl.getBackStopStatus(i+1)
                               &&!mCarControl.getFrontControl(i+1)&&!mCarControl.getBackControl(i+1)
                               &&!mCarControl.getHighSpeedControl(i+1)&&!mCarControl.getLowSpeedControl(i+1)
                               &&mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&!mCarControl.getGasBrakeCloseControl(i+1))   retBool=true;
                            //小车后退  气刹关关闭
                            if(!mCarControl.getBackStopStatus(i+1)&&mCarControl.getGasBrakeCloseControl(i+1))
                            {
                                mCarControl.setGasBrakeCloseControl(false,i+1);
                                continue;
                            }
                            //小车后退  气刹开打开
                            if(!mCarControl.getBackStopStatus(i+1)&&!mCarControl.getGasBrakeControl(i+1))
                            {
                                mCarControl.setGasBrakeControl(true,i+1);
                                continue;
                            }
                            //小车后退  高速关闭
                            if(!mCarControl.getBackStopStatus(i+1)&&mCarControl.getHighSpeedControl(i+1))
                            {
                                mCarControl.setHighSpeedControl(false,i+1);
                                continue;
                            }
                            //小车后退 前进关闭
                            if(!mCarControl.getBackStopStatus(i+1)&&mCarControl.getFrontControl(i+1))
                            {
                                mCarControl.setFrontControl(false,i+1);
                                continue;
                            }
                            //小车后退 后退打开
                            if(!mCarControl.getBackStopStatus(i+1)&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getBackControl(i+1))
                            {
                                mCarControl.setBackControl(true,i+1);
                                continue;
                            }
                            //小车后退 低速打开
                            if(!mCarControl.getBackStopStatus(i+1)&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getLowSpeedControl(i+1))
                            {
                                mCarControl.setLowSpeedControl(true,i+1);
                                continue;
                            }
                            //清除速度
                            if(mCarControl.getBackStopStatus(i+1))
                            {
                               mCarControl.clearOutput(i+1);                              
                            }
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"护栏上升");
                    }
                    break;
                }
                //护栏上升
                if(steppath2==3)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            if(!railUpFromIn.getStatus(i+1)||!railReverFromIn.getStatus(i+1))
                               railUpFromOut.setControl(true,i+1);
                            if(railUpFromIn.getStatus(i+1)&&railReverFromIn.getStatus(i+1)&&railUpFromOut.getStatus(i+1))
                               railUpFromOut.setControl(false,i+1);
                            if(railUpFromIn.getStatus(i+1)&&railReverFromIn.getStatus(i+1)&&!railUpFromOut.getStatus(i+1))
                               retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"平台运动到顶");
                    }
                    break;
                }
                //平台运动到顶
                if(steppath2==4)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //平台回顶位
                            if(platfromStatus[i]==11&&!cylinderFromIn.getStatus(i+1))  retBool=true;
                            //平台回中位
                            if((platfromStatus[i]==1||platfromStatus[i]==13)&&cylinderFromIn.getStatus(i+1))
                            {  mMotusPlatfrom.sendPlatfromCmd(6,i); }
                            else if((platfromStatus[i]==1||platfromStatus[i]==13)&&!cylinderFromIn.getStatus(i+1))
                            {
                                pathStep=7;
                                errorMessage.sprintf("%d%s",i+1,"号平台出错");
                                mMotusRunLog->addStringLog(errorMessage);
                                break;
                            }
                            //平台中位到顶
                            if((platfromStatus[i]==3||platfromStatus[i]==15)&&!cylinderFromIn.getStatus(i+1))
                            { mMotusPlatfrom.sendPlatfromCmd(12,i);}
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"号平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"座位照明开");
                    }
                    break;
                }
                //座位照明开
                if(steppath2==5)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //座位照明开
                            if(seatLightFromOut.setControl(true,i+1))  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"主待客指示灯点亮");
                    }
                    break;
                }
                //主待客指示灯点亮
                if(steppath2==6&&plcStatus[0]&&waitHostOut.setControl(true,0))
                {
                    steppath2+=1;
                    stepMessage.sprintf("%d%s",steppath2,"从待客指示灯点亮");
                    break;
                }
                else if(steppath2==6&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //从待客指示灯点亮
                if(steppath2==7)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        mMotusOperaterStatus.makesure[i]=false;
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //从待客指示灯点亮
                            if(waitFromOut.getStatus(i+1))  retBool=true;
                            //从待客指示灯点亮
                            if(!waitFromOut.getStatus(i+1)) waitFromOut.setValue(true,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"可以上客和下客");
                    }
                    break;
                }
                //压下压杆后，确认指示灯亮后，按下确认按键
                if(steppath2==8)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //初始化确认数组参数
                            if(!pleverDownFromIn.getStatus(i+1))
                                mMotusOperaterStatus.makesure[i]=false;
                            //确认指示灯点亮
                            if(pleverDownFromIn.getStatus(i+1)&&!mSureFromOut.getStatus(i+1)&&!mMotusOperaterStatus.makesure[i])
                             mSureFromOut.setValue(true,i+1);
                            //标志位置true
                            if(pleverDownFromIn.getStatus(i+1)
                              &&saftBelt1FromIn.getStatus(i+1)
                              &&saftBelt2FromIn.getStatus(i+1)
                              &&saftBelt3FromIn.getStatus(i+1)
                              &&saftBelt4FromIn.getStatus(i+1)
                              &&msureFormIn.getStatus(i+1)
                              &&mSureFromOut.getStatus(i+1)
                              &&!mMotusOperaterStatus.makesure[i])
                             mMotusOperaterStatus.makesure[i]=true;
                             //熄灭确认指示灯
                            if(pleverDownFromIn.getStatus(i+1)&&mSureFromOut.getStatus(i+1)&&mMotusOperaterStatus.makesure[i])
                             mSureFromOut.setValue(false,i+1);
                            //运行按键
                            if(pleverDownFromIn.getStatus(i+1)
                                &&saftBelt1FromIn.getStatus(i+1)
                                &&saftBelt2FromIn.getStatus(i+1)
                                &&saftBelt3FromIn.getStatus(i+1)
                                &&saftBelt4FromIn.getStatus(i+1)
                               &&!mSureFromOut.getStatus(i+1)
                               &&mMotusOperaterStatus.makesure[i])
                            retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2=0;
                        pathStep=3;
                        stepMessage.sprintf("%d%s",steppath2,"待客状态");
                    }
                    break;
                }
            }
            //待客处理
            if(pathStep==3)
            {
                emit sendHandPermissin(true); //不允许手动操作
                emit sendPlay(false);
                if(airLowHostIn.getStatus(0))
                {
                    stepMessage.sprintf("%d%s",0,"气压不足请检查气压设备");
                    break;
                }
                else
                {
                    stepMessage.sprintf("%d%s",0,"待客状态");
                }
                if(!resetHostOut.getStatus(0))
                {
                    resetHostOut.setValue(true,0);
                    break;
                }
                if(!renewHostOut.getStatus(0))
                {
                    renewHostOut.setValue(true,0);
                    break;
                }
                if(renewHostIn.getStatus(0))
                {
                    pathStep=4;
                }
            }
            //运动准备
            if(pathStep==4)
            {
                static int steppath4=0;
                int i=0;                                  //循环计数
                bool retBool=false;                       //返回结果
                int retCount=0;                           //结果个数
                int totalCount=0;                         //统计个数
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[4]){  steppath4=0;  resetBit[4]=false; }                
                //主待客指示灯熄灭
                if(steppath4==0&&plcStatus[0]&&waitHostOut.setControl(false,0))
                {
                    steppath4+=1;
                    stepMessage.sprintf("%d%s",steppath4,"运行指示灯熄灭");
                    break;
                }
                else if(steppath4==0&&plcStatus[0]&&waitHostOut.getStatus(0))
                {
                    stepMessage.sprintf("%d%s",steppath4,"主待客指示灯熄灭");
                    break;
                }
                else if(steppath4==0&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //运行指示灯熄灭
                if(steppath4==1&&plcStatus[0]&&renewHostOut.setControl(false,0))
                {
                    steppath4+=1;
                    stepMessage.sprintf("%d%s",steppath4,"从待客指示灯熄灭");
                    break;
                }
                else if(steppath4==1&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //从待客指示灯熄灭
                if(steppath4==2)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //从待客指示灯熄灭
                            if(!waitFromOut.getStatus(i+1))  retBool=true;
                            //从待客指示灯熄灭
                            if(waitFromOut.getStatus(i+1)) waitFromOut.setValue(false,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath4+=1;
                        stepMessage.sprintf("%d%s",steppath4,"主复位按键点亮");
                    }
                    break;
                }
                //主复位按键点亮
                if(steppath4==3&&plcStatus[0]&&resetHostOut.setControl(true,0))
                {
                    steppath4+=1;
                    stepMessage.sprintf("%d%s",steppath4,"座位照明关");
                    break;
                }
                else if(steppath4==3&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //座位照明关
                if(steppath4==4)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //从待客指示灯熄灭
                            if(!seatLightFromOut.getStatus(i+1))  retBool=true;
                            //从待客指示灯熄灭
                            if(seatLightFromOut.getStatus(i+1)) seatLightFromOut.setValue(false,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath4+=1;
                        stepMessage.sprintf("%d%s",steppath4,"平台运动中位 护栏下降");
                    }
                    break;
                }
                //影片载入
                if(steppath4==5)
                {
                    //LOAD 001.XML
                    char mdata[14] = {0x4C,0x4F,0x41,0x44,0x20,0x30,0x30,0x31,0x2E,0x58,0x4D,0x4C,0x0D,0x0A };
                    int  sque=movieSque.toInt();
                    mdata[5]=0x30+sque/100;
                    mdata[6]=0x30+(sque%100)/10;
                    mdata[7]=0x30+sque%10;
                    mQTcpSocket.write(mdata,14);
                    steppath4+=1;
                    break;
                }
                //平台运动中位 护栏下降
                if(steppath4==6)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        mMotusOperaterStatus.slowbit[i]=false;
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //平台回中位 护栏下降
                            if((platfromStatus[i]==3||platfromStatus[i]==15)                             
                               &&railDownFromIn.getStatus(i+1)
                               &&!railDownFromOut.getStatus(i+1)
                               ) retBool=true;
                            //平台回中
                            if(platfromStatus[i]==11) mMotusPlatfrom.sendPlatfromCmd(2,i);
                            //护栏下降
                            if(!railDownFromIn.getStatus(i+1)) railDownFromOut.setValue(true,i+1);
                            //护栏下降后输出关闭
                            if(railDownFromIn.getStatus(i+1)&&railDownFromOut.getStatus(i+1)) railDownFromOut.setValue(false,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"号平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath4+=1;
                        stepMessage.sprintf("%d%s",steppath4,"平台前进");
                    }
                    break;
                }
                //平台前进
                if(steppath4==7)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        mMotusOperaterStatus.liftbelt[i]=true;
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //小车触碰到极限位 进行报错处理
                            if(mCarControl.getLimitStatus(i+1))
                            {
                                pathStep=7;
                                errorMessage.sprintf("%d%s",i+1,"小车触碰到极限位");
                                mMotusRunLog->addStringLog(errorMessage);
                                break;
                            }
                            //判断小车是否到位
                            if(mCarControl.getFrontStopStatus(i+1)
                               &&!mCarControl.getFrontControl(i+1)&&!mCarControl.getBackControl(i+1)
                               &&!mCarControl.getHighSpeedControl(i+1)&&!mCarControl.getLowSpeedControl(i+1)
                               &&mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&!mCarControl.getGasBrakeCloseControl(i+1)) retBool=true;
                            //小车前进  气刹关关闭
                            if(!mCarControl.getFrontStopStatus(i+1)&&mCarControl.getGasBrakeCloseControl(i+1))
                            {
                                mCarControl.setGasBrakeCloseControl(false,i+1);
                                continue;
                            }
                            //小车前进  气刹开打开
                            if(!mCarControl.getFrontStopStatus(i+1)&&!mCarControl.getGasBrakeControl(i+1))
                            {
                                mCarControl.setGasBrakeControl(true,i+1);
                                continue;
                            }
                            //小车前进  后退关闭
                            if(!mCarControl.getFrontStopStatus(i+1)&&mCarControl.getBackControl(i+1))
                            {
                                mCarControl.setBackControl(false,i+1);
                                continue;
                            }
                            //小车后退 前进打开
                            if(!mCarControl.getFrontStopStatus(i+1)&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getFrontControl(i+1))
                            {
                                mCarControl.setFrontControl(true,i+1);
                                continue;
                            }
                            //小车前进  高速打开
                            if(!mCarControl.getFrontStopStatus(i+1)&&!mMotusOperaterStatus.slowbit[i]&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getHighSpeedControl(i+1))
                            {
                                mCarControl.setHighSpeedControl(true,i+1);
                                continue;
                            }
                            //小车前进  高速关闭
                            if(!mCarControl.getFrontStopStatus(i+1)&&!mMotusOperaterStatus.slowbit[i]&&mCarControl.getFrontLowSpeed(i+1))
                            {
                               mMotusOperaterStatus.slowbit[i]=true;
                            }
                            //小车前进  高速打开
                            if(!mCarControl.getFrontStopStatus(i+1)&&mMotusOperaterStatus.slowbit[i]&&!mCarControl.getGasBrake(i+1)&&mCarControl.getHighSpeedControl(i+1))
                            {
                                mCarControl.setHighSpeedControl(false,i+1);
                                continue;
                            }
                            //小车后退 低速打开
                            if(!mCarControl.getFrontStopStatus(i+1)&&mMotusOperaterStatus.slowbit[i]&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getLowSpeedControl(i+1))
                            {
                                mCarControl.setLowSpeedControl(true,i+1);
                                continue;
                            }
                            //清除速度
                            if(mCarControl.getFrontStopStatus(i+1))
                            {
                               mCarControl.clearOutput(i+1);                              
                            }
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath4+=1;
                        stepMessage.sprintf("%d%s",steppath4,"平台运动");
                    }
                    break;
                }
                //影片播放信号发送
                if(steppath4==8)
                {
                    const char VideoPlayCmd[6] = {0x50,0x4C,0x41,0x59,0x0D,0x0A};
                    mQTcpSocket.write(VideoPlayCmd,6);
                    mMotusOperaterStatus.playbit=true;
                    steppath4=0;
                    pathStep=5;
                    break;
                }
            }
            //平台运动
            if(pathStep==5)
            {
                static int steppath5=0;         //步骤标志位
                int i=0;                        //循环计数
                bool retBool=false;             //返回结果
                int retCount=0;                 //结果个数
                int totalCount=0;               //统计个数
                bool safeBelt=false;            //安全带判断
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(true);
                //复位初始化
                if(resetBit[5]){  steppath5=0;  resetBit[5]=false; }
                //平台运动
                if(steppath5==0)
                {
                    static int mainpos[3]={11,12,13};
                    bool mainbit[3];
                    static int sencondpos[2]={45,46};
                    bool sencondbit[2];
                    retCount=0; totalCount=0; float data[6]={0.f,0.f,0.f,0.f,0.f,0.f};
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            safeBelt=false;
                            //安全带检测
                            if(saftBelt1FromIn.getStatus(i+1)&&saftBelt2FromIn.getStatus(i+1)
                               &&saftBelt2FromIn.getStatus(i+1)&&saftBelt3FromIn.getStatus(i+1))  safeBelt=true;
                            //安全带松开回中
                            if(!safeBelt&&mMotusOperaterStatus.liftbelt[i]) mMotusOperaterStatus.liftbelt[i]=false;
                            //安全带松开回中执行
                            if(!mMotusOperaterStatus.liftbelt[i])
                            {
                                mMotusPlatfrom.sendPlatfromCmd(2,i);      //平台回中
                                errorMessage.sprintf("%d%s",i+1,"安全带未插好");
                                mMotusRunLog->addStringLog(errorMessage);
                                mainbit[0]=mainbit[1]=mainbit[2]=false;
                                sencondbit[0]=sencondbit[1]=false;
                                mMotusBasePlc.writePlcIo(mainbit,mainpos,3,0);
                                mMotusBasePlc.writePlcIo(sencondbit,sencondpos,2,i+1);
                            }
                            //////////////////////////////////////////////////////////////////////////////////////
                            //平台回中位 护栏下降
                            if((platfromStatus[i]==3||platfromStatus[i]==15||platfromStatus[i]==4)
                               &&railDownFromIn.getStatus(i+1)
                               &&pleverDownFromIn.getStatus(i+1)
                               &&mMotusOperaterStatus.liftbelt[i]
                               )
                            {
                                if(playMovieCount<playMovieData.size())
                                {
                                   data[0]=playMovieData[playMovieCount].atu1;
                                   data[1]=playMovieData[playMovieCount].atu2;
                                   data[2]=playMovieData[playMovieCount].atu3;
                                   data[3]=playMovieData[playMovieCount].atu4;
                                   data[4]=playMovieData[playMovieCount].atu5;
                                   data[5]=playMovieData[playMovieCount].atu6;
                                   mMotusPlatfrom.sendPlatfromAttu(data,i);
                                   if((playMovieData[playMovieCount].speeff&0x1)==0x1) mainbit[0]=true;
                                   else mainbit[0]=false;
                                   if((playMovieData[playMovieCount].speeff&0x2)==0x2) mainbit[1]=true;
                                   else mainbit[1]=false;
                                   if((playMovieData[playMovieCount].speeff&0x4)==0x4) mainbit[2]=true;
                                   else mainbit[2]=false;
                                   if((playMovieData[playMovieCount].speeff&0x8)==0x8) sencondbit[0]=true;
                                   else sencondbit[0]=false;
                                   if((playMovieData[playMovieCount].speeff&0x10)==0x10) sencondbit[1]=true;
                                   else sencondbit[1]=false;
                                   mMotusBasePlc.writePlcIo(mainbit,mainpos,3,0);
                                   mMotusBasePlc.writePlcIo(sencondbit,sencondpos,2,i+1);
                                }
                            }
                            ////////////////////////////////////////////////////////////////////////////////////////
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            playMovieCount=0;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    playMovieCount++;
                    if(playMovieCount>=playMovieData.size())
                    {
                        playMovieCount=0;
                        emit sendPlay(false);
                        mMotusOperaterStatus.playbit=false;
                        steppath5+=1;
                        stepMessage.sprintf("%d%s",steppath5,"烟雾特效关");
                    }
                    break;
                }
                //烟雾特效关
                if(steppath5==1&&plcStatus[0]&&smokeHostOut.setControl(false,0))
                {
                    steppath5+=1;
                    stepMessage.sprintf("%d%s",steppath5,"频闪特效关");
                    break;
                }
                else if(steppath5==1&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //频闪特效关
                if(steppath5==2&&plcStatus[0]&&strobeHostOut.setControl(false,0))
                {
                    steppath5+=1;
                    stepMessage.sprintf("%d%s",steppath5,"泡泡特效关");
                    break;
                }
                else if(steppath5==2&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //泡泡特效关
                if(steppath5==3&&plcStatus[0]&&hubbleHostOut.setControl(false,0))
                {
                    steppath5+=1;
                    stepMessage.sprintf("%d%s",steppath5,"冷风特效关");
                    break;
                }
                else if(steppath5==3&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //冷风特效关
                if(steppath5==4)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //冷风特效关
                            if(coolWindFromOut.setControl(false,0))  retBool=true;
                            else if(mMotusOperaterStatus.freerror[i]) retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath5+=1;
                        stepMessage.sprintf("%d%s",steppath5,"喷水特效关");
                    }
                    break;
                }
                //喷水特效关
                if(steppath5==5)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //冷风特效关
                            if(waterSprayFromOut.setControl(false,0))  retBool=true;
                            else if(mMotusOperaterStatus.freerror[i]) retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath5=0;
                        pathStep=6;
                        stepMessage.sprintf("%d%s",steppath5,"平台回中");
                    }
                    break;
                }
            }
            //恢复处理
            if(pathStep==6)
            {
                static int steppath6=0;                   //步骤标志位
                int i=0;                                  //循环计数
                bool retBool=false;                       //返回结果
                int retCount=0;                           //结果个数
                int totalCount=0;                         //统计个数
                emit sendHandPermissin(false);            //不允许手动操作
                emit sendPlay(false);

                //复位初始化
                if(resetBit[6]){  steppath6=0;  resetBit[6]=false; }
                //平台回中
                if(steppath6==0)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        mMotusOperaterStatus.slowbit[i]=false;
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {                            
                            totalCount+=1;  retBool=false;
                            //平台回中位
                            if((platfromStatus[i]==3||platfromStatus[i]==15)
                               &&mCarControl.getFrontStopStatus(i+1)
                               &&pleverDownFromIn.getStatus(i+1)
                               ) retBool=true;
                            else if(mMotusOperaterStatus.platerror[i])
                            {
                                retBool=true;
                                if(retBool) retCount+=1;
                                continue;
                            }
                            //平台回中
                            if(platfromStatus[i]==4
                               &&mCarControl.getFrontStopStatus(i+1)
                               &&pleverDownFromIn.getStatus(i+1)) mMotusPlatfrom.sendPlatfromCmd(2,i);
                            //小车未在前进停止位 压杆未压下
                            if(!mCarControl.getFrontStopStatus(i+1)||!pleverDownFromIn.getStatus(i+1))
                            {
                                pathStep=7;
                                if(!mCarControl.getFrontStopStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"平台未在前进停止位");
                                mMotusRunLog->addStringLog(errorMessage);
                                if(!pleverDownFromIn.getStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"压杆未锁定");
                                mMotusRunLog->addStringLog(errorMessage);
                                break;
                            }
                            //计数
                            if(retBool) {retCount+=1;}
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }                    
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"平台后退");
                    }
                    break;
                }
                //平台后退
                if(steppath6==1)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用。
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //小车触碰到极限位 进行报错处理
                            if(mCarControl.getLimitStatus(i+1))
                            {
                                errorMessage.sprintf("%d%s",i+1,"小车触碰到极限位");
                                mMotusRunLog->addStringLog(errorMessage);
                                mMotusOperaterStatus.freerror[i]=true;
                                retBool=true;
                                if(retBool) retCount+=1;
                                continue;
                            }
                            //判断小车是否到位
                            if(mCarControl.getBackStopStatus(i+1)
                               &&!mCarControl.getFrontControl(i+1)&&!mCarControl.getBackControl(i+1)
                               &&!mCarControl.getHighSpeedControl(i+1)&&!mCarControl.getLowSpeedControl(i+1)
                               &&mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&!mCarControl.getGasBrakeCloseControl(i+1)) retBool=true;
                            else if(mMotusOperaterStatus.freerror[i])
                            {
                                retBool=true;
                                if(retBool) retCount+=1;
                                continue;
                            }
                            //小车后退  气刹关关闭
                            if(!mCarControl.getBackStopStatus(i+1)&&mCarControl.getGasBrakeCloseControl(i+1))
                            {
                                mCarControl.setGasBrakeCloseControl(false,i+1);
                                continue;
                            }
                            //小车后退  气刹开打开
                            if(!mCarControl.getBackStopStatus(i+1)&&!mCarControl.getGasBrakeControl(i+1))
                            {
                                mCarControl.setGasBrakeControl(true,i+1);
                                continue;
                            }
                            //小车后退  前进关闭
                            if(!mCarControl.getBackStopStatus(i+1)&&mCarControl.getFrontControl(i+1))
                            {
                                mCarControl.setFrontControl(false,i+1);
                                continue;
                            }
                            //小车后退 后退打开
                            if(!mCarControl.getBackStopStatus(i+1)&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getBackControl(i+1))
                            {
                                mCarControl.setBackControl(true,i+1);
                                continue;
                            }
                            //小车后退  高速打开
                            if(!mCarControl.getBackStopStatus(i+1)&&!mMotusOperaterStatus.slowbit[i]&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getHighSpeedControl(i+1))
                            {
                                mCarControl.setHighSpeedControl(true,i+1);
                                continue;
                            }
                            //小车前进  高速关闭
                            if(!mCarControl.getBackStopStatus(i+1)&&!mMotusOperaterStatus.slowbit[i]&&mCarControl.getBackLowSpeed(i+1))
                            {
                               mMotusOperaterStatus.slowbit[i]=true;
                            }
                            //小车前进  高速打开
                            if(!mCarControl.getBackStopStatus(i+1)&&mMotusOperaterStatus.slowbit[i]&&!mCarControl.getGasBrake(i+1)&&mCarControl.getHighSpeedControl(i+1))
                            {
                                mCarControl.setHighSpeedControl(false,i+1);
                                continue;
                            }
                            //小车后退 低速打开
                            if(!mCarControl.getBackStopStatus(i+1)&&mMotusOperaterStatus.slowbit[i]&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getLowSpeedControl(i+1))
                            {
                                mCarControl.setLowSpeedControl(true,i+1);
                                continue;
                            }
                            //清除速度
                            if(mCarControl.getBackStopStatus(i+1))
                            {
                               mCarControl.clearOutput(i+1);                              
                            }
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"平台落下，护栏升起");
                    }
                    break;
                }
                //平台顶位 护栏上升
                if(steppath6==2)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //平台回顶位   护栏上升
                            if((platfromStatus[i]==11)
                               &&railUpFromIn.getStatus(i+1)
                               &&railReverFromIn.getStatus(i+1)
                               &&!railUpFromOut.getStatus(i+1)
                               ) retBool=true;
                            else  if(mMotusOperaterStatus.platerror[i]
                                     &&railUpFromIn.getStatus(i+1)
                                     &&railReverFromIn.getStatus(i+1)
                                     &&!railUpFromOut.getStatus(i+1)
                                     ) retBool=true;
                            else  if((platfromStatus[i]==11)
                                     &&mMotusOperaterStatus.freerror[i]
                                     ) retBool=true;
                            else  if(mMotusOperaterStatus.platerror[i]
                                     &&mMotusOperaterStatus.freerror[i]
                                     ) retBool=true;
                            if(retBool)
                            {
                                retCount+=1;
                                continue;
                            }
                            //平台到顶
                            if(platfromStatus[i]==3||platfromStatus[i]==15) mMotusPlatfrom.sendPlatfromCmd(12,i);
                            //护栏上升
                            if(mCarControl.getBackStopStatus(i+1)&&(!railUpFromIn.getStatus(i+1)||!railReverFromIn.getStatus(i+1))) railUpFromOut.setValue(true,i+1);
                            //护栏锁定
                            if(railUpFromIn.getStatus(i+1)&&railReverFromIn.getStatus(i+1)&&railUpFromOut.getStatus(i+1)) railUpFromOut.setValue(false,i+1);
                            //计数

                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"待客指示灯亮");
                    }
                    break;
                }                
                //主待客指示灯点亮
                if(steppath6==3&&plcStatus[0]&&waitHostOut.setControl(true,0))
                {
                    steppath6+=1;
                    stepMessage.sprintf("%d%s",steppath6,"从待客指示灯点亮");
                    break;
                }
                else if(steppath6==3&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                    break;
                }
                //从待客指示灯点亮
                if(steppath6==4)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //从待客指示灯点亮
                            if(waitFromOut.getStatus(i+1))  retBool=true;
                            else if(mMotusOperaterStatus.freerror[i])  retBool=true;
                            if(retBool)
                            {
                                retCount+=1;//计数
                                continue;
                            }
                            //从待客指示灯点亮
                            if(!waitFromOut.getStatus(i+1)) waitFromOut.setValue(true,i+1);

                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"座位照明开");
                    }
                    break;
                }
                //座位照明开
                if(steppath6==5)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        mMotusOperaterStatus.makesure[i]=false;
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //座位照明开
                            if(seatLightFromOut.setControl(true,i+1))  retBool=true;
                            else if(mMotusOperaterStatus.freerror[i])  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"可以下客和上客");
                    }
                    break;
                }
                //压下压杆后，确认指示灯亮后，按下确认按键
                if(steppath6==6)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            if(!pleverDownFromIn.getStatus(i+1))  mMotusOperaterStatus.makesure[i]=false;
                            //确认指示灯点亮
                            if(pleverDownFromIn.getStatus(i+1)&&!mSureFromOut.getStatus(i+1)&&!mMotusOperaterStatus.makesure[i])
                             mSureFromOut.setValue(true,i+1);
                            //标志位置true
                            if(pleverDownFromIn.getStatus(i+1)
                                    &&saftBelt1FromIn.getStatus(i+1)
                                    &&saftBelt2FromIn.getStatus(i+1)
                                    &&saftBelt3FromIn.getStatus(i+1)
                                    &&saftBelt4FromIn.getStatus(i+1)
                                    &&msureFormIn.getStatus(i+1)
                                    &&mSureFromOut.getStatus(i+1)
                                    &&!mMotusOperaterStatus.makesure[i])
                             mMotusOperaterStatus.makesure[i]=true;
                             //熄灭确认指示灯
                            if(pleverDownFromIn.getStatus(i+1)&&mSureFromOut.getStatus(i+1)&&mMotusOperaterStatus.makesure[i])
                             mSureFromOut.setValue(false,i+1);
                            //运行按键
                            if(pleverDownFromIn.getStatus(i+1)
                                    &&saftBelt1FromIn.getStatus(i+1)
                                    &&saftBelt2FromIn.getStatus(i+1)
                                    &&saftBelt3FromIn.getStatus(i+1)
                                    &&saftBelt4FromIn.getStatus(i+1)
                                    &&!mSureFromOut.getStatus(i+1)
                                    &&mMotusOperaterStatus.makesure[i])
                                retBool=true;
                            else if(mMotusOperaterStatus.freerror[i])
                                retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6=0;
                        pathStep=3;
                        stepMessage.sprintf("%d%s",steppath6,"待客状态");
                    }
                    break;
                }
            }
            //故障处理
            if(pathStep==7)
            {
                static int steppath7=0;
                int i=0;              //循环计数
                bool retBool=false;   //返回结果
                int retCount=0;       //结果个数
                int totalCount=0;     //统计个数
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[7]){steppath7=0;resetBit[7]=false;}
                if(mMotusOperaterStatus.playbit)
                {
                    const char VideoStopCmd[6] = {0x53,0x54,0x4F,0x50,0x0D ,0x0A };
                    mQTcpSocket.write(VideoStopCmd,6);
                    mMotusOperaterStatus.playbit=false;
                    playMovieCount=0;
                }
                //////////////////////////主PLC程序控制//////////////////////
                //点亮故障指示灯
                if(steppath7==0&&plcStatus[0]&&faultHostOut.setControl(true,0))
                {                    
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭恢复指示灯");
                    break;
                }
                else if(steppath7==0&&plcStatus[0]&&!faultHostOut.getStatus(0))
                {
                    stepMessage.sprintf("%d%s",steppath7,"点亮故障指示灯");
                    break;
                }
                else if(steppath7==0&&!plcStatus[0])
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭恢复指示灯");
                    break;
                }
                //熄灭恢复指示灯
                if(steppath7==1&&plcStatus[0]&&renewHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭复位指示灯");
                    break;
                }
                else if(steppath7==1&&!plcStatus[0])
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭复位指示灯");
                    break;
                }
                //熄灭复位指示灯
                if(steppath7==2&&plcStatus[0]&&resetHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭继续指示灯");
                    break;
                }
                else if(steppath7==2&&!plcStatus[0])
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭继续指示灯");
                    break;
                }
                //熄灭继续指示灯
                if(steppath7==3&&plcStatus[0]&&continueHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭待客指示灯");
                    break;
                }
                else if(steppath7==3&&!plcStatus[0])
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭待客指示灯");
                    break;
                }
                //熄灭待客指示灯
                if(steppath7==4&&plcStatus[0]&&waitHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"烟雾特效关");
                    break;
                }
                else if(steppath7==4&&!plcStatus[0])
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"烟雾特效关");
                    break;
                }
                //烟雾特效关
                if(steppath7==5&&plcStatus[0]&&smokeHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"频闪特效关");
                    break;
                }
                else if(steppath7==5&&!plcStatus[0])
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"频闪特效关");
                    break;
                }
                //频闪特效关
                if(steppath7==6&&plcStatus[0]&&strobeHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"泡泡特效关");
                    break;
                }
                else if(steppath7==6&&!plcStatus[0])
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"泡泡特效关");
                    break;
                }
                //泡泡特效关
                if(steppath7==7&&plcStatus[0]&&hubbleHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"点亮故障指示灯");
                    break;
                }
                else if(steppath7==7&&!plcStatus[0])
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"点亮故障指示灯");
                    break;
                }
                //////////////////////////主PLC控制结束//////////////////////////
                //////////////////////////从PLC控制//////////////////////////////
                //点亮故障指示灯
                if(steppath7==8)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=faultFromOut.setControl(true,i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"熄灭确认指示灯");
                    }
                    break;
                }
                //熄灭确认指示灯
                if(steppath7==9)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=mSureFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"熄灭待客指示灯");
                    }
                    break;
                }
                //熄灭待客指示灯
                if(steppath7==10)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=waitFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"冷风特效关");
                    }
                    break;
                }
                //冷风特效关
                if(steppath7==11)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=coolWindFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"喷水特效关");
                    }
                    break;
                }
                //喷水特效关
                if(steppath7==12)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=waterSprayFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"座椅照明开");
                    }
                    break;
                }
                //座椅照明开
                if(steppath7==13)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=seatLightFromOut.setControl(true,i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"护栏上输出关");
                    }
                    break;
                }
                //护栏上输出关
                if(steppath7==14)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=railUpFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"护栏下输出关");
                    }
                    break;
                }
                //护栏下输出关
                if(steppath7==15)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=railDownFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"小车输出关");
                    }
                    break;
                }
                //小车输出关
                if(steppath7==16)
                {
                    retCount=0;   totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=mCarControl.clearOutput(i+1);
                            if(retBool) retCount+=1;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath7+=1;
                        stepMessage.sprintf("%d%s",steppath7,"点亮复位指示灯");
                    }
                    break;
                }
                //点亮复位指示灯
                if(steppath7==17&&plcStatus[0]&&resetHostOut.setControl(true,0))
                {
                    stepMessage.sprintf("%d%s",steppath7,"故障处理完成");
                    break;
                }
                else if(steppath7==17&&!plcStatus[0])
                {
                    stepMessage.sprintf("%d%s",steppath7,"故障处理完成");
                    break;
                }
            }
            //关机处理
            if(pathStep==8)
            {
                static int steppath8=0;
                int i=0;                                  //循环计数
                bool retBool=false;                       //返回结果
                int retCount=0;                           //结果个数
                int totalCount=0;                         //统计个数
                emit sendHandPermissin(false);            //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[8]){steppath8=0;resetBit[8]=false;}
                //////////////////////////主PLC程序控制//////////////////////
                //熄灭待客指示灯
                if(steppath8==0&&plcStatus[0]&&waitHostOut.setControl(false,0))
                {
                    steppath8+=1;
                    stepMessage.sprintf("%d%s",steppath8,"熄灭复位指示灯");
                    break;
                }
                else if(steppath8==0&&plcStatus[0]&&waitHostOut.getStatus(0))
                {
                    stepMessage.sprintf("%d%s",steppath8,"熄灭主待客指示灯关");
                    break;
                }
                else if(steppath8==0&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                }
                //熄灭复位指示灯
                if(steppath8==1&&plcStatus[0]&&resetHostOut.setControl(false,0))
                {
                    steppath8+=1;
                    stepMessage.sprintf("%d%s",steppath8,"熄灭运行指示灯");
                    break;
                }
                else if(steppath8==1&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                }
                //熄灭运行指示灯
                if(steppath8==2&&plcStatus[0]&&renewHostOut.setControl(false,0))
                {
                    steppath8+=1;
                    stepMessage.sprintf("%d%s",steppath8,"熄灭从待客指示灯");
                    break;
                }
                else if(steppath8==2&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"号PLC通讯异常");
                    mMotusRunLog->addStringLog(errorMessage);
                }
                //熄灭待客指示灯
                if(steppath8==3)
                {
                    retCount=0;totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=waitFromOut.setControl(false,i+1);
                            if(retBool)retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath8+=1;
                        stepMessage.sprintf("%d%s",steppath8,"座位照明关");
                    }
                    break;
                }
                //座位照明关
                if(steppath8==4)
                {
                    retCount=0;totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=seatLightFromOut.setControl(false,i+1);
                            if(retBool)retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath8+=1;
                        stepMessage.sprintf("%d%s",steppath8,"平台回底");
                    }
                    break;
                }
                //平台回底
                if(steppath8==5)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //底位限位亮 目的
                            if((platfromStatus[i]==1||platfromStatus[i]==13)&&cylinderFromIn.getStatus(i+1))  retBool=true;
                            //平台中位回底
                            if(platfromStatus[i]==3||platfromStatus[i]==15) mMotusPlatfrom.sendPlatfromCmd(7,i);
                            //平台顶位回中
                            if(platfromStatus[i]==11) mMotusPlatfrom.sendPlatfromCmd(2,i);
                            //平台运动回底
                            if(platfromStatus[i]==4) mMotusPlatfrom.sendPlatfromCmd(7,i);
                            //平台初始化寻低
                            if(platfromStatus[i]==55) mMotusPlatfrom.sendPlatfromCmd(4,i);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"号平台通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"号PLC通讯异常");
                            mMotusRunLog->addStringLog(errorMessage);
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        stepMessage.sprintf("%d%s",steppath8,"关机结束");
                        pathStep=0;
                    }
                    break;
                }
            }
            break;
        }
        case HandMove:
        {
            switch (currentSonCmd)
            {
                //频闪
                case StrobeEffect:
                     {
                        if(plcStatus[0])
                        {
                           strobeHostOut.setValue(actionValue,0);
                        }
                        break;
                     }
                //待客指示灯
                case WaittingCustomer:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                           waitFromOut.setValue(actionValue,currentHandPlatfrom);
                        }
                        break;
                     }
                //护栏升
                case GuardBarUp:
                     {
                        if(plcStatus[currentHandPlatfrom]
                          &&(!railUpFromIn.getStatus(currentHandPlatfrom)
                             ||!railReverFromIn.getStatus(currentHandPlatfrom)))
                        {
                           railUpFromOut.setControl(true,currentHandPlatfrom);
                        }
                        else  if(plcStatus[currentHandPlatfrom]
                                 &&railUpFromIn.getStatus(currentHandPlatfrom)
                                 &&railReverFromIn.getStatus(currentHandPlatfrom))
                        {
                           railUpFromOut.setControl(false,currentHandPlatfrom);
                        }
                        break;
                     }
                //护栏降
                case GuardBarDown:
                     {
                        if(plcStatus[currentHandPlatfrom]
                           &&!railDownFromIn.getStatus(currentHandPlatfrom))
                        {
                           railDownFromOut.setControl(true,currentHandPlatfrom);
                        }
                        else if(plcStatus[currentHandPlatfrom]                                
                                &&railDownFromIn.getStatus(currentHandPlatfrom))
                         {
                            railDownFromOut.setControl(false,currentHandPlatfrom);
                         }
                        break;
                     }
                //冷风特效
                case ColdWind:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                           coolWindFromOut.setValue(actionValue,currentHandPlatfrom);
                        }
                        break;
                     }
                //喷水特效
                case SprayWater:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                           waterSprayFromOut.setValue(actionValue,currentHandPlatfrom);
                        }
                        break;
                     }
                //座椅照明
                case SeatLight:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                           seatLightFromOut.setValue(actionValue,currentHandPlatfrom);
                        }
                        break;
                     }
                //平台升
                case PlatfromUp:
                     {
                        if(mMotusPlatfrom.getConnect(currentHandPlatfrom-1))
                        {
                            mMotusPlatfrom.sendPlatfromCmd(6,currentHandPlatfrom-1);
                        }
                        break;
                     }
                //平台降
                case PlatfromDown:
                     {
                        if(mMotusPlatfrom.getConnect(currentHandPlatfrom-1))
                        {
                            mMotusPlatfrom.sendPlatfromCmd(7,currentHandPlatfrom-1);
                        }
                        break;
                     }
                //小车前进
                case CarFront:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                            //小车触碰到极限位 进行处理 后退关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getBackControl(currentHandPlatfrom))
                            {
                                mCarControl.setBackControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰极限位 前进关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getFrontControl(currentHandPlatfrom))
                            {
                                mCarControl.setFrontControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  高速关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getHighSpeedControl(currentHandPlatfrom))
                            {
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  低速关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getLowSpeedControl(currentHandPlatfrom))
                            {
                                mCarControl.setLowSpeedControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  气刹开关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getGasBrakeControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  气刹关打开
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&!mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getGasBrakeCloseControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeCloseControl(true,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  气刹刹住 气刹关输出关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getGasBrake(currentHandPlatfrom)&&mCarControl.getGasBrakeCloseControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeCloseControl(false,currentHandPlatfrom);
                                break;
                            }
                            //判断小车是否到位
                            if(mCarControl.getFrontStopStatus(currentHandPlatfrom)
                               &&!mCarControl.getFrontControl(currentHandPlatfrom)&&!mCarControl.getBackControl(currentHandPlatfrom)
                               &&!mCarControl.getHighSpeedControl(currentHandPlatfrom)&&!mCarControl.getLowSpeedControl(currentHandPlatfrom)
                               &&mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getGasBrakeControl(currentHandPlatfrom)
                               &&!mCarControl.getGasBrakeCloseControl(currentHandPlatfrom)) break;

                            //小车后退  气刹关关闭
                            if(!mCarControl.getFrontStopStatus(currentHandPlatfrom)&&mCarControl.getGasBrakeCloseControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeCloseControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车后退  气刹开打开
                            if(!mCarControl.getFrontStopStatus(currentHandPlatfrom)&&!mCarControl.getGasBrakeControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeControl(true,currentHandPlatfrom);
                                break;
                            }
                            //小车后退  高速关闭
                            if(!mCarControl.getFrontStopStatus(currentHandPlatfrom)&&mCarControl.getHighSpeedControl(currentHandPlatfrom))
                            {
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车后退 后退关闭
                            if(!mCarControl.getFrontStopStatus(currentHandPlatfrom)&&mCarControl.getBackControl(currentHandPlatfrom))
                            {
                                mCarControl.setBackControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车后退 前进打开
                            if(!mCarControl.getFrontStopStatus(currentHandPlatfrom)&&!mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getFrontControl(currentHandPlatfrom))
                            {
                                mCarControl.setFrontControl(true,currentHandPlatfrom);
                                break;
                            }
                            //小车后退 低速打开
                            if(!mCarControl.getFrontStopStatus(currentHandPlatfrom)&&!mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getLowSpeedControl(currentHandPlatfrom))
                            {
                                mCarControl.setLowSpeedControl(true,currentHandPlatfrom);
                                break;
                            }
                            //清除速度
                            if(mCarControl.getFrontStopStatus(currentHandPlatfrom))
                            {
                               mCarControl.clearOutput(currentHandPlatfrom);
                               break;
                            }
                        }
                        break;
                     }
                //小车后退
                case CarBack:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                            //小车触碰到极限位 进行处理 后退关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getBackControl(currentHandPlatfrom))
                            {
                                mCarControl.setBackControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰极限位 前进关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getFrontControl(currentHandPlatfrom))
                            {
                                mCarControl.setFrontControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  高速关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getHighSpeedControl(currentHandPlatfrom))
                            {
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  低速关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getLowSpeedControl(currentHandPlatfrom))
                            {
                                mCarControl.setLowSpeedControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  气刹开关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getGasBrakeControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  小车没有刹住 气刹关打开
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&!mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getGasBrakeCloseControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeCloseControl(true,currentHandPlatfrom);
                                break;
                            }
                            //小车触碰到极限位  小车已经刹住 气刹关关闭
                            if(mCarControl.getLimitStatus(currentHandPlatfrom)&&mCarControl.getGasBrake(currentHandPlatfrom)&&mCarControl.getGasBrakeCloseControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeCloseControl(false,currentHandPlatfrom);
                                break;
                            }
                            //判断小车是否到位
                            if(mCarControl.getBackStopStatus(currentHandPlatfrom)
                               &&!mCarControl.getFrontControl(currentHandPlatfrom)&&!mCarControl.getBackControl(currentHandPlatfrom)
                               &&!mCarControl.getHighSpeedControl(currentHandPlatfrom)&&!mCarControl.getLowSpeedControl(currentHandPlatfrom)
                               &&mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getGasBrakeControl(currentHandPlatfrom)
                               &&!mCarControl.getGasBrakeCloseControl(currentHandPlatfrom))   break;

                            //小车后退  气刹关关闭
                            if(!mCarControl.getBackStopStatus(currentHandPlatfrom)&&mCarControl.getGasBrakeCloseControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeCloseControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车后退  气刹开打开
                            if(!mCarControl.getBackStopStatus(currentHandPlatfrom)&&!mCarControl.getGasBrakeControl(currentHandPlatfrom))
                            {
                                mCarControl.setGasBrakeControl(true,currentHandPlatfrom);
                                break;
                            }
                            //小车后退  高速关闭
                            if(!mCarControl.getBackStopStatus(currentHandPlatfrom)&&mCarControl.getHighSpeedControl(currentHandPlatfrom))
                            {
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车后退 前进关闭
                            if(!mCarControl.getBackStopStatus(currentHandPlatfrom)&&mCarControl.getFrontControl(currentHandPlatfrom))
                            {
                                mCarControl.setFrontControl(false,currentHandPlatfrom);
                                break;
                            }
                            //小车后退 后退打开
                            if(!mCarControl.getBackStopStatus(currentHandPlatfrom)&&!mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getBackControl(currentHandPlatfrom))
                            {
                                mCarControl.setBackControl(true,currentHandPlatfrom);
                                break;
                            }
                            //小车后退 低速打开
                            if(!mCarControl.getBackStopStatus(currentHandPlatfrom)&&!mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getLowSpeedControl(currentHandPlatfrom))
                            {
                                mCarControl.setLowSpeedControl(true,currentHandPlatfrom);
                                break;
                            }
                            //清除速度
                            if(mCarControl.getBackStopStatus(currentHandPlatfrom))
                            {
                               mCarControl.clearOutput(currentHandPlatfrom);
                               break;
                            }
                        }
                        break;
                     }
                //输出清除
                case OutputClear:
                 {
                    if(plcStatus[currentHandPlatfrom])
                    {
                        mCarControl.clearOutput(currentHandPlatfrom);
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
              for(int j=0;j<6;j++)
              {
                  valuedata[j*6]=platfromAble[j];
                  if(mMotusBasePlc.getPlcIOStatus(status,lenght,j+1))
                  {
                      valuedata[j*6+1]=status[4];
                      valuedata[j*6+2]=status[21];
                      valuedata[j*6+3]=status[22];
                      valuedata[j*6+4]=status[23];
                      valuedata[j*6+5]=status[24];
                  }
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
                mMotusaFlatformStatus->viewPlcStatus(plcStatus,7);
                if(mMotusBasePlc.getPlcIOStatus(status,lenght,0))
                {
                    mMotusaFlatformStatus->viewMainPlcStatus(status,lenght);
                }
                if(mMotusBasePlc.getPlcIOStatus(status,lenght,plcIndex))
                {
                    mMotusaFlatformStatus->viewFromPlcStatus(status,lenght);
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

//从PLC的选择
void MainWindow::recvPlatfromStatus(int index)
{
    plcIndex=index;
}

//手动按键
void MainWindow::on_handButton_clicked()
{
    ui->m_stackwidget->setCurrentWidget(mMotusHand);
    viewId=2;
    setWindowTitle("手动主页");
}

//平台是否能够使用
void MainWindow::recvPlatfromAble(int num ,bool able)
{
    platfromAble[num]=able;
}

//接收主窗口的命令
void MainWindow::recvMainWindoewCmd(int num,int cmd,int ma,bool ret)
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
        //当前平台
        currentHandPlatfrom=num;
        //输出
        actionValue=ret;
    }
    else if(ma==2)
    {
        //子命令
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
