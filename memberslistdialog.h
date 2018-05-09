#ifndef MEMBERSLISTDIALOG_H
#define MEMBERSLISTDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class MembersListDialog;
}

class MembersListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MembersListDialog(QWidget *parent = 0);
    ~MembersListDialog();

private:
    Ui::MembersListDialog *ui;

    QSqlTableModel* m_model;
};

#endif // MEMBERSLISTDIALOG_H
