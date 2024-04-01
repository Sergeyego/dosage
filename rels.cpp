#include "rels.h"

Rels* Rels::rels_instance=0;

Rels *Rels::instance()
{
    if (rels_instance==0)
        rels_instance = new Rels();
    return rels_instance;
}

bool Rels::sendRequest(QString path, QString req, const QByteArray &data, QByteArray &respData)
{
    //QString authData = user+":"+password;
    QNetworkRequest request(QUrl("http://192.168.1.10:7000/"+path));
    request.setRawHeader("Accept","application/xml");
    request.setRawHeader("Accept-Charset", "UTF-8");
    request.setRawHeader("Content-Type", "application/xml");
    request.setRawHeader("User-Agent", "Appszsm");
    //request.setRawHeader("Authorization", "Basic "+authData.toUtf8().toBase64());
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
        reply=man.sendCustomRequest(request,req.toUtf8(),data);
    }
    if (!reply->isFinished()){
        loop.exec();
    }
    respData=reply->readAll();
    bool ok=(reply->error()==QNetworkReply::NoError);
    if (!ok){
         QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString()+"\n"+reply->readAll(),QMessageBox::Cancel);
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

    relCex = new DbSqlRelation("bunk_cex","id","nam",this);
    relCex->setSort("id desc");
    relCex->setFilter("bunk_cex.en=true");

    relMark = new DbSqlRelation("elrtr","id","marka",this);
    relMark->setSort("marka");
    relMark->setFilter("elrtr.id<>0");

    relBunk = new DbSqlRelation("bunk","id","numer",this);
    relBunk->setSort("bunk.nomer");
    relBunk->setFilter("bunk.id=0 or bunk.is_tiny=0");
}

