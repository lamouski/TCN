#ifndef KASSATABLEMODEL_H
#define KASSATABLEMODEL_H

#include <QSqlRelationalTableModel>
#include <QDate>
#include <QObject>
#include <QSqlQuery>

class DayKassaTableModel : public QSqlRelationalTableModel
{
public:
    DayKassaTableModel(QObject *parent);

//    // Header:
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
//    //bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

//    // Basic functionality:
//    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

//    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//    // Editable:
//    bool setData(const QModelIndex &index, const QVariant &value,
//                 int role = Qt::EditRole) override;

//    //Qt::ItemFlags flags(const QModelIndex& index) const override;

    //QDate day() const;
    void setDay(const QDate &day);
    void setOperation(int operation);


protected slots:
    void updateQuery();

private:
//    QSqlQuery m_queryBookings;
//    int m_queryBookingsSize = 0;
//    QSqlQuery m_queryKassa;
//    int m_queryKassaSize = 0;

    int m_operation = 0; //0 - revenues 1 - costs
    QDate m_day;

//    QStringList m_cols_names;

//    bool m_new_record_is_inserting = false;
//    QString m_new_record_info;
//    int m_new_record_type_index;
//    double m_new_record_sum;

};

#endif // KASSATABLEMODEL_H
