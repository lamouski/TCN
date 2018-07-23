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

#include "daybookingtablemodel.h"

#include <QBrush>
#include <QDate>
#include <QFont>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include <QSqlError>

#include "dbmanager.h"

DayBookingTableModel::DayBookingTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_first_time_slot(8)
    , m_nr_time_slots(15)
    , m_season(2)
{

}

bool DayBookingTableModel::queryData() {

    int day_mask = 1 << (m_day.dayOfWeek() - 1);
    int season_mask = 1 << (m_season - 1);
    QSqlQuery field_query(QString("SELECT id, name, days, seasons FROM fields "
                                  "WHERE (`days` & %1) = %1 "
                                  "AND (`seasons` & %2) = %2;").arg(day_mask).arg(season_mask));
    m_fields_IDis.clear();
    m_fields_names.clear();
    while (field_query.next())
    {
        m_fields_IDis.push_back(field_query.value(0).toInt());
        m_fields_names.push_back(field_query.value(1).toString());
    }
    m_query.prepare("SELECT (surname || ' ' || firstname), date, timeslot, fieldid, sum, memberid, priceid "
                    " FROM bookings LEFT OUTER JOIN members ON bookings.memberid = members.id "
                    " LEFT OUTER JOIN prices ON bookings.priceid = prices.id "
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
        int timeslot = m_query.value(2).toInt();
        int fieldId = m_query.value(3).toInt();
        m_index_hash[QPair<int, int>(fieldId, timeslot)] = ind++;
    }

   //emit QAbstractListModel::dataChanged(index(0,0), index(0,0));
    return true;
}

QVariant DayBookingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole){
        switch(orientation) {
        case Qt::Horizontal: {
            QTime time(section + m_first_time_slot, 0); //todo define start timeslot and number of timeslots
            return time.toString("hh:mm");
        }
            break;
        case Qt::Vertical:
            return m_fields_names[section];
        }
    }
    return QVariant();
}

bool DayBookingTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
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

    switch(role)
    {
        case Qt::DisplayRole:
            if(m_index_hash.contains(index_key))
            {
                int index = m_index_hash[index_key];
                const_cast<QSqlQuery&>(m_query).seek(index);
                QString member_mame = m_query.record().value(0).toString();
                QVariant price_variant =m_query.record().value(4);
                double price = price_variant.toDouble();
                return QString("%1 (%2)").arg(member_mame).arg(price);
            }            
            break;
        case Qt::UserRole:
            if(m_index_hash.contains(index_key))
            {
                int index = m_index_hash[index_key];
                const_cast<QSqlQuery&>(m_query).seek(index);
                int memberId = m_query.record().value(5).toInt();
                int priceId = m_query.record().value(6).toInt();
                QPair<int, int> member_price_pair(memberId, priceId);
                return QVariant::fromValue(member_price_pair);
            }
            else
                return QVariant();
            break;

/*        case Qt::FontRole:
            if (row == 0 && col == 0) //change font only for cell(0,0)
            {
                QFont boldFont;
                boldFont.setBold(true);
                return boldFont;
            }
            break;*/
        case Qt::BackgroundRole:
            if(m_index_hash.contains(index_key))
            {
                QBrush background;
                switch (index.row() % 3) {
                case 0:
                    background = QBrush(qRgb(174, 234, 174));
                    break;
                case 1:
                    background = QBrush(qRgb(154, 229, 154));
                    break;
                case 2:
                    background = QBrush(qRgb(133, 224, 133));
                    break;
                default:
                    break;
                }
                return background;
            }
            else
            {
                int minute = (QDateTime(m_day, QTime(index.column() + m_first_time_slot, 0)).toSecsSinceEpoch() - QDateTime::currentDateTime().toSecsSinceEpoch()) / 60;
                if(minute < 0)
                    return QBrush(qRgb(242, 242, 242));
                if(minute < 15)
                    return QBrush(qRgb(255, 51, 0));
                if(minute < 30)
                    return QBrush(qRgb(255, 153, 0));
                return QBrush(qRgb(255, 242, 230));
            }
            break;
        case Qt::TextAlignmentRole:
            if(m_index_hash.contains(index_key))
            {
                return Qt::AlignLeft + Qt::AlignVCenter;
            }
            break;
        }
    return QVariant();
}

bool DayBookingTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        switch (role) {
        case Qt::UserRole:
            QPair<int, int> member_price_pair = value.value<QPair<int, int> >();
            DbManager::instance()->addBooking(member_price_pair.first,
                                              m_day,
                                              index.column() + m_first_time_slot,
                                              m_fields_IDis[index.row()],
                                              member_price_pair.second);
            break;
        }
        queryData();
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags DayBookingTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled;
}

QDate DayBookingTableModel::day() const
{
    return m_day;
}


void DayBookingTableModel::setDay(const QDate &day)
{
    if(m_day != day) {
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

QString DayBookingTableModel::fieldName(int row) const
{
    if(row < m_fields_names.size())
        return m_fields_names[row];
    else
        return QString();
}

int DayBookingTableModel::timeSlot(int column) const
{
    return column + m_first_time_slot;
}

void DayBookingTableModel::setFirstTimeSlot(int first_time_slot)
{
    m_first_time_slot = first_time_slot;
}
