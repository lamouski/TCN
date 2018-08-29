#include "fielddelegate.h"
#include "fieldlistdialog.h"
#include "ui_fieldlistdialog.h"

#include "fieldstablemodel.h"

FieldListDialog::FieldListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FieldListDialog)
{
    ui->setupUi(this);

    m_model = new FieldsTableModel(this);

    ui->m_view_fields->setModel(m_model);
    //ui->m_view_fields->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->m_view_fields->setItemDelegate(new FieldDelegate(ui->m_view_fields));
    //ui->m_view_fields->setColumnHidden(m_model->fieldIndex("id"), true);
    ui->m_view_fields->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->m_view_fields->resizeColumnsToContents();
}

FieldListDialog::~FieldListDialog()
{
    delete ui;
}
