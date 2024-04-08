#include "formrepdoz.h"
#include "ui_formrepdoz.h"

FormRepDoz::FormRepDoz(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepDoz)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());
    ui->pushButtonUpd->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));

    QStringList header;
    header<<tr("Компонент")<<tr("Партия")<<tr("Рецепт., кг")<<tr("Фактич., кг");
    modelRep = new TableModel(this);
    modelRep->setHeader(header);
    ui->tableView->setModel(modelRep);
    modelRep->setDecimal(2);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxPart,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
}

FormRepDoz::~FormRepDoz()
{
    delete ui;
}

void FormRepDoz::upd()
{
    QVector<QVector<QVariant>> data;
    QSqlQuery query;
    QString q;
    q=ui->checkBoxPart->isChecked()? QString("select m.nam, d.parti as part, sum(d.kvo_comp) as pln, sum(d.kvo_fact) as fct "
                       "from dosage_spnd d inner join matr m on m.id=d.id_comp "
                       "inner join dosage g on g.id = d.id_dos where g.dat between :d1 and :d2 "
                       "group by nam, part order by nam") :
               QString("select m.nam, NULL, sum(d.kvo_comp) as pln, sum(d.kvo_fact) as fct "
                       "from dosage_spnd d inner join matr m on m.id=d.id_comp "
                       "inner join dosage g on g.id = d.id_dos where g.dat between :d1 and :d2 "
                       "group by nam order by nam");
    query.prepare(q);
    query.bindValue(":d1",ui->dateEditBeg->date());
    query.bindValue(":d2",ui->dateEditEnd->date());
    double sumrcp=0.0;
    double sumfact=0.0;
    if (query.exec()){
        while (query.next()){
            QVector<QVariant> dt;
            for (int j=0; j<query.record().count(); j++){
                dt.push_back(query.value(j));
            }
            sumrcp+=dt.at(2).toDouble();
            sumfact+=dt.at(3).toDouble();
            data.push_back(dt);
        }
        QVector<QVariant> sdt;
        sdt.push_back(tr("Итого"));
        sdt.push_back(QString());
        sdt.push_back(sumrcp);
        sdt.push_back(sumfact);
        data.push_back(sdt);
    } else {
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
    modelRep->setModelData(data);
    ui->tableView->resizeToContents();
}

void FormRepDoz::save()
{
    QString header=tr("Расход компонентов на линии дозировки с %1 по %2 ").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));
    header+=ui->checkBoxPart->isChecked()? QString::fromUtf8("по партиям") : QString::fromUtf8("без партий");
    ui->tableView->save(header,2,true);
}
