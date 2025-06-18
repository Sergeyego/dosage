#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QThread>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QReadWriteLock>
#include <QMutex>
#include <QMutexLocker>
#include <QApplication>

class ModelPrg;

class GetNamer : public QObject
{
    Q_OBJECT
public:
    static GetNamer *instance();
    QString getName();
    ~GetNamer();

protected:
    GetNamer(QObject *parent = nullptr);

private:
    static GetNamer* get_namer_instance;
    int n;
    QMutex mutex;
};

class Executor : public QThread
{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = nullptr);
    ~Executor();
    void run();
    void setQuery(QString qu);
    QVector<QVector<QVariant>> getData();
    QStringList getTitles();

private:
    QString query;
    QVector<QVector<QVariant>> data;
    QStringList titles;
    QString databaseName;
    QString hostName;
    int port;
    QString userName;
    QString password;
    QReadWriteLock dataLock;
    QReadWriteLock queryLock;

private slots:
    void showError(QString text);
signals:
    void sigError(QString mes);
};

#endif // EXECUTOR_H
