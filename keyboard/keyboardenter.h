#ifndef KEYBOARDENTER_H
#define KEYBOARDENTER_H

#include <QInputContext>
class KeyBoard;

class KeyBoardEnter : public QInputContext
{
    Q_OBJECT
public:
    explicit KeyBoardEnter(QObject *parent = 0);
    ~KeyBoardEnter();

    QString identifierName();
    QString language();
    bool isComposing() const;
    void reset();

    bool filterEvent(const QEvent* event);

private:
    KeyBoard *KeyT;
    QRect deskRect;
    void updatePosition();
    void intKey(int Key);

public Q_SLOTS:
    void charSlot(QChar character);
    void intSlot(int key);

};

#endif // KEYBOARDENTER_H
