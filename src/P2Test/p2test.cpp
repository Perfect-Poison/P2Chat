#include "p2test.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QTableWidget>
#include <QtCore/QByteArray>
#include <QtGui/QTextBlock>
#include "../P2ChatServer/p2server_common.h"
#include "uthash.h"

void AttrsTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0 || index.column() == 1 || index.column() == 2)
    {
        int enumVal = index.model()->data(index, Qt::DisplayRole).toInt();
        QString text = "";
        if (index.column() == 0)
            text = QString::fromWCharArray(find_attr_code_param_by_enum(enumVal).strVal);
        else if (index.column() == 1)
            text = QString::fromWCharArray(find_attr_flags_param_by_enum(enumVal).strVal);
        else if (index.column() == 2)
            text = QString::fromWCharArray(find_attr_datatype_param_by_enum(enumVal).strVal);
        text += "(" + QString::number(enumVal) + ")";
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
        {
            QString text = QString::fromWCharArray(sAttrCodeParam[i].strVal);
            text += "(" + QString::number(sAttrCodeParam[i].enumVal) + ")";
            editor->addItem(text, sAttrCodeParam[i].enumVal);
        }
    }
    else if (index.column() == 1)
    {
        for (size_t i = 0; i < ATTR_FLAGS_NUM; i++)
        {
            QString text = QString::fromWCharArray(sAttrFlagsParam[i].strVal);
            text += "(" + QString::number(sAttrFlagsParam[i].enumVal) + ")";
            editor->addItem(text, sAttrFlagsParam[i].enumVal);
        }
    }
    else if (index.column() == 2)
    {
        for (size_t i = 0; i < ATTR_DATA_TYPE_NUM; i++)
        {
            QString text = QString::fromWCharArray(sAttrDataTypeParam[i].strVal);
            text += "(" + QString::number(sAttrDataTypeParam[i].enumVal) + ")";
            editor->addItem(text, sAttrDataTypeParam[i].enumVal);
        }
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
            text = QString::fromWCharArray(find_attr_code_param_by_enum(enumVal).strVal);
        else if (index.column() == 1)
            text = QString::fromWCharArray(find_attr_flags_param_by_enum(enumVal).strVal);
        else if (index.column() == 2)
            text = QString::fromWCharArray(find_attr_datatype_param_by_enum(enumVal).strVal);
        text += "(" + QString::number(enumVal) + ")";
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
        text = text.left(text.lastIndexOf(QChar('(')));
        int enumVal = 0;
        if (index.column() == 0)
            enumVal = find_attr_code_param_by_str(text.toStdWString().c_str()).enumVal;
        else if (index.column() == 1)
            enumVal = find_attr_flags_param_by_str(text.toStdWString().c_str()).enumVal;
        else if (index.column() == 2)
            enumVal = find_attr_datatype_param_by_str(text.toStdWString().c_str()).enumVal;
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
    {
        QString text = QString::fromWCharArray(sMsgCodeParam[i].strVal);
        text += "(" + QString::number(sMsgCodeParam[i].enumVal) + ")";
        ui.msgCodeCombo->addItem(text, sMsgCodeParam[i].enumVal);
    }

    for (size_t i = 0; i < MSG_FLAGS_NUM; i++)
    {
        QString text = QString::fromWCharArray(sMsgFlagsParam[i].strVal);
        text += "(" + QString::number(sMsgFlagsParam[i].enumVal) + ")";
        ui.msgFlagsCombo->addItem(text, sMsgFlagsParam[i].enumVal);
    }

    ui.msgSizeLabel->setText("16");
    ui.msgIDLineEdit->setText("0");
    ui.attrNumLabel->setText("0");

    /**
     *	消息载荷
     */
    fUdpSocket = new QUdpSocket(this);
    fContextMenu = new QMenu(this);
    fAddAction = new QAction(tr("增加"), this);
    fDelAction = new QAction(tr("删除"), this);
    fMessage = nullptr;
    fContextMenu->addAction(fAddAction);
    fContextMenu->addAction(fDelAction);
    ui.attrsTable->setItemDelegate(new AttrsTableDelegate(this));
    ui.tickLabel->setText("\t      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");

    /**
     *	Server
     */
    ui.ipLineEdit->setText("127.0.0.1");
    ui.portLineEdit->setText(QString::number(SERVER_PORT_FOR_UDP));
    fUdpSocket->bind(QHostAddress::LocalHost, BIND_PORT_FOR_UDP);

    /**
     *	信号槽
     */
    connect(ui.attrsTable, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    connect(fAddAction, SIGNAL(triggered()), this, SLOT(addOneRow()));
    connect(fDelAction, SIGNAL(triggered()), this, SLOT(delOneRow()));
    connect(ui.refreshButton, SIGNAL(clicked()), this, SLOT(msgDataUpdate()));
    connect(fUdpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    connect(ui.sendMsgButton, SIGNAL(clicked()), this, SLOT(sendDatagrams()));
    connect(ui.logTextEdit, SIGNAL(msgUpdate(const QByteArray&)), this, SLOT(msgUpdate(const QByteArray&)));
}

void P2Test::msgUpdate(const QByteArray& rawMsg)
{
    if (fMessage)
        safe_delete(fMessage);
    fMessage = new Message((MESSAGE *)rawMsg.data());

    ui.msgCodeCombo->setCurrentIndex(fMessage->GetCode());
    ui.msgFlagsCombo->setCurrentIndex(fMessage->GetFlags());
    ui.msgSizeLabel->setText(QString::number(fMessage->GetSize()));
    ui.msgIDLineEdit->setText(QString::number(fMessage->GetID()));
    ui.attrNumLabel->setText(QString::number(fMessage->GetAttrNum()));
    clearRows();
    int rowCount = fMessage->GetAttrNum();
    ui.attrsTable->setRowCount(rowCount);
    QAbstractItemModel* model = ui.attrsTable->model();
    MessageAttr *entry, *tmp;
    uint32 totalAttrSize = 0;
    int rowNumber = 0;
    HASH_ITER(hh, fMessage->GetAttrs(), entry, tmp)
    {
        MESSAGE_ATTR *rawMsgAttr = &entry->data;
        model->setData(model->index(rowNumber, 0), (int)rawMsgAttr->code, Qt::DisplayRole);
        model->setData(model->index(rowNumber, 1), (int)rawMsgAttr->flags, Qt::DisplayRole);
        model->setData(model->index(rowNumber, 2), (int)rawMsgAttr->dataType, Qt::DisplayRole);
        switch (rawMsgAttr->dataType) 
        {
        case dt_int16:       // 16位(signed/unsigned)整型
            if (rawMsgAttr->flags & af_unsigned)
                model->setData(model->index(rowNumber, 3), (uint16)rawMsgAttr->i16, Qt::DisplayRole);
            else
                model->setData(model->index(rowNumber, 3), rawMsgAttr->i16, Qt::DisplayRole);
            break;
        case dt_int32:       // 32位(signed/unsigned)整型
            if (rawMsgAttr->flags & af_unsigned)
                model->setData(model->index(rowNumber, 3), (uint)rawMsgAttr->i32, Qt::DisplayRole);
            else
                model->setData(model->index(rowNumber, 3), rawMsgAttr->i32, Qt::DisplayRole);
            break;
        case dt_int64:       // 64位(signed/unsigned)整型
            if (rawMsgAttr->flags & af_unsigned)
                model->setData(model->index(rowNumber, 3), (uint64)rawMsgAttr->i64, Qt::DisplayRole);
            else
                model->setData(model->index(rowNumber, 3), rawMsgAttr->i64, Qt::DisplayRole);
            break;
        case dt_float32:     // 32位浮点型
            model->setData(model->index(rowNumber, 3), rawMsgAttr->f32, Qt::DisplayRole);
            break;
        case dt_float64:     // 64位浮点型
            model->setData(model->index(rowNumber, 3), rawMsgAttr->f64, Qt::DisplayRole);
            break;
        case dt_binary:      // 二进制类型
        {
            TCHAR hexStr[1500];
            ::memset(hexStr, 0, sizeof(hexStr));
            bin_to_str(rawMsgAttr->bin.data, rawMsgAttr->bin.size, hexStr);
            model->setData(model->index(rowNumber, 3), QString::fromWCharArray(hexStr), Qt::DisplayRole);
            break;
        }
        case dt_string:       // 宽字符串类型
            model->setData(model->index(rowNumber, 3), QString::fromWCharArray((TCHAR*)&rawMsgAttr->str.data[0], rawMsgAttr->str.size / 2), Qt::DisplayRole);
            break;
        }
        ++rowNumber;
    }
    ui.attrsTable->reset();
    msgDataUpdate();
}

void P2Test::showContextMenu(const QPoint& pos)
{
    fAddAction->setEnabled(true);
    if (ui.attrsTable->rowCount() <= 0)
        fDelAction->setEnabled(false);
    else
        fDelAction->setEnabled(true);

    fContextMenu->exec(QCursor::pos());
}

void P2Test::addOneRow()
{
    int totalRow = ui.attrsTable->rowCount();
    ui.attrsTable->insertRow(totalRow);
    ui.attrNumLabel->setText(QString::number(totalRow + 1));
}

void P2Test::delOneRow()
{
    int totalRow = ui.attrsTable->rowCount();
    int curRow = ui.attrsTable->currentRow();
    ui.attrsTable->removeRow(curRow);
    ui.attrNumLabel->setText(QString::number(totalRow - 1));
}

void P2Test::clearRows()
{
    ui.attrsTable->clearContents();
}

void P2Test::msgDataUpdate()
{
    if (fMessage)
        safe_delete(fMessage);
    fMessage = new Message;
    fMessage->SetCode(ui.msgCodeCombo->currentData(Qt::UserRole).toUInt());
    fMessage->SetFlags(ui.msgFlagsCombo->currentData(Qt::UserRole).toUInt());
    fMessage->SetID(ui.msgIDLineEdit->text().toUInt());

    size_t rowNum = ui.attrsTable->rowCount();
    QAbstractItemModel *model = ui.attrsTable->model();
    for (size_t i = 0; i < rowNum; i++)
    {
        attr_code attrCode = model->index(i, 0).data(Qt::DisplayRole).toUInt();
        attr_flags attrFlags = model->index(i, 1).data(Qt::DisplayRole).toUInt();
        attr_datatype attrDataType = model->index(i, 2).data(Qt::DisplayRole).toUInt();
        QString attrDataStr;
        if (ui.attrsTable->item(i, 3) != nullptr)
            attrDataStr = ui.attrsTable->item(i, 3)->text();
        switch (attrDataType) 
        {
        case dt_int16:
            if (attrFlags & af_unsigned)
                fMessage->SetAttr(attrCode, (uint16)attrDataStr.toUShort());
            else
                fMessage->SetAttr(attrCode, (int16)attrDataStr.toShort());
            break;
        case dt_int32:
            if (attrFlags & af_unsigned)
                fMessage->SetAttr(attrCode, (uint32)attrDataStr.toUInt());
            else
                fMessage->SetAttr(attrCode, (int32)attrDataStr.toInt());
            break;
        case dt_int64:
            if (attrFlags & af_unsigned)
                fMessage->SetAttr(attrCode, (uint64)attrDataStr.toULongLong());
            else
                fMessage->SetAttr(attrCode, (int64)attrDataStr.toLongLong());
            break;
        case dt_float32:
            fMessage->SetAttr(attrCode, attrDataStr.toFloat());
            break;
        case dt_float64:
            fMessage->SetAttr(attrCode, attrDataStr.toDouble());
            break;
        case dt_binary:
            fMessage->SetAttr(attrCode, (BYTE*)attrDataStr.data(), attrDataStr.size());
            break;
        case dt_string:
            fMessage->SetAttr(attrCode, attrDataStr.toStdWString().c_str(), attrDataStr.toStdWString().length());
            break;
        default:
            _tprintf(_T("Message::Message no such data type!\n"));
            break;
        }
    }

    MESSAGE *msg = fMessage->CreateMessage();
    int msgSize = ntohl(msg->size);
    QByteArray byteData((char*)msg, msgSize);
    QString hexDataStr = byteData.toHex().data();
    hexDataStr = hexDataStr.toUpper();
    QString hexDataTrimed;
    rowNum = ceil(msgSize / 16.0);
    for (int i = 0, c = 0; i < rowNum; i++)
    {
        int t = ntohl(i);
        hexDataTrimed += tr("0x") + QByteArray((char*)&t, sizeof(i)).toHex().data();
        hexDataTrimed += QChar('\t');
        for (int j = 0; j < 32 && c < hexDataStr.length(); j += 2, c += 2)
        {
            hexDataTrimed += hexDataStr.mid(c, 2);
            hexDataTrimed += " ";
        }
        hexDataTrimed += QChar('\n');
    }
    ui.msgDataTextEdit->setText(hexDataTrimed);
    ui.msgLengthLabel->setText(QString::number(msgSize));
    safe_delete(msg);
}

void P2Test::readPendingDatagrams()
{
    char buff[96 * 1024];
    while (fUdpSocket->hasPendingDatagrams() && fUdpSocket->pendingDatagramSize())
    {
        memset(buff, 0, sizeof(buff));
        qint64 recvSize = fUdpSocket->pendingDatagramSize();
        QNetworkDatagram datagram = fUdpSocket->receiveDatagram();
        QString remoteHost = datagram.senderAddress().toString();
        quint16 remotePort = datagram.senderPort();
        
        TCHAR hexStr[1500 * 2 + 1];
        ::memset(hexStr, 0, sizeof(hexStr));
        bin_to_str((BYTE *)datagram.data().data(), datagram.data().size(), hexStr);
        QString hexData = QString::fromWCharArray(hexStr);
        ui.logTextEdit->append(QString("[RecvFr(%1:%2):%3B]S->C:%4").arg(remoteHost).arg(remotePort).arg(recvSize).arg(hexData));

    }

}

void P2Test::sendDatagrams()
{
    if (fMessage == nullptr)
        return;
    MESSAGE *msg = fMessage->CreateMessage();
    int msgSize = ntohl(msg->size);
    char *data = (char *)msg;
    QHostAddress remoteHost(ui.ipLineEdit->text());
    quint16 remotePort = ui.portLineEdit->text().toUShort();
    int ret = fUdpSocket->writeDatagram(data, msgSize, remoteHost, remotePort);
    assert(ret != -1);

    TCHAR hexStr[1500 * 2 + 1];
    ::memset(hexStr, 0, sizeof(hexStr));
    bin_to_str((BYTE *)msg, msgSize, hexStr);
    QString hexData = QString::fromWCharArray(hexStr);
    ui.logTextEdit->append(QString("[SendTo(%1:%2):%3B]C->S:%4").arg(remoteHost.toString()).arg(remotePort).arg(msgSize).arg(hexData));
    safe_delete(msg);
}

