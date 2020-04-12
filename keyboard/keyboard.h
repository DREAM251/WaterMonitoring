#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include "ui_keyboard.h"
#include <QEvent>
#include <QMouseEvent>
#include <QList>
#include <QChar>
#include <QString>
#include <QPoint>

class KeyBoardEnter;


class KeyBoard : public QWidget, public Ui_KeyBoard
{
    Q_OBJECT

public:
    explicit KeyBoard(QWidget *parent = 0);
    ~KeyBoard();


private:

    QList<QPushButton *> btns;

    QWidget *lastFocusedWidget;

    bool should_move;
    QPoint  mouse_pos;
    QPoint  widget_pos;

    void myMapping();
    void mousePressEvent(QEvent *e);
    void mouseReleaseEvent(QEvent *e);
    void mouseMoveEvent(QEvent *e);

protected:
    bool event(QEvent *e);

private Q_SLOTS:
    void btnClicked(QWidget *w);
    void saveFocusWidget(QWidget *oldFocus, QWidget *newFocus);

Q_SIGNALS:
    void sendChar(QChar ch);
    void sendInt(int key);
};

#endif // KEYBOARD_H
