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

#ifndef WEEKVIEWWIDGET_H
#define WEEKVIEWWIDGET_H

#include <QDate>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QWidget>

class DayBookingTableModel;

namespace Ui {
class WeekViewWidget;
}

class WeekViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeekViewWidget(QWidget *parent = 0);
    ~WeekViewWidget();

    inline QDate firstDayOfCurrientWeek() const;
    inline QDate lastDayOfCurrientWeek() const;

    inline QDate currientDate() const;

    QPushButton *getReturnButton() const;

public slots:
    void setCurrientDate(QDate date);
    void fillCurrientWeek();

protected slots:
    void updateGUI();

private:
    Ui::WeekViewWidget *ui;

    QVector<QLabel*> m_day_labels;
    QVector<QTableView*> m_booking_tables;

    DayBookingTableModel* m_day_booking_models[7];
    //currient data
    QDate m_date;
};


#endif // WEEKVIEWWIDGET_H
