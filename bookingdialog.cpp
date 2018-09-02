#include "bookingdialog.h"
#include "ui_bookingdialog.h"

#include <QDate>
#include <QSqlQuery>
#include <QSqlRecord>

BookingDialog::BookingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookingDialog),
    m_last_selected_id(-1),
    m_last_selected_member_number(0),
    m_last_selected_price_id(-1)
{
    ui->setupUi(this);

    m_memberlist_model = new QSqlQueryModel(this);
    m_memberlist_base_query_string = QString("SELECT (firstname || ' ' || surname) AS name, id, membernumber FROM members");
    m_memberlist_model->setQuery(m_memberlist_base_query_string);
    ui->m_list_view_members->setModel(m_memberlist_model);
    ui->m_list_view_members->setModelColumn(0);

    connect(ui->m_list_view_members->selectionModel(),
          &QItemSelectionModel::currentChanged, this, &BookingDialog::handleCurrentMemberChanged);

    m_prices_model = new QSqlQueryModel(this);
    m_prices_base_query_string = QString("SELECT (price_name || ' - ' || sum) "
                                         " AS info, sum, id FROM prices");
    m_prices_model->setQuery(m_prices_base_query_string);
    ui->m_combo_price->setModel(m_prices_model);
    ui->m_combo_price->setModelColumn(0);
}

BookingDialog::~BookingDialog()
{
    delete ui;
}

void BookingDialog::reset()
{
    ui->m_line_edit_name->clear();
    updateMembersQuery(ui->m_line_edit_name->text());
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

void BookingDialog::setMemberId(int id)
{
    QSqlQuery query(m_memberlist_base_query_string + QString(" WHERE id = %0").arg(id));
    QString name;
    if(query.exec())
        if(query.next())
            name = query.record().value(0).toString();
    ui->m_line_edit_name->setText(name);
    updateMembersQuery(ui->m_line_edit_name->text());
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

int BookingDialog::selectedId() const
{
    return m_last_selected_id;
}

int BookingDialog::selectedPrice() const
{
    return m_last_selected_price_id;
}

void BookingDialog::handleCurrentMemberChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    if(current.isValid())
    {
        ui->m_line_edit_name->setText(m_memberlist_model->record(current.row()).value(0).toString());
        selectCurrientId(current);
        updatePriceQuery();
    }
}

void BookingDialog::updateMembersQuery(const QString &find_string)
{
    QStringList key_words = find_string.split(" ", QString::SkipEmptyParts);
    if(key_words.empty())
        m_memberlist_model->setQuery(m_memberlist_base_query_string);
    else
    {
        QString query_string = m_memberlist_base_query_string;
        QString and_string(" WHERE ");
        foreach (QString key_word, key_words)
        {
            query_string += and_string + "name LIKE '%"+key_word+"%'";
            and_string = QString(" AND ");
        }
        m_memberlist_model->setQuery(query_string);
    }
}

void BookingDialog::updatePriceQuery()
{
    QString current_price = ui->m_combo_price->currentText();
    QString query_string = m_prices_base_query_string;
    query_string += " WHERE start_time_slot <= " + QString("%1").arg(m_timeslot) +
                    " AND end_time_slot > " + QString("%1").arg(m_timeslot);
    query_string += QString(" AND Winter = ") + "'false'";
    query_string += QString(" AND Mitglied = ") + (m_last_selected_member_number > 0 ? "'true'" : "'false'");

    m_prices_model->setQuery(query_string);

    int index_in_new_list = ui->m_combo_price->findText(current_price);
    if(index_in_new_list >= 0)
        ui->m_combo_price->setCurrentIndex(index_in_new_list);
    else
        if(ui->m_combo_price->count() > 0)
            ui->m_combo_price->setCurrentIndex(0);

}

void BookingDialog::on_m_line_edit_name_textEdited(const QString &arg1)
{
    updateMembersQuery(arg1);
}

void BookingDialog::selectCurrientId(const QModelIndex &index)
{
    if(index.isValid())
    {
        m_last_selected_id = m_memberlist_model->record(index.row()).value(1).toInt();
        m_last_selected_member_number = m_memberlist_model->record(index.row()).value(2).toInt();
    }
}

void BookingDialog::on_m_list_view_members_clicked(const QModelIndex &index)
{
   selectCurrientId(index);
}

void BookingDialog::on_m_list_view_members_activated(const QModelIndex &index)
{
    selectCurrientId(index);
    accept();
}

void BookingDialog::on_m_combo_price_currentIndexChanged(int index)
{
    int row = ui->m_combo_price->currentIndex();
    QModelIndex idx = m_prices_model->index(row, 2); // third column
    m_last_selected_price_id = m_prices_model->data(idx).toInt();
}
