#include "blockbookingsview.h"
#include "dbmanager.h"
#include "ui_blockbookingsview.h"

#include <QSqlQueryModel>

BlockBookingsView::BlockBookingsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlockBookingsView)
{
    ui->setupUi(this);
}

BlockBookingsView::~BlockBookingsView()
{
    delete ui;
}


QPushButton *BlockBookingsView::getReturnButton() const
{
    return ui->m_button_back_to_main_menu;
}

void BlockBookingsView::showEvent(QShowEvent */*e*/)
{
    fillData();
    updateGUI();
}

void BlockBookingsView::hideEvent(QHideEvent */*e*/)
{

}

void BlockBookingsView::fillData() {
    DbManager* db = DbManager::instance();
    if(!db)
        return;

    if(!m_model)
    {
        m_model = new QSqlQueryModel (this);
    }
    m_model->setQuery("SELECT block_bookings.date, bookings.date, bookings.blockid, amount, (surname || ' ' || firstname) as name_info, bookings.info, bookings.sum, fields.name, timeslot, bookings.status from  block_bookings "
                      "LEFT OUTER JOIN members ON bookings.memberid = members.id "
                      "LEFT OUTER JOIN fields ON bookings.fieldid = fields.id "
                      "JOIN bookings where bookings.blockid=block_bookings.id "
                      "ORDER BY bookings.blockid, CASE WHEN sum>0.0 THEN 0 ELSE 1 END,bookings.date");

//    m_model->setHeaderData(3, Qt::Horizontal, tr("Info"));
//    m_model->setHeaderData(4, Qt::Horizontal, tr("Type"));
//    m_model->setHeaderData(5, Qt::Horizontal, tr("Sum"));

    ui->m_table_view->setModel(m_model);
    ui->m_table_view->resizeRowsToContents();
//    ui->m_table_view->hideColumn(0);
//    ui->m_table_view->hideColumn(1);
//    ui->m_table_view->hideColumn(2);
//    //ui//->m_table_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
//    //ui->m_table_view->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
//    //ui->m_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
//    ui->m_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
//    //ui->m_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);


//    connect(ui->m_costs_table_view->selectionModel(), &QItemSelectionModel::currentChanged,
//            this, &KassaViewWidget::handleCurrentExpenseChanged);
}


void BlockBookingsView::updateGUI()
{
    //ui->m_day_label->setText(Settings::currentDate().toString("dd MMMM yyyy"));
}
