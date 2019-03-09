#ifndef BLOCKBOOKINGSVIEW_H
#define BLOCKBOOKINGSVIEW_H

#include <QPushButton>
#include <QSqlQueryModel>
#include <QWidget>

namespace Ui {
class BlockBookingsView;
}

class BlockBookingsModel;

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

protected slots:
    void handleCurrentBlockChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Ui::BlockBookingsView *ui;

    BlockBookingsModel *m_model_blocks = nullptr;
    BlockBookingsModel *m_model_bookings = nullptr;
};

#endif // BLOCKBOOKINGSVIEW_H
