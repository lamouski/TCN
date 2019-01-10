#include "kassaviewwidget.h"
#include "ui_kassaviewwidget.h"

KassaViewWidget::KassaViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KassaViewWidget)
{
    ui->setupUi(this);
}

KassaViewWidget::~KassaViewWidget()
{
    delete ui;
}
