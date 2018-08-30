#ifndef PRICELISTDIALOG_H
#define PRICELISTDIALOG_H

#include <QDialog>

class QSqlTableModel;

namespace Ui {
class PriceListDialog;
}

class PriceListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PriceListDialog(QWidget *parent = 0);
    ~PriceListDialog();

private:
    Ui::PriceListDialog *ui;
    QSqlTableModel* m_model;
};

#endif // PRICELISTDIALOG_H
