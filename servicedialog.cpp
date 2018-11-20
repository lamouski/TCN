/*
 * TCN Field Booking Application
 * Copyright (C) 2018 Dzianis Lamouski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
