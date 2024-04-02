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
    ui->tableViewRcp->setColumnHidden(0,true);
    ui->tableViewRcp->setColumnWidth(1,150);

    QStringList headerRcpData;
    headerRcpData<<tr("Компонент")<<tr("Кол-во, кг");
    modelRcpData = new TableModel(this);
    modelRcpData->setHeader(headerRcpData);
    modelRcpData->setDecimalForColumn(1,2);
    ui->tableViewCont->setModel(modelRcpData);

    QStringList headerDozData;
    headerDozData<<tr("Шаг")<<tr("Компонент")<<tr("Партия")<<tr("Бункер")<<tr("Кол-во, кг")<<tr("Обший, кг")<<tr("Маломер")<<tr("id_comp")<<tr("id-bunk");
    modelDozData = new TableModel(this);
    modelDozData->setHeader(headerDozData);
    modelDozData->setDecimalForColumn(4,2);
    modelDozData->setDecimalForColumn(5,2);

    if (!Rels::instance()->relCex->model()->isInital()){
        Rels::instance()->relCex->refreshModel();
    }
    ui->comboBoxCex->setModel(Rels::instance()->relCex->model());
    ui->comboBoxCex->setEditable(false);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());
    ui->lineEditMas->setValidator(new QDoubleValidator(0,1000000,1,this));
    ui->lineEditMas->setText(QString("750"));

    connect(ui->pushButtonWrite,SIGNAL(clicked(bool)),this,SLOT(writeRcp()));
    connect(ui->pushButtonSelect,SIGNAL(clicked(bool)),this,SLOT(selectRcp()));
    connect(ui->pushButtonEsc,SIGNAL(clicked(bool)),this,SLOT(escRcp()));
    connect(ui->tableViewRcp->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updRcpData(QModelIndex)));

    getRcp();
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
    QVector<QVector<QVariant>> modelData;
    QByteArray data, respData;
    bool ok = Rels::instance()->sendRequest("dosage/recipes","GET",data,respData);
    if (ok){
        QDomDocument domDoc;
        domDoc.setContent(respData);
        QDomElement root = domDoc.documentElement();
        QDomNodeList items = root.elementsByTagName("item");
        for (int i=0; i<items.count(); i++){
            QDomElement el = items.at(i).toElement();
            QVector<QVariant> dt;
            dt.push_back(getXmlVal(el,"id").toInt());
            dt.push_back(getXmlVal(el,"nam"));
            modelData.push_back(dt);
        }
    }
    modelRcp->setModelData(modelData);
    if (modelRcp->rowCount()){
        ui->tableViewRcp->selectRow(0);
    }
}

QString DialogNew::getXmlVal(QDomElement &el, QString nam)
{
    QString val;
    QDomNodeList l = el.elementsByTagName(nam);
    if (l.count()){
        val=l.at(0).toElement().text();
    }
    return val;
}

QDomElement DialogNew::newXmlElement(QString nam, QString val, QDomDocument *doc)
{
    QDomElement l = doc->createElement(nam);
    if (!val.isEmpty()){
        l.appendChild(doc->createTextNode(val));
    }
    return l;
}

void DialogNew::selectRcp()
{
    int id_rcp=ui->tableViewRcp->model()->data(ui->tableViewRcp->model()->index(ui->tableViewRcp->currentIndex().row(),0),Qt::EditRole).toInt();
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    QDateTime dt;
    dt.setDate(ui->dateEdit->date());
    dt.setTime(ui->timeEdit->time());
    QString smas=ui->lineEditMas->text().replace(",",".");
    double mas=smas.toDouble();

    QVector<QVector<QVariant>> modelData;
    QByteArray data, respData;
    bool ok = Rels::instance()->sendRequest("dosage/recipes/"+QString::number(id_cex)+"/"+QString::number(id_rcp)+"/"+QString::number(mas)+"?dtm="+dt.toString("yyyy-MM-ddThh:mm:ss"),"GET",data,respData);
    if (ok){
        QDomDocument domDoc;
        domDoc.setContent(respData);
        QDomElement root = domDoc.documentElement();
        QDomNodeList items = root.elementsByTagName("item");
        double sum=0.0;
        for (int i=0; i<items.count(); i++){
            QDomElement el = items.at(i).toElement();
            QVector<QVariant> dt;
            double kvo=getXmlVal(el,"kvo").toDouble();
            sum+=kvo;
            dt.push_back(i+1);
            dt.push_back(getXmlVal(el,"nam"));
            dt.push_back(getXmlVal(el,"parti"));
            dt.push_back(getXmlVal(el,"nbunk"));
            dt.push_back(kvo);
            dt.push_back(sum);
            dt.push_back(getXmlVal(el,"tiny").toInt());
            dt.push_back(getXmlVal(el,"id_matr").toInt());
            dt.push_back(getXmlVal(el,"id_bunk").toInt());
            modelData.push_back(dt);
        }
        modelDozData->setModelData(modelData);
        ui->tableViewCont->setModel(modelDozData);
        ui->tableViewCont->setColumnHidden(7,true);
        ui->tableViewCont->setColumnHidden(8,true);
        ui->tableViewCont->resizeToContents();
        lock(true);
    }
}

void DialogNew::writeRcp()
{
    if (!modelDozData->rowCount()){
        return;
    }
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\""));
    QDomElement root = doc.createElement("root");
    doc.appendChild(root);
    for (int i=0; i<modelDozData->rowCount(); i++){
        QDomElement item = doc.createElement("item");
        item.appendChild(newXmlElement("idmatr",modelDozData->data(modelDozData->index(i,7),Qt::EditRole).toString(),&doc));
        item.appendChild(newXmlElement("kvo",modelDozData->data(modelDozData->index(i,4),Qt::EditRole).toString(),&doc));
        item.appendChild(newXmlElement("idbunk",modelDozData->data(modelDozData->index(i,8),Qt::EditRole).toString(),&doc));
        item.appendChild(newXmlElement("parti",modelDozData->data(modelDozData->index(i,2),Qt::EditRole).toString(),&doc));
        root.appendChild(item);
    }

    int id_rcp=ui->tableViewRcp->model()->data(ui->tableViewRcp->model()->index(ui->tableViewRcp->currentIndex().row(),0),Qt::EditRole).toInt();
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    QDateTime dt;
    dt.setDate(ui->dateEdit->date());
    dt.setTime(ui->timeEdit->time());
    QString smas=ui->lineEditMas->text().replace(",",".");
    double mas=smas.toDouble();

    QByteArray data, respData;

    QTextStream stream( &data );
    stream.setCodec(QTextCodec::codecForName("UTF-8"));
    doc.save(stream,1);

    bool ok = Rels::instance()->sendRequest("dosage/recipes/"+QString::number(id_cex)+"/"+QString::number(id_rcp)+"/"+QString::number(mas)+"?dtm="+dt.toString("yyyy-MM-ddThh:mm:ss"),"POST",data,respData);
    if (ok){
        QDomDocument domDoc;
        domDoc.setContent(respData);
        QDomElement root = domDoc.documentElement();
        new_id=getXmlVal(root,"id").toInt();
        this->accept();
    }
}

void DialogNew::escRcp()
{
    lock(false);
    ui->tableViewCont->setModel(modelRcpData);
    ui->tableViewCont->resizeToContents();
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

void DialogNew::updRcpData(QModelIndex index)
{
    int id_rcp=ui->tableViewRcp->model()->data(ui->tableViewRcp->model()->index(index.row(),0),Qt::EditRole).toInt();
    ui->labelRcp->setText(ui->tableViewRcp->model()->data(ui->tableViewRcp->model()->index(index.row(),1),Qt::EditRole).toString());
    QVector<QVector<QVariant>> modelData;
    QByteArray data, respData;
    bool ok = Rels::instance()->sendRequest("dosage/recipes/"+QString::number(id_rcp),"GET",data,respData);
    if (ok){
        QDomDocument domDoc;
        domDoc.setContent(respData);
        QDomElement root = domDoc.documentElement();
        QDomNodeList items = root.elementsByTagName("item");
        for (int i=0; i<items.count(); i++){
            QDomElement el = items.at(i).toElement();
            QVector<QVariant> dt;
            dt.push_back(getXmlVal(el,"fnam"));
            dt.push_back(getXmlVal(el,"kvo").toDouble());
            modelData.push_back(dt);
        }
    }
    modelRcpData->setModelData(modelData);
    ui->tableViewCont->resizeToContents();
}
