/****************************************************************************
** Copyright (c) 2013-2014 Debao Zhang <hello@debao.me>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#ifndef XLSXWORKSHEET_P_H
#define XLSXWORKSHEET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt Xlsx API.  It exists for the convenience
// of the Qt Xlsx.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "xlsxworksheet.h"
#include "xlsxabstractsheet_p.h"
#include "xlsxcell.h"
#include "xlsxdatavalidation.h"
#include "xlsxconditionalformatting.h"
#include "xlsxcellformula.h"

#include <QImage>
#include <QSharedPointer>
#include <QRegularExpression>

class QXmlStreamWriter;
class QXmlStreamReader;

namespace QXlsx {

const int XLSX_ROW_MAX = 1048576;
const int XLSX_COLUMN_MAX = 16384;
const int XLSX_STRING_MAX = 32767;
const int XLSX_COLUMN_BREAK_MAX = 65535;
const int XLSX_ROW_BREAK_MAX = 16383;

class SharedStrings;

struct XlsxHyperlinkData
{
    enum LinkType
    {
        External,
        Internal
    };

    XlsxHyperlinkData(LinkType linkType=External, const QString &target=QString(), const QString &location=QString()
            , const QString &display=QString(), const QString &tip=QString())
        :linkType(linkType), target(target), location(location), display(display), tooltip(tip)
    {

    }

    LinkType linkType;
    QString target; //For External link
    QString location;
    QString display;
    QString tooltip;
};

// ECMA-376 Part1 18.3.1.81
struct XlsxSheetFormatProps
{
    XlsxSheetFormatProps(int baseColWidth = 8,
                         bool customHeight = true,
                         double defaultColWidth = 0.0,
                         double defaultRowHeight = 15,
                         quint8 outlineLevelCol = 0,
                         quint8 outlineLevelRow = 0,
                         bool thickBottom = false,
                         bool thickTop = false,
                         bool zeroHeight = false) :
        baseColWidth(baseColWidth),
        customHeight(customHeight),
        defaultColWidth(defaultColWidth),
        defaultRowHeight(defaultRowHeight),
        outlineLevelCol(outlineLevelCol),
        outlineLevelRow(outlineLevelRow),
        thickBottom(thickBottom),
        thickTop(thickTop),
        zeroHeight(zeroHeight) {
    }

    int baseColWidth;
    bool customHeight;
    double defaultColWidth;
    double defaultRowHeight;
    quint8 outlineLevelCol;
    quint8 outlineLevelRow;
    bool thickBottom;
    bool thickTop;
    bool zeroHeight;
};

struct XlsxPrintOptions{
    XlsxPrintOptions(
    bool headings=false,
    bool gridLines=false,
    bool gridLinesSet=true,
    bool horizontalCentered=false,
    bool verticalCentered=false) :
        headings(headings),
        gridLines(gridLines),
        gridLinesSet(gridLinesSet),
        horizontalCentered(horizontalCentered),
        verticalCentered(verticalCentered) {
    }

    bool headings;
    bool gridLines;
    bool gridLinesSet;
    bool horizontalCentered;
    bool verticalCentered;
};

struct XlsxRowInfo
{
    XlsxRowInfo(double height=0, const Format &format=Format(), bool hidden=false) :
        customHeight(false), height(height), format(format), hidden(hidden), outlineLevel(0)
      , collapsed(false)
    {

    }

    bool customHeight;
    double height;
    Format format;
    bool hidden;
    int outlineLevel;
    bool collapsed;
};

struct XlsxBreaks
{
    XlsxBreaks(bool man=true, int max=XLSX_ROW_BREAK_MAX, int min=0) :
        man(true), max(XLSX_ROW_BREAK_MAX), min(0)
    {

    }

    bool man;
    int max;
    int min;
};

struct XlsxHeaderFooter
{
    XlsxHeaderFooter(bool differentFirst=false, bool differentOddEven=false, bool scaleWithDoc=true, bool alignWithMargins=true) :
        differentFirst(false), differentOddEven(false), scaleWithDoc(true), alignWithMargins(true)
    {

    }

    bool differentFirst;
    bool differentOddEven;
    bool scaleWithDoc;
    bool alignWithMargins;
    QMap <XlsxHeaderFooterType, QString > HeaderData;
    QMap <XlsxHeaderFooterType, QString > FooterData;
};

struct XlsxColumnInfo
{
    XlsxColumnInfo(int firstColumn=0, int lastColumn=1, double width=0, const Format &format=Format(), bool hidden=false) :
        firstColumn(firstColumn), lastColumn(lastColumn), customWidth(false), width(width), format(format), hidden(hidden)
      , outlineLevel(0), collapsed(false)
    {

    }
    int firstColumn;
    int lastColumn;
    bool customWidth;
    double width;    
    Format format;
    bool hidden;
    int outlineLevel;
    bool collapsed;
};

enum sheet_view_type
{
    normal,
    page_break_preview,
};

enum XlsxPaneState
{
    XLSX_PANE_FROZEN,
    XLSX_PANE_FROZEN_SPLIT,
    XLSX_PANE_SPLIT
};

struct XlsxPane{
    int xSplit;
    int ySplit;
    CellReference topLeftCell;
    XlsxPanePos activePane;
    XlsxPaneState state;
};

struct XlsxSelection
{
    XlsxPanePos pane;
    CellReference activeCell;
    CellRange sqref;
};

class XLSX_AUTOTEST_EXPORT WorksheetPrivate : public AbstractSheetPrivate
{
    Q_DECLARE_PUBLIC(Worksheet)
public:
    WorksheetPrivate(Worksheet *p, Worksheet::CreateFlag flag);
    ~WorksheetPrivate();
    int checkDimensions(int row, int col, bool ignore_row=false, bool ignore_col=false);
    Format cellFormat(int row, int col) const;
    QString generateDimensionString() const;
    void calculateSpans() const;
    void splitColsInfo(int colFirst, int colLast);
    void validateDimension();

    void saveXmlSheetViews(QXmlStreamWriter &writer) const;
    void saveXmlSheetData(QXmlStreamWriter &writer) const;
    void saveXmlCellData(QXmlStreamWriter &writer, int row, int col, QSharedPointer<Cell> cell) const;
    void saveXmlMergeCells(QXmlStreamWriter &writer) const;
    void saveXmlHeaderFooter(QXmlStreamWriter &writer) const;
    void saveXmlRowBreaks(QXmlStreamWriter &writer) const;
    void saveXmlColumnBreaks(QXmlStreamWriter &writer) const;
    void saveXmlHyperlinks(QXmlStreamWriter &writer) const;
    void saveXmlDrawings(QXmlStreamWriter &writer) const;
    void saveXmlDataValidations(QXmlStreamWriter &writer) const;
    int rowPixelsSize(int row) const;
    int colPixelsSize(int col) const;

    void loadXmlSheetData(QXmlStreamReader &reader);
    void loadXmlColumnsInfo(QXmlStreamReader &reader);
    void loadXmlMergeCells(QXmlStreamReader &reader);
    void loadXmlDataValidations(QXmlStreamReader &reader);
    void loadXmlSheetFormatProps(QXmlStreamReader &reader);
    void loadXmlSheetViews(QXmlStreamReader &reader);
    void loadXmlHyperlinks(QXmlStreamReader &reader);
    void loadXmlPrintOptions(QXmlStreamReader &reader);
    void loadXmlPageMargins(QXmlStreamReader &reader);
    void loadXmlPageSetup(QXmlStreamReader &reader);
    void loadXmlHeaderFooter(QXmlStreamReader &reader);
    void loadXmlRowBreaks(QXmlStreamReader &reader);
    void loadXmlColumnBreaks(QXmlStreamReader &reader);

    QList<QSharedPointer<XlsxRowInfo> > getRowInfoList(int rowFirst, int rowLast);
    QList <QSharedPointer<XlsxColumnInfo> > getColumnInfoList(int colFirst, int colLast);
    QList<int> getColumnIndexes(int colFirst, int colLast);
    bool isColumnRangeValid(int colFirst, int colLast);

    SharedStrings *sharedStrings() const;

    QMap<int, QMap<int, QSharedPointer<Cell> > > cellTable;
    QMap<int, QMap<int, QString> > comments;
    QMap<int, QMap<int, QSharedPointer<XlsxHyperlinkData> > > urlTable;
    QList<CellRange> merges;
    QMap<int, XlsxBreaks> rowBreaks;
    QMap<int, XlsxBreaks> columnBreaks;
    QMap<int, QSharedPointer<XlsxRowInfo> > rowsInfo;
    QMap<int, QSharedPointer<XlsxColumnInfo> > colsInfo;
    QMap<int, QSharedPointer<XlsxColumnInfo> > colsInfoHelper;

    QList<DataValidation> dataValidationsList;
    QList<ConditionalFormatting> conditionalFormattingList;
    QMap<int, CellFormula> sharedFormulaMap;

    CellRange dimension;

    mutable QMap<int, QString> row_spans;
    QMap<int, double> row_sizes;
    QMap<int, double> col_sizes;

    sheet_view_type viewType;
    XlsxSheetFormatProps formatProps;
    XlsxPrintOptions xlsxPrintOptions;
    XlsxPageMargins pageMargins;
    XlsxPageSetup pageSetup;
    XlsxHeaderFooter headerFooter;

    bool windowProtection;
    bool showFormulas;
    bool showGridLines;
    bool showRowColHeaders;
    bool showZeros;
    bool rightToLeft;
    bool tabSelected;
    bool showRuler;
    bool showOutlineSymbols;
    bool showWhiteSpace;

    XlsxPane *pane=0;
    QList<XlsxSelection> selections;

    QRegularExpression urlPattern;
private:
    static double calculateColWidth(int characters);
};

}
#endif // XLSXWORKSHEET_P_H
