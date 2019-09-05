#ifndef MOTUSUSERMANAGEMENT_H
#define MOTUSUSERMANAGEMENT_H

#include <QDialog>
#include <QList>
typedef struct UserPermission
{
    QString userName;    //用户名字
    int  userDegree;     //用户等级
    QString userPassword;//用户密码
    UserPermission()
    {
       userName="";
       userDegree=0;
       userPassword="";
    }
}M_UserPermission;


namespace Ui {
class MotusUserManagement;
}

class MotusUserManagement : public QDialog
{
    Q_OBJECT

public:
    explicit MotusUserManagement(QWidget *parent = 0);
    ~MotusUserManagement();
    void showLoginDialog();
signals:
    //用户等级改变信号
    void sendLevelChange(int Level);
    void sendLoginCode(int code);
private slots:
    //密码改变按键
    void on_changePasswordButton_clicked();
    //用户切换按键
    void on_userSwitchButton_clicked();
    //接收用户旧密码新密码
    void recvPassword(QString oldPassword,QString newPassword);
    //接收用户等级
    void recvUserLevel(int Level);
    //接收用户名和用户密码
    void recvLogin(QString strName,QString strPassword);
private:
    Ui::MotusUserManagement *ui;
    int currentDegree;                //当前用户等级
    M_UserPermission currentUser;     //当前用户
    QList<M_UserPermission>m_mianUser;//存放用户链表
    void setbuttontext(int lever);
    QByteArray qstringToByte(const QString &strInfo);
    QString byteToQString(const QByteArray &byte);
    QString getXorEncryptDecrypt(const QString &str, const char &key);
    bool writeuserfile(QString filename);
    bool readuserfile(QString filename);//读取用户数据
    void showLoginDialogClose();
};

#endif // MOTUSUSERMANAGEMENT_H
