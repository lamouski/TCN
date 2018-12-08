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

#include "bookingdata.h"

#include <QObject>
#include <QDate>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QWidget>

class DayBookingTableModel;
class BookingDialog;

class QMenu;

namespace Ui {
class WeekViewWidget;
}

class WeekViewWidget : public QWidget
{
    Q_OBJECT

    enum ProcessingFlag
    {
        NEW_BOOKING = 0,
        CURRENT_BOOKING,
        ALL_BOOKINGS
    };

public:
    explicit WeekViewWidget(QWidget *parent = 0);
    ~WeekViewWidget();

    inline QDate firstDayOfCurrentWeek() const;
    inline QDate lastDayOfCurrentWeek() const;

    QPushButton *getReturnButton() const;

    public slots:
    //
    void setCurrentDate(QDate date);

protected:
    void showEvent(QShowEvent *);

    bool singleBooking(const BookingSlot& slot, const BookingData& data);
    bool blockBooking(const BookingSlot& slot, const BookingData& data);
    bool multiBooking(const BookingSlot& slot, const BookingData& data);

protected slots:
    void updateGUI();

    void processBooking(int day, const QModelIndex &index, ProcessingFlag flag = NEW_BOOKING);
    void cancleBooking(int day, const QModelIndex &index, ProcessingFlag flag);
    void processBookingContextMenu(int day, const QModelIndex &index, const QPoint &pos);

    void fillCurrentWeek();

private slots:
     void on_m_button_previous_week_clicked();
     void on_m_button_next_week_clicked();

private:
    Ui::WeekViewWidget *ui;

    QVector<QLabel*> m_day_labels;
    QVector<QTableView*> m_booking_tables;

    QVector<DayBookingTableModel*> m_day_booking_models;

    BookingDialog* m_booking_dialog;
    void set_signal_slots_connections();

    QMenu* m_contextMenu = nullptr;
    QAction* m_action_change_abo_cur = nullptr;
    QAction* m_action_change_abo_all = nullptr;
    QAction* m_action_cancle_abo_cur = nullptr;
    QAction* m_action_cancle_abo_all = nullptr;
    QAction* m_action_change_booking = nullptr;
    QAction* m_action_cancle_booking = nullptr;
    int m_selected_day = -1;
    QModelIndex m_selected_index = QModelIndex();

};

#endif // WEEKVIEWWIDGET_H
