#include "formreport.h"
#include "ui_formreport.h"

FormReport::FormReport(QString title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReport)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->pushButtonSaveComp->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->pushButtonSaveDiff->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->pushButtonSaveRec->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));

    modelComp = new ModelRo(this);
    ui->tableViewComp->setModel(modelComp);
    ui->tableViewComp->setWindowTitle(title);

    modelDiff = new ModelRo(this);
    modelProxyDiff = new QSortFilterProxyModel(this);
    modelProxyDiff->setSourceModel(modelDiff);
    modelProxyDiff->setSortRole(Qt::EditRole);
    ui->tableViewDiff->setModel(modelProxyDiff);
    ui->tableViewDiff->setWindowTitle(tr("Сравнение данных опрессовки и упаковки ")+title.right(22));

    modelRec = new ModelRo(this);
    modelProxyRec = new QSortFilterProxyModel(this);
    modelProxyRec->setSourceModel(modelRec);
    modelProxyRec->setSortRole(Qt::EditRole);
    ui->tableViewRec->setModel(modelProxyRec);
    ui->tableViewRec->setWindowTitle(tr("Отходы шихты ")+title.right(22));

    connect(ui->radioButtonPart,SIGNAL(clicked(bool)),this,SLOT(updComp()));
    connect(ui->radioButtonMark,SIGNAL(clicked(bool)),this,SLOT(updComp()));
    connect(ui->radioButtonGroup,SIGNAL(clicked(bool)),this,SLOT(updComp()));
    connect(ui->radioButtonPartComp,SIGNAL(clicked(bool)),this,SLOT(updComp()));
    connect(ui->radioButtonComp,SIGNAL(clicked(bool)),this,SLOT(updComp()));
    connect(ui->pushButtonSaveComp,SIGNAL(clicked(bool)),ui->tableViewComp,SLOT(saveXLSX()));

    connect(ui->radioButtonDiffPart,SIGNAL(clicked(bool)),this,SLOT(updDiff()));
    connect(ui->radioButtonDiffMark,SIGNAL(clicked(bool)),this,SLOT(updDiff()));
    connect(ui->pushButtonSaveDiff,SIGNAL(clicked(bool)),ui->tableViewDiff,SLOT(saveXLSX()));

    connect(ui->radioButtonRecPart,SIGNAL(clicked(bool)),this,SLOT(updRec()));
    connect(ui->radioButtonRecMark,SIGNAL(clicked(bool)),this,SLOT(updRec()));
    connect(ui->pushButtonSaveRec,SIGNAL(clicked(bool)),ui->tableViewRec,SLOT(saveXLSX()));

    updComp();
    updDiff();
    updRec();
}

FormReport::~FormReport()
{
    delete ui;
}

void FormReport::updComp()
{
    QSqlQuery query;
    QStringList header;
    if (ui->radioButtonPart->isChecked()){
        query.prepare("select p2.n_s||'-'||p2.yea as parti, e.marka as marka, cast(p2.diam as varchar(3)) as diam, "
                      "CASE WHEN p2.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' end as var, "
                      "et.nam as grp, coalesce(p.nam||' ф '||d2.sdim, m.nam) as comp, rn.cmp_part as comp_part, "
                      "rn.prod,rn.brk,rn.loss_drb,rn.loss_prs,rn.prs,rn.dry,rn.real_return "
                      "from tmp_report_new rn "
                      "left join matr m on m.id = rn.id_matr and rn.id_dim=-1 "
                      "left join provol p on p.id = rn.id_matr and rn.id_dim<>-1 "
                      "left join diam d2 on d2.id=rn.id_dim and rn.id_dim<>-1 "
                      "inner join parti p2 on p2.id=rn.id_part "
                      "inner join elrtr_vars ev on ev.id = p2.id_var "
                      "inner join elrtr e on e.id=p2.id_el "
                      "inner join el_types et on et.id = e.id_vid "
                      "order by parti, m.cod, marka, diam, var");
        header<<tr("Партия")<<tr("Марка")<<tr("Ф")<<tr("Вариант")<<tr("Группа")<<tr("Компонент")<<tr("Парт.комп.")
             <<tr("Упаков.")<<tr("В брак")<<tr("Дроб.руб.")<<tr("Безв.пот.")<<tr("Возв.пот.")<<tr("Шихт.брак")<<tr("Возврат");
    } else if (ui->radioButtonMark->isChecked()){
        query.prepare("select z.marka, z.diam, z.var, z.grp, z.comp, z.comp_part, "
                      "sum(prod), sum(brk), sum(loss_drb), sum(loss_prs), sum(prs), sum(dry), sum(real_return) "
                      "from( "
                      "select p2.n_s||'-'||p2.yea as parti, e.marka as marka, cast(p2.diam as varchar(3)) as diam, "
                      "CASE WHEN p2.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' end as var, "
                      "et.nam as grp, coalesce(p.nam||' ф '||d2.sdim, m.nam) as comp, rn.cmp_part as comp_part, "
                      "rn.prod,rn.brk,rn.loss_drb,rn.loss_prs,rn.prs,rn.dry,rn.real_return, m.cod "
                      "from tmp_report_new rn "
                      "left join matr m on m.id = rn.id_matr and rn.id_dim=-1 "
                      "left join provol p on p.id = rn.id_matr and rn.id_dim<>-1 "
                      "left join diam d2 on d2.id=rn.id_dim and rn.id_dim<>-1 "
                      "inner join parti p2 on p2.id=rn.id_part "
                      "inner join elrtr_vars ev on ev.id = p2.id_var "
                      "inner join elrtr e on e.id=p2.id_el "
                      "inner join el_types et on et.id = e.id_vid "
                      ") as z "
                      "group by z.marka, z.diam, z.var, z.grp, z.comp, z.comp_part, z.cod "
                      "order by z.marka, z.diam, z.var, z.grp, z.cod, z.comp, z.comp_part");
        header<<tr("Марка")<<tr("Ф")<<tr("Вариант")<<tr("Группа")<<tr("Компонент")<<tr("Парт.комп.")
             <<tr("Упаков.")<<tr("В брак")<<tr("Дроб.руб.")<<tr("Безв.пот.")<<tr("Возв.пот.")<<tr("Шихт.брак")<<tr("Возврат");
    } else if (ui->radioButtonGroup->isChecked()){
        query.prepare("select z.grp, z.comp, z.comp_part, "
                      "sum(prod), sum(brk), sum(loss_drb), sum(loss_prs), sum(prs), sum(dry), sum(real_return) "
                      "from( "
                      "select p2.n_s||'-'||p2.yea as parti, e.marka as marka, cast(p2.diam as varchar(3)) as diam, "
                      "CASE WHEN p2.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' end as var, "
                      "et.nam as grp, coalesce(p.nam||' ф '||d2.sdim, m.nam) as comp, rn.cmp_part as comp_part, "
                      "rn.prod,rn.brk,rn.loss_drb,rn.loss_prs,rn.prs,rn.dry,rn.real_return, m.cod "
                      "from tmp_report_new rn "
                      "left join matr m on m.id = rn.id_matr and rn.id_dim=-1 "
                      "left join provol p on p.id = rn.id_matr and rn.id_dim<>-1 "
                      "left join diam d2 on d2.id=rn.id_dim and rn.id_dim<>-1 "
                      "inner join parti p2 on p2.id=rn.id_part "
                      "inner join elrtr_vars ev on ev.id = p2.id_var "
                      "inner join elrtr e on e.id=p2.id_el "
                      "inner join el_types et on et.id = e.id_vid "
                      ") as z "
                      "group by z.grp, z.comp, z.comp_part, z.cod "
                      "order by z.grp, z.cod, z.comp, z.comp_part");
        header<<tr("Группа")<<tr("Компонент")<<tr("Парт.комп.")
             <<tr("Упаков.")<<tr("В брак")<<tr("Дроб.руб.")<<tr("Безв.пот.")<<tr("Возв.пот.")<<tr("Шихт.брак")<<tr("Возврат");
    } else if (ui->radioButtonPartComp->isChecked()){
        query.prepare("select z.comp, z.comp_part, "
                      "sum(prod), sum(brk), sum(loss_drb), sum(loss_prs), sum(prs), sum(dry), sum(real_return) "
                      "from( "
                      "select p2.n_s||'-'||p2.yea as parti, e.marka as marka, cast(p2.diam as varchar(3)) as diam, "
                      "CASE WHEN p2.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' end as var, "
                      "et.nam as grp, coalesce(p.nam||' ф '||d2.sdim, m.nam) as comp, rn.cmp_part as comp_part, "
                      "rn.prod,rn.brk,rn.loss_drb,rn.loss_prs,rn.prs,rn.dry,rn.real_return, m.cod "
                      "from tmp_report_new rn "
                      "left join matr m on m.id = rn.id_matr and rn.id_dim=-1 "
                      "left join provol p on p.id = rn.id_matr and rn.id_dim<>-1 "
                      "left join diam d2 on d2.id=rn.id_dim and rn.id_dim<>-1 "
                      "inner join parti p2 on p2.id=rn.id_part "
                      "inner join elrtr_vars ev on ev.id = p2.id_var "
                      "inner join elrtr e on e.id=p2.id_el "
                      "inner join el_types et on et.id = e.id_vid "
                      ") as z "
                      "group by z.comp, z.comp_part, z.cod "
                      "order by z.cod, z.comp, z.comp_part");
        header<<tr("Компонент")<<tr("Парт.комп.")
             <<tr("Упаков.")<<tr("В брак")<<tr("Дроб.руб.")<<tr("Безв.пот.")<<tr("Возв.пот.")<<tr("Шихт.брак")<<tr("Возврат");
    } else if (ui->radioButtonComp->isChecked()){
        query.prepare("select z.comp, "
                      "sum(prod), sum(brk), sum(loss_drb), sum(loss_prs), sum(prs), sum(dry), sum(real_return) "
                      "from( "
                      "select p2.n_s||'-'||p2.yea as parti, e.marka as marka, cast(p2.diam as varchar(3)) as diam, "
                      "CASE WHEN p2.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' end as var, "
                      "et.nam as grp, coalesce(p.nam||' ф '||d2.sdim, m.nam) as comp, rn.cmp_part as comp_part, "
                      "rn.prod,rn.brk,rn.loss_drb,rn.loss_prs,rn.prs,rn.dry,rn.real_return, m.cod "
                      "from tmp_report_new rn "
                      "left join matr m on m.id = rn.id_matr and rn.id_dim=-1 "
                      "left join provol p on p.id = rn.id_matr and rn.id_dim<>-1 "
                      "left join diam d2 on d2.id=rn.id_dim and rn.id_dim<>-1 "
                      "inner join parti p2 on p2.id=rn.id_part "
                      "inner join elrtr_vars ev on ev.id = p2.id_var "
                      "inner join elrtr e on e.id=p2.id_el "
                      "inner join el_types et on et.id = e.id_vid "
                      ") as z "
                      "group by z.comp, z.cod "
                      "order by z.cod, z.comp");
        header<<tr("Компонент")
             <<tr("Упаков.")<<tr("В брак")<<tr("Дроб.руб.")<<tr("Безв.пот.")<<tr("Возв.пот.")<<tr("Шихт.брак")<<tr("Возврат");
    }
    if (modelComp->execQuery(query)){
        for (int i=0; i<header.size(); i++){
            modelComp->setHeaderData(i,Qt::Horizontal,header.at(i));
        }
    }
    ui->tableViewComp->resizeToContents();
}

void FormReport::updDiff()
{
    QSqlQuery query;
    QStringList header;
    if (ui->radioButtonDiffPart->isChecked()){
        query.prepare("select p.n_s, e.marka, cast(p.diam as varchar(3)), CASE WHEN p.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' end, pd.shcht_pack, pd.shcht_fact, "
                      "pd.glass_pack, pd.glass_fact, pd.prov_pack, pd.prov_fact, pd.els_pack, pd.els_fact "
                      "from tmp_part_diff pd "
                      "inner join parti p on p.id = pd.id_part "
                      "inner join elrtr e on e.id = p.id_el "
                      "inner join elrtr_vars ev on ev.id = p.id_var "
                      "order by p.n_s ");
        header<<tr("Партия")<<tr("Марка")<<tr("Ф")<<tr("Вариант")<<tr("Шихта упак.")<<tr("Шихта прес.")<<tr("Ж.ст. упак.")<<tr("Ж.ст. прес.")<<tr("Пров. упак.")<<tr("Пров. прес.")<<tr("Элект. упак.")<<tr("Элект. прес.");
    } else {
        query.prepare("select e.marka, cast(p.diam as varchar(3)), CASE WHEN p.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' end, sum(pd.shcht_pack), sum(pd.shcht_fact), "
                      "sum(pd.glass_pack), sum(pd.glass_fact), sum(pd.prov_pack), sum(pd.prov_fact), sum(pd.els_pack), sum(pd.els_fact) "
                      "from tmp_part_diff pd "
                      "inner join parti p on p.id = pd.id_part "
                      "inner join elrtr e on e.id = p.id_el "
                      "inner join elrtr_vars ev on ev.id = p.id_var "
                      "group by e.marka, p.diam, ev.nam, p.id_var "
                      "order by e.marka, p.diam, ev.nam");
         header<<tr("Марка")<<tr("Ф")<<tr("Вариант")<<tr("Шихта упак.")<<tr("Шихта прес.")<<tr("Ж.ст. упак.")<<tr("Ж.ст. прес.")<<tr("Пров. упак.")<<tr("Пров. прес.")<<tr("Элект. упак.")<<tr("Элект. прес.");
    }
    if (modelDiff->execQuery(query)){
        for (int i=0; i<header.size(); i++){
            modelDiff->setHeaderData(i,Qt::Horizontal,header.at(i));
        }
    }
    ui->tableViewDiff->resizeToContents();
}

void FormReport::updRec()
{
    QSqlQuery query;
    QStringList header;
    if (ui->radioButtonRecPart->isChecked()){
        query.prepare("select n_s,marka,cast(diam as varchar(3)),wtsh,drsh "
                      "from parti inner join elrtr on parti.id_el=elrtr.id "
                      "inner join "
                      "(select id_part, sum(prs) as drsh from tmp_report_new "
                      "group by id_part) as dr on dr.id_part=parti.id "
                      "inner join "
                      "(select id_part, sum(loss) as wtsh from part_prod "
                      "where id_part in (select id_part from tmp_report_new) "
                      "group by id_part) as wt on wt.id_part=parti.id "
                      "order by marka,diam");
        header<<tr("Партия")<<tr("Марка")<<tr("Ф")<<tr("Вл. отходы")<<tr("Сух. отходы");
    } else {
        query.prepare("select z.marka, z.diam, sum(z.wtsh), sum (z.drsh) "
                      "from ( "
                      "select p.n_s,e.marka,cast(p.diam as varchar(3)) as diam,wtsh,drsh "
                      "from parti p "
                      "inner join elrtr e on p.id_el=e.id "
                      "inner join "
                      "(select id_part, sum(prs) as drsh from tmp_report_new "
                      "group by id_part) as dr on dr.id_part=p.id "
                      "inner join "
                      "(select id_part, sum(loss) as wtsh from part_prod "
                      "where id_part in (select id_part from tmp_report_new) "
                      "group by id_part) as wt on wt.id_part=p.id "
                      ") as z group by z.marka, z.diam "
                      "order by  z.marka, z.diam");
        header<<tr("Марка")<<tr("Ф")<<tr("Вл. отходы")<<tr("Сух. отходы");
    }
    if (modelRec->execQuery(query)){
        for (int i=0; i<header.size(); i++){
            modelRec->setHeaderData(i,Qt::Horizontal,header.at(i));
        }
    }
    ui->tableViewRec->resizeToContents();
}
