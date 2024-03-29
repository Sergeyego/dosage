#ifndef DBLOGIN_H
#define DBLOGIN_H

#include <QDialog>
#include <QMessageBox>
#include <QtSql>
#include <QPixmap>

namespace Ui {
    class DbLogin;
}

class DbLogin : public QDialog
{
    Q_OBJECT

public:
    explicit DbLogin(const QString title, QPixmap &pix, QWidget *parent = nullptr);
    ~DbLogin();

private:
    Ui::DbLogin *ui;
    QSqlDatabase db;

private slots:
    void dBconnect();
};

#endif // DBLOGIN_H
