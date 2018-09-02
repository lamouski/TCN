#ifndef WEEKREPORTWIDGET_H
#define WEEKREPORTWIDGET_H

#include <QDate>
#include <QPushButton>
#include <QWidget>

namespace Ui {
class WeekReportWidget;
}

class WeekReportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeekReportWidget(QWidget *parent = 0);
    ~WeekReportWidget();

    inline QDate currientDate() const;

    QPushButton *getReturnButton() const;

public slots:
    void setCurrientDate(QDate date);

protected:
    void update();

private:
    Ui::WeekReportWidget *ui;

    //currient data
    QDate m_date;
    QString m_template_str;
};

#endif // WEEKREPORTWIDGET_H
