#ifndef DBDATEEDIT_H
#define DBDATEEDIT_H

#include <QDateEdit>
#include <QCalendarWidget>
#include <QLineEdit>
#include <QShowEvent>

class CustomCalendarWidget : public QCalendarWidget
{
    Q_OBJECT
public:
    CustomCalendarWidget(QWidget *parent=nullptr);
    virtual void showEvent(QShowEvent *event);
signals:
    void shown();
};

class DbDateEdit : public QDateEdit
{
    Q_OBJECT
public:
    DbDateEdit(QWidget *parent = nullptr);
private slots:
    void txtChangeSlot(QString txt);
    void shVid();
};

#endif // DBDATEEDIT_H
