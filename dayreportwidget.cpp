#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

#include "dayreportwidget.h"
#include "ui_dayreportwidget.h"

DayReportWidget::DayReportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DayReportWidget)
{
    ui->setupUi(this);

    ui->m_document_widget->setHtml("<p><span style=\"color:#000000; font-weight:600\">Tests</span></p>");

}

DayReportWidget::~DayReportWidget()
{
    delete ui;
}

QPushButton *DayReportWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}

/*
 * Sets currient day
 */
void DayReportWidget::setCurrientDate(QDate date) {
    if(m_date != date)
    {
        m_date = date;
        update();
    }
}


QDate DayReportWidget::currientDate() const
{
    return m_date;
}

void DayReportWidget::update()
{
    QSqlQuery query;
    query.prepare("SELECT (surname || ' ' || firstname), date, name, sum FROM bookings"
                    " LEFT OUTER JOIN members ON bookings.memberid = members.id "
                    " LEFT OUTER JOIN fields ON bookings.fieldid = fields.id "
                    " LEFT OUTER JOIN prices ON bookings.priceid = prices.id "
                    " WHERE date = :day ");
    query.bindValue(":day", m_date.toJulianDay());
    if(!query.exec())
    {
        qDebug() << "query day " << m_date.toString("yyyy-MM-dd") << " bookings error:  "
              << query.lastError();
        return;
    }


    QString html_text = "<h2 style=\"text-align: center;\">Platz buchung TCN Passau!</h2>";
    html_text += "<p style=\"font-size: 1.5em; text-align: center;\">"+m_date.toString("yyyy-MM-dd") + "</p>";

    html_text += "<table class==\"Table\" style=\"width: 600px; height: 95px;\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\">";
    html_text += "<tbody>";

    while(query.next())
    {
        html_text += "<tr>";
        html_text += "<td style=\"width: 300px;\"><strong>" + query.value(0).toString() + "</strong></td>";
        html_text += "<td style=\"width: 150px;\"><strong>" + query.value(2).toString() + "</strong></td>";
        html_text += "<td style=\"width: 120px;\"><strong>" + query.value(3).toString() + "</strong></td>";
        html_text += "<tr>";
     }

    ui->m_document_widget->setHtml(html_text);

}
