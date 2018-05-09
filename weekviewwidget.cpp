#include "weekviewwidget.h"
#include "ui_weekviewwidget.h"

WeekViewWidget::WeekViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeekViewWidget)
{
    ui->setupUi(this);
}

WeekViewWidget::~WeekViewWidget()
{
    delete ui;
}
