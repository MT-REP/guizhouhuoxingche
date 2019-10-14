#include "motushand.h"
#include "ui_motushand.h"
#include "mainwindow.h"
#include "QMessageBox"
#include "qdebug.h"
MotusHand::MotusHand(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusHand)
{
    ui->setupUi(this);
    staticPermission=false;
    autoPermission=true;
    platfromIndex=1;
}

//析构函数
MotusHand::~MotusHand()
{
    delete ui;
}

//手动按键
void MotusHand::on_handButton_clicked()
{
    static int flag=0;
    if(autoPermission)
    {
        if(flag==0)
        {
            ui->handButton->setText("手动");
            staticPermission=true;
            flag=1;
            //主循环命令改为手动
            emit sendMainWindoewCmd(0,HandMove,0);
        }
        else
        {
            ui->handButton->setText("自动");
            staticPermission=false;
            flag=0;
            //主循环命令改为自动
            emit sendMainWindoewCmd(0,Automatic,0);
        }
    }
    else
    {
        QMessageBox::information(NULL,"友情提示","系统不在安全状态，\r\n不能切换换状态。");
    }
}

//接收手动权限
void MotusHand::recvHandPermissin(bool able)
{
    //是否可以点击手动按键
    autoPermission=able;
}

////////////////////////////////主plc手动功能//////////////////////////////////
//频闪特效
void MotusHand::on_strobeEffect_clicked()
{
    static bool ret=false;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(0,StrobeEffect,2,!ret);
    ret=!ret;
}
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////禁用平台功能//////////////////////////////////
//禁用平台1
void MotusHand::on_disablePlatfrom1_clicked()
{
    static int flag=0;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    if(flag==0)
    {
        ui->disablePlatfrom1->setStyleSheet("background-color: rgb(255, 85, 0);");
        sendPlatfromAble(0,false);
        flag=1;
    }
    else
    {
        ui->disablePlatfrom1->setStyleSheet("background-color: rgb(170, 255, 0);");
        sendPlatfromAble(0,true);
        flag=0;
    }
}

//禁用平台2
void MotusHand::on_disablePlatfrom2_clicked()
{
    static int flag=0;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    if(flag==0)
    {
        ui->disablePlatfrom2->setStyleSheet("background-color: rgb(255, 85, 0);");
        sendPlatfromAble(1,false);
        flag=1;
    }
    else
    {
        ui->disablePlatfrom2->setStyleSheet("background-color: rgb(170, 255, 0);");
        sendPlatfromAble(1,true);
        flag=0;
    }
}

//禁用平台3
void MotusHand::on_disablePlatfrom3_clicked()
{
    static int flag=0;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    if(flag==0)
    {
        ui->disablePlatfrom3->setStyleSheet("background-color: rgb(255, 85, 0);");
        sendPlatfromAble(2,false);
        flag=1;
    }
    else
    {
        ui->disablePlatfrom3->setStyleSheet("background-color: rgb(170, 255, 0);");
        sendPlatfromAble(2,true);
        flag=0;
    }
}

//禁用平台4
void MotusHand::on_disablePlatfrom4_clicked()
{
    static int flag=0;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    if(flag==0)
    {
        ui->disablePlatfrom4->setStyleSheet("background-color: rgb(255, 85, 0);");
        sendPlatfromAble(3,false);
        flag=1;
    }
    else
    {
        ui->disablePlatfrom4->setStyleSheet("background-color: rgb(170, 255, 0);");
        sendPlatfromAble(3,true);
        flag=0;
    }
}

//禁用平台5
void MotusHand::on_disablePlatfrom5_clicked()
{
    static int flag=0;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    if(flag==0)
    {
        ui->disablePlatfrom5->setStyleSheet("background-color: rgb(255, 85, 0);");
         sendPlatfromAble(4,false);
        flag=1;
    }
    else
    {
        ui->disablePlatfrom5->setStyleSheet("background-color: rgb(170, 255, 0);");
        sendPlatfromAble(4,true);
        flag=0;
    }
}

//禁用平台6
void MotusHand::on_disablePlatfrom6_clicked()
{
    static int flag=0;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    if(flag==0)
    {
        ui->disablePlatfrom6->setStyleSheet("background-color: rgb(255, 85, 0);");
        sendPlatfromAble(5,false);
        flag=1;
    }
    else
    {
        ui->disablePlatfrom6->setStyleSheet("background-color: rgb(170, 255, 0);");
        sendPlatfromAble(5,true);
        flag=0;
    }
}

/////////////////////////////////////////////////////////////////////////////


////////////////////////////从PLC的功能///////////////////////////////////////////
//当前索引值
void MotusHand::on_comboBox_currentIndexChanged(int index)
{
    platfromIndex=index+1;
}

//护栏升
void MotusHand::on_guardBarUp_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,GuardBarUp,1);
}

//护栏降
void MotusHand::on_guardBarDown_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,GuardBarDown,1);
}

//待客指示灯
void MotusHand::on_waitingLight_clicked()
{
    static bool ret=false;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,WaittingCustomer,1,!ret);
    ret=!ret;
}

//小车前进
void MotusHand::on_carFront_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,CarFront,1);
}

//小车后退
void MotusHand::on_carBack_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,CarBack,1);
}

//平台上升
void MotusHand::on_platfromUp_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
     emit sendMainWindoewCmd(platfromIndex,PlatfromUp,1);
}

//平台下降
void MotusHand::on_platfromDown_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
     emit sendMainWindoewCmd(platfromIndex,PlatfromDown,1);
}

//冷风特效
void MotusHand::on_coldWind_clicked()
{
    static bool ret=false;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,ColdWind,1,!ret);
    ret=!ret;
}

//喷水特效
void MotusHand::on_sprayWater_clicked()
{
    static bool ret=false;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,SprayWater,1,!ret);
    ret=!ret;
}

//座椅照明
void MotusHand::on_seatLight_clicked()
{
    static bool ret=false;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,SeatLight,1,!ret);
    ret=!ret;
}

//输出清除
void MotusHand::on_outputClear_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    emit sendMainWindoewCmd(platfromIndex,OutputClear,1);
}
