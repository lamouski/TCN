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
{
    QSqlQueryModel model;
    model.setQuery("SELECT * FROM fields", DbManager::db());
    for (int i = 0; i < model.rowCount(); ++i) {
        int id = model.record(i).value("id").toInt();
        m_fields[id]=model.record(i).value("name").toString();
        qDebug() << id << m_fields[id];
    }

    m_query.prepare("SELECT * FROM :table WHERE date=':day'");
    m_query.bindValue(":table", "bookings");


}

bool DayBookingTableModel::queryData() {
    m_query.bindValue(":day", m_day.toString("yyyy-MM-dd"));
    if(m_query.exec())
        return true;
    else {
        qDebug() << "query day " << m_day.toString("yyyy-MM-dd") << " bookings error:  "
              << m_query.lastError();
    }
}

QVariant DayBookingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole){
        switch(orientation) {
        case Qt::Horizontal: {
            QTime time(section + 9, 0);
            return time.toString("hh:mm");
        }
            break;
        case Qt::Vertical:
            return tr("Field %1").arg(section);
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
    //if(m_query.isValid())
    //    return 3; //number of available fields
    return 3;
}

int DayBookingTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 13; // number of hours in the day
}

QVariant DayBookingTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int col = index.column();

    // generate a log message when this method gets called
//    qDebug() << QString("row %1, col%2, role %3")
//                .arg(row).arg(col).arg(role);

        switch(role){
        case Qt::DisplayRole:
            return QString("name\n %1-%2")
                    .arg(row + 1)
                    .arg(col + 1);
            break;
        case Qt::FontRole:
            if (row == 0 && col == 0) //change font only for cell(0,0)
            {
                QFont boldFont;
                boldFont.setBold(true);
                return boldFont;
            }
            break;
        case Qt::BackgroundRole:

            if (row == 1 && col == 2)  //change background only for cell(1,2)
            {
                QBrush redBackground(Qt::red);
                return redBackground;
            }
            break;
        case Qt::TextAlignmentRole:

            if (row == 1 && col == 1) //change text alignment only for cell(1,1)
            {
                return Qt::AlignRight + Qt::AlignVCenter;
            }
            break;
//        case Qt::CheckStateRole:
//            if (row == 1 && col == 0) //add a checkbox to cell(1,0)
//            {
//                return Qt::Checked;
//            }
        }
        return QVariant();
}

bool DayBookingTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags DayBookingTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
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
    };
}
