#include <QSqlQuery>
#include <QSqlRelationalDelegate>

#include "dbmanager.h"
#include "settings.h"
#include "ui_kassaviewwidget.h"
#include "daykassatablemodel.h"

#include "kassaviewwidget.h"
#include "kassaviewwidget.h"

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

    if(!m_revenue_model)
    {
        m_revenue_model = new QSqlRelationalTableModel (this);
    }
    m_revenue_model->setTable("cash_register");
    QString filter = QString("date = %1 AND operation = %2 ").arg(Settings::currentDate().toJulianDay()).arg(0);
    m_revenue_model->setFilter(filter); //revenue
//    m_revenue_model->setDay(Settings::currentDate());
//    m_revenue_model->setOperation(0);

    m_revenue_model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    int accountIdx = m_revenue_model->fieldIndex("account");
    m_revenue_model->setRelation(accountIdx, QSqlRelation("revenues", "id", "type"));

    m_revenue_model->setHeaderData(3, Qt::Horizontal, tr("Info"));
    m_revenue_model->setHeaderData(4, Qt::Horizontal, tr("Type"));
    m_revenue_model->setHeaderData(5, Qt::Horizontal, tr("Sum"));

    m_revenue_model->select();

    ui->m_revenue_table_view->setModel(m_revenue_model);
    ui->m_revenue_table_view->setItemDelegate(new QSqlRelationalDelegate(ui->m_revenue_table_view));
    ui->m_revenue_table_view->resizeRowsToContents();
    ui->m_revenue_table_view->hideColumn(0);
    ui->m_revenue_table_view->hideColumn(1);
    ui->m_revenue_table_view->hideColumn(2);
    ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    //ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);


    if(!m_costs_model)
    {
        m_costs_model = new QSqlRelationalTableModel(this);
    }
    m_costs_model->setTable("cash_register");
    filter = QString("date = %1 AND operation = %2 ").arg(Settings::currentDate().toJulianDay()).arg(1);
    m_costs_model->setFilter(filter); //expenses
//    m_costs_model->setDay(Settings::currentDate());
//    m_costs_model->setOperation(1);

    m_costs_model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    accountIdx = m_costs_model->fieldIndex("account");
    m_costs_model->setRelation(accountIdx, QSqlRelation("expenses", "id", "type"));

    m_costs_model->setHeaderData(3, Qt::Horizontal, tr("Info"));
    m_costs_model->setHeaderData(4, Qt::Horizontal, tr("Type"));
    m_costs_model->setHeaderData(5, Qt::Horizontal, tr("Sum"));

    m_costs_model->select();

    ui->m_costs_table_view->setModel(m_costs_model);
    ui->m_costs_table_view->setItemDelegate(new QSqlRelationalDelegate(ui->m_costs_table_view));
    ui->m_costs_table_view->resizeRowsToContents();
    ui->m_costs_table_view->hideColumn(0);
    ui->m_costs_table_view->hideColumn(1);
    ui->m_costs_table_view->hideColumn(2);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    //ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);


    connect(ui->m_revenue_table_view->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &KassaViewWidget::handleCurrentRevenueChanged);

    connect(ui->m_costs_table_view->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &KassaViewWidget::handleCurrentExpenseChanged);
}

void KassaViewWidget::updateGUI()
{
    ui->m_day_label->setText(Settings::currentDate().toString("dd MMMM yyyy"));
}

void KassaViewWidget::on_m_add_revenue_clicked()
{
    const int row = m_revenue_model->rowCount();
    m_revenue_model->insertRows(row, 1);
    QModelIndex index = m_revenue_model->index(row, 0);
    ui->m_revenue_table_view->setCurrentIndex(index);
    edit_current_revenue();
}

void KassaViewWidget::edit_current_revenue()
{
    QModelIndex index = ui->m_revenue_table_view->currentIndex();
    if(index.isValid())
    {
        m_edited_revenue_index = index;
        ui->m_revenue_table_view->edit(index);
        ui->m_revenue_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
}

void KassaViewWidget::delete_current_revenue()
{
    const int row = ui->m_revenue_table_view->currentIndex().row();
    m_revenue_model->removeRows(row, 1);
    m_revenue_model->select();
}

void KassaViewWidget::handleCurrentRevenueChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(m_edited_revenue_index == previous)
    {
        ui->m_revenue_table_view->closePersistentEditor(m_edited_revenue_index);
        ui->m_revenue_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);        
        m_edited_revenue_index = QModelIndex();
    }
}


void KassaViewWidget::on_m_add_cost_clicked()
{
    const int row = m_costs_model->rowCount();
    m_costs_model->insertRows(row, 1);
    QModelIndex index = m_costs_model->index(row, 0);
    ui->m_costs_table_view->setCurrentIndex(index);
    //edit_current_expense();
}


void KassaViewWidget::handleCurrentExpenseChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(current.row() != m_edited_revenue_index.row())
    {
        ui->m_costs_table_view->closePersistentEditor(current);
        ui->m_costs_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_edited_expence_index = QModelIndex();
    }
}

