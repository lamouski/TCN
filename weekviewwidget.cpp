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
#include "settings.h"

#include <QTableWidget>
#include <QDebug>
#include <QMessageBox>

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
    connect(ui->m_date_edit_current_day, &QDateEdit::dateChanged,
            [this](const QDate &date) { if(Settings::currentDate() != date) setCurrentDate(date); });
}


QPushButton *WeekViewWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}


void WeekViewWidget::processBooking(int day, const QModelIndex &index)
{
    if(!m_day_booking_models[day])
        return;

    QVariant curr_data = m_day_booking_models[day]->data(m_day_booking_models[day]->index(index.row(), index.column()), Qt::UserRole);

    int rest_minutes = (QDateTime(m_day_booking_models[day]->day(), QTime(m_day_booking_models[day]->timeSlot(index.column()), 0)).toSecsSinceEpoch() -
                  QDateTime::currentDateTime().toSecsSinceEpoch()) / 60;

    if(rest_minutes < 0)
    {
        //if(!curr_data.isNull())
        {
            QMessageBox::information(this, QString(), QString("This time interval is in the past. It can't be booked."));
            return;
        }
    }

    if(!m_booking_dialog)
        m_booking_dialog = new BookingDialog(this);

    m_booking_dialog->setDay(firstDayOfCurrentWeek().addDays(day));
    m_booking_dialog->setField(m_day_booking_models[day]->fieldName(index.row()));
    m_booking_dialog->setTimeslot(m_day_booking_models[day]->timeSlot(index.column()));

    if(!curr_data.isNull())
    {
        QPair<int, int> member_price_pair = curr_data.value<QPair<int, int> >();
        m_booking_dialog->setMemberId(member_price_pair.first);
        m_booking_dialog->setPriceId(member_price_pair.second);
    }
    else
    {
        m_booking_dialog->reset();
    }

    if(m_booking_dialog->exec() == QDialog::Accepted)
    {
        int selected_member_id = m_booking_dialog->selectedId();

        int selected_price_id = m_booking_dialog->selectedPrice();
        qDebug() << "Selected member id " << selected_member_id;
        if(selected_member_id > 0)
            m_day_booking_models[day]->setData(index,
                                               QVariant::fromValue(QPair<int, int>(selected_member_id, selected_price_id)),
                                               Qt::UserRole);
        else
            m_day_booking_models[day]->setData(index,
                                               QVariant::fromValue(QPair<QString, int>(m_booking_dialog->info(), selected_price_id)),
                                               Qt::UserRole);
    }
}


/*
 * Sets current day
 */
void WeekViewWidget::setCurrentDate(QDate date) {

    if(Settings::currentDate().weekNumber() != date.weekNumber())
    {
        Settings::instance()->setCurrentDate(date);
        fillCurrentWeek();
    }
    else
        Settings::instance()->setCurrentDate(date);

    ui->m_date_edit_current_day->setDate(Settings::currentDate());
    updateGUI();
}


void WeekViewWidget::updateGUI()
{
    QDate first_day = firstDayOfCurrentWeek();
    QDate last_day = lastDayOfCurrentWeek();
    if(first_day.month() == last_day.month())
        ui->m_week_label->setText( QString("%1 - %2 %3")
                    .arg(first_day.day())
                    .arg(last_day.day())
                    .arg(Settings::currentDate().toString("MMMM yyyy")));
    else
        ui->m_week_label->setText( QString("%1 - %2")
                    .arg(first_day.toString("dd MMMM"))
                    .arg(last_day.toString("dd MMMM yyyy")));
}


//current week handling functions
inline QDate WeekViewWidget::firstDayOfCurrentWeek() const
{
    const QDate& date = Settings::currentDate();
    return date.addDays(Qt::Monday - date.dayOfWeek());
}


inline QDate WeekViewWidget::lastDayOfCurrentWeek() const
{
    const QDate& date = Settings::currentDate();
    return date.addDays(Qt::Sunday - date.dayOfWeek());
}

void WeekViewWidget::showEvent(QShowEvent */*e*/)
{
    fillCurrentWeek();
}

void WeekViewWidget::fillCurrentWeek() {
    DbManager* db = DbManager::instance();
    if(!db)
        return;
    QDate day = firstDayOfCurrentWeek();
    for(qint64 i = 0; i < 7; i ++) {
        DayBookingTableModel* model = m_day_booking_models[i];
        if(!model) {
            model = new DayBookingTableModel(this);
            m_day_booking_models[i] = model;
        }
        m_booking_tables[i]->setModel(nullptr);
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
    setCurrentDate(Settings::currentDate().addDays(-7));
}

void WeekViewWidget::on_m_button_next_week_clicked()
{
    setCurrentDate(Settings::currentDate().addDays(7));
}
