#ifndef DAYREPORTWIDGET_H
#define DAYREPORTWIDGET_H

#include <QDate>
#include <QPushButton>
#include <QWidget>

namespace Ui {
class DayReportWidget;
}

class DayReportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DayReportWidget(QWidget *parent = 0);
    ~DayReportWidget();

    inline QDate currientDate() const;

    QPushButton *getReturnButton() const;

public slots:
    void setCurrientDate(QDate date);

protected:
    void update();

private:
    Ui::DayReportWidget *ui;

    //currient data
    QDate m_date;
};

#endif // DAYREPORTWIDGET_H
