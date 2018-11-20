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
