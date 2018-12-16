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

class Settings
{
public:
    Settings();

    static inline const QDate &currentDate()
    {
        return m_instance->m_current_date;
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
        QDate end =  m_instance->m_sommer_begin.addYears(1).addDays(-1);
        if(m_instance->m_season_starts_from_monday)
        {
            int day_of_the_week = end.dayOfWeek();
            if(day_of_the_week < 3)
              end = end.addDays( Qt::Monday - day_of_the_week );
            else
              end = end.addDays( Qt::Sunday - day_of_the_week + 1);
        }
        return end;
    }

    static inline QDate sommerSeasonBegin()
    {
        return m_instance->m_sommer_begin;
    }

    static inline QDate sommerSeasonEnd()
    {
        return m_instance->m_winter_begin.addDays(-1);
    }

    static inline bool canclePreviousBookingBeforeUpdate()
    {
        return m_instance->m_cancel_previous_booking_before_update;
    }

    static inline QString weekReportRevenuesFilename()
    {
        return m_instance->m_week_report_revenues_filename;
    }

    static inline QString weekReportCostsFilename()
    {
        return m_instance->m_week_report_costs_filename;
    }

    static inline bool exportBookingTableHtml()
    {
        return m_instance->m_export_booking_table_html;
    }

    static inline QString bookingTableHtmlPath()
    {
        return m_instance->m_booking_table_html_path;
    }

    void setCurrentDate(const QDate& date);

    static Settings* instance();

public:
    void readDataFromDb();

private:
    void setSeason();

private:
    static Settings* m_instance;

    QDate m_current_date;
    bool m_winter_season;

    QDate m_winter_begin;
    QDate m_sommer_begin;
    bool m_season_starts_from_monday;
    bool m_cancel_previous_booking_before_update;

    QString m_week_report_revenues_filename;
    QString m_week_report_costs_filename;

    bool m_export_booking_table_html;
    QString m_booking_table_html_path;

};

#endif // SETTINGS_H
