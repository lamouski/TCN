#include "servicedialog.h"
#include "ui_servicedialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSqlTableModel>

#include "dbmanager.h"

ServiceDialog::ServiceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServiceDialog)
{
    ui->setupUi(this);

    m_model = new QSqlTableModel(this);
    m_model->setTable("settings");
    m_model->select();

    m_model->setHeaderData(1, Qt::Horizontal, tr("Parameter"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Value"));

    ui->m_view_settings->setModel(m_model);
    ui->m_view_settings->hideColumn(0);
    ui->m_view_settings->hideColumn(3);
    ui->m_view_settings->resizeColumnsToContents();
}

ServiceDialog::~ServiceDialog()
{
    delete ui;
}

void ServiceDialog::on_pushButton_clicked()
{

    QString filename = QFileDialog::getOpenFileName(this, tr("Open member list file"), QString(), tr("Table CSV (*.csv)"));

    if(filename.isEmpty())
        return;

    if(!QFileInfo(filename).exists()){
        QMessageBox::warning(this, tr("File open error"), tr("The file is not exist."));
    }

    if(!DbManager::instance()->importMembersFile(filename))
    {
        QMessageBox::warning(this, tr("File import error"), tr("The member list can't be imported."));

    }

}
