#ifndef MOTUSUSERLOGIN_H
#define MOTUSUSERLOGIN_H

#include <QDialog>
#include <QCloseEvent>
namespace Ui {
class MotusUserLogin;
}

class MotusUserLogin : public QDialog
{
    Q_OBJECT

public:
    explicit MotusUserLogin(QWidget *parent = 0,int code=1);
    ~MotusUserLogin();
signals:
    void sendNameAndPassword(QString name,QString password);
public slots:
    void dealRecvMessage(int recvCode);
private slots:
    void on_okButton_clicked();
private:
    Ui::MotusUserLogin *ui;
    QString strName;
    QString strPassword;
    int code;
    void closeEvent(QCloseEvent *event);
};

#endif // MOTUSUSERLOGIN_H
