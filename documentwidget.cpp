#include "documentwidget.h"

#include <QPainter>
#include <QTextDocument>

DocumentWidget::DocumentWidget(QWidget *parent) :
    QWidget(parent)
{
    doc = new QTextDocument(this);
    doc->setUndoRedoEnabled(false);
}

DocumentWidget::~DocumentWidget()
{
    delete doc;
}

void DocumentWidget::setHtml(const QString &html) {
     doc->setHtml(html);
     update();
}

void DocumentWidget::paintEvent(QPaintEvent * event) {
     QPainter p(this);
     p.setRenderHint(QPainter::Antialiasing, true);
     doc->drawContents(&p);
}

void DocumentWidget::resizeEvent(QResizeEvent * event) {
     QWidget::resizeEvent(event);
     doc->setTextWidth(width());
     update();
}
