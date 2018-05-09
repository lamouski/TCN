#include "memberslistdialog.h"
#include "ui_memberslistdialog.h"

#include <QSqlTableModel>

MembersListDialog::MembersListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MembersListDialog)
{
    ui->setupUi(this);

    m_model = new QSqlTableModel(this);
    m_model->setTable("members");
    m_model->select();

    m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("First name"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Last name"));

    ui->m_view_members->setModel(m_model);
    ui->m_view_members->resizeColumnsToContents();
}

MembersListDialog::~MembersListDialog()
{
    delete ui;
    delete m_model;
}
