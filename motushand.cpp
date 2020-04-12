#include "motushand.h"
#include "ui_motushand.h"
#include "mainwindow.h"
#include "QMessageBox"
#include "qdebug.h"
//构造函数
MotusHand::MotusHand(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusHand)
{
    ui->setupUi(this);
    staticPermission=false;
    autoPermission=true;
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
            emit sendMainWindoewCmd(HandMove,0);
        }
        else
        {
            ui->handButton->setText("自动");
            staticPermission=false;
            flag=0;
            //主循环命令改为自动
            emit sendMainWindoewCmd(Automatic,0);
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

//禁用安全带
void MotusHand::on_disableSafetyBelt_clicked()
{
    static int flag=0;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
    if(flag==0)
    {
        ui->disableSafetyBelt->setStyleSheet("background-color: rgb(255, 85, 0);");
        emit sendSafetyBeltAble(false);
        flag=1;
    }
    else
    {
        ui->disableSafetyBelt->setStyleSheet("background-color: rgb(170, 255, 0);");
        emit sendSafetyBeltAble(true);
        flag=0;
    }
}

//平台上升
void MotusHand::on_platfromUp_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
     emit sendMainWindoewCmd(PlatfromUp,1);
}

//平台下降
void MotusHand::on_platfromDown_clicked()
{
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
     emit sendMainWindoewCmd(PlatfromDown,1);
}

//室内照明
void MotusHand::on_illumination_clicked()
{
    static bool actionret = true;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
     emit sendMainWindoewCmd(SeatLight,1,actionret);
     actionret = !actionret;
}

//风扇
void MotusHand::on_fan_clicked()
{
    static bool actionret = true;
    if(!staticPermission)
    {
        QMessageBox::information(NULL,"友情提示","请在手动模式下操作");
        return;
    }
     emit sendMainWindoewCmd(ColdWind,1,actionret);
    actionret = !actionret;

}








