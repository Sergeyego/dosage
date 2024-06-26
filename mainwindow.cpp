#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    actAction(ui->actionDoz,&MainWindow::newFormDoz);
    actAction(ui->actionBackfill,&MainWindow::newFormBackfill);
    actAction(ui->actionBackfillHist,&MainWindow::newFormBackfillHist);
    actAction(ui->actionOst,&MainWindow::newFormOst);
    actAction(ui->actionComp,&MainWindow::newFormComp);
    actAction(ui->actionGroups,&MainWindow::newFormGroup);
    actAction(ui->actionRepDoz,&MainWindow::newFormRepDoz);
    actAction(ui->actionRepPack,&MainWindow::newFormRepPack);
    actAction(ui->actionRepBunk,&MainWindow::newFormRepBunk);

    loadSettings();
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::closeTab(int index)
{
    ui->tabWidget->widget(index)->close();
}

void MainWindow::newFormDoz()
{
    if (!exist(sender())){
        addSubWindow(new FormDoz(),sender());
    }
}

void MainWindow::newFormBackfill()
{
    if (!exist(sender())){
        addSubWindow(new FormBackfill(),sender());
    }
}

void MainWindow::newFormBackfillHist()
{
    if (!exist(sender())){
        addSubWindow(new FormBackfillHist(),sender());
    }
}

void MainWindow::newFormOst()
{
    if (!exist(sender())){
        addSubWindow(new FormOst(),sender());
    }
}

void MainWindow::newFormComp()
{
    if (!exist(sender())){
        addSubWindow(new FormComp(),sender());
    }
}

void MainWindow::newFormGroup()
{
    if (!exist(sender())){
        addSubWindow(new FormGroup(),sender());
    }
}

void MainWindow::newFormRepDoz()
{
    if (!exist(sender())){
        addSubWindow(new FormRepDoz(),sender());
    }
}

void MainWindow::newFormRepPack()
{
    if (!exist(sender())){
        addSubWindow(new FormRepPack(),sender());
    }
}

void MainWindow::newFormRepBunk()
{
    if (!exist(sender())){
        addSubWindow(new FormRepBunk(),sender());
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->restoreGeometry(settings.value("main_geometry").toByteArray());
    this->restoreState(settings.value("main_state").toByteArray());
    QString opentab=settings.value("main_opentab").toString();
    QString current=settings.value("main_currenttab").toString();

    if (!opentab.isEmpty()){
        QStringList l=opentab.split("|");
        for (QString a : l) {
            if (actions.contains(a)){
                actions.value(a)->trigger();
            }
        }
    }
    setActiveSubWindow(current);
}

void MainWindow::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("main_state", this->saveState());
    settings.setValue("main_geometry", this->saveGeometry());
    QString opentab, currenttab;
    for (int i=0; i<ui->tabWidget->count(); i++){
        if (!opentab.isEmpty()){
            opentab+="|";
        }
        opentab+=ui->tabWidget->tabText(i);
    }
    currenttab=ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    settings.setValue("main_opentab", opentab);
    settings.setValue("main_currenttab",currenttab);
}

bool MainWindow::exist(QObject *a)
{
    bool b=false;
    QAction *action = qobject_cast<QAction *>(a);
    if (action){
        b=setActiveSubWindow(action->text());
    }
    return b;
}

void MainWindow::actAction(QAction *a, void (MainWindow::*sl)())
{
    connect(a, &QAction::triggered, this, sl);
    actions.insert(a->text(),a);
}

void MainWindow::addSubWindow(QWidget *w, QObject *a)
{
    w->setAttribute(Qt::WA_DeleteOnClose);
    QAction *action = qobject_cast<QAction *>(a);
    if (action){
        w->setWindowTitle(action->text());
    }
    ui->tabWidget->addTab(w,w->windowTitle());
    ui->tabWidget->setCurrentWidget(w);
}

bool MainWindow::setActiveSubWindow(QString t)
{
    bool b=false;
    for (int i=0; i<ui->tabWidget->count(); i++){
        if (ui->tabWidget->tabText(i)==t){
            ui->tabWidget->setCurrentIndex(i);
            b=true;
            break;
        }
    }
    return b;
}

