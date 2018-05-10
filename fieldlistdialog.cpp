#include "fieldlistdialog.h"
#include "ui_fieldlistdialog.h"

#include <QSqlTableModel>

FieldListDialog::FieldListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FieldListDialog)
{
    ui->setupUi(this);

    m_model = new QSqlTableModel(this);
    m_model->setTable("fields");
    m_model->select();

    m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Days"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Season"));

    ui->m_view_fields->setModel(m_model);
    ui->m_view_fields->resizeColumnsToContents();
}

FieldListDialog::~FieldListDialog()
{
    delete ui;
}
