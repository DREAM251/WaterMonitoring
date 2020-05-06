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
#include <QSqlDatabase>


QStringList loadCommandFileLines(const QString &filePath);
bool saveCommandFile(const QStringList &lines, const QString &filePath);

QString  Int2Hex(int a , int b);
QString  Float4(float value);
int  CRC16( char* pchMsg, unsigned int wDataLen);
void  print_callstack();
bool  copyFiles(QString file,QString tagFile);
int  setSystemTime(const char *time_string); // yyyy-MM-dd hh:mm:ss


bool isNan(float f);
bool isNan(double f);
bool isInf(float f);
bool isInf(double f);
float setPrecision(float v, int n, QString *pshow=NULL);

// 提取数据库中某一条数据，index仿python用法，-1代码最后一条数据
bool readSqlValues(QSqlDatabase *db, const QString &table, QStringList &value, int index = -1);
void addMeasureData(QList<QVariant> &data);
void addErrorMsg(QString strMsg, int level);
void addLogger(QString strMsg);
bool getUserDataBase(QSqlDatabase &db);

class DriverSelectionDialog : protected QDialog
{
    Q_OBJECT
public:
    DriverSelectionDialog (QWidget *parent=NULL);

    int showModule();
    QString getSelectedDriver();
    void addExclusiveDriver(const QString &dri);

private:
    QVBoxLayout *layout;
    QStringList dir ;
    QStringList info ;
    QList<QRadioButton *> btnList;
    QStringList exclusives;

private slots:
    void ok();
    void cancel();

public:
    static void GetDrives(QStringList &driverDir , QStringList &driverInfo, QStringList &exclusive);
};


/**
 * @brief Reclog 记录信息到文件，如果记录的信息大于MAX_FILE_SIZE，则将数据备份到
 *                 Log/rec?.bak文件当中，然后重新记录
 * @param Msg 需要记录的信息
 */
class LOG_WRITER
{
protected:
    LOG_WRITER();
    static void writeLog(QString Msg1 , QString Msg2 , QString prefix , int fcount);

public:
    static LOG_WRITER *getObject(const QString &filePath);

    void notice( const QString &x );
    void info( const QString &x );
    void debug(const QString &x);
    void error(  const QString &x  );
    void fatal(  const QString &x  );
    void setCount(int c);

protected:
    QString filePath;
    int fcount;
};

#endif // COMMON_H
