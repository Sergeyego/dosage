#include "tablemodel.h"

TableModel::TableModel(QObject *parent): QAbstractTableModel(parent)
{
    decimal=1;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        return QVariant();
    }
    QVariant origData=p_d[index.row()][index.column()];
    QVariant::Type type=origData.type();
    if (role==Qt::DisplayRole){
        if (type==QVariant::Double){
            return (origData.isNull() || origData==0) ? QString("") : QLocale().toString(origData.toDouble(),'f',mdecimal.value(index.column(),decimal));
        } else if (type==QVariant::Date){
            return (origData.isNull()) ? QString("") : origData.toDate().toString("dd.MM.yy");
        }
    } else if (role==Qt::TextAlignmentRole){
        return (type==QVariant::Int || type==QVariant::Double || type==QVariant::LongLong ) ?
                    int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    if (role==Qt::EditRole || role == Qt::DisplayRole){
        return origData;
    }
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role==Qt::EditRole){
        p_d[index.row()][index.column()]=value;
        emit dataChanged(index,index);
        return true;
    }
    return false;
}

bool TableModel::removeRow(int row, const QModelIndex &/*parent*/)
{
    if (row>=0 && row<rowCount()){
        beginRemoveRows(QModelIndex(),row,row);
        p_d.remove(row);
        endRemoveRows();
        return true;
    }
    return false;
}

int TableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return p_d.size();
}

int TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return p_header.size();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && role==Qt::DisplayRole && section>=0 && section<columnCount()){
        return p_header.at(section);
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

void TableModel::setModelData(const QVector<QVector<QVariant> > &data, const QStringList &hdata)
{
    beginResetModel();
    p_d=data;
    if(!hdata.isEmpty()){
        p_header=hdata;
    }
    endResetModel();
    emit sigUpd();
}

void TableModel::setHeader(const QStringList &hdata)
{
    beginResetModel();
    p_header=hdata;
    endResetModel();
}

QStringList TableModel::getHeader() const
{
    return p_header;
}

void TableModel::clear()
{
    if (rowCount()){
        beginResetModel();
        p_d.clear();
        endResetModel();
    }
}

void TableModel::setDecimal(int dec)
{
    beginResetModel();
    decimal=dec;
    endResetModel();
}

void TableModel::setDecimalForColumn(int section, int dec)
{
    beginResetModel();
    mdecimal.insert(section,dec);
    endResetModel();
}
