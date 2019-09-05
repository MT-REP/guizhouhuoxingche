#ifndef MOTUSRUNLOG_H
#define MOTUSRUNLOG_H

#include <QDialog>
#include <QString>
#include <QFile>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QModelIndex>

#define ROW 20

//信息记录结构体
typedef struct errorRecord
{
    unsigned int month;//事件月份
    QString strTime;   //事件时间
    QString strEvent;  //事件
    errorRecord()
    {
       month=0;
    }
}ErrorRecord;

namespace Ui
{
    class MotusRunLog;
}

class MotusRunLog : public QDialog
{
    Q_OBJECT
public:
    explicit MotusRunLog(QWidget *parent = 0);
    ~MotusRunLog();
    void init();
    void addStringLog(QString strLog);
    void writeLog();
    void readLog();
private:
    Ui::MotusRunLog *ui;
    QString currentFileName;         //记录信息目录
    QFile file;                      //文件操作
    QList<ErrorRecord>merrorRecord;  //程序运行记录
    int m_row;                       //函数
    QStandardItem *m_item[ROW][2];   //数据
    QStandardItemModel *m_ErrorRecordModel;  //错误文件记录模式    
private slots:
    void on_nextButton_clicked();
    void on_previousButton_clicked();
};









#endif // MOTUSRUNLOG_H
