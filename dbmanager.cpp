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
#include <QFile>
#include <QSqlRecord>

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

bool DbManager::importMembersFile(QString filename)
{
    // you should check if args are ok first...
    QFile f(filename);
    if(f.open (QIODevice::ReadOnly))
    {
        QSqlQuery query;
        QTextStream ts (&f);

        //Travel through the csv file
        while(!ts.atEnd())
        {
            QString req = "INSERT OR REPLACE INTO members VALUES(";
            // split every lines on semikolumn
            QStringList line = ts.readLine().split(';');
            for(int i=0; i<line.length(); ++i)
            {
                req.append("\"");
                req.append(line.at(i));
                req.append("\", ");
            }
            req.chop(2); // remove the trailing comma
            req.append(");");
            if(!query.exec(req))
            {
                 qDebug() << "import member error:  "
                          << query.lastError();
            }
        }
        f.close();
    }
    else
        return false;
    return true;
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

bool DbManager::addBooking(const int memberID,
                           const QString& booking_info,
                           const QDate& date,
                           const int timeSlot,
                           const int fieldID,
                           const int priceID,
                           const int blockID)
{
   bool success = false;
   // you should check if args are ok first...
   QSqlQuery query;

   query.exec(QString("SELECT id, blockid, aboid FROM bookings "
                      "WHERE date=%1 "
                      "AND timeslot=%2 "
                      "AND fieldid=%3;").arg(date.toJulianDay()).arg(timeSlot).arg(fieldID));

   int old_block_id = -1;
   //int old_abo_id = -1;

   if(query.first()) //this field is already booked. update
   {
        int id = query.record().value(0).toInt();
        old_block_id = query.record().value(1).toInt();
        //old_abo_id = query.record().value(2).toInt();

        query.prepare("UPDATE bookings SET "
                     "info=:info, "
                     "memberid=:memberid, "
                     "priceid=:priceid, "
                     "blockid=:blockid, "
                     "aboid=NULL "
                     "WHERE id=:id;");
        query.bindValue(":info",booking_info);
        query.bindValue(":memberid", memberID);
        query.bindValue(":priceid", priceID);
        query.bindValue(":blockid", blockID);
        query.bindValue(":id", id);
   }
   else
   {       
       //insert
       query.prepare("INSERT OR REPLACE INTO bookings "
                     "( info,  memberid,  date,  timeslot,  fieldid,  priceid,  blockid)"
              "VALUES (:info, :memberid, :date, :timeslot, :fieldid, :priceid, :blockid) ;");
       query.bindValue(":info", booking_info);
       query.bindValue(":memberid", memberID);
       query.bindValue(":date", date.toJulianDay());
       query.bindValue(":timeslot", timeSlot);
       query.bindValue(":fieldid", fieldID);
       query.bindValue(":priceid", priceID);
       query.bindValue(":blockid", blockID);
   }
   if(query.exec())
   {
       success = true;
   }
   else
   {
        qDebug() << "addBooking error:  "
                 << query.lastError();
   }

   if(success && old_block_id >=0 && old_block_id != blockID)
   {
       deleteBlock(old_block_id);
   }
   return success;
}


int DbManager::addBlock(const int memberID,
                        const QString& booking_info,
                        const QDate& start_date,
                        const int priceID,
                        const int numOfBlocks)
{
    int new_record_id = -1;
    // you should check if args are ok first...
    QSqlQuery query;

    //insert

    if(numOfBlocks < 2)
        return -1;

    query.prepare("INSERT OR REPLACE INTO block_bookings (info, memberid, date, priceid, amount)"
                 "VALUES (:info, :memberid, :date, :priceid, :amount) ;");
    query.bindValue(":info", booking_info);
    query.bindValue(":memberid", memberID);
    query.bindValue(":date", start_date.toJulianDay());
    query.bindValue(":priceid", priceID);
    query.bindValue(":amount", numOfBlocks);
    if(query.exec())
    {
       new_record_id = query.lastInsertId().toInt();
    }
    else
    {
        qDebug() << "addBlock error:  "
                 << query.lastError();
    }
    return new_record_id;
}

bool DbManager::deleteBlock(const int blockId)
{
    QSqlQuery query;
    bool success = false;
    query.exec(QString("SELECT count(*) FROM bookings "
                       "WHERE blockid=%1;").arg(blockId));
    int used_blocks = -1;
    if(query.first()) //this field is already booked. update
    {
        used_blocks = query.value(0).toInt();
    }
    if(used_blocks == 0)
    {
        success = query.exec(QString("DELETE FROM block_bookings "
                           "WHERE id=%1;").arg(blockId));
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
                    "PRIMARY KEY(`id`)"
                    ");");
    }
    if ( !table_names.contains( QLatin1String("fields") ))
    {
         QSqlQuery query;
         query.exec("CREATE TABLE `fields` ("
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
                    "PRIMARY KEY(`id`)"
                    ");");
    }
    if ( !table_names.contains( QLatin1String("bookings") ))
    {
         QSqlQuery query;
         query.exec("CREATE TABLE `bookings` ("
                    "`id`	INTEGER,"
                    "`info`	TEXT,"
                    "`memberid`	INTEGER,"
                    "`date`	INTEGER,"
                    "`timeslot`	INTEGER,"
                    "`fieldid`	INTEGER,"
                    "`priceid`	INTEGER,"
                    "`blockid`	INTEGER,"
                    "`aboid`	INTEGER,"
                    "PRIMARY KEY(`id`)"
                    ");");
    }
    if ( !table_names.contains( QLatin1String("block_bookings") ))
    {
         QSqlQuery query;
         query.exec("CREATE TABLE `block_bookings` ("
                    "`id`	INTEGER,"
                    "`info`	TEXT,"
                    "`memberid`	INTEGER,"
                    "`date`	INTEGER,"
                    "`priceid`	INTEGER,"
                    "`amount`	INTEGER,"
                    "FOREIGN KEY(`priceid`) REFERENCES `prices`(`id`),"
                    "PRIMARY KEY(`id`),"
                    "FOREIGN KEY(`memberid`) REFERENCES `members`(`id`)"
                    ");");
    }
    if ( !table_names.contains( QLatin1String("cash_register") ))
    {
         QSqlQuery query;
         query.exec("CREATE TABLE `cash_register` ("
                    "`id`	INTEGER,"
                    "`date`	INTEGER NOT NULL,"
                    "`operation`	INTEGER NOT NULL,"
                    "`sum`	REAL,"
                    "PRIMARY KEY(`id`)"
                    ");");
    }
    if ( !table_names.contains( QLatin1String("prices") ))
    {
         QSqlQuery query;
         query.exec("CREATE TABLE `prices` ("
                    "`id`	INTEGER,"
                    "`price_name`	TEXT,"
                    "`member`	TEXT,"
                    "`winter`	TEXT,"
                    "`abo`	TEXT,"
                    "`guest`	TEXT,"
                    "`days`	INTEGER,"
                    "`start_time_slot`	INTEGER,"
                    "`end_time_slot`	INTEGER,"
                    "`sum`	REAL,"
                    "`account`	INTEGER,"
                    "PRIMARY KEY(`id`)"
                    ");");
    }
    if(!table_names.contains( QLatin1String("accounts") ))
    {
        QSqlQuery query;
        query.exec("CREATE TABLE `accounts` ("
                   "`number`	INTEGER NOT NULL UNIQUE,"
                   "`account_name`	TEXT,"
                   "PRIMARY KEY(`number`)"
                   ");");
    }
    if ( !table_names.contains( QLatin1String("settings") ))
    {
         QSqlQuery query;
         query.exec("CREATE TABLE `settings` ("
                    "`id`	TEXT NOT NULL UNIQUE,"
                    "`name`	TEXT NOT NULL UNIQUE,"
                    "`val`	TEXT,"
                    "`type`	INTEGER,"
                    "PRIMARY KEY(`id`)"
                    ");");
    }

    return true;
}

