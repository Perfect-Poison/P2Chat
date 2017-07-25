#ifndef P2TEST_H
#define P2TEST_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QtWidgets/QItemDelegate>
#include "ui_p2test.h"
#include "common.h"


class QMenu;

class AttrsTableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    AttrsTableDelegate(QObject *parent = nullptr) :QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

class P2Test : public QMainWindow
{
	Q_OBJECT

public:
	P2Test(QWidget *parent = 0);
	~P2Test();
    void init();
    private slots:
    void showContextMenu(const QPoint&);
    void addOneRow();
    void delOneRow();
private:
	Ui::P2TestClass ui;
    QUdpSocket *fUdpSocket;
    QMenu *fContextMenu;
    QAction *fAddAction;
    QAction *fDelAction;
};

#endif // P2TEST_H
