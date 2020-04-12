/*!
\class KeyBoardEnter
\version 1.0.0
\author Veelang

\note
软键盘上层接口
*/


#include "keyboardenter.h"
#include <QPointer>
#include "keyboard.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

/*!
\fn KeyBoardEnter::KeyBoardEnter(QObject *parent)
Default constructor.
*/
KeyBoardEnter::KeyBoardEnter(QObject *parent) :
    QInputContext(parent)
{
    KeyT = new KeyBoard;
    QDesktopWidget* desktopWidget = QApplication::desktop();
    //获取可用桌面大小
    deskRect = desktopWidget->availableGeometry();
    //获取设备屏幕大小
    //QRect screenRect = desktopWidget->screenGeometry();
    connect(KeyT,SIGNAL(sendChar(QChar)),this,SLOT(charSlot(QChar)));
    connect(KeyT, SIGNAL(sendInt(int)), this, SLOT(intSlot(int)));
}

KeyBoardEnter::~KeyBoardEnter()
{
    delete KeyT;
}


QString KeyBoardEnter::identifierName()
{
    return "KeyBoardEnter";
}

void KeyBoardEnter::reset()
{
}

bool KeyBoardEnter::isComposing() const
{
    return false;
}

QString KeyBoardEnter::language()
{
    return "zh_CN";//"en_US";
}

/*!
\fn bool KeyBoardEnter::filterEvent(const QEvent* event)
事件过滤函数
*/
bool KeyBoardEnter::filterEvent(const QEvent* event)
{
    if (event->type() == QEvent::RequestSoftwareInputPanel)
    {
        updatePosition();
        KeyT->show();
        return true;
    }
    else if (event->type() == QEvent::CloseSoftwareInputPanel)
    {
        KeyT->hide();
        return true;
    }
    return false;
}


/*!
\fn void KeyBoardEnter::updatePosition()
软键盘位置更新
*/
void KeyBoardEnter::updatePosition()
{
    QWidget *widget = focusWidget();
    if (!widget)
        return;

    QRect widgetRect = widget->rect();
    QPoint panelPos = QPoint(widgetRect.left(), widgetRect.bottom() + 2);

    panelPos = widget->mapToGlobal(panelPos);

    //qDebug()<<panelPos<<deskRect.width()<<deskRect.height();
    if(panelPos.rx()+385 > deskRect.width())
        panelPos.setX(panelPos.rx() + widgetRect.width() -385);
    if(panelPos.ry()+225 > deskRect.height())
        panelPos.setY(panelPos.ry() - widgetRect.height() - 4 - 225);

    KeyT->move(panelPos);
}

/*!
\fn void KeyBoardEnter::intKey(int Key)
特殊键单独处理
*/
void KeyBoardEnter::intKey(int Key)
{
    QPointer<QWidget> w = focusWidget();

    if (!w)
        return;

    QKeyEvent keyPress(QEvent::KeyPress, Key, Qt::NoModifier, QString());
    QApplication::sendEvent(w, &keyPress);
}


/*!
\fn void KeyBoardEnter::charSlot(QChar character)
数值键处理
*/
void KeyBoardEnter::charSlot(QChar character)
{
    QPointer<QWidget> w = focusWidget();

    if (!w)
        return;

    QKeyEvent keyPress(QEvent::KeyPress, character.unicode(), Qt::NoModifier, QString(character));
    QApplication::sendEvent(w, &keyPress);

    if (!w)
        return;

    QKeyEvent keyRelease(QEvent::KeyPress, character.unicode(), Qt::NoModifier, QString());
    QApplication::sendEvent(w, &keyRelease);

}

/*!
\fn void KeyBoardEnter::intKey(int Key)
特殊键总体处理
*/
void KeyBoardEnter::intSlot(int key)
{
    QPointer<QWidget> w = focusWidget();

    if (!w)
        return;
    switch(key)
    {
    case 51:
        intKey(Qt::Key_Left);
        break;
    case 52:
        intKey(Qt::Key_Right);
        break;
    case 54:
        intKey(Qt::Key_Backspace);
        break;
    case 55:
        intKey(Qt::Key_Enter);
        break;
    case 56:
        KeyT->hide();
        break;
    default:
        return;
    }
}
