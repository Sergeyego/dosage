#include "dialogolapflt.h"
#include "ui_dialogolapflt.h"

DialogOlapFlt::DialogOlapFlt(ProxyDataModel *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOlapFlt)
{
    ui->setupUi(this);
    proxyModel = m;
    for (int j=0; j<proxyModel->columnCount(); j++){
        QStringList itemList = proxyModel->getSourceVal(j);
        QStringList selItem = proxyModel->getSelectVal(j);
        FormFilter *f = new FormFilter(itemList,selItem,this);
        ui->tabWidget->addTab(f,proxyModel->headerData(j,Qt::Horizontal).toString());
        if (f->getEn()){
            ui->tabWidget->setCurrentIndex(j);
            ui->tabWidget->setTabText(j,"("+ui->tabWidget->tabText(j)+")");
        }
        connect(f,SIGNAL(enChanged(bool)),this,SLOT(enChangedSlot(bool)));
    }
}

DialogOlapFlt::~DialogOlapFlt()
{
    delete ui;
}

void DialogOlapFlt::accept()
{
    for (int i=0; i<ui->tabWidget->count(); i++){
        FormFilter *f = qobject_cast<FormFilter *>(ui->tabWidget->widget(i));
        if (f){
            proxyModel->setSelectVal(i,f->selectedItems());
        }
    }
    return QDialog::accept();
}

void DialogOlapFlt::enChangedSlot(bool c)
{
    FormFilter *f = qobject_cast<FormFilter *>(sender());
    if (f){
        for (int i=0; i<ui->tabWidget->count(); i++){
            if (ui->tabWidget->widget(i)==f){
                QString origTit = proxyModel->headerData(i,Qt::Horizontal).toString();
                QString title = c? "("+origTit+")" : origTit;
                ui->tabWidget->setTabText(i,title);
            }
        }
    }
}

