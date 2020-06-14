#include "common.h"
#include "globelvalues.h"
#include <QFile>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#if defined(Q_OS_UNIX)
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#elif defined(Q_OS_WIN)
#include <windows.h>
#endif


QString Int2Hex(int a , int b)
{
    if(a < 0) a= 0;
    else if (a>255) a = 255;

    if(b < 0) b= 0;
    else if (b>255) b = 255;

    QString sa = (QString("00") + QString::number(a,16)).right(2).toUpper();
    QString sb = (QString("00") + QString::number(b,16)).right(2).toUpper();

    return sa + sb;
}


int CRC16( char* pchMsg,  int wDataLen)
{
    //算法校验
    int code=0xffff;
    for(int i=0;i<wDataLen;i++)
    {
        code=(code>>8)^pchMsg[i];

        for(int j=0;j<8;j++)
        {
            if(code&1)
            {
                code>>=1;
                code^=0xa001;
            }
            else
                code>>=1;
        }
    }
    return code;
}


bool copyFiles(QString file,QString tagFile)
{
    system(QString("cp -rf  %1 %2").arg(file).arg(tagFile).toAscii().data());
    return true;
}

/*0.0-99999.0之间的数保留四位有效数字*/
QString Float4(float value)
{
    QString strRet;
    if(value < -99999.0)
        strRet = "-99999";
    else if(value > 99999.0)
        strRet = "99999";
    else {
        strRet = QString("%100000").arg(value,0,'f').left(strRet>=0?5:6);
        if(strRet.endsWith("."))
            strRet = strRet.left(strRet>=0?4:5);
    }
    return strRet;
}

void print_callstack()
{
//    void *array[32];
//    size_t size;
//    char **strings;
//    size_t i;

//    size = backtrace (array, 32);
//    strings = backtrace_symbols (array, size);

//    printf ("Obtained %d stack frames.\n", size);

//    for (i = 0; i < size; i++)
//         printf ("%s\n", strings);

//    free (strings);
}

#if defined(Q_OS_UNIX)
int setSystemTime(const char *time_string)
{
//    struct rtc_time ;
    struct tm _tm, tm;
    struct timeval tv;
    time_t timep;
    sscanf(time_string, "%d-%d-%d %d:%d:%d", &tm.tm_year,
        &tm.tm_mon, &tm.tm_mday,&tm.tm_hour,
        &tm.tm_min, &tm.tm_sec);
    _tm.tm_sec = tm.tm_sec;
    _tm.tm_min = tm.tm_min;
    _tm.tm_hour = tm.tm_hour;
    _tm.tm_mday = tm.tm_mday;
    _tm.tm_mon = tm.tm_mon - 1;
    _tm.tm_year = tm.tm_year - 1900;

    timep = mktime(&_tm);
    tv.tv_sec = timep;
    tv.tv_usec = 0;
    if(settimeofday(&tv, (struct timezone *) 0) < 0)
        return -1;
    return 0;
}
#elif defined(Q_WS_WIN)
int setSystemTime(const char *time_string)
{
    SYSTEMTIME system_time;
    GetLocalTime(&system_time);

    sscanf(time_string, "%d-%d-%d %d:%d:%d", system_time.wYear,
                       system_time.wMonth,
                       system_time.wDay,
                       system_time.wHour,
                       system_time.wMinute,
                      system_time.wSecond);

    if (!SetLocalTime(&system_time))
        return -1;
    return 0;
}
#endif


bool isNan(float f)
{
    uchar *ch = (uchar *)(&f);
    if(QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        return  ((ch[0] & 0x7f) == 0x7f) && (ch[1] > 0x80);
    }
    else
    {
        return ((ch[3] & 0x7f) == 0x7f) && (ch[2] > 0x80);
    }
}
bool isNan(double f)
{
    uchar *ch = (uchar *)(&f);
    if(QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        return  ((ch[0] & 0x7f) == 0x7f) && (ch[1] > 0xf0);
    }
    else
    {
        return ((ch[7] & 0x7f) == 0x7f) && (ch[6] > 0xf0);
    }
}
bool isInf(float f)
{
    uchar *ch = (uchar *)(&f);
    if(QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        return  ((ch[0] & 0x7f) == 0x7f) && (ch[1] == 0x80);
    }
    else
    {
        return ((ch[3] & 0x7f) == 0x7f) && (ch[2] == 0x80);
    }
}
bool isInf(double f)
{
    uchar *ch = (uchar *)(&f);
    if(QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        return  ((ch[0] & 0x7f) == 0x7f) && (ch[1] == 0xf0);
    }
    else
    {
        return ((ch[7] & 0x7f) == 0x7f) && (ch[6] == 0xf0);
    }
}

// n > 0 时按照保留n位小数位数的方式确定精度
// n < 0 时按照-n位有效数据的方式确定精度
float setPrecision(float v, int n, QString *pshow)
{
    if (n >= 0)
    {
        float f = pow(10.0, n);
        if (pshow)
            *pshow = QString("%1").arg(v, 0, 'f', n);
        return ((float)(int)(v*f))/f;
    }
    else
    {
         // 确定保留小数的位数
        int x = floor(v);
        int m = 0;
        while(x > 0)
        {
            m++;
            x /= 10;
        }
        m = -m - n;
        if (m < 0)
            m = 0;
        return setPrecision(v, m, pshow);
    }
}


bool readSqlValues(QSqlDatabase *sqldb, const QString &table, QStringList &value, int index)
{
    int pindex = abs(index);
    if(sqldb == NULL)
        return false;

    QString strquery;
    if (index < 0)
        strquery = QString("SELECT * FROM %1 order by TimeID desc").arg(table);
    else if (index > 0)
        strquery = QString("SELECT * FROM %1").arg(table);
    else
        return false;
    QSqlQuery query(strquery, *sqldb);

    //显示当前页条目
    int i = 1;
    int column = query.record().count();
    while (query.next())
    {
        if (i == pindex)
        {
            for (int j = 0; j < column; j++)
                value << query.value(j).toString();
            return true;
        }
        i++;
    }
    return false;
}

QStringList loadCommandFileLines(const QString &filePath)
{
    QStringList ct;
    QFile file(elementPath + filePath);
    char buf[256];
    qint64 len = 0;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while ((len = file.readLine(buf,256)) > 0)
            ct << QString::fromLatin1(buf, len - 1).remove("\r\n");
    }
    return ct;
}


bool saveCommandFile(const QStringList &lines, const QString &filePath)
{
    QString data;
    QFile file(elementPath + filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        for (int i = 0; i < lines.count(); i++)
            data +=  lines[i] + "\n";

        file.write(data.toLatin1());
        return true;
    }
    else
        return false;
}


bool getUserDataBase(QSqlDatabase &sqlitedb)
{
    const QString dbuserdata = elementPath + "UserData.db";
    sqlitedb = QSqlDatabase::database(dbuserdata);

    if (!sqlitedb.isValid()) {
        sqlitedb = QSqlDatabase::addDatabase("QSQLITE", dbuserdata);
        sqlitedb.setDatabaseName(dbuserdata);
    }

    if (!sqlitedb.isOpen() && !sqlitedb.open()) {
        qDebug() << sqlitedb.lastError() << dbuserdata;
        return false;
    }
    return true;
}



void addMeasureData(QList<QVariant> &data)
{
    QSqlDatabase sqlitedb;
    if (!getUserDataBase(sqlitedb))
        return;

    QString TimeID = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString Time = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm");
    QString strMsg;

    int varCount = data.count();
    for (int i = 0; i < 17; i++)
    {
        strMsg += ",'";
        if (i < varCount)
            strMsg += data[i].toString();
        strMsg += "'";
    }

    QSqlQuery sqlquery(sqlitedb);
    if (!sqlquery.exec(QString("INSERT INTO Data(ID,TimeID,A1,A2,A3,A4,A5,A6,A7,A8,A9,B1,B2,B3,B4,B5,B6,B7,B8,B9)"
                               "VALUES(NULL,'%1','%2'%3);")
                       .arg(TimeID).arg(Time).arg(strMsg)))
        qDebug() << sqlquery.lastError().text() << sqlquery.lastQuery();
    sqlquery.clear();
}

void addCalibrationData(QList<QVariant> &data)
{
    QSqlDatabase sqlitedb;
    if (!getUserDataBase(sqlitedb))
        return;

    QString TimeID = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString Time = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm");
    QString strMsg;

    int varCount = data.count();
    for (int i = 0; i < 17; i++)
    {
        strMsg += ",'";
        if (i < varCount)
            strMsg += data[i].toString();
        strMsg += "'";
    }

    QSqlQuery sqlquery(sqlitedb);
    if (!sqlquery.exec(QString("INSERT INTO Calibration(ID,TimeID,A1,A2,A3,A4,A5,A6,A7,A8,A9,B1,B2,B3,B4,B5,B6,B7,B8,B9)"
                               "VALUES(NULL,'%1','%2'%3);")
                       .arg(TimeID).arg(Time).arg(strMsg)))
        qDebug() << sqlquery.lastError().text() << sqlquery.lastQuery();
    sqlquery.clear();
}

QString getLastErrorMsg()
{
    return errorMessage;
}

void clearLastErrorMsg()
{
    errorMessage.clear();
}

void addErrorMsg(const QString &strMsg, int level)
{
    if (level) {errorMessage = strMsg;}

    QSqlDatabase sqlitedb;
    if (!getUserDataBase(sqlitedb))
        return;

    QString TimeID = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString Time = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm");

    QSqlQuery sqlquery(sqlitedb);
    if (!sqlquery.exec(QString("INSERT INTO Error(ID,TimeID,A1,A2,A3,A4,A5,A6,A7,A8,A9,B1,B2,B3,B4,B5,B6,B7,B8,B9)"
                               "VALUES(NULL,'%1','%2','%3','%4','','','','','','','','','','','','','','','');")
                       .arg(TimeID).arg(Time).arg(level?QObject::tr("警告"):QObject::tr("提示")).arg(strMsg)))
        qDebug() << sqlitedb.lastError();
    sqlquery.clear();
}

void addLogger(QString strMsg, LoggerType type)
{
    QSqlDatabase sqlitedb;
    if (!getUserDataBase(sqlitedb))
        return;

    QString TimeID = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString Time = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm");
    QString mesgType;
    switch (type)
    {
    case LoggerTypeRunning: mesgType = QObject::tr("运行日志");break;
    case LoggerTypeMaintiance: mesgType = QObject::tr("维护");break;
    case LoggerTypeOperations: mesgType = QObject::tr("校准信息");break;
    case LoggerTypeSettingsChanged: mesgType = QObject::tr("参数变更");break;
    default:
         mesgType = QObject::tr("其他");break;
    }

    QSqlQuery sqlquery(sqlitedb);
    if (!sqlquery.exec(QString("INSERT INTO Log(ID,TimeID,A1,A2,A3,A4,A5,A6,A7,A8,A9,B1,B2,B3,B4,B5,B6,B7,B8,B9)"
                          "VALUES(NULL,'%1','%2','%3','%4','','','','','','','','','','','','','','','');")
                  .arg(TimeID).arg(Time).arg(mesgType).arg(strMsg)))
        qDebug() << sqlitedb.lastError();
    sqlquery.clear();
}


DriverSelectionDialog::DriverSelectionDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("数据导出"));
    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setFixedWidth(400);

    QPushButton *btOk = new QPushButton(tr("确定"));
    QPushButton *btCancel = new QPushButton(tr("取消"));
    QGroupBox *group = new QGroupBox(tr("请选择需要导出的磁盘:"));
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(group,0,0,1,3);
    mainLayout->addWidget(btOk,1,1,1,1);
    mainLayout->addWidget(btCancel,1,2,1,1);
    layout = new QVBoxLayout(group);

    connect(btOk , SIGNAL(clicked()) , this , SLOT(ok()));
    connect(btCancel , SIGNAL(clicked()) , this , SLOT(cancel()));
}

int DriverSelectionDialog::showModule()
{
    dir.clear();
    info.clear();
    DriverSelectionDialog::GetDrives(dir , info, exclusives);
    for(int i=0;i<dir.count();i++)
    {
        QRadioButton *rbtn = new QRadioButton(tr("%1(%2)").arg(info[i]).arg(dir[i]));
        layout->addWidget(rbtn);
        btnList << rbtn;
    }
    if (btnList.count() > 0)
        btnList.last()->setChecked(true);

    return exec();
}

QString DriverSelectionDialog::getSelectedDriver()
{
    int i=0;
    for(;i<btnList.count();i++)
    {
        if(btnList.at(i)->isChecked())
            break;
    }

    if (dir.count() > 0)
        return dir[i];
    else
        return "";
}

void DriverSelectionDialog::addExclusiveDriver(const QString &dri)
{
    exclusives <<  dri;
}

void DriverSelectionDialog::ok()
{
    this->accept();
}

void DriverSelectionDialog::cancel()
{
    this->reject();
}

#if defined(Q_OS_WIN)
void DriverSelectionDialog::GetDrives(QStringList &driverDir, QStringList &driverInfo, QStringList &)
{
    char rootPath[10] = {0};
    UINT nType;

    for(char a = 'A'; a <= 'Z'; a++)
    {
        sprintf(rootPath, "%c:\\", a);
        nType = GetDriveTypeA(rootPath);
        if(nType != DRIVE_NO_ROOT_DIR)// DRIVE_NO_ROOT_DIR: 路径无效
        {
            driverDir << QString(rootPath);
            switch(nType)
            {
            case DRIVE_FIXED:
                driverInfo << tr("硬盘");
                break;
            case DRIVE_REMOVABLE:
                driverInfo << tr("可移动磁盘");
                break;
            case DRIVE_CDROM:
                driverInfo << tr("光盘");
                break;
            case DRIVE_RAMDISK:
                driverInfo << tr("RAM盘");
                break;
            case DRIVE_REMOTE:
                driverInfo << tr("Remote(Network) drive 网络磁盘");
                break;
            case DRIVE_UNKNOWN:
            default:
                driverInfo << tr("未知盘");
                break;
            }
        }
    }
}
#elif defined(Q_OS_UNIX)
void DriverSelectionDialog::GetDrives(QStringList &driverDir, QStringList &driverInfo, QStringList &exclusive)
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

            if (exclusive.contains(devName))
                continue;

            if(devName.startsWith("/dev/sd"))
            {
                driverInfo << tr("U盘");
                driverDir << mountPoint;
            }
            else if(devName.startsWith("/dev/mmc"))
            {
                driverInfo << tr("SD卡");
                driverDir << mountPoint;
            }
            else if(devName.startsWith("/dev/"))
            {
                driverInfo << tr("未知设备") + devName;
                driverDir << mountPoint;
            }
        }
    }
}
#endif


LOG_WRITER::LOG_WRITER() : fcount(4) {}

void LOG_WRITER::writeLog(QString Msg1, QString Msg2, QString prefix, int fcount)
{
    const qint64 maxFileSize = 5*1024*1024;//5MB
    QFile recfile(prefix + ".log");
    if(recfile.open(QIODevice::Append | QIODevice::Text))
    {
        //记录信息到文件
        QString strREC = QString("{%1}[%2]%3\n")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
                .arg(Msg1).arg(Msg2);
        recfile.write(strREC.toLocal8Bit());

        //根据文件大小，确定是否备份并重新记录
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

LOG_WRITER *LOG_WRITER::getObject(const QString &filePath)
{
    static LOG_WRITER *writer = NULL;
    if(writer == NULL) {
        writer = new LOG_WRITER;
        QString st = elementPath + "/logs";
        QDir dir(st);
        if (!dir.exists())
        {
            dir.setPath("");
            if (!dir.mkdir(st))
                qDebug() << "mkdir logs failed!";
        }
    }
    writer->filePath = filePath;
    return writer;
}

void LOG_WRITER::notice(const QString &x)
{
    LOG_WRITER::writeLog("notice" , x , filePath , fcount);
}

void LOG_WRITER::info(const QString &x)
{
    LOG_WRITER::writeLog("info" , x , filePath , fcount);
}

void LOG_WRITER::debug(const QString &x)
{
    LOG_WRITER::writeLog("debug" , x , filePath , fcount );
}

void LOG_WRITER::error(const QString &x)
{
    LOG_WRITER::writeLog("error" , x , filePath , fcount );
}

void LOG_WRITER::fatal(const QString &x)
{
    LOG_WRITER::writeLog("fatal" , x , filePath , fcount );
}

void LOG_WRITER::setCount(int c) { fcount = c;}

