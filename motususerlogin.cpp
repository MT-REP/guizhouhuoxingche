#include "motususerlogin.h"
#include "ui_motususerlogin.h"
#include "qmessagebox.h"
//#include <QDebug>
MotusUserLogin::MotusUserLogin(QWidget *parent,int code) :
    QDialog(parent),
    ui(new Ui::MotusUserLogin)
{
    ui->setupUi(this);
    this->code=code;
    setWindowTitle("登录界面");
    //qDebug()<<this->code;
}

//析构函数
MotusUserLogin::~MotusUserLogin()
{
    delete ui;
}

//登录按键
void MotusUserLogin::on_okButton_clicked()
{
    //得到按键上的值
    strName=ui->userNameLineEdit->text();
    strPassword=ui->userPassLineEdit->text();
    //用户名不能为空
    if(strName.isEmpty())
    {
       QMessageBox::information(this,"友情提示","用户名不能为空,\r\n请输入用户名。");
       return;
    }
    //密码不能为空
    if(strPassword.isEmpty())
    {
       QMessageBox::information(this,"友情提示","密码不能为空,\r\n请输入密码。");
       return;
    }
    //发送数值
    emit sendNameAndPassword(strName,strPassword);
}

//错误代码处理函数
void MotusUserLogin::dealRecvMessage(int recvCode)
{
    switch (recvCode)
    {
        case 0:
            code=0;
            close();
            break;
        case 1:
            code=1;
            QMessageBox::information(this,"友情提示","用户名或密码错误");
            break;
        default:
            code=1;
            break;
    }
}

//关闭处理函数
void MotusUserLogin::closeEvent(QCloseEvent *event)
{
    if(code==1)
        event->ignore();
}
