#ifndef KASSAVIEWWIDGET_H
#define KASSAVIEWWIDGET_H

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

private:
    Ui::KassaViewWidget *ui;
};

#endif // KASSAVIEWWIDGET_H
