#ifndef DBVIEWER_H
#define DBVIEWER_H

#include <QtGui>
#include "dbtablemodel.h"
#include "dbdelegate.h"
#include <QTableView>
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include "dbrelationeditdialog.h"

class QMenu;
class QAction;
class DbViewer : public QTableView
{
    Q_OBJECT
public:
    DbViewer(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model);
    void setColumnsWidth(QVector<int> width);

protected:
    virtual void keyPressEvent (QKeyEvent * e );
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    QAction *updAct;
    QAction *removeAct;
    bool menuEnabled;
    bool writeOk;

private slots:
     void upd();
     void remove();
     void submit(QModelIndex ind, QModelIndex oldInd);
     void focusOutEvent(QFocusEvent *event);
     void edtRel(QModelIndex index);

public slots:
     void setMenuEnabled(bool value);
};

#endif // DBVIEWER_H
