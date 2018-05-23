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
    bool addField(const QString& name, const int days, const int seasons);
    bool addBooking(const int memberID, const QDate &date, const int tymeslot, const int fieldid, const int priceid);

    bool checkDB();

    static DbManager* instance();    
    static QSqlDatabase db();

private:
    static DbManager* m_instance;
    QSqlDatabase m_db;

};

#endif // DBMANAGER_H