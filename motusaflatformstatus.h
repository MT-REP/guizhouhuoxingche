#ifndef MOTUSAFLATFORMSTATUS_H
#define MOTUSAFLATFORMSTATUS_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class MotusaFlatformStatus;
}

class MotusaFlatformStatus : public QDialog
{
    Q_OBJECT

public:
    explicit MotusaFlatformStatus(QWidget *parent = 0);
    void viewPlatfromStatus(int status[6]);
    void viewPlcStatus(bool status[7],int length);
    void viewMainPlcStatus(bool status[13],int num);
    void viewFromPlcStatus(bool status[13],int num);
    ~MotusaFlatformStatus();
signals:
    void platfromSwitch(int platfromSwitch);
private slots:
    //当前索引状态改变
    void on_comboBox_currentIndexChanged(int index);
private:
    Ui::MotusaFlatformStatus *ui;
    QLineEdit *statusLineEdit[6];
    QPushButton *plcStatusButton[7];
    QPushButton *mainPushButton[14];
    QPushButton *fromPushButton[48];
};

#endif // MOTUSAFLATFORMSTATUS_H
