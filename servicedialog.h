#ifndef SERVICEDIALOG_H
#define SERVICEDIALOG_H

#include <QDialog>

class QSqlTableModel;

namespace Ui {
class ServiceDialog;
}

class ServiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServiceDialog(QWidget *parent = 0);
    ~ServiceDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ServiceDialog *ui;

    QSqlTableModel *m_model;
};

#endif // SERVICEDIALOG_H
