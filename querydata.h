#ifndef QUERYDATA_H
#define QUERYDATA_H

#include "ui_querydata.h"
#include <QDialog>
#include <QThread>
class QSqlDatabase;
class QStandardItemModel;
class QueryData;
class myThread : public QThread
{
    Q_OBJECT
public:
    explicit myThread(QObject *parent = 0);


protected:
    void run();

    void readData(QString &table,QString &filename);

private:

signals:
    void isDone(QStringList str);
public slots:

};

class QueryData : public QWidget ,public Ui_QueryData
{
    Q_OBJECT
    
public:
    explicit QueryData(int column = 8, int row = 12, QWidget* parent = NULL);
    ~QueryData();

private:
    int column,row;
    int itemcount;//行数
    int curpage;//当前页码
    QStandardItemModel *model;
    void InitModel();
    void InitSlots();
    void queryData(int page);
    void getQueryTotalItemCountAndEndId();
    int getTotalPages(int totalItemCnt, int row);
    void PrinterPageData();
private slots:
    void slot_QueryJumpPages(int);
    void slot_QueryFirst();
    void slot_QueryLast();
    void slot_QueryNext();
    void slot_QueryFinal();
    void slot_cbSearch(int);
    void slot_QueryDateChange(QDate);
    void slot_PrinterData(QModelIndex);
    void slot_PrinterSelectUi();
    void dealDone(QStringList qstr);
signals:
    void sigPrinterData(QStringList);
    void sigPrinterPageData(QStringList);
protected:
    void paintEvent(QPaintEvent *);
private:
    QStringList name;
    QList<int> width;
    QString table;
    QString items;
    int queryItemsEndID;    /*检索方式的最终ID序号*/
    QStringList printerPageData;
    QStringList printerData;
    myThread *thread;
public:
    void setHeaderName(int i , const QString &name);
    void setColumnWidth(int i,int nwidth);
    void setSqlString(QString &table, QString &items);
    void setLabel(QString &label);
    void UpdateModel();
    void hidePrinterBut(bool);
    void setSQLDatabase(QSqlDatabase *);
    void setColumnIsHidden(int, bool);
    void initFirstPageQuery();
    int runInthread(QString &table);

};

#endif // QUERYDATA_H
