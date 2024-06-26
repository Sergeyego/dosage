#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "formdoz.h"
#include "formbackfill.h"
#include "formbackfillhist.h"
#include "formcomp.h"
#include "formgroup.h"
#include "formrepdoz.h"
#include "formreppack.h"
#include "formost.h"
#include "formrepbunk.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void closeTab(int index);
    void newFormDoz();
    void newFormBackfill();
    void newFormBackfillHist();
    void newFormOst();
    void newFormComp();
    void newFormGroup();
    void newFormRepDoz();
    void newFormRepPack();
    void newFormRepBunk();

private:
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();
    bool exist(QObject *a);
    void actAction(QAction *a, void (MainWindow::*sl)());
    void addSubWindow(QWidget *w, QObject *a);
    bool setActiveSubWindow(QString t);
    QMap <QString,QAction*> actions;
};
#endif // MAINWINDOW_H
