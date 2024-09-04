#ifndef FORMREPORT_H
#define FORMREPORT_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include "db/modelro.h"

namespace Ui {
class FormReport;
}

class FormReport : public QWidget
{
    Q_OBJECT

public:
    explicit FormReport(QString title, QWidget *parent = nullptr);
    ~FormReport();

private:
    Ui::FormReport *ui;
    ModelRo *modelComp;
    ModelRo *modelDiff;
    ModelRo *modelRec;

    QSortFilterProxyModel *modelProxyDiff;
    QSortFilterProxyModel *modelProxyRec;

private slots:
    void updComp();
    void updDiff();
    void updRec();
};

#endif // FORMREPORT_H
