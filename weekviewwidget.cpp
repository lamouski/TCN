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
#include <QMenu>

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

        m_booking_tables[day]->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_booking_tables[day] , &QWidget::customContextMenuRequested,
                [this, day](const QPoint& pos)
        { const QModelIndex &index = m_booking_tables[day]->indexAt(pos);
            processBookingContextMenu(day, index, pos);
        });
    }
    connect(ui->m_date_edit_current_day, &QDateEdit::dateChanged,
            [this](const QDate &date) { if(Settings::currentDate() != date) setCurrentDate(date); });
}


QPushButton *WeekViewWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}


void WeekViewWidget::processBooking(int day, const QModelIndex &index, ProcessingFlag flag)
{
    if(!m_day_booking_models[day])
        return;

    QVariant curr_data = m_day_booking_models[day]->data(index, Qt::UserRole);
    int booking_status = m_day_booking_models[day]->bookingStatus(index);

    if(!curr_data.isNull() && flag == NEW_BOOKING && booking_status >= 0)
        return;

    int rest_minutes = (QDateTime(m_day_booking_models[day]->day(),
       QTime(m_day_booking_models[day]->timeSlot(index.column()), 0)).toSecsSinceEpoch() -
       QDateTime::currentDateTime().toSecsSinceEpoch()) / 60;

    if(rest_minutes < 0)
    {
        if(booking_status > 0) //
        {
            QMessageBox::information(this, QString(), QString(tr("This booking is already entered in the cash register. It can't be changed.")));
            return;
        }
    }

    if(!m_booking_dialog)
        m_booking_dialog = new BookingDialog(this);

    const int time_slot = m_day_booking_models[day]->timeSlot(index.column());

    m_booking_dialog->setDay(firstDayOfCurrentWeek().addDays(day));
    m_booking_dialog->setField(m_day_booking_models[day]->fieldName(index.row()));
    m_booking_dialog->setTimeslot(time_slot);

    if(!curr_data.isNull())
    {
        if(curr_data.canConvert<QPair<int, int> >()) //the booking for member
        {
            QPair<int, int> member_price_pair = curr_data.value<QPair<int, int> >();
            m_booking_dialog->setMemberId(member_price_pair.first);
            m_booking_dialog->setPriceId(member_price_pair.second);
        }
        else if(curr_data.canConvert<QPair<QString, int> >()) //the booking for others
        {
            QPair<QString, int> info_price_pair = curr_data.value<QPair<QString, int> >();
            m_booking_dialog->setInfo(info_price_pair.first);
            m_booking_dialog->setPriceId(info_price_pair.second);
        }
        else
        {
             m_booking_dialog->reset();
        }
    }
    else
    {
        m_booking_dialog->reset();
    }

    if(m_booking_dialog->exec() == QDialog::Accepted)
    {
        BookingData data = m_booking_dialog->getSelectedData();
        qDebug() << "Selected member id " << data.memberID;

        BookingSlot slot = { m_day_booking_models[day]->day(),
                             m_day_booking_models[day]->timeSlot(index.column()),
                             m_day_booking_models[day]->fieldId(index.row())};

        int old_bookingId;
        BookingData old_data;
        bool slot_is_free = DbManager::instance()->bookingSlotIsFree(slot, old_bookingId, old_data);

        if(!slot_is_free)
        {
            if(old_data.aboID>0 && flag == NEW_BOOKING)
            {
                QMessageBox::information(this, QString(), QString(tr("The Abo Booking can't be direct rebooked. Please uses special funktions.")));
                return;
            }
        }

        if(data.memberID <= 0 && data.booking_info.isEmpty())
        {
            QMessageBox::information(this, QString(), QString(tr("The information about bookin is not filled. The booking can't be saved.")));
            return;
        }

        if(m_booking_dialog->isSingleBooking())
        {            
            singleBooking(slot, data);
        }
        else if(m_booking_dialog->isBlockBooking())
        {
            blockBooking(slot, data);
        }
        else if(m_booking_dialog->isMultyBooking())
        {
            multiBooking(slot, data);
        }
        m_day_booking_models[day]->select();
    }
}

void WeekViewWidget::cancleBooking(int day, const QModelIndex &index, WeekViewWidget::ProcessingFlag flag)
{
    if(!m_day_booking_models[day])
        return;

    QVariant curr_data = m_day_booking_models[day]->data(index, Qt::UserRole);

    int booking_status = m_day_booking_models[day]->bookingStatus(index);

    if(flag == NEW_BOOKING && (!curr_data.isNull() || booking_status > 0))
        return;

    int rest_minutes = (QDateTime(m_day_booking_models[day]->day(),
       QTime(m_day_booking_models[day]->timeSlot(index.column()), 0)).toSecsSinceEpoch() -
       QDateTime::currentDateTime().toSecsSinceEpoch()) / 60;

    if(rest_minutes < 0)
    {
        if(booking_status > 0) //
        {
            QMessageBox::information(this, QString(), QString(tr("This booking is already entered in the cash register. It can't be changed.")));
            return;
        }
    }

    int bookingID = m_day_booking_models[day]->bookingId(index);

    DbManager::instance()->cancleBooking(bookingID);
    m_day_booking_models[day]->select();
}

void WeekViewWidget::processBookingContextMenu(int day, const QModelIndex &index, const QPoint& pos)
{

    if(!m_day_booking_models[day])
        return;

    m_selected_day = day;
    m_selected_index = index;
    if(!m_contextMenu)
    {
        m_contextMenu = new QMenu(tr("Context menu"), this);

        m_action_change_abo_cur = new QAction(tr("Change currient Abo booking"), this);
        connect(m_action_change_abo_cur, &QAction::triggered,
                [this] () { processBooking(m_selected_day, m_selected_index, CURRENT_BOOKING ); });

        m_action_change_abo_all = new QAction(tr("Change complet Abo booking"), this);
        connect(m_action_change_abo_all, &QAction::triggered,
                [this] () { processBooking(m_selected_day, m_selected_index, ALL_BOOKINGS);});

        m_action_cancle_abo_cur = new QAction(tr("Cancel currient Abo booking"), this);
        connect(m_action_cancle_abo_cur, &QAction::triggered,
                [this] () { cancleBooking(m_selected_day, m_selected_index, CURRENT_BOOKING);});

        m_action_cancle_abo_all = new QAction(tr("Cancel complet Abo booking"), this);
        connect(m_action_cancle_abo_all, &QAction::triggered,
                [this] () { cancleBooking(m_selected_day, m_selected_index, ALL_BOOKINGS);});

        m_action_change_booking = new QAction(tr("Change booking"), this);
        connect(m_action_change_booking, &QAction::triggered,
                [this] () { processBooking(m_selected_day, m_selected_index, CURRENT_BOOKING); } );

        m_action_cancle_booking = new QAction(tr("Cancel booking"), this);
        connect(m_action_cancle_booking, &QAction::triggered,
                [this] () { cancleBooking(m_selected_day, m_selected_index, CURRENT_BOOKING);});
    }

    int abo_id = m_day_booking_models[day]->aboId(index);


    QVariant curr_data = m_day_booking_models[day]->data(index, Qt::UserRole);
    if(!curr_data.isNull())
    {
        m_contextMenu->clear();
        if(abo_id > 0)
        {
            m_contextMenu->addAction(m_action_change_abo_cur);
            m_contextMenu->addAction(m_action_change_abo_all);
            m_contextMenu->addSeparator();
            m_contextMenu->addAction(m_action_cancle_abo_cur);
            m_contextMenu->addAction(m_action_cancle_abo_all);
        }
        else
        {
            m_contextMenu->addAction(m_action_change_booking);
            m_contextMenu->addSeparator();
            m_contextMenu->addAction(m_action_cancle_booking);
        }

        m_contextMenu->exec(m_booking_tables[day]->mapToGlobal(pos));
    }
}

bool WeekViewWidget::singleBooking(const BookingSlot& slot, const BookingData& data)
{
    bool sucsess = false;
    int old_bookingId;
    BookingData old_data;
    if(DbManager::instance()->bookingSlotIsFree(slot, old_bookingId, old_data))
    {
         sucsess = DbManager::instance()->addBooking(slot, data);
    }
    else
    {
        sucsess = DbManager::instance()->updateBooking(old_bookingId, data);
        if(old_data.blockID > 0)
        {
            if(DbManager::instance()->numOfUsedBlocks(old_data.blockID) == 0)
                sucsess &= DbManager::instance()->deleteBlock(old_data.blockID);
        }
        //or
        //DbManager::instance()->cancleBooking(old_bookingId);
        //DbManager::instance()->addBooking(slot, data);
    }
    return sucsess;
}

bool  WeekViewWidget::blockBooking(const BookingSlot& slot, const BookingData& data)
{
    BookingData data_with_block_id = data;
    data_with_block_id.blockID = DbManager::instance()->addBlock(slot, data);
    return singleBooking(slot, data_with_block_id);
}

bool WeekViewWidget::multiBooking(const BookingSlot& slot, const BookingData& data)
{
    QDate booking_day = data.aboStart.addDays( slot.date.dayOfWeek() < data.aboStart.dayOfWeek() ? 7 : 0 +
                                               slot.date.dayOfWeek() - data.aboStart.dayOfWeek());
    //check if some slots is alredy booked
    QString all_abo_days_str;
    QVector<qint64> all_abo_days;
    while(booking_day < data.aboEnd)
    {
        qint64 julian_day = booking_day.toJulianDay();
        all_abo_days << julian_day;
        if(!all_abo_days_str.isEmpty())
            all_abo_days_str += ",";
        all_abo_days_str += QString::number(julian_day);
        booking_day = booking_day.addDays(7);
    }
    //check if some slots days is alredy booked
    QSqlQuery query;
    QString query_str = QString("SELECT (surname || ' ' || firstname), info, date, timeslot, fieldid"
                  " FROM bookings LEFT OUTER JOIN members ON bookings.memberid = members.id "
                  " WHERE timeslot = %1"
                  " AND fieldid = %2"
                  " AND aboid != %3"
                  " AND date IN (%4)").arg(slot.timeSlot).arg(slot.fieldID).arg(data.aboID).arg(all_abo_days_str);

    if(query.exec(query_str))
    {
        //qDebug()  << query.lastQuery();
        //qDebug()  << query_str;
        QString conflicts;
        bool have_date = query.first();
        while(have_date)
        {
            QString name = query.value(0).toString();
            QString info = query.value(1).toString();
            QDate date = QDate::fromJulianDay(query.value(2).toLongLong());
            conflicts += QString("\n%1 at %2")
                    .arg(name.isEmpty() ? info : (name + (!info.isEmpty() ? "("+info+")" : "")))
                    .arg(date.toString());
            have_date = query.next();
        }
        if(!conflicts.isEmpty())
        {
            QMessageBox::information(this, QString(tr("Booking conflict")),
                                 QString(tr("The selected time slot is already booked: "))+conflicts);
            return false;
        }
    }


    BookingData data_wiwh_abo_id = data;
    if(data_wiwh_abo_id.aboID == -1)
        if(query.exec("SELECT MAX(aboid) AS max_aboid FROM bookings"))
            if(query.first())
                data_wiwh_abo_id.aboID = query.value(0).toInt() + 1;

//    query.prepare("INSERT INTO bookings (info, memberid, date, timeslot, fieldid, priceid, aboid)"
//                  "VALUES (:info, :memberid, :date, :timeslot, :fieldid, :priceid, :aboid) ;");

//    query.bindValue(":info", info);
//    query.bindValue(":memberid", member_id);

//    query.bindValue(":timeslot", time_slot);
//    query.bindValue(":fieldid", field_id);
//    query.bindValue(":priceid", price_id);
//    query.bindValue(":aboid", abo_id);

    BookingSlot abo_slot = slot;
    for(qint64 julian_day : all_abo_days)
    {
        //query.bindValue(":date", julian_day);
        abo_slot.date = QDate::fromJulianDay(julian_day);

        if(!singleBooking(abo_slot, data_wiwh_abo_id))
        { QMessageBox::information(this, QString(tr("Booking error")),
                                   QString(tr("Error when try to save the multibooking")));
              return false;
        }
    }
    return true;
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
