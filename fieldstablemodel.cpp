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

#include "dbmanager.h"
#include "fieldstablemodel.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>

#include <math.h>

FieldsTableModel::FieldsTableModel(QObject *parent)
                                   : QAbstractTableModel(parent)
{
    m_query.prepare("SELECT id, name, day0, day1, day2, day3, day4, day5, day6, seasons FROM fields");
    queryData();
}

QVariant FieldsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole){
        switch(orientation)
        {
            case Qt::Horizontal:
                {
                    switch (section)
                    {
                    case 0: return QString(tr("Name"));
                    case 1: return QString(tr("Days"));
                    case 2: return QString(tr("Seasons"));
                    default : return QVariant();
                    }
                }                
            case Qt::Vertical:
                return QString("%1").arg(section);
        }
    }
    return QVariant();
}

int FieldsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_days_masks.size();
}

int FieldsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant FieldsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int col = index.column();

    switch(role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch(col)
            {
            case 0: // name
                const_cast<QSqlQuery&>(m_query).seek(row);
                return m_query.record().value(1).toString();
            case 1: // days mask
                return m_days_masks.at(row);
            case 2: // seasons
                const_cast<QSqlQuery&>(m_query).seek(row);
                return m_query.record().value(9).toInt();
            default:
                return QVariant();
            }
    }
    return QVariant();
}

bool FieldsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    int col = index.column();

    if (data(index, Qt::EditRole) != value)
    {
        switch (role)
        {
        case Qt::EditRole:
            switch(col)
            {
            case 0: // name
            {
                const_cast<QSqlQuery&>(m_query).seek(row);
                int id = m_query.record().value(0).toInt();
                DbManager::instance()->updateFieldName(id, value.toString());
            }
                break;
            case 1: // days mask
            {
                const_cast<QSqlQuery&>(m_query).seek(row);
                int id = m_query.record().value(0).toInt();
                int day_mask = (m_days_masks.at(row) & ~value.toInt()) | (~m_days_masks.at(row) & value.toInt());
                m_days_masks[row] = value.toInt();
                int day_index = (int) log2((double) day_mask);
                int time_mask = m_query.record().value(day_index + 2).toInt();
                if(!time_mask)
                    time_mask = 65520; //default time mask
                if(time_mask & 33554432) //25-th bit is set to 1. it means that day is enabled
                    time_mask = time_mask & 33554431; //disable 25-th bit
                else
                    time_mask = time_mask | 33554432; //enable 25-th bit
                DbManager::instance()->updateFieldTimeMask(id, day_index, time_mask);
            }
                break;
            case 2: // seasons
            {
                const_cast<QSqlQuery&>(m_query).seek(row);
                int id = m_query.record().value(0).toInt();
                DbManager::instance()->updateFieldSeasons(id, value.toInt());
            }
                break;
            }
            break;
        }
        queryData();
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

bool FieldsTableModel::queryData()
{
    if(!m_query.exec())
    {
        qDebug() << "query fields error:  "
              << m_query.lastError();
        return false;
    }

    m_days_masks.clear();
    while(m_query.next())
    {
        int days_mask = 0;
        for(int i = 0; i < 7; i++)
        {
            if(m_query.value(i + 2).toInt() & 33554432) //25-th bit is set then enabled
                days_mask |= (1 << i);
        }
        m_days_masks.push_back(days_mask);
    }
    return true;
}
