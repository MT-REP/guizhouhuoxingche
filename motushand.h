#ifndef MOTUSHAND_H
#define MOTUSHAND_H

#include <QDialog>

namespace Ui {
class MotusHand;
}

class MotusHand : public QDialog
{
    Q_OBJECT

public:
    explicit MotusHand(QWidget *parent = 0);
    ~MotusHand();
signals:
    //发送安全带是否可用
    void sendSafetyBeltAble(bool able);
    //发送主窗口命令
    void sendMainWindoewCmd(int cmd,int ma,bool ret=true);
public slots:
    void recvHandPermissin(bool able);
private slots:
    //禁用安全带
    void on_disableSafetyBelt_clicked();
    //手动按键
    void on_handButton_clicked();
    //平台升
    void on_platfromUp_clicked();
    //平台降
    void on_platfromDown_clicked();
    //照明
    void on_illumination_clicked();
    //风扇
    void on_fan_clicked();
private:
    Ui::MotusHand *ui;
    bool staticPermission;//私有权限开启可以操作界面按键
    bool autoPermission;  //外部权限开启可以改变私有权限
};

#endif // MOTUSHAND_H
