#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include <QWidget>

namespace Ui {
class SystemWindow;
}

class SystemWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SystemWindow(QWidget *parent = 0);
    ~SystemWindow();

public slots:
    void screenCalibration();
    void platformSelect(int i);
    void updateProgram();
    void setTime();
private slots:
    void on_pushButton_clicked();
private:
    Ui::SystemWindow *ui;
};

#endif // SYSTEMWINDOW_H
