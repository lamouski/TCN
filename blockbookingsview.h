#ifndef BLOCKBOOKINGSVIEW_H
#define BLOCKBOOKINGSVIEW_H

#include <QPushButton>
#include <QSqlQueryModel>
#include <QWidget>

namespace Ui {
class BlockBookingsView;
}

class BlockBookingsView : public QWidget
{
    Q_OBJECT

public:
    explicit BlockBookingsView(QWidget *parent = nullptr);
    ~BlockBookingsView();

    QPushButton *getReturnButton() const;

public slots:
    void fillData();
    void updateGUI();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    Ui::BlockBookingsView *ui;

    QSqlQueryModel *m_model = nullptr;
};

#endif // BLOCKBOOKINGSVIEW_H
