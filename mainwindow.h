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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>

class MembersListDialog;
class FieldListDialog;
class PriceListDialog;
class ServiceDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_m_button_members_clicked();
    void on_m_button_fields_clicked();
    void on_m_button_prices_clicked();

    void on_m_button_service_clicked();

private:
    Ui::MainWindow *ui;

    MembersListDialog* m_members_list_dialog = nullptr;
    FieldListDialog* m_fields_list_dialog = nullptr;
    PriceListDialog* m_prices_list_dialog = nullptr;
    ServiceDialog* m_service_dialog = nullptr;
};

#endif // MAINWINDOW_H
