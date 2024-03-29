#include "formdoz.h"
#include "ui_formdoz.h"

FormDoz::FormDoz(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDoz)
{
    ui->setupUi(this);

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    actionUpd = new QAction(this->style()->standardIcon(QStyle::SP_BrowserReload),tr("Обновить"),this);
    ui->toolButtonUpd->setDefaultAction(actionUpd);

    modelDoz = new ModelDoz(this);
    ui->tableViewDoz->setModel(modelDoz);
    ui->tableViewDoz->setColumnHidden(0,true);
    ui->tableViewDoz->setColumnWidth(1,70);
    ui->tableViewDoz->setColumnWidth(2,60);
    ui->tableViewDoz->setColumnWidth(3,140);
    ui->tableViewDoz->setColumnWidth(4,110);
    ui->tableViewDoz->setColumnWidth(5,60);
    ui->tableViewDoz->setColumnWidth(6,40);
    ui->tableViewDoz->setColumnWidth(7,60);

    connect(ui->radioButtonDate,SIGNAL(clicked(bool)),actionUpd,SLOT(trigger()));
    connect(ui->radioButtonNam,SIGNAL(clicked(bool)),actionUpd,SLOT(trigger()));

    connect(actionUpd,SIGNAL(triggered(bool)),this,SLOT(upd()));
    upd();
}

FormDoz::~FormDoz()
{
    delete ui;
}

void FormDoz::upd()
{
    modelDoz->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),ui->radioButtonDate->isChecked());
    ui->tableViewDoz->selectRow(modelDoz->rowCount()-1);
}

ModelDoz::ModelDoz(QWidget *parent) : DbTableModel("dosage",parent)
{
    addColumn("id",tr("id"));
    addColumn("dat",tr("Дата"));
    addColumn("tm",tr("Время"));
    addColumn("id_rcp",tr("Рецептура"),Rels::instance()->relRcp);
    addColumn("id_cex",tr("Цех"),Rels::instance()->relCex);
    addColumn("kvo_tot",tr("К-во, кг"));
    addColumn("result",tr("OK"));
    addColumn("parti",tr("Партия"));

    for (int i=0; i<columnCount()-1; i++){
        setColumnFlags(i,Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    }
}

void ModelDoz::refresh(QDate beg, QDate end, bool srt)
{
    QString f=name()+".dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'";
    setFilter(f);
    if (srt){
        setSort(name()+".dat, "+name()+".tm");
    } else {
        setSort("rcp_nam.nam,  "+name()+".dat, "+name()+".tm");
    }
    select();
}

