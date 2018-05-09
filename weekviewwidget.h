#ifndef WEEKVIEWWIDGET_H
#define WEEKVIEWWIDGET_H

#include <QDate>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QWidget>

class DayBookingTableModel;

namespace Ui {
class WeekViewWidget;
}

class WeekViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeekViewWidget(QWidget *parent = 0);
    ~WeekViewWidget();

    inline QDate firstDayOfCurrientWeek() const;
    inline QDate lastDayOfCurrientWeek() const;

    inline QDate currientDate() const;

    QPushButton *getReturnButton() const;

public slots:
    void setCurrientDate(QDate date);
    void fillCurrientWeek();

protected slots:
    void updateGUI();

private:
    Ui::WeekViewWidget *ui;

    QVector<QLabel*> m_day_labels;
    QVector<QTableView*> m_booking_tables;

    DayBookingTableModel* m_day_booking_models[7];
    //currient data
    QDate m_date;
};


#endif // WEEKVIEWWIDGET_H
