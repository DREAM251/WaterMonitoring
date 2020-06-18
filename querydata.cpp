#include "querydata.h"
#include "common.h"
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QStringList>
#include <QSqlTableModel>
#include "globelvalues.h"

myThread::myThread(QObject *parent) :
    QThread(parent)
{
}


void myThread::run()
{
//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    QString stdata = QString("/dist/%1/UserData.db").arg(elementPath);
//    db.setDatabaseName(stdata);
//    db.open();


   QString tablename1 = "data";
   QString filename1 = QString("/dist/%1/csv/data.csv").arg(elementPath);

   QString tablename2 = "error";
   QString filename2 = QString("/dist/%1/csv/error.csv").arg(elementPath);

   QString tablename3 = "calibration";
   QString filename3 = QString("/dist/%1/csv/calibration.csv").arg(elementPath);

   QString tablename4 = "log";
   QString filename4 = QString("/dist/%1/csv/log.csv").arg(elementPath);

   QString tablename5 = "qc";
   QString filename5 = QString("/dist/%1/csv/qc.csv").arg(elementPath);

   readData(tablename1,filename1);
   readData(tablename2,filename2);
   readData(tablename3,filename3);
   readData(tablename4,filename4);
   readData(tablename5,filename5);

   QStringList s ;
   s.append(tablename1);
   s.append(tablename2);
   s.append(tablename3);
   s.append(tablename4);
   s.append(tablename5);
   emit isDone(s);
}

void myThread::readData(QString &table, QString &filename)
{

    QSqlTableModel *exportModel = new QSqlTableModel();
    exportModel->setTable(table);
    exportModel->select();
    QStringList strList;//记录数据库中的一行报警数据
    QString strString;

    QFile csvFile(filename);
    if (csvFile.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&csvFile);
        stream.setCodec("UTF-8");
        for(int i=0;i<exportModel->columnCount();i++)
        {
            stream << exportModel->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()<<",";
            if(i==exportModel->columnCount()-1)
            {
                stream << "\n";
            }
        }
        for (int i=0;i<exportModel->rowCount();i++)
        {
            for(int j=0;j<exportModel->columnCount();j++)
            {
                if(j==1)
                {
                    continue;
                }
                strList.insert(j,exportModel->data(exportModel->index(i,j)).toString());//把每一行的每一列数据读取到strList中
            }
            strString = strList.join(", ")+"\n";//给两个列数据之前加“,”号，一行数据末尾加回车
            strList.clear();//记录一行数据后清空，再记下一行数据
            stream << strString;
            //csvFile.write(strString.toUtf8());//使用方法：转换为Utf8格式后在windows下的excel打开是乱码,可先用notepad++打开并转码为unicode，再次用excel打开即可。
            qDebug()<<strString.toUtf8();
        }
        csvFile.close();

    }

}
QueryData::QueryData(int ucolumn, int urow, QWidget* parent) :
    QWidget(parent , Qt::FramelessWindowHint) , row(urow)
{
    this->setParent(parent);
    thread = new myThread(this);
    connect(thread,SIGNAL(isDone(QStringList)),this,SLOT(dealDone(QStringList)),Qt::QueuedConnection);
    setupUi(this);
    column = ucolumn;
    for(int i=0;i<column;i++){
        name+=tr("行%1").arg(i);
        width+=150;
    }
    table = "data8";
    items = "Time,COD,NH3N,TP,PH,TDS,DO,TSS,TEMP";
    itemcount = 0;
    curpage = 1;
    queryItemsEndID = 0;
    endDate->setDate(QDate::currentDate());
    InitModel();
    InitSlots();
    cwDateSel->setSelectedDate(QDate::currentDate());
    swPageSel->setCurrentIndex(0);
    swSearch->setCurrentIndex(0);
    //    hidePrinterBut(true);
}

QueryData::~QueryData()
{
    delete model;
}
void QueryData::InitModel()
{
    model = new QStandardItemModel(row, column, this);
    tvShowData->setModel(model);//设置模式
    tvShowData->setSelectionBehavior(QAbstractItemView::SelectRows);
    tvShowData->setEditTriggers(QAbstractItemView::NoEditTriggers);//不能编辑模式
    tvShowData->horizontalHeader()->setFont(QFont("Sans Serif", 9,QFont::Black));
    tvShowData->setFont(QFont("Sans Serif", 8));
    for(int i=0;i<column;i++){
        model->setHeaderData(i, Qt::Horizontal, name.at(i));//设置表头
        tvShowData->setColumnWidth(i, width.at(i));
    }
    for(int i = 0; i<row; i++)
    {
        tvShowData->setRowHeight(i,26);
    }
}
void QueryData::UpdateModel()
{
    for(int i=0;i<column;i++)
    {
        model->setHeaderData(i, Qt::Horizontal, name.at(i));//设置表头
        tvShowData->setColumnWidth(i, width.at(i));
    }
}
void QueryData::hidePrinterBut(bool level)
{
    pbPrinter->setEnabled(level);
    pbPrinter->setVisible(false);
}
void QueryData::setSQLDatabase(QSqlDatabase *db)
{
}
/**
 * @brief QueryData::setColumnIsHidden
 * 设置隐藏列
 * @param column
 * @param value
 */
void QueryData::setColumnIsHidden(int column, bool value)
{
    tvShowData->setColumnHidden(column,value);
}

/**
 * @brief QueryData::initFirstPageQuery
 * 初始化首页查询
 */
void QueryData::initFirstPageQuery()
{
    slot_QueryFirst();
}

int QueryData::runInthread(QString &table)
{
    QString filepath = "/myfile";

    DriverSelectionDialog dsd;
    dsd.addExclusiveDriver("/dev/root");
    dsd.addExclusiveDriver("/dev/mmcblk0p3");
    dsd.addExclusiveDriver("/dev/mmcblk0p7");

    if(dsd.showModule())
    {
        QString strTargetDir = dsd.getSelectedDriver();
        qDebug()<<QString("%1").arg(strTargetDir);

        if(QFile::exists(strTargetDir))
        {
            QString strSource1 = strTargetDir + filepath;

             system(QString("cp /dist/%1/csv/%2.csv %3").arg(elementPath).arg(table).arg(strSource1).toLatin1().data());

             QString listcsv = QString("/%1.csv").arg(table);
             QString fullFileName = strSource1 + listcsv;
             QFileInfo fileInfo(fullFileName);
             if(fileInfo.isFile())
             {
                 QMessageBox::information(NULL, tr("提示"),tr("导出成功！"));
                 return 0;
             }
             else
                 return -1;
        }
    }
}

void QueryData::InitSlots()
{
    connect(cbSearch, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbSearch(int)));
    connect(spJumpPage, SIGNAL(valueChanged(int)), this, SLOT(slot_QueryJumpPages(int)));
    connect(pbFristPage, SIGNAL(clicked()), this, SLOT(slot_QueryFirst()));
    connect(pbFinalPage, SIGNAL(clicked()), this, SLOT(slot_QueryFinal()));
    connect(pbLastPage,  SIGNAL(clicked()),this,SLOT(slot_QueryLast()));
    connect(pbNextPage,  SIGNAL(clicked()),this,SLOT(slot_QueryNext()));
    connect(endDate,     SIGNAL(dateChanged(QDate)), this, SLOT(slot_QueryDateChange(QDate)));
    connect(tvShowData,  SIGNAL(clicked(QModelIndex)), this, SLOT(slot_PrinterData(QModelIndex)));
    connect(pbPrinter,  SIGNAL(clicked()), this, SLOT(slot_PrinterSelectUi()));
}

//跳转页号
void QueryData::slot_QueryJumpPages(int page)
{
    curpage = page;
    queryData(curpage);
}

//首页
void QueryData::slot_QueryFirst()
{
    getQueryTotalItemCountAndEndId();
    curpage = 1;
    queryData(curpage);
}

//上一页
void QueryData::slot_QueryLast()
{
    if(curpage == 1)
    {
        slot_QueryFirst();
    }
    else
    {
        curpage -= 1;
        queryData(curpage);
    }
}

//下一页
void QueryData::slot_QueryNext()
{
    curpage += 1;
    queryData(curpage);
}

//尾页
void QueryData::slot_QueryFinal()
{
    curpage = getTotalPages(itemcount, row);
    queryData(curpage);
}

void QueryData::slot_cbSearch(int index)
{
    swSearch->setCurrentIndex(index);
}

void QueryData::slot_QueryDateChange(QDate)
{
    QSqlDatabase sqldb;
    if (!getUserDataBase(sqldb)){
        lbPages->setText(tr("未连接数据库！"));
        return ;
    }

    QString timeID1 = endDate->date().toString("yyyyMMdd")+"999999";
    QString strquery = QString("SELECT %1 FROM %2 WHERE TimeID < %3")
            .arg(items)
            .arg(table)
            .arg(timeID1);

    QSqlQuery query(strquery,sqldb);
    query.last();
    int currentItemCnt = query.at() + 1;
    curpage = getTotalPages(itemcount, row) - currentItemCnt/row;
    queryData(curpage);
}


/**
 * @brief QueryData::getQueryTotalItemCountAndEndId
 * 获取总的条目数和检索方式的最终ID
 */
void QueryData::getQueryTotalItemCountAndEndId()
{
    QSqlDatabase sqldb;
    if (!getUserDataBase(sqldb)){
        lbPages->setText(tr("未连接数据库！"));
        return ;
    }

    QString strquery;
    strquery = QString("SELECT ID,%1 FROM %2").arg(items).arg(table);
    QSqlQuery query(strquery,sqldb);
    query.last();

    itemcount = query.at() + 1;
    queryItemsEndID = query.value(0).toInt();
}

/**
 * @brief QueryData::getTotalPages
 * 页数目
 * @param totalItemCnt
 * 总的条数
 * @param row
 * 行数
 * @return
 * 页总数
 */
int QueryData::getTotalPages(int totalItemCnt, int row)
{
    if( totalItemCnt % row == 0)
    {
        return totalItemCnt/row;
    }
    else
    {
        return totalItemCnt/row + 1;
    }
}

/**
 * @brief QueryData::queryData
 * 查询显示
 * @param page
 * curpage
 */
void QueryData::queryData(int page)
{
    QSqlDatabase sqldb;
    if (!getUserDataBase(sqldb)){
        lbPages->setText(tr("未连接数据库！"));
        return ;
    }
    int totalPagesCnt = getTotalPages(itemcount, row);
    if( page < 1 )
        curpage = 1;
    else if(page > totalPagesCnt )
        curpage = totalPagesCnt;

    int queryEndID = (queryItemsEndID - ((curpage-1)*row) );
    int queryStartID = queryEndID - (row -1);

    QString strquery = QString("SELECT %1 FROM %2  where ID >= %3 and ID <= %4 order by TimeID desc")
            .arg(items)
            .arg(table)
            .arg(queryStartID)
            .arg(queryEndID);
    QSqlQuery query(strquery,sqldb);

    //清除所有条目防止下一页的条目数目小于row数从而显示上一页的相对应条目
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<column;j++)
            model->setData(model->index(i,j), " ");
    }
    //显示当前页条目
    int index = 0;
    while(query.next())
    {
        if(index >= 0 && index < row)
        {
            for(int j=0;j<column;j++)
            {
                QString str = query.value(j).toString();
                if (!str.endsWith("   "))
                {
                    model->item(index%row,j)->setText(str);
                    model->item(index%row,j)->setBackground(Qt::white);
                    model->item(index%row,j)->setForeground(Qt::black);
                }else{
                    model->item(index%row,j)->setText(str.remove(" "));
                    model->item(index%row,j)->setBackground(Qt::red);
                    model->item(index%row,j)->setForeground(Qt::white);
                }
            }
        }
        index++;
    }
    lbPages->setText(tr("页码：%1/%2").arg(curpage).arg( totalPagesCnt ));
}

void QueryData::setHeaderName(int i ,const QString& name1)
{
    name.replace(i,name1);
}

void QueryData::setColumnWidth(int i,int width1)
{
    width.replace(i,width1);
}

void QueryData::setSqlString(QString &table1, QString &items1)
{
    table = table1;
    items = items1;
}

void QueryData::setLabel(QString &)
{
}


////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief QueryData::PrinterPageData
 *选中当前页数据
 */
void QueryData::PrinterPageData()
{
    QString middata = "";
    QModelIndex pageIndex;
    printerPageData.clear();
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<2; j++)
        {
            pageIndex = model->index(i,j);
            middata = model->data(pageIndex).toString();
            printerPageData.append(middata);
        }
    }
}

/**
 * @brief QueryData::slot_PrinterData
 *选中当前行数据
 * @param index
 */
void QueryData::slot_PrinterData(QModelIndex index)
{
    printerData.clear();
    QModelIndex selectIndex;
    QString midStr="";
    for(int i(0); i<=1; i++)
    {
        selectIndex = model->index(index.row(),i);
        midStr = model->data(selectIndex).toString();
        printerData.append(midStr);
    }
}

//void QueryData::readData(QString &table, QString &filename)
//{



//}
void QueryData::slot_PrinterSelectUi()
{
    thread->start();

}

void QueryData::dealDone(QStringList qstr)
{
    for(int i=0;i<qstr.count();i++)
    {
        QString s = qstr.at(i);
        runInthread(s);
        qDebug() << i;

    }
    qDebug() << 8;
    thread->quit();
    thread->wait();
    qDebug() << 9;
}

void QueryData::paintEvent(QPaintEvent *)
{
    if(curpage == 1)
        pbLastPage->setEnabled(false);
    else
        pbLastPage->setEnabled(true);

    if( curpage == getTotalPages(itemcount, row) )
        pbNextPage->setEnabled(false);
    else
        pbNextPage->setEnabled(true);
}

