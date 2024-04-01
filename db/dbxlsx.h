#ifndef DBXLSX_H
#define DBXLSX_H

#include <QObject>
#include <QTableView>
#include "xlsx/xlsxdocument.h"
#include <QDir>
#include <QFileDialog>
#include <QHeaderView>
#include <QSettings>
#include "dbtablemodel.h"

using namespace QXlsx;

class DbXlsx : public QObject
{
    Q_OBJECT
public:
    explicit DbXlsx(QTableView *v, QString head, QObject *parent = nullptr);
private:
    QTableView *viewer;
    QString title;
public slots:
    void saveToFile();
};

#endif // DBXLSX_H
