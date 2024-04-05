#include "formost.h"
#include "ui_formost.h"

FormOst::FormOst(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormOst)
{
    ui->setupUi(this);
    ui->pushButtonUpd->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));

    if (!Rels::instance()->relCex->model()->isInital()){
        Rels::instance()->relCex->refreshModel();
    }
    ui->comboBoxCex->setModel(Rels::instance()->relCex->model());
    ui->comboBoxCex->setEditable(false);

    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    modelOstData = new ModelOstData(this);
    ui->tableViewData->setModel(modelOstData);
    ui->tableViewData->setColumnHidden(0,true);
    ui->tableViewData->setColumnWidth(1,70);
    ui->tableViewData->setColumnWidth(2,210);
    ui->tableViewData->setColumnWidth(3,70);
    ui->tableViewData->setColumnWidth(4,70);

    modelOst = new ModelOst(this);
    ui->tableViewNakl->setModel(modelOst);
    ui->tableViewNakl->setColumnHidden(0,true);
    ui->tableViewNakl->setColumnHidden(1,true);
    ui->tableViewNakl->setColumnWidth(2,80);
    ui->tableViewNakl->setColumnWidth(3,80);

    mapper = new DbMapper(ui->tableViewNakl,this);
    mapper->addMapping(ui->lineEditNum,2);
    mapper->addMapping(ui->dateEditDat,3);
    mapper->addEmptyLock(ui->tableViewData);
    mapper->addLock(ui->comboBoxCex);
    mapper->addLock(ui->pushButtonUpd);

    ui->horizontalLayoutMap->insertWidget(0,mapper);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxCex,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updData(int)));

    upd();
}

FormOst::~FormOst()
{
    delete ui;
}

void FormOst::upd()
{
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    modelOst->refresh(id_cex,ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormOst::updData(int index)
{
    int id_ost=mapper->modelData(index,0).toInt();
    modelOstData->refresh(id_ost);
}

ModelOst::ModelOst(QWidget *parent) : DbTableModel("bunk_ost",parent)
{
    addColumn("id",tr("id"));
    addColumn("id_cex",tr("Цех"));
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));

    setSort("bunk_ost.dat, bunk_ost.num");
}

bool ModelOst::insertRow(int row, const QModelIndex &parent)
{
    select();
    if (rowCount()>0 && !isAdd()) {
        int old_num=this->data(this->index(rowCount()-1,2),Qt::EditRole).toInt();
        setDefaultValue(2,QString("%1").arg((old_num+1),3,'d',0,QChar('0')));
        setDefaultValue(3,QDate::currentDate());
    }
    return DbTableModel::insertRow(row,parent);
}

void ModelOst::refresh(int id_cex, QDate begDate, QDate endDate)
{
    QString filter=name()+".dat between '"+begDate.toString("yyyy-MM-dd")+"' and '"+endDate.toString("yyyy-MM-dd")+"' and "+name()+".id_cex = "+QString::number(id_cex);
    setFilter(filter);
    setDefaultValue(1,id_cex);
    select();
}

ModelOstData::ModelOstData(QWidget *parent) : DbTableModel("bunk_ost_data",parent)
{
    addColumn("id_ost",tr("id_ost"));
    addColumn("id_bunk",tr("Бункер"),Rels::instance()->relBunk);
    addColumn("id_comp",tr("Компонент"),Rels::instance()->relCmp);
    addColumn("parti",tr("Партия"));
    addColumn("kvo",tr("К-во, кг"));

    setSort("bunk.nomer");
}

void ModelOstData::refresh(int id_ost)
{
    setFilter("bunk_ost_data.id_ost="+QString::number(id_ost));
    setDefaultValue(0,id_ost);
    select();
}
