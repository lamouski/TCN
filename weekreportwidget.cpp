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
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>

#include "dbmanager.h"
#include "weekreportwidget.h"
#include "ui_weekreportwidget.h"
#include "settings.h"

WeekReportWidget::WeekReportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeekReportWidget)
{
    ui->setupUi(this);

    QString curr_dir = QDir::currentPath();
    QFile file(curr_dir+"/db/week_report_template.htm");
    if (!file.open(QFile::ReadOnly))
        m_template_str = "week_report_template.htm is not found";

    QByteArray data = file.readAll();
    QTextCodec *codec = QTextCodec::codecForHtml(data);
    m_template_str = codec->toUnicode(data);

    QMenu* mode_menu = new QMenu(this);
    mode_menu->addAction(QString(tr("Export as txt (CVS)")), [this](){ exportCVS();});
    ui->m_button_extended->setMenu(mode_menu);

    connect(ui->m_from_date, &QDateEdit::dateChanged,
            [this](const QDate &/*date*/) { update(); });

    connect(ui->m_till_date, &QDateEdit::dateChanged,
            [this](const QDate &/*date*/) { update(); });
}

WeekReportWidget::~WeekReportWidget()
{
    delete ui;
    if(m_query)
        delete m_query;
}

QPushButton *WeekReportWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}

void WeekReportWidget::showEvent(QShowEvent */*e*/)
{

    const QDate& date = Settings::currentDate();
    //current week handling functions
    m_do_not_update = true;
    ui->m_from_date->setDate(date.addDays(Qt::Monday - date.dayOfWeek()));
    ui->m_till_date->setDate(date.addDays(Qt::Sunday - date.dayOfWeek()));
    m_do_not_update = false;
    update();
}


void WeekReportWidget::updateQuery(bool group_by_date)
{
    if(!m_query)
         m_query = new QSqlQuery();

    //current period handling functions
    const QDate from_date = ui->m_from_date->date();
    const QDate till_date = ui->m_till_date->date();

    m_query->prepare(QString("SELECT operation, "
                     "       CASE operation "
                     "        WHEN 0 THEN revenues.type "
                     "        WHEN 1 THEN expenses.type "
                     "       END AS account_type, "
                     "       CASE operation "
                     "        WHEN 0 THEN revenues.account "
                     "        WHEN 1 THEN expenses.account"
                     "       END AS account_number,"
                     "       TOTAL(sum), date FROM cash_register "
                     "LEFT OUTER JOIN revenues ON cash_register.account = revenues.id "
                     "LEFT OUTER JOIN expenses ON cash_register.account = expenses.id "
                     "WHERE (date between :from_day AND :till_day) "
                     "GROUP BY ") + (group_by_date ? QString("date, ") : "") +
                     QString("operation, cash_register.account, revenues.type, revenues.account "));
    m_query->bindValue(":from_day", from_date.toJulianDay());
    m_query->bindValue(":till_day", till_date.toJulianDay());
    if(!m_query->exec())
    {

        qDebug() << "query period from " << from_date.toString("yyyy-MM-dd")
                 << " till " << till_date.toString("yyyy-MM-dd") << " cash_register error:  "
              << m_query->lastError();
        return;
    }
}

QString extract_tag(const QString& tag, const QString& html_template)
{
    const QString start_tag = QString("%") + tag + QString("_start%");
    int position = html_template.indexOf(start_tag);
    const QString end_tag = QString("%") + tag + QString("_end%");
    int end_position = html_template.indexOf(end_tag);
    QString tag_string = html_template.mid(position + start_tag.length(),
                                           end_position - position - start_tag.length());
    //html_template.remove(position, end_position - position + end_tag.length());
    return tag_string;
}

void WeekReportWidget::update()
{
    if(m_do_not_update)
        return;

    //current period handling functions
    const QDate from_date = ui->m_from_date->date();
    const QDate till_date = ui->m_till_date->date();

    QString html_text;
    QTextStream html_stream(&html_text);

    QString doc_header = extract_tag("doc_header", m_template_str);
    doc_header.replace("%from_datum%", from_date.toString("dd.MM.yyyy"));
    doc_header.replace("%till_datum%", till_date.toString("dd.MM.yyyy"));
    html_stream << doc_header;

    for(int i = 0; i < 2; i++)
    {
        QString table_name("table");
        if(i == 0)
        {
            updateQuery(true);
        }
        else {
            table_name = "second_table";
            updateQuery(false);
        }
        QString header_string = extract_tag(table_name + "_header", m_template_str);
        QString row_string = extract_tag(table_name + "_row", m_template_str);
        QString footer_string = extract_tag(table_name + "_footer", m_template_str);

        html_stream << header_string;
        double total_sum_revenues = 0.0;
        double total_sum_expenses = 0.0;
        while(m_query->next())
        {
            QString tmp_string = row_string;
            tmp_string.replace("%account_name%", m_query->value(1).toString());
            tmp_string.replace("%account%", m_query->value(2).toString());

            if(m_query->value(0).toInt() == 0)
            {
                tmp_string.replace("%sum_revenues%", m_query->value(3).toString() + " €");
                total_sum_revenues += m_query->value(3).toDouble();
                tmp_string.replace("%sum_expenses%", "");
            }
            else {
                tmp_string.replace("%sum_expenses%", m_query->value(3).toString() + " €");
                total_sum_expenses += m_query->value(3).toDouble();
                tmp_string.replace("%sum_revenues%", "");
            }

            tmp_string.replace("%date%", QDate::fromJulianDay(m_query->value(4).toInt()).toString("dd.MM.yyyy"));
            html_stream << tmp_string;
        }
        footer_string.replace("%total_sum_revenues%", QString("%1 €").arg(total_sum_revenues));
        footer_string.replace("%total_sum_expenses%", QString("%1 €").arg(total_sum_expenses));
        html_stream << footer_string;

    }

    QString doc_footer = extract_tag("doc_footer", m_template_str);
    html_stream << doc_footer;
    html_stream.flush();
    ui->m_text_editor->setHtml(html_text);

}

void WeekReportWidget::exportCVS()
{

    QFileDialog fileDialog(this, tr("Export TXT Directory"));
    fileDialog.setFileMode(QFileDialog::Directory);
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString dirName = fileDialog.selectedFiles().first();
    QString kassa_account = Settings::getString("cash_register_account");
    QFile file_revenues(dirName+"/"+Settings::getString("week_report_revenues_filename"));
    if (file_revenues.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
    {
        QTextStream stream(&file_revenues);
        updateQuery(true);
        bool new_record = m_query->first();
        while(new_record)
        {
            if(m_query->value(0).toInt() == 0)
            {
                stream << m_query->value(1).toString() << ";";
                stream << m_query->value(2).toString() << ";";
                stream << m_query->value(3).toString() + " €" << ";";
                stream << QDate::fromJulianDay(m_query->value(4).toInt()).toString("dd.MM.yyyy") << ";";
                stream << kassa_account << ";";
                stream << endl;
            }
            new_record = m_query->next();
        }
    }
    else
    {
        QMessageBox::information(this, QString(), QString(tr("The Revenues file can't be created.")));
        return;
    }
    file_revenues.close();

    QFile file_expenses(dirName+"/"+Settings::getString("week_report_costs_filename"));
    if (file_expenses.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
    {
        QTextStream stream(&file_expenses);
        bool new_record = m_query->first();
        while(new_record)
        {
            if(m_query->value(0).toInt() == 1)
            {
                stream << m_query->value(1).toString() << ";";
                stream << kassa_account << ";";
                stream << m_query->value(3).toString() + " €" << ";";
                stream << QDate::fromJulianDay(m_query->value(4).toInt()).toString("dd.MM.yyyy") << ";";
                stream << m_query->value(2).toString() << ";";
                stream << endl;
            }
            new_record = m_query->next();
        }
    }
    else
    {
        QMessageBox::information(this, QString(), QString(tr("The Expenses file can't be created.")));
        return;
    }
    file_expenses.close();
}
