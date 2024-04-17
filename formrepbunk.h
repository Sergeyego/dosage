#ifndef FORMREPBUNK_H
#define FORMREPBUNK_H

#include <QWidget>
#include "db/modelro.h"
#include "rels.h"

namespace Ui {
class FormRepBunk;
}

class FormRepBunk : public QWidget
{
    Q_OBJECT

public:
    explicit FormRepBunk(QWidget *parent = nullptr);
    ~FormRepBunk();

private:
    Ui::FormRepBunk *ui;
    ModelRo *modelRep;

private slots:
    void upd();
    void save();
};

#endif // FORMREPBUNK_H
