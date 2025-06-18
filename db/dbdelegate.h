#ifndef DBDELEGATE_H
#define DBDELEGATE_H
#include <QtGui>
#include <QtSql>
#include "dbtablemodel.h"
#include "dbcombobox.h"
#include "dbdateedit.h"
#include <QDateEdit>
#include <QCompleter>
#include <QCalendarWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QLayout>
#include <QItemDelegate>

class DbDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DbDelegate(QObject *parent=nullptr);
    QWidget *createEditor(
            QWidget *parent,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(
            QWidget *editor,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    bool eventFilter(QObject *object, QEvent *event);

signals:
    void createEdt(const QModelIndex index) const;
    void sigActionEdtRel(const QModelIndex index) const;
};

#endif // DBDELEGATE_H
