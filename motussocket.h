#ifndef MOTUSSOCKET_H
#define MOTUSSOCKET_H

#include <QObject>
#include <QUdpSocket>

#define MaxNum 2

class MotusSocket : public QObject
{
    Q_OBJECT
public:
    explicit MotusSocket(QObject *parent = nullptr);//构造函数
    ~MotusSocket();//析构函数
    bool initSocket(int bindPort);//绑定端口
    void setRemoteIpAndPort(QString remoteIP,int remotePort,int index);//设置远程IP和端口
    void sendData(char data[],int length,int index);
private:
    QUdpSocket *motusUdpServer;        //网络接收Socket
    QHostAddress *remoteaddr[MaxNum];  //远程地址
    int remotePort[MaxNum];            //远程地址端口
    QHostAddress recvRemoteaddr;       //接收IP地址
    bool isInit;                       //是否初始化完成
    void closeSocket();                //关闭Socket
signals:
    void sendDataSign(char *data,int lenght,QHostAddress recvRemoteaddr);
public slots:
    void readPendingDatagrams();
};

#endif // MOTUSSOCKET_H
