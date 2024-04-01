#include "dialognew.h"
#include "ui_dialognew.h"

DialogNew::DialogNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNew)
{
    ui->setupUi(this);
    new_id=-1;

    QStringList headerRcp;
    headerRcp<<tr("id")<<tr("Наименование");

    modelRcp = new TableModel(this);
    modelRcp->setHeader(headerRcp);
    ui->tableViewRcp->setModel(modelRcp);

    getRcp();

    /*modelDozNew = new ModelDozNew(this);
    ui->tableViewCont->setModel(modelDozNew);*/
    ui->comboBoxCex->setModel(Rels::instance()->relCex->model());

    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());
    ui->lineEditMas->setValidator(new QDoubleValidator(0,1000000,1,this));
    ui->lineEditMas->setText(QString("750"));

    /*ui->tableViewRcp->setModel(Rels::instance()->relRcp->model());
    Rels::instance()->relRcp->proxyModel()->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Рецептура"));
    ui->tableViewRcp->setColumnHidden(0,true);
    for (int i=2; i<ui->tableViewRcp->model()->columnCount(); i++){
        ui->tableViewRcp->setColumnHidden(i,true);
    }
    ui->tableViewRcp->setColumnWidth(1,150);*/

    //connect(ui->pushButtonCansel,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect(ui->pushButtonWrite,SIGNAL(clicked(bool)),this,SLOT(writeRcp()));
    connect(ui->pushButtonSelect,SIGNAL(clicked(bool)),this,SLOT(selectRcp()));
    connect(ui->pushButtonEsc,SIGNAL(clicked(bool)),this,SLOT(escRcp()));
}

DialogNew::~DialogNew()
{
    delete ui;
}

int DialogNew::getNewId()
{
    return new_id;
}

void DialogNew::getRcp()
{
    QByteArray data, respData;
    bool ok = Rels::instance()->sendRequest("dosage/recipes","GET",data,respData);
    if (ok){
        //qDebug()<<respData;
        QDomDocument domDoc;
        domDoc.setContent(respData);
        QDomElement root = domDoc.documentElement();
        qDebug()<<root.toDocument().toString();
    }
}

void DialogNew::selectRcp()
{
    //int id_cex=ui->comboBoxCex->model()->data(ui->comboBoxCex->model()->index(ui->comboBoxCex->currentIndex(),0),Qt::EditRole).toInt();

    //QDateTime dt;
    //dt.setDate(ui->dateEdit->date());
    //dt.setTime(ui->timeEdit->time());
    //QString smas=ui->lineEditMas->text().replace(",",".");
    //double mas=smas.toDouble();
    /*QModelIndex indexRcp=ui->tableViewRcp->model()->index(ui->tableViewRcp->currentIndex().row(),0);
    QModelIndex indexNam=ui->tableViewRcp->model()->index(ui->tableViewRcp->currentIndex().row(),1);
    int id_rcp=ui->tableViewRcp->model()->data(indexRcp,Qt::EditRole).toInt();
    ui->labelRcp->setText(ui->tableViewRcp->model()->data(indexNam,Qt::DisplayRole).toString());*/

    /*modelDozNew->refresh(mas,dt,id_rcp,id_cex);
    ui->tableViewCont->setColumnWidth(0,170);
    ui->tableViewCont->setColumnWidth(1,70);
    ui->tableViewCont->setColumnWidth(2,70);
    ui->tableViewCont->setColumnWidth(3,70);
    ui->tableViewCont->setColumnHidden(4,true);
    ui->tableViewCont->setColumnHidden(5,true);*/

    lock(true);
}

void DialogNew::writeRcp()
{
    //if (modelDozNew->writeRcp()){
        this->accept();
    //}
}

void DialogNew::escRcp()
{
    lock(false);
}

void DialogNew::lock(bool b)
{
    ui->pushButtonWrite->setEnabled(b);
    ui->pushButtonEsc->setEnabled(b);
    ui->pushButtonSelect->setEnabled(!b);
    ui->tableViewRcp->setEnabled(!b);
    ui->lineEditMas->setEnabled(!b);
    ui->dateEdit->setEnabled(!b);
    ui->timeEdit->setEnabled(!b);
    ui->comboBoxCex->setEnabled(!b);
}
