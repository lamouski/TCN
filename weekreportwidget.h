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

    QPushButton *getReturnButton() const;

protected:
    void update();    
    void showEvent(QShowEvent *);

private:
    Ui::WeekReportWidget *ui;

    QString m_template_str;
};

#endif // WEEKREPORTWIDGET_H
