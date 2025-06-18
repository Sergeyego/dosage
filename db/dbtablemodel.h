#ifndef DBTABLEMODEL_H
#define DBTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QValidator>
#include <QSortFilterProxyModel>
#include <QLocale>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlIndex>
#include "tablemodel.h"
#include "executor.h"

class DbSqlRelation;

class DbSqlLikeModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    DbSqlLikeModel(DbSqlRelation *r, QObject *parent = nullptr);
    void setAsync(bool b);
    void setLimit(int l);
    TableModel *originalModel();
    DbSqlRelation *getRelation();
    bool isInital();
    bool isAsync();

public slots:
    void startSearch(QString s);

private slots:
    void queryFinished();

private:
    DbSqlRelation *relation;
    int limit;
    bool async;
    bool inital;
    TableModel *origModel;
signals:
    void searchFinished(QString s);
};

class DbSqlRelation : public QObject
{
    Q_OBJECT
public:
    DbSqlRelation(QString tableRel, QString cKey, QString cDisplay, QObject *parent=nullptr);
    QString getTable(bool prefix=true);
    QString getCKey(bool prefix=true);
    QString getCDisplay(bool prefix=true);
    QString getCFilter(bool prefix=true);
    QString getSort();
    QString getFilter();
    QString getCurrentFilterRegExp();
    bool getAsyncSearch();
    QString joinStr(QString tablename, QString tablecol);
    QString getDisplayValue(QVariant key, QString column=QString());
    DbSqlLikeModel *model();
    void setAlias(QString a);
    void setSort(QString s);
    void setFilter(QString f);
    void setFilterColumn(QString c);
    void setEditable(bool b);
    void setAsyncSearch(bool b);
    bool isEditable();
    bool isInital();
private:
    QString table;
    QString alias;
    QString key;
    QString display;
    QString sort;
    QString filter;
    QString filterColumn;
    DbSqlLikeModel *limModel;
    QString currentFilterRegExp;
    bool editable;
    bool asyncSearch;
public slots:
    void refreshModel();
    void setFilterRegExp(QString pattern);
signals:
    void filterRegExpInstalled(QString pattern);
};

struct colVal
{
    QString disp;
    QVariant val;
    bool operator==(const colVal& rh) const {
        return (this->disp==rh.disp) && (this->val==rh.val);
    }
};


struct col
{
    QString name;
    QString display;
    QVector<colVal> data;
    QValidator *validator;
    Qt::ItemFlags flags;
    DbSqlRelation *sqlRelation;
    colVal defaultVal;
};


class MData : public QObject
{
    Q_OBJECT
public:
    MData(QObject *parent=nullptr);
    int rowCount();
    int columnCount();
    colVal value(int row, int column);
    col *column(int c);
    void addColumn(col &column);
    void clear();
    void delRow(int pos);
    void setRow(QVector<colVal> &row, int pos);
    QVector<colVal> row(int r) const;
protected:
    void insertRow(int pos, QVector<colVal> &row);
    void setValue(const colVal &val, int row, int column);
    friend class DataEditor;
private:
    QVector<col> data;
};

class DataEditor : public QObject
{
    Q_OBJECT
public:
    DataEditor(MData *dat, QObject *parent=nullptr);
    bool add(int p, QVector<colVal> &row);
    bool edt(int row, int col, colVal val);
    void submit();
    void esc();
    bool isAdd();
    bool isEdt();
    int currentPos();
    QVector<colVal> oldRow();
    QVector<colVal> newRow();
private:
    MData *mData;
    QVector<colVal> saveRow;
    bool addFlag;
    bool edtFlag;
    int pos;
};

class DbTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DbTableModel(QString table, QObject *parent=nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool addColumn(QString name, QString display, DbSqlRelation *relation=nullptr);
    virtual bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    void setFilter(QString s);
    void setSort(QString s);
    void setSuffix(QString s);
    bool isAdd() const;
    bool isEdt() const;
    bool isEmpty() const;
    bool isInsertable() const;
    virtual bool insertRow(int row, const QModelIndex &parent=QModelIndex());
    DbSqlRelation *sqlRelation(int column) const;
    QVariant::Type columnType(int column) const;
    QVariant nullVal(int column) const;
    int currentEdtRow() const;
    QValidator* validator(int column) const;
    void setValidator(int column, QValidator *validator);
    void setDefaultValue(int column, QVariant value);
    void setColumnFlags(int column, Qt::ItemFlags flags);
    QVariant defaultValue(int column) const;
    bool setDecimals(int column, int dec);
    void setInsertable(bool b);
    QString name() const;
    virtual void refreshRow(int row);

protected:
    QString tableName;
    QString filter;
    QString sort;
    QString suffix;
    virtual bool insertDb();
    virtual bool updateDb();
    virtual bool deleteDb(int row);
    
private:
    MData *modelData;
    DataEditor *editor;
    bool block;
    bool insertable;
    QSqlIndex pkList;
    QSqlRecord defaultRecord;

signals:
    void sigUpd();
    void sigRefresh();
    
public slots:
    virtual bool select();
    virtual void revert();
    virtual bool submit();
    void refreshRelsModel();
};

#endif // DBTABLEMODEL_H
