#ifndef LOGINDATADIALOG_H
#define LOGINDATADIALOG_H

#include <QDialog>

namespace Ui {
class LoginDataDialog;
}

class LoginDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDataDialog(QWidget *parent = nullptr);
    ~LoginDataDialog();

    QString getLogin() const;
    QString getPassword() const;

private:
    Ui::LoginDataDialog *ui;
};

#endif // LOGINDATADIALOG_H
