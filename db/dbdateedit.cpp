#include "dbdateedit.h"


DbDateEdit::DbDateEdit(QWidget *parent) : QDateEdit(parent)
{
    this->setCalendarPopup(true);
    CustomCalendarWidget * pCW = new CustomCalendarWidget(this);
    pCW->setFirstDayOfWeek( Qt::Monday );
    this->setCalendarWidget( pCW );
    this->setDisplayFormat("dd.MM.yy");
    this->setSpecialValueText("NULL");
    connect(this->lineEdit(),SIGNAL(textChanged(QString)),this,SLOT(txtChangeSlot(QString)));
    connect(pCW,SIGNAL(shown()),this,SLOT(shVid()));
}

void DbDateEdit::txtChangeSlot(QString txt)
{
    if (txt.isEmpty()){
        this->setDate(this->minimumDate());
    }
}

void DbDateEdit::shVid()
{
    if (QDateEdit::date()==this->minimumDate()){
        this->setDate(QDate::currentDate());
    }
}

CustomCalendarWidget::CustomCalendarWidget(QWidget *parent) : QCalendarWidget(parent)
{
    this->setFirstDayOfWeek( Qt::Monday );
}

void CustomCalendarWidget::showEvent(QShowEvent *event)
{
    if (event->type() == QEvent::Show){
        emit shown();
    }
    QCalendarWidget::showEvent(event);
}
