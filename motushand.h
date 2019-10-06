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
    //平台是否可用
    void sendPlatfromAble(int num,bool able);
    //发送主窗口命令
    void sendMainWindoewCmd(int num,int cmd,int main,bool ret=true);
public slots:
    void recvHandPermissin(bool able);
private slots:
    //禁用平台1
    void on_disablePlatfrom1_clicked();
    //禁用平台2
    void on_disablePlatfrom2_clicked();
    //禁用平台3
    void on_disablePlatfrom3_clicked();
    //禁用平台4
    void on_disablePlatfrom4_clicked();
    //禁用平台5
    void on_disablePlatfrom5_clicked();
    //禁用平台6
    void on_disablePlatfrom6_clicked();
    //手动按键
    void on_handButton_clicked();
    //平台调试切换
    void on_comboBox_currentIndexChanged(int index);

    //护栏上
    void on_guardBarUp_clicked();
    //护栏降
    void on_guardBarDown_clicked();
    //小车前进
    void on_carFront_clicked();
    //小车后退
    void on_carBack_clicked();
    //平台升
    void on_platfromUp_clicked();
    //平台将
    void on_platfromDown_clicked();
    //冷风特效
    void on_coldWind_clicked();
    //喷水特效
    void on_sprayWater_clicked();
    //座椅照明
    void on_seatLight_clicked();
    //小车输出关闭
    void on_outputClear_clicked();
    //喷雾特效
    void on_smokeEffect_clicked();
    //频闪特效
    void on_strobeEffect_clicked();
    //泡泡特效
    void on_hubbleEffect_clicked();
    void on_waitingLight_clicked();

private:
    Ui::MotusHand *ui;
    bool staticPermission;//私有权限
    bool autoPermission;  //外部权限
    int platfromIndex;    //平台索引
};

#endif // MOTUSHAND_H
