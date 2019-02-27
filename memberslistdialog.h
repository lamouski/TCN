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

#ifndef MEMBERSLISTDIALOG_H
#define MEMBERSLISTDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class MembersListDialog;
}

class MembersListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MembersListDialog(QWidget *parent = 0);
    ~MembersListDialog();



private slots:
    //void copy_current();
    void edit_current();
    void mark_as_inactive();

    void on_m_add_button_clicked();

private:
    Ui::MembersListDialog *ui;

    QSqlTableModel* m_model;
};

#endif // MEMBERSLISTDIALOG_H
