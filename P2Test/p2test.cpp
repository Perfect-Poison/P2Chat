#include "p2test.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QTableWidget>

void AttrsTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0 || index.column() == 1 || index.column() == 2)
    {
        int enumVal = index.model()->data(index, Qt::DisplayRole).toInt();
        QString text = "";
        if (index.column() == 0)
            text = find_attr_code_param_by_enum(enumVal).strVal;
        else if (index.column() == 1)
            text = find_attr_flags_param_by_enum(enumVal).strVal;
        else if (index.column() == 2)
            text = find_attr_datatype_param_by_enum(enumVal).strVal;

        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignHCenter | Qt::AlignVCenter;

        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
    }
    else
    {
        QItemDelegate::paint(painter, option, index);
    }
}

QWidget * AttrsTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    if (index.column() == 0)
    {
        for (size_t i = 0; i < ATTR_CODE_NUM; i++)
            editor->addItem(sAttrCodeParam[i].strVal, sAttrCodeParam[i].enumVal);
    }
    else if (index.column() == 1)
    {
        for (size_t i = 0; i < ATTR_FLAGS_NUM; i++)
            editor->addItem(sAttrFlagsParam[i].strVal, sAttrFlagsParam[i].enumVal);
    }
    else if (index.column() == 2)
    {
        for (size_t i = 0; i < ATTR_DATA_TYPE_NUM; i++)
            editor->addItem(sAttrDataTypeParam[i].strVal, sAttrDataTypeParam[i].enumVal);
    }
    else
        return QItemDelegate::createEditor(parent, option, index);
    return editor;
}

void AttrsTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == 0 || index.column() == 1 || index.column() == 2)
    {
        int enumVal = index.model()->data(index, Qt::DisplayRole).toInt();
        QString text = "";
        if (index.column() == 0)
            text = find_attr_code_param_by_enum(enumVal).strVal;
        else if (index.column() == 1)
            text = find_attr_flags_param_by_enum(enumVal).strVal;
        else if (index.column() == 2)
            text = find_attr_datatype_param_by_enum(enumVal).strVal;
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
        comboBox->setCurrentIndex(comboBox->findText(text));
    }
    else
    {
        QItemDelegate::setEditorData(editor, index);
    }
}

void AttrsTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == 0 || index.column() == 1 || index.column() == 2)
    {
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
        QString text = comboBox->currentText();
        int enumVal = 0;
        if (index.column() == 0)
            enumVal = find_attr_code_param_by_str(text.toLatin1().data()).enumVal;
        else if (index.column() == 1)
            enumVal = find_attr_flags_param_by_str(text.toLatin1().data()).enumVal;
        else if (index.column() == 2)
            enumVal = find_attr_datatype_param_by_str(text.toLatin1().data()).enumVal;
        model->setData(index, enumVal, Qt::DisplayRole);
    }
    else
    {
        QItemDelegate::setModelData(editor, model, index);
    }
}

P2Test::P2Test(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
    init();
}

P2Test::~P2Test()
{

}

void P2Test::init()
{
    /**
     *	消息头
     */
    for (size_t i = 0; i < MSG_CODE_NUM; i++) 
        ui.msgCodeCombo->addItem(sMsgCodeParam[i].strVal, sMsgCodeParam[i].enumVal);

    for (size_t i = 0; i < MSG_FLAGS_NUM; i++)
        ui.msgFlagsCombo->addItem(sMsgFlagsParam[i].strVal, sMsgFlagsParam[i].enumVal);

    ui.msgIDLineEdit->setText("0");

    ui.attrNumLineEdit->setText("0");

    /**
     *	消息载荷
     */
    fContextMenu = new QMenu(this);
    fAddAction = new QAction(tr("增加"), this);
    fDelAction = new QAction(tr("删除"), this);
    fContextMenu->addAction(fAddAction);
    fContextMenu->addAction(fDelAction);
    ui.attrsTable->setItemDelegate(new AttrsTableDelegate(ui.attrsTable));


    /**
     *	Server
     */
    ui.ipLineEdit->setText("127.0.0.1");
    ui.portLineEdit->setText(QString::number(SERVER_PORT_FOR_UDP));

    /**
     *	信号槽
     */
    connect(ui.attrsTable, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    connect(fAddAction, SIGNAL(triggered()), this, SLOT(addOneRow()));
    connect(fDelAction, SIGNAL(triggered()), this, SLOT(delOneRow()));
}

void P2Test::showContextMenu(const QPoint& pos)
{
    QList<QAction*> actions = fContextMenu->actions();
    foreach(QAction* action, actions)
        action->setEnabled(true);

    fContextMenu->exec(QCursor::pos());
}

void P2Test::addOneRow()
{
    int totalRow = ui.attrsTable->rowCount();
    ui.attrsTable->insertRow(totalRow);

//     QComboBox *itmAttrCode = new QComboBox;
//     for (size_t i = 0; i < ATTR_CODE_NUM; i++)
//         itmAttrCode->addItem(sAttrCodeParam[i].strVal, sAttrCodeParam[i].enumVal);
// 
//     QComboBox *itmAttrFlags = new QComboBox;
//     for (size_t i = 0; i < ATTR_FLAGS_NUM; i++)
//         itmAttrFlags->addItem(sAttrFlagsParam[i].strVal, sAttrFlagsParam[i].enumVal);
// 
//     QComboBox *itmAttrDataType = new QComboBox;
//     for (size_t i = 0; i < ATTR_DATA_TYPE_NUM; i++)
//         itmAttrDataType->addItem(sAttrDataTypeParam[i].strVal, sAttrDataTypeParam[i].enumVal);
// 
//     QTextEdit *itmAttrData = new QTextEdit;
//     
// 
//     ui.attrsTable->setCellWidget(totalRow, 0, itmAttrCode);
//     ui.attrsTable->setCellWidget(totalRow, 1, itmAttrFlags);
//     ui.attrsTable->setCellWidget(totalRow, 2, itmAttrDataType);
//     ui.attrsTable->setCellWidget(totalRow, 3, itmAttrData);
// 
//     ui.attrsTable->resizeColumnsToContents();
    
}

void P2Test::delOneRow()
{
    int curRow = ui.attrsTable->currentRow();
    ui.attrsTable->removeRow(curRow);
}

