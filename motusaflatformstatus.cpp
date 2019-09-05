#include "motusaflatformstatus.h"
#include "ui_motusaflatformstatus.h"
#include <QDebug>
MotusaFlatformStatus::MotusaFlatformStatus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusaFlatformStatus)
{
    ui->setupUi(this);
    //显示平台的状态
    statusLineEdit[0]=ui->lineEdit_1; statusLineEdit[1]=ui->lineEdit_2; statusLineEdit[2]=ui->lineEdit_3;
    statusLineEdit[3]=ui->lineEdit_4; statusLineEdit[4]=ui->lineEdit_5; statusLineEdit[5]=ui->lineEdit_6;
    //主控PLC状态
    mainPushButton[0]=ui->pushButton_1; mainPushButton[5]=ui->pushButton_6; mainPushButton[10]=ui->pushButton_11;
    mainPushButton[1]=ui->pushButton_2; mainPushButton[6]=ui->pushButton_7; mainPushButton[11]=ui->pushButton_12;
    mainPushButton[2]=ui->pushButton_3; mainPushButton[7]=ui->pushButton_8; mainPushButton[12]=ui->pushButton_13;
    mainPushButton[3]=ui->pushButton_4; mainPushButton[8]=ui->pushButton_9; mainPushButton[13]=ui->pushButton_14;
    mainPushButton[4]=ui->pushButton_5; mainPushButton[9]=ui->pushButton_10;
    //从PLC状态
    fromPushButton[0]=ui->pButton_1;  fromPushButton[12]=ui->pButton_13; fromPushButton[24]=ui->pButton_25; fromPushButton[36]=ui->pButton_37;
    fromPushButton[1]=ui->pButton_2;  fromPushButton[13]=ui->pButton_14; fromPushButton[25]=ui->pButton_26; fromPushButton[37]=ui->pButton_38;
    fromPushButton[2]=ui->pButton_3;  fromPushButton[14]=ui->pButton_15; fromPushButton[26]=ui->pButton_27; fromPushButton[38]=ui->pButton_39;
    fromPushButton[3]=ui->pButton_4;  fromPushButton[15]=ui->pButton_16; fromPushButton[27]=ui->pButton_28; fromPushButton[39]=ui->pButton_40;
    fromPushButton[4]=ui->pButton_5;  fromPushButton[16]=ui->pButton_17; fromPushButton[28]=ui->pButton_29; fromPushButton[40]=ui->pButton_41;
    fromPushButton[5]=ui->pButton_6;  fromPushButton[17]=ui->pButton_18; fromPushButton[29]=ui->pButton_30; fromPushButton[41]=ui->pButton_42;
    fromPushButton[6]=ui->pButton_7;  fromPushButton[18]=ui->pButton_19; fromPushButton[30]=ui->pButton_31; fromPushButton[42]=ui->pButton_43;
    fromPushButton[7]=ui->pButton_8;  fromPushButton[19]=ui->pButton_20; fromPushButton[31]=ui->pButton_32; fromPushButton[43]=ui->pButton_44;
    fromPushButton[8]=ui->pButton_9;  fromPushButton[20]=ui->pButton_21; fromPushButton[32]=ui->pButton_33; fromPushButton[44]=ui->pButton_45;
    fromPushButton[9]=ui->pButton_10; fromPushButton[21]=ui->pButton_22; fromPushButton[33]=ui->pButton_34; fromPushButton[45]=ui->pButton_46;
    fromPushButton[10]=ui->pButton_11;fromPushButton[22]=ui->pButton_23; fromPushButton[34]=ui->pButton_35; fromPushButton[46]=ui->pButton_47;
    fromPushButton[11]=ui->pButton_12;fromPushButton[23]=ui->pButton_24; fromPushButton[35]=ui->pButton_36; fromPushButton[47]=ui->pButton_48;
    //
    plcStatusButton[0]=ui->plcStatus0;plcStatusButton[1]=ui->plcStatus1;plcStatusButton[2]=ui->plcStatus2;
    plcStatusButton[3]=ui->plcStatus3;plcStatusButton[4]=ui->plcStatus4;plcStatusButton[5]=ui->plcStatus5;
    plcStatusButton[6]=ui->plcStatus6;
}

//析构函数
MotusaFlatformStatus::~MotusaFlatformStatus()
{
    delete ui;
}

//显示平台状态
void MotusaFlatformStatus::viewPlatfromStatus(int status[6])
{
    for(int i=0;i<6;i++)
    {
        statusLineEdit[i]->setText(QString("%1").arg(status[i]));
    }
}

//显示PLC状态
void MotusaFlatformStatus::viewPlcStatus(bool status[7],int length)
{
    for(int i=0;i<length;i++)
    {
        if(!status[i])
            plcStatusButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(85, 85, 0);");
        else
            plcStatusButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(0, 255, 0);");
    }
}

//显示主控PLC的状态
void MotusaFlatformStatus::viewMainPlcStatus(bool status[14],int num)
{
    for(int i=0;i<num;i++)
    {
        if(!status[i])
            mainPushButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(85, 85, 0);");
        else
            mainPushButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(0, 255, 0);");
    }
}

//显示从PLC的状态
void MotusaFlatformStatus::viewFromPlcStatus(bool status[48],int num)
{
    for(int i=0;i<num;i++)
    {
        if(!status[i])
            fromPushButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(85, 85, 0);");
        else
            fromPushButton[i]->setStyleSheet("border:5px groove gray;border-radius:15px;background-color: rgb(0, 255, 0);");
    }
}

//平台切换
void MotusaFlatformStatus::on_comboBox_currentIndexChanged(int index)
{
    emit platfromSwitch(index+1);
}
