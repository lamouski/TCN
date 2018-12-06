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

#ifndef DAYBOOKINGTABLEMODEL_H
#define DAYBOOKINGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QDate>
#include <QSqlQuery>

class DayBookingTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DayBookingTableModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
//    bool setData(const QModelIndex &index, const QVariant &value,
//                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QDate day() const;
    int fieldId(int row) const;
    QString fieldName(int row) const;
    int timeSlot(int column) const;

    void setDay(const QDate &day);

    void setFirstTimeSlot(int first_time_slot);
    void setNumOfTimeSlots(int num_of_time_slots);

    int aboId(const QModelIndex &index);    
    int bookingStatus(const QModelIndex &index);

public slots:
    void setPreviousWeek();
    void setNextWeek();

    void select();
protected slots:
    bool queryData();

private:
    QSqlQuery m_query;    

    QDate m_day;

    QHash<QPair<int, int>, int> m_index_hash;

    QVector<int> m_fields_IDis;
    QStringList m_fields_names;
    QVector<int> m_fields_time_masks;

    int m_first_time_slot;
    int m_nr_time_slots;
};

#endif // DAYBOOKINGTABLEMODEL_H
