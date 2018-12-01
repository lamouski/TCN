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

#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class BookingDialog;
}

class BookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDialog(QWidget *parent = 0);
    ~BookingDialog();

    void reset();

public slots:
    void setField(const QString& fieldName);
    void setTimeslot(int timeSlot);
    void setDay(const QDate& date);

    void setMemberId(int id);
    void setPriceId(int id);
    void setInfo(const QString& info);

    void updateMembersQuery(const QString &find_string);
    void updateBlocksQuery(const QString &find_string);
    void updatePriceQuery();

public:
    int selectedId() const;    
    QString info() const;

    bool isSingleBooking() const;
    int selectedPrice() const;
    int selectedBlock() const;

    bool isBlockBooking() const;
    int numOfBlocks() const;

    bool isMultyBooking();
    QDate aboStartDate() const;
    QDate aboEndDate() const;

protected slots:
    void on_m_line_edit_name_textEdited(const QString &arg1);
    //void on_m_list_view_members_doubleClicked(const QModelIndex &index);
    void on_m_list_view_members_clicked(const QModelIndex &index);
    void on_m_list_view_members_activated(const QModelIndex &index);
    void on_m_combo_price_currentIndexChanged(int index);

    void handleCurrentBlockChanged(const QModelIndex &current, const QModelIndex &previous);
    void handleCurrentMemberChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_m_list_view_blocks_activated(const QModelIndex &index);
    void on_m_list_view_blocks_clicked(const QModelIndex &index);

private:
    Ui::BookingDialog *ui;

    QSqlQueryModel *m_memberlist_model;
    QString m_memberlist_base_query_string;

    QSqlQueryModel *m_blockslist_model;
    QString m_blockslist_base_query_string;

    QSqlQueryModel *m_prices_model;
    QString m_prices_base_query_string;

    int m_timeslot;
    int m_last_selected_member_id = -1;
    int m_last_selected_block_id = -1;
    int m_last_selected_price_id = -1;
    int m_days_mask = 255;

    enum BookingMode
    {
        MODE_SINGLE,
        MODE_BLOCK,
        MODE_ABO
    };

    BookingMode m_mode = MODE_SINGLE;
    void setMode(BookingMode mode);

    void selectCurrentMemberId(const QModelIndex &index);
    void selectCurrentBlockId(const QModelIndex &index);

};

#endif // BOOKINGDIALOG_H
