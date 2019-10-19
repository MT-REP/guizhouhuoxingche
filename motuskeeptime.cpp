#include "motuskeeptime.h"
#include <QDateTime>
#include <QTextStream>
MotusKeepTime::MotusKeepTime(QObject *parent) : QObject(parent)
{
    init();
}

//析构函数
MotusKeepTime::~MotusKeepTime()
{
}

//初始化文件
//没有则创建数据文件
//有怎不创建数据文件
void MotusKeepTime::init()
{
    currentFileName = "data\\";
    currentFileName += "time.data";
    file.setFileName(currentFileName);
    if(!file.exists ())
    {
         file.open(QIODevice::ReadWrite|QIODevice::Text);
         file.close();
    }
}

//读事件函数
bool MotusKeepTime::readLog(void)
{
    file.setFileName(currentFileName);
    if(file.exists ())
    {
         unsigned int time=0;
         QString tempStr;
         QDateTime current_date_time = QDateTime::currentDateTime();
         //得到年数
         tempStr.clear();
         tempStr=current_date_time.toString("yyyy");
         time=tempStr.toInt();
         //得到总月份
         tempStr.clear();
         tempStr=current_date_time.toString("MM");
         time=time*12+tempStr.toInt();

         file.open(QIODevice::ReadOnly);
         QTextStream out(&file);
         while(!out.atEnd())
         {
             QString  tempstr;
             QStringList  tempstrlist;
             tempstr=out.readLine();
             tempstrlist=tempstr.split (",");
             if(tempstrlist.isEmpty())
             {
                file.close();
                return false;
             }
             int recordTime=tempstrlist.at(0).toInt();
             int openClose=tempstrlist.at(1).toInt();
             if(openClose==0)
             {
                 file.close();
                 return true;
             }
             if(recordTime-time>0)
             {
                 file.close();
                 return  true;
             }
             file.close();
             return  false;
         }
    }
    return false;
}
