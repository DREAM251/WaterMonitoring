#include "common.h"
#include <QFile>

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
