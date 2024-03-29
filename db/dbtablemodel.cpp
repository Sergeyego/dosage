#include "dbtablemodel.h"


DbTableModel::DbTableModel(QString table, QObject *parent) :
    QAbstractTableModel(parent)
{
    tableName=table;
    modelData = new MData(this);
    editor = new DataEditor(modelData,this);
    block=false;
    pkList=QSqlDatabase::database().driver()->primaryIndex(tableName);
    defaultRecord=QSqlDatabase::database().driver()->record(tableName);
    //qDebug()<<pkList;
    //qDebug()<<defaultRecord;
}

Qt::ItemFlags DbTableModel::flags(const QModelIndex &index) const
{
    return modelData->column(index.column())->flags;
}

QVariant DbTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    QVariant value;
    QVariant origVal=modelData->value(index.row(),index.column()).val;
    QVariant::Type type=columnType(index.column());
    switch (role) {
        case Qt::DisplayRole:
            if (modelData->column(index.column())->sqlRelation){
                value = modelData->value(index.row(),index.column()).disp;
            } else {
                if (type==QVariant::Date){
                    value=origVal.toDate().toString("dd.MM.yy");
                } else if (type==QVariant::DateTime){
                    value=origVal.toDateTime().toString("dd.MM.yy hh:mm");
                } else if (type==QVariant::Double){
                    int dec=3;
                    if (modelData->column(index.column())->validator){
                        QDoubleValidator *doublevalidator = qobject_cast<QDoubleValidator*>(modelData->column(index.column())->validator);
                        if (doublevalidator) dec=doublevalidator->decimals();
                    }
                    value=(origVal.isNull() || origVal.toString().isEmpty())? QString("") : QLocale().toString(origVal.toDouble(),'f',dec);
                } else if (type==QVariant::Int) {
                    value=(origVal.isNull() || origVal.toString().isEmpty())? QString("") : QLocale().toString(origVal.toInt());
                } else if (type==QVariant::Bool){
                    value=origVal.toBool()? QString(QString::fromUtf8("Да")) : QString(QString::fromUtf8("Нет"));
                } else {
                    value=origVal;
                }
            }
            break;

        case Qt::EditRole:
            value=origVal;
            break;

        case Qt::TextAlignmentRole:
            value=((type==QVariant::Int || type==QVariant::Double || type==QVariant::LongLong) && !modelData->column(index.column())->sqlRelation)?
            int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
            break;

        case Qt::CheckStateRole:
            if (type==QVariant::Bool){
                value=(origVal.toBool())? Qt::Checked :  Qt::Unchecked;
            } else value=QVariant();
            break;

        default:
            value=QVariant();
            break;
    }
    return value;
}

int DbTableModel::rowCount(const QModelIndex& /*parent*/) const
{
    return modelData->rowCount();
}

int DbTableModel::columnCount(const QModelIndex& /*parent*/) const
{
    return modelData->columnCount();
}

bool DbTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!(this->flags(index) & Qt::ItemIsEditable)) return false;
    colVal setVal=modelData->value(index.row(),index.column());
    if (columnType(index.column())==QVariant::Bool){
        setVal.val=value.toBool();
    } else if(!data(index,Qt::CheckStateRole).isNull() && columnType(index.column())==QVariant::Int){
        setVal.val=value.toBool()? 1 : 0;
    } else {
        if (role==Qt::DisplayRole){
            setVal.disp=value.toString();
        } else {
            setVal.val=value;
        }
    }

    bool ok=false;
    ok=editor->edt(index.row(),index.column(),setVal);
    emit dataChanged(index,index);
    emit headerDataChanged(Qt::Vertical,index.row(),index.row());
    return ok;
}

QVariant DbTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        if (editor->isEdt() && section==editor->currentPos()) return QString("|");
        return (editor->isAdd() && (section==editor->currentPos()))? QString("*"): QString("  ");
    }
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return modelData->column(section)->display;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool DbTableModel::addColumn(QString name, QString display, DbSqlRelation *relation)
{
    QVariant emptyval=defaultRecord.value(name);
    QValidator *validator(nullptr);
    if (emptyval.type()==QVariant::Int || emptyval.type()==QVariant::LongLong){
        validator = new QIntValidator(this);
    } else if (emptyval.type()==QVariant::Double){
        QDoubleValidator *v = new QDoubleValidator(this);
        v->setDecimals(1);
        validator = v;
    }
    if (validator){
        validator->setLocale(QLocale::English);
    }
    col tmpColumn;
    tmpColumn.name=name;
    tmpColumn.display=display;
    tmpColumn.validator=validator;
    tmpColumn.data.resize(rowCount());
    tmpColumn.sqlRelation=relation;
    tmpColumn.flags=Qt::ItemIsEditable | Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    tmpColumn.defaultVal.val=emptyval.type()==QVariant::Date ? QDate::currentDate() : emptyval;
    modelData->addColumn(tmpColumn);
    return true;
}

bool DbTableModel::removeRow(int row, const QModelIndex& parent)
{
    revert();
    if (!rowCount() || row<0 || row>=rowCount() || (editor->isAdd() && rowCount()==1)) return false;
    QString dat;
    for(int i=0; i<columnCount(); i++) dat+=data(this->index(row,i)).toString()+", ";
    dat.truncate(dat.count()-2);
    int n=QMessageBox::question(nullptr,QString::fromUtf8("Подтвердите удаление"),
                                QString::fromUtf8("Подтветждаете удаление ")+dat+QString::fromUtf8("?"),QMessageBox::Yes| QMessageBox::No);
    bool ok=false;
    if (n==QMessageBox::Yes) {
        if (deleteDb(row)) {
            beginRemoveRows(parent,row,row);
            modelData->delRow(row);
            endRemoveRows();
            ok=true;
        }
    }
    if (ok){
        if (modelData->rowCount()<1) {
            this->insertRow(0);
        }
        emit sigUpd();
    }
    return ok;
}

void DbTableModel::setFilter(QString s)
{
    filter=s;
}

void DbTableModel::setSort(QString s)
{
    sort=s;
}

void DbTableModel::setSuffix(QString s)
{
    suffix=s;
}

bool DbTableModel::isAdd() const
{
    return editor->isAdd();
}

bool DbTableModel::isEdt() const
{
    return (editor->isEdt());
}

bool DbTableModel::isEmpty() const
{
    return (rowCount()==1 && isAdd()) || (rowCount()<1);
}

bool DbTableModel::insertRow(int /*row*/, const QModelIndex& /*parent*/)
{
    if (block) return false;
    bool ok=false;
    if (!editor->isAdd() && !editor->isEdt()){
        QVector<colVal> tmpRow;
        for (int i=0; i<columnCount();i++){
            tmpRow.push_back(modelData->column(i)->defaultVal);
        }
        beginInsertRows(QModelIndex(),rowCount(),rowCount());
        ok=editor->add(rowCount(),tmpRow);
        endInsertRows();
    }
    return ok;
}

DbSqlRelation *DbTableModel::sqlRelation(int column) const
{
    return modelData->column(column)->sqlRelation;
}

QVariant::Type DbTableModel::columnType(int column) const
{
    return nullVal(column).type();
}

QVariant DbTableModel::nullVal(int column) const
{
    return defaultRecord.value(modelData->column(column)->name);
}

int DbTableModel::currentEdtRow() const
{
    return editor->currentPos();
}

QValidator *DbTableModel::validator(int column) const
{
    return modelData->column(column)->validator;
}

void DbTableModel::setValidator(int column, QValidator *validator)
{
    if (modelData->column(column)->validator){
        modelData->column(column)->validator->deleteLater();
    }
    if (validator){
        validator->setLocale(QLocale::English);
    }
    modelData->column(column)->validator=validator;
}

void DbTableModel::setDefaultValue(int column, QVariant value)
{
    modelData->column(column)->defaultVal.val=value;
    if (modelData->column(column)->sqlRelation){
        modelData->column(column)->defaultVal.disp=modelData->column(column)->sqlRelation->getDisplayValue(value);
    }
}

void DbTableModel::setColumnFlags(int column, Qt::ItemFlags flags)
{
    modelData->column(column)->flags=flags;
}

QVariant DbTableModel::defaultValue(int column) const
{
    return modelData->column(column)->defaultVal.val;
}

bool DbTableModel::setDecimals(int column, int dec)
{
    bool ok=false;
    QValidator *validator=modelData->column(column)->validator;
    if (validator){
        QDoubleValidator *doublevalidator = qobject_cast<QDoubleValidator*>(validator);
        if (doublevalidator) {
            ok=true;
            doublevalidator->setDecimals(dec);
        }
    }
    return ok;
}

QString DbTableModel::name() const
{
    return tableName;
}

bool DbTableModel::insertDb()
{
    QSqlQuery query;
    QString qu;
    QString rows, vals, rets;
    QVector<colVal> tmpRow=editor->newRow();

    for (int i=0; i<modelData->columnCount(); i++){
        if(!tmpRow[i].val.isNull()) {
            if (!rows.isEmpty()){
                rows+=", ";
            }
            rows+=modelData->column(i)->name;

            if (!vals.isEmpty()){
                vals+=", ";
            }
            vals+=(":"+modelData->column(i)->name);
        }
        if (!rets.isEmpty()){
            rets+=", ";
        }
        rets+=modelData->column(i)->name;
    }

    qu="INSERT INTO "+tableName+" ( "+rows+" ) VALUES ( "+vals+" ) RETURNING "+rets;

    query.prepare(qu);
    for (int i=0; i<modelData->columnCount(); i++){
        if (!tmpRow[i].val.isNull()){
            query.bindValue(":"+modelData->column(i)->name,tmpRow.at(i).val);
        }
    }
    //qDebug()<<query.executedQuery();
    bool ok=query.exec();
    if (!ok) {
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    } else {
        while (query.next()){
            for (int i=0; i<tmpRow.size(); i++) {
                tmpRow[i].val=query.value(i);
                if (modelData->column(i)->sqlRelation){
                    tmpRow[i].disp=modelData->column(i)->sqlRelation->getDisplayValue(query.value(i));
                }
            }

        }
        modelData->setRow(tmpRow,editor->currentPos());
        int r = rowCount()-1;
        emit dataChanged(this->index(r,0),this->index(r,columnCount()-1));
        emit headerDataChanged(Qt::Vertical,r,r);
    }
    return ok;
}

bool DbTableModel::updateDb()
{
    QSqlQuery query;
    QVector<colVal> newRow=editor->newRow();
    QVector<colVal> oldRow=editor->oldRow();
    int r = editor->currentPos();
    if (newRow==oldRow) {
        emit headerDataChanged(Qt::Vertical,r,r);
        return true;
    }
    QString qu;
    QString sets;
    QString pkeys;
    QString rets;
    QVector<colVal> tmpRow=editor->newRow();

    for (int i=0; i<modelData->columnCount(); i++){
        if(newRow[i].val!=oldRow[i].val || (newRow[i].val.isNull() && !oldRow[i].val.isNull()) || (!newRow[i].val.isNull() && oldRow[i].val.isNull())){
            if (!sets.isEmpty()){
                sets+=", ";
            }
            sets+=modelData->column(i)->name+" = :new"+modelData->column(i)->name;
        }
        if (pkList.contains(modelData->column(i)->name)) {
            if (!pkeys.isEmpty()){
                pkeys+=" AND ";
            }
            pkeys+=(modelData->column(i)->name +" = :pk"+modelData->column(i)->name);
        }
        if (!rets.isEmpty()){
            rets+=", ";
        }
        rets+=modelData->column(i)->name;
    }

    qu="UPDATE "+tableName+" SET "+sets+" WHERE "+pkeys+" RETURNING "+rets;
    query.prepare(qu);
    for (int i=0; i<modelData->columnCount(); i++){
        if(newRow[i].val!=oldRow[i].val || (newRow[i].val.isNull() && !oldRow[i].val.isNull()) || (!newRow[i].val.isNull() && oldRow[i].val.isNull())){
            query.bindValue(":new"+modelData->column(i)->name,newRow[i].val);
        }
        if (pkList.contains(modelData->column(i)->name)) {
            query.bindValue(":pk"+modelData->column(i)->name,oldRow[i].val);
        }
    }

    //qDebug()<<query.executedQuery()<<" "<<qu;
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    } else {
        while (query.next()){
            for (int i=0; i<tmpRow.size(); i++){
                tmpRow[i].val=query.value(i);
                if (modelData->column(i)->sqlRelation){
                    tmpRow[i].disp=modelData->column(i)->sqlRelation->getDisplayValue(query.value(i));
                }
            }
        }
        modelData->setRow(tmpRow,editor->currentPos());
        emit dataChanged(this->index(r,0),this->index(r,columnCount()-1));
        emit headerDataChanged(Qt::Vertical,r,r);
    }
    return ok;
}

bool DbTableModel::deleteDb(int row)
{
    QSqlQuery query;
    QString qu;
    QString pkeys;

    for (int i=0; i<modelData->columnCount(); i++){
        if (pkList.contains(modelData->column(i)->name)) {
            if (!pkeys.isEmpty()){
                pkeys+=" AND ";
            }
            pkeys+=(modelData->column(i)->name +" = :pk"+modelData->column(i)->name);
        }
    }

    qu="DELETE FROM "+tableName+" WHERE "+pkeys;
    query.prepare(qu);

    for (int i=0; i<modelData->columnCount(); i++){
        if (pkList.contains(modelData->column(i)->name)) {
            query.bindValue(":pk"+modelData->column(i)->name,modelData->value(row,i).val);
        }
    }
    //qDebug()<<query.executedQuery()<<" "<<qu;
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
    return ok;
}

bool DbTableModel::select()
{
    QSqlQuery query;
    query.setForwardOnly(true);
    QString qu;
    QString cols;
    QString rels;

    for (int i=0; i<modelData->columnCount(); i++){
        if (!cols.isEmpty()){
            cols+=", ";
        }
        cols+=tableName+"."+modelData->column(i)->name;
    }

    for (int i=0; i<modelData->columnCount(); i++){
        if (!cols.isEmpty()){
            cols+=", ";
        }
        if (modelData->column(i)->sqlRelation){
            if (!modelData->column(i)->defaultVal.val.isNull()){
                modelData->column(i)->defaultVal.disp=modelData->column(i)->sqlRelation->getDisplayValue(modelData->column(i)->defaultVal.val);
            }
            cols+=modelData->column(i)->sqlRelation->getCDisplay();
        } else {
            cols+="NULL";
        }
    }

    qu="SELECT "+cols+" FROM "+tableName;

    for (int i=0; i<modelData->columnCount(); i++){
        if (modelData->column(i)->sqlRelation){
            if (!modelData->column(i)->sqlRelation->isInital()){
                modelData->column(i)->sqlRelation->refreshModel();
            }
            if (!rels.isEmpty()){
                rels+=" ";
            }
            rels+=modelData->column(i)->sqlRelation->joinStr(tableName,modelData->column(i)->name);
        }
    }
    if (!rels.isEmpty()) qu+=" "+rels;
    if (!suffix.isEmpty()) qu+=" "+suffix;
    if (!filter.isEmpty()) qu+=" WHERE "+filter;
    if (!sort.isEmpty()) qu+=" ORDER BY "+sort;

    query.prepare(qu);
    //qDebug()<<query.executedQuery()/*<<" "<<qu*/;
    beginResetModel();
    if (query.exec()){
        editor->esc();
        modelData->clear();
        while (query.next()){
            for (int i=0; i<modelData->columnCount(); i++){
                colVal c;
                c.val=query.value(i);
                c.disp=query.value(i+modelData->columnCount()).toString();
                modelData->column(i)->data.push_back(c);
            }
        }
    } else {
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
        return false;
    }
    int r = modelData->rowCount();
    if (!r) {
        QVector<colVal> tmpRow;
        for (int i=0; i<columnCount();i++){
            tmpRow.push_back(modelData->column(i)->defaultVal);
        }
        editor->add(0,tmpRow);
    }
    endResetModel();
    emit sigRefresh();
    return true;
}

void DbTableModel::revert()
{
    block=true;
    int r=editor->currentPos();
    if (editor->isAdd() && rowCount()>1){
        beginRemoveRows(QModelIndex(),r,r);
        editor->esc();
        endRemoveRows();
    } else if (editor->isEdt()){
        editor->esc();
        emit dataChanged(this->index(r,0),this->index(r,columnCount()-1));
        emit headerDataChanged(Qt::Vertical,r,r);
    }
    block=false;
}

bool DbTableModel::submit()
{
    if (block) return false;
    if (editor->isEdt()){
        if (editor->isAdd()){
            if (insertDb()) {
                editor->submit();
                emit sigUpd();
                //qDebug()<<"SUBMIT_ADD";
            }
        } else if (!editor->isAdd()){
            if (updateDb()){
                editor->submit();
                emit sigUpd();
                //qDebug()<<"SUBMIT_EDT";
            }
        }
    } else if (editor->isAdd()){
        revert();
    }
    return !(editor->isAdd() || editor->isEdt());
}

void DbTableModel::refreshRelsModel()
{
    for (int i=0; i<modelData->columnCount(); i++){
        if (modelData->column(i)->sqlRelation){
            modelData->column(i)->sqlRelation->refreshModel();
        }
    }
}

MData::MData(QObject *parent):QObject(parent)
{
}

void MData::addColumn(col &column)
{
    data.push_back(column);
}

void MData::setRow(QVector<colVal> &row, int pos)
{
    for (int i=0; i<data.size(); i++)
        data[i].data[pos]=row.at(i);
}

void MData::insertRow(int pos, QVector<colVal> &row)
{
    for (int i=0; i<data.size(); i++)
        data[i].data.insert(pos,row.at(i));
}

void MData::delRow(int pos)
{
    for (int i=0; i<data.size(); i++)
        data[i].data.remove(pos,1);
}

int MData::rowCount()
{
    return data.size()? data[0].data.size() : 0;
}

int MData::columnCount()
{
    return data.size();
}

colVal MData::value(int row, int column)
{
    return data[column].data[row];
}

col *MData::column(int c)
{
    return &data[c];
}

QVector<colVal> MData::row(int r) const
{
    QVector<colVal> tmp;
    for (int i=0; i<data.size(); i++)
        tmp.push_back(data[i].data[r]);
    return tmp;
}

void MData::setValue(const colVal &val, int row, int column)
{
    data[column].data[row]=val;
}

void MData::clear()
{
    for (int i=0; i<data.size(); i++)
        data[i].data.clear();
}


DataEditor::DataEditor(MData *dat, QObject *parent)
    :QObject(parent)
{
    mData=dat;
    pos=-1;
    addFlag=false;
    edtFlag=false;
}

bool DataEditor::add(int p, QVector<colVal> &row)
{
    bool ok=false;
    if (!addFlag && !edtFlag){
        mData->insertRow(p,row);
        pos=p;
        addFlag=true;
        ok=true;
    }
    return ok;
}

bool DataEditor::edt(int row, int col, colVal val)
{
    if((edtFlag && row!=pos) || (addFlag && row!=pos)) return false;
    if (!edtFlag){
        pos=row;
        saveRow=mData->row(row);
    }
    mData->setValue(val,row,col);
    edtFlag=true;
    return true;
}

void DataEditor::submit()
{
    addFlag=false;
    edtFlag=false;
    pos=-1;
}

void DataEditor::esc()
{
    if ((edtFlag && !addFlag) || (addFlag && mData->rowCount()==1 && edtFlag)){
        mData->setRow(saveRow,pos);
        edtFlag=false;
    } else if (addFlag && mData->rowCount()>1){
        mData->delRow(pos);
        addFlag=false;
        edtFlag=false;
    } else {
        edtFlag=false;
        addFlag=false;
        pos=-1;
    }
}

bool DataEditor::isAdd()
{
    return addFlag;
}

bool DataEditor::isEdt()
{
    return edtFlag;
}

int DataEditor::currentPos()
{
    return pos;
}

QVector<colVal> DataEditor::oldRow()
{
    return saveRow;
}

QVector<colVal> DataEditor::newRow()
{
    QVector<colVal> r;
    if (addFlag || edtFlag){
        for (int i=0; i<mData->columnCount(); i++){
            r.push_back(mData->column(i)->data.at(pos));
        }
    }
    return r;
}

DbSqlRelation::DbSqlRelation(QString tableRel, QString cKey, QString cDisplay, QObject *parent) : QObject(parent), table(tableRel), key(cKey), display(cDisplay)
{
    editable=false;
    alias=table;
    filterColumn=display;
    sort=getCDisplay();
    limModel = new DbSqlLikeModel(this,this);
    limModel->setLimit(-1);
    limModel->setAsync(true);
    currentFilterRegExp=QString();
}

QString DbSqlRelation::getTable(bool prefix)
{
    return (alias==table || !prefix)? table : table+" AS "+alias;
}

QString DbSqlRelation::getCKey(bool prefix)
{
    return prefix? (alias+"."+key) : key;
}

QString DbSqlRelation::getCDisplay(bool prefix)
{
    return prefix? (alias+"."+display) : display;
}

QString DbSqlRelation::getCFilter(bool prefix)
{
    return prefix? (alias+"."+filterColumn) : filterColumn;
}

QString DbSqlRelation::getSort()
{
    return sort;
}

QString DbSqlRelation::getFilter()
{
    return filter;
}

QString DbSqlRelation::getCurrentFilterRegExp()
{
    return currentFilterRegExp;
}

QString DbSqlRelation::joinStr(QString tablename, QString tablecol)
{
    return "LEFT JOIN "+getTable()+" ON "+getCKey()+" = "+tablename+"."+tablecol;
}

QString DbSqlRelation::getDisplayValue(QVariant key, QString column)
{
    QString s;
    QString scol = column.isEmpty()? getCDisplay() : getTable()+"."+column;
    QString qu="SELECT "+scol+" FROM "+getTable()+" WHERE "+getCKey()+" = :key";
    QSqlQuery query;
    query.prepare(qu);
    query.bindValue(":key",key);
    if (query.exec()){
        if (query.next()){
            s=query.value(0).toString();
        }
    } else {
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
    return s;
}

DbSqlLikeModel *DbSqlRelation::model()
{
    return limModel;
}

void DbSqlRelation::setAlias(QString a)
{
    alias=a;
    sort=getCDisplay();
}

void DbSqlRelation::setSort(QString s)
{
    sort=s;
}

void DbSqlRelation::setFilter(QString f)
{
    filter=f;
}

void DbSqlRelation::setFilterColumn(QString c)
{
    filterColumn=c;
}

void DbSqlRelation::setEditable(bool b)
{
    editable=b;
}

bool DbSqlRelation::isEditable()
{
    return editable;
}

bool DbSqlRelation::isInital()
{
    return limModel->isInital();
}

void DbSqlRelation::refreshModel()
{
    limModel->startSearch("");
}

void DbSqlRelation::setFilterRegExp(QString pattern)
{
    currentFilterRegExp=pattern;
    emit filterRegExpInstalled(pattern);
}

DbSqlLikeModel::DbSqlLikeModel(DbSqlRelation *r, QObject *parent) : QSortFilterProxyModel(parent), relation(r)
{
    origModel = new TableModel(this);
    limit=50;
    QStringList header;
    header<<"id"<<"name"<<"filter";
    origModel->setHeader(header);
    async=false;
    inital=false;
    setSourceModel(origModel);
    setFilterKeyColumn(2);
    setFilterRegExp(relation->getCurrentFilterRegExp());
    connect(relation,SIGNAL(filterRegExpInstalled(QString)),this,SLOT(setFilterRegExp(QString)));
}

void DbSqlLikeModel::setAsync(bool b)
{
    async=b;
}

void DbSqlLikeModel::setLimit(int l)
{
    limit=l;
}

TableModel *DbSqlLikeModel::originalModel()
{
    return origModel;
}

DbSqlRelation *DbSqlLikeModel::getRelation()
{
    return relation;
}

bool DbSqlLikeModel::isInital()
{
    return inital;
}

void DbSqlLikeModel::startSearch(QString s)
{
    emit searchRequested(s);
    QString lim;
    QString srt;
    QString flt;
    QString pattern = s;
    pattern.replace("'","''");

    if (!relation->getFilter().isEmpty() || !s.isEmpty()){
        flt+=" WHERE ";
        if (!relation->getFilter().isEmpty()){
            flt+=relation->getFilter();
            if (!s.isEmpty()){
                flt+=" AND ";
            }
        }
        if (!s.isEmpty()){
            flt+=relation->getCDisplay()+" ILIKE '"+pattern+"%'";
        }
    }

    if (limit>0){
        lim=QString(" LIMIT %1").arg(limit);
    }
    if (!relation->getSort().isEmpty()){
        srt+=" ORDER BY "+relation->getSort();
    }

    QString query="SELECT "+relation->getCKey()+", "+relation->getCDisplay()+", "+relation->getCFilter()+" FROM "+relation->getTable()+flt+srt+lim;

    if (async && inital){
        Executor *e = new Executor;
        e->setQuery(query);
        connect(e,SIGNAL(finished()),this,SLOT(queryFinished()));
        e->setProperty("path",s);
        e->start();
    } else {
        QSqlQuery qu;
        qu.prepare(query);
        if (qu.exec()){
            int colCount=qu.record().count();
            QVector<QVector<QVariant>> data;
            while (qu.next()){
                QVector<QVariant> row;
                for (int i=0; i<colCount; i++){
                    row.push_back(qu.value(i));
                }
                data.push_back(row);
            }
            origModel->setModelData(data);
        } else {
            invalidate();
            QMessageBox::critical(nullptr,tr("Error"),qu.lastError().text(),QMessageBox::Cancel);
        }
        inital=true;
        emit searchFinished(s);
    }
}

void DbSqlLikeModel::queryFinished()
{
    Executor *e = qobject_cast<Executor *>(sender());
    if (e){
        origModel->setModelData(e->getData());
        QString s=e->property("path").toString();
        inital=true;
        emit searchFinished(s);
        e->deleteLater();
    }
}
