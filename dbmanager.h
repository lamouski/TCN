#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QDate>
#include <QSqlDatabase>
#include <QString>

class DbManager
{
public:
    DbManager(const QString& path);
    bool addMember(const QString &name, const QString &surname);
    bool addBooking(const int memberID, const QDate &date, const int tymeslot, const int fieldid, const int priceid);

    bool checkDB();

    static DbManager* instance();    
    static QSqlDatabase db();

private:
    static DbManager* m_instance;
    QSqlDatabase m_db;

};

#endif // DBMANAGER_H
