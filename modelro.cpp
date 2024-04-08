#include "modelro.h"

ModelRo::ModelRo(QObject *parent) : QSqlQueryModel(parent)
{
    dec=2;
}

QVariant ModelRo::data(const QModelIndex &item, int role) const
{
    QVariant origData=QSqlQueryModel::data(item,Qt::EditRole);
    QVariant::Type type=origData.type();
    if (role==Qt::DisplayRole){
        if (type==QVariant::Double){
            return (origData.isNull() || origData==0) ? QString("") : QLocale().toString(origData.toDouble(),'f',mdecimal.value(item.column(),dec));
        } else if (type==QVariant::Date){
            return (origData.isNull()) ? QString("") : origData.toDate().toString("dd.MM.yy");
        } else if (type==QVariant::DateTime){
            return origData.toDateTime().toString("dd.MM.yy hh:mm");
        }
    } else if (role==Qt::TextAlignmentRole){
        return (type==QVariant::Int || type==QVariant::Double || type==QVariant::LongLong ) ?
        int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    return QSqlQueryModel::data(item,role);
}

bool ModelRo::execQuery(QSqlQuery &query)
{
    bool ok=query.exec();
    if (ok){
        setQuery(query);
    } else {
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
        clear();
    }
    return ok;
}

void ModelRo::setQuery(const QSqlQuery &query)
{
    QSqlQueryModel::setQuery(query);
    emit newQuery();
}

void ModelRo::setDecimal(int d)
{
    dec=d;
}

void ModelRo::setDecimalForColumn(int section, int d)
{
    mdecimal.insert(section,d);
}

void ModelRo::select()
{
    QSqlQuery q(query());
    execQuery(q);
}
