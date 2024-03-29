#include "dbmapper.h"

DbMapper::DbMapper(QAbstractItemView *v, QWidget *parent) :
    QWidget(parent)
{
    cmdNew = new QPushButton(QString::fromUtf8("Нов."),this);
    cmdWrite = new QPushButton(QString::fromUtf8("Запись"),this);
    cmdEdt = new QPushButton(QString::fromUtf8("Редакт."),this);
    cmdEsc = new QPushButton(QString::fromUtf8("Сброс"),this);
    cmdDel = new QPushButton(QString::fromUtf8("Удал."),this);
    cmdWrite->setEnabled(false);
    cmdEsc->setEnabled(false);
    cmdNew->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    cmdEdt->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
    cmdWrite->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    cmdDel->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    cmdEsc->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    mainLayout = new QHBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->addWidget(cmdEdt);
    mainLayout->addWidget(cmdNew);
    mainLayout->addWidget(cmdDel);
    mainLayout->addWidget(cmdWrite);
    mainLayout->addWidget(cmdEsc);

    viewer=v;
    viewer->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewer->setSelectionMode(QAbstractItemView::SingleSelection);
    viewer->setSelectionBehavior(QAbstractItemView::SelectRows);
    addLock(viewer);
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(v->model());
    DbDelegate *delegate = new DbDelegate(this);
    setItemDelegate(delegate);

    isEdt=false;
    defaultFocus=0;

    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(mapper->model());
    if (sqlModel){
        connect(sqlModel,SIGNAL(sigRefresh()),this,SLOT(last()));
        connect(sqlModel,SIGNAL(sigRefresh()),this,SLOT(checkEmpty()));
    }

    connect(cmdNew,SIGNAL(clicked()),this,SLOT(slotNew()));
    connect(cmdWrite,SIGNAL(clicked()),this,SLOT(slotWrite()));
    connect(cmdEdt,SIGNAL(clicked()),this,SLOT(slotEdt()));
    connect(cmdEsc,SIGNAL(clicked()),this,SLOT(slotEsc()));
    connect(cmdDel,SIGNAL(clicked()),this,SLOT(slotDel()));
    connect(mapper->itemDelegate(),SIGNAL(commitData(QWidget*)),this,SLOT(slotEdt()));
    connect(viewer->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),mapper,SLOT(setCurrentModelIndex(QModelIndex)));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SIGNAL(currentIndexChanged(int)));
    connect(delegate,SIGNAL(sigActionEdtRel(QModelIndex)),this,SLOT(edtRels(QModelIndex)));

}

DbMapper::~DbMapper()
{

}

void DbMapper::addLock(QWidget *widget)
{
    lock1.push_back(widget);
}

void DbMapper::addUnLock(QWidget *widget)
{
    lock2.push_back(widget);
}

void DbMapper::addEmptyLock(QWidget *widget)
{
    lockEmpty.push_back(widget);
}

void DbMapper::lock(bool val)
{
    isEdt=val;
    cmdDel->setEnabled(!val);
    cmdEdt->setEnabled(!val);
    cmdEsc->setEnabled(val);
    cmdNew->setEnabled(!val);
    cmdWrite->setEnabled(val);
    for (int i=0; i<lock1.size(); i++) lock1[i]->setEnabled(!val);
    for (int i=0; i<lock2.size(); i++) lock2[i]->setEnabled(val);
    for (int i=0; i<lockEmpty.size(); i++) lockEmpty[i]->setEnabled(!val);
    emit lockChanged(val);
}

void DbMapper::edtRels(QModelIndex index)
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(mapper->model());
    if (sqlModel){
        DbRelationEditDialog d(index);
        if (d.exec()==QDialog::Accepted){
            colVal c = d.currentData();
            sqlModel->setData(index,c.val,Qt::EditRole);
            sqlModel->setData(index,c.disp,Qt::DisplayRole);
        }
    }
}

void DbMapper::checkEmpty()
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(mapper->model());
    if (sqlModel) {
        bool val= sqlModel->rowCount()<=1 && sqlModel->isAdd();
        for (int i=0; i<lockEmpty.size(); i++) lockEmpty[i]->setEnabled(!val);
        cmdEdt->setEnabled(!val);
        cmdDel->setEnabled(!val);
        emit lockChanged(val);
    }
}

bool DbMapper::isLock()
{
    return (cmdWrite->isEnabled());
}

void DbMapper::addMapping(QWidget *widget, int section)
{
    QPalette pal=widget->palette();
    pal.setColor(QPalette::Disabled, QPalette::Text, pal.color(QPalette::Active,QPalette::Text));
    pal.setColor(QPalette::Disabled, QPalette::WindowText, pal.color(QPalette::Active,QPalette::WindowText));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, pal.color(QPalette::Active,QPalette::ButtonText));
    widget->setEnabled(false);
    widget->setPalette(pal);
    addUnLock(widget);
    mapper->addMapping(widget,section);
}

int DbMapper::currentIndex()
{
    return mapper->currentIndex();
}

void DbMapper::setDefaultFocus(int n)
{
    defaultFocus=n;
}

void DbMapper::setItemDelegate(QAbstractItemDelegate *delegate)
{
    mapper->setItemDelegate(delegate);
    DbDelegate *d = qobject_cast<DbDelegate *>(delegate);
    if (d){
        connect(d,SIGNAL(sigActionEdtRel(QModelIndex)),this,SLOT(edtRels(QModelIndex)));
    }
}

QVariant DbMapper::modelData(int row, int column)
{
    QAbstractItemModel *model = mapper->model();
    return model? model->data(mapper->model()->index(row,column),Qt::EditRole) : QVariant();
}

void DbMapper::refresh()
{
    mapper->setCurrentIndex(mapper->currentIndex());
}

void DbMapper::setCurrentViewRow(int row)
{
    int n=0;
    QTableView *table = qobject_cast<QTableView *> (viewer);
    if (table){
        bool hidden=table->isColumnHidden(n);
        while (hidden && n<table->model()->columnCount()){
            n++;
            hidden=table->isColumnHidden(n);
        }
    } else {
        QListView *listview = qobject_cast<QListView *> (viewer);
        if (listview){
            n=listview->modelColumn();
        }
    }
    viewer->setCurrentIndex(viewer->model()->index(row,n));
    viewer->scrollTo(viewer->currentIndex());
}


void DbMapper::slotNew()
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(mapper->model());
    if (sqlModel) {
        sqlModel->insertRow(sqlModel->rowCount());
        mapper->toLast();
        setCurrentViewRow(sqlModel->rowCount()-1);
        if (sqlModel->isAdd()) lock(true);
        if (!defaultFocus){
            for (int i=0; i<mapper->model()->columnCount(); i++){
                if (mapper->mappedWidgetAt(i)) {
                    mapper->mappedWidgetAt(i)->setFocus();
                    break;
                }
            }
        } else {
            if (mapper->mappedWidgetAt(defaultFocus)) mapper->mappedWidgetAt(defaultFocus)->setFocus();
        }
    }
}

void DbMapper::slotEdt()
{
    lock(true);
    /*if (!isEdt) {
        for (int i=0; i<mapper->model()->columnCount(); i++){
            if (mapper->mappedWidgetAt(i)) {
                mapper->mappedWidgetAt(i)->setFocus();
                break;
            }
        }
    }*/
}

void DbMapper::slotDel()
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(mapper->model());
    if (sqlModel) {
        sqlModel->removeRow(mapper->currentIndex());
    }
    checkEmpty();
}


void DbMapper::slotWrite()
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(mapper->model());
    if (sqlModel) {
        this->setFocus();
        bool ok=mapper->submit();
        if (ok) {
            lock(false);
        }
        mapper->setCurrentIndex(mapper->currentIndex());
        if (ok){
            emit sigWrite();
        }
    }
}

void DbMapper::slotEsc()
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(mapper->model());
    this->setFocus();
    if (sqlModel){
        if (sqlModel->isAdd()) {
            sqlModel->revert();
            setCurrentViewRow(sqlModel->rowCount()-1);
        } else if (sqlModel->isEdt()){
            sqlModel->revert();
        }
    }
    lock(false);
    mapper->setCurrentIndex(mapper->currentIndex());
    checkEmpty();
}

void DbMapper::first()
{
    mapper->toFirst();
    setCurrentViewRow(0);
}

void DbMapper::last()
{
    mapper->toLast();
    setCurrentViewRow(mapper->model()->rowCount()-1);
}
