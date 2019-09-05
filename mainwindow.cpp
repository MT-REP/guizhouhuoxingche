#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "motustimer.h"
//#include <QMessageBox>
#include<QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //播放主页
    mMotusOperationView=new MotusOperationView(this);
    connect(this,SIGNAL(sendPlay(bool)),mMotusOperationView,SLOT(recvPlay(bool)));
    connect(mMotusOperationView,SIGNAL(txtDataChange(QList<M_MovieData>&)),this,SLOT(txtDataChange(QList<M_MovieData>&)));
    connect(mMotusOperationView,SIGNAL(sendOperationCmd(int)),this,SLOT(recvOperationCmd(int)));
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
    railUpControl.initPara(&mMotusBasePlc,7,37);   //护栏上控制
    railDownControl.initPara(&mMotusBasePlc,8,38); //护栏下控制
    railLockFromIn.initPara(&mMotusBasePlc,9);   //护栏锁限位
    mCarControl.initPara(&mMotusBasePlc);        //小车控制
    freChangeFaultFromIn.initPara(&mMotusBasePlc,16); //变频器故障
    driverFaultFromIn.initPara(&mMotusBasePlc,17); //驱动器故障
    cylinderFromIn.initPara(&mMotusBasePlc,20);    //电动缸信号
    saftBelt1FromIn.initPara(&mMotusBasePlc,21);   //1#安全带
    saftBelt2FromIn.initPara(&mMotusBasePlc,22);   //2#安全带
    saftBelt3FromIn.initPara(&mMotusBasePlc,23);   //3#安全带
    saftBelt4FromIn.initPara(&mMotusBasePlc,24);   //4#安全带
    saftBelt5FromIn.initPara(&mMotusBasePlc,25);   //5#安全带
    saftBelt6FromIn.initPara(&mMotusBasePlc,26);   //6#安全带
    saftBelt7FromIn.initPara(&mMotusBasePlc,27);   //7#安全带
    saftBelt8FromIn.initPara(&mMotusBasePlc,28);   //8#安全带
    faultFromOut.initPara(&mMotusBasePlc,29);     //故障指示灯
    mSureFromOut.initPara(&mMotusBasePlc,30);     //确认指示灯
    resetFromOut.initPara(&mMotusBasePlc,31);     //复位指示灯
    handFromOut.initPara(&mMotusBasePlc,32);      //手动指示灯
    waitFromOut.initPara(&mMotusBasePlc,33);      //待客指示灯
    pleverLockFromOut.initPara(&mMotusBasePlc,36);   //压杆锁
    railLockFromOut.initPara(&mMotusBasePlc,39);     //护栏锁定
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
void MainWindow::txtDataChange(QList<M_MovieData>&movieData)
{
    playMovieData=movieData;
}


//主时钟
void MainWindow::masterClock(void)
{
    //qDebug()<<scramOControl.getStatus(0);
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
                //////////////////////////主PLC程序控制//////////////////////
                //熄灭故障指示灯
                if(steppath1==0&&faultHostOut.setControl(false,0))
                {
                    steppath1+=1;
                    stepMessage.sprintf("%d%s",steppath1,"熄灭恢复指示灯");
                    break;
                }
                else if(steppath1==0&&!faultHostOut.getStatus(0))
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                            retBool=railUpControl.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                            retBool=railDownControl.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath1=0;
                        pathStep=2;
                        stepMessage.sprintf("%d%s",steppath1,"平台回底");
                    }
                    break;
                }
                //////////////////////////从PLC控制/////////////////////////////
            } else if(pathStep==1&&!plcStatus[0])
            {
                pathStep=7;
                errorMessage.sprintf("%d%s",0,"PLC通讯异常");
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
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[2]){  steppath2=0;  resetBit[2]=false; }
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
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"护栏锁输出关");
                    }
                    break;
                }
                //护栏锁定输出关
                if(steppath2==1)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //护栏锁输出关  护栏锁限位
                            if(!railLockFromIn.getStatus(i+1)&&!railLockFromOut.getStatus(i+1))  retBool=true;
                            //护栏锁输出关
                            if(railLockFromOut.getStatus(i+1)) railLockFromOut.setValue(false,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath2==2)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //护栏降
                            if(railDownControl.action(i+1))  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath2==3)
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
                                break;
                            }
                            //判断小车是否到位
                            if(mCarControl.getBackStopStatus(i+1)&&!mCarControl.getBackControl(i+1)
                               &&!mCarControl.getHighSpeedControl(i+1)&&!mCarControl.getLowSpeedControl(i+1)
                               &&mCarControl.getGasBrake(i+1)&&mCarControl.getGasBrakeControl(i+1)) retBool=true;
                            //气动抱闸打开
                            if(!mCarControl.getBackStopStatus(i+1)
                                &&(mCarControl.getGasBrake(i+1)||mCarControl.getGasBrakeControl(i+1)))
                            {
                                mCarControl.setGasBrakeControl(false,i+1);
                            }
                            //设置速度
                            if(!mCarControl.getBackStopStatus(i+1)&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&(!mCarControl.getBackControl(i+1)||mCarControl.getHighSpeedControl(i+1)||!mCarControl.getLowSpeedControl(i+1)))
                            {
                                mCarControl.setBackControl(true,i+1);
                                mCarControl.setHighSpeedControl(false,i+1);
                                mCarControl.setLowSpeedControl(true,i+1);
                            }
                            //清除速度
                            if(mCarControl.getBackStopStatus(i+1)&&
                               (mCarControl.getBackControl(i+1)||mCarControl.getHighSpeedControl(i+1)
                                ||mCarControl.getLowSpeedControl(i+1)||!mCarControl.getGasBrakeControl(i+1)))
                            {
                                mCarControl.setBackControl(false,i+1);
                                mCarControl.setLowSpeedControl(false,i+1);
                                mCarControl.setHighSpeedControl(false,i+1);
                                mCarControl.setGasBrakeControl(true,i+1);
                            }
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath2==4)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //护栏上升
                            if(railUpControl.action(i+1))  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"护栏锁定");
                    }
                    break;
                }
                //护栏锁定
                if(steppath2==5)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //护栏锁输出 护栏锁限位
                            if(railLockFromIn.getStatus(i+1)&&railLockFromOut.getStatus(i+1))  retBool=true;
                            //护栏锁输出
                            if(!railLockFromOut.getStatus(i+1)) railLockFromOut.setValue(true,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath2==6)
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
                                errorMessage.sprintf("%d%s",i+1,"平台出错");
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
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2+=1;
                        stepMessage.sprintf("%d%s",steppath2,"压杆锁定开");
                    }
                    break;
                }
                //压杆锁定开
                if(steppath2==7)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //压杆锁输出关  压杆锁限位
                            if(!pleverLockFromIn.getStatus(i+1)&&!pleverLockFromOut.getStatus(i+1))  retBool=true;
                            //压杆锁输出关
                            if(railLockFromOut.getStatus(i+1)) railLockFromOut.setValue(false,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath2==8)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //压杆锁输出关  压杆锁限位
                            if(seatLightFromOut.setControl(true,i+1))  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath2==9&&plcStatus[0]&&waitHostOut.setControl(true,0))
                {
                    steppath2+=1;
                    stepMessage.sprintf("%d%s",steppath2,"从待客指示灯点亮");
                    break;
                }
                else if(steppath2==9&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    break;
                }
                //从待客指示灯点亮
                if(steppath2==10)
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
                            //从待客指示灯点亮
                            if(!waitFromOut.getStatus(i+1)) waitFromOut.setValue(true,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath2=0;
                        pathStep=3;
                        stepMessage.sprintf("%d%s",steppath2,"运行按键有效");
                    }
                    break;
                }
            }
            //待客处理
            if(pathStep==3)
            {
                emit sendHandPermissin(true); //不允许手动操作
                emit sendPlay(false);
               resetBit[8]=true;
            }
            //运动准备
            if(pathStep==4)
            {
                static int steppath4=0;
                int i=0;                                  //循环计数
                bool retBool=false;                       //返回结果
                int retCount=0;                           //结果个数
                int totalCount=0;                         //统计个数
                bool safeBelt=false;                      //安全带标志位
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[4]){  steppath4=0;  resetBit[4]=false; }
                //压杆锁定
                if(steppath4==0)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false; safeBelt=false;
                            //安全带检测
                            if(saftBelt1FromIn.getStatus(i+1)&&saftBelt2FromIn.getStatus(i+1)
                               &&saftBelt2FromIn.getStatus(i+1)&&saftBelt3FromIn.getStatus(i+1)
                               &&saftBelt4FromIn.getStatus(i+1)&&saftBelt5FromIn.getStatus(i+1)
                               &&saftBelt6FromIn.getStatus(i+1)&&saftBelt7FromIn.getStatus(i+1))  safeBelt=true;
                            //压杆检测
                            if(pleverDownFromIn.getStatus(i+1)
                               &&pleverLockFromOut.getStatus(i+1)
                               &&pleverLockFromIn.getStatus(i+1)
                               &&safeBelt
                               &&!mSureFromOut.getStatus(i+1))  retBool=true;
                            //确认指示灯点亮和熄灭
                            if(!mSureFromOut.getStatus(i+1)
                               &&pleverDownFromIn.getStatus(i+1)
                               &&safeBelt
                               &&!pleverLockFromIn.getStatus(i+1)) mSureFromOut.setValue(true,i+1);
                            else   if(mSureFromOut.getStatus(i+1)
                                      &&(!pleverDownFromIn.getStatus(i+1)||!safeBelt)) mSureFromOut.setValue(false,i+1);
                            //确认指示灯熄灭，压杆锁定
                            if(pleverDownFromIn.getStatus(i+1)
                               &&safeBelt
                               &&(!pleverLockFromOut.getStatus(i+1)||mSureFromOut.getStatus(i+1))
                               &&msureFormIn.getStatus(i+1))
                            { mSureFromOut.setValue(false,i+1);pleverLockFromOut.setValue(true,i+1);}
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath4+=1;
                        stepMessage.sprintf("%d%s",steppath4,"待客指示灯熄灭");
                    }
                    break;
                }
                //主待客指示灯熄灭
                if(steppath4==1&&plcStatus[0]&&waitHostOut.setControl(false,0))
                {
                    steppath4+=1;
                    stepMessage.sprintf("%d%s",steppath4,"从待客指示灯熄灭");
                    break;
                }
                else if(steppath4==1&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
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
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                //平台运动中位 护栏下降
                if(steppath4==5)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //平台回中位 护栏下降
                            if((platfromStatus[i]==3||platfromStatus[i]==15)
                               &&!cylinderFromIn.getStatus(i+1)
                               &&railDownControl.getIStatus(i+1)
                               &&!railDownControl.getOStatus(i+1)
                               &&railLockFromIn.getStatus(i+1)
                               &&railLockFromOut.getStatus(i+1)
                               ) retBool=true;
                            //平台回中
                            if(platfromStatus[i]==11&&!cylinderFromIn.getStatus(i+1)) mMotusPlatfrom.sendPlatfromCmd(2,i);
                            //锁定解除
                            if(!railDownControl.getIStatus(i+1)&&railLockFromOut.getStatus(i+1)) railLockFromOut.setValue(false,i+1);
                            //护栏锁定
                            else if(railDownControl.action(i+1)&&!railLockFromOut.getStatus(i+1)) railLockFromOut.setValue(true,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath4==6)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //正确停止
                            if(mCarControl.getFrontStopStatus(i+1)&&mCarControl.getGasBrake(i+1)
                               &&mCarControl.getGasBrakeControl(i+1)
                               &&!mCarControl.getFrontControl(i+1)&&!mCarControl.getBackControl(i+1)
                               &&!mCarControl.getHighSpeedControl(i+1)&&!mCarControl.getLowSpeedControl(i+1)
                              ) retBool=true;
                            //解除锁定
                            if(!mCarControl.getFrontStopStatus(i+1)&&!mCarControl.getLimitStatus(i+1)&&mCarControl.getGasBrakeControl(i+1))
                            {
                                mCarControl.setFrontControl(false,i+1); mCarControl.setBackControl(false,i+1); mCarControl.setHighSpeedControl(false,i+1);
                                mCarControl.setLowSpeedControl(false,i+1);mCarControl.setGasBrakeControl(false,i+1);
                            }
                            //小车前进
                            if(mCarControl.getBackStopStatus(i+1)&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&!mCarControl.getLowSpeedControl(i+1)&&(!mCarControl.getFrontControl(i+1)||!mCarControl.getHighSpeedControl(i+1)))
                            {
                                mCarControl.setFrontControl(true,i+1);mCarControl.setHighSpeedControl(true,i+1);
                            }
                            //小车减速
                            if(!mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&mCarControl.getFrontLowSpeed(i+1)&&mCarControl.getHighSpeedControl(i+1))
                            {
                                mCarControl.setHighSpeedControl(false,i+1);mCarControl.setLowSpeedControl(true,i+1);
                            }
                            //小车停止
                            if(!mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&mCarControl.getFrontStopStatus(i+1)
                               &&(mCarControl.getFrontControl(i+1)||mCarControl.getLowSpeedControl(i+1)||mCarControl.getHighSpeedControl(i+1))
                               )
                            {
                                mCarControl.setFrontControl(false,i+1); mCarControl.setHighSpeedControl(false,i+1);
                                mCarControl.setLowSpeedControl(false,i+1);
                            }
                            //小车刹车
                            if(!mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&mCarControl.getFrontStopStatus(i+1)
                               &&!mCarControl.getFrontControl(i+1)&&!mCarControl.getLowSpeedControl(i+1)&&!mCarControl.getHighSpeedControl(i+1)
                               )  mCarControl.setGasBrakeControl(true,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath4=0;
                        pathStep=5;
                        stepMessage.sprintf("%d%s",steppath4,"平台运动");
                    }
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
                //恢复指示灯熄灭
                if(steppath5==0)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //恢复指示灯熄灭
                            if(renewHostOut.setControl(false,0))  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath5+=1;
                        stepMessage.sprintf("%d%s",steppath5,"继续指示灯熄灭");
                    }
                    break;
                }
                //继续指示灯熄灭
                if(steppath5==1)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //继续指示灯熄灭
                            if(continueHostOut.setControl(false,0))  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                        saftBeltBit[i]=true;
                    }
                    if(retCount==totalCount)
                    {
                        steppath5+=1;
                        stepMessage.sprintf("%d%s",steppath5,"平台动作");
                    }
                    break;
                }
                //平台运动
                if(steppath5==2)
                {
                    retCount=0; totalCount=0; float data[6]={0.f,0.f,0.f,0.f,0.f,0.f};
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            safeBelt=false;
                            //安全带检测
                            if(saftBelt1FromIn.getStatus(i+1)&&saftBelt2FromIn.getStatus(i+1)
                               &&saftBelt2FromIn.getStatus(i+1)&&saftBelt3FromIn.getStatus(i+1)
                               &&saftBelt4FromIn.getStatus(i+1)&&saftBelt5FromIn.getStatus(i+1)
                               &&saftBelt6FromIn.getStatus(i+1)&&saftBelt7FromIn.getStatus(i+1))  safeBelt=true;
                            //安全带松开回中
                            if(!safeBelt&&saftBeltBit[i]) saftBeltBit[i]=false;
                            //安全带松开回中执行
                            if(!saftBeltBit[i])
                            {
                                mMotusPlatfrom.sendPlatfromCmd(2,i);    //平台回中
                                coolWindFromOut.setValue(false,i+1);    //冷风特效关
                                waterSprayFromOut.setValue(false,i+1);  //喷水特效关
                                smokeHostOut.setValue(false,0);         //烟雾特效
                                strobeHostOut.setValue(false,0);        //频闪特效
                                hubbleHostOut.setValue(false,0);        //泡泡特效
                                continue;
                            }
                            //////////////////////////////////////////////////////////////////////////////////////
                            //平台回中位 护栏下降
                            if((platfromStatus[i]==3||platfromStatus[i]==15||platfromStatus[i]==4)
                               &&!cylinderFromIn.getStatus(i+1)
                               &&railDownControl.getIStatus(i+1)
                               &&!railDownControl.getOStatus(i+1)
                               &&railLockFromIn.getStatus(i+1)
                               &&railLockFromOut.getStatus(i+1)
                               &&pleverLockFromIn.getStatus(i+1)
                               &&pleverLockFromOut.getStatus(i+1)
                               &&safeBelt
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
                                   if((playMovieData[playMovieCount].speeff&0x1)==0x1) smokeHostOut.setValue(true,0);
                                   else smokeHostOut.setValue(false,0);

                                   if((playMovieData[playMovieCount].speeff&0x2)==0x2) strobeHostOut.setValue(true,0);
                                   else strobeHostOut.setValue(false,0);

                                   if((playMovieData[playMovieCount].speeff&0x4)==0x4) hubbleHostOut.setValue(true,0);
                                   else hubbleHostOut.setValue(false,0);

                                   if((playMovieData[playMovieCount].speeff&0x8)==0x8) coolWindFromOut.setValue(true,i+1);
                                   else coolWindFromOut.setValue(false,i+1);

                                   if((playMovieData[playMovieCount].speeff&0x10)==0x10) waterSprayFromOut.setValue(true,i+1);
                                   else waterSprayFromOut.setValue(false,i+1);
                                }
                            }
                            ////////////////////////////////////////////////////////////////////////////////////////
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=6;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(playMovieCount>=playMovieData.size())
                    {
                        playMovieCount=0;
                        emit sendPlay(false);
                        steppath5+=1;
                        stepMessage.sprintf("%d%s",steppath5,"烟雾特效关");
                    }
                    break;
                }
                //烟雾特效关
                if(steppath5==3&&plcStatus[0]&&smokeHostOut.setControl(false,0))
                {
                    steppath5+=1;
                    stepMessage.sprintf("%d%s",steppath5,"频闪特效关");
                    break;
                }
                else if(steppath5==3&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    break;
                }
                //频闪特效关
                if(steppath5==4&&plcStatus[0]&&strobeHostOut.setControl(false,0))
                {
                    steppath5+=1;
                    stepMessage.sprintf("%d%s",steppath5,"泡泡特效关");
                    break;
                }
                else if(steppath5==4&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    break;
                }
                //泡泡特效关
                if(steppath5==5&&plcStatus[0]&&hubbleHostOut.setControl(false,0))
                {
                    steppath5+=1;
                    stepMessage.sprintf("%d%s",steppath5,"冷风特效关");
                    break;
                }
                else if(steppath5==5&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    break;
                }
                //冷风特效关
                if(steppath5==6)
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
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath5==7)
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
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath5+=1;
                        stepMessage.sprintf("%d%s",steppath5,"继续/恢复指示灯亮");
                    }
                    break;
                }
                //继续/恢复指示灯亮
                if(steppath5==8&&plcStatus[0]&&renewHostOut.setControl(true,0)
                                             &&continueHostOut.setControl(true,0))
                {
                    steppath5+=1;
                    stepMessage.sprintf("%d%s",steppath5,"平台恢复/继续选择");
                    break;
                }
                else if(steppath5==8&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    break;
                }
                //平台恢复/继续选择
                if(steppath5==8&&plcStatus[0])
                {
                    if(renewHostIn.getStatus(0))
                    {
                        steppath5=0;
                        pathStep=6;
                        stepMessage.sprintf("%d%s",steppath5,"恢复指示灯关");
                    }
                    if(continueHostIn.getStatus(0))
                    {
                        steppath5=0;
                        stepMessage.sprintf("%d%s",steppath5,"恢复指示灯熄灭");
                    }
                    break;
                }
                else if(steppath5==8&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
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
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[6]){  steppath6=0;  resetBit[6]=false; }
                //继续指示灯关
                if(steppath6==0)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //继续指示灯关
                            if(continueHostOut.setControl(false,0))  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"恢复指示灯关");
                    }
                    break;
                }
                //恢复指示灯关
                if(steppath6==1)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //恢复指示灯关
                            if(renewHostOut.setControl(false,0))  retBool=true;
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"平台回中");
                    }
                    break;
                }
                //平台回中
                if(steppath6==2)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //平台回中位
                            if((platfromStatus[i]==3||platfromStatus[i]==15)
                               &&!cylinderFromIn.getStatus(i+1)
                               &&mCarControl.getFrontStopStatus(i+1)
                               &&pleverLockFromOut.getStatus(i+1)
                               ) retBool=true;
                            //平台回中
                            if(platfromStatus[i]==4
                               &&!cylinderFromIn.getStatus(i+1)
                               &&mCarControl.getFrontStopStatus(i+1)
                               &&pleverLockFromOut.getStatus(i+1)) mMotusPlatfrom.sendPlatfromCmd(2,i);
                            //小车未在前进停止位 压杆未压下
                            if(!mCarControl.getFrontStopStatus(i+1)||!pleverLockFromOut.getStatus(i+1))
                            {
                                pathStep=7;
                                if(!mCarControl.getFrontStopStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"平台未在前进停止位");
                                if(!pleverLockFromOut.getStatus(i+1)) errorMessage.sprintf("%d%s",i+1,"压杆未锁定");
                                break;
                            }
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath6==3)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用。
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //正确停止
                            if(mCarControl.getBackStopStatus(i+1)&&mCarControl.getGasBrake(i+1)&&mCarControl.getGasBrakeControl(i+1)
                               &&!mCarControl.getFrontControl(i+1)&&!mCarControl.getBackControl(i+1)
                               &&!mCarControl.getHighSpeedControl(i+1)&&!mCarControl.getLowSpeedControl(i+1)
                              ) retBool=true;
                            //解除锁定
                            if(!mCarControl.getBackStopStatus(i+1)&&!mCarControl.getLimitStatus(i+1)
                               &&mCarControl.getGasBrakeControl(i+1))
                            {
                                mCarControl.setFrontControl(false,i+1); mCarControl.setBackControl(false,i+1);
                                mCarControl.setHighSpeedControl(false,i+1); mCarControl.setLowSpeedControl(false,i+1);
                                mCarControl.setGasBrakeControl(false,i+1);
                            }else if(mCarControl.getLimitStatus(i+1))
                            {
                                pathStep=7;
                                errorMessage.sprintf("%d%s",i+1,"平台触碰到极限位");
                                break;
                            }
                            //小车后退
                            if(mCarControl.getFrontStopStatus(i+1)&&!mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&(!mCarControl.getBackControl(i+1)||!mCarControl.getHighSpeedControl(i+1)))
                            {
                                mCarControl.setBackControl(true,i+1); mCarControl.setHighSpeedControl(true,i+1);
                            }
                            //小车减速
                            if(!mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&mCarControl.getBackLowSpeed(i+1)&&mCarControl.getHighSpeedControl(i+1))
                            {
                                mCarControl.setHighSpeedControl(false,i+1);mCarControl.setLowSpeedControl(true,i+1);
                            }
                            //小车停止
                            if(!mCarControl.getGasBrake(i+1)&&!mCarControl.getGasBrakeControl(i+1)
                               &&mCarControl.getBackStopStatus(i+1)
                               &&(mCarControl.getBackControl(i+1)||mCarControl.getLowSpeedControl(i+1)||mCarControl.getHighSpeedControl(i+1))
                               )
                            {
                                mCarControl.setFrontControl(false,i+1);mCarControl.setBackControl(false,i+1);
                                mCarControl.setHighSpeedControl(false,i+1); mCarControl.setLowSpeedControl(false,i+1);
                            }
                            //小车刹车
                            if(!mCarControl.getGasBrakeControl(i+1)
                               &&mCarControl.getBackStopStatus(i+1)
                               &&!mCarControl.getBackControl(i+1)&&!mCarControl.getLowSpeedControl(i+1)&&!mCarControl.getHighSpeedControl(i+1)
                               )mCarControl.setGasBrakeControl(true,i+1);
                            /////////////////////////////////////////////////////////////////////////////////////////
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                //平台顶位 护栏下降
                if(steppath6==4)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&mMotusPlatfrom.getConnect(i)&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //平台回顶位   护栏下降
                            if((platfromStatus[i]==11)
                               &&!cylinderFromIn.getStatus(i+1)
                               &&railUpControl.getIStatus(i+1)
                               &&!railUpControl.getOStatus(i+1)
                               &&railLockFromIn.getStatus(i+1)
                               &&railLockFromOut.getStatus(i+1)
                               ) retBool=true;
                            //平台回中
                            if((platfromStatus[i]==3||platfromStatus[i]==15)&&!cylinderFromIn.getStatus(i+1)) mMotusPlatfrom.sendPlatfromCmd(12,i);
                            //锁定解除
                            if(!railUpControl.getIStatus(i+1)&&railLockFromOut.getStatus(i+1)) railLockFromOut.setValue(false,i+1);
                            //护栏锁定
                            else if(railUpControl.action(i+1)&&!railLockFromOut.getStatus(i+1)) railLockFromOut.setValue(true,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&(!mMotusPlatfrom.getConnect(i)||!plcStatus[i+1]))
                        {
                            pathStep=7;
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"压杆锁定开");
                    }
                    break;
                }
                //压杆锁定开
                if(steppath6==5)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        //平台是否连接 plc是否连接 平台是否可用
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;  retBool=false;
                            //压杆锁输出关  压杆锁限位
                            if(!pleverLockFromIn.getStatus(i+1)&&!pleverLockFromOut.getStatus(i+1))  retBool=true;
                            //压杆锁输出关
                            if(railLockFromOut.getStatus(i+1)) railLockFromOut.setValue(false,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6+=1;
                        stepMessage.sprintf("%d%s",steppath6,"主待客指示灯点亮");
                    }
                    break;
                }
                //主待客指示灯点亮
                if(steppath6==6&&plcStatus[0]&&waitHostOut.setControl(true,0))
                {
                    steppath6+=1;
                    stepMessage.sprintf("%d%s",steppath6,"从待客指示灯点亮");
                    break;
                }
                else if(steppath6==6&&!plcStatus[0])
                {
                    pathStep=7;
                    errorMessage.sprintf("%d%s",0,"PLC通讯异常");
                    break;
                }
                //从待客指示灯点亮
                if(steppath6==7)
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
                            //从待客指示灯点亮
                            if(!waitFromOut.getStatus(i+1)) waitFromOut.setValue(true,i+1);
                            //计数
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            pathStep=7;
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath6==8)
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
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        steppath6=0;
                        pathStep=3;
                        stepMessage.sprintf("%d%s",steppath6,"按下运行按键");
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
                //////////////////////////主PLC程序控制//////////////////////
                //点亮故障指示灯
                if(steppath7==0&&faultHostOut.setControl(true,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭恢复指示灯");
                    break;
                }
                else if(steppath7==0&&faultHostOut.getStatus(0))
                {
                    stepMessage.sprintf("%d%s",steppath7,"点亮故障指示灯");
                    break;
                }
                //熄灭恢复指示灯
                if(steppath7==1&&renewHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭复位指示灯");
                    break;
                }
                //熄灭复位指示灯
                if(steppath7==2&&resetHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭继续指示灯");
                    break;
                }
                //熄灭继续指示灯
                if(steppath7==3&&continueHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"熄灭待客指示灯");
                    break;
                }
                //熄灭待客指示灯
                if(steppath7==4&&waitHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"烟雾特效关");
                    break;
                }
                //烟雾特效关
                if(steppath7==5&&smokeHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"频闪特效关");
                    break;
                }
                //频闪特效关
                if(steppath7==6&&strobeHostOut.setControl(false,0))
                {
                    steppath7+=1;
                    stepMessage.sprintf("%d%s",steppath7,"泡泡特效关");
                    break;
                }
                //泡泡特效关
                if(steppath7==7&&hubbleHostOut.setControl(false,0))
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
                    retCount=0;    totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=faultFromOut.setControl(true,i+1);
                            if(retBool)retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {  errorMessage.sprintf("%d%s",i+1,"PLC通讯异常"); }
                    }
                    if(retCount==totalCount) {steppath7+=1;stepMessage.sprintf("%d%s",steppath7,"熄灭确认指示灯");}
                    break;
                }
                //熄灭确认指示灯
                if(steppath7==9)
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
                        {   errorMessage.sprintf("%d%s",i+1,"PLC通讯异常"); }
                    }
                    if(retCount==totalCount) {steppath7+=1;stepMessage.sprintf("%d%s",steppath7,"熄灭待客指示灯");}
                    break;
                }
                //熄灭待客指示灯
                if(steppath7==10)
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
                        {   errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");  }
                    }
                    if(retCount==totalCount) {steppath7+=1;stepMessage.sprintf("%d%s",steppath7,"冷风特效关");}
                    break;
                }
                //冷风特效关
                if(steppath7==11)
                {
                    retCount=0;totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=coolWindFromOut.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                        }
                    }
                    if(retCount==totalCount)  {steppath7+=1;stepMessage.sprintf("%d%s",steppath7,"喷水特效关");}
                    break;
                }
                //喷水特效关
                if(steppath7==12)
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                        }
                    }
                    if(retCount==totalCount) {steppath7+=1;stepMessage.sprintf("%d%s",steppath7,"座椅照明开");}
                    break;
                }
                //座椅照明关
                if(steppath7==13)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=seatLightFromOut.setControl(true,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                        }
                    }
                    if(retCount==totalCount) {steppath7+=1;stepMessage.sprintf("%d%s",steppath7,"护栏上输出关");}
                    break;
                }
                //护栏上输出关
                if(steppath7==14)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=railUpControl.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                        }
                    }
                    if(retCount==totalCount) {steppath7+=1;stepMessage.sprintf("%d%s",steppath7,"护栏下输出关");}
                    break;
                }
                //护栏下输出关
                if(steppath7==15)
                {
                    retCount=0; totalCount=0;
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            totalCount+=1;
                            retBool=railDownControl.setControl(false,i+1);
                            if(retBool) retCount+=1;
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                        }
                    }
                    if(retCount==totalCount) {steppath7+=1;stepMessage.sprintf("%d%s",steppath7,"小车输出关");}
                    break;
                }
                //小车输出关
                if(steppath7==16)
                {
                    for(i=0;i<6;i++)
                    {
                        if(platfromAble[i]&&plcStatus[i+1])
                        {
                            mCarControl.clearOutput(i+1);
                        }
                        else if(platfromAble[i]&&!plcStatus[i+1])
                        {
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                        }
                    }
                    break;
                }
                //////////////////////////从PLC控制结束/////////////////////////////
                /// 添加复位功能按键
                ///////////////////////////////////////////////////////////////////
            }
            //关机处理
            if(pathStep==8)
            {
                static int steppath8=0;
                int i=0;                                  //循环计数
                bool retBool=false;                       //返回结果
                int retCount=0;                           //结果个数
                int totalCount=0;                         //统计个数
                emit sendHandPermissin(false); //不允许手动操作
                emit sendPlay(false);
                //复位初始化
                if(resetBit[8]){steppath8=0;resetBit[8]=false;}
                //////////////////////////主PLC程序控制//////////////////////
                //熄灭待客指示灯
                if(steppath8==0&&waitHostOut.setControl(false,0))
                {
                    steppath8+=1;
                    stepMessage.sprintf("%d%s",steppath8,"从待客指示灯关");
                    break;
                }
                else if(steppath8==0&&waitHostOut.getStatus(0))
                {
                    stepMessage.sprintf("%d%s",steppath8,"熄灭主待客指示灯关");
                    break;
                }
                //熄灭待客指示灯
                if(steppath8==1)
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath8==2)
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
                            errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
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
                if(steppath8==3)
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
                            if(!mMotusPlatfrom.getConnect(i)) errorMessage.sprintf("%d%s",i+1,"平台通讯异常");
                            if(!plcStatus[i+1]) errorMessage.sprintf("%d%s",i+1,"PLC通讯异常");
                            break;
                        }
                    }
                    if(retCount==totalCount)
                    {
                        pathStep=0;
                        stepMessage.sprintf("%d%s",steppath8,"关机结束");
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
                //烟雾
                case SmokeEffect:
                     {
                        if(plcStatus[0])
                        {
                           smokeHostOut.setValue(actionValue,0);
                        }
                        break;
                     }
                //频闪
                case StrobeEffect:
                     {
                        if(plcStatus[0])
                        {
                           strobeHostOut.setValue(actionValue,0);
                        }
                        break;
                     }
                //特效
                case HubbleEffect:
                     {
                        if(plcStatus[0])
                        {
                           hubbleHostOut.setValue(actionValue,0);
                        }
                        break;
                     }
                //压杆锁定
                case CompressBarLock:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                           pleverLockFromOut.setValue(actionValue,currentHandPlatfrom);
                        }
                        break;
                     }
                //护栏升
                case GuardBarUp:
                     {
                        if(plcStatus[currentHandPlatfrom]
                          &&!railUpControl.getIStatus(currentHandPlatfrom))
                        {
                           railUpControl.action(currentHandPlatfrom);
                        }
                        break;
                     }
                //护栏降
                case GuardBarDown:
                     {
                        if(plcStatus[currentHandPlatfrom]
                           &&!railDownControl.getIStatus(currentHandPlatfrom))
                        {
                           railDownControl.action(currentHandPlatfrom);
                        }
                        break;
                     }
                //护栏锁定
                case GuardBarLock:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                           railLockFromOut.setValue(actionValue,currentHandPlatfrom);
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
                            //小车触碰到极限位 进行报错处理
                            if(mCarControl.getLimitStatus(currentHandPlatfrom))
                            {
                                mCarControl.setBackControl(false,currentHandPlatfrom);
                                mCarControl.setFrontControl(false,currentHandPlatfrom);
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setLowSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setGasBrakeControl(true,currentHandPlatfrom);
                                break;
                            }
                            //判断小车是否到位
                            if(mCarControl.getFrontStopStatus(currentHandPlatfrom)&&!mCarControl.getFrontControl(currentHandPlatfrom)
                               &&!mCarControl.getHighSpeedControl(currentHandPlatfrom)&&!mCarControl.getLowSpeedControl(currentHandPlatfrom)
                               &&mCarControl.getGasBrake(currentHandPlatfrom)&&mCarControl.getGasBrakeControl(currentHandPlatfrom)) break;
                            //气动抱闸打开
                            if(!mCarControl.getFrontStopStatus(currentHandPlatfrom)
                                &&(mCarControl.getGasBrake(currentHandPlatfrom)||mCarControl.getGasBrakeControl(currentHandPlatfrom)))
                            {
                                mCarControl.setBackControl(false,currentHandPlatfrom);
                                mCarControl.setFrontControl(false,currentHandPlatfrom);
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setLowSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setGasBrakeControl(false,currentHandPlatfrom);
                            }
                            //设置速度
                            if(!mCarControl.getFrontStopStatus(currentHandPlatfrom)&&!mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getGasBrakeControl(currentHandPlatfrom)
                               &&(!mCarControl.getFrontControl(currentHandPlatfrom)||mCarControl.getHighSpeedControl(currentHandPlatfrom)||!mCarControl.getLowSpeedControl(currentHandPlatfrom)))
                            {
                                mCarControl.setFrontControl(true,currentHandPlatfrom);
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setLowSpeedControl(true,currentHandPlatfrom);
                            }
                            //清除速度
                            if(mCarControl.getFrontStopStatus(currentHandPlatfrom)&&
                               (mCarControl.getFrontControl(currentHandPlatfrom)||mCarControl.getHighSpeedControl(currentHandPlatfrom)
                                ||mCarControl.getLowSpeedControl(currentHandPlatfrom)||!mCarControl.getGasBrakeControl(currentHandPlatfrom)))
                            {
                                mCarControl.setFrontControl(false,currentHandPlatfrom);
                                mCarControl.setLowSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setGasBrakeControl(true,currentHandPlatfrom);
                            }
                        }
                        break;
                     }
                //小车后退
                case CarBack:
                     {
                        if(plcStatus[currentHandPlatfrom])
                        {
                            //小车触碰到极限位 进行报错处理
                            if(mCarControl.getLimitStatus(currentHandPlatfrom))
                            {
                                mCarControl.setBackControl(false,currentHandPlatfrom);
                                mCarControl.setFrontControl(false,currentHandPlatfrom);
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setLowSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setGasBrakeControl(true,currentHandPlatfrom);
                                break;
                            }
                            //判断小车是否到位
                            if(mCarControl.getBackStopStatus(currentHandPlatfrom)&&!mCarControl.getBackControl(currentHandPlatfrom)
                               &&!mCarControl.getHighSpeedControl(currentHandPlatfrom)&&!mCarControl.getLowSpeedControl(currentHandPlatfrom)
                               &&mCarControl.getGasBrake(currentHandPlatfrom)&&mCarControl.getGasBrakeControl(currentHandPlatfrom)) break;
                            //气动抱闸打开
                            if(!mCarControl.getBackStopStatus(currentHandPlatfrom)
                                &&(mCarControl.getGasBrake(currentHandPlatfrom)||mCarControl.getGasBrakeControl(currentHandPlatfrom)))
                            {
                                mCarControl.setBackControl(false,currentHandPlatfrom);
                                mCarControl.setFrontControl(false,currentHandPlatfrom);
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setLowSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setGasBrakeControl(false,currentHandPlatfrom);
                            }
                            //设置速度
                            if(!mCarControl.getBackStopStatus(currentHandPlatfrom)&&!mCarControl.getGasBrake(currentHandPlatfrom)&&!mCarControl.getGasBrakeControl(currentHandPlatfrom)
                               &&(!mCarControl.getBackControl(currentHandPlatfrom)||mCarControl.getHighSpeedControl(currentHandPlatfrom)||!mCarControl.getLowSpeedControl(currentHandPlatfrom)))
                            {
                                mCarControl.setBackControl(true,currentHandPlatfrom);
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setLowSpeedControl(true,currentHandPlatfrom);
                            }
                            //清除速度
                            if(mCarControl.getBackStopStatus(currentHandPlatfrom)&&
                               (mCarControl.getBackControl(currentHandPlatfrom)||mCarControl.getHighSpeedControl(currentHandPlatfrom)
                                ||mCarControl.getLowSpeedControl(currentHandPlatfrom)||!mCarControl.getGasBrakeControl(currentHandPlatfrom)))
                            {
                                mCarControl.setBackControl(false,currentHandPlatfrom);
                                mCarControl.setLowSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setHighSpeedControl(false,currentHandPlatfrom);
                                mCarControl.setGasBrakeControl(true,currentHandPlatfrom);
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
     //qDebug()<<strName<<" "<<strPassword;
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
        case StartUp:
             {
                 if(pathStep==0)
                 {
                    pathStep=1;
                 }
                 break;
             }
        case PowerOff:
             {
                 if(pathStep==3)
                 {
                   pathStep=8;
                 }
                 break;
             }
       case Operation:
            {
                if(pathStep==3)
                {
                  pathStep=4;
                }
                break;
            }
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
    //scramOControl.setValue(true,0);
    //bool test[8]={false,false,false,false,false,false,false,false};
    //int postion[8]={6,7,8,9,10,11,12,13};
    //mMotusBasePlc.writePlcIo(test,postion,8,0);
}

//从PLC的选择
void MainWindow::recvPlatfromStatus(int index)
{
    plcIndex=index;
    //qDebug()<<index;
}

//手动按键
void MainWindow::on_handButton_clicked()
{
    ui->m_stackwidget->setCurrentWidget(mMotusHand);
    viewId=2;
    setWindowTitle("手动主页");
    //scramOControl.setValue(false,0);
    //bool test[8]={true,false,true,false,true,false,true,false};
    //int postion[8]={6,7,8,9,10,11,12,13};
    //mMotusBasePlc.writePlcIo(test,postion,8,0);
}

//平台是否能够使用
void MainWindow::recvPlatfromAble(int num ,bool able)
{
    platfromAble[num]=able;
    //qDebug()<<num<<able;
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

//退出按键
void MainWindow::on_exitButton_clicked()
{

}
