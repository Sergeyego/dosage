#ifndef FORMOST_H
#define FORMOST_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "rels.h"

namespace Ui {
class FormOst;
}

class ModelOst : public DbTableModel
{    Q_OBJECT

 public:
     explicit ModelOst(QWidget *parent = nullptr);
     bool insertRow(int row, const QModelIndex &parent);
     void refresh(int id_cex, QDate begDate, QDate endDate);

};

class ModelOstData : public DbTableModel
{    Q_OBJECT

 public:
     explicit ModelOstData(QWidget *parent = nullptr);
     void refresh(int id_ost);

};

class FormOst : public QWidget
{
    Q_OBJECT

public:
    explicit FormOst(QWidget *parent = nullptr);
    ~FormOst();

private:
    Ui::FormOst *ui;
    ModelOst *modelOst;
    DbMapper *mapper;
    ModelOstData *modelOstData;

private slots:
    void upd();
    void updData(int index);
    void ins();
};

#endif // FORMOST_H
