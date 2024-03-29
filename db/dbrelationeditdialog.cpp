#include "dbrelationeditdialog.h"
#include "ui_dbrelationeditdialog.h"

DbRelationEditDialog::DbRelationEditDialog(const QModelIndex &dbIndex, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbRelationEditDialog)
{
    ui->setupUi(this);
    const DbTableModel *sqlModel = qobject_cast<const DbTableModel *>(dbIndex.model());
    if (sqlModel){
        DbSqlRelation *r=sqlModel->sqlRelation(dbIndex.column());
        QString title = tr("Редактирование списка '")+(sqlModel->headerData(dbIndex.column(),Qt::Horizontal).toString())+"'";
        this->setWindowTitle(title);
        if (r){
            createModel(r);
        }
    }
}

DbRelationEditDialog::DbRelationEditDialog(DbSqlRelation *r, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbRelationEditDialog)
{
    ui->setupUi(this);
    if (r){
        createModel(r);
    }
}

DbRelationEditDialog::~DbRelationEditDialog()
{
    delete ui;
}

colVal DbRelationEditDialog::currentData()
{
    colVal c;
    QModelIndex ind=ui->tableView->currentIndex();
    if (model && ind.isValid()){
        c.val=ui->tableView->model()->data(ui->tableView->model()->index(ind.row(),0),Qt::EditRole);
        c.disp=ui->tableView->model()->data(ui->tableView->model()->index(ind.row(),1),Qt::EditRole).toString();
    }
    return c;
}

void DbRelationEditDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->text()=="\r"){
        e->ignore();
    } else {
        return QDialog::keyPressEvent(e);
    }
}

void DbRelationEditDialog::createModel(DbSqlRelation *r)
{
    model = new DbTableModel(r->getTable(false),this);
    model->addColumn(r->getCKey(false),tr("id"));
    model->addColumn(r->getCDisplay(false),tr("Название"));
    model->setFilter(r->getFilter());
    model->setSort(r->getSort());
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1,430);

    connect(model,SIGNAL(sigUpd()),r,SLOT(refreshModel()));

    if (ui->tableView->model()->rowCount()){
        ui->tableView->setCurrentIndex(ui->tableView->model()->index(ui->tableView->model()->rowCount()-1,1));
    }
}

