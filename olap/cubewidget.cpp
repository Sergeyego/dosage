#include "cubewidget.h"
#include "ui_cubewidget.h"

CubeWidget::CubeWidget(QString head, QStringList axes, QString qu, int dec, QWidget *parent) :
    QWidget(parent)
{
    inital(head,axes,qu,dec);
}

CubeWidget::CubeWidget(int id_cube, QWidget *parent) :
    QWidget(parent)
{
    QString nam, qu;
    QVariant cols;
    QStringList axes;
    int dec=3;
    QSqlQuery query;
    query.prepare("select nam, array_to_json(columns), query, dc from olaps where id = :id");
    query.bindValue(":id",id_cube);
    if (query.exec()){
        while (query.next()){
            nam=query.value(0).toString();
            cols=query.value(1);
            qu=query.value(2).toString();
            dec=query.value(3).toInt();
        }
        QVariantList list;
        QJsonDocument obj = QJsonDocument::fromJson(cols.toByteArray());
        if (obj.isArray()){
            list = obj.array().toVariantList();
        }
        for (QVariant v : list){
            axes.push_back(v.toString());
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Ok);
    }
    inital(nam,axes,qu,dec);
}

CubeWidget::~CubeWidget()
{
    delete ui;
}

void CubeWidget::setRange(QDate beg, QDate end, bool block)
{
    ui->dateEditBeg->setDate(beg);
    ui->dateEditEnd->setDate(end);
    ui->dateEditBeg->setReadOnly(block);
    ui->dateEditEnd->setReadOnly(block);
    updQuery();
}

void CubeWidget::setSum(double s)
{
    sum=s;
}

double CubeWidget::getSum()
{
    double s=0;
    int col=proxyModel->columnCount()-1;
    for (int i=0; i<proxyModel->rowCount(); i++){
        s+=proxyModel->data(proxyModel->index(i,col),Qt::EditRole).toDouble();
    }
    return s;
}

void CubeWidget::inital(QString head, QStringList axes, QString qu, int dec)
{
    ui = new Ui::CubeWidget;
    ui->setupUi(this);
    sum=0.0;
    query=qu;
    decimal=dec;
    ui->cmdUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->cmdSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    header=axes;
    header<<"Сумма";

    quModel = new QSqlQueryModel(this);
    proxyModel = new ProxyDataModel(this);
    proxyModel->setSourceModel(quModel);

    QCalendarWidget *begCalendarWidget = new QCalendarWidget(this);
    begCalendarWidget->setFirstDayOfWeek(Qt::Monday);
    ui->dateEditBeg->setCalendarWidget(begCalendarWidget);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    QCalendarWidget *endCalendarWidget = new QCalendarWidget(this);
    endCalendarWidget->setFirstDayOfWeek(Qt::Monday);
    ui->dateEditEnd->setCalendarWidget(endCalendarWidget);
    ui->dateEditEnd->setDate(QDate::currentDate());

    axisX = new AxisWidget(axes, this);
    axisY = new AxisWidget(axes, this);
    ui->groupBoxX->layout()->addWidget(axisX);
    ui->groupBoxY->layout()->addWidget(axisY);

    this->setWindowTitle(head);
    olapmodel = new OlapModel(axes,dec,this);
    ui->tableView->setModel(olapmodel);
    updQuery();
    connect(ui->cmdUpd,SIGNAL(clicked()),this,SLOT(updQuery()));
    connect(ui->radioButtonSum,SIGNAL(clicked(bool)),olapmodel,SLOT(setTypeSum(bool)));
    connect(ui->radioButtonAvg,SIGNAL(clicked(bool)),olapmodel,SLOT(setTypeAvg(bool)));
    connect(ui->radioButtonMin,SIGNAL(clicked(bool)),olapmodel,SLOT(setTypeMin(bool)));
    connect(ui->radioButtonMax,SIGNAL(clicked(bool)),olapmodel,SLOT(setTypeMax(bool)));
    connect(axisX,SIGNAL(sigUpd(QStringList)),olapmodel,SLOT(setX(QStringList)));
    connect(axisY,SIGNAL(sigUpd(QStringList)),olapmodel,SLOT(setY(QStringList)));
    connect(ui->cmdSave,SIGNAL(clicked()),this,SLOT(saveXls()));
    connect(olapmodel,SIGNAL(sigRefresh()),ui->tableView,SLOT(resizeToContents()));
    connect(ui->checkBoxFlt,SIGNAL(clicked(bool)),this,SLOT(fltEnable(bool)));
    connect(ui->cmdCfgFlt,SIGNAL(clicked(bool)),this,SLOT(cfgFlt()));
}

void CubeWidget::updQuery()
{
    QString squery=query;
    squery.replace(":d1","'"+ui->dateEditBeg->date().toString("yyyy-MM-dd")+"'");
    squery.replace(":d2","'"+ui->dateEditEnd->date().toString("yyyy-MM-dd")+"'");
    QSqlQuery qu;
    qu.prepare(squery);
    if (qu.exec()){
        quModel->setQuery(qu);
        for (int i=0; i<quModel->columnCount(); i++){
            quModel->setHeaderData(i,Qt::Horizontal,header.at(i));
        }
        upd();
    } else {
        QMessageBox::critical(this,tr("Ошибка"),qu.lastError().text(),QMessageBox::Ok);
    }
}

void CubeWidget::saveXls()
{
    ui->tableView->save(this->windowTitle()+QString::fromUtf8(" с ")+ui->dateEditBeg->date().toString("dd.MM.yy")+QString::fromUtf8(" по ")+ui->dateEditEnd->date().toString("dd.MM.yy"),decimal);
}

void CubeWidget::fltEnable(bool b)
{
    ui->cmdCfgFlt->setEnabled(b);
    proxyModel->setFilterEnabled(b);
    upd();
}

void CubeWidget::upd()
{
    data_cube d;
    double sumfact=0.0;
    if (sum>0){
        sumfact=getSum();
    }
    for (int i=0; i<proxyModel->rowCount(); i++){
        l_cube l;
        for (int j=0; j<proxyModel->columnCount(); j++){
            QVariant dt=proxyModel->data(proxyModel->index(i,j),Qt::EditRole);
            if (j!=proxyModel->columnCount()-1){
               l.dims.push_back(dt.toString()+'\n');
            } else {
                double s=dt.toDouble();
                if (sum>0 && sumfact!=0.0){
                    s=s*(sum/sumfact);
                }
                l.r=s;
            }
        }
        d.push_back(l);
    }
    olapmodel->setCubeData(d);
}

void CubeWidget::cfgFlt()
{
    DialogOlapFlt d(proxyModel);
    if (d.exec()==QDialog::Accepted){
        upd();
    }
}
