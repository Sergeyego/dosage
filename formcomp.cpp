#include "formcomp.h"
#include "ui_formcomp.h"

FormComp::FormComp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormComp)
{
    ui->setupUi(this);
    modelComp = new DbTableModel("matr",this);
    modelComp->addColumn("id",tr("id"));
    modelComp->addColumn("nam",tr("Наименование"));
    modelComp->addColumn("loss_new",tr("К.потерь"));
    modelComp->addColumn("id_type",tr("Тип"),Rels::instance()->relCmpType);
    modelComp->addColumn("sort_order",tr("Приоритет доз."));

    modelComp->setSort("matr.nam");
    modelComp->select();

    ui->tableView->setModel(modelComp);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1,270);
    ui->tableView->setColumnWidth(2,80);
    ui->tableView->setColumnWidth(3,90);
    ui->tableView->setColumnWidth(4,110);

    connect(modelComp,SIGNAL(sigRefresh()),modelComp,SLOT(refreshRelsModel()));
    connect(modelComp,SIGNAL(sigUpd()),Rels::instance()->relCmp,SLOT(refreshModel()));
}

FormComp::~FormComp()
{
    delete ui;
}
