#include "logindatadialog.h"
#include "ui_logindatadialog.h"

LoginDataDialog::LoginDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDataDialog)
{
    ui->setupUi(this);
}

LoginDataDialog::~LoginDataDialog()
{
    delete ui;
}

QString LoginDataDialog::getLogin() const
{
    return ui->m_username_line->text();
}

QString LoginDataDialog::getPassword() const
{
     return ui->m_pass_line->text();
}
