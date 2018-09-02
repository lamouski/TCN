#include "texteditor.h"
#include "ui_texteditor.h"

#include <QFileDialog>
#include <QPrintDialog>

#include <QtPrintSupport/QPrinter>

TextEditor::TextEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextEditor)
{
    ui->setupUi(this);
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::setHtml(const QString& html)
{
    ui->m_text_edit->setHtml(html);
}

void TextEditor::on_m_button_export_clicked()
{
#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    ui->m_text_edit->document()->print(&printer);
    //statusBar()->showMessage(tr("Exported \"%1\"")
    //                         .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif
}

void TextEditor::on_m_button_print_clicked()
{
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (ui->m_text_edit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        ui->m_text_edit->print(&printer);
    delete dlg;
#endif
}
