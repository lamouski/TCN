#include "weekviewwidget.h"
#include "ui_weekviewwidget.h"
#include "dbmanager.h"
#include "daybookingtablemodel.h"

WeekViewWidget::WeekViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeekViewWidget)
{
    ui->setupUi(this);
    m_day_labels = QVector<QLabel*>({ui->m_label_day1, ui->m_label_day2, ui->m_label_day3, ui->m_label_day4, ui->m_label_day5, ui->m_label_day6, ui->m_label_day7});
    m_booking_tables = QVector<QTableView*>({ui->m_table_booking_1,ui->m_table_booking_2,ui->m_table_booking_3,ui->m_table_booking_4,ui->m_table_booking_5,ui->m_table_booking_6,ui->m_table_booking_7});
}

WeekViewWidget::~WeekViewWidget()
{
    delete ui;
}

QPushButton *WeekViewWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}


/*
 * Sets currient day
 */
void WeekViewWidget::setCurrientDate(QDate date) {
    m_date = date;
    fillCurrientWeek();
    updateGUI();
}

QDate  WeekViewWidget::currientDate() const
{
    return m_date;
}

void WeekViewWidget::updateGUI()
{
    ui->m_week_label->setText(
                QString("%1 - %2 %3")
                    .arg(firstDayOfCurrientWeek().day())
                    .arg(lastDayOfCurrientWeek().day())
                    .arg(m_date.toString("MMMM yyyy")));
}

//current week handling functions

inline QDate WeekViewWidget::firstDayOfCurrientWeek() const
{
    return m_date.addDays(Qt::Monday - m_date.dayOfWeek());
}

inline QDate WeekViewWidget::lastDayOfCurrientWeek() const
{
    return m_date.addDays(m_date.dayOfWeek() - Qt::Sunday);
}

void WeekViewWidget::fillCurrientWeek() { //on show
    DbManager* db = DbManager::instance();
    if(!db)
        return;
    QDate day = firstDayOfCurrientWeek();
    for(qint64 i = 0; i < 7; i ++) {
        DayBookingTableModel* model = new DayBookingTableModel(this);
        model->setDay(day);
        m_booking_tables[i]->setModel(model);
        m_booking_tables[i]->resizeColumnsToContents();
        m_booking_tables[i]->resizeRowsToContents();

        m_day_booking_models[i] = model;

        day = day.addDays(1);
    }
}
