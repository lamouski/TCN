#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include "memberslistdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private slots:
    void on_m_button_members_clicked();

    void on_delete_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    MembersListDialog* m_members_list_dialog;

};

#endif // MAINWINDOW_H
