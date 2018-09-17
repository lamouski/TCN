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
            QString val = query.record().value(1).toString();
            if(id =="winter_start")
            {
                QStringList values = val.split(".");
                if(values.size()>=2)
                {
                    int day = values[0].toInt();
                    int month = values[1].toInt();
                    m_winter_begin = QDate(m_current_date.year(), month, day);
                }
            }
            else if(id == "sommer_start")
            {
                QStringList values = val.split(".");
                if(values.size()>=2)
                {
                    int day = values[0].toInt();
                    int month = values[1].toInt();
                    m_sommer_begin = QDate(m_current_date.year(), month, day);
                }
            }
            else if (id == "sesason_starts_from_monday")
            {
                if(val == "true")
                    m_season_starts_from_monday = true;
                else
                    m_season_starts_from_monday = false;
            }
        }
    }

    if(m_season_starts_from_monday)
    {
        int day_of_the_week = m_winter_begin.dayOfWeek();
        if(day_of_the_week < 3)
          m_winter_begin = m_winter_begin.addDays( Qt::Monday - day_of_the_week );
        else
          m_winter_begin = m_winter_begin.addDays( Qt::Sunday - day_of_the_week + 1);

        day_of_the_week = m_sommer_begin.dayOfWeek();
        if(day_of_the_week < 3)
          m_sommer_begin = m_sommer_begin.addDays( Qt::Monday - day_of_the_week );
        else
          m_sommer_begin = m_sommer_begin.addDays( Qt::Sunday - day_of_the_week + 1);
    }

    setSeason();
}

void Settings::setCurrentDate(const QDate& date)
{
    m_current_date = date;
    setSeason();
}

void Settings::setSeason()
{
    if(m_current_date < m_sommer_begin || m_current_date >= m_winter_begin)
        m_winter_season = true;
    else
        m_winter_season = false;
}
