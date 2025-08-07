#ifndef DBCOMBOBOX_H
#define DBCOMBOBOX_H

#include <QComboBox>
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QCompleter>
#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include "dbrelationeditdialog.h"
#include "dbtablemodel.h"
#include "dbviewer.h"

class CustomCompletter : public QCompleter
{
    Q_OBJECT
public:
    CustomCompletter(QObject *parent=nullptr);
    ~CustomCompletter();
    bool eventFilter(QObject *o, QEvent *e);
    void setModel(QAbstractItemModel *c);
    void setWidget(QWidget *widget);
private slots:
    void actComp(QString s);
    void setCurrentKey(QModelIndex index);
    void actFinished(QString s);
signals:
    void currentDataChanged(colVal d);
};

class DbComboBox : public QComboBox
{
    Q_OBJECT
public:
    DbComboBox(QWidget *parent=nullptr);
    colVal getCurrentData();
    ~DbComboBox();
    void setIndex(const QModelIndex &index);
    void setModel(QAbstractItemModel *model);
private:
    CustomCompletter *sqlCompleter;
    colVal currentData;
    colVal saveData;
    QAction *actionEdt;
    QModelIndex dbModelIndex;
    bool isReset;

signals:
    void sigActionEdtRel(const QModelIndex &index);

private slots:
    void indexChanged(int n);
    void edtRel();
    void updData();
    void mAboutReset();
    void mReset();

public slots:
    void setCurrentData(colVal data);
};

#endif // DBCOMBOBOX_H
