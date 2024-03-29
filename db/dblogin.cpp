#include "dblogin.h"
#include "ui_dblogin.h"

DbLogin::DbLogin(const QString title, QPixmap &pix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbLogin)
{
    ui->setupUi(this);
    ui->labelImg->setPixmap(pix);
    this->setWindowTitle(title);
    ui->edtHost->setText("192.168.1.10");
    ui->groupBox->setVisible(false);
    db = QSqlDatabase::addDatabase("QPSQL");
    connect(ui->cmdShowOpt,SIGNAL(clicked(bool)),ui->groupBox,SLOT(setVisible(bool)));
    connect(ui->cmdConnect,SIGNAL(clicked()),this,SLOT(dBconnect()));
}

DbLogin::~DbLogin()
{
    delete ui;
    if (db.isOpen()) db.close();
}

void DbLogin::dBconnect()
{
    db.setDatabaseName("neo_rtx");
    db.setHostName(ui->edtHost->text());
    db.setPort(ui->edtPort->text().toInt());
    db.setUserName(ui->edtUser->text());
    db.setPassword(ui->edtPasswd->text());
    if (!db.open()) {
        QMessageBox::critical(nullptr,"Error",db.lastError().text(),QMessageBox::Ok);
    } else this->accept();
}
