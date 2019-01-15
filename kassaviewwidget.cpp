#include "dbmanager.h"
#include "kassaviewwidget.h"
#include "settings.h"
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


QPushButton *KassaViewWidget::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}

void KassaViewWidget::showEvent(QShowEvent */*e*/)
{
    fillCurrentDay();
    updateGUI();
}

void KassaViewWidget::fillCurrentDay() {
    DbManager* db = DbManager::instance();
    if(!db)
        return;


    if(!m_revenue_model) {
        m_revenue_model = new DayKassaTableModel(this);
    }
    ui->m_revenue_table_view->setModel(nullptr);
    m_revenue_model->setOperation(0); //revenue
    m_revenue_model->setDay(Settings::currentDate());
    m_revenue_model->select();
    ui->m_revenue_table_view->setModel(m_revenue_model);
    ui->m_revenue_table_view->resizeRowsToContents();
    ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);


    if(!m_costs_model)
    {
        m_costs_model = new DayKassaTableModel(this);
    }
    ui->m_costs_table_view->setModel(nullptr);
    m_costs_model->setOperation(1); //costs
    m_costs_model->setDay(Settings::currentDate());
    m_costs_model->select();
    ui->m_costs_table_view->setModel(m_costs_model);
    ui->m_costs_table_view->resizeRowsToContents();
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

}

void KassaViewWidget::updateGUI()
{
    ui->m_day_label->setText(Settings::currentDate().toString("dd MMMM yyyy"));
}
