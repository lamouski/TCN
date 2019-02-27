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

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QFile>
#include <QTextCodec>
#include <QDir>

#include "dayreportwidget.h"
#include "settings.h"
#include "ui_dayreportwidget.h"

DayReportWidget::DayReportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DayReportWidget)
{
    ui->setupUi(this);

    QString curr_dir = QDir::currentPath();
    QFile file(curr_dir+"/db/day_report_template.htm");
    if (!file.open(QFile::ReadOnly))
        m_template_str = "day_report_template.htm is not found";

    QByteArray data = file.readAll();
    QTextCodec *codec = QTextCodec::codecForHtml(data);
    m_template_str = codec->toUnicode(data);

}

DayReportWidget::~DayReportWidget()
{
    delete ui;
}

QPushButton *DayReportWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}

void DayReportWidget::showEvent(QShowEvent */*e*/)
{
    update();
}

void DayReportWidget::update()
{
    const QDate& date = Settings::currentDate();
    QSqlQuery query;
    query.prepare("SELECT cash_register.info, cash_register.operation, "
                  "       CASE cash_register.operation "
                  "        WHEN 0 THEN revenues.type "
                  "        WHEN 1 THEN expenses.type "
                  "       END AS account_type, "
                  "       CASE operation "
                  "        WHEN 0 THEN revenues.account "
                  "        WHEN 1 THEN expenses.account "
                  "       END AS account_number, "
                  "       cash_register.sum, fields.name, timeslot, bookings.status FROM cash_register "
                  "LEFT OUTER JOIN revenues ON cash_register.account = revenues.id "
                  "LEFT OUTER JOIN expenses ON cash_register.account = expenses.id "
                  "LEFT OUTER JOIN bookings ON cash_register.id = bookings.status "
                  "LEFT JOIN fields ON bookings.fieldid = fields.id "
                  "WHERE cash_register.date = :day "
                  "AND (aboid IS NULL OR aboid <= 0)");

    query.bindValue(":day", date.toJulianDay());
    if(!query.exec())
    {
        qDebug() << "query day " <<date.toString("yyyy-MM-dd") << " cash_register error:  "
              << query.lastError();
        return;
    }

    QSqlQuery query_other;
    query_other.prepare("SELECT (surname || ' ' || firstname) as name_info, bookings.info, revenues.type, revenues.account, bookings.sum, fields.name, timeslot, bookings.status FROM bookings "
                        "LEFT OUTER JOIN members ON bookings.memberid = members.id "
                        "LEFT OUTER JOIN fields ON bookings.fieldid = fields.id "
                        "LEFT OUTER JOIN prices ON bookings.priceid = prices.id "
                        "LEFT OUTER JOIN revenues ON prices.revenue = revenues.id "
                        "WHERE bookings.date = :day "
                        "AND (aboid IS NULL OR aboid <= 0) "
                        "AND (bookings.status IS NULL OR bookings.status <= 0)");
    query_other.bindValue(":day", date.toJulianDay());
    if(!query_other.exec())
    {
        qDebug() << "query day " <<date.toString("yyyy-MM-dd") << " bookings error:  "
              << query.lastError();
        return;
    }

    QString html_text = m_template_str;
    html_text.replace("%datum%", date.toString("dd.MM.yyyy"));
    const QString table_row_start_tag("%table_row_start%");
    int position = html_text.indexOf(table_row_start_tag);
    const QString table_row_end_tag("%table_row_end%");
    int end_position = html_text.indexOf(table_row_end_tag);
    QString row_string = html_text.mid(position+table_row_start_tag.length(), end_position - position - table_row_start_tag.length());
    html_text.remove(position, end_position - position + table_row_end_tag.length());

    double total_sum_revenues = 0.0;
    double total_sum_expenses = 0.0;
    while(query.next())
    {
        QString tmp_string = row_string;
        QString info = query.value(0).toString();
        tmp_string.replace("%full_name%", info);
        tmp_string.replace("%account_name%", query.value(2).toString());
        tmp_string.replace("%account%", query.value(3).toString());
        if(query.value(1).toInt() == 0)
        {
            tmp_string.replace("%sum_revenues%", query.value(4).toString() + " €");
            total_sum_revenues += query.value(4).toDouble();
            tmp_string.replace("%sum_expenses%", "");
        }
        else
        {
            tmp_string.replace("%sum_expenses%", query.value(4).toString() + " €");
            total_sum_expenses += query.value(4).toDouble();
            tmp_string.replace("%sum_revenues%", "");
        }
        tmp_string.replace("%field_name%", query.value(5).toString());
        tmp_string.replace("%time_slot%", query.value(6).toString());
        html_text.insert(position,tmp_string); position += tmp_string.length();
    }
    while(query_other.next())
    {
        QString tmp_string = row_string;
        QString full_name = query_other.value(0).toString();
        QString info = query_other.value(1).toString();
        if(!info.isEmpty())
        {
            if(full_name.isEmpty())
                full_name = info;
            else
                full_name += "(" + info + ")";
        }
        tmp_string.replace("%full_name%", full_name);
        tmp_string.replace("%account_name%", query_other.value(2).toString());
        tmp_string.replace("%account%", query_other.value(3).toString());
        int status = query_other.value(7).toInt();
        switch(status)
        {
        case -1: //canceled
            tmp_string.replace("%sum_revenues%", tr("(Canceled)"));
            break;
        case 0: default:
            tmp_string.replace("%sum_revenues%", tr("(Not paid)"));
            break;
        }
        tmp_string.replace("%sum_expenses%", "");
        tmp_string.replace("%field_name%", query_other.value(5).toString());
        tmp_string.replace("%time_slot%", query_other.value(6).toString());
        html_text.insert(position,tmp_string); position += tmp_string.length();
    }
    html_text.replace("%total_sum_revenues%", QString("%1 €").arg(total_sum_revenues));
    html_text.replace("%total_sum_expenses%", QString("%1 €").arg(total_sum_expenses));

    ui->m_text_editor->setHtml(html_text);

}
