#ifndef FORMFILTER_H
#define FORMFILTER_H

#include <QWidget>

namespace Ui {
class FormFilter;
}

class FormFilter : public QWidget
{
    Q_OBJECT

public:
    explicit FormFilter(const QStringList &items, const QStringList &select, QWidget *parent = 0);
    ~FormFilter();
    QStringList selectedItems();
    bool getEn();

private:
    Ui::FormFilter *ui;

private slots:
    void checkAll();
    void uncheckAll();
    void setEn();

signals:
    void enChanged(bool c);
};

#endif // FORMFILTER_H
