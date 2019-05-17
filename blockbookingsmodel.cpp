#include <QFont>
#include <QSqlRecord>

#include "blockbookingsmodel.h"

BlockBookingsModel::BlockBookingsModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant BlockBookingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole){
        switch(orientation)
        {
            case Qt::Horizontal:
                {
                    switch (section)
                    {
                    case 0: return QString(tr("Date"));
                    case 1: return QString(tr("Block ID"));
                    case 2: return QString(tr("Info"));
                    case 3: return m_mode == MODE_BLOCKS ? QString(tr("Amount")) : QString(tr("Sum"));
                    case 4: return QString(tr("Field"));
                    case 5: return QString(tr("Time"));
                    case 6: return QString(tr("Status"));
                    default : return QVariant();
                    }
                }
            case Qt::Vertical:
                return QString("%1").arg(section);
        }
    }
    return QVariant();

}

int BlockBookingsModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_count;
}

int BlockBookingsModel::columnCount(const QModelIndex &/*parent*/) const
{
    switch (m_mode)
    {
    case MODE_BLOCKS:
        return 4;
    case MODE_INFO:
        return 3;
    case MODE_BOOKINGS:
        return 7;
    }
}

QVariant BlockBookingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int col = index.column();

    const_cast<QSqlQuery&>(m_query).seek(row);

    switch(role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch(col)
            {
            case 0: // date
                return QDate::fromJulianDay(m_query.value(0).toInt()).toString();
            case 1: // id
            {                
                return m_query.value(1).toInt();
            }            
            case 2: // info
            {
                QString info = m_query.value(2).toString();
                if(!m_query.value(3).isNull())
                {
                    info += " (" + m_query.value(3).toString() + ")";
                }
                if(m_mode == MODE_INFO)
                {
                   info += " (" +
                           tr("booked ") + m_query.record().value(4).toString() + ", " +
                           tr("at ") + QDate::fromJulianDay(m_query.value(0).toInt()).toString() + ", " +
                           tr("used ") + m_query.record().value(5).toString() + " )";
                }
                return info;
            }
            case 3: // amount or sum
                return m_query.record().value(4).toString();
            case 4: // field
                return m_query.record().value(5).toString();
            case 5: // time
                return m_query.record().value(6).toString() + ":00";
            case 6: // status
            {
                int status = m_query.record().value(7).toInt();
                if(status == -1)
                        return tr("Canceled");
                if(status > 0)
                        return tr("Payed");
                return tr("Not payed");
            }
            default:
                return QVariant();
            }
        case Qt::FontRole:
            {
                QFont standart_font;
                int status = m_query.record().value(7).toInt();
                if(status < 0)
                {
                    standart_font.setStrikeOut(true);
                    standart_font.setWeight(QFont::ExtraLight);
                }
                else if(status == 0 && col == 6)
                {
                    standart_font.setBold(true);
                }
                return QVariant::fromValue<QFont>(standart_font);

            }

    }
    return QVariant();
}


QVariant BlockBookingsModel::plainData(int row, int col) const
{
    const_cast<QSqlQuery&>(m_query).seek(row);
    return m_query.value(col);
}


void BlockBookingsModel::setMode(BlockBookingsModel::Mode mode)
{
    if(m_mode != mode)
    {
        m_mode = mode;
        updateQuery();        
    }
}

void BlockBookingsModel::setBlockId(int id)
{
    if(m_blockid != id)
    {
        m_blockid = id;
        updateQuery();
    }
}

void BlockBookingsModel::setConditions(const QString &conditions)
{
//    if(m_additional_condition != conditions)
//    {
        m_additional_condition = conditions;
        updateQuery();
//   }
}

void BlockBookingsModel::updateQuery()
{
    if(m_mode == MODE_BLOCKS)
        m_query.exec(QString("SELECT block_bookings.date, block_bookings.id, (surname || ' ' || firstname) as name_info, "
                     "block_bookings.info, block_bookings.amount from block_bookings "
                     "LEFT OUTER JOIN members ON members.id=block_bookings.memberid "
                     "ORDER BY block_bookings.date"));
    else if(m_mode == MODE_BOOKINGS)
        m_query.exec(QString("SELECT bookings.date, bookings.blockid, (surname || ' ' || firstname) as name_info, bookings.info, bookings.sum, fields.name, timeslot, bookings.status from bookings "
                     "LEFT OUTER JOIN members ON members.id=bookings.memberid "
                     "LEFT OUTER JOIN fields ON fields.id=bookings.fieldid "
                     "WHERE bookings.blockid IS NOT NULL ") +
                     (m_blockid > 0 ? QString("AND bookings.blockid=%1 ").arg(m_blockid) : "") +
                     QString("ORDER BY bookings.date "));
    else if(m_mode == MODE_INFO)
    {
         m_query.exec(QString("SELECT block_bookings.date, block_bookings.id, (surname || ' ' || firstname) as name_info,"
                      "block_bookings.info, block_bookings.amount, "
                      "(SELECT count(id) FROM bookings "
                              "WHERE bookings.blockid=block_bookings.id "
                              "AND (bookings.status IS NULL OR bookings.status!=-1) ) AS used_amount, "
                      "block_bookings.priceid, block_bookings.memberid "
                      "FROM block_bookings "
                      "LEFT OUTER JOIN members ON block_bookings.memberid = members.id ") +
                      (!m_additional_condition.isEmpty() ? QString("WHERE %1 ").arg(m_additional_condition) : "") +
                      QString("ORDER BY block_bookings.date "));

    }
    else
    {
        m_count = 0;
        emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
        return;
    }
    int count = 0;
    while(m_query.next())
        count++;
    m_count = count;

    emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    emit layoutChanged();
}
