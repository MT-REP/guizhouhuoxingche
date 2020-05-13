#include "motusrunlog.h"
#include "ui_motusrunlog.h"
#include <QDateTime>
#include <QTextStream>
#include<QAbstractItemView>
#include<QHeaderView>
//#include <QDir>
#include <QDebug>
MotusRunLog::MotusRunLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusRunLog)
{
    ui->setupUi(this);
    //tableViewc初始化
    m_ErrorRecordModel = new QStandardItemModel(this);
    m_ErrorRecordModel->setHorizontalHeaderItem(0, new QStandardItem("记录时间"));
    m_ErrorRecordModel->setHorizontalHeaderItem(1, new QStandardItem("记录事件"));
    ui->errorRecordTableView->setModel(m_ErrorRecordModel);
    ui->errorRecordTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->errorRecordTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->errorRecordTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->errorRecordTableView->verticalHeader()->hide();
    ui->errorRecordTableView->setAlternatingRowColors(true);
    //创建数据文件
    init();
    //读取数据文件
    readLog();
    //行数初始化
    m_row=0;
    for(int i=0;i<ROW;i++)
    {
        for(int j=0;j<2;j++)
        {
            m_item[i][j]=new QStandardItem ("");
            m_ErrorRecordModel->setItem(i,j,m_item[i][j]);
        }
    }
    for(int i=0;i<ROW&&i<merrorRecord.size();i++)
    {
        m_item[i][0]->setText(merrorRecord.at(i).strTime);
        m_item[i][1]->setText(merrorRecord.at(i).strEvent);
        m_row+=1;
    }
}

//析构函数
MotusRunLog::~MotusRunLog()
{
    //写事件
    writeLog();
    delete ui;
}

//初始化文件
//没有则创建数据文件
//有怎不创建数据文件
void MotusRunLog::init()
{
    currentFileName = "data\\";
    currentFileName += "error.data";
    file.setFileName(currentFileName);
    if(!file.exists ())
    {
         file.open(QIODevice::ReadWrite|QIODevice::Text);
         file.close();
         //qDebug()<<"文件创建成功";
    }
    //qDebug()<<"文件文件存在";
}

//添加事件函数
void MotusRunLog::addStringLog(QString strLog)
{
    if(strLog.isNull())
    {
        return;
    }
    ErrorRecord tempValue;
    unsigned int time=0;
    QString tempStr;
    QDateTime current_date_time = QDateTime::currentDateTime();
    //得到年数
    tempStr.clear();
    tempStr=current_date_time.toString("yyyy");
    time=tempStr.toInt();
    //得到总月数
    tempStr.clear();
    tempStr=current_date_time.toString("MM");
    time=time*12+tempStr.toInt();
    //得到总月数 时间 事件
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    tempValue.month=time;
    tempValue.strTime=current_date;
    tempValue.strEvent=strLog;
    merrorRecord.append(tempValue);
}

//写事件函数
void MotusRunLog::writeLog()
{
    if(merrorRecord.isEmpty())
    {
        return;
    }
    QFile file;
    file.setFileName(currentFileName);
    file.open(QIODevice::Text|QIODevice::WriteOnly|QIODevice::Truncate);
    QTextStream out(&file);
    for ( int i=0; i!=merrorRecord.size(); ++i )
    {
         out<<merrorRecord.at(i).month;
         out<<",";
         out<<merrorRecord.at(i).strTime;
         out<<",";
         out<<merrorRecord.at(i).strEvent;
         out<<'\n';
    }
    file.close();
}

//读事件函数
//只读取三个月以内的事件
void MotusRunLog::readLog()
{
    merrorRecord.clear();
    QFile file;
    file.setFileName(currentFileName);
    if(file.exists ())
    {
         int time=0;
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
             ErrorRecord tempValue;
             tempstr=out.readLine();
             //qDebug()<<tempstr;
             tempstrlist=tempstr.split (",");
             if(tempstrlist.isEmpty())
             {
                file.close();
                return;
             }
             int recordTime=tempstrlist.at(0).toInt();
             if((time-recordTime)<3)
             {
                tempValue.month=tempstrlist.at(0).toInt();
                tempValue.strTime=tempstrlist.at(1);
                tempValue.strEvent=tempstrlist.at(2);
                merrorRecord.append(tempValue);
             }
         }
         file.close();
    }
}

//上一页按键
void MotusRunLog::on_previousButton_clicked()
{
    int showRow = m_row%ROW;
    if(showRow == 0)
    {
        showRow = ROW;
    }
    if(m_row >= ROW + showRow)
    {
        for(int i=(m_row-ROW-showRow),j=0;i<(m_row-showRow);i++,j++)
        {
            m_item[j][0]->setText(merrorRecord.at(i).strTime);
            m_item[j][1]->setText(merrorRecord.at(i).strEvent);
        }
        m_row=m_row-showRow;
    }
}

//下一页按键
void MotusRunLog::on_nextButton_clicked()
{
    if(m_row<merrorRecord.size())
    {
        int saveRow=m_row;
        for(int i=m_row,j=0;i<m_row+ROW;i++,j++)
        {
            if(i<merrorRecord.size())
            {
                m_item[j][0]->setText(merrorRecord.at(i).strTime);
                m_item[j][1]->setText(merrorRecord.at(i).strEvent);
                saveRow+=1;
            }
            else
            {
                m_item[j][0]->setText("");
                m_item[j][1]->setText("");
            }
        }
        m_row=saveRow;
    }
}
