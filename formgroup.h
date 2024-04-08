#ifndef FORMGROUP_H
#define FORMGROUP_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "rels.h"

namespace Ui {
class FormGroup;
}

class ModelGroupEl : public DbTableModel
{
public:
    ModelGroupEl(QObject *parent = nullptr);
    bool insertRow(int row, const QModelIndex &parent);
    bool removeRow(int row, const QModelIndex &parent);
};

class FormGroup : public QWidget
{
    Q_OBJECT

public:
    explicit FormGroup(QWidget *parent = nullptr);
    ~FormGroup();

private:
    Ui::FormGroup *ui;
    ModelGroupEl *modelGroupEl;
};

#endif // FORMGROUP_H
