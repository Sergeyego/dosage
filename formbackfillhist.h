#ifndef FORMBACKFILLHIST_H
#define FORMBACKFILLHIST_H

#include <QWidget>
#include "modelro.h"
#include "rels.h"

namespace Ui {
class FormBackfillHist;
}

class FormBackfillHist : public QWidget
{
    Q_OBJECT

public:
    explicit FormBackfillHist(QWidget *parent = nullptr);
    ~FormBackfillHist();

private:
    Ui::FormBackfillHist *ui;
    ModelRo *modelHist;

private slots:
    void updHist();
};

#endif // FORMBACKFILLHIST_H
