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

#include "settings.h"
#include "settings.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

Settings* Settings::m_instance = nullptr;

Settings::Settings()
{
    m_current_date = QDate::currentDate();
    readDataFromDb();
    if(m_instance)
        delete m_instance;
    m_instance = this;
}

Settings* Settings::instance()
{
    return m_instance;
}

void Settings::readDataFromDb()
{
    QSqlQuery query("SELECT id, val, type FROM settings");
    if(query.exec())
    {
        while(query.next())
        {
            QString id = query.record().value(0).toString();
            QVariant val = query.record().value(1);
            int type = query.record().value(2).toInt();

            QVariant vr_value;

            switch(type)
            {
            case 0: //int
                vr_value.setValue(val.toInt());
                break;
            case 1: //bool
                if(val.toString() == "true")
                    vr_value.setValue(true);
                else
                    vr_value.setValue(false);
                break;
            case 2: default:
                vr_value.setValue(val.toString());
                break;
            case 3: //date
            {
                QStringList values = val.toString().split(".");
                if(values.size() >= 2)
                {
                    int day = values[0].toInt();
                    int month = values[1].toInt();
                    int year = values.size() > 2 ? values[2].toInt() : m_current_date.year();
                    vr_value.setValue(QDate(year, month, day));
                }
            }
                break;
            }
            m_settings_map[id] = vr_value;
        }
    }

    settingsPreprocessing();
}

void Settings::settingsPreprocessing()
{
    m_sommer_begin = m_settings_map["sommer_start"].toDate();
    m_winter_begin = m_settings_map["winter_start"].toDate();

    m_sommer_end = m_winter_begin.addDays(-1);
    m_winter_end = m_sommer_begin.addDays(-1);

    if(m_settings_map["season_starts_from_monday"].toBool())
    {
        int day_of_the_week = m_winter_begin.dayOfWeek();
        if(day_of_the_week < 4)
          m_winter_begin = m_winter_begin.addDays( Qt::Monday - day_of_the_week );
        else
          m_winter_begin = m_winter_begin.addDays( Qt::Sunday - day_of_the_week + 1);

        day_of_the_week = m_winter_end.dayOfWeek();
        if(day_of_the_week < 3)
          m_winter_end = m_winter_end.addDays( Qt::Monday - day_of_the_week - 1);
        else
          m_winter_end = m_winter_end.addDays( Qt::Sunday - day_of_the_week);

        day_of_the_week = m_sommer_begin.dayOfWeek();
        if(day_of_the_week < 4)
          m_sommer_begin = m_sommer_begin.addDays( Qt::Monday - day_of_the_week );
        else
          m_sommer_begin = m_sommer_begin.addDays( Qt::Sunday - day_of_the_week + 1);

        day_of_the_week = m_sommer_end.dayOfWeek();
        if(day_of_the_week < 3)
          m_sommer_end = m_sommer_end.addDays( Qt::Monday - day_of_the_week - 1);
        else
          m_sommer_end = m_sommer_end.addDays( Qt::Sunday - day_of_the_week);
     }

    setSeason();
}

void Settings::setCurrentDate(const QDate& date)
{
    m_current_date = date;
    if(m_current_date.year() != m_sommer_begin.year())
        readDataFromDb();
    else
        setSeason();
}

void Settings::setSeason()
{
    if(m_current_date < m_sommer_begin || m_current_date >= m_winter_begin)
        m_winter_season = true;
    else
        m_winter_season = false;
}
