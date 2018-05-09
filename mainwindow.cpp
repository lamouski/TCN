#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dbmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_members_list_dialog(nullptr)
{
    ui->setupUi(this);

    DbManager* db = new DbManager("/home/lamouski/Projects/TCN2020/test_db/tcn2020.db");

    db->checkDB();

    ui->m_week_view->setCurrientDate(QDate::currentDate());

    connect(ui->m_button_bookings, &QPushButton::clicked,
            [this](){ui->m_stacked_widget->setCurrentWidget(ui->m_week_view);});
    connect(ui->m_week_view->getReturnButton(), &QPushButton::clicked,
            [this](){ui->m_stacked_widget->setCurrentWidget(ui->m_main_view);});

}

MainWindow::~MainWindow()
{
    delete ui;
    if(m_members_list_dialog)
        delete m_members_list_dialog;
}

void MainWindow::on_m_button_members_clicked()
{
    if(!m_members_list_dialog)
        m_members_list_dialog = new MembersListDialog();
    m_members_list_dialog->show();
}



void MainWindow::on_delete_pushButton_clicked()
{
    DbManager* db = DbManager::instance();

    db->addMember("Denis", "Lamouski");
    db->addMember("Natallia", "Lamouskaya");
    db->addMember("Peter", "Mayer");
    db->addMember("Viktor", "Stadtler");
    db->addMember("Hans", "Koch");



}
