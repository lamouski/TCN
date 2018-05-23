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

    void setMemberId(int id);
    void setPriceId(int id);

    void updateQuery(const QString &find_string);

public:
    int selectedId() const;

private slots:
    void on_m_line_edit_name_textEdited(const QString &arg1);
    //void on_m_list_view_members_doubleClicked(const QModelIndex &index);
    void on_m_list_view_members_clicked(const QModelIndex &index);
    void on_m_list_view_members_activated(const QModelIndex &index);

    void handleCurrentMemberChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Ui::BookingDialog *ui;

    QSqlQueryModel *m_model;

    int m_last_selected_id;

    void selectCurrientId(const QModelIndex &index);
};

#endif // BOOKINGDIALOG_H
