#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QLocale>
#include <QDate>
#include <QDebug>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:    
    TableModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool removeRow(int row, const QModelIndex &parent);
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setModelData(const QVector<QVector<QVariant>> &data, const QStringList &hdata = QStringList());
    void setHeader(const QStringList &hdata);
    QStringList getHeader() const;
    void clear();
    void setDecimal(int dec);
    void setDecimalForColumn(int section, int dec);


protected:
    QVector<QVector<QVariant>> p_d;
    QStringList p_header;
    int decimal;
    QMap<int,int> mdecimal;

signals:
    void sigUpd();
};

#endif // TABLEMODEL_H
