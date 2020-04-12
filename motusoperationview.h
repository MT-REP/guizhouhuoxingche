#ifndef MOTUSOPERATIONVIEW_H
#define MOTUSOPERATIONVIEW_H

#include <QDialog>
#include <QList>
#include <QModelIndex>
#include <QProgressBar>

typedef struct MovieIndex
{
    QString fileMovieName;    //电影名称
    QString fileTxtName;      //电影数据txt名称
    QString fileMovieSequence;//电影序列
    MovieIndex()
    {
        fileMovieName.clear();
        fileTxtName.clear();
        fileMovieSequence.clear();
    }
}M_MovieIndex;

//保存文件路径信息
typedef struct SaveMovieMessage
{
    M_MovieIndex mMovieIndex;
    QString strDir;
}M_SaveMovieMessage;

//当前电影
typedef struct CurrentMovie
{
    QString fileMovieName;   //当前电影名字
    bool isExist;
    bool isPlay;
    QModelIndex currentModelIndex;
    CurrentMovie()
    {
        fileMovieName.clear();
        isPlay=false;
        isExist=false;
    }
}M_CurrentMovie;

//电影数据结构体
typedef struct MovieData
{
    double atu1;//纵倾
    double atu2;//横摇
    double atu3;//航向
    double atu4;//横移
    double atu5;//前倾
    double atu6;//升降
    int   speeff;//特效
    MovieData()
    {
        atu1=0.0f;
        atu2=0.0f;
        atu3=0.0f;
        atu4=0.0f;
        atu5=0.0f;
        atu6=0.0f;
        speeff=0;
    }
}M_MovieData;

class QStandardItemModel;//文件模式
namespace Ui {
class MotusOperationView;
}

class MotusOperationView : public QDialog
{
    Q_OBJECT
public:
    explicit MotusOperationView(QWidget *parent = 0);
    void init();
    void setViewStatus(bool ret[8]);
    void setCyinder(int cylinder[6]);
    void updateOperationStatus(int step, QString hintMessage,QString errorMessage, int playCount);
    ~MotusOperationView();
signals:
    void sendOperationCmd(int cmd);
    void txtDataChange(QList<M_MovieData>&movieData,QString movieorder);
public slots:
    void recvOperateViewText(QString text);
private slots:
    void recvPlay(bool isPlay);
    void tableViewDoubleClicked(const QModelIndex &temp);
    void on_deleteFileButton_clicked();
    void on_selectFileButton_clicked();
    void on_saveFileButton_clicked();
    void on_playFileLoad_clicked();
    void on_playRestartButton_clicked();
    void on_playStopButton_clicked();
private:
    bool readmoviefile(QString filename,QList<M_MovieIndex>&mMovieiIndexList);  //读电影配置数据
    bool writemoviefile(QString filename,QList<M_MovieIndex>&mMovieiIndexList); //写电影配置数据
    bool readtxtfile(QString filename,QList<M_MovieData>&movieData,bool def=true);            //读取电影数据
    bool writetxtfile(QString filename,QList<M_MovieData>&movieData);           //写入电影数据
    bool deletetxtfile(const QString &filename);
private:
    QPushButton * ptn[8];
    QProgressBar *pbar[6];
    Ui::MotusOperationView *ui;
    QStandardItemModel *m_playmodel;     //文件模式
    QList<M_MovieIndex>mMovieiIndexList; //电影索引链表
    M_CurrentMovie mCurrentMovieMessage; //当前正在播放的电影
    QList<M_MovieData>mMovieData;        //电影数据
    M_SaveMovieMessage mSaveMovieMessage;//保存文件信息
};

#endif // MOTUSOPERATIONVIEW_H
