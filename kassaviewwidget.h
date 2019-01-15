#ifndef KASSAVIEWWIDGET_H
#define KASSAVIEWWIDGET_H

#include "daykassatablemodel.h"

#include <QPushButton>
#include <QWidget>

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

    DayKassaTableModel *m_revenue_model = nullptr;
    DayKassaTableModel *m_costs_model = nullptr;
private:
    Ui::KassaViewWidget *ui;
};

#endif // KASSAVIEWWIDGET_H
