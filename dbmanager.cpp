#include "dbmanager.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

DbManager* DbManager::m_instance = nullptr;

DbManager::DbManager(const QString& path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
    {
      qDebug() << "Error: connection with database fail";
    }
    else
    {
      qDebug() << "Database: connection ok";
      if(m_instance)
          delete m_instance;
      m_instance = this;
    }
}

bool DbManager::addMember(const QString &name, const QString &surname)
{

   bool success = false;
   // you should check if args are ok first...
   QSqlQuery query;
   query.prepare("INSERT INTO members (firstname, surname) VALUES (:name, :surname)");
   query.bindValue(":name", name);
   query.bindValue(":surname", surname);
   if(query.exec())
   {
       success = true;
   }
   else
   {
        qDebug() << "addPerson error:  "
                 << query.lastError();
   }
   return success;
}

bool DbManager::addBooking(const int memberID, const QDate& date, const int timeSlot, const int fieldID, const int priceID)
{

   bool success = false;
   // you should check if args are ok first...
   QSqlQuery query;
   query.prepare("INSERT INTO bookings "
                 "VALUES (:memberid, :date, :timeslot, : fieldid, :priceid)");
   query.bindValue(":memberid", memberID);
   query.bindValue(":date", date.toString("yyyy-MM-dd"));
   query.bindValue(":timeslot", timeSlot);
   query.bindValue(":fieldid", fieldID);
   query.bindValue(":priceid", priceID);
   if(query.exec())
   {
       success = true;
   }
   else
   {
        qDebug() << "addBooking error:  "
                 << query.lastError();
   }
   return success;
}

bool DbManager::checkDB()
{
    QStringList table_names = m_db.tables();
    if ( !table_names.contains( QLatin1String("members") )) {
         QSqlQuery query;
         query.exec("create table IF NOT EXISTS members"
                   "(id integer primary key, "
                   "firstname varchar(100), "
                   "surname varchar(100))");
    }
    if ( !table_names.contains( QLatin1String("fields") )) {
     QSqlQuery query;
     query.exec("create table IF NOT EXISTS fields"
                "(id integer primary key, "
                "name vchar(25), "
                "days set(1,2,3,4,5,6,7)"
                "seasons set('Sommer','Winter')");
     query.prepare("INSERT (name, days, seasons) INTO fields VALUES ('Field 5', '1,2,3,4,5,6,7', 'Sommer')");
     query.prepare("INSERT (name, days, seasons) INTO fields VALUES ('Field 6', '1,2,3,4,5,6,7', 'Sommer')");
     query.prepare("INSERT (name, days, seasons) INTO fields VALUES ('Field 7', '1,2,3,4,5,6,7', 'Sommer')");
     query.prepare("INSERT (name, days, seasons) INTO fields VALUES ('Field 1', '6,7', 'Sommer')");


    }
    if ( !table_names.contains( QLatin1String("bookings") )) {
     QSqlQuery query;
     query.exec("create table IF NOT EXISTS bookings"
                "(memberid integer, "
                "date DATE(100), "
                "timeslot integer,"
                "fieldid integer,"
                "priceid integer)");

    }
    if ( !table_names.contains( QLatin1String("prices") )) {
     QSqlQuery query;
     query.exec("create table IF NOT EXISTS prices"
                "(id integer primary key, "
                "price smallmoney)");
    }

    return true;
}

DbManager *DbManager::instance()
{
    return m_instance;
}

QSqlDatabase DbManager::db()
{
    if(m_instance)
        return m_instance->m_db;
    else
        return QSqlDatabase();
}
