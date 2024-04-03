#include "formbackfillhist.h"
#include "ui_formbackfillhist.h"

FormBackfillHist::FormBackfillHist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBackfillHist)
{
    ui->setupUi(this);
    ui->dateTimeEdit->setDate(QDate::currentDate().addDays(-365));
    ui->dateTimeEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);

    if (!Rels::instance()->relCex->model()->isInital()){
        Rels::instance()->relCex->refreshModel();
    }
    ui->comboBoxCex->setModel(Rels::instance()->relCex->model());
    ui->comboBoxCex->setEditable(false);

    if (!Rels::instance()->relCmp->model()->isInital()){
        Rels::instance()->relCmp->refreshModel();
    }

    modelHist = new ModelRo(this);
    ui->tableViewHist->setModel(modelHist);

    ui->tableViewComp->setModel(Rels::instance()->relCmp->model());
    ui->tableViewComp->setColumnHidden(0,true);
    ui->tableViewComp->setColumnWidth(1,270);
    ui->tableViewComp->setColumnHidden(2,true);

    connect(ui->tableViewComp->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updHist()));
    connect(ui->dateTimeEdit,SIGNAL(dateTimeChanged(QDateTime)),this,SLOT(updHist()));
    connect(modelHist,SIGNAL(newQuery()),ui->tableViewHist,SLOT(resizeToContents()));
    connect(ui->comboBoxCex,SIGNAL(currentIndexChanged(int)),this,SLOT(updHist()));
}

FormBackfillHist::~FormBackfillHist()
{
    delete ui;
}

void FormBackfillHist::updHist()
{
    QModelIndex indComp=ui->tableViewComp->model()->index(ui->tableViewComp->currentIndex().row(),0);
    int id_comp=ui->tableViewComp->model()->data(indComp,Qt::EditRole).toInt();
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    QSqlQuery query;
    query.prepare("select bc.dtm as dtm, b.numer, bc.parti, tp.nam as typ, bo.nam from bunk_comp as bc "
                  "inner join bunk as b on b.id=bc.id_bunk "
                  "inner join matr as m on m.id=bc.id_comp "
                  "inner join el_types as tp on tp.id=bc.id_grp "
                  "inner join bunk_op as bo on bo.id=bc.id_op "
                  "where bc.dtm >= :dt and bc.id_comp = :id_comp and bc.id_cex= :id_cex "
                  "order by dtm desc");
    query.bindValue(":dt",ui->dateTimeEdit->dateTime());
    query.bindValue(":id_comp",id_comp);
    query.bindValue(":id_cex",id_cex);
    if (modelHist->execQuery(query)){
        modelHist->setHeaderData(0,Qt::Horizontal,QString::fromUtf8("Дата засыпки"));
        modelHist->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Бункер"));
        modelHist->setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Партия"));
        modelHist->setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Группа"));
        modelHist->setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Операция"));
    }
}
