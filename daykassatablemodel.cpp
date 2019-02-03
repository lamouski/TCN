

#include <QBrush>
#include <QDate>
#include <QFont>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include <QSqlError>

#include "dbmanager.h"
#include "settings.h"

#include "daykassatablemodel.h"

DayKassaTableModel::DayKassaTableModel(QObject *parent)
    : QSqlRelationalTableModel(parent)
{
//    m_cols_names.push_back(tr("Info"));
//    m_cols_names.push_back(tr("Type"));
//    m_cols_names.push_back(tr("Account"));
//    m_cols_names.push_back(tr("Sum"));
}

//QVariant DayKassaTableModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    if (role == Qt::DisplayRole){
//        switch(orientation) {
//        case Qt::Horizontal:
//        {
//            return m_cols_names[section];
//            }
//        case Qt::Vertical:
//            return QString("%1").arg(section);
//        }
//    }
//    return QVariant();
//}

//int DayKassaTableModel::rowCount(const QModelIndex &/*parent*/) const
//{
//    return m_queryKassaSize + m_queryBookingsSize;
//}

//int DayKassaTableModel::columnCount(const QModelIndex &/*parent*/) const
//{
//    return m_cols_names.size();
//}

//QVariant DayKassaTableModel::data(const QModelIndex &index, int role) const
//{
//    if (!index.isValid())
//        return QVariant();

//    int row = index.row();
//    int col = index.column();

//    switch(role)
//    {
//        case Qt::DisplayRole:
//            if(row < m_queryKassaSize)
//            {
//                const_cast<QSqlQuery&>(m_queryKassa).seek(row);
//                QString string = m_queryKassa.value(col).toString();
//                if(col == 3)
//                {
//                    string += "€";
//                }
//                return string;
//            }
//            else
//            {
//                int tmp_row = row - m_queryKassaSize;
//                const_cast<QSqlQuery&>(m_queryBookings).seek(tmp_row);
//                QString string = m_queryBookings.value(col).toString();
//                if(col == 0)
//                {
//                    QString additional_info = m_queryBookings.value(4).toString();
//                    if(!additional_info.isNull())
//                    {
//                        string += " " + additional_info;
//                    }
//                }
//                if(col == 3)
//                {
//                    string += "€";
//                }
//                return string;
//            }
//            break;
////        case Qt::UserRole:
////            break;
////        case Qt::BackgroundRole:
////            break;
//        case Qt::TextAlignmentRole:
//            switch(col)
//            {
//            case 0: default:
//                return Qt::AlignLeft + Qt::AlignVCenter;
//            case 1:
//                return Qt::AlignLeft + Qt::AlignVCenter;
//            case 2:
//                return Qt::AlignCenter;
//            case 3:
//                return Qt::AlignRight + Qt::AlignVCenter;
//            }

////        case Qt::FontRole:
////            break;
//    }
//    return QVariant();
//}

void DayKassaTableModel::setDay(const QDate &day)
{
    if(m_day != day)
    {
        m_day = day;
        updateQuery();
        emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    };
}


void DayKassaTableModel::setOperation(int operation)
{
    if(m_operation != operation)
    {
        m_operation = operation;
        updateQuery();
        emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    }
}

//bool DayKassaTableModel::insertNew()
//{
//    m_new_record_is_inserting = true;

//    m_new_record_info = QString();
//    m_new_record_type_index = -1;
//    m_new_record_sum = 0.0;

//    //m_busyInsertingRows = false;

//    return true;
//}


void DayKassaTableModel::updateQuery()
{


    QSqlQuery query = QSqlQuery(QString("SELECT info, account_name, account, sum FROM cash_register "
                  "LEFT OUTER JOIN accounts ON cash_register.account = revenues.number "
                  "WHERE date = %1 "
                  "AND operation = %2 ").arg(Settings::currentDate().toJulianDay()).arg(0));
    setQuery(query);

//    if(m_operation == 0)
//    {
//        m_queryBookings.prepare("SELECT (surname || ' ' || firstname), type, account, bookings.sum, info, status FROM bookings "
//                      "LEFT OUTER JOIN members ON bookings.memberid = members.id "
//                      "LEFT OUTER JOIN prices ON bookings.priceid = prices.id "
//                      "LEFT OUTER JOIN revenues ON prices.revenue = revenues.id "
//                      "WHERE date = :day "
//                      "AND (aboid IS NULL OR aboid <= 0)");
//        m_queryBookings.bindValue(":day", m_day.toJulianDay());
//        if(!m_queryBookings.exec())
//        {
//            qDebug() << "query day " <<m_day.toString("yyyy-MM-dd") << " bookings error:  "
//                  << m_queryBookings.lastError();
//        }
//        if(m_queryBookings.last())
//            m_queryBookingsSize = m_queryBookings.at() + 1;
//        else
//            m_queryBookingsSize = 0;
//    }
//    else {
//       m_queryBookings.clear();
//       m_queryBookingsSize = 0;
//    }

//    m_queryKassa.prepare("SELECT info, account_name, account, sum FROM cash_register "
//                  "LEFT OUTER JOIN accounts ON cash_register.account = accounts.number "
//                  "WHERE date = :day "
//                  "AND operation = :operation ");
//    m_queryKassa.bindValue(":day", m_day.toJulianDay());
//    m_queryKassa.bindValue(":operation", m_operation);
//    if(!m_queryKassa.exec())
//    {
//        qDebug() << "query day " <<m_day.toString("yyyy-MM-dd") << " kassa error:  "
//              << m_queryKassa.lastError();
//    }
//    if(m_queryKassa.last())
//        m_queryKassaSize = m_queryKassa.at() + 1;
//    else
//        m_queryKassaSize = 0;
}
