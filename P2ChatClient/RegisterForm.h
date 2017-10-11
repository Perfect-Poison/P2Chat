#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>
#include "ui_RegisterForm.h"
#include "p2client_common.h"

class RegisterForm : public QWidget
{
    Q_OBJECT

public:
    RegisterForm(QWidget *parent = 0);
    ~RegisterForm();

private:
    Ui::RegisterForm ui;
};

#endif // REGISTERFORM_H
