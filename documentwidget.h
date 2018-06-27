#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include <QTextDocument>
#include <QWidget>

class DocumentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentWidget(QWidget *parent = 0);
    ~DocumentWidget();

public slots:
    void setHtml(const QString &html);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QTextDocument *doc;
};

#endif // DOCUMENTWIDGET_H
