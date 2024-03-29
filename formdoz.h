#ifndef FORMDOZ_H
#define FORMDOZ_H

#include <QWidget>
#include <QAction>
#include "db/dbtablemodel.h"
#include "rels.h"


namespace Ui {
class FormDoz;
}

class ModelDoz : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelDoz(QWidget *parent = nullptr);
    void refresh(QDate beg, QDate end, bool srt);
};

class FormDoz : public QWidget
{
    Q_OBJECT

public:
    explicit FormDoz(QWidget *parent = nullptr);
    ~FormDoz();

private:
    Ui::FormDoz *ui;
    QAction *actionUpd;
    ModelDoz *modelDoz;

private slots:
    void upd();
};

#endif // FORMDOZ_H
