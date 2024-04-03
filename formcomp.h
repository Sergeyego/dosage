#ifndef FORMCOMP_H
#define FORMCOMP_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "rels.h"

namespace Ui {
class FormComp;
}

class FormComp : public QWidget
{
    Q_OBJECT

public:
    explicit FormComp(QWidget *parent = nullptr);
    ~FormComp();

private:
    Ui::FormComp *ui;
    DbTableModel *modelComp;
};

#endif // FORMCOMP_H
