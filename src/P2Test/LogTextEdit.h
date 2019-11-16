#ifndef LOGTEXTEDIT_H
#define LOGTEXTEDIT_H

#include <QTextEdit>
#include "ui_LogTextEdit.h"

class LogTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    LogTextEdit(QWidget *parent = 0);
    ~LogTextEdit();
    void mouseDoubleClickEvent(QMouseEvent *e);
    signals:
    void msgUpdate(const QByteArray&);
private:
    Ui::LogTextEdit ui;
};

#endif // LOGTEXTEDIT_H
