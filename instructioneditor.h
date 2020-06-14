#ifndef _COMMANDEDIT_EX__H__
#define _COMMANDEDIT_EX__H__

#include <QWidget>
#include <QGridLayout>
#include <QDebug>
#include <QPushButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QTableView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

struct ColumnInfo
{
    enum ColumnDelegateType {
        CDT_None,
        CDT_Combox
    };

    QString header;
    int itembytes;
    QString defaultString;

    ColumnDelegateType type;
    QString delegateParameter;
};

struct CommondFileInfo {
    QString fileName;
    QString path;
};


class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(const QString& itemList,QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QStringList curItemStrList;
    QStringList explainList;
};


class InstructionEditor : public QWidget
{
    Q_OBJECT

public:
    explicit InstructionEditor(const QList<ColumnInfo> &header,
                               const QList<CommondFileInfo> &files,
                               QWidget *parent = 0 );
    void setReadOnly(bool v);

public Q_SLOTS:
    void FileIndexChanged(int idx);
    void Save();
    void Cancel();
    void Copy();
    void Paste();
    void Insert();
    void Remove();
    void UpdateFile();

private:
    QStringList analysisOneLine(const QString &oneline);
    QString combineOneLine(const QStringList &misc);

    QStringList getColumnNames(const QList<ColumnInfo> &ci);

private:
    QTableView *tableView;
    QStandardItemModel *tableModel;
    QPushButton *insertBtn;
    QPushButton *removeBtn;
    QPushButton *okBtn;
    QPushButton *updateBtn;
    QPushButton *cancelBtn;
    QPushButton *pasteBtn;
    QPushButton *copyBtn;
    QComboBox *fileSelect;

    QList<ColumnInfo> header;
    QList<CommondFileInfo> files;
    CommondFileInfo currentFile;
};

#endif // COMMANDEDIT_H
