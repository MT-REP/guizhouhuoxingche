#ifndef MOTUSKEEPTIME_H
#define MOTUSKEEPTIME_H

#include <QObject>
#include <QFile>
class MotusKeepTime : public QObject
{
    Q_OBJECT
public:
    explicit MotusKeepTime(QObject *parent = nullptr);
    ~MotusKeepTime();
    bool readLog();
private:
    QString currentFileName;
    QFile file;                      //文件操作
    void init();
signals:

public slots:
};

#endif // MOTUSKEEPTIME_H
