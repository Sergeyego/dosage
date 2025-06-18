#ifndef FORMREPPACK_H
#define FORMREPPACK_H

#include <QWidget>
#include <QProgressDialog>
#include "modelro.h"
#include "olap/cubewidget.h"
#include "formreport.h"

namespace Ui {
class FormRepPack;
}

class FormRepPack : public QWidget
{
    Q_OBJECT

public:
    explicit FormRepPack(QWidget *parent = nullptr);
    ~FormRepPack();

private:
    Ui::FormRepPack *ui;
    ModelRo *modelPart;
    bool addRasx(int row);
    bool calc_one();
    bool calc_all();
    bool calc();

private slots:
    void upd();
    void calc_sum();
    void clear_rasx();
    void showReportOlap();
    void showReportTbl();
};

#endif // FORMREPPACK_H
