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

#ifndef PRICELISTDIALOG_H
#define PRICELISTDIALOG_H

#include <QDialog>
#include <QModelIndex>

class QSqlTableModel;

namespace Ui {
class PriceListDialog;
}

class PriceListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PriceListDialog(QWidget *parent = nullptr);
    ~PriceListDialog();

protected slots:
    void delete_currient();
    void edit_currient();

private slots:
    void on_m_add_button_clicked();

    void handleCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Ui::PriceListDialog *ui;
    QSqlTableModel* m_model;

    QModelIndex m_edited_index;
};

#endif // PRICELISTDIALOG_H
