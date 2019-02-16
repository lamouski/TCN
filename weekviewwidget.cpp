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
#include "fieldselectiondialog.h"
#include "weekviewwidget.h"
#include "logindatadialog.h"

#include "simplecrypt.h"

#include <QTableWidget>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QTextCodec>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>


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

void WeekViewWidget::markBookingAsPaid(int day, const QModelIndex &index)
{
    if(!m_day_booking_models[day])
        return;

    QVariant curr_data_var = m_day_booking_models[day]->data(index, Qt::UserRole);
    BookingIdDataPair curr_data;
    if(!curr_data_var.isNull())
        curr_data = curr_data_var.value<BookingIdDataPair>();
    else
       return;

    int booking_status = curr_data.second.status;
    if(booking_status > 0)
    {
        QMessageBox::information(this, QString(), QString(tr("This booking is already entered in the cash register.")));
        return;
    }

    int bookingID = m_day_booking_models[day]->bookingId(index);

    DbManager::instance()->markBookingAsPaid(bookingID);
    m_day_booking_models[day]->select();
}

void WeekViewWidget::markBookingAsUnpaid(int day, const QModelIndex &index)
{
    if(!m_day_booking_models[day])
        return;

    QVariant curr_data_var = m_day_booking_models[day]->data(index, Qt::UserRole);
    BookingIdDataPair curr_data;
    if(!curr_data_var.isNull())
        curr_data = curr_data_var.value<BookingIdDataPair>();
    else
       return;

    int booking_status = curr_data.second.status;
    if(booking_status > 0)
    {
        int bookingID = m_day_booking_models[day]->bookingId(index);

        DbManager::instance()->markBookingAsUnpaid(bookingID);
        m_day_booking_models[day]->select();
    }
}


void WeekViewWidget::processBooking(int day, const QModelIndex &index, ProcessingFlag flag)
{
    if(!m_day_booking_models[day])
        return;

    QVariant curr_data_var = m_day_booking_models[day]->data(index, Qt::UserRole);
    BookingIdDataPair curr_data;
    if(!curr_data_var.isNull())
        curr_data = curr_data_var.value<BookingIdDataPair>();
    else
        curr_data.first = -1;

    if(flag == NEW_BOOKING && curr_data.first > 0  && curr_data.second.status >= 0)
        return;

    long long rest_minutes = (QDateTime(m_day_booking_models[day]->day(),
       QTime(m_day_booking_models[day]->timeSlot(index.column()), 0)).toSecsSinceEpoch() -
       QDateTime::currentDateTime().toSecsSinceEpoch()) / 60;

    if( rest_minutes < 0 )
    {
        if( curr_data.second.status > 0 ) //
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

    if(curr_data.first > 0)
    {
        m_booking_dialog->setData(curr_data.second);
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

        bool succses = false;
        if(m_booking_dialog->isSingleBooking())
        {            
            succses = singleBooking(slot, data);
        }
        else if(m_booking_dialog->isBlockBooking())
        {
            succses = blockBooking(slot, data);
        }
        else if(m_booking_dialog->isMultyBooking())
        {
            succses = multiBooking(slot, data);
        }

        if(!succses)
        {
            QMessageBox::information(this, QString(), QString(tr("The booking can't be saved.")));
        }

        m_day_booking_models[day]->select();
        if(Settings::getBool("export_booking_table_html"))
            exportBookingWeekHtml();
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
    if(Settings::getBool("export_booking_table_html"))
        exportBookingWeekHtml();
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

        m_action_mark_paid = new QAction(tr("Mark as paid"), this);
        connect(m_action_mark_paid, &QAction::triggered,
                [this] () { markBookingAsPaid(m_selected_day, m_selected_index ); });

        m_action_mark_unpaid = new QAction(tr("Mark as unpaid"), this);
        connect(m_action_mark_unpaid, &QAction::triggered,
                [this] () { markBookingAsUnpaid(m_selected_day, m_selected_index ); });

        m_action_change_abo_cur = new QAction(tr("Change currient Abo booking"), this);
        connect(m_action_change_abo_cur, &QAction::triggered,
                [this] () { processBooking(m_selected_day, m_selected_index, CURRENT_BOOKING ); });

        m_action_change_abo_all = new QAction(tr("Change complet Abo booking"), this);
        connect(m_action_change_abo_all, &QAction::triggered,
                [this] () { processBooking(m_selected_day, m_selected_index, ALL_BOOKINGS);});

        m_action_change_field_abo_all = new QAction(tr("Change the field for complet Abo"), this);
        connect(m_action_change_field_abo_all, &QAction::triggered,
                [this] () { changeField(m_selected_day, m_selected_index, ALL_BOOKINGS);});

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

        m_action_change_field = new QAction(tr("Change the field"), this);
        connect(m_action_change_field, &QAction::triggered,
                [this] () { changeField(m_selected_day, m_selected_index, CURRENT_BOOKING);});

    }

    QVariant curr_data_var = m_day_booking_models[day]->data(index, Qt::UserRole);
    if(!curr_data_var.isNull())
    {
        BookingIdDataPair curr_data = curr_data_var.value<BookingIdDataPair>();

        m_contextMenu->clear();
        if(curr_data.second.aboID > 0)
        {
            m_contextMenu->addAction(m_action_change_abo_cur);
            m_contextMenu->addAction(m_action_change_abo_all);
            m_contextMenu->addSeparator();
            m_contextMenu->addAction(m_action_change_field);
            m_contextMenu->addAction(m_action_change_field_abo_all);
            m_contextMenu->addSeparator();
            m_contextMenu->addAction(m_action_cancle_abo_cur);
            m_contextMenu->addAction(m_action_cancle_abo_all);
        }
        else
        {
            switch (curr_data.second.status) {
            case -1:
                break;
            case 0:
                m_contextMenu->addAction(m_action_mark_paid);
                m_contextMenu->addAction(m_action_change_booking);
                m_contextMenu->addSeparator();
                m_contextMenu->addAction(m_action_change_field);
                m_contextMenu->addSeparator();
                m_contextMenu->addAction(m_action_cancle_booking);
                break;
            default:
                m_contextMenu->addAction(m_action_mark_unpaid);
            }
        }
        m_contextMenu->exec(m_booking_tables[day]->mapToGlobal(pos));
    }
}

void WeekViewWidget::changeField(int day, const QModelIndex &index, ProcessingFlag flag)
{
    if(!m_day_booking_models[day])
        return;

    QVariant curr_data_var = m_day_booking_models[day]->data(index, Qt::UserRole);

    if(curr_data_var.isNull())
        return;

    BookingIdDataPair curr_data = curr_data_var.value<BookingIdDataPair>();

//    int booking_status = curr_data.second.status;

//    int rest_minutes = (QDateTime(m_day_booking_models[day]->day(),
//       QTime(m_day_booking_models[day]->timeSlot(index.column()), 0)).toSecsSinceEpoch() -
//       QDateTime::currentDateTime().toSecsSinceEpoch()) / 60;

//    if(rest_minutes < 0)
//    {
//        if(booking_status > 0) //
//        {
//            QMessageBox::information(this, QString(), QString(tr("This booking is already entered in the cash register. It can't be changed.")));
//            return;
//        }
//    }

    int bookingID = m_day_booking_models[day]->bookingId(index);


    QStringList fields;
    fields << tr("Please select new field.");
    for(int i = 0; i < m_day_booking_models[day]->rowCount(); i++)
    {
        if(i == index.row())
            continue;
        fields << m_day_booking_models[day]->fieldName(i);
    }
    FieldSelectionDialog filed_selection_dialog;
    filed_selection_dialog.setFields(fields);
    if(filed_selection_dialog.exec() == QDialog::Accepted)
    {
        int selected_row = filed_selection_dialog.selectedFieldRow();
        if(selected_row < 1)
            return;
        selected_row--;
        if(selected_row >= index.row())
            selected_row++;
        int new_fieldID = m_day_booking_models[day]->fieldId(selected_row);

        if(flag == CURRENT_BOOKING)
            DbManager::instance()->changeBookingField(bookingID, new_fieldID);
        else if (flag == ALL_BOOKINGS) {
            DbManager::instance()->changeAboBookingField(bookingID, new_fieldID);
        }


        m_day_booking_models[day]->select();
        if(Settings::getBool("export_booking_table_html"))
            exportBookingWeekHtml();
    };

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
        if(Settings::getBool("cancel_previous_booking_before_update"))
        {
            sucsess = DbManager::instance()->cancleBooking(old_bookingId);
            sucsess &= DbManager::instance()->addBooking(slot, data);
        }
        else
            sucsess = DbManager::instance()->updateBooking(old_bookingId, data);

//        if(old_data.blockID > 0)
//        {
//            if(DbManager::instance()->numOfUsedBlocks(old_data.blockID) == 0)
//                sucsess &= DbManager::instance()->deleteBlock(old_data.blockID);
//        }
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
    QDate booking_day = data.aboStart.addDays( (slot.date.dayOfWeek() < data.aboStart.dayOfWeek() ? 7 : 0) +
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

    BookingData data_with_abo_id = data;
    if(data_with_abo_id.aboID == -1)
        if(query.exec("SELECT MAX(aboid) AS max_aboid FROM bookings"))
            if(query.first())
                data_with_abo_id.aboID = query.value(0).toInt() + 1;

    BookingSlot abo_slot = slot;
    for(qint64 julian_day : all_abo_days)
    {
        //query.bindValue(":date", julian_day);
        abo_slot.date = QDate::fromJulianDay(julian_day);

        if(!singleBooking(abo_slot, data_with_abo_id))
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
    for(int i = 0; i < 7; i ++) {
        DayBookingTableModel* model = m_day_booking_models[i];
        if(!model) {
            model = new DayBookingTableModel(this);
            m_day_booking_models[i] = model;
        }
        if(day != model->day())
        {
            m_booking_tables[i]->setModel(nullptr);
            model->setDay(day);
            m_booking_tables[i]->setModel(model);
        }
        else {
            model->select();
        }
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


void WeekViewWidget::exportBookingWeekHtml()
{

   //initialisation
    QString curr_dir = QDir::currentPath();
    QFile file(curr_dir+"/db/fields_availability.htm");
    if (!file.open(QFile::ReadOnly))
    {
        //template_str = "fields_availability.htm template is not found";
        QMessageBox::information(this, QString(), QString(tr("The file fields_availability.htm template is not found. Please check the settings.")));
        return;
    }

    QByteArray data = file.readAll();
    QTextCodec *codec = QTextCodec::codecForHtml(data);
    QString template_str = codec->toUnicode(data);
    file.close();


    //for result

    data.clear();

    QTextStream stream(&data);

    //
    QDateTime curr_moment = QDateTime::currentDateTime();
    const QDate& date = curr_moment.date();

    QString html_text = template_str;

    const QString table_start_tag("%table_start%");
    int position = html_text.indexOf(table_start_tag);
    const QString table_end_tag("%table_end%");
    int end_position = html_text.indexOf(table_end_tag);
    QString table_string = html_text.mid(position+table_start_tag.length(), end_position - position - table_start_tag.length());
    QString doc_begin = html_text.left(position);
    QString doc_end = html_text.mid(end_position + table_end_tag.length());


    const QString table_row_start_tag("%table_row_start%");
    position = table_string.indexOf(table_row_start_tag);
    const QString table_row_end_tag("%table_row_end%");
    end_position = table_string.indexOf(table_row_end_tag);
    QString row_string = table_string.mid(position+table_row_start_tag.length(), end_position - position - table_row_start_tag.length());
    table_string.remove(position, end_position - position + table_row_end_tag.length());

    DayBookingTableModel day_booking_model;

    doc_begin.replace("%last_update_day%", date.toString("dd.MM.yyyy"));
    doc_begin.replace("%last_update_time%", curr_moment.time().toString("hh:mm:ss"));
    stream << doc_begin;

    for(int days_count = 0; days_count < 7; days_count ++)
    {
        QDate table_date = date.addDays(days_count);

        day_booking_model.setDay(table_date);
        int rows = day_booking_model.rowCount();
        //int cols = day_booking_model.columnCount();

        QString tmp_table_string = table_string;
        QString day_info_key = "\%day_information\%";
        QString day_info_string = table_date.toString("dddd, dd MMMM yyyy");
        tmp_table_string.replace(day_info_key, day_info_string);
        int tmp_position = position - day_info_key.length() + day_info_string.length();
        for(int row = 0; row < rows; row++)
        {
            QString tmp_string = row_string;
            tmp_string.replace("%field_name%", day_booking_model.fieldName(row));
            int first_time_slot = day_booking_model.timeSlot(0);
            for (int time_slot = 8; time_slot < 23; time_slot++)
            {
                int col = time_slot - first_time_slot;
                QModelIndex index = day_booking_model.index(row, col);
                int booking_id  = day_booking_model.bookingId(index);
                int booking_status = day_booking_model.bookingStatus(index);
                QString col_key = QString("\%data%1\%").arg(time_slot);
                if(booking_id > 0 && booking_status >= 0)
                    tmp_string.replace(col_key, tr("Booked")); //"Belegt"
                else
                    tmp_string.replace(col_key, "");
            }

            tmp_table_string.insert(tmp_position,tmp_string); tmp_position += tmp_string.length();

        }

        stream << tmp_table_string;
    }

    stream << doc_end;
    stream.flush();


    QString export_type = Settings::getString("booking_table_html_typ");
    if(export_type == "file")
    {
        QString fileName = Settings::getString("booking_table_html_path");
        file.setFileName(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
        {
            QMessageBox::information(this, QString(), QString(tr("The file for export %1 can't be created. Please check the settings.").arg(fileName)));
            return;
        }
        file.write(data);
        file.close();
    }
    else if (export_type == "ftp") {

        if(!m_qnam)
        {
            m_qnam = new QNetworkAccessManager(this);

            connect(m_qnam, &QNetworkAccessManager::finished,
                [this](QNetworkReply* rep)
                {
                    if(rep->error() != 0)
                    {
                        QMessageBox::information(this, QString(), QString(tr("Upload is finished with error %1.").arg(rep->errorString())));
                    }
                    else
                    {
                        //m_statusBar->
                    }
                });

            SimpleCrypt crypto(Q_UINT64_C(0xd47fc5e668e23524));

            QString fileName = "ftp.conf";
            file.setFileName(fileName);
            if (file.open(QFile::ReadOnly | QFile::Text))
            {
                QTextStream login_data_stream(&file);
                m_login = login_data_stream.readLine();
                m_password = crypto.decryptToString(login_data_stream.readLine());
                file.close();
            }
            if(m_login.isEmpty())
            {
                LoginDataDialog dialog;
                if(dialog.exec() == QDialog::Accepted)
                {
                    m_login = dialog.getLogin();
                    m_password = dialog.getPassword();

                    if (file.open(QFile::WriteOnly | QFile::Text))
                    {
                        QTextStream login_data_stream(&file);
                        login_data_stream << m_login << '\n';
                        login_data_stream << crypto.encryptToString(m_password) << '\n';
                        file.close();
                    }
                }
            }
        }

        QString path = Settings::getString("booking_table_html_path");
        QUrl url = QUrl(path);
        url.setUserName(m_login);
        url.setPassword(m_password);
        url.setPort(21);

        QNetworkRequest upload(url);
        m_qnam->put(upload, data);
    }
}
