#include "dbxlsx.h"

DbXlsx::DbXlsx(QTableView *v, QString head, QObject *parent) : QObject(parent), viewer(v), title(head)
{

}

void DbXlsx::saveToFile()
{
    int rows,cols;
    rows=viewer->model()->rowCount();
    cols=viewer->model()->columnCount();

    if (rows*cols>1){
        Document xlsx;
        Worksheet *ws=xlsx.currentWorksheet();
        XlsxPageSetup pageSetup;
        pageSetup.fitToPage=true;
        pageSetup.fitToWidth=1;
        pageSetup.fitToHeight=0;
        ws->setPageSetup(pageSetup);
        QFont defaultFont("Arial", 10);
        QFont titleFont("Arial", 10);
        titleFont.setBold(true);

        if (!title.isEmpty()){
            ws->writeString(CellReference("A1"),title);
        }

        Format strFormat;
        strFormat.setBorderStyle(Format::BorderThin);
        strFormat.setFont(defaultFont);
        Format numFormat;
        numFormat.setBorderStyle(Format::BorderThin);
        numFormat.setFont(defaultFont);
        Format titleFormat;
        titleFormat.setBorderStyle(Format::BorderThin);
        titleFormat.setFont(titleFont);

        int m=1;
        ws->setRowHeight(2,2,viewer->verticalHeader()->height()/18.0);
        for(int i=0;i<cols;i++) {
            if (!viewer->isColumnHidden(i)) {
                QString hCubeell=viewer->model()->headerData(i,Qt::Horizontal).toString();
                hCubeell.replace(QChar('\n'),QChar('\n'));
                ws->writeString(2,m,hCubeell,titleFormat);
                ws->setColumnWidth(m,m,viewer->columnWidth(i)/6.9);
                m++;
            }
        }

        DbTableModel *sqlModel = qobject_cast<DbTableModel *>(viewer->model());

        for (int i=0;i<rows;i++){
            m=1;
            for(int j=0;j<cols;j++){
                if (!viewer->isColumnHidden(j)) {
                    int role=Qt::EditRole;
                    int dec=3;
                    if (sqlModel){
                        if (sqlModel->sqlRelation(j) || sqlModel->columnType(j)==QVariant::Bool || sqlModel->columnType(j)==QVariant::Time || sqlModel->columnType(j)==QVariant::Date){
                            role=Qt::DisplayRole;
                        }
                        if (sqlModel->validator(j)){
                            QDoubleValidator *doublevalidator = qobject_cast<QDoubleValidator*>(sqlModel->validator(j));
                            if (doublevalidator) dec=doublevalidator->decimals();
                        }
                    }

                    QVariant value=viewer->model()->data(viewer->model()->index(i,j),role);

                    if ((value.typeName()==QString("double"))){
                        if (!sqlModel){
                            QString tmp=viewer->model()->data(viewer->model()->index(i,j),Qt::DisplayRole).toString();
                            int pos = tmp.indexOf(QRegularExpression("[.,]"));
                            if (pos>0){
                                dec=tmp.size()-pos-1;
                            }
                        }
                        if (dec>=1){
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
                            QString fmt=QString("# ##0.%1").arg((0),dec,10,QChar('0'));
#else
                            QString fmt=QString("# ##0.%1").arg((0),dec,'d',0,QChar('0'));
#endif
                            numFormat.setNumberFormat(fmt);
                        } else {
                            numFormat.setNumberFormat("0");
                        }
                        if (!value.isNull()){
                            ws->writeNumeric(i+3,m,value.toDouble(),numFormat);
                        } else {
                            ws->writeBlank(i+3,m,numFormat);
                        }
                    } else if (value.typeName()==QString("int")){
                        numFormat.setNumberFormat("0");
                        ws->writeNumeric(i+3,m,value.toDouble(),numFormat);
                    } else if (value.typeName()==QString("QDate")){
                        ws->writeString(i+3,m,value.toDate().toString("dd.MM.yy"),strFormat);
                    } else if (value.typeName()==QString("bool")){
                        ws->writeString(i+3,m,value.toBool() ? QString("Да") : QString("Нет"),strFormat);
                    }else {
                        ws->writeString(i+3,m,value.toString(),strFormat);
                    }
                    m++;
                }
            }
        }

        QSettings settings("szsm", QApplication::applicationName());
        QDir dir(settings.value("savePath",QDir::homePath()).toString());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить документ"),
                                                        dir.path()+"/"+title+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            if (filename.right(5)!=".xlsx"){
                filename+=".xlsx";
            }
            xlsx.saveAs(filename);
            QFile file(filename);
            QFileInfo info(file);
            settings.setValue("savePath",info.path());
        }
    }
}
