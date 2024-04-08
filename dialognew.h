#ifndef DIALOGNEW_H
#define DIALOGNEW_H

#include <QDialog>
#include <QDomDocument>
#include "rels.h"
#include "db/tablemodel.h"

namespace Ui {
class DialogNew;
}

class DialogNew : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNew(QWidget *parent = nullptr);
    ~DialogNew();
    int getNewId();

private:
    Ui::DialogNew *ui;
    int new_id;
    TableModel *modelRcp;
    TableModel *modelRcpData;
    TableModel *modelDozData;
    void getRcp();
    QString getXmlVal(QDomElement &el, QString nam);
    QDomElement newXmlElement(QString nam, QString val, QDomDocument *doc);


private slots:
    void selectRcp();
    void writeRcp();
    void escRcp();
    void lock(bool b);
    void updRcpData(QModelIndex index);
};

#endif // DIALOGNEW_H
