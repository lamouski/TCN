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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDate>
#include <QMap>
#include <QString>
#include <QVariant>

class Settings
{
public:
    Settings();

    static inline const QDate &currentDate()
    {
        return m_instance->m_current_date;
    }

    static inline int getInt(const QString & setting_key)
    {
        return m_instance->m_settings_map[setting_key].toInt();
    }

    static inline QString getString(const QString & setting_key)
    {
        return m_instance->m_settings_map[setting_key].toString();
    }

    static inline bool getBool(const QString & setting_key)
    {
        return m_instance->m_settings_map[setting_key].toBool();
    }

    static inline QDate getDate(const QString & setting_key)
    {
        return m_instance->m_settings_map[setting_key].value<QDate>();
    }

    static inline bool winterSeason()
    {
        return m_instance->m_winter_season;
    }

    static inline QDate winterSeasonBegin()
    {
        return m_instance->m_winter_begin;
    }

    static inline QDate winterSeasonEnd()
    {
        return m_instance->m_winter_end;
    }

    static inline QDate sommerSeasonBegin()
    {
        return m_instance->m_sommer_begin;
    }

    static inline QDate sommerSeasonEnd()
    {
        return m_instance->m_sommer_end;
    }

    void setCurrentDate(const QDate& date);

    static Settings* instance();

public:
    void readDataFromDb();

protected:
    void settingsPreprocessing();
private:
    void setSeason();

private:
    static Settings* m_instance;

    QMap<QString, QVariant> m_settings_map;

    QDate m_current_date;
    bool m_winter_season;

    QDate m_winter_begin, m_winter_end;
    QDate m_sommer_begin, m_sommer_end;

};

#endif // SETTINGS_H
