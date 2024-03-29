#include "rels.h"

Rels* Rels::rels_instance=0;

Rels *Rels::instance()
{
    if (rels_instance==0)
        rels_instance = new Rels();
    return rels_instance;
}


Rels::Rels(QObject *parent) : QObject(parent)
{  
    relRcp = new DbSqlRelation("rcp_nam","id","nam",this);
    relRcp->setSort("nam");
    relRcp->setFilter("rcp_nam.lev=1");

    relCex = new DbSqlRelation("bunk_cex","id","nam",this);
    relCex->setSort("id desc");
    relCex->setFilter("bunk_cex.en=true");

    relMark = new DbSqlRelation("elrtr","id","marka",this);
    relMark->setSort("marka");
    relMark->setFilter("elrtr.id<>0");
}

