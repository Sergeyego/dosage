#include "dbdelegate.h"

DbDelegate::DbDelegate(QObject *parent): QItemDelegate(parent)
{

}

QWidget *DbDelegate::createEditor (QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const DbTableModel *sqlModel = qobject_cast<const DbTableModel *>(index.model());
    if (!sqlModel) return QItemDelegate::createEditor(parent, option, index);
    QWidget *editor=nullptr;
    if (sqlModel->sqlRelation(index.column())){
        editor=new DbComboBox(parent);
    } else {
        switch (sqlModel->columnType(index.column())){
        case QVariant::Bool:
        {
            editor=nullptr;
            break;
        }
        case QVariant::String:
        {
            editor=new QLineEdit(parent);
            break;
        }
        case QVariant::Int:
        {
            editor= sqlModel->data(index,Qt::CheckStateRole).isNull() ? new QLineEdit(parent) : nullptr;
            break;
        }
        case QVariant::LongLong:
        {
            editor= new QLineEdit(parent);
            break;
        }
        case QVariant::Double:
        {
            editor=new QLineEdit(parent);
            break;
        }
        case QVariant::Date:
        {
            editor= new DbDateEdit(parent);
            break;
        }
        default:
        {
            editor=QItemDelegate::createEditor(parent, option, index);
            break;
        }
        }
    }
    if (editor) {
        editor->installEventFilter(const_cast<DbDelegate *>(this));
        emit createEdt(index);
    }
    return editor;
}

void DbDelegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
{
    const DbTableModel *sqlModel = qobject_cast<const DbTableModel *>(index.model());
    if (sqlModel){
        if (sqlModel->sqlRelation(index.column())){
            DbComboBox *combo = qobject_cast<DbComboBox *>(editor);
            if (combo) {
                if (combo->model()!=sqlModel->sqlRelation(index.column())->model()){
                    connect(combo,SIGNAL(sigActionEdtRel(QModelIndex)),this,SIGNAL(sigActionEdtRel(QModelIndex)));
                }
                combo->setIndex(index);
                return;
            } else {
                QLineEdit *le = qobject_cast<QLineEdit *>(editor);
                if (le){
                    le->setText(sqlModel->data(index,Qt::DisplayRole).toString());
                    return;
                }
            }
        }
        QVariant dat=sqlModel->data(index,Qt::EditRole);
        if (sqlModel->columnType(index.column()==QMetaType::QDate)){
            DbDateEdit *dateEdit = qobject_cast<DbDateEdit *>(editor);
            if (dateEdit){
                if (dat.isNull()){
                    dateEdit->setDate(dateEdit->minimumDate());
                } else {
                    dateEdit->setDate(dat.toDate());
                }
                return;
            }
        }
        QLineEdit *line = qobject_cast<QLineEdit *>(editor);
        if (line) {
            if (sqlModel->validator(index.column())){
                if (line->validator()!=sqlModel->validator(index.column())){
                    line->setValidator(sqlModel->validator(index.column()));
                }
                QDoubleValidator *dval = qobject_cast<QDoubleValidator *>(sqlModel->validator(index.column()));
                if (dval){
                    if (dat.isNull()){
                        line->clear();
                    } else {
                        line->setText(QString::number(dat.toDouble(),'f',dval->decimals()));
                    }
                    return;
                }
            }

            if (dat.isNull()){
                line->clear();
            } else {
                line->setText(dat.toString());
            }
            return;
        }
    }
    return QItemDelegate::setEditorData(editor, index);
}

void DbDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
    if (!index.isValid())
        return;
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(model);
    if (sqlModel) {
        if (sqlModel->sqlRelation(index.column())){
            DbComboBox *combo = qobject_cast<DbComboBox *>(editor);
            if (combo) {
                colVal data=combo->getCurrentData();
                if (combo->currentText().isEmpty()){
                    data.val=sqlModel->nullVal(index.column());
                    data.disp=QString();
                }
                sqlModel->setData(index,data.val,Qt::EditRole);
                sqlModel->setData(index,data.disp,Qt::DisplayRole);
                return;
            }

        } else {
            QLineEdit *le = qobject_cast<QLineEdit *>(editor);
            if (le){
                if (le->text().isEmpty()) {
                    sqlModel->setData(index,sqlModel->nullVal(index.column()),Qt::EditRole);
                    return;
                }
            }
            if (sqlModel->columnType(index.column())==QVariant::Date){
                DbDateEdit *dateEdit = qobject_cast<DbDateEdit *>(editor);
                if (dateEdit){
                    if (dateEdit->date()==dateEdit->minimumDate()){
                        sqlModel->setData(index,sqlModel->nullVal(index.column()),Qt::EditRole);
                    } else {
                        sqlModel->setData(index,dateEdit->date(),Qt::EditRole);
                    }
                    return;
                }
            }
            QCheckBox *cb=qobject_cast<QCheckBox *>(editor);
            if (cb && sqlModel->columnType(index.column())==QVariant::Int){
                sqlModel->setData(index,cb->isChecked() ? 1 : 0,Qt::EditRole);
                return;
            }
            return QItemDelegate::setModelData(editor, model, index);
        }
    } else {
        return QItemDelegate::setModelData(editor, model, index);
    }
}

void DbDelegate::updateEditorGeometry(
            QWidget *editor,
            const QStyleOptionViewItem &option,
            const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}

bool DbDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type()== QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(keyEvent->text()=="\r" || keyEvent->key()==Qt::Key_Tab || keyEvent->key()==Qt::Key_Down || keyEvent->key()==Qt::Key_Up){
            QWidget *editor = qobject_cast<QWidget*>(object);
            emit commitData(editor);
            emit closeEditor(editor);
            return false;
        }

        QLineEdit *line = qobject_cast<QLineEdit *>(object);
        if (line && line->validator()) {
            const QDoubleValidator *val = qobject_cast< const QDoubleValidator *>(line->validator());
            if (val){
                if (keyEvent->text()==",") {
                    line->insert(".");
                    return true;
                }
            }
        }
    }
    return QItemDelegate::eventFilter(object,event);
}
