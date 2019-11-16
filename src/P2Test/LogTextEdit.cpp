#include "LogTextEdit.h"

LogTextEdit::LogTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    ui.setupUi(this);
}

LogTextEdit::~LogTextEdit()
{

}

void LogTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextCursor cur = textCursor();
    cur.select(QTextCursor::LineUnderCursor);
    setTextCursor(cur);
    QString msg = cur.selectedText();
    msg = msg.mid(msg.lastIndexOf(QChar(':'))+1);
    QByteArray rawMsg(QByteArray::fromHex(msg.toLatin1()));
    if (!msg.isEmpty())
        emit msgUpdate(rawMsg);
}
