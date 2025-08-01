#include "formdoz.h"
#include "ui_formdoz.h"

FormDoz::FormDoz(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDoz)
{
    ui->setupUi(this);
    loadSettings();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    actionUpd = new QAction(this->style()->standardIcon(QStyle::SP_BrowserReload),tr("Обновить"),this);
    ui->toolButtonUpd->setDefaultAction(actionUpd);

    actionDel = new QAction(this->style()->standardIcon(QStyle::SP_TrashIcon),tr("Удалить"),this);
    ui->toolButtonDelete->setDefaultAction(actionDel);

    actionConfirm = new QAction(this->style()->standardIcon(QStyle::SP_DialogApplyButton),tr("Принять"),this);
    ui->toolButtonConfirm->setDefaultAction(actionConfirm);

    actionNew = new QAction(this->style()->standardIcon(QStyle::SP_FileIcon),tr("Новая"),this);
    ui->toolButtonNew->setDefaultAction(actionNew);

    actionSave = new QAction(this->style()->standardIcon(QStyle::SP_DialogSaveButton),tr("Сохранить в файл"),this);
    ui->toolButtonSave->setDefaultAction(actionSave);

    modelDoz = new ModelDoz(this);
    ui->tableViewDoz->setModel(modelDoz);
    ui->tableViewDoz->setColumnHidden(0,true);
    ui->tableViewDoz->setColumnWidth(1,65);
    ui->tableViewDoz->setColumnWidth(2,50);
    ui->tableViewDoz->setColumnWidth(3,140);
    ui->tableViewDoz->setColumnWidth(4,110);
    ui->tableViewDoz->setColumnWidth(5,60);
    ui->tableViewDoz->setColumnWidth(6,40);
    ui->tableViewDoz->setColumnWidth(7,60);

    modelDozData = new ModelDozData(this);
    ui->tableViewDozData->setModel(modelDozData);
    ui->tableViewDozData->setColumnHidden(0,true);
    ui->tableViewDozData->setColumnWidth(1,210);
    ui->tableViewDozData->setColumnWidth(2,60);
    ui->tableViewDozData->setColumnWidth(3,60);
    ui->tableViewDozData->setColumnWidth(4,60);
    ui->tableViewDozData->setColumnWidth(5,60);
    ui->tableViewDozData->setColumnWidth(6,135);
    ui->tableViewDozData->setColumnHidden(7,true);

    connect(ui->radioButtonDate,SIGNAL(clicked(bool)),actionUpd,SLOT(trigger()));
    connect(ui->radioButtonNam,SIGNAL(clicked(bool)),actionUpd,SLOT(trigger()));
    connect(ui->tableViewDoz->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updDozData(QModelIndex)));
    connect(ui->toolButtonUpd,SIGNAL(clicked(bool)),modelDozData,SLOT(refreshRelsModel()));

    connect(actionUpd,SIGNAL(triggered(bool)),this,SLOT(upd()));
    connect(actionDel,SIGNAL(triggered(bool)),this,SLOT(delDoz()));
    connect(actionConfirm,SIGNAL(triggered(bool)),this,SLOT(confirmDoz()));
    connect(actionNew,SIGNAL(triggered(bool)),this,SLOT(newDoz()));
    connect(actionSave,SIGNAL(triggered(bool)),this,SLOT(saveFile()));

    upd();
}

FormDoz::~FormDoz()
{
    saveSettings();
    delete ui;
}

void FormDoz::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->ui->splitter->restoreState(settings.value("doz_splitter_width").toByteArray());
}

void FormDoz::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("doz_splitter_width",ui->splitter->saveState());
}

void FormDoz::upd()
{
    modelDoz->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),ui->radioButtonDate->isChecked());
    ui->tableViewDoz->selectRow(modelDoz->rowCount()-1);
}

void FormDoz::updDozData(QModelIndex index)
{
    QModelIndex indexId=ui->tableViewDoz->model()->index(index.row(),0);
    int id_doz=ui->tableViewDoz->model()->data(indexId,Qt::EditRole).toInt();
    modelDozData->refresh(id_doz);
}

void FormDoz::delDoz()
{
    modelDoz->removeRow(ui->tableViewDoz->currentIndex().row());
}

void FormDoz::confirmDoz()
{
    int row=ui->tableViewDoz->currentIndex().row();
    QString dat;
    for(int i=0; i<modelDoz->columnCount(); i++){
        if (!dat.isEmpty()){
            dat+=", ";
        }
        dat+=modelDoz->data(modelDoz->index(row,i),Qt::DisplayRole).toString();
    }
    int n=QMessageBox::question(this,tr("Принять дозировку"), tr("Принять дозировку ")+dat+tr("?"),QMessageBox::Yes| QMessageBox::No);
    if (n==QMessageBox::Yes){
        modelDoz->confirmDoz(row);
        modelDozData->select();
    }
}

void FormDoz::newDoz()
{
    DialogNew d;
    if (d.exec()==QDialog::Accepted){
        upd();
        const int newId=d.getNewId();
        for(int i=ui->tableViewDoz->model()->rowCount()-1; i>=0; i--){
            QModelIndex ind=ui->tableViewDoz->model()->index(i,0);
            if (ui->tableViewDoz->model()->data(ind,Qt::EditRole).toInt()==newId){
                ui->tableViewDoz->selectRow(i);
            }
        }
    }
}

void FormDoz::saveFile()
{
    int row=ui->tableViewDoz->currentIndex().row();
    QString rcp=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,3),Qt::DisplayRole).toString();
    QString dat=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,1),Qt::EditRole).toDate().toString("dd.MM.yy");
    QString tim=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,2),Qt::EditRole).toTime().toString("hh:mm");
    QString kvo=QString::number(ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,5),Qt::EditRole).toDouble());
    DbXlsx s(ui->tableViewDozData,rcp+" "+dat+" "+tim+" "+kvo+tr(" кг"),this);
    s.saveToFile();
}

ModelDoz::ModelDoz(QWidget *parent) : DbTableModel("dosage",parent)
{
    addColumn("id",tr("id"));
    addColumn("dat",tr("Дата"));
    addColumn("tm",tr("Время"));
    addColumn("id_rcp",tr("Рецептура"),Rels::instance()->relRcp);
    addColumn("id_cex",tr("Цех"),Rels::instance()->relCex);
    addColumn("kvo_tot",tr("К-во, кг"));
    addColumn("result",tr("OK"));
    addColumn("parti",tr("Партия"));

    for (int i=0; i<columnCount()-2; i++){
        setColumnFlags(i,Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    }
    setColumnFlags(6,Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void ModelDoz::refresh(QDate beg, QDate end, bool srt)
{
    QString f=name()+".dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'";
    setFilter(f);
    if (srt){
        setSort(name()+".dat, "+name()+".tm");
    } else {
        setSort("rcp_nam.nam,  "+name()+".dat, "+name()+".tm");
    }
    select();
}

QVariant ModelDoz::data(const QModelIndex &index, int role) const
{
    if (index.column()==6){
        if (role==Qt::CheckStateRole){
            return this->data(index,Qt::EditRole).toBool() ? Qt::Checked : Qt::Unchecked;
        } else if (role==Qt::DisplayRole){
            return QVariant();
        }
    }
    return DbTableModel::data(index,role);
}

bool ModelDoz::insertRow(int /*row*/, const QModelIndex &/*parent*/)
{
    return false;
}

void ModelDoz::confirmDoz(int row)
{
    if (setData(index(row,6),1,Qt::EditRole)){
        if (submit()){
            int id_doz=data(index(row,0),Qt::EditRole).toInt();
            QSqlQuery query;
            query.prepare("update dosage_spnd set kvo_fact = kvo_comp where id_dos = :id ");
            query.bindValue(":id",id_doz);
            if (!query.exec()){
                QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
            }
        }
    }
}

bool ModelDoz::deleteDb(int row)
{
    int id_doz=data(index(row,0),Qt::EditRole).toInt();
    QSqlQuery query;
    query.prepare("delete from dosage_spnd where id_dos = :id ");
    query.bindValue(":id",id_doz);
    if (query.exec()){
        return DbTableModel::deleteDb(row);
    } else {
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
        return false;
    }
}

ModelDozData::ModelDozData(QWidget *parent) : DbTableModel("dosage_spnd",parent)
{
    addColumn("id_dos",tr("id_dos"));
    addColumn("id_comp",tr("Компонент"),Rels::instance()->relCmp);
    addColumn("id_bunk",tr("Бункер"),Rels::instance()->relBunk);
    addColumn("parti",tr("Партия"));
    addColumn("kvo_comp",tr("Рец., кг"));
    addColumn("kvo_fact",tr("Факт., кг"));
    addColumn("id_rcp",QString::fromUtf8("Рецепт. отходов"),Rels::instance()->relRcp);
    addColumn("parti_man",QString::fromUtf8("Руч. парт."));
    setSort("matr.nam");
    setDecimals(4,2);
    setDecimals(5,2);
}

void ModelDozData::refresh(int id_doz)
{
    setFilter("dosage_spnd.id_dos = "+QString::number(id_doz));
    setDefaultValue(0,id_doz);
    select();
}

QVariant ModelDozData::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole && index.column()==3 && data(this->index(index.row(),7),Qt::EditRole).toBool()){
        return QVariant(QColor(170,255,170));
    }
    return DbTableModel::data(index,role);
}

bool ModelDozData::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column()==3){
        setData(this->index(index.row(),7),true,Qt::EditRole);
    }
    return DbTableModel::setData(index,value,role);
}
