#include "common.h"
#include <QFile>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QSettings>

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
/**
 * @brief Reclog ��¼��Ϣ���ļ��������¼����Ϣ����MAX_FILE_SIZE�������ݱ��ݵ�
 *                 Log/rec.bak�ļ����У�Ȼ�����¼�¼
 * @param Msg ��Ҫ��¼����Ϣ
 */
void Reclog(QString Msg)
{
    const qint64 MAX_FILE_SIZE = 10*1024*1024;//10MB
    const QString filename = "logs/log.log";
    const QString filebak = "logs/log.bak";

    QFile recfile(filename);
    if(recfile.open(QIODevice::Append))
    {
        //��¼��Ϣ���ļ�
        QDateTime dt = QDateTime::currentDateTime();
        QString strREC = dt.toString("yyyy-MM-dd hh:mm:ss zzz\t") + Msg + "\r\n";
        recfile.write(strREC.toLatin1());

        //�����ļ���С��ȷ���Ƿ񱸷ݲ����¼�¼
        if(recfile.size() > MAX_FILE_SIZE)
        {
            if(QFile::exists(filebak)) QFile::remove(filebak);
            recfile.rename(filebak);
        }

        recfile.close();
    }
}

/*
 *��������void RecordTemp(int temp , QString strPath)
 *���ܣ����ı���ʽ��¼�¶����ݣ��ļ�����strPathĿ¼�µ�temp.log��
 *     ��ʽ��ʱ��+"\t\t"+�¶�+"\r\n"
 */
void RecordTemp(QString temp , QString strPath)
{
    const qint64 MAX_FILE_SIZE = 20*1024*1024;//20MB
#if defined (Q_OS_WIN)
    const QString filebak = QString("D:/logs/%1.bak").arg( strPath );
#else
    const QString filebak = QString("logs/%1.bak").arg( strPath );
#endif
    QFile file("logs/"+strPath);   //����д���ļ�
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        qDebug() << "Open file failed!";
        return;
    }
    QTextStream tfile(&file);
    QDateTime dt = QDateTime::currentDateTime();
    tfile<<dt.toString("yy-MM-dd hh:mm:ss:zzz")<<"\t"<<temp<<"\r\n";

    //�����ļ���С��ȷ���Ƿ񱸷ݲ����¼�¼
    if(file.size() > MAX_FILE_SIZE)
    {
        if(QFile::exists(filebak))
            QFile::remove(filebak);
        file.rename(filebak);
    }
    file.close();
}


int CRC16( char* pchMsg,  int wDataLen)
{
    //�㷨У��
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

/*0.0-99999.0֮�����������λ��Ч����*/
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
struct linux_rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
#define RTC_SET_TIME    _IOW('p', 0x0a, struct linux_rtc_time) /* Set RTC time    */
int from_sys_clock()
{
    struct timeval tv;
    struct tm tm_time;
    gettimeofday(&tv, NULL);
    time_t t = tv.tv_sec;/* Prepare tm_time */
    localtime_r(&t, &tm_time);
    tm_time.tm_isdst = 0;

    int rtc;
    rtc = open("/dev/rtc", O_WRONLY);
    if(rtc != -1){
        int err = 0;
        if(ioctl(rtc, RTC_SET_TIME, &tm_time) == -1)
            err = -1;
        if(close(rtc) == -1)
            err  = -2;
        return err;
    }else{
        return -3;
    }
}
#elif defined(Q_WS_WIN)
int set_local_time(const char *time_string)
{
    SYSTEMTIME system_time;
    char year[4 + 1] = {0};
    char month[2 + 1] = {0};
    char day[2 + 1] = {0};
    char hour[2 + 1] = {0};
    char minute[2 + 1] = {0};
    char second[2 + 1] = {0};
    int index = 0;

    strncpy(year, time_string + index, 4);
    index += 4;
    strncpy(month, time_string + index, 2);
    index += 2;
    strncpy(day, time_string + index, 2);
    index += 2;
    strncpy(hour, time_string + index, 2);
    index += 2;
    strncpy(minute, time_string + index, 2);
    index += 2;
    strncpy(second, time_string + index, 2);
    index += 2;

    GetLocalTime(&system_time);

    system_time.wYear = atoi(year);
    system_time.wMonth = atoi(month);
    system_time.wDay = atoi(day);
    system_time.wHour = atoi(hour);
    system_time.wMinute = atoi(minute);
    system_time.wSecond = atoi(second);

    if (0 == SetLocalTime(&system_time))
    {
        return -1;
    }

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

// n > 0 ʱ���ձ���nλС��λ���ķ�ʽȷ������
// n < 0 ʱ����-nλ��Ч���ݵķ�ʽȷ������
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
         // ȷ������С����λ��
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

    //��ʾ��ǰҳ��Ŀ
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
    QFile file(filePath);
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
    QFile file(filePath);
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
