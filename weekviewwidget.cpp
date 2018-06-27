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

#include "weekviewwidget.h"
#include "ui_weekviewwidget.h"
#include "dbmanager.h"

#include "bookingdialog.h"
#include "daybookingtablemodel.h"

#include <QTableWidget>
#include <QDebug>

WeekViewWidget::WeekViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeekViewWidget),
    m_booking_dialog(nullptr)
{
    ui->setupUi(this);
    m_day_labels = QVector<QLabel*>({ui->m_label_day1, ui->m_label_day2, ui->m_label_day3, ui->m_label_day4, ui->m_label_day5, ui->m_label_day6, ui->m_label_day7});
    m_booking_tables = QVector<QTableView*>({ui->m_table_booking_1,ui->m_table_booking_2,ui->m_table_booking_3,ui->m_table_booking_4,ui->m_table_booking_5,ui->m_table_booking_6,ui->m_table_booking_7});

    m_day_booking_models.resize(7);
    for(qint64 i = 0; i < 7; i++)
        m_day_booking_models[i] = nullptr;

    set_signal_slots_connections();
}


WeekViewWidget::~WeekViewWidget()
{
    delete ui;
}


void WeekViewWidget::set_signal_slots_connections()
{
    for(int day = 0; day < 7; day++)
    {
        connect(m_booking_tables[day], &QTableView::doubleClicked,
                [this, day](const QModelIndex &index){ processBooking(day, index);});
    }
    connect(ui->m_date_edit_currient_day, &QDateEdit::dateChanged,
            [this](const QDate &date) { if(m_date != date) setCurrientDate(date); });
}


QPushButton *WeekViewWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}


void WeekViewWidget::processBooking(int day, const QModelIndex &index)
{
    if(!m_day_booking_models[day])
        return;

    if(!m_booking_dialog)
        m_booking_dialog = new BookingDialog(this);
    
    m_booking_dialog->setField(m_day_booking_models[day]->fieldName(index.row()));
    m_booking_dialog->setTimeslot(m_day_booking_models[day]->timeSlot(index.column()));
    QVariant curr_data = m_day_booking_models[day]->data(m_day_booking_models[day]->index(index.row(), index.column()), Qt::UserRole);
    if(!curr_data.isNull())
    {
        QPair<int, int> member_price_pair = curr_data.value<QPair<int, int> >();
        m_booking_dialog->setMemberId(member_price_pair.first);
        m_booking_dialog->setPriceId(member_price_pair.second);
    }
    else
        m_booking_dialog->reset();
    if(m_booking_dialog->exec() == QDialog::Accepted)
    {
        int selected_member_id = m_booking_dialog->selectedId();
        int selected_price_id = m_booking_dialog->selectedPrice();
        qDebug() << "Selected member id " << selected_member_id;

        m_day_booking_models[day]->setData(index, QVariant::fromValue(QPair<int, int>(selected_member_id, selected_price_id)), Qt::UserRole);
    }
}


/*
 * Sets currient day
 */
void WeekViewWidget::setCurrientDate(QDate date) {
    if(m_date.weekNumber() != date.weekNumber())
    {
        m_date = date;
        fillCurrientWeek();
        updateGUI();
    }
    else
        m_date = date;
    ui->m_date_edit_currient_day->setDate(m_date);
}


void WeekViewWidget::updateGUI()
{
    ui->m_week_label->setText(
                QString("%1 - %2 %3")
                    .arg(firstDayOfCurrientWeek().day())
                    .arg(lastDayOfCurrientWeek().day())
                    .arg(m_date.toString("MMMM yyyy")));
}


//current week handling functions
inline QDate WeekViewWidget::firstDayOfCurrientWeek() const
{
    return m_date.addDays(Qt::Monday - m_date.dayOfWeek());
}


inline QDate WeekViewWidget::lastDayOfCurrientWeek() const
{
    return m_date.addDays(m_date.dayOfWeek() - Qt::Sunday);
}


void WeekViewWidget::fillCurrientWeek() { //on show
    DbManager* db = DbManager::instance();
    if(!db)
        return;
    QDate day = firstDayOfCurrientWeek();
    for(qint64 i = 0; i < 7; i ++) {
        DayBookingTableModel* model = m_day_booking_models[i];
        if(!model) {
            model = new DayBookingTableModel(this);
            m_day_booking_models[i] = model;
        }
        model->setDay(day);
        m_booking_tables[i]->setModel(model);
        m_booking_tables[i]->resizeRowsToContents();
        m_booking_tables[i]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        if(i)
            m_booking_tables[i]->horizontalHeader()->hide();
        m_day_labels[i]->setText(day.toString("dddd d.M.yyyy"));
        day = day.addDays(1);
    }
}

void WeekViewWidget::on_m_button_previous_week_clicked()
{
    setCurrientDate(m_date.addDays(-7));
}

void WeekViewWidget::on_m_button_next_week_clicked()
{
    setCurrientDate(m_date.addDays(7));
}
