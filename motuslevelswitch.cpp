#include "motuslevelswitch.h"
#include "ui_motuslevelswitch.h"

//构造函数
MotusLevelSwitch::MotusLevelSwitch(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusLevelSwitch)
{
    ui->setupUi(this);
    setWindowTitle("用户切换界面");
}

//析构函数
MotusLevelSwitch::~MotusLevelSwitch()
{
    delete ui;
}

//发送用户等级1
void MotusLevelSwitch::on_operatorButton_clicked()
{
    emit sendUserLevel(1);
    close();
}

//发送用户等级2
void MotusLevelSwitch::on_AdministratorButton_clicked()
{
    emit sendUserLevel(2);
    close();
}
