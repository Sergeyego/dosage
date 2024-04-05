#include "formrepbunk.h"
#include "ui_formrepbunk.h"

FormRepBunk::FormRepBunk(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepBunk)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());
    ui->pushButtonUpd->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));

    if (!Rels::instance()->relCex->model()->isInital()){
        Rels::instance()->relCex->refreshModel();
    }
    ui->comboBoxCex->setModel(Rels::instance()->relCex->model());
    ui->comboBoxCex->setEditable(false);

    modelRep = new ModelRo(this);
    ui->tableView->setModel(modelRep);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
}

FormRepBunk::~FormRepBunk()
{
    delete ui;
}

void FormRepBunk::upd()
{
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    QSqlQuery query;
    query.prepare("select m.nam, r.part, b.numer, r.ostbeg, r.bas, r.dos, r.korr, r.ostend "
                  "from calc_bunk_report(:d1,:d2,:id_cex) as r "
                  "inner join matr as m on m.id=r.id_comp "
                  "inner join bunk as b on b.id=r.id_bunk "
                  "order by b.nomer, m.nam, r.part");
    query.bindValue(":d1",ui->dateEditBeg->date().addDays(-1));
    query.bindValue(":d2",ui->dateEditEnd->date());
    query.bindValue(":id_cex",id_cex);
    if (modelRep->execQuery(query)){
        modelRep->setHeaderData(0,Qt::Horizontal,tr("Компонент"));
        modelRep->setHeaderData(1,Qt::Horizontal,tr("Партия"));
        modelRep->setHeaderData(2,Qt::Horizontal,tr("Бункер"));
        modelRep->setHeaderData(3,Qt::Horizontal,tr("Нал.на нач."));
        modelRep->setHeaderData(4,Qt::Horizontal,tr("Засыпано"));
        modelRep->setHeaderData(5,Qt::Horizontal,tr("Отдозировано"));
        modelRep->setHeaderData(6,Qt::Horizontal,tr("Скорректир."));
        modelRep->setHeaderData(7,Qt::Horizontal,tr("Ост.на кон."));
    }
    ui->tableView->resizeToContents();
}

void FormRepBunk::save()
{
    QString header=tr("Отчет по бункерам с %1 по %2 ").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));
    header+=ui->comboBoxCex->getCurrentData().disp;
    ui->tableView->save(header,2,true);
}
