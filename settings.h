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
    };

    static inline bool winterSeason()
    {
        return m_instance->m_winter_season;
    };

    static inline QDate winterSeasonBegin()
    {
        return m_instance->m_winter_begin;
    };

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
    };

    static inline QDate sommerSeasonBegin()
    {
        return m_instance->m_sommer_begin;
    };

    static inline QDate sommerSeasonEnd()
    {
        return m_instance->m_winter_begin.addDays(-1);
    };

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

};

#endif // SETTINGS_H
