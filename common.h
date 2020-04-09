#ifndef COMMON_H
#define COMMON_H

#include <QStringList>
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <QLabel>
#include <QRadioButton>
#include <QGroupBox>
#include <QPushButton>
#include <QDialog>
#include <QLayout>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#if defined(Q_OS_UNIX)
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#elif defined(Q_OS_WIN)
#include <QSqlDatabase>
#include <windows.h>
#endif


QStringList loadCommandFileLines(const QString &filePath);

bool saveCommandFile(const QStringList &lines, const QString &filePath);

class DriverSelectionDialog : protected QDialog
{
    Q_OBJECT
public:
    DriverSelectionDialog (QWidget *parent=NULL) :
        QDialog(parent)
    {
        setWindowTitle(tr("���ݵ���"));
        setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        setFixedWidth(400);
        setStyleSheet( "QRadioButton::indicator {width: 30px;height: 30px; }"
                       "QRadioButton::indicator:checked {image: url(:/image/radiobutton-checked.png);}"
                       "QRadioButton::indicator:unchecked {image: url(:/image/radiobutton-unchecked.png);}");
        QPushButton *btOk = new QPushButton(tr("ȷ��"));
        QPushButton *btCancel = new QPushButton(tr("ȡ��"));
        QGroupBox *group = new QGroupBox(tr("��ѡ����Ҫ�����Ĵ���:"));
        QGridLayout *mainLayout = new QGridLayout(this);
        mainLayout->addWidget(group,0,0,1,3);
        mainLayout->addWidget(btOk,1,1,1,1);
        mainLayout->addWidget(btCancel,1,2,1,1);
        layout = new QVBoxLayout(group);

        connect(btOk , SIGNAL(clicked()) , this , SLOT(ok()));
        connect(btCancel , SIGNAL(clicked()) , this , SLOT(cancel()));
    }

    int showModule()
    {
        dir.clear();
        info.clear();
        DriverSelectionDialog::GetDrives(dir , info);
        for(int i=0;i<dir.count();i++)
        {
            QRadioButton *rbtn = new QRadioButton(tr("%1(%2)").arg(info[i]).arg(dir[i]));
            layout->addWidget(rbtn);
            btnList << rbtn;
        }
        btnList.last()->setChecked(true);

        return exec();
    }

    QString getSelectedDriver()
    {
        int i=0;
        for(;i<btnList.count();i++)
        {
            if(btnList.at(i)->isChecked())
                break;
        }

        return dir[i];
    }

private:
    QVBoxLayout *layout;
    QStringList dir ;
    QStringList info ;
    QList<QRadioButton *> btnList;

private slots:
    void ok()
    {
        this->accept();
    }

    void cancel()
    {
        this->reject();
    }

public:

#if defined(Q_OS_WIN)
    static void GetDrives(QStringList &dirverDir , QStringList &driverInfo)
    {
        char rootPath[10] = {0};
        UINT nType;

        for(char a = 'A'; a <= 'Z'; a++)
        {
            sprintf(rootPath, "%c:\\", a);
            nType = GetDriveTypeA(rootPath);
            if(nType != DRIVE_NO_ROOT_DIR)// DRIVE_NO_ROOT_DIR: ·����Ч
            {
                dirverDir << QString(rootPath);
                switch(nType)
                {
                case DRIVE_FIXED:
                    driverInfo << tr("Ӳ��");
                    break;
                case DRIVE_REMOVABLE:
                    driverInfo << tr("���ƶ�����");
                    break;
                case DRIVE_CDROM:
                    driverInfo << tr("����");
                    break;
                case DRIVE_RAMDISK:
                    driverInfo << tr("RAM��");
                    break;
                case DRIVE_REMOTE:
                    driverInfo << tr("Remote(Network) drive �������");
                    break;
                case DRIVE_UNKNOWN:
                default:
                    driverInfo << tr("δ֪��");
                    break;
                }
            }
        }
    }
#elif defined(Q_OS_UNIX)
    static void GetDrives(QStringList &driverDir , QStringList &driverInfo)
    {
        QFile file("/proc/mounts");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        char cLine[1024];
        for(;;)
        {
            int count = file.readLine(cLine , 1024);
            if(count <= 0){
                file.close();
                break;
            }

            QString strLine = QByteArray(cLine,count);
            QStringList strSections = strLine.split(" ");
            if(strSections.count() == 6)
            {
                QString devName = strSections.at(0) , mountPoint = strSections.at(1);

                if(devName.startsWith("/dev/sd"))
                {
                    driverInfo << tr("U��");
                    driverDir << mountPoint;
                }
                else if(devName.startsWith("/dev/mmc"))
                {
                    driverInfo << tr("SD��");
                    driverDir << mountPoint;
                }
                else if(devName.startsWith("/dev/"))
                {
                    driverInfo << tr("δ֪�豸") + devName;
                    driverDir << mountPoint;
                }
            }
        }
    }
#endif
};

/**
 * @brief Reclog ��¼��Ϣ���ļ��������¼����Ϣ����MAX_FILE_SIZE�������ݱ��ݵ�
 *                 Log/rec?.bak�ļ����У�Ȼ�����¼�¼
 * @param Msg ��Ҫ��¼����Ϣ
 */
class LOG_WRITER
{
protected:
    LOG_WRITER() : fcount(4) {}

    static void writeLog(QString Msg1 , QString Msg2 , QString prefix , int fcount)
    {
        const qint64 maxFileSize = 5*1024*1024;//5MB
        QFile recfile(prefix + ".log");
        if(recfile.open(QIODevice::Append | QIODevice::Text))
        {
            //��¼��Ϣ���ļ�
            QString strREC = QString("{%1}[%2]%3\n")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
                    .arg(Msg1).arg(Msg2);
            recfile.write(strREC.toLocal8Bit());

            //�����ļ���С��ȷ���Ƿ񱸷ݲ����¼�¼
            if(recfile.size() > maxFileSize)
            {
                for(int i=fcount-1;i>0;i--)
                {
                    QString backupName = prefix+QString("%1.log").arg(i);
                    if(QFile::exists(backupName))
                    {
                        if(i==fcount-1)
                        {
                            QFile::remove(backupName);
                        }
                        else
                        {
                            QString targetName = prefix + QString("%1.log").arg(i+1);
                            QFile::rename(backupName , targetName);
                        }
                    }
                }
                recfile.rename(prefix + "1.log");
            }
            recfile.close();
        }
    }

public:
    static LOG_WRITER *getObject(const QString &filePath)
    {
        static LOG_WRITER *writer = NULL;
        if(writer == NULL)
            writer = new LOG_WRITER;
        writer->filePath = filePath;
        return writer;
    }

    void notice( const QString &x )
    {
        LOG_WRITER::writeLog("notice" , x , filePath , fcount);
    }

    void info( const QString &x )
    {
        LOG_WRITER::writeLog("info" , x , filePath , fcount);
    }

    void debug(const QString &x)
    {
        LOG_WRITER::writeLog("debug" , x , filePath , fcount );
    }

    void error(  const QString &x  )
    {
        LOG_WRITER::writeLog("error" , x , filePath , fcount );
    }

    void fatal(  const QString &x  )
    {
        LOG_WRITER::writeLog("fatal" , x , filePath , fcount );
    }
    void setCount(int c) { fcount = c;}

protected:
    QString filePath;
    int fcount;
};


QString  Int2Hex(int a , int b);
void  Reclog(QString Msg);
void  RecordTemp(QString temp , QString strPath);
bool  importData(QString strDir);
QString  Float4(float value);
int  CRC16( char* pchMsg, unsigned int wDataLen);
void  print_callstack();
bool  copyFiles(QString file,QString tagFile);
int  set_local_time(const char *time_string);


bool  isNan(float f);
bool  isNan(double f);
bool  isInf(float f);
bool  isInf(double f);
float  setPrecision(float v, int n, QString *pshow=NULL);

// ��ȡ���ݿ���ĳһ�����ݣ�index��python�÷���-1�������һ������
bool readSqlValues(QSqlDatabase *db, const QString &table, QStringList &value, int index = -1);


#endif // COMMON_H
