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

#include "pricelistdialog.h"
#include "ui_pricelistdialog.h"

#include "settings.h"

#include <QMenu>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

PriceListDialog::PriceListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PriceListDialog)
{
    ui->setupUi(this);

    QMenu *pop_up = new QMenu(this);
    pop_up->addAction(tr("Edit"), this, &PriceListDialog::edit_current);
    pop_up->addAction(tr("Copy"), this, &PriceListDialog::copy_current);
    pop_up->addAction(tr("Delete"), this, &PriceListDialog::delete_current);
    ui->m_other_button->setMenu(pop_up);

    m_model = new QSqlRelationalTableModel(this);
    m_model->setTable("prices");

    m_model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    int revenueIdx = m_model->fieldIndex("revenue");

    //m_revenue_relation = QSqlRelation("revenues", "id", "type, account");
    m_model->setRelation(revenueIdx, QSqlRelation("revenues", "id", "representation"));

    //m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Member"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Season"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Abo"));
    m_model->setHeaderData(5, Qt::Horizontal, tr("Guest"));
    m_model->setHeaderData(6, Qt::Horizontal, tr("Days"));
    m_model->setHeaderData(7, Qt::Horizontal, tr("From"));
    m_model->setHeaderData(8, Qt::Horizontal, tr("Till"));
    m_model->setHeaderData(9, Qt::Horizontal, tr("Summe"));
    m_model->setHeaderData(10, Qt::Horizontal, tr("Revenue type"));
    m_model->setHeaderData(11, Qt::Horizontal, tr("Account"));

    m_model->setEditStrategy(QSqlTableModel::OnRowChange);
    connect(m_model, &QSqlTableModel::primeInsert,
            [](int /*row*/, QSqlRecord &record)
    {
        record.setGenerated("id", false);
        record.setGenerated("price_name", false);
        record.setGenerated("member", false);
        record.setValue("winter", QVariant::fromValue<QString>(Settings::winterSeason() ? "true" : "false"));
        record.setGenerated("winter", true);
        record.setValue("abo", QVariant::fromValue<QString>("false"));
        record.setGenerated("abo", true);
        record.setValue("guest", QVariant::fromValue<QString>("false"));
        record.setGenerated("guest", true);
        record.setValue("days", QVariant::fromValue<int>(127));
        record.setGenerated("days", true);
        record.setValue("start_time_slot", QVariant::fromValue<int>(9));
        record.setGenerated("start_time_slot", true);
        record.setValue("end_time_slot", QVariant::fromValue<int>(21));
        record.setGenerated("end_time_slot", true);
        record.setGenerated("sum", false);
        record.setGenerated("revenue", false);
    });

    // Populate the model:
    if (!m_model->select())
    {
        QMessageBox::critical(this, "Unable to initialize Pricedialog",
                    "Error initializing database: " + m_model->lastError().text());
        return;
    }

    ui->m_view_prices->setModel(m_model);
    ui->m_view_prices->setItemDelegate(new QSqlRelationalDelegate(ui->m_view_prices));
    ui->m_view_prices->hideColumn(0);
    //ui->m_view_prices->setItemDelegate(new PricesDelegate(ui->m_view_prices));
    ui->m_view_prices->resizeColumnsToContents();
    ui->m_view_prices->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->m_view_prices->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &PriceListDialog::handleCurrentChanged);

}

PriceListDialog::~PriceListDialog()
{
    delete m_model;
    delete ui;
}

void PriceListDialog::edit_current()
{
    QModelIndex index = ui->m_view_prices->currentIndex();
    if(index.isValid())
    {
        m_edited_index = index;
        //m_model->setRelation(m_model->fieldIndex("revenue"), QSqlRelation("revenues", "id", "type"));
        ui->m_view_prices->edit(index);
        ui->m_view_prices->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
}

void PriceListDialog::delete_current()
{
    const int row = ui->m_view_prices->currentIndex().row();
    m_model->removeRows(row, 1);
    m_model->select();
}

void PriceListDialog::copy_current()
{

}

void PriceListDialog::on_m_add_button_clicked()
{
    const int row = m_model->rowCount();
    m_model->insertRows(row, 1);
    QModelIndex index = m_model->index(row, 0);
    ui->m_view_prices->setCurrentIndex(index);
    edit_current();
}

void PriceListDialog::handleCurrentChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    if(current.row() != m_edited_index.row())
    {
        ui->m_view_prices->closePersistentEditor(m_edited_index);
        ui->m_view_prices->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_edited_index = QModelIndex();

    }
}
