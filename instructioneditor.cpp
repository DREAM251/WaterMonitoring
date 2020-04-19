#include "instructioneditor.h"
#include <QMessageBox>
#include <QFile>
#include <QStandardItemModel>
#include <QFile>
#include <QApplication>
#include <QClipboard>

ComboBoxDelegate::ComboBoxDelegate(const QString &itemList, QObject* parent):
    QStyledItemDelegate(parent)
{
    curItemStrList = itemList.split(",");
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    QComboBox *curComboBox = new QComboBox(parent);
    curComboBox->setFixedHeight(option.rect.height());
    curComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    curComboBox->addItems(curItemStrList);
    curComboBox->installEventFilter(const_cast<ComboBoxDelegate*>(this));
    return curComboBox;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *curComboBox = dynamic_cast<QComboBox*>(editor);
    if(curComboBox)
    {
        int curIndex = index.model()->data(index).toInt();
        curComboBox->setCurrentIndex(curIndex);
    }
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *curComboBox = dynamic_cast<QComboBox*>(editor);

    if(curComboBox)
    {
        model->setData(index, curComboBox->currentIndex());
    }
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}



InstructionEditor::InstructionEditor(const QList<ColumnInfo> &h,
                                     const QList<CommondFileInfo> &f,
                                     QWidget *parent) :
    QWidget(parent),
    header(h),
    files(f)
{
    tableModel = new QStandardItemModel();
    tableModel->setColumnCount(header.count());
    tableModel->setHorizontalHeaderLabels(getColumnNames(header));

    tableView = new QTableView();
    tableView->setModel(tableModel);
    tableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    for (int i = 0; i < header.count(); i++)
    {
        switch (header[i].type)
        {
        case ColumnInfo::CDT_Combox:
            tableView->setItemDelegateForColumn(i, new ComboBoxDelegate(header[i].delegateParameter));
            break;
        default:
            break;
        }
    }


    fileSelect = new QComboBox();
    for (int i = 0; i < files.count(); i++)
        fileSelect->addItem(files[i].fileName);

    insertBtn = new QPushButton(tr("插入"));
    removeBtn = new QPushButton(tr("移除"));
    copyBtn = new QPushButton(tr("复制"));
    pasteBtn = new QPushButton(tr("粘贴"));
    updateBtn = new QPushButton(tr("更换命令"));
    okBtn = new QPushButton(tr("保存"));
    cancelBtn = new QPushButton(tr("刷新"));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(tableView,0,0,1,10);
    gridLayout->addWidget(fileSelect,1,0);
    gridLayout->addWidget(insertBtn,1,2);
    gridLayout->addWidget(removeBtn,1,3);
    gridLayout->addWidget(copyBtn,1,4);
    gridLayout->addWidget(pasteBtn,1,5);
    gridLayout->addWidget(updateBtn,1,7);
    gridLayout->addWidget(okBtn,1,8);
    gridLayout->addWidget(cancelBtn,1,9);
    setLayout(gridLayout);

    connect(fileSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(FileIndexChanged(int)));

    connect(insertBtn, SIGNAL(clicked()), this, SLOT(Insert()));
    connect(removeBtn, SIGNAL(clicked()), this, SLOT(Remove()));
    connect(copyBtn, SIGNAL(clicked()), this, SLOT(Copy()));
    connect(pasteBtn, SIGNAL(clicked()), this, SLOT(Paste()));

    connect(updateBtn,SIGNAL(clicked()),this, SLOT(UpdateFile()));
    connect(okBtn, SIGNAL(clicked()), this, SLOT(Save()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(Cancel()));

    Cancel();
}


void InstructionEditor::setReadOnly(bool b)
{
    bool v = !b;
    insertBtn->setEnabled(v);
    removeBtn->setEnabled(v);
    copyBtn->setEnabled(v);
    pasteBtn->setEnabled(v);
    updateBtn->setEnabled(v);
    okBtn->setEnabled(v);
}

void InstructionEditor::FileIndexChanged(int idx)
{
    if (idx >= 0 && idx < files.count())
    {
        QStringList cl = loadCommandFileLines(files[idx].path);
        currentFile = files[idx];

        tableModel->setRowCount(cl.count());
        for (int i = 0; i < cl.count(); i++)
        {
            QStringList cc = analysisOneLine(cl[i]);

            for (int j = 0; j < cc.count() && j < tableModel->columnCount(); j++)
            {
                tableModel->setItem(i, j, new QStandardItem(cc[j]));
            }
        }

        tableView->selectionModel()->clear();
    }
}

void InstructionEditor::Save()
{
    if (currentFile.fileName.isEmpty())
        return;

    int column = tableModel->columnCount();
    int row = tableModel->rowCount();
    QStringList lines;

    for (int i = 0; i < row; i++)
    {
        QStringList lineData;
        for (int j = 0; j < column; j++)
        {
            QStandardItem *item = tableModel->item(i,j);
            if (item)
                lineData << item->text();
            else
                lineData << "";
        }
        lines << combineOneLine(lineData);
    }
    saveCommandFile(lines, currentFile.path);
}

void InstructionEditor::Cancel()
{
    FileIndexChanged(fileSelect->currentIndex());
}

void InstructionEditor::Copy()
{
    QItemSelectionModel *selectModel = tableView->selectionModel();
    QString copyStr;

    if (!selectModel->hasSelection())
        return;

    QModelIndexList ml = selectModel->selectedIndexes();
    int firstRow = -1;

    for (int i = 0; i < ml.count(); i++)
    {
        if (firstRow < 0)
            firstRow = ml[i].row();
        else if (firstRow == ml[i].row())
            copyStr += "\n";
        else
            copyStr += "\t";

        copyStr += ml[i].data().toString();
    }

    QApplication::clipboard()->setText(copyStr);
}

void InstructionEditor::Paste()
{
    QString pasteStr = QApplication::clipboard()->text();
    QItemSelectionModel *selectModel = tableView->selectionModel();

    if (pasteStr.isEmpty())
        return;

    if (!selectModel->hasSelection())
        return;

    QModelIndexList ml = selectModel->selectedIndexes();
    QStringList linelist = pasteStr.split("\n");
    int firstRow = ml[0].row();
    int firstColumn = ml[0].column();
    int maxRow = tableModel->rowCount();
    int maxColumn = tableModel->columnCount();

    for (int j = 0; j + firstColumn < maxColumn && j < linelist.count(); j++)
    {
        QStringList columnList = linelist[j].split("\t");
        for (int i = 0; i + firstRow < maxRow && i < columnList.count(); i++)
        {
            tableModel->setItem(i + firstRow, j + firstColumn, new QStandardItem(columnList[i]));
        }
    }
}

void InstructionEditor::Insert()
{
    QItemSelectionModel *selectModel = tableView->selectionModel();
    QModelIndexList ml = selectModel->selectedIndexes();
    int row = 0;
    if (!selectModel->hasSelection())
    {
        row = tableModel->rowCount();
    }
    else
    {
        row = ml.last().row();
    }
    tableModel->insertRow(row);
}

void InstructionEditor::Remove()
{
    QItemSelectionModel *selectModel = tableView->selectionModel();
    if (!selectModel->hasSelection())
        return;

    QModelIndexList ml = selectModel->selectedIndexes();
    tableModel->removeRow(ml[0].row());
}

void InstructionEditor::UpdateFile()
{
}

////////////////////////////////////////////////////////////////////////////

QStringList InstructionEditor::getColumnNames(const QList<ColumnInfo> &ci)
{
    QStringList list;

    for (int i = 0; i < ci.count(); i++)
        list << ci[i].header;

    return list;
}

QStringList InstructionEditor::loadCommandFileLines(const QString &filePath)
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

bool InstructionEditor::saveCommandFile(const QStringList &lines, const QString &filePath)
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

QStringList InstructionEditor::analysisOneLine(const QString &oneline)
{
    QStringList ld;
    int left = 0;
    int len = 0;
    int maxLen = oneline.count();

    for (int i = 0; i < header.count() && left < maxLen; i++)
    {
        len = header[i].itembytes;
        ld << oneline.mid(left, len);
        left += len;
    }
    return ld;
}

QString InstructionEditor::combineOneLine(const QStringList &misc)
{
    QString str;
    int c = misc.count();


    for (int i = 0; i < header.count(); i++)
    {
        QString itemData;

        if (i < c)
            itemData = misc[i];
        else
            itemData = header[i].defaultString;

        if (itemData.isEmpty())
            itemData = header[i].defaultString;
        else if (itemData.length() != header[i].itembytes)
            itemData = (QString(header[i].itembytes, QChar('0')) + itemData).right(header[i].itembytes);
        str += itemData;
    }
    return str;
}
