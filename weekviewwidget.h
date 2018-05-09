#ifndef WEEKVIEWWIDGET_H
#define WEEKVIEWWIDGET_H

#include <QWidget>

namespace Ui {
class WeekViewWidget;
}

class WeekViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeekViewWidget(QWidget *parent = 0);
    ~WeekViewWidget();

private:
    Ui::WeekViewWidget *ui;
};

#endif // WEEKVIEWWIDGET_H
