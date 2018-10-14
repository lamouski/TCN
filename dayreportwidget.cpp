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
    query.prepare("SELECT (surname || ' ' || firstname), info, account_name, account, sum, name, timeslot FROM bookings "
                  "LEFT OUTER JOIN members ON bookings.memberid = members.id "
                  "LEFT OUTER JOIN fields ON bookings.fieldid = fields.id "
                  "LEFT OUTER JOIN prices ON bookings.priceid = prices.id "
                  "LEFT OUTER JOIN accounts ON prices.account = accounts.number "
                  "WHERE date = :day "
                  "AND aboid IS NULL OR aboid = ''");
    query.bindValue(":day", date.toJulianDay());
    if(!query.exec())
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
    while(query.next())
    {
        QString tmp_string = row_string;
        QString full_name = query.value(0).toString();
        QString info = query.value(1).toString();
        if(!info.isEmpty())
        {
            if(full_name.isEmpty())
                full_name = info;
            else
                full_name += "(" + info + ")";
        }
        tmp_string.replace("%full_name%", full_name);
        tmp_string.replace("%account_name%", query.value(2).toString());
        tmp_string.replace("%account%", query.value(3).toString());
        tmp_string.replace("%price%", query.value(4).toString() + "€");
        tmp_string.replace("%field_name%", query.value(5).toString());
        tmp_string.replace("%time_slot%", query.value(6).toString());
        html_text.insert(position,tmp_string); position += tmp_string.length();
    }
    ui->m_text_editor->setHtml(html_text);

}
