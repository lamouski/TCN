#ifndef KASSAVIEWWIDGET_H
#define KASSAVIEWWIDGET_H

#include <QModelIndex>
#include <QPushButton>
#include <QWidget>

class QSqlRelationalTableModel;

namespace Ui {
class KassaViewWidget;
}

class KassaViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KassaViewWidget(QWidget *parent = nullptr);
    ~KassaViewWidget();

    QPushButton *getReturnButton() const;

protected:
    void updateGUI();

    void showEvent(QShowEvent *);

    void fillCurrentDay();

    QSqlRelationalTableModel  *m_revenue_model = nullptr;
    QSqlRelationalTableModel  *m_costs_model = nullptr;

private slots:
    void on_m_add_revenue_clicked();
    void on_m_add_cost_clicked();

    void edit_current_revenue();
    void delete_current_revenue();
    void handleCurrentRevenueChanged(const QModelIndex &current, const QModelIndex &previous);

    void handleCurrentExpenseChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Ui::KassaViewWidget *ui;

    QModelIndex m_edited_revenue_index;
    QModelIndex m_edited_expence_index;
};

#endif // KASSAVIEWWIDGET_H
