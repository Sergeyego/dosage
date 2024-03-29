#include "executor.h"

GetNamer* GetNamer::get_namer_instance = nullptr;

Executor::Executor(QObject *parent) :
    QThread(parent)
{
    QSqlDatabase db=QSqlDatabase::database();
    databaseName=db.databaseName();
    hostName=db.hostName();
    port=db.port();
    userName=db.userName();
    password=db.password();
    GetNamer::instance();
    connect(this,SIGNAL(sigError(QString)),this,SLOT(showError(QString)));
}

Executor::~Executor()
{

}

void Executor::run()
{
    QString dbName=GetNamer::instance()->getName();
    {
        QSqlDatabase db=QSqlDatabase::addDatabase("QPSQL",dbName);
        db.setDatabaseName(databaseName);
        db.setHostName(hostName);
        db.setPort(port);
        db.setUserName(userName);
        db.setPassword(password);
        if (db.open()){
            QSqlQuery qu(db);
            queryLock.lockForRead();
            qu.prepare(query);
            queryLock.unlock();
            if (qu.exec()){
                int colCount=qu.record().count();
                QWriteLocker locker(&dataLock);
                data.clear();
                titles.clear();
                for (int i=0; i<qu.record().count(); i++){
                    titles+=qu.record().fieldName(i);
                }
                while (qu.next()){
                    QVector<QVariant> dt;
                    for (int i=0; i<colCount; i++){
                        dt.push_back(qu.value(i));
                    }
                    data.push_back(dt);
                }
            } else {
                emit sigError(qu.lastError().text());
            }
        } else {
            emit sigError(db.lastError().text());
        }
        if (db.isOpen()) db.close();
    }
    QSqlDatabase::removeDatabase(dbName);
}

void Executor::setQuery(QString qu)
{
    QWriteLocker locker(&queryLock);
    query=qu;
}

QVector<QVector<QVariant> > Executor::getData()
{
    QReadLocker locker(&dataLock);
    return data;
}

QStringList Executor::getTitles()
{
    QReadLocker locker(&dataLock);
    return titles;
}

void Executor::showError(QString text)
{
    QMessageBox::critical(nullptr,tr("Ошибка"),text,QMessageBox::Cancel);
}

GetNamer *GetNamer::instance()
{
    if (get_namer_instance == nullptr){
        get_namer_instance = new GetNamer(/*qApp*/);
    }
    return get_namer_instance;
}

QString GetNamer::getName()
{
    QMutexLocker locker(&mutex);
    n++;
    return QString("Base")+QString::number(n);
}

GetNamer::~GetNamer()
{

}

GetNamer::GetNamer(QObject *parent) : QObject(parent)
{
    n=0;
    connect(qApp, SIGNAL(aboutToQuit()), this,SLOT(deleteLater()));
}
