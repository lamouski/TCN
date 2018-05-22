#include "bookingdialog.h"
#include "ui_bookingdialog.h"

#include <QDate>
#include <QSqlQuery>
#include <QSqlRecord>

BookingDialog::BookingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookingDialog),
    m_last_selected_id(-1)
{
    ui->setupUi(this);

    m_model = new QSqlQueryModel(this);
    m_model->setQuery("SELECT (firstname || ' ' || surname), id AS name FROM members");
    ui->m_list_view_members->setModel(m_model);
    ui->m_list_view_members->setModelColumn(0);

    connect(ui->m_list_view_members->selectionModel(),
          & QItemSelectionModel::currentChanged, this, &BookingDialog::handleCurrentMemberChanged);
 }

BookingDialog::~BookingDialog()
{
    delete ui;
}

void BookingDialog::setField(const QString& fieldName)
{
    ui->m_label_field->setText(fieldName);
}

void BookingDialog::setTimeslot(int timeSlot)
{
    ui->m_label_time->setText(QTime(timeSlot, 0).toString("HH:mm") + " - " + QTime(timeSlot+1, 0).toString("HH:mm"));
}

int BookingDialog::selectedId() const
{
    return m_last_selected_id;
}

void BookingDialog::handleCurrentMemberChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    //QSqlQuery q = m_model->query();
    //q.seek(current.row());
    if(current.isValid())
    {
        ui->m_line_edit_name->setText(m_model->record(current.row()).value(0).toString());
        selectCurrientId(current);
    }
}

void BookingDialog::on_m_line_edit_name_textEdited(const QString &arg1)
{
    if(arg1.isEmpty())
        m_model->setQuery("SELECT (firstname || ' ' || surname) AS name, id FROM members");
    else
        m_model->setQuery("SELECT (firstname || ' ' || surname) AS name, id FROM members WHERE name LIKE '%"+arg1+"%';");
}

void BookingDialog::selectCurrientId(const QModelIndex &index)
{
    //QSqlQuery q = m_model->query();
    //q.seek(index.row());
    if(index.isValid())
        m_last_selected_id = m_model->record(index.row()).value(1).toInt();
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
