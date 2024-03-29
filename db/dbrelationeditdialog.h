#ifndef DBRELATIONEDITDIALOG_H
#define DBRELATIONEDITDIALOG_H

#include <QDialog>
#include "dbtablemodel.h"

namespace Ui {
class DbRelationEditDialog;
}

class DbRelationEditDialog : public QDialog
{
    Q_OBJECT

public:
    DbRelationEditDialog(const QModelIndex &dbIndex, QWidget *parent = 0);
    DbRelationEditDialog(DbSqlRelation *r, QWidget *parent = 0);
    ~DbRelationEditDialog();
    colVal currentData();
    void keyPressEvent(QKeyEvent *e);

private:
    void createModel(DbSqlRelation *r);
    Ui::DbRelationEditDialog *ui;
    DbTableModel *model;
};

#endif // DBRELATIONEDITDIALOG_H
