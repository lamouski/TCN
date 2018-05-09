#include "memberslistdialog.h"
#include "ui_memberslistdialog.h"

MembersListDialog::MembersListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MembersListDialog)
{
    ui->setupUi(this);
}

MembersListDialog::~MembersListDialog()
{
    delete ui;
}
