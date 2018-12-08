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

#include "bookingdialog.h"
#include "settings.h"
#include "ui_bookingdialog.h"

#include <QDate>
#include <QMenu>
#include <QSqlQuery>
#include <QSqlRecord>

BookingDialog::BookingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookingDialog)
{
    ui->setupUi(this);

    m_memberlist_model = new QSqlQueryModel(this);
    m_memberlist_base_query_string = QString("SELECT (firstname || ' ' || surname) AS name, id FROM members ");
    m_memberlist_model->setQuery(m_memberlist_base_query_string);
    ui->m_list_view_members->setModel(m_memberlist_model);
    ui->m_list_view_members->setModelColumn(0);

    connect(ui->m_list_view_members->selectionModel(),
          &QItemSelectionModel::currentChanged, this, &BookingDialog::handleCurrentMemberChanged);

    m_blockslist_model = new QSqlQueryModel(this);
    m_blockslist_base_query_string = QString("SELECT (firstname || ' ' || surname) AS name,"
    "( firstname || ' ' || surname || ' - " +tr("Block")+ "' || amount || ' - " + tr("Used") + " ' || (SELECT count(id) FROM bookings WHERE bookings.blockid = block_bookings.id )) AS info_string, "
    "block_bookings.id, block_bookings.priceid, block_bookings.memberid, "
    "(SELECT count(id) FROM bookings WHERE bookings.blockid = block_bookings.id ) AS used_amount "
    "FROM block_bookings "
    "LEFT OUTER JOIN members ON block_bookings.memberid = members.id "
    "WHERE amount > used_amount ");
    m_blockslist_model->setQuery(m_blockslist_base_query_string);
    ui->m_list_view_blocks->setModel(m_blockslist_model);
    ui->m_list_view_blocks->setModelColumn(1);

    connect(ui->m_list_view_blocks->selectionModel(),
          &QItemSelectionModel::currentChanged, this, &BookingDialog::handleCurrentBlockChanged);

    m_prices_model = new QSqlQueryModel(this);
    m_prices_base_query_string = QString("SELECT (price_name || ' - ' || sum) "
                                         " AS info, sum, id, abo, member FROM prices");
    m_prices_model->setQuery(m_prices_base_query_string);
    ui->m_combo_price->setModel(m_prices_model);
    ui->m_combo_price->setModelColumn(0);

    QMenu* mode_menu = new QMenu(this);
    mode_menu->addAction(QString(tr("Single booking")), [this](){ setMode(MODE_SINGLE);});
    mode_menu->addAction(QString(tr("Block booking")), [this](){ setMode(MODE_BLOCK);});
    mode_menu->addAction(QString(tr("Abo booking")), [this](){ setMode(MODE_ABO);});
    ui->m_mode_tool_button->setMenu(mode_menu);

    ui->m_combo_price->setValidator(new QDoubleValidator(0, 10000, 2, this));

    Settings* settings = Settings::instance();
    if(settings->winterSeason())
    {
        ui->m_start_abo_date->setDateRange(settings->winterSeasonBegin(),settings->winterSeasonEnd());
        ui->m_start_abo_date->setDate(settings->winterSeasonBegin());

        ui->m_end_abo_date->setDateRange(settings->winterSeasonBegin(), settings->winterSeasonEnd());
        ui->m_end_abo_date->setDate(settings->winterSeasonEnd());
    }
    else
    {
        ui->m_start_abo_date->setDateRange(settings->sommerSeasonBegin(), settings->sommerSeasonEnd());
        ui->m_start_abo_date->setDate(settings->sommerSeasonBegin());

        ui->m_end_abo_date->setDateRange(settings->sommerSeasonBegin(), settings->sommerSeasonEnd());
        ui->m_end_abo_date->setDate(settings->sommerSeasonEnd());
    }
}


BookingDialog::~BookingDialog()
{
    delete ui;
}


void BookingDialog::reset()
{
    ui->m_line_edit_name->clear();
    updateMembersQuery(ui->m_line_edit_name->text());
    updateBlocksQuery(ui->m_line_edit_name->text());
    setMode(MODE_SINGLE);
    ui->m_line_edit_name->setFocus();
}


void BookingDialog::setField(const QString& fieldName)
{
    ui->m_label_field->setText(fieldName);
}


void BookingDialog::setTimeslot(int timeSlot)
{
    m_timeslot = timeSlot;
    ui->m_label_time->setText(QTime(m_timeslot, 0).toString("HH:mm") + " - " + QTime(m_timeslot+1, 0).toString("HH:mm"));
    updatePriceQuery();
}


void BookingDialog::setDay(const QDate& date)
{
    m_days_mask = 1 << (date.dayOfWeek() -1);
    ui->m_label_day->setText(date.toString("dddd"));
}


void BookingDialog::setMemberId(int id)
{
    QSqlQuery query(m_memberlist_base_query_string + QString(" WHERE id = %0").arg(id));
    QString name;
    if(query.exec())
        if(query.next())
            name = query.record().value(0).toString();
    m_last_selected_member_id = id;
    ui->m_line_edit_name->setText(name);
    updateMembersQuery(ui->m_line_edit_name->text());
    updateBlocksQuery(ui->m_line_edit_name->text());

    setMode(MODE_SINGLE);
}


void BookingDialog::setInfo(const QString& info)
{
    m_last_selected_member_id = -1;
    ui->m_line_edit_name->setText(info);
    updateMembersQuery(ui->m_line_edit_name->text());
    updateBlocksQuery(ui->m_line_edit_name->text());

    setMode(MODE_SINGLE);
}


void BookingDialog::setPriceId(int id)
{
    QSqlQuery query(m_prices_base_query_string + QString(" WHERE id = %0").arg(id));
    QString name;
    if(query.exec())
        if(query.next())
            name = query.record().value(0).toString();
    int index_in_new_list = ui->m_combo_price->findText(name);
    if(index_in_new_list >= 0)
        ui->m_combo_price->setCurrentIndex(index_in_new_list);
    else
        if(ui->m_combo_price->count() > 0)
            ui->m_combo_price->setCurrentIndex(0);
}


BookingData BookingDialog::getSelectedData() const
{
    BookingData data;

    QModelIndex member_index = ui->m_list_view_members->currentIndex();
    QModelIndex block_index = ui->m_list_view_blocks->currentIndex();
    if(member_index.isValid())
    {
        data.memberID = m_memberlist_model->record(member_index.row()).value(1).toInt();
    }
    if(block_index.isValid())
    {
        data.blockID = m_blockslist_model->record(block_index.row()).value(2).toInt();
        data.priceID = m_blockslist_model->record(block_index.row()).value(3).toInt();
        data.memberID = m_blockslist_model->record(block_index.row()).value(4).toInt();
    }
    if(data.memberID == -1)
        data.booking_info = ui->m_line_edit_name->text();

    QModelIndex price_index = m_prices_model->index(ui->m_combo_price->currentIndex(), 2);
    if(price_index.isValid())
    {
        data.priceID = m_prices_model->data(price_index).toInt(); // third column
    }

    data.sum = ui->m_summ_line_edit->text().toFloat();

    if(m_mode == MODE_BLOCK)
        data.numOfBlocks = ui->m_num_of_blocks_spin_box->value();

    if(m_mode == MODE_ABO)
    {
        data.aboStart = ui->m_start_abo_date->date();
        data.aboEnd = ui->m_end_abo_date->date();
    }
    return data;
}

//int BookingDialog::selectedId() const
//{
//    return m_last_selected_member_id;
//}


bool BookingDialog::isSingleBooking() const
{
    return m_mode == MODE_SINGLE;
}


//int BookingDialog::selectedPrice() const
//{
//    return m_last_selected_price_id;
//}


//int BookingDialog::selectedBlock() const
//{
//    return m_last_selected_block_id;
//}

//float BookingDialog::sum() const
//{
//    return ui->m_summ_line_edit->text().toFloat();
//}


//QString BookingDialog::info() const
//{
//    return ui->m_line_edit_name->text();
//}


bool BookingDialog::isBlockBooking() const
{
    return m_mode == MODE_BLOCK;
}


//int BookingDialog::numOfBlocks() const
//{
//    if(m_mode == MODE_BLOCK)
//        return ui->m_num_of_blocks_spin_box->value();
//    else
//        return -1;
//}


bool BookingDialog::isMultyBooking()
{
    return m_mode == MODE_ABO;
}


QDate BookingDialog::aboStartDate() const
{
    if(m_mode == MODE_ABO)
        return ui->m_start_abo_date->date();
    else
        return QDate();
}


QDate BookingDialog::aboEndDate() const
{if(m_mode == MODE_ABO)
        return ui->m_start_abo_date->date();
    else
        return QDate();
    if(m_mode == MODE_ABO)
        return ui->m_end_abo_date->date();
    else
        return QDate();
}


void BookingDialog::handleCurrentMemberChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    if(current.isValid())
    {
        ui->m_line_edit_name->setText(m_memberlist_model->record(current.row()).value(0).toString());
        selectCurrentMemberId(current);
        ui->m_list_view_blocks->clearSelection();
        updatePriceQuery();
    }
}


void BookingDialog::handleCurrentBlockChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    if(current.isValid())
    {
        ui->m_line_edit_name->setText(m_blockslist_model->record(current.row()).value(0).toString());
        selectCurrentBlockId(current);
        ui->m_list_view_members->clearSelection();
        updatePriceQuery();
    }
}


void BookingDialog::on_m_combo_price_currentIndexChanged(int row)
{
    if(row >= 0)
    {
        int new_selected_price_id = m_prices_model->data(m_prices_model->index(row, 2)).toInt(); // third column
        if(new_selected_price_id != m_last_selected_price_id)
        {
            m_last_selected_price_id = new_selected_price_id;
            ui->m_summ_line_edit->setText(m_prices_model->data(m_prices_model->index(row, 1)).toString());
        }
    }
}


void BookingDialog::setMode(BookingDialog::BookingMode mode)
{
    m_mode = mode;
    ui->m_block_date_widget->setVisible(m_mode == MODE_BLOCK);
    ui->m_abo_date_widget->setVisible(m_mode == MODE_ABO);

    updatePriceQuery();
}


QString member_name_condition_for_query(const QString &find_string)
{
    QStringList key_words = find_string.split(" ", QString::SkipEmptyParts);
    if(key_words.empty())
        return QString();
    else
    {
        QString condition_string;
        QString and_string(" ");
        foreach (QString key_word, key_words)
        {
            condition_string += and_string + "name LIKE '%"+key_word+"%'";
            and_string = QString(" AND ");
        }
        return condition_string;
    }
}


void BookingDialog::updateMembersQuery(const QString &find_string)
{
    QString condition = member_name_condition_for_query(find_string);
    m_memberlist_model->setQuery(m_memberlist_base_query_string +
                                 (condition.isEmpty() ? "" : " WHERE " + condition));
    if(m_memberlist_model->rowCount() == 1) //one with entered name
    {
       m_last_selected_member_id = m_memberlist_model->record(0).value(1).toInt();
    }
    else if(m_memberlist_model->rowCount() == 0) //no member with entered name
    {
       m_last_selected_member_id = -1;

    }
    else
    {
        if(m_last_selected_member_id >= 0)
        {
            //find the member with given id in the list
        }
    }
    updatePriceQuery();
}


void BookingDialog::updateBlocksQuery(const QString &find_string)
{
    QString condition = member_name_condition_for_query(find_string);
    m_blockslist_model->setQuery(m_blockslist_base_query_string +
                                 (condition.isEmpty() ? "" : " AND " + condition));
    if(m_blockslist_model->rowCount() == 0) //one with entered name
        ui->m_list_view_blocks->hide();
    else
        ui->m_list_view_blocks->show();
}


void BookingDialog::updatePriceQuery()
{
    bool not_member = (m_memberlist_model->rowCount() == 0);

    QString current_price = ui->m_combo_price->currentText();
    int curr_index = ui->m_combo_price->currentIndex();

    QString curr_member_string =  m_prices_model->data(m_prices_model->index(curr_index, 4)).toString(); // fith column
    bool curr_member = curr_member_string == "true" ? true : false;

    QString query_string = m_prices_base_query_string;
    query_string += " WHERE start_time_slot <= " + QString("%1").arg(m_timeslot) +
                    " AND end_time_slot > " + QString("%1").arg(m_timeslot);
    query_string += QString(" AND winter = ") +
            (Settings::winterSeason() ? QString("'true'") : QString("'false'"));
    query_string += QString(" AND abo = ") +
            (m_mode == MODE_ABO ? QString("'true'") : QString("'false'"));
    if(!not_member)
        query_string += QString(" AND member = 'true'");

    query_string += QString(" AND (`days` & %1) = %1").arg(m_days_mask);
    query_string += QString(" ORDER BY abo ");
    if(not_member)
        query_string += QString(" , member ASC ");
    m_prices_model->setQuery(query_string);

    int index_in_new_list = ui->m_combo_price->findText(current_price);
     if(index_in_new_list >= 0 && curr_member != not_member)
    {
        ui->m_combo_price->setCurrentIndex(index_in_new_list);
    }
    else
        if(ui->m_combo_price->count() > 0)
            ui->m_combo_price->setCurrentIndex(0);

}


void BookingDialog::on_m_line_edit_name_textEdited(const QString &arg1)
{
    updateMembersQuery(arg1);
    updateBlocksQuery(arg1);
}


void BookingDialog::selectCurrentMemberId(const QModelIndex &index)
{
    if(index.isValid())
    {
        m_last_selected_member_id = m_memberlist_model->record(index.row()).value(1).toInt();
        m_last_selected_block_id = -1;
    }
}


void BookingDialog::selectCurrentBlockId(const QModelIndex &index)
{
    if(index.isValid())
    {
        m_last_selected_block_id = m_blockslist_model->record(index.row()).value(2).toInt();
        m_last_selected_price_id = m_blockslist_model->record(index.row()).value(3).toInt();
        m_last_selected_member_id = m_blockslist_model->record(index.row()).value(4).toInt();
        m_num_of_used_bloks = m_blockslist_model->record(index.row()).value(5).toInt();
        setMode(MODE_SINGLE);
    }
}


void BookingDialog::on_m_list_view_members_clicked(const QModelIndex &index)
{
   handleCurrentMemberChanged(index, QModelIndex());
}


void BookingDialog::on_m_list_view_members_activated(const QModelIndex &index)
{
    handleCurrentMemberChanged(index, QModelIndex());
    accept();
}


void BookingDialog::on_m_list_view_blocks_clicked(const QModelIndex &index)
{
    handleCurrentBlockChanged(index, QModelIndex());
}


void BookingDialog::on_m_list_view_blocks_activated(const QModelIndex &index)
{
    handleCurrentBlockChanged(index, QModelIndex());
    accept();
}
