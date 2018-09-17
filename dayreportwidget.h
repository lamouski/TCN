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

    QPushButton *getReturnButton() const;

protected:
    void update();

    void showEvent(QShowEvent *);

private:
    Ui::DayReportWidget *ui;

    QString m_template_str;
};

#endif // DAYREPORTWIDGET_H
