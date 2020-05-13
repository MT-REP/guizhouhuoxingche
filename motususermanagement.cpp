#include "motususermanagement.h"
#include "ui_motususermanagement.h"
#include "motuschangepassword.h"
#include "motuslevelswitch.h"
#include "motususerlogin.h"
#include <QTextCodec>
#include <QDir>
#include <QMessageBox>
#include "qdebug.h"
MotusUserManagement::MotusUserManagement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusUserManagement)
{
    ui->setupUi(this);
    //读取配置文件
    readuserfile("user.ini");
}

//析构函数
MotusUserManagement::~MotusUserManagement()
{
    //写配置文件
    writeuserfile("user.ini");
    delete ui;
}

//更改密码
void MotusUserManagement::on_changePasswordButton_clicked()
{
    static int isShow=0;
    if(isShow==1)
        return;
    isShow=1;
    MotusChangePassword mMotusChangePassword(this);//定义密码修改子窗口对象
    connect(&mMotusChangePassword,SIGNAL(sendPassword(QString,QString)),this,SLOT(recvPassword(QString,QString)));
    mMotusChangePassword.show();//显示窗口
    mMotusChangePassword.exec();//循环等待消息
    isShow=0;

}

//接收用户旧密码新密码
void MotusUserManagement::recvPassword(QString oldPassword,QString newPassword)
{
    //检测是否和当前密码一致
    if(currentUser.userPassword.compare(oldPassword)==0)
    {
        for ( int i=0; i!=m_mianUser.size(); ++i )
        {
            if(m_mianUser.at(i).userName.compare(currentUser.userName)==0)
            {
                m_mianUser.removeAt(i);
                break;
            }
        }
        currentUser.userPassword=newPassword;
        m_mianUser.append(currentUser);
        writeuserfile("user.ini");
        QMessageBox::information(this,"友情提示","密码修改成功");
    }
    else
    {
        QMessageBox::information(this,"友情提示","旧密码输入错误\r\n密码修改失败");
    }
}

//用户等级切换切换
void MotusUserManagement::on_userSwitchButton_clicked()
{
    static int isShow=0;
    if(isShow==1)
        return;
    isShow=1;
    MotusLevelSwitch mMotusLevelSwitch;//定义用户等级切换子窗口对象
    connect(&mMotusLevelSwitch,SIGNAL(sendUserLevel(int)),this,SLOT(recvUserLevel(int)));
    mMotusLevelSwitch.show();//显示窗口
    mMotusLevelSwitch.exec();//循环等待消息
    isShow=0;
}

//接收用户等级
void MotusUserManagement::recvUserLevel(int Level)
{
    if(Level<=currentDegree)
    {
        currentDegree=Level;          //更改当前用户等级
        setbuttontext(currentDegree); //设置用户等级图标
        emit sendLevelChange(currentDegree); //发送用户等级改变信息
        //QMessageBox::information(this,"友情提示","切换用户身份为操作员");
    }
    else
    {
        showLoginDialogClose();
    }
}

//自用登录函数
void MotusUserManagement::showLoginDialogClose()
{
    static int isShow=0;
    if(isShow==1)
        return;
    isShow=1;
    MotusUserLogin mMotusUserLogin(this,0);//
    connect(&mMotusUserLogin,SIGNAL(sendNameAndPassword(QString,QString)),this,SLOT(recvLogin(QString,QString)));
    connect(this,SIGNAL(sendLoginCode(int)),&mMotusUserLogin,SLOT(dealRecvMessage(int)));
    mMotusUserLogin.show();//显示窗口
    mMotusUserLogin.exec();//循环等待消息
    isShow=0;
}

//登录界面显示
void MotusUserManagement::showLoginDialog()
{
    static int isShow=0;
    if(isShow==1)
        return;
    isShow=1;
    MotusUserLogin mMotusUserLogin(this,1);//
    connect(&mMotusUserLogin,SIGNAL(sendNameAndPassword(QString,QString)),this,SLOT(recvLogin(QString,QString)));
    connect(this,SIGNAL(sendLoginCode(int)),&mMotusUserLogin,SLOT(dealRecvMessage(int)));
    mMotusUserLogin.show();//显示窗口
    mMotusUserLogin.exec();//循环等待消息
    isShow=0;
}

//登录子窗口处理信息
void MotusUserManagement::recvLogin(QString strName,QString strPassword)
{
    //qDebug()<<strName;
    //qDebug()<<strPassword;
    for ( int i=0; i!=m_mianUser.size(); ++i )
    {
        if(m_mianUser.at(i).userName.compare(strName)==0)
        {
            if(m_mianUser.at(i).userPassword.compare(strPassword)==0)
            {
                currentUser.userName=strName;
                currentUser.userDegree=m_mianUser.at(i).userDegree;
                currentDegree=m_mianUser.at(i).userDegree;
                currentUser.userPassword=strPassword;
                setbuttontext(currentDegree);
                emit sendLevelChange(currentDegree);
                emit sendLoginCode(0);
                return;
            }
        }
    }
    emit sendLoginCode(1);
}

//用户等级改变
void MotusUserManagement::setbuttontext(int lever)
{
    switch(lever)
    {
    case 1:
        {
            ui->userLabel->setStyleSheet("border-image: url(:/picture/caozuoyuan.PNG);");
            break;
        }
    case 2:
        {
             ui->userLabel->setStyleSheet("border-image: url(:/picture/guanliyuan.PNG);");
            break;
        }
    default:break;
    }
}

//用户账户的读取函数
bool MotusUserManagement::readuserfile(QString filename)//读取用户数据
{
    //QDir dir;
    //qDebug()<<dir.currentPath();
    QString str="config\\";
    str+=filename;
    QFile file;
    file.setFileName(str);
    if(file.exists())
    {
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        while(!in.atEnd())
        {
            M_UserPermission tempuser;
            QString tempstr;
            tempstr.clear();
            in>>tempstr;//读取用户名
            tempuser.userName=getXorEncryptDecrypt(tempstr,11);
            //qDebug()<<tempuser.userName;
            in>>tempuser.userDegree;//读取用户等级
            //qDebug()<<tempuser.userDegree;
            tempstr.clear();
            in>>tempstr;//读取用户密码
            tempuser.userPassword=getXorEncryptDecrypt(tempstr,11);
            //qDebug()<<tempuser.userPassword;
            m_mianUser.append(tempuser);
        }
        file.close();
        return true;
    }else
    {
        return false;
    }
}

//用户账户的写入
bool MotusUserManagement::writeuserfile(QString filename)//写用户数据
{
    QString str="config\\";
    str+=filename;
    QFile file;
    file.setFileName(str);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate);
    QDataStream out(&file);
//    QString tempstr;
//    tempstr.clear();
//    tempstr=getXorEncryptDecrypt("motus",11);
//    out<<tempstr;//写入用户名
//    out<<2;//写入用户等级
//    tempstr.clear();
//    tempstr=getXorEncryptDecrypt("motus",11);
//    out<<tempstr;//写入用户密码
//    tempstr=getXorEncryptDecrypt("donge",11);
//    out<<tempstr;//写入用户名
//    out<<1;//写入用户等级
//    tempstr.clear();
//    tempstr=getXorEncryptDecrypt("donge",11);
//    out<<tempstr;//写入用户密码
    for ( int i=0; i!=m_mianUser.size(); ++i )
    {
         QString tempstr;
         tempstr.clear();
         //qDebug()<<m_mianUser.at(i).userName;
         tempstr=getXorEncryptDecrypt(m_mianUser.at(i).userName,11);
         out<<tempstr;//写入用户名
         out<<m_mianUser.at(i).userDegree;//写入用户等级
         //qDebug()<<m_mianUser.at(i).userDegree;
         tempstr.clear();
         //qDebug()<<m_mianUser.at(i).userPassword;
         tempstr=getXorEncryptDecrypt(m_mianUser.at(i).userPassword,11);
         out<<tempstr;//写入用户密码
    }
    file.close();
    return true;
}

//文件的加密
QString MotusUserManagement::getXorEncryptDecrypt(const QString &str, const char &key)
{
     QString result;
     QByteArray bs = qstringToByte(str);
     for(int i=0; i<bs.size(); i++)
     {
         bs[i] = bs[i] ^ key;
     }
      result = byteToQString(bs);
     return result;
}

//字节转QString
QString MotusUserManagement::byteToQString(const QByteArray &byte)
{
     QString result;
    if(byte.size() > 0)
    {
      QTextCodec *codec = QTextCodec::codecForName("utf-8");
      result = codec->toUnicode(byte);
    }
     return result;
}

//QString转字节
QByteArray MotusUserManagement::qstringToByte(const QString &strInfo)
{
  QByteArray result;
  if(strInfo.length() > 0)
  {
  QTextCodec *codec = QTextCodec::codecForName("utf-8");
  result = codec->fromUnicode(strInfo);
  }
  return result;
}



