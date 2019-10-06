#include "motusoperationview.h"
#include "ui_motusoperationview.h"
#include "mainwindow.h"
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
MotusOperationView::MotusOperationView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotusOperationView)
{
    ui->setupUi(this);
    m_playmodel = new QStandardItemModel(this);//tableview的模式
    m_playmodel->setHorizontalHeaderItem(0, new QStandardItem("电影名称"));//tableview的列表头第一个
    m_playmodel->setHorizontalHeaderItem(1, new QStandardItem("电影序列"));//tableview的列表头的第二个
    ui->playTableView->setModel(m_playmodel);//加载模式
    ui->playTableView->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->playTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许编译
    ui->playTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //双击
    connect(ui->playTableView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(tableViewDoubleClicked(const QModelIndex &)));
    //读电影数据
    if(readmoviefile("movie\\movie.txt",mMovieiIndexList))
    {
        for(int i=0;i<mMovieiIndexList.size();i++)
        {
           m_playmodel->setItem(i,0,new QStandardItem(mMovieiIndexList.at(i).fileMovieName));
           m_playmodel->setItem(i,1,new QStandardItem(mMovieiIndexList.at(i).fileMovieSequence));
        }
        if(mMovieiIndexList.size()>0)
        {
            ui->playMovieEdit->setText(mMovieiIndexList.at(0).fileMovieName);
            ui->playDataEdit->setText(mMovieiIndexList.at(0).fileTxtName);
            ui->playMovieSequenceEdit->setText(mMovieiIndexList.at(0).fileMovieSequence);
        }
    }
    //mQTcpSocket.connectToHost("192.168.1.7",23);
}

//初始化函数
void MotusOperationView::init()
{
    //读取数据
    if(mMovieiIndexList.size()!=0&&readtxtfile(mMovieiIndexList.at(0).fileTxtName,mMovieData))
    {
        emit txtDataChange(mMovieData,mMovieiIndexList.at(0).fileMovieSequence);
        ui->playProgressBar->setRange(0,mMovieData.size()-1);
        mCurrentMovieMessage.fileMovieName=mMovieiIndexList.at(0).fileMovieName;
        mCurrentMovieMessage.isExist=true;
        mCurrentMovieMessage.isPlay=false;
    }
    else
    {
        QMessageBox::information(this,"友情提示","数据文件不存在");
    }
}

//析构函数
MotusOperationView::~MotusOperationView()
{
    delete ui;
}

//双击相应函数
void MotusOperationView::tableViewDoubleClicked(const QModelIndex &temp)
{
    QString strName;
    mCurrentMovieMessage.currentModelIndex=temp;
    strName=m_playmodel->item(mCurrentMovieMessage.currentModelIndex.row(),0)->text();
    if(mCurrentMovieMessage.fileMovieName.compare(strName)!=0&&!mCurrentMovieMessage.isPlay)
    {
        for(int i=0;i<mMovieiIndexList.size();i++)
        {
           if(mMovieiIndexList.at(i).fileMovieName.compare(strName)==0)
           {
               if(readtxtfile(mMovieiIndexList.at(i).fileTxtName,mMovieData))
               {
                   emit txtDataChange(mMovieData,mMovieiIndexList.at(i).fileMovieSequence);
                   ui->playProgressBar->setRange(0,mMovieData.size()-1);
                   mCurrentMovieMessage.fileMovieName=strName;
                   mCurrentMovieMessage.isExist=true;
                   mCurrentMovieMessage.isPlay=false;
                   ui->playMovieEdit->setText(mMovieiIndexList.at(i).fileMovieName);
                   ui->playDataEdit->setText(mMovieiIndexList.at(i).fileTxtName);
                   ui->playMovieSequenceEdit->setText(mMovieiIndexList.at(i).fileMovieSequence);
               }
           }
        }
    }
}

//影片是否正在播放
void MotusOperationView::recvPlay(bool isPlay)
{
    mCurrentMovieMessage.isPlay=isPlay;
}

//打开文件
void MotusOperationView::on_selectFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("Txt (*.txt)"));
    if (!fileName.isNull()) //用户选择了文件
    {
      mSaveMovieMessage.strDir=fileName;
      QMessageBox::information(this, "Document", "Has document", QMessageBox::Ok); // 处理文件
    }
    else // 用户取消选择
    {
      mSaveMovieMessage.strDir.clear();
      QMessageBox::information(this, "Document", "No document", QMessageBox::Ok);
    }
}

//保存文件
void MotusOperationView::on_saveFileButton_clicked()
{
    if(mSaveMovieMessage.strDir.isNull())
    {
        QMessageBox::information(this, "友情提示", "选择文件为空", QMessageBox::Ok); // 处理文件
        return;
    }
    if(ui->movieLineEdit->text()=="")
    {
        QMessageBox::information(this, "友情提示", "请输入电影名称", QMessageBox::Ok); // 处理文件
        return;
    }
    if(ui->dataLineEdit->text()=="")
    {
        QMessageBox::information(this, "友情提示", "请输入数据名称", QMessageBox::Ok); // 处理文件
        return;
    }
    if(ui->sequenceLineEdit->text()=="")
    {
        QMessageBox::information(this, "友情提示", "请输入电影序列", QMessageBox::Ok ); // 处理文件
        return;
    }
    for(int i=0;i<mMovieiIndexList.size();i++)
    {
        if(mMovieiIndexList.at(i).fileMovieName.compare(ui->movieLineEdit->text())==0)
        {
            QMessageBox::information(this, "友情提示", "电影名称重名", QMessageBox::Ok ); // 处理文件
            return;
        }
        if(mMovieiIndexList.at(i).fileTxtName.compare(ui->dataLineEdit->text())==0)
        {
            QMessageBox::information(this, "友情提示", "电影数据重名", QMessageBox::Ok ); // 处理文件
            return;
        }
    }
    QString str[3];
    QList<QStandardItem *>templist;
    mSaveMovieMessage.mMovieIndex.fileMovieName=str[0]=ui->movieLineEdit->text();
    mSaveMovieMessage.mMovieIndex.fileTxtName=str[2]=ui->dataLineEdit->text();
    mSaveMovieMessage.mMovieIndex.fileMovieSequence=str[1]=ui->sequenceLineEdit->text();
    for(int i=0;i<2;i++)
    {
        templist.append(new QStandardItem(str[i]));
    }
    //读取文件
    readtxtfile(mSaveMovieMessage.strDir,mMovieData,false);
    //写文件
    writetxtfile(mSaveMovieMessage.mMovieIndex.fileTxtName,mMovieData);
    mMovieiIndexList.append(mSaveMovieMessage.mMovieIndex);
    writemoviefile("movie\\movie.txt",mMovieiIndexList);
    m_playmodel->appendRow(templist);
}

//删除槽函数
void MotusOperationView::on_deleteFileButton_clicked()
{
    if(!mCurrentMovieMessage.currentModelIndex.isValid())
    {
       QMessageBox::information(this,"友情提示","未选中文件");
       return;
    }
    if(QMessageBox::information(this, "温馨提示", "是否删除文件", QMessageBox::Ok | QMessageBox::Cancel)==QMessageBox::Ok) // 处理文件
    {
        QString strname;
        strname=m_playmodel->item(mCurrentMovieMessage.currentModelIndex.row(),0)->text();
        //qDebug()<<m_playmodel->item(mCurrentMovieMessage.currentModelIndex.row(),0)->text();
        if(mCurrentMovieMessage.isPlay&&mCurrentMovieMessage.fileMovieName.compare(strname)==0)
        {
           QMessageBox::information(this, "温馨提示", "正在播放该文件不能删除该文件", QMessageBox::Ok | QMessageBox::Cancel);
        }
        else
        {
            for(int i=0;i<mMovieiIndexList.size();i++)
            {
               if(mMovieiIndexList.at(i).fileMovieName.compare(strname)==0)
               {
                    deletetxtfile(mMovieiIndexList.at(i).fileTxtName);
                    mMovieiIndexList.removeAt(i);
               }
            }
            m_playmodel->removeRow(mCurrentMovieMessage.currentModelIndex.row());
            writemoviefile("movie\\movie.txt",mMovieiIndexList);
        }
   }
}

//删除文件函数
bool MotusOperationView::deletetxtfile(const QString &filename)
{
     QFile file;
     QString str="movie\\";
     str+=filename;
     if(file.exists(str))
     {
        file.remove(str);
        return true;
     }
     else
     {
        return false;
     }
}

//读取电影文件配置
bool MotusOperationView::readmoviefile(QString filename,QList<M_MovieIndex>&mMovieiIndexList)
{
   //QDir dir;
   //qDebug()<<dir.currentPath();
   QFile file;
   file.setFileName(filename);
   mMovieiIndexList.clear();
   if(file.exists ())
   {
        //qDebug()<<123;
        file.open(QIODevice::ReadOnly);
        QTextStream out(&file);
        while(!out.atEnd())
        {
            QString  tempstr;
            QStringList  tempstrlist;
            M_MovieIndex tempmovieindex;
            tempstr=out.readLine();
            tempstrlist=tempstr.split (",");
            //qDebug()<<tempstrlist.size();
            if(tempstrlist.size()!=3)
            {
                if(tempstrlist.size()==1)
                {
                    continue;
                }
                else
                {
                    QMessageBox::information(this,"友情提示","文件格式不正确，\r\n请检查影片配置文件movie.txt");
                    file.close();
                    return false;
                }
            }
            tempmovieindex.fileMovieName=tempstrlist.at(0);
            tempmovieindex.fileTxtName=tempstrlist.at(1);
            tempmovieindex.fileMovieSequence=tempstrlist.at(2);
            //qDebug()<<tempstrlist.at(0)<<tempstrlist.at(1)<<tempstrlist.at(2);
            mMovieiIndexList.append(tempmovieindex);
        }
        file.close();
        return true;
   }else
   {
        return false;
   }
}

//写电影配置文件
bool MotusOperationView::writemoviefile(QString filename,QList<M_MovieIndex>&mMovieiIndexList)
{
    QFile file;
    file.setFileName(filename);
    if(file.exists ())
    {
         file.open(QIODevice::Text|QIODevice::WriteOnly|QIODevice::Truncate);
         QTextStream out(&file);
         for ( int i=0; i!=mMovieiIndexList.size(); ++i )
         {
             out<<mMovieiIndexList.at(i).fileMovieName;
             out<<",";
             out<<mMovieiIndexList.at(i).fileTxtName;
             out<<",";
             out<<mMovieiIndexList.at(i).fileMovieSequence;
             out<<endl;
         }
         file.close();
         return true;
    }else
    {
         return false;
    }
}

//写电影数据文件
bool MotusOperationView::writetxtfile(QString filename,QList<M_MovieData>&movieData)
{
    QString str="movie\\";
    str+=filename;
    QFile file;
    file.setFileName(str);
    file.open(QIODevice::Text|QIODevice::WriteOnly|QIODevice::Truncate);
    QTextStream out(&file);
    for ( int i=0; i!=movieData.size(); ++i )
    {
         out<<movieData.at(i).atu1;
         out<<",";
         out<<movieData.at(i).atu2;
         out<<",";
         out<<movieData.at(i).atu3;
         out<<",";
         out<<movieData.at(i).atu4;
         out<<",";
         out<<movieData.at(i).atu5;
         out<<",";
         out<<movieData.at(i).atu6;
         out<<",";
         out<<movieData.at(i).speeff;
         out<<'\n';
    }
    file.close();
    return true;
}

//读取电影数据文件配置
bool MotusOperationView::readtxtfile(QString filename,QList<M_MovieData>&movieData,bool def)
{
    QString str="movie\\";
    if(def)
    {
        str+=filename;
    }
    else
    {
       str = filename;
    }
    QFile file;
    file.setFileName(str);
    movieData.clear();
    if(file.exists ())
    {
         file.open(QIODevice::ReadOnly);
         QTextStream out(&file);
         while(!out.atEnd())
         {
             QString  tempstr;
             QStringList  tempstrlist;
             M_MovieData tempmoviedata;
             tempstr=out.readLine();
             tempstrlist=tempstr.split (",");
             if(tempstrlist.size()!=7)
             {
                file.close();
                QMessageBox::information(this,"友情提示","文件格式不正确");
                return false;
             }
             tempmoviedata.atu1=tempstrlist.at(0).toDouble();
             tempmoviedata.atu2=tempstrlist.at(1).toDouble();
             tempmoviedata.atu3=tempstrlist.at(2).toDouble();
             tempmoviedata.atu4=tempstrlist.at(3).toDouble();
             tempmoviedata.atu5=tempstrlist.at(4).toDouble();
             tempmoviedata.atu6=tempstrlist.at(5).toDouble();
             if(tempstrlist.size()>6)
                 tempmoviedata.speeff=tempstrlist.at(6).toInt();
             movieData.append(tempmoviedata);
         }
         file.close();
         return true;
    }else
    {
         return false;
    }
}

//设置开机按键上的文字
void MotusOperationView::recvOperateViewText(QString text)
{
    ui->playFileLoad->setText(text);
}

//开机关机按键
void MotusOperationView::on_playFileLoad_clicked()
{
  QString str=ui->playFileLoad->text();
  if(str.compare("开机")==0)
    emit sendOperationCmd(StartUp);
  else if(str.compare("关机")==0)
    emit sendOperationCmd(PowerOff);
}

//复位按键
void MotusOperationView::on_playRestartButton_clicked()
{
    emit sendOperationCmd(Restoration);
}

//运行按键
void MotusOperationView::on_playStopButton_clicked()
{
    //const char VideoPlayCmd[6] = {0x50,0x4C,0x41,0x59,0x0D,0x0A};
    //mQTcpSocket.write(VideoPlayCmd,6);
    emit sendOperationCmd(Operation);
}

//更新状态
void MotusOperationView::updateOperationStatus(int step, QString hintMessage,QString errorMessage, int playCount)
{
    static QString strStep[9]={"0初始默认值","1自检处理:","2准备处理:","3待客处理:","4运动准备:","5平台运动:","6恢复处理:","7故障处理:","8关机处理:"};
    QString tempstr;
    tempstr=strStep[step];
    tempstr+=hintMessage;
    ui->playCurrentFlowPath->setText(tempstr);
    ui->playCurrentErrorMessage->setText(errorMessage);
    ui->playProgressBar->setValue(playCount);
}
