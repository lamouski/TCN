#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class BookingDialog;
}

class BookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDialog(QWidget *parent = 0);
    ~BookingDialog();

    void reset();

public slots:
    void setField(const QString& fieldName);
    void setTimeslot(int timeSlot);
    void setDaysMask(int daysMask = 255);

    void setMemberId(int id);
    void setPriceId(int id);

    void updateMembersQuery(const QString &find_string);
    void updatePriceQuery();

public:
    int selectedId() const;    
    int selectedPrice() const;
    QString info() const;

private slots:
    void on_m_line_edit_name_textEdited(const QString &arg1);
    //void on_m_list_view_members_doubleClicked(const QModelIndex &index);
    void on_m_list_view_members_clicked(const QModelIndex &index);
    void on_m_list_view_members_activated(const QModelIndex &index);

    void handleCurrentMemberChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_m_combo_price_currentIndexChanged(int index);

private:
    Ui::BookingDialog *ui;

    QSqlQueryModel *m_memberlist_model;
    QString m_memberlist_base_query_string;

    QSqlQueryModel *m_prices_model;
    QString m_prices_base_query_string;

    int m_timeslot;
    int m_last_selected_member_id;
    int m_last_selected_price_id;
    int m_days_mask = 255;

    void selectCurrientId(const QModelIndex &index);
};

#endif // BOOKINGDIALOG_H
