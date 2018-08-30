#include "pricelistdialog.h"
#include "ui_pricelistdialog.h"

#include <QSqlTableModel>

PriceListDialog::PriceListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PriceListDialog)
{
    ui->setupUi(this);

    m_model = new QSqlTableModel(this);
    m_model->setTable("prices");
    m_model->select();

    //m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    //m_model->setHeaderData(1, Qt::Horizontal, tr("First name"));
    //m_model->setHeaderData(2, Qt::Horizontal, tr("Last name"));

    ui->m_view_prices->setModel(m_model);
    ui->m_view_prices->hideColumn(0);
    ui->m_view_prices->resizeColumnsToContents();
}

PriceListDialog::~PriceListDialog()
{
    delete m_model;
    delete ui;
}
