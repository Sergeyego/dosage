#include "dbcombobox.h"

DbComboBox::DbComboBox(QWidget *parent) : QComboBox(parent)
{
    this->setEditable(true);
    sqlCompleter = new CustomCompletter(this);
    sqlCompleter->setWidget(this);
    this->setCompleter(nullptr);
    isReset=false;

    actionEdt = new QAction(QIcon(":images/key.png"),tr("Редактировать"),this);

    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
    connect(sqlCompleter,SIGNAL(currentDataChanged(colVal)),this,SLOT(setCurrentData(colVal)));
}

colVal DbComboBox::getCurrentData()
{
    return currentData;
}

DbComboBox::~DbComboBox()
{
    //qDebug()<<"delete DbComboBox";
}

void DbComboBox::setIndex(const QModelIndex &index)
{
    const DbTableModel *sqlModel = qobject_cast<const DbTableModel *>(index.model());
    if (sqlModel){
        dbModelIndex=index;
        if (sqlModel->sqlRelation(index.column())){
            if (this->model()!=sqlModel->sqlRelation(index.column())->model()){
                this->setModel(sqlModel->sqlRelation(index.column())->model());
            }
            colVal c;
            c.disp=sqlModel->data(index,Qt::DisplayRole).toString();
            c.val=sqlModel->data(index,Qt::EditRole);
            setCurrentData(c);
        }
    }
}

void DbComboBox::setModel(QAbstractItemModel *model)
{
    DbSqlLikeModel *sqlModel = qobject_cast<DbSqlLikeModel *>(model);
    if (sqlModel){
        if (!sqlModel->isInital()){
            sqlModel->startSearch("");
        }
        QComboBox::setModel(sqlModel);
        setModelColumn(1);

        connect(sqlModel,SIGNAL(searchFinished(QString)),this,SLOT(updData()));
        connect(sqlModel,SIGNAL(modelAboutToBeReset()),this,SLOT(mAboutReset()));
        connect(sqlModel,SIGNAL(modelReset()),this,SLOT(mReset()));

        DbSqlLikeModel *likeModel = new DbSqlLikeModel(sqlModel->getRelation(),this);
        likeModel->setAsync(sqlModel->getRelation()->getAsyncSearch());
        sqlCompleter->setModel(likeModel);
        sqlCompleter->setCompletionColumn(1);

        if (sqlModel->getRelation()->isEditable() && this->isEditable()){
            this->lineEdit()->addAction(actionEdt,QLineEdit::TrailingPosition);
            connect(actionEdt,SIGNAL(triggered(bool)),this,SLOT(edtRel()));
        }
        return;
    }
    return QComboBox::setModel(model);
}


void DbComboBox::indexChanged(int n)
{
    if (n>=0 && !isReset){
        colVal newVal;
        newVal.val=this->model()->data(this->model()->index(n,0),Qt::EditRole);
        newVal.disp=this->model()->data(this->model()->index(n,1),Qt::EditRole).toString();
        currentData=newVal;
    }
}

void DbComboBox::edtRel()
{
    emit sigActionEdtRel(dbModelIndex);
}

void DbComboBox::updData()
{
    this->blockSignals(true);
    this->setCurrentData(currentData);
    this->blockSignals(false);
}

void DbComboBox::mAboutReset()
{
    isReset=true;
    saveData=currentData;
}

void DbComboBox::mReset()
{
    isReset=false;
    setCurrentData(saveData);
}

void DbComboBox::setCurrentData(colVal data)
{
    currentData=data;
    bool ok=false;
    if (this->model() && !currentData.val.isNull()){
        for (int i=0; i<this->model()->rowCount(); i++){
            if (this->model()->data(this->model()->index(i,0),Qt::EditRole)==currentData.val){
                this->setCurrentIndex(i);
                ok=true;
                break;
            }
        }
    }
    if (!ok){
        this->setCurrentIndex(-1);
        if (this->isEditable()){
            this->lineEdit()->setText(data.disp);
        }
    }
}

CustomCompletter::CustomCompletter(QObject *parent) : QCompleter(parent)
{
    setCompletionMode(QCompleter::PopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);
    setWrapAround(false);
    connect(this,SIGNAL(activated(QModelIndex)),this,SLOT(setCurrentKey(QModelIndex)));
}

CustomCompletter::~CustomCompletter()
{
    //qDebug()<<"delete completer";
}

bool CustomCompletter::eventFilter(QObject *o, QEvent *e)
{
    if (e->type()==QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        if ((keyEvent->text()=="\r" || keyEvent->key()==Qt::Key_Tab) && this->popup()->isVisible()) {
            if (this->popup()->currentIndex().isValid()){
                emit activated(this->popup()->model()->index(this->popup()->currentIndex().row(),1));
            } else if (this->popup()->model()->rowCount()){
                emit activated(this->popup()->model()->index(0,1));
            }
            return true;
        }
    }
    return QCompleter::eventFilter(o,e);
}

void CustomCompletter::setModel(QAbstractItemModel *c)
{
    DbSqlLikeModel *mod = qobject_cast<DbSqlLikeModel *>(c);
    if (mod){
        connect(mod,SIGNAL(searchFinished(QString)),this,SLOT(actFinished(QString)));
    }
    return QCompleter::setModel(c);
}

void CustomCompletter::setWidget(QWidget *widget)
{
    DbComboBox *combo = qobject_cast<DbComboBox *>(widget);
    if (combo){
        connect(combo->lineEdit(),SIGNAL(textEdited(QString)),this,SLOT(actComp(QString)));
    }
    return QCompleter::setWidget(widget);
}

void CustomCompletter::actComp(QString s)
{
    DbSqlLikeModel *mod = qobject_cast<DbSqlLikeModel *>(this->model());
    if (mod){
        mod->startSearch(s);
    }
}

void CustomCompletter::setCurrentKey(QModelIndex index)
{
    if (index.isValid()){
        colVal d;
        d.disp=this->popup()->model()->data(this->popup()->model()->index(index.row(),1),Qt::EditRole).toString();
        d.val=this->popup()->model()->data(this->popup()->model()->index(index.row(),0),Qt::EditRole);
        emit currentDataChanged(d);
    }
}

void CustomCompletter::actFinished(QString s)
{
    setCompletionPrefix(s);
    if (s.size()){
        complete();
    } else {
        this->popup()->close();
    }
}
