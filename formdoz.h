#ifndef FORMDOZ_H
#define FORMDOZ_H

#include <QWidget>
#include <QAction>
#include "db/dbtablemodel.h"
#include "rels.h"
#include "db/dbxlsx.h"
#include "dialognew.h"

namespace Ui {
class FormDoz;
}

class ModelDoz : public DbTableModel
{
    Q_OBJECT
public:
    explicit ModelDoz(QWidget *parent = nullptr);
    void refresh(QDate beg, QDate end, bool srt);
    QVariant data(const QModelIndex &index, int role) const;
    bool insertRow(int row, const QModelIndex &parent);
    void confirmDoz(int row);
protected:
    bool deleteDb(int row);
};

class ModelDozData : public DbTableModel
{
    Q_OBJECT
public:
    explicit ModelDozData(QWidget *parent = nullptr);
    void refresh(int id_doz);
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
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
    QAction *actionDel;
    QAction *actionConfirm;
    QAction *actionSave;
    QAction *actionNew;
    ModelDoz *modelDoz;
    ModelDozData *modelDozData;
    void loadSettings();
    void saveSettings();

private slots:
    void upd();
    void updDozData(QModelIndex index);
    void delDoz();
    void confirmDoz();
    void newDoz();
    void saveFile();
};

#endif // FORMDOZ_H
