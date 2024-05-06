#include "formbackfill.h"
#include "ui_formbackfill.h"

FormBackfill::FormBackfill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBackfill)
{
    ui->setupUi(this);
    ui->dateTimeEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    ui->pushButtonUpd->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonUpdStat->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->pushButtonUpdStatTime->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));

    sqlExecutor = new ProgressExecutor(this);

    if (!Rels::instance()->relCex->model()->isInital()){
        Rels::instance()->relCex->refreshModel();
    }
    ui->comboBoxCex->setModel(Rels::instance()->relCex->model());
    ui->comboBoxCex->setEditable(false);

    modelStatBunk = new ModelStatBunk(this);

    modelBunk = new QSortFilterProxyModel(this);
    modelBunk->setSourceModel(modelStatBunk);
    modelBunk->setFilterKeyColumn(5);
    modelBunk->setFilterFixedString("0");

    modelPart = new QSortFilterProxyModel(this);
    modelPart->setSourceModel(modelStatBunk);
    modelPart->setFilterKeyColumn(5);
    modelPart->setFilterFixedString("1");

    ui->tableViewBunk->setModel(modelBunk);
    ui->tableViewPart->setModel(modelPart);
    updStatTime();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelLoadBunk = new ModelLoadBunk(this);
    ui->tableViewLoad->setModel(modelLoadBunk);
    updLoad();
    ui->tableViewLoad->setColumnHidden(0,true);
    ui->tableViewLoad->setColumnWidth(1,90);
    ui->tableViewLoad->setColumnWidth(2,60);
    ui->tableViewLoad->setColumnWidth(3,80);
    ui->tableViewLoad->setColumnWidth(4,190);
    ui->tableViewLoad->setColumnWidth(5,70);
    ui->tableViewLoad->setColumnWidth(6,120);
    ui->tableViewLoad->setColumnWidth(7,100);
    ui->tableViewLoad->setColumnWidth(8,90);
    ui->tableViewLoad->setColumnHidden(9,true);

    connect(ui->pushButtonUpdStatTime,SIGNAL(clicked(bool)),this,SLOT(updStatTime()));
    connect(ui->pushButtonUpdStat,SIGNAL(clicked(bool)),this,SLOT(updStat()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updLoad()));
    connect(modelLoadBunk,SIGNAL(sigUpd()),this,SLOT(updStat()));
    connect(ui->comboBoxCex,SIGNAL(currentIndexChanged(QString)),this,SLOT(updLoad()));
    connect(ui->pushButtonPart,SIGNAL(clicked(bool)),this,SLOT(updPart()));
}

FormBackfill::~FormBackfill()
{
    delete ui;
}

void FormBackfill::updStat()
{
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    modelStatBunk->refresh(ui->dateTimeEdit->dateTime(),id_cex);
    ui->tableViewBunk->setColumnHidden(5,true);
    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->setColumnHidden(5,true);
    ui->tableViewBunk->resizeToContents();
    ui->tableViewPart->resizeToContents();
}

void FormBackfill::updStatTime()
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    updStat();
}

void FormBackfill::updLoad()
{
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    modelLoadBunk->refresh(ui->dateEditBeg->date(), ui->dateEditEnd->date(),id_cex);
    updStatTime();
    if (sender()==ui->pushButtonUpd){
        modelLoadBunk->refreshRelsModel();
    }
}

void FormBackfill::save()
{
    TableView v;
    v.setModel(modelStatBunk);
    v.setColumnHidden(5,true);
    v.verticalHeader()->setVisible(false);
    v.resizeToContents();
    v.save(QString::fromUtf8("Состояние бункеров на ")+ui->dateTimeEdit->dateTime().toString("dd.MM.yy HH.mm"));
}

void FormBackfill::updPart()
{
    if (ui->dateEditEnd->date().addDays(-3)>=ui->dateEditBeg->date()){
        QMessageBox::critical(this,QString::fromUtf8("Недопустимое действие"),QString::fromUtf8("Запрещено пересчитывать партии более чем за 3 дня"),QMessageBox::Cancel);
        return;
    }
    QString text=QString::fromUtf8("Будут обновлены партии на дозировках с %1 по %2.").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));
    int q = QMessageBox::question(this,QString::fromUtf8("Подтвердите действие"),text,QMessageBox::Ok,QMessageBox::Cancel);
    if (q==QMessageBox::Ok){
        QString query=QString("select * from calc_doz_parti_recalc('%1', '%2')").arg(ui->dateEditBeg->date().toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
        sqlExecutor->setQuery(query);
        sqlExecutor->start();
    }
}

ModelStatBunk::ModelStatBunk(QObject *parent) : ModelRo(parent)
{

}

void ModelStatBunk::refresh(QDateTime datetime, int id_cex)
{
    QSqlQuery query;
    query.prepare("select num, nam, parti, typ, dtm, is_tiny from calc_doz_stat_new(:dt, :id_cex) order by num, nam");
    query.bindValue(":dt",datetime);
    query.bindValue(":id_cex",id_cex);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,QString::fromUtf8("Бункер"));
        setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Компонент"));
        setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Партия"));
        setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Группа"));
        setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Дата засыпки"));
    }
}

ModelLoadBunk::ModelLoadBunk(QObject *parent): DbTableModel("bunk_comp",parent)
{
    addColumn("id",QString::fromUtf8("id"));
    addColumn("dat",QString::fromUtf8("Дата"));
    addColumn("tm",QString::fromUtf8("Время"));
    addColumn("id_bunk",QString::fromUtf8("Бункер"),Rels::instance()->relBunk);
    addColumn("id_comp",QString::fromUtf8("Компонент"),Rels::instance()->relCmp);
    addColumn("parti",QString::fromUtf8("Партия"));
    addColumn("id_grp",QString::fromUtf8("Группа"),Rels::instance()->relGrp);
    addColumn("id_op",QString::fromUtf8("Операция"),Rels::instance()->relOp);
    addColumn("kvo",QString::fromUtf8("Кол-во, кг"));
    addColumn("id_cex",QString::fromUtf8("Цех"),Rels::instance()->relCex);
    setSort("bunk_comp.dat, bunk_comp.tm");

    setDefaultValue(2,QTime::currentTime());
    setDefaultValue(7,1);
}

void ModelLoadBunk::refresh(QDate beg, QDate end, int id_cex)
{
    setDefaultValue(9,id_cex);
    setFilter("bunk_comp.dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"' and bunk_comp.id_cex = "+QString::number(id_cex));
    select();
}
