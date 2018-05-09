#ifndef MEMBERSLISTDIALOG_H
#define MEMBERSLISTDIALOG_H

#include <QDialog>

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
};

#endif // MEMBERSLISTDIALOG_H
