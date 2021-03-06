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

#include <QMenu>
#include <QSqlRecord>
#include <QSqlTableModel>

MembersListDialog::MembersListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MembersListDialog)
{
    ui->setupUi(this);

    QMenu *pop_up = new QMenu(this);
    pop_up->addAction(tr("Edit"), this, &MembersListDialog::edit_current);
    //pop_up->addAction(tr("Copy"), this, &MembersListDialog::copy_current);
    pop_up->addAction(tr("Mark as inactive"), this, &MembersListDialog::mark_as_inactive);
    ui->m_other_button->setMenu(pop_up);

    m_model = new QSqlTableModel(this);
    m_model->setEditStrategy(QSqlTableModel::OnRowChange);
    connect(m_model, &QSqlTableModel::primeInsert,
            [](int /*row*/, QSqlRecord &record)
    {
        record.setGenerated("id", false);
        record.setValue("membernumber", -1);
        record.setGenerated("membernumber", true);
        record.setValue("status", QVariant::fromValue<int>(0)); //active
        record.setGenerated("status", true);
    });
    m_model->setTable("members");
    m_model->select();

    m_model->setHeaderData(1, Qt::Horizontal, tr("Member number"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("First name"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Last name"));

    ui->m_view_members->setModel(m_model);
    //ui->m_view_members->hideColumn(0);
    ui->m_view_members->resizeColumnsToContents();
}

MembersListDialog::~MembersListDialog()
{
    delete ui;
    delete m_model;
}

void MembersListDialog::mark_as_inactive()
{
    const int row = ui->m_view_members->currentIndex().row();
    if(row >= 0)
    {
        QModelIndex index = m_model->index(row, 6); // status column
        m_model->setData(index, -1);
        m_model->submit();
        m_model->select();
    }
}

void MembersListDialog::edit_current()
{
    QModelIndex index = ui->m_view_members->currentIndex();
    ui->m_view_members->edit(index);
}

void MembersListDialog::on_m_add_button_clicked()
{
    const int row = m_model->rowCount();
    m_model->insertRow(row);
    QModelIndex index = m_model->index(row, 1);
    ui->m_view_members->setCurrentIndex(index);
    edit_current();
    //
}
