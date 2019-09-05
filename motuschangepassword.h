#ifndef MOTUSCHANGEPASSWORD_H
#define MOTUSCHANGEPASSWORD_H

#include <QDialog>

namespace Ui {
class MotusChangePassword;
}

class MotusChangePassword : public QDialog
{
    Q_OBJECT

public:
    explicit MotusChangePassword(QWidget *parent = 0);
    ~MotusChangePassword();
signals:
    void sendPassword(QString oldPassword,QString newPassword);
private slots:
    void on_trueButton_clicked();
private:
    Ui::MotusChangePassword *ui;
    QString oldpassword;
    QString newpassword;
    QString againpassword;
};

#endif // MOTUSCHANGEPASSWORD_H
