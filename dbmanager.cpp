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

bool DbManager::addField(const QString& name, const int days[7], const int seasons) const
{
   bool success = false;
   // you should check if args are ok first...
   QSqlQuery query;

   query.prepare("SELECT id FROM fields "
                 "WHERE name=:name;");
   query.bindValue(":name", name);
   if(query.exec())
   {
       if(query.first()) //at least one field with this name exists
                         //existing record must be updated
       {
           query.prepare("UPDATE fields SET 'day0'=:day0, "
                         "'day1'=:day1, "
                         "'day2'=:day2, "
                         "'day3'=:day3, "
                         "'day4'=:day4, "
                         "'day5'=:day5, "
                         "'day6'=:day6, "
                         "'seasons'=:seasons"
                         "WHERE id=:id;");
       }
       else //the field must be inserted
       {
           query.prepare("INSERT INTO fields (name, day0, day1, day2, day3, day4, day5, day6, seasons) "
                         "VALUES (:name, :day0, :day1, :day2, :day3, :day4, :day5, :day6, :seasons)");
       }
   }
   else
   {
        qDebug() << "addField error: check existence of the filed in the database "
                 << query.lastError();
        return false;
   }

   query.bindValue(":name", name);
   query.bindValue(":day0", days[0]);
   query.bindValue(":day1", days[1]);
   query.bindValue(":day2", days[2]);
   query.bindValue(":day3", days[3]);
   query.bindValue(":day4", days[4]);
   query.bindValue(":day5", days[5]);
   query.bindValue(":day6", days[6]);
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

bool DbManager::updateFieldName(const int id, const QString &name) const
{
    bool success = false;
    // you should check if args are ok first...
    QSqlQuery query;
    query.prepare("UPDATE fields SET 'name'=:name "
                  "WHERE id=:id;");

    query.bindValue(":id", id);
    query.bindValue(":name", name);

    if(query.exec())
    {
        success = true;
    }
    else
    {
         qDebug() << "update field name error:  "
                  << query.lastError();
    }
    return success;
}

bool DbManager::updateFieldTimeMask(const int id, const int day_index, const int time_mask) const
{
    bool success = false;
    // you should check if args are ok first...
    QSqlQuery query;
    query.prepare(QString("UPDATE fields SET 'day%0'=:time_mask "
                  "WHERE id=:id;").arg(day_index));

    query.bindValue(":id", id);
    query.bindValue(":time_mask", time_mask);

    if(query.exec())
    {
        success = true;
    }
    else
    {
         qDebug() << "update field time mask error:  "
                  << query.lastError();
    }
    return success;
}

bool DbManager::updateFieldSeasons(const int id, const int seasons) const
{
    bool success = false;
    // you should check if args are ok first...
    QSqlQuery query;
    query.prepare("UPDATE fields SET 'seasons'=:seasons "
                  "WHERE id=:id;");

    query.bindValue(":id", id);
    query.bindValue(":seasons", seasons);

    if(query.exec())
    {
        success = true;
    }
    else
    {
         qDebug() << "update field seasons error:  "
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
         query.exec("CREATE TABLE `members` ("
                    "`id`	INTEGER,"
                    "`firstname`	TEXT,"
                    "`surname`	TEXT,"
                    "`phone1`	TEXT,"
                    "`phone2`	TEXT,"
                    "`employment`	TEXT,"
                    "`info1`	TEXT,"
                    "`info2`	TEXT,"
                    "`info3`	TEXT,"
                    "`membernumber`	TEXT,"
                    "`debitentry`	INTEGER,"
                    "`blockbooking`	INTEGER,"
                    "`blockbookingnum`	TEXT,"
                    "PRIMARY KEY(`id`)"
                ");");
    }
    if ( !table_names.contains( QLatin1String("fields") ))
    {
     QSqlQuery query;
     query.exec("CREATE TABLE `fields` ( "
                "`id`	INTEGER,"
                "`name`	TEXT,"
                "`day0`	INTEGER,"
                "`day1`	INTEGER,"
                "`day2`	INTEGER,"
                "`day3`	INTEGER,"
                "`day4`	INTEGER,"
                "`day5`	INTEGER,"
                "`day6`	INTEGER,"
                "`seasons`	INTEGER NOT NULL,"
                "PRIMARY KEY(`id`));");
    }
    if ( !table_names.contains( QLatin1String("bookings") ))
    {
     QSqlQuery query;
     query.exec("CREATE TABLE `bookings` ("
                "`id`	INTEGER,"
                "`memberid`	INTEGER,"
                "`date`	INTEGER,"
                "`timeslot`	INTEGER,"
                "`fieldid`	INTEGER,"
                "`priceid`	INTEGER,"
                "FOREIGN KEY(`memberid`) REFERENCES `members`(`id`),"
                "FOREIGN KEY(`fieldid`) REFERENCES `fields`(`id`),"
                "PRIMARY KEY(`id`));");
    }
    if ( !table_names.contains( QLatin1String("prices") )) {
     QSqlQuery query;
     query.exec("CREATE TABLE `prices` ("
                "`id`	INTEGER,"
                "`price`	REAL,"
                "`start_time_slot`	INTEGER,"
                "`end_time_slot`	INTEGER,"
                "`flags`	INTEGER UNIQUE,"  //bit 1 - gast oder member, bit 2 - abo, bit 3 - winter oder sommer
                "PRIMARY KEY(`id`));");
    }

    if(!table_names.contains( QLatin1String("accounts") )) {
        QSqlQuery query;
        if(!query.exec("CREATE TABLE `accounts` ("
            "`number`	INTEGER NOT NULL UNIQUE,"
            "`name`	TEXT,"
            "PRIMARY KEY(`number`)"
        ");"))
            qDebug() << "create table 'accounts' error:  "
                     << query.lastError();;
    }

    if ( !table_names.contains( QLatin1String("settings") )) {
     QSqlQuery query;
     query.exec("CREATE TABLE `settings` ("
                "`name`	TEXT NOT NULL UNIQUE,"
                "`val`	NUMERIC );");
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
