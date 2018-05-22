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

    //m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("First name"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Last name"));

    ui->m_view_members->setModel(m_model);
    ui->m_view_members->hideColumn(0);
    ui->m_view_members->resizeColumnsToContents();
}

MembersListDialog::~MembersListDialog()
{
    delete ui;
    delete m_model;
}
