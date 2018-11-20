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

#ifndef SERVICEDIALOG_H
#define SERVICEDIALOG_H

#include <QDialog>

class QSqlTableModel;

namespace Ui {
class ServiceDialog;
}

class ServiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServiceDialog(QWidget *parent = 0);
    ~ServiceDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ServiceDialog *ui;

    QSqlTableModel *m_model;
};

#endif // SERVICEDIALOG_H
