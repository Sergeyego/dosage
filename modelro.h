#ifndef MODELRO_H
#define MODELRO_H

#include <QSqlQueryModel>
#include <QLocale>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

class ModelRo : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelRo(QObject *parent);
    QVariant data(const QModelIndex &item, int role) const;
    bool execQuery(QSqlQuery &query);
    void setQuery(const QSqlQuery &query);
    void setDecimal(int d);
    void setDecimalForColumn(int section, int d);
protected:
    int dec;
    QMap<int,int> mdecimal;
public slots:
    void select();
signals:
    void newQuery();
};

#endif // MODELRO_H
