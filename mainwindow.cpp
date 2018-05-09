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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dbmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_members_list_dialog(nullptr)
{
    ui->setupUi(this);

    DbManager* db = new DbManager("../test_db/tcn2020.db");

    db->checkDB();

    ui->m_week_view->setCurrientDate(QDate::currentDate());

    connect(ui->m_button_bookings, &QPushButton::clicked,
            [this](){ui->m_stacked_widget->setCurrentWidget(ui->m_week_view);});
    connect(ui->m_week_view->getReturnButton(), &QPushButton::clicked,
            [this](){ui->m_stacked_widget->setCurrentWidget(ui->m_main_view);});

}

MainWindow::~MainWindow()
{
    delete ui;
    if(m_members_list_dialog)
        delete m_members_list_dialog;
}

void MainWindow::on_m_button_members_clicked()
{
    if(!m_members_list_dialog)
        m_members_list_dialog = new MembersListDialog();
    m_members_list_dialog->show();
}



void MainWindow::on_delete_pushButton_clicked()
{
    DbManager* db = DbManager::instance();

    db->addMember("Denis", "Lamouski");
    db->addMember("Natasha", "Lind");
    db->addMember("Peter", "Mayer");
    db->addMember("Viktor", "Stadtler");
    db->addMember("Hans", "Koch");



}
