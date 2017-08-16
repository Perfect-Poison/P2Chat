#pragma execution_character_set("utf-8")
#ifndef LOGINDIAGLOG_H
#define LOGINDIAGLOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>

class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;
class LoginDiaglog : public QDialog
{
    Q_OBJECT

public:
    LoginDiaglog(QWidget *parent = nullptr);
    ~LoginDiaglog();
    void Init();
private:
    QComboBox   *fUserNameLCombo;
    QLineEdit   *fPassWordLEdit;
    QPushButton *fLoginBt;
    QPushButton *fRegisterBt;
    QPushButton *fFindPasswordBt;
    QCheckBox   *fRemPassword;
    QCheckBox   *fAutoLogin;
};

#endif // LOGINDIAGLOG_H
