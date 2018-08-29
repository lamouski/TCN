#ifndef FIELDLISTDIALOG_H
#define FIELDLISTDIALOG_H

#include <QDialog>

class FieldsTableModel;

namespace Ui {
class FieldListDialog;
}

class FieldListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FieldListDialog(QWidget *parent = 0);
    ~FieldListDialog();

private:
    Ui::FieldListDialog *ui;

    FieldsTableModel *m_model;
};

#endif // FIELDLISTDIALOG_H
