#include "axiswidget.h"
#include "ui_axiswidget.h"

AxisWidget::AxisWidget(QStringList axis, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxisWidget)
{
    ui->setupUi(this);
    ui->cmdUp->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp)));
    ui->cmdDown->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown)));

    for (int i=0; i<axis.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::EditRole,i);
        item->setData(Qt::DisplayRole,axis.at(i));
        item->setData(Qt::CheckStateRole,Qt::Unchecked);
        ui->listWidget->addItem(item);
    }
    connect(ui->listWidget,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(upd()));
    connect(ui->cmdUp,SIGNAL(clicked()),this,SLOT(up()));
    connect(ui->cmdDown,SIGNAL(clicked()),this,SLOT(down()));
}

AxisWidget::~AxisWidget()
{
    delete ui;
}

void AxisWidget::upd()
{
    QStringList sel;
    for (int i=0; i<ui->listWidget->count(); i++){
        if (ui->listWidget->item(i)->checkState()==Qt::Checked)
            sel.push_back(ui->listWidget->item(i)->data(Qt::DisplayRole).toString());
    }
    emit sigUpd(sel);
}

void AxisWidget::up()
{
    QListWidgetItem *sx,*sxm1;
    for(int x=1; x<ui->listWidget->count(); x++){
         if(ui->listWidget->item(x)->isSelected()){
            sxm1 = ui->listWidget->takeItem(x-1);
            sx = ui->listWidget->takeItem(x-1);
            ui->listWidget->insertItem(x-1,sxm1);
            ui->listWidget->insertItem(x-1,sx);
            ui->listWidget->item(x-1)->setSelected(true);
         }
    }
    upd();
}

void AxisWidget::down()
{
    QListWidgetItem *sx,*sxm1;
    for(int x=ui->listWidget->count()-2; x>=0; x--){
         if(ui->listWidget->item(x)->isSelected()){
            sxm1 = ui->listWidget->takeItem(x);
            sx = ui->listWidget->takeItem(x);
            ui->listWidget->insertItem(x,sxm1);
            ui->listWidget->insertItem(x,sx);
            ui->listWidget->item(x+1)->setSelected(true);
         }
    }
    upd();
}
