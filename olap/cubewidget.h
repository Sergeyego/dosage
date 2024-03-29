#ifndef CUBEWIDGET_H
#define CUBEWIDGET_H

#include <QWidget>
#include "olapmodel.h"
#include <QCalendarWidget>
#include "axiswidget.h"
#include <QSqlQueryModel>
#include "dialogolapflt.h"

namespace Ui {
class CubeWidget;
}

class CubeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CubeWidget(QString head, QStringList axes, QString qu, int dec, QWidget *parent = 0);
    explicit CubeWidget(int id_cube, QWidget *parent = 0);
    ~CubeWidget();
    void setRange(QDate beg, QDate end, bool block=true);
    void setSum(double s);
    double getSum();
    
private:
    void inital(QString head, QStringList axes, QString qu, int dec);
    Ui::CubeWidget *ui;
    OlapModel *olapmodel;
    QString query;
    AxisWidget *axisX;
    AxisWidget *axisY;
    QStringList header;
    int decimal;
    double sum;
    QSqlQueryModel *quModel;
    ProxyDataModel *proxyModel;

private slots:
    void updQuery();
    void saveXls();
    void fltEnable(bool b);
    void upd();
    void cfgFlt();
};

#endif // CUBEWIDGET_H
