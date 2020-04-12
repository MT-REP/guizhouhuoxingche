#ifndef MOTUSPLAYER_H
#define MOTUSPLAYER_H

#include <QObject>
#include "motussocket.h"
class MotusPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MotusPlayer(QObject *parent = nullptr);
    MotusSocket mMotusSocket;       //网络接收对象
    QString recvStr;                //接收数据
    //初始化函数
    void initPara();
    QString getData();
    void sendData(char *data,int lenght);
public slots:
    void recvDataSign(char *data,int lenght,QHostAddress recvRemoteaddr);
};

#endif // MOTUSPLAYER_H
