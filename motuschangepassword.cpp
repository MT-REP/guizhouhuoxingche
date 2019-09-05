#include "motuschangepassword.h"
#include "ui_motuschangepassword.h"
#include <QMessageBox>
MotusChangePassword::MotusChangePassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusChangePassword)
{
    ui->setupUi(this);
}

//析构函数
MotusChangePassword::~MotusChangePassword()
{
    delete ui;
}

//确认按键按下
void MotusChangePassword::on_trueButton_clicked()
{
    oldpassword=ui->oldpasswordlineEdit->text();
    newpassword=ui->newpasswordlineEdit->text();
    againpassword=ui->againpasswordlineEdit->text();
    //原密码密码为空处理
    if(oldpassword.isEmpty())
    {
        QMessageBox::information(this,"友情提示","旧密码不能为空，请输入旧密码。");
        return;
    }
    //新密码为空处理
    if(newpassword.isEmpty())
    {
        QMessageBox::information(this,"友情提示","新密码不能为空，请输入新密码");
        return;
    }
    //确认密码为空处理
    if(againpassword.isEmpty())
    {
        QMessageBox::information(this,"友情提示","确认密码不能为空，请输入确认密码");
        return;
    }
    //两次密码一致
    if(newpassword.compare(againpassword)==0)
    {
         emit sendPassword(oldpassword,newpassword);
         this->close();
    }
    else
    {
        QMessageBox::information(this,"友情提示","两次密码输入不一致");
    }
}
