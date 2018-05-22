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

bool DbManager::addField(const QString& name, const int days, const int seasons)
{
   bool success = false;
   // you should check if args are ok first...
   QSqlQuery query;
   query.prepare("INSERT INTO fields (name, days, seasons) "
                 "VALUES (:name, :days, :seasons)");
   query.bindValue(":name", name);
   query.bindValue(":days", days);
   query.bindValue(":seasons", seasons);
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

bool DbManager::addBooking(const int memberID, const QDate& date, const int timeSlot, const int fieldID, const int priceID)
{
   bool success = false;
   // you should check if args are ok first...
   QSqlQuery query;
   query.prepare("INSERT INTO bookings (memberid, date, timeslot, fieldid, priceid)"
                 "VALUES (:memberid, :date, :timeslot, :fieldid, :priceid) ;");
   query.bindValue(":memberid", memberID);
   query.bindValue(":date", date.toJulianDay());
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
    if ( !table_names.contains( QLatin1String("members") ))
    {
         QSqlQuery query;
         query.exec("create table IF NOT EXISTS members"
                   "(id integer primary key, "
                   "firstname varchar(100), "
                   "surname varchar(100))");
    }
    if ( !table_names.contains( QLatin1String("fields") ))
    {
     QSqlQuery query;
     query.exec("create table IF NOT EXISTS fields"
                "(id integer primary key, "
                "name varchar(25), "
                "days integer, " //set(1,2,3,4,5,6,7)
                "seasons integer)"); //set('Sommer','Winter')
    }
    if ( !table_names.contains( QLatin1String("bookings") ))
    {
     QSqlQuery query;
     query.exec("create table IF NOT EXISTS bookings"
                "(id integer primary key, "
                "memberid integer, "
                "date integer, "
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
