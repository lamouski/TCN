#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QFile>
#include <QTextCodec>
#include <QDir>

#include "weekreportwidget.h"
#include "ui_weekreportwidget.h"

WeekReportWidget::WeekReportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeekReportWidget)
{
    ui->setupUi(this);

    QString curr_dir = QDir::currentPath();
    QFile file(curr_dir+"/test_db/week_report_template.htm");
    if (!file.open(QFile::ReadOnly))
        m_template_str = "week_report_template.htm is not found";

    QByteArray data = file.readAll();
    QTextCodec *codec = QTextCodec::codecForHtml(data);
    m_template_str = codec->toUnicode(data);

}

WeekReportWidget::~WeekReportWidget()
{
    delete ui;
}

QPushButton *WeekReportWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}

/*
 * Sets currient day
 */
void WeekReportWidget::setCurrientDate(QDate date) {
    if(m_date != date)
    {
        m_date = date;
        update();
    }
}


QDate WeekReportWidget::currientDate() const
{
    return m_date;
}

void WeekReportWidget::update()
{
    //current week handling functions
    const QDate firstDayOfCurrientWeek = m_date.addDays(Qt::Monday - m_date.dayOfWeek());
    const QDate lastDayOfCurrientWeek = m_date.addDays(Qt::Sunday - m_date.dayOfWeek());

    QSqlQuery query;
    query.prepare("SELECT account_name, account, TOTAL(sum), date FROM bookings "
                  "LEFT OUTER JOIN prices ON bookings.priceid = prices.id "
                  "LEFT OUTER JOIN accounts ON prices.account = accounts.number "
                  "WHERE date between :from_day AND :till_day "
                  "GROUP BY account, date "                  );
    query.bindValue(":from_day", firstDayOfCurrientWeek.toJulianDay());
    query.bindValue(":till_day", lastDayOfCurrientWeek.toJulianDay());
    if(!query.exec())
    {
        qDebug() << "query day " << m_date.toString("yyyy-MM-dd") << " bookings error:  "
              << query.lastError();
        return;
    }


    QString html_text = m_template_str;
    html_text.replace("%from_datum%", firstDayOfCurrientWeek.toString("dd.MM.yyyy"));
    html_text.replace("%till_datum%", lastDayOfCurrientWeek.toString("dd.MM.yyyy"));

    const QString table_row_start_tag("%table_row_start%");
    int position = html_text.indexOf(table_row_start_tag);
    const QString table_row_end_tag("%table_row_end%");
    int end_position = html_text.indexOf(table_row_end_tag);
    QString row_string = html_text.mid(position+table_row_start_tag.length(), end_position - position - table_row_start_tag.length());
    html_text.remove(position, end_position - position + table_row_end_tag.length());
    while(query.next())
    {
        QString tmp_string = row_string;
        tmp_string.replace("%account_name%", query.value(0).toString());
        tmp_string.replace("%account%", query.value(1).toString());
        tmp_string.replace("%price%", query.value(2).toString() + "€");
        tmp_string.replace("%date%", QDate::fromJulianDay(query.value(3).toInt()).toString("dd.MM.yyyy"));
        html_text.insert(position,tmp_string); position += tmp_string.length();
    }
    ui->m_text_editor->setHtml(html_text);

}
