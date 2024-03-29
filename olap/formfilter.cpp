#include "formfilter.h"
#include "ui_formfilter.h"


FormFilter::FormFilter(const QStringList &items, const QStringList &select, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormFilter)
{
    ui->setupUi(this);

    bool en=false;

    for (QString it : items){
        QListWidgetItem *item = new QListWidgetItem(it,ui->listWidget);
        bool isSel = select.contains(it);
        item->setData(Qt::CheckStateRole,isSel ? Qt::Checked : Qt::Unchecked);
        if (isSel){
            en=true;
        }
    }

    if (en){
        ui->radioButtonEn->setChecked(true);
        setEn();
    }

    connect(ui->radioButtonDis,SIGNAL(clicked(bool)),this,SLOT(setEn()));
    connect(ui->radioButtonEn,SIGNAL(clicked(bool)),this,SLOT(setEn()));
    connect(ui->toolButtonCheckAll,SIGNAL(clicked(bool)),this,SLOT(checkAll()));
    connect(ui->toolButtonUncheckAll,SIGNAL(clicked(bool)),this,SLOT(uncheckAll()));
}

FormFilter::~FormFilter()
{
    delete ui;
}

QStringList FormFilter::selectedItems()
{
    QStringList list;
    if (getEn()){
        for (int i=0; i<ui->listWidget->model()->rowCount(); i++){
            const QListWidgetItem *item=ui->listWidget->item(i);
            if (item->checkState()==Qt::Checked){
                list.push_back(item->text());
            }
        }
    }
    return list;
}

bool FormFilter::getEn()
{
    return ui->radioButtonEn->isChecked();
}

void FormFilter::checkAll()
{
    for (int i=0; i<ui->listWidget->model()->rowCount(); i++){
        ui->listWidget->item(i)->setData(Qt::CheckStateRole,Qt::Checked);
    }
}

void FormFilter::uncheckAll()
{
    for (int i=0; i<ui->listWidget->model()->rowCount(); i++){
        ui->listWidget->item(i)->setData(Qt::CheckStateRole,Qt::Unchecked);
    }
}

void FormFilter::setEn()
{
    bool en=getEn();
    ui->listWidget->setEnabled(en);
    ui->toolButtonCheckAll->setEnabled(en);
    ui->toolButtonUncheckAll->setEnabled(en);
    emit enChanged(en);
}
