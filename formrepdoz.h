#ifndef FORMREPDOZ_H
#define FORMREPDOZ_H

#include <QWidget>
#include "db/tablemodel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlRecord>

namespace Ui {
class FormRepDoz;
}

class FormRepDoz : public QWidget
{
    Q_OBJECT

public:
    explicit FormRepDoz(QWidget *parent = nullptr);
    ~FormRepDoz();

private:
    Ui::FormRepDoz *ui;
    TableModel *modelRep;

private slots:
    void upd();
    void save();
};

#endif // FORMREPDOZ_H
