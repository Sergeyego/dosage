#include "olapmodel.h"

OlapModel::OlapModel(QStringList axes, int dec, QObject *parent) :
    QAbstractTableModel(parent)
{
    decimal=dec;
    tSum = new Sum(QString::fromUtf8("Сумма"),this);
    tAvg = new Avg(QString::fromUtf8("Среднее"),this);
    tMin = new Min(QString::fromUtf8("Минимум"),this);
    tMax = new Max(QString::fromUtf8("Максимум"),this);
    pTtl=tSum;
    hCube = new hyper_cube;
    n=axes.size();
    for(int i=0; i<n; i++){
        m_axe[axes[i]]=i;
    }
}

OlapModel::~OlapModel()
{
    delete hCube;
}

int OlapModel::rowCount(const QModelIndex& /*parent*/) const
{
    return (xJ.size() && yI.size())? hCube->Ny+xJ.size() : 0;
}

int OlapModel::columnCount(const QModelIndex& /*parent*/) const
{
    return (yI.size() && xJ.size())? hCube->Nx+yI.size() : 0;
}

QVariant OlapModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::DisplayRole){
        double val=hCube->dp[index.row()][index.column()];
        return val!=0 ? QLocale().toString(hCube->dp[index.row()][index.column()],'f',decimal) : QString("");
    } else if (role==Qt::EditRole){
        return hCube->dp[index.row()][index.column()];
    } else if (role==Qt::TextAlignmentRole){
        return int(Qt::AlignRight | Qt::AlignVCenter);
    }
    return QVariant();
}

QVariant OlapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QString ms;
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        if (section<hCube->pY->size()){
            ms=hCube->pY->key(section);
            ms.chop(1);
            ms=ms.replace(QChar('\n'),QChar(' '));
        } else if (section==hCube->pY->size()) ms=pTtl->getName();
        return ms;
    }
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section<hCube->pX->size()){
            ms=hCube->pX->key(section);
            ms.chop(1);
        } else if (section==hCube->pX->size()) ms=pTtl->getName();
        return ms;
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

void OlapModel::setCubeData(const data_cube &d)
{
    hCube->setData(d,n);
    refresh();
}

void OlapModel::setX(QStringList x)
{
    i_dims old=xJ;
    xJ.clear();
    for (int i=0; i<x.size(); ++i){
        xJ.push_back(m_axe[x.at(i)]);
    }
    if (xJ!=old) refresh();
}

void OlapModel::setY(QStringList y)
{
    i_dims old=yI;
    yI.clear();
    for (int i=0; i<y.size(); ++i){
        yI.push_back(m_axe[y.at(i)]);
    }
    if (yI!=old) refresh();
}

void OlapModel::refresh()
{
    beginResetModel();
    if( !(yI.empty()) && !(xJ.empty()) ) {
        hCube->make_proj(yI,xJ,pTtl);
    }
    endResetModel();
    emit headerDataChanged(Qt::Vertical,0,rowCount()-1);
    emit sigRefresh();
}

void OlapModel::setTypeSum(bool b)
{
    if (b){
        pTtl=tSum;
        refresh();
    }
}

void OlapModel::setTypeAvg(bool b)
{
    if (b){
        pTtl=tAvg;
        refresh();
    }
}

void OlapModel::setTypeMax(bool b)
{
    if (b){
        pTtl=tMax;
        refresh();
    }
}

void OlapModel::setTypeMin(bool b)
{
    if (b){
        pTtl=tMin;
        refresh();
    }
}

ProxyDataModel::ProxyDataModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    en=false;
}

bool ProxyDataModel::filterAcceptsRow(int source_row, const QModelIndex &/*source_parent*/) const
{
    bool b=true;
    if (en) {
        for (int j=0; j<sourceModel()->columnCount(); j++){
            QString dt=sourceModel()->data(sourceModel()->index(source_row,j),Qt::EditRole).toString();
            QStringList list = selection.value(j);
            b = b && (list.isEmpty() || list.contains(dt));
        }
    }
    return b;
}

QStringList ProxyDataModel::getSelectVal(int column)
{
    return selection.value(column);
}

QStringList ProxyDataModel::getSourceVal(int column)
{
    QStringList l;
    for (int i=0; i<sourceModel()->rowCount(); i++){
        QString dt=sourceModel()->data(sourceModel()->index(i,column),Qt::EditRole).toString();
        if (!l.contains(dt)){
            l.push_back(dt);
        }
    }
    std::sort(l.begin(),l.end());
    return l;
}

void ProxyDataModel::setSelectVal(int column, QStringList vals)
{
    beginResetModel();
    selection.remove(column);
    selection.insert(column,vals);
    endResetModel();
}

void ProxyDataModel::setFilterEnabled(bool b)
{
    beginResetModel();
    en=b;
    endResetModel();
}
