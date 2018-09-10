#include "servicedialog.h"
#include "ui_servicedialog.h"

#include <QFileDialog>
#include <QMessageBox>

#include "dbmanager.h"

ServiceDialog::ServiceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServiceDialog)
{
    ui->setupUi(this);
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
