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
    update();
}


void WeekReportWidget::updateQuery()
{
    if(!m_query)
         m_query = new QSqlQuery();
    const QDate& date = Settings::currentDate();
    //current week handling functions
    const QDate firstDayOfCurrientWeek = date.addDays(Qt::Monday - date.dayOfWeek());
    const QDate lastDayOfCurrientWeek = date.addDays(Qt::Sunday - date.dayOfWeek());

    m_query->prepare("SELECT account_name, account, TOTAL(bookings.sum), date FROM bookings "
                  "LEFT OUTER JOIN prices ON bookings.priceid = prices.id "
                  "INNER JOIN accounts ON prices.account = accounts.number "
                  "WHERE (date between :from_day AND :till_day) "
                  "AND (aboid IS NULL OR aboid <= 0) "
                  "AND (status IS NULL OR status != -1) "
                  "GROUP BY date, account ");
    m_query->bindValue(":from_day", firstDayOfCurrientWeek.toJulianDay());
    m_query->bindValue(":till_day", lastDayOfCurrientWeek.toJulianDay());
    if(!m_query->exec())
    {

        qDebug() << "query week " << date.toString("yyyy-MM-dd") << " bookings error:  "
              << m_query->lastError();

        qDebug() << m_query->lastQuery();
        return;
    }
}

void WeekReportWidget::update()
{
    updateQuery();

    const QDate& date = Settings::currentDate();
    //current week handling functions
    const QDate firstDayOfCurrientWeek = date.addDays(Qt::Monday - date.dayOfWeek());
    const QDate lastDayOfCurrientWeek = date.addDays(Qt::Sunday - date.dayOfWeek());

    QString html_text = m_template_str;
    html_text.replace("%from_datum%", firstDayOfCurrientWeek.toString("dd.MM.yyyy"));
    html_text.replace("%till_datum%", lastDayOfCurrientWeek.toString("dd.MM.yyyy"));

    const QString table_row_start_tag("%table_row_start%");
    int position = html_text.indexOf(table_row_start_tag);
    const QString table_row_end_tag("%table_row_end%");
    int end_position = html_text.indexOf(table_row_end_tag);
    QString row_string = html_text.mid(position+table_row_start_tag.length(), end_position - position - table_row_start_tag.length());
    html_text.remove(position, end_position - position + table_row_end_tag.length());
    double total_sum = 0.0;
    while(m_query->next())
    {
        QString tmp_string = row_string;
        tmp_string.replace("%account_name%", m_query->value(0).toString());
        tmp_string.replace("%account%", m_query->value(1).toString());
        tmp_string.replace("%sum%", m_query->value(2).toString() + " €");
        total_sum += m_query->value(2).toDouble();
        tmp_string.replace("%date%", QDate::fromJulianDay(m_query->value(3).toInt()).toString("dd.MM.yyyy"));
        html_text.insert(position,tmp_string); position += tmp_string.length();
    }
    html_text.replace("%total_sum%", QString("%1 €").arg(total_sum));
    ui->m_text_editor->setHtml(html_text);

}

void WeekReportWidget::exportCVS()
{
    QFileDialog fileDialog(this, tr("Export TXT Directory"));
    fileDialog.setFileMode(QFileDialog::Directory);
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString dirName = fileDialog.selectedFiles().first();

    QFile file(dirName+"/"+Settings::weekReportRevenuesFilename());
    if (file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
    {
        QTextStream stream(&file);
        bool new_record = m_query->first();
        while(new_record)
        {
            stream << m_query->value(0).toString() << ";";
            stream << m_query->value(1).toString() << ";";
            stream << m_query->value(2).toString() + " €" << ";";
            stream << QDate::fromJulianDay(m_query->value(3).toInt()).toString("dd.MM.yyyy") << ";";
            stream << endl;
            new_record = m_query->next();
        }
    }
    else
    {
        QMessageBox::information(this, QString(), QString(tr("The Revenues file can't be created.")));
        return;
    }
    file.close();
}
