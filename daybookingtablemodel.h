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
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QDate day() const;
    void setDay(const QDate &day);

protected slots:
    bool queryData();

private:
    QSqlQuery m_query;
    QDate m_day;

    QMap<int, QString> m_fields;
};

#endif // DAYBOOKINGTABLEMODEL_H
