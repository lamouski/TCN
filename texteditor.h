#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QWidget>

namespace Ui {
class TextEditor;
}

class TextEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = 0);
    ~TextEditor();

    void setHtml(const QString& html);

private slots:
    void on_m_button_export_clicked();

    void on_m_button_print_clicked();

private:
    Ui::TextEditor *ui;
};

#endif // TEXTEDITOR_H
