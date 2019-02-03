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

#define QF_ID 0
#define QF_NAME 1
#define QF_INFO 2
#define QF_TIMESLOT 3
#define QF_FIELDID 4
#define QF_MEMBERID 5
#define QF_PRICEID 6
#define QF_ABOID 7
#define QF_BLOCKID 8
#define QF_STATUS 9
#define QF_SUM 10

#include "daybookingtablemodel.h"

#include <QBrush>
#include <QDate>
#include <QFont>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include <QSqlError>

#include <math.h>

#include "dbmanager.h"
#include "settings.h"

DayBookingTableModel::DayBookingTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_first_time_slot(9)
    , m_nr_time_slots(14)
    {

}

bool DayBookingTableModel::queryData()
{

    int season_mask = Settings::winterSeason() ? 1 : 2;
    QSqlQuery field_query(QString("SELECT id, name, day%1, seasons FROM fields "
                                  "WHERE (`day%1` & 33554432) = 33554432 " //25-th bit is set to 1
                                  "AND (`seasons` & %2) = %2;").arg(m_day.dayOfWeek() - 1).arg(season_mask));
    m_fields_IDis.clear();
    m_fields_names.clear();
    m_fields_time_masks.clear();
    while (field_query.next())
    {
        m_fields_IDis.push_back(field_query.value(0).toInt());
        m_fields_names.push_back(field_query.value(1).toString());
        int time_mask = field_query.value(2).toInt() & 33554431; // we mask the 25-th bit
        m_fields_time_masks.push_back(time_mask);
        int strange_number = time_mask & (~time_mask + 1);
        int first_time_slot = (int) std::log2( (float) strange_number);
        m_first_time_slot = std::min(m_first_time_slot, first_time_slot);
        int last_time_slot = (int) std::log2( (float) time_mask);
        m_nr_time_slots = std::max(m_first_time_slot + m_nr_time_slots, last_time_slot) - m_first_time_slot;
    }
    m_query.prepare("SELECT bookings.id, (surname || ' ' || firstname), info, timeslot, fieldid, memberid, priceid, aboid, blockid, status, sum"
                    " FROM bookings LEFT OUTER JOIN members ON bookings.memberid = members.id "
                    " WHERE date = :day ");
    m_query.bindValue(":day", m_day.toJulianDay());
    if(!m_query.exec())
    {
        qDebug() << "query day " << m_day.toString("yyyy-MM-dd") << " bookings error:  "
              << m_query.lastError();
        return false;
    }

    m_index_hash.clear();
    int ind = 0;
    while(m_query.next()) {
        int timeslot = m_query.value(QF_TIMESLOT).toInt();
        int fieldId = m_query.value(QF_FIELDID).toInt();
        m_index_hash[QPair<int, int>(fieldId, timeslot)] = ind++;
    }

   //emit QAbstractListModel::dataChanged(index(0,0), index(0,0));
    return true;
}

QVariant DayBookingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole){
        switch(orientation) {
        case Qt::Horizontal:
        {
            QTime time(section + m_first_time_slot, 0); //todo define start timeslot and number of timeslots
            return time.toString("hh:mm");
        }
        case Qt::Vertical:
            return m_fields_names[section];
        }
    }
    return QVariant();
}

bool DayBookingTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role))
    {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int DayBookingTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_fields_IDis.size();
}

int DayBookingTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_nr_time_slots; // number of hours in the day
}

QVariant DayBookingTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int col = index.column();

    QPair<int, int> index_key(m_fields_IDis[row], m_first_time_slot + col);

    int time_mask = m_fields_time_masks[row];
    bool time_slot_is_active = (time_mask & (1 << (m_first_time_slot + col))) != 0;

    switch(role)
    {
        case Qt::DisplayRole:
            if(m_index_hash.contains(index_key))
            {
                const_cast<QSqlQuery&>(m_query).seek(m_index_hash[index_key]);
                QString member_name = m_query.value(QF_NAME).toString();
                QString booking_info = m_query.value(QF_INFO).toString();

//                int abo_id = m_query.value(QF_ABOID).toInt();
                //double price = m_query.value(QF_SUM).toDouble();
                QString result = !member_name.isEmpty() ? member_name : booking_info;
//                if(abo_id > 0)
//                    result += " (Abo)";
//                else
//                    result += QString(" (%2 EUR)").arg(price);
                return result;
            }            
            break;
        case Qt::UserRole:
            if(m_index_hash.contains(index_key))
            {
                const_cast<QSqlQuery&>(m_query).seek(m_index_hash[index_key]);
                BookingData data;

                data.memberID = m_query.value(QF_MEMBERID).toInt();
                data.priceID = m_query.value(QF_PRICEID).toInt();
                data.booking_info = m_query.value(QF_INFO).toString();
                data.sum = m_query.value(QF_SUM).toFloat();
                data.blockID = m_query.value(QF_BLOCKID).toInt();
                data.aboID = m_query.value(QF_ABOID).toInt();
                data.status = m_query.value(QF_STATUS).toInt();

                int bookingId = m_query.value(QF_ID).toInt();

                return QVariant::fromValue<BookingIdDataPair>(BookingIdDataPair(bookingId, data));

            }
            else
                return QVariant();
        case Qt::BackgroundRole:
            if(!time_slot_is_active)
            {
                return QBrush(qRgb(242, 242, 242));
            }
            if(m_index_hash.contains(index_key))
            {
                const_cast<QSqlQuery&>(m_query).seek(m_index_hash[index_key]);
                const int status = m_query.value(QF_STATUS).toInt();
                if(status >= 0) //
                {
                    int abo_id = m_query.value(QF_ABOID).toInt();
                    int lightnes = 80;
                    switch (index.row() % 3) {
                    case 0:
                        lightnes = 160;// 80%
                        break;
                    case 1:
                        lightnes = 191; //~75%
                        break;
                    case 2:
                        lightnes = 179; //~70%
                        break;
                    default:
                        break;
                    }
                    QColor base_color;
                    if(abo_id <= 0)
                        if(status == 0)
                            base_color = QColor::fromHsl(60, 150, lightnes);
                        else
                            base_color = QColor::fromHsl(120, 150, lightnes);
                    else
                        base_color = QColor::fromHsl(180, 150, lightnes);
                    return QBrush(base_color);
                }
            }            
            {
                int minute = (QDateTime(m_day, QTime(index.column() + m_first_time_slot, 0)).toSecsSinceEpoch() -
                              QDateTime::currentDateTime().toSecsSinceEpoch()) / 60;
                if(minute < 0)
                    return QBrush(qRgb(242, 242, 242));
                if(minute < 15)
                    return QBrush(qRgb(255, 51, 0));
                if(minute < 30)
                    return QBrush(qRgb(255, 153, 0));
                return QBrush(qRgb(255, 242, 230));
            }
        case Qt::TextAlignmentRole:
            if(m_index_hash.contains(index_key))
            {
                return Qt::AlignLeft + Qt::AlignVCenter;
            }
            break;
        case Qt::FontRole:
            {
                if(m_index_hash.contains(index_key))
                {
                   const_cast<QSqlQuery&>(m_query).seek(m_index_hash[index_key]);
                   int status = m_query.value(QF_STATUS).toInt();
                   if(status < 0)
                   {
                       QFont standart_font;
                        standart_font.setStrikeOut(true);
                        standart_font.setWeight(QFont::ExtraLight);
                        return QVariant::fromValue<QFont>(standart_font);
                   }
                }
            }
            break;
    }
    return QVariant();
}

Qt::ItemFlags DayBookingTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    const int row = index.row();
    const int col = index.column();
    const int time_mask = m_fields_time_masks[row];
    const bool time_slot_is_active = (time_mask & (1 << (m_first_time_slot + col))) != 0;
    if(time_slot_is_active)
        return Qt::ItemIsEnabled;
    else
        return Qt::NoItemFlags;
}

QDate DayBookingTableModel::day() const
{
    return m_day;
}

void DayBookingTableModel::setDay(const QDate &day)
{
    if(m_day != day)
    {
        m_day = day;
        queryData();
        emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    };
}

void DayBookingTableModel::setPreviousWeek()
{
    setDay(m_day.addDays(-7));
}

void DayBookingTableModel::setNextWeek()
{
    setDay(m_day.addDays(7));
}

void DayBookingTableModel::select()
{
    queryData();
}

int DayBookingTableModel::fieldId(int row) const
{
    if(row < m_fields_IDis.size())
        return m_fields_IDis[row];
    else
        return -1;
}

QString DayBookingTableModel::fieldName(int row) const
{
    if(row < m_fields_names.size())
        return m_fields_names[row];
    else
        return QString();
}

int DayBookingTableModel::aboId(const QModelIndex &index)
{
    if (!index.isValid())
        return 0;

    int row = index.row();
    int col = index.column();

    QPair<int, int> index_key(m_fields_IDis[row], m_first_time_slot + col);
    if(m_index_hash.contains(index_key))
    {
        const_cast<QSqlQuery&>(m_query).seek(m_index_hash[index_key]);
        return m_query.value(QF_ABOID).toInt();
    }
    return 0;
}

int DayBookingTableModel::bookingStatus(const QModelIndex &index)
{
    if (!index.isValid())
        return 0;

    int row = index.row();
    int col = index.column();

    QPair<int, int> index_key(m_fields_IDis[row], m_first_time_slot + col);
    if(m_index_hash.contains(index_key))
    {
        const_cast<QSqlQuery&>(m_query).seek(m_index_hash[index_key]);
        return m_query.value(QF_STATUS).toInt();
    }
    return 0;
}

int DayBookingTableModel::bookingId(const QModelIndex &index)
{
    if (!index.isValid())
        return 0;

    int row = index.row();
    int col = index.column();

    QPair<int, int> index_key(m_fields_IDis[row], m_first_time_slot + col);
    if(m_index_hash.contains(index_key))
    {
        const_cast<QSqlQuery&>(m_query).seek(m_index_hash[index_key]);
        return m_query.value(QF_ID).toInt();
    }
    return 0;
}

int DayBookingTableModel::timeSlot(int column) const
{
    return column + m_first_time_slot;
}

void DayBookingTableModel::setFirstTimeSlot(int first_time_slot)
{
    m_first_time_slot = first_time_slot;
}
