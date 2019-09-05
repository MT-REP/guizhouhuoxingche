#ifndef MOTUSLEVELSWITCH_H
#define MOTUSLEVELSWITCH_H

#include <QDialog>

namespace Ui {
class MotusLevelSwitch;
}

class MotusLevelSwitch : public QDialog
{
    Q_OBJECT

public:
    explicit MotusLevelSwitch(QWidget *parent = 0);
    ~MotusLevelSwitch();
signals:
    void sendUserLevel(int Level);
private slots:
    //操作员按键
    void on_operatorButton_clicked();
    //管理员按键
    void on_AdministratorButton_clicked();

private:
    Ui::MotusLevelSwitch *ui;
};

#endif // MOTUSLEVELSWITCH_H
