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
    explicit DialogNew(QWidget *parent = 0);
    ~DialogNew();
    int getNewId();

private:
    Ui::DialogNew *ui;
    int new_id;
    TableModel *modelRcp;
    TableModel *modelRcpData;
    void getRcp();


private slots:
    void selectRcp();
    void writeRcp();
    void escRcp();
    void lock(bool b);
};

#endif // DIALOGNEW_H
