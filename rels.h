#ifndef RELS_H
#define RELS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "db/dbtablemodel.h"

class Rels : public QObject
{
    Q_OBJECT
public:
    static Rels *instance();
    DbSqlRelation *relRcp;
    DbSqlRelation *relMark;
    DbSqlRelation *relCex;
    DbSqlRelation *relCmp;
    DbSqlRelation *relBunk;
    DbSqlRelation *relGrp;
    DbSqlRelation *relOp;
    DbSqlRelation *relCmpType;
    bool sendRequest(QString path, QString req, const QByteArray &data, QByteArray &respData);

protected:
    explicit Rels(QObject *parent = nullptr);

private:
    static Rels *rels_instance;

};

#endif // RELS_H
