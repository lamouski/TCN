#ifndef BLOCKBOOKINGSMODEL_H
#define BLOCKBOOKINGSMODEL_H

#include <QDate>
#include <QModelIndex>
#include <QObject>
#include <QSqlQuery>

class BlockBookingsModel : public QAbstractTableModel
{
public:

    typedef enum
    {
        MODE_BLOCKS,
        MODE_BOOKINGS
    } Mode;

    BlockBookingsModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //Qt::ItemFlags flags(const QModelIndex& index) const override;

    //Date day() const;
    //void setDay(const QDate &day);
    void setMode(Mode mode);
    void setBlockId(int id);

public slots:
    void updateQuery();

    private:
        QSqlQuery m_query;
        QDate m_stat_day, m_end_day;

        Mode m_mode = MODE_BLOCKS;
        int m_blockid = -1;
        int m_count = 0;

};

#endif // BLOCKBOOKINGSMODEL_H
