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
    void viewPlatfromStatus(int status);
    void viewPlcStatus(bool status);
    void viewMainPlcStatus(bool *status,int num);
    ~MotusaFlatformStatus();
private:
    Ui::MotusaFlatformStatus *ui;
    QLineEdit *statusLineEdit;
    QPushButton *plcStatusButton;
    QPushButton *mainPushButton[19];
};

#endif // MOTUSAFLATFORMSTATUS_H
