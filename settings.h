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
        return m_instance->m_sommer_begin.addYears(1).addDays(-1);
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
