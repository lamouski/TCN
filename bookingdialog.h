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

#include "bookingdata.h"

namespace Ui {
class BookingDialog;
}

class BlockBookingsModel;

class BookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDialog(QWidget *parent = nullptr);
    ~BookingDialog();

    void reset();

public slots:
    void setField(const QString& fieldName);
    void setTimeslot(int timeSlot);
    void setDay(const QDate& date);

    void setData(const BookingData &data);

    void updateMembersQuery(const QString &find_string);
    void updateNonMembersQuery(const QString &find_string);
    void updateBlocksQuery(const QString &find_string);
    void updatePriceQuery();

public:
    BookingData getSelectedData() const;

    bool isSingleBooking() const;
    bool isBlockBooking() const;

    bool isMultyBooking();

protected slots:
    void handleCurrentMemberChanged(const QModelIndex &current, const QModelIndex &previous);
    void handleCurrentNonMemberChanged(const QModelIndex &current, const QModelIndex &previous);
    void handleCurrentBlockChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_m_line_edit_name_textEdited(const QString &arg1);

    void on_m_list_view_members_clicked(const QModelIndex &index);
    void on_m_list_view_members_activated(const QModelIndex &index);
    void on_m_list_view_non_members_clicked(const QModelIndex &index);
    void on_m_list_view_non_members_activated(const QModelIndex &index);
    void on_m_list_view_blocks_clicked(const QModelIndex &index);
    void on_m_list_view_blocks_activated(const QModelIndex &index);
    void on_m_combo_price_currentIndexChanged(int index);

    void updateGUI();

private slots:
    void on_m_add_new_nonmember_toggled(bool checked);

private:
    Ui::BookingDialog *ui;

    QSqlQueryModel *m_memberlist_model;
    QString m_memberlist_base_query_string;

    QSqlQueryModel *m_nonmemberlist_model;
    QString m_nonmemberlist_base_query_string;

    BlockBookingsModel *m_blockslist_model;

    QSqlQueryModel *m_prices_model;
    QString m_prices_base_query_string;

    int m_timeslot;

    int m_selected_member_id = -1;
    int m_selected_block_id = -1;
    int m_selected_price_id = -1;

    int m_num_of_used_bloks = 0;

    int m_days_mask = 255;

    enum BookingMode
    {
        MODE_SINGLE,
        MODE_BLOCK,
        MODE_ABO
    };

    BookingMode m_mode = MODE_SINGLE;
    void setMode(BookingMode mode);

    void selectCurrentMemberId(bool auto_selection_of_single_member);
    void selectCurrentNonMember(const QString& info);
    void selectCurrentBlockId();

};

#endif // BOOKINGDIALOG_H
