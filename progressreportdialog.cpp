#include "progressreportdialog.h"
#include "ui_progressreportdialog.h"

ProgressReportDialog::ProgressReportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressReportDialog)
{
    ui->setupUi(this);
}

ProgressReportDialog::~ProgressReportDialog()
{
    delete ui;
}

void ProgressReportDialog::closeEvent(QCloseEvent *pe)
{
    pe->ignore();
}
