#include "motusaflatformstatus.h"
#include "ui_motusaflatformstatus.h"
#include <QDebug>
MotusaFlatformStatus::MotusaFlatformStatus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusaFlatformStatus)
{
    ui->setupUi(this);
    //显示平台的状态
    statusLineEdit=ui->lineEdit_1;
    //PLC是否连接
    plcStatusButton=ui->plcStatus0;
    //主控PLC状态
    mainPushButton[0]=ui->pushButton1; mainPushButton[5]=ui->pushButton6; mainPushButton[10]=ui->pushButton11; mainPushButton[15]=ui->pushButton16;mainPushButton[20]=ui->pushButton21;
    mainPushButton[1]=ui->pushButton2; mainPushButton[6]=ui->pushButton7; mainPushButton[11]=ui->pushButton12; mainPushButton[16]=ui->pushButton17;
    mainPushButton[2]=ui->pushButton3; mainPushButton[7]=ui->pushButton8; mainPushButton[12]=ui->pushButton13; mainPushButton[17]=ui->pushButton18;
    mainPushButton[3]=ui->pushButton4; mainPushButton[8]=ui->pushButton9; mainPushButton[13]=ui->pushButton14; mainPushButton[18]=ui->pushButton19;
    mainPushButton[4]=ui->pushButton5; mainPushButton[9]=ui->pushButton10; mainPushButton[14]=ui->pushButton15;mainPushButton[19]=ui->pushButton20;
}

//析构函数
MotusaFlatformStatus::~MotusaFlatformStatus()
{
    delete ui;
}

//显示平台状态
void MotusaFlatformStatus::viewPlatfromStatus(int status)
{
   statusLineEdit->setText(QString("%1").arg(status));
}

//显示PLC状态
void MotusaFlatformStatus::viewPlcStatus(bool status)
{
    if(!status)
        plcStatusButton->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(85, 85, 0);");
    else
        plcStatusButton->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(0, 255, 0);");
}

//显示主控PLC的状态
void MotusaFlatformStatus::viewMainPlcStatus(bool *status,int num)
{
    for(int i=0;i<num;i++)
    {
        if(i==0||i==3)
        {
            if(!status[i])
                mainPushButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(85,85, 0);");
            else
                mainPushButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(255,0, 0);");
            continue;
        }
        if(!status[i])
            mainPushButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(85, 85, 0);");
        else
            mainPushButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(0, 255, 0);");
    }
}

