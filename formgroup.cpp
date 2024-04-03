#include "formgroup.h"
#include "ui_formgroup.h"

FormGroup::FormGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormGroup)
{
    ui->setupUi(this);
    modelGroupEl = new ModelGroupEl(this);
    ui->tableView->setModel(modelGroupEl);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1,150);
    ui->tableView->setColumnWidth(2,150);
}

FormGroup::~FormGroup()
{
    delete ui;
}

ModelGroupEl::ModelGroupEl(QObject *parent) : DbTableModel("elrtr",parent)
{
    addColumn("id",tr("id"));
    addColumn("marka",tr("Марка"));
    addColumn("id_vid_doz",tr("Группа"),Rels::instance()->relGrp);
    setSort("elrtr.marka");
    setFilter("elrtr.id<>0");
    setColumnFlags(0,Qt::ItemIsSelectable  | Qt::ItemIsEnabled);
    setColumnFlags(1,Qt::ItemIsSelectable  | Qt::ItemIsEnabled);
    setColumnFlags(2,Qt::ItemIsEditable | Qt::ItemIsSelectable  | Qt::ItemIsEnabled);
    select();
    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshRelsModel()));
}

bool ModelGroupEl::insertRow(int /*row*/, const QModelIndex &/*parent*/)
{
    return false;
}

bool ModelGroupEl::removeRow(int /*row*/, const QModelIndex &/*parent*/)
{
    return false;
}
