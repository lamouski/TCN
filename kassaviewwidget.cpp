#include <QMenu>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

#include <QSqlError>
#include <QDebug>

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

    QMenu *pop_up_revenue = new QMenu(this);
    pop_up_revenue->addAction(tr("Edit"), this, &KassaViewWidget::edit_current_revenue);
    //pop_up_revenue->addAction(tr("Copy"), this, &KassaViewWidget::copy_current_revenue);
    pop_up_revenue->addAction(tr("Delete"), this, &KassaViewWidget::delete_current_revenue);
    ui->m_revenue_others->setMenu(pop_up_revenue);

    QMenu *pop_up_expenses = new QMenu(this);
    pop_up_expenses->addAction(tr("Edit"), this, &KassaViewWidget::edit_current_revenue);
    //pop_up_expenses->addAction(tr("Copy"), this, &KassaViewWidget::copy_current_revenue);
    pop_up_expenses->addAction(tr("Delete"), this, &KassaViewWidget::delete_current_revenue);
    ui->m_costs_others->setMenu(pop_up_expenses);
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
        m_revenue_model->setEditStrategy(QSqlTableModel::OnRowChange);
        connect(m_revenue_model, &QSqlTableModel::primeInsert,
                [](int /*row*/, QSqlRecord &record)
        {
//            QSqlQuery query("SELECT max(id)+1 FROM cash_register");
//            query.exec();
//            query.first();
//            record.setValue("id", query.value(0).toInt());
            record.setGenerated("id", false);
            record.setValue("date", QVariant::fromValue<long long>(Settings::currentDate().toJulianDay()));
            record.setGenerated("date", true);
            record.setValue("operation", QVariant::fromValue<int>(0));
            record.setGenerated("operation", true);
        });
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
    ui->m_revenue_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);


    if(!m_expence_model)
    {
        m_expence_model = new QSqlRelationalTableModel(this);
        m_expence_model->setEditStrategy(QSqlTableModel::OnRowChange);
        connect(m_expence_model, &QSqlTableModel::primeInsert,
                [](int /*row*/, QSqlRecord &record)
        {
//            QSqlQuery query("SELECT max(id)+1 FROM cash_register");
//            query.exec();
//            query.first();
//            record.setValue("id", query.value(0).toInt());
            record.setGenerated("id", false);
            record.setValue("date", static_cast<long long>(Settings::currentDate().toJulianDay()));
            record.setGenerated("date", true);
            record.setValue("operation", static_cast<int>(1));
            record.setGenerated("operation", true);
        });
    }

    m_expence_model->setTable("cash_register");
    filter = QString("date = %1 AND operation = %2 ").arg(Settings::currentDate().toJulianDay()).arg(1);
    m_expence_model->setFilter(filter); //expenses
//    m_costs_model->setDay(Settings::currentDate());
//    m_costs_model->setOperation(1);

    m_expence_model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    accountIdx = m_expence_model->fieldIndex("account");
    m_expence_model->setRelation(accountIdx, QSqlRelation("expenses", "id", "type"));

    m_expence_model->setHeaderData(3, Qt::Horizontal, tr("Info"));
    m_expence_model->setHeaderData(4, Qt::Horizontal, tr("Type"));
    m_expence_model->setHeaderData(5, Qt::Horizontal, tr("Sum"));

    m_expence_model->select();

    ui->m_costs_table_view->setModel(m_expence_model);
    ui->m_costs_table_view->setItemDelegate(new QSqlRelationalDelegate(ui->m_costs_table_view));
    ui->m_costs_table_view->resizeRowsToContents();
    ui->m_costs_table_view->hideColumn(0);
    ui->m_costs_table_view->hideColumn(1);
    ui->m_costs_table_view->hideColumn(2);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    //ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->m_costs_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);


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

void KassaViewWidget::handleCurrentRevenueChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    if(m_edited_revenue_index.isValid() &&
            current.row() != m_edited_revenue_index.row())
    {
        if( m_revenue_model->submit())
        {
            ui->m_revenue_table_view->closePersistentEditor(m_edited_revenue_index);
            ui->m_revenue_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
            m_edited_revenue_index = QModelIndex();
        }
        else
        {
            qDebug() << "m_revenue_model->submit() error:  "
                  << m_revenue_model->lastError();
        }
    }
}


void KassaViewWidget::on_m_add_cost_clicked()
{
    const int row = m_expence_model->rowCount();
    m_expence_model->insertRows(row, 1);
    QModelIndex index = m_expence_model->index(row, 0);
    ui->m_costs_table_view->setCurrentIndex(index);
    edit_current_expense();
}

void KassaViewWidget::edit_current_expense()
{
    QModelIndex index = ui->m_costs_table_view->currentIndex();
    if(index.isValid())
    {
        m_edited_expence_index = index;
        ui->m_costs_table_view->edit(index);
        ui->m_costs_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
}

void KassaViewWidget::delete_current_expense()
{
    const int row = ui->m_costs_table_view->currentIndex().row();
    m_expence_model->removeRows(row, 1);
    m_expence_model->select();
}

void KassaViewWidget::handleCurrentExpenseChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{

    if(m_edited_expence_index.isValid() &&
            current.row() != m_edited_expence_index.row())
    {
        if( m_expence_model->submit())
        {
            ui->m_costs_table_view->closePersistentEditor(current);
            ui->m_costs_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
            m_edited_expence_index = QModelIndex();
        }
        else
        {
            qDebug() << "m_expence_model->submit() error:  "
                  << m_expence_model->lastError();
        }
    }
}

