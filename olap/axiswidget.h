#ifndef AXISWIDGET_H
#define AXISWIDGET_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class AxisWidget;
}

class AxisWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit AxisWidget(QStringList axis, QWidget *parent = 0);
    ~AxisWidget();
    
private:
    Ui::AxisWidget *ui;

private slots:
    void upd();
    void up();
    void down();

signals:
    void sigUpd(QStringList selected);
};

#endif // AXISWIDGET_H
