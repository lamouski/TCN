#include "fieldselectiondialog.h"
#include "ui_fieldselectiondialog.h"

FieldSelectionDialog::FieldSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FieldSelectionDialog)
{
    ui->setupUi(this);
    connect(ui->m_button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->m_button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

FieldSelectionDialog::~FieldSelectionDialog()
{
    delete ui;
}

void FieldSelectionDialog::setFields(const QStringList& texts)
{
    ui->m_field_combo_box->addItems(texts);
}

int FieldSelectionDialog::selectedFieldRow() const
{
    return ui->m_field_combo_box->currentIndex();
}
