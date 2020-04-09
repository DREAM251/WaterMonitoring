#ifndef USERDLG_H
#define USERDLG_H

#include "loginmanage.h"
#include <QFrame>

enum AccessLevel{
    al_notlogin = 0,
    al_operation,
    al_admin,
    al_supper
};

namespace Ui {
class UserDlg;
}

class UserDlg : public QFrame
{
    Q_OBJECT
    
public:
    explicit UserDlg(QWidget *parent = 0);
    ~UserDlg();
    
private slots:
    void slot_back();
    void slot_Logout();
    void on_lbChangePW_linkActivated(const QString &link);
    void on_lbForgetPW_linkActivated(const QString &link);
    void on_pbLogin_clicked();
    void on_pbChangePW_clicked();
    void on_pbForgetPW_clicked();
    void slot_showMain();

protected:
    virtual void showEvent(QShowEvent *);

Q_SIGNALS:
    void back();
    void login(int level);

private:
    Ui::UserDlg *ui;
    int loginLevel;
    LoginManage *gpLoginManage;
};

#endif // USERDLG_H
