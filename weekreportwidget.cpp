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


void WeekReportWidget::updateQuery()
{
    if(!m_query)
         m_query = new QSqlQuery();

    //current period handling functions
    const QDate from_date = ui->m_from_date->date();
    const QDate till_date = ui->m_till_date->date();

    m_query->prepare("SELECT operation, revenues.type, revenues.account, TOTAL(sum), date FROM cash_register "
                  "INNER JOIN revenues ON cash_register.account = revenues.id "
                  "WHERE (date between :from_day AND :till_day) "
                  "GROUP BY operation, date, revenues.type, revenues.account ");
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

void WeekReportWidget::update()
{
    if(m_do_not_update)
        return;
    updateQuery();

    //current period handling functions
    const QDate from_date = ui->m_from_date->date();
    const QDate till_date = ui->m_till_date->date();

    QString html_text = m_template_str;
    html_text.replace("%from_datum%", from_date.toString("dd.MM.yyyy"));
    html_text.replace("%till_datum%", till_date.toString("dd.MM.yyyy"));

    const QString table_row_start_tag("%table_row_start%");
    int position = html_text.indexOf(table_row_start_tag);
    const QString table_row_end_tag("%table_row_end%");
    int end_position = html_text.indexOf(table_row_end_tag);
    QString row_string = html_text.mid(position+table_row_start_tag.length(), end_position - position - table_row_start_tag.length());
    html_text.remove(position, end_position - position + table_row_end_tag.length());
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
        html_text.insert(position,tmp_string); position += tmp_string.length();
    }
    html_text.replace("%total_sum_revenues%", QString("%1 €").arg(total_sum_revenues));
    html_text.replace("%total_sum_expenses%", QString("%1 €").arg(total_sum_expenses));
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
