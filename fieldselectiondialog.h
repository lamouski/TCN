#ifndef FIELDSELECTIONDIALOG_H
#define FIELDSELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class FieldSelectionDialog;
}

class FieldSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FieldSelectionDialog(QWidget *parent = nullptr);
    ~FieldSelectionDialog();

    void setFields(const QStringList& texts);

    int selectedFieldRow() const;

private:
    Ui::FieldSelectionDialog *ui;
};

#endif // FIELDSELECTIONDIALOG_H
