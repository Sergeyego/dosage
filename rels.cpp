#include "rels.h"

Rels* Rels::rels_instance=nullptr;

Rels *Rels::instance()
{
    if (rels_instance==nullptr)
        rels_instance = new Rels();
    return rels_instance;
}

bool Rels::sendRequest(QString path, QString req, const QByteArray &data, QByteArray &respData)
{
    QNetworkRequest request(QUrl("http://192.168.1.10:7000/"+path));
    request.setRawHeader("Accept","application/xml");
    request.setRawHeader("Accept-Charset", "UTF-8");
    request.setRawHeader("Content-Type", "application/xml");
    request.setRawHeader("User-Agent", "Appszsm");
    QEventLoop loop;
    QNetworkAccessManager man;
    connect(&man,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
    QNetworkReply *reply;
    if (req=="GET"){
        reply=man.get(request);
    } else if (req=="POST"){
        reply=man.post(request,data);
    } else if (req=="DELETE"){
        reply=man.deleteResource(request);
    } else {
        reply=man.sendCustomRequest(request,req.toUtf8()/*,data*/);
    }
    if (!reply->isFinished()){
        loop.exec();
    }
    respData=reply->readAll();
    bool ok=(reply->error()==QNetworkReply::NoError);
    if (!ok){
         QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString()+"\n"+respData,QMessageBox::Cancel);
    }
    reply->deleteLater();
    return ok;
}


Rels::Rels(QObject *parent) : QObject(parent)
{  
    relRcp = new DbSqlRelation("rcp_nam","id","nam",this);
    relRcp->setSort("nam");
    relRcp->setFilter("rcp_nam.lev=1");

    relCmp = new DbSqlRelation("matr","id","nam",this);
    relCmp->setSort("nam");
    QStringList header;
    header<<"id"<<tr("Компонент")<<"filter";
    relCmp->model()->originalModel()->setHeader(header);

    relCex = new DbSqlRelation("bunk_cex","id","nam",this);
    relCex->setSort("id desc");
    relCex->setFilter("bunk_cex.en=true");
    relCex->model()->setAsync(false);

    relMark = new DbSqlRelation("elrtr","id","marka",this);
    relMark->setSort("marka");
    relMark->setFilter("elrtr.id<>0");

    relBunk = new DbSqlRelation("bunk","id","numer",this);
    relBunk->setSort("bunk.nomer");
    relBunk->setFilter("bunk.id=0 or bunk.is_tiny=0");

    relGrp = new DbSqlRelation("el_types","id","nam",this);
    relGrp->setSort("el_types.nam");

    relOp = new DbSqlRelation("bunk_op","id","nam",this);
    relOp->setSort("bunk_op.id");

    relCmpType = new DbSqlRelation("matr_type","id","nam",this);
    relCmpType->setSort("matr_type.nam");
}

