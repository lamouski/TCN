#include "blockbookingsmodel.h"
#include "blockbookingsview.h"
#include "blockbookingsview.h"
#include "dbmanager.h"
#include "ui_blockbookingsview.h"

#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>

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

    if(!m_model_blocks)
    {
        m_model_blocks = new BlockBookingsModel(this);
        m_model_blocks->setMode(BlockBookingsModel::MODE_BLOCKS);
    }
    if(!m_model_bookings)
    {
        m_model_bookings = new BlockBookingsModel(this);
        m_model_bookings->setMode(BlockBookingsModel::MODE_BOOKINGS);
    }
    m_model_blocks->updateQuery();
    m_model_bookings->updateQuery();

    ui->m_blocks_table_view->setModel(m_model_blocks);
    ui->m_blocks_table_view->resizeRowsToContents();
    ui->m_blocks_table_view->verticalHeader()->hide();
    ui->m_blocks_table_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->m_blocks_table_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->m_blocks_table_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->m_blocks_table_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    connect(ui->m_blocks_table_view->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &BlockBookingsView::handleCurrentBlockChanged);

    ui->m_bookings_table_view->setModel(m_model_bookings);
    ui->m_bookings_table_view->resizeRowsToContents();
    ui->m_bookings_table_view->verticalHeader()->hide();
    ui->m_bookings_table_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->m_bookings_table_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->m_bookings_table_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->m_bookings_table_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->m_bookings_table_view->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->m_bookings_table_view->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->m_bookings_table_view->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    //ui->m_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void BlockBookingsView::handleCurrentBlockChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(current.isValid())
    {
        QModelIndex block_id_index = m_model_blocks->index(current.row(), 1);
        m_model_bookings->setBlockId(m_model_blocks->data(block_id_index).toInt());        
    }
    else {
        m_model_bookings->setBlockId(-1);
    }
    //ui->m_bookings_table_view->setModel(m_model_bookings);
}

void BlockBookingsView::updateGUI()
{
    //ui->m_day_label->setText(Settings::currentDate().toString("dd MMMM yyyy"));
}
