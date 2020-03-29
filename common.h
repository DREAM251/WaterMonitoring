#ifndef COMMON_H
#define COMMON_H

#include <QStringList>


QStringList loadCommandFileLines(const QString &filePath);

bool saveCommandFile(const QStringList &lines, const QString &filePath);


#endif // COMMON_H
