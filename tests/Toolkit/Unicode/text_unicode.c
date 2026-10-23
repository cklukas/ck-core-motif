/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/DataF.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/TextUnicode.h>
#include <Xm/TextLayout.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern Boolean _XmImPreeditRangeValid(unsigned int, int, int, unsigned int, int);
static void require(int condition, const char *detail) { if (!condition) { fprintf(stderr, "%s\n", detail); exit(1); } }
static void check_map(void) {
    /* Combining accent, family ZWJ, RI flag, CJK and RTL letters. */
    const char *source = "e\xcc\x81" "\xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x91\xa7\xe2\x80\x8d\xf0\x9f\x91\xa6" "\xf0\x9f\x87\xa9\xf0\x9f\x87\xaa" "\xe4\xb8\xad" "\xd7\x90" "\xd8\xa8";
    XmTextUnicodeMap *map = XmTextUnicodeMapCreate(source, strlen(source));
    size_t scalar, byte, next;
    require(map != NULL && XmTextUnicodeGraphemeCount(map) == 6 && XmTextUnicodeScalarCount(map) == 14, "Unicode counts");
    require(!XmTextUnicodeIsGraphemeBoundary(map, 1), "combining split accepted");
    require(XmTextUnicodeNextGrapheme(map, 0, &next) && next == 2, "combining next boundary");
    require(XmTextUnicodeNextGrapheme(map, 2, &next) && next == 9, "ZWJ family split");
    require(XmTextUnicodeNextGrapheme(map, 9, &next) && next == 11, "regional indicator pair split");
    for (scalar = 0; scalar <= XmTextUnicodeScalarCount(map); ++scalar) {
        size_t inverse;
        require(XmTextUnicodeByteAtScalar(map, scalar, &byte) && XmTextUnicodeScalarAtByte(map, byte, &inverse) && inverse == scalar, "scalar byte roundtrip");
    }
    require(!XmTextUnicodeScalarAtByte(map, 2, &scalar), "middle of UTF8 sequence accepted");
    XmTextUnicodeMapFree(map);
    require(!XmTextUnicodeMapCreate("\xc0\xaf", 2), "overlong UTF8 accepted");
    require(!XmTextUnicodeMapCreate("\xed\xa0\x80", 3), "surrogate UTF8 accepted");
    require(!XmTextUnicodeMapCreate("\xf4\x90\x80\x80", 4), "out of range scalar accepted");
    require(!XmTextUnicodeMapCreate("a\0b", 3), "embedded NUL accepted");
    require(_XmImPreeditRangeValid(5, 2, 3, 2, 4), "valid preedit change refused");
    require(!_XmImPreeditRangeValid(5, -1, 0, 0, 0), "negative preedit start accepted");
    require(!_XmImPreeditRangeValid(5, 4, 2, 0, 0), "preedit removal outside remaining buffer accepted");
    require(!_XmImPreeditRangeValid(65535, 0, 0, 1, 0), "preedit length overflow accepted");
    require(!_XmImPreeditRangeValid(5, 0, 0, 1, 7), "preedit caret outside result accepted");
}
static void check_widget(Widget widget, int field) {
    XEvent event;
    char *value;
    Position x, y;
    memset(&event, 0, sizeof(event));
    event.xkey.type = KeyPress; event.xkey.display = XtDisplay(widget); event.xkey.window = XtWindow(widget); event.xkey.time = CurrentTime;
    if (field) { XmTextFieldSetString(widget, "e\xcc\x81" "X"); XmTextFieldSetInsertionPosition(widget, 0); }
    else { XmTextSetString(widget, "e\xcc\x81" "X"); XmTextSetInsertionPosition(widget, 0); }
    XtCallActionProc(widget, "forward-character", &event, NULL, 0);
    require((field ? XmTextFieldGetInsertionPosition(widget) : XmTextGetInsertionPosition(widget)) == 2, "interactive cursor split combining cluster");
    XtCallActionProc(widget, "delete-previous-character", &event, NULL, 0);
    value = field ? XmTextFieldGetString(widget) : XmTextGetString(widget);
    require(strcmp(value, "X") == 0, "interactive deletion split combining cluster"); XtFree(value);
    if (field) {
        XmTextFieldSetString(widget, "e\xcc\x81" "XY");
        require(XmTextFieldPosToXY(widget, 3, &x, &y), "field scalar position geometry failed");
        require(XmTextFieldXYToPos(widget, x, y) == 3, "field byte/scalar/grapheme units confused in mouse hit");
    } else {
        XmTextSetString(widget, "e\xcc\x81" "XY");
        require(XmTextPosToXY(widget, 3, &x, &y), "text scalar position geometry failed");
        require(XmTextXYToPos(widget, x, y) == 3, "text byte/scalar/grapheme units confused in mouse hit");
    }
}
static void check_complex(Widget widget, int field) {
    const char *source = "abc \xd7\x90\xd7\x91\xd7\x92 \xd8\xa8\xd8\xad\xd8\xb1 e\xcc\x81 \xe4\xb8\xad\xe6\x96\x87";
    XmTextUnicodeMap *map = XmTextUnicodeMapCreate(source, strlen(source));
    size_t g;
    Position x, y, inverse_x, inverse_y;
    require(map != NULL, "complex widget fixture mapping");
    if (field) XmTextFieldSetString(widget, (char *)source);
    else XmTextSetString(widget, (char *)source);
    for (g = 0; g <= XmTextUnicodeGraphemeCount(map); ++g) {
        size_t scalar;
        XmTextPosition hit;
        XmTextUnicodeScalarAtGrapheme(map, g, &scalar);
        require(field ? XmTextFieldPosToXY(widget, (XmTextPosition)scalar, &x, &y) :
                        XmTextPosToXY(widget, (XmTextPosition)scalar, &x, &y), "complex caret geometry unavailable");
        hit = field ? XmTextFieldXYToPos(widget, x, y) : XmTextXYToPos(widget, x, y);
        require(hit >= 0 && XmTextUnicodeIsGraphemeBoundary(map, (size_t)hit), "complex widget hit split a grapheme");
        require(field ? XmTextFieldPosToXY(widget, hit, &inverse_x, &inverse_y) :
                        XmTextPosToXY(widget, hit, &inverse_x, &inverse_y), "complex hit inverse geometry unavailable");
        /* Bidi boundaries can give distinct logical positions the same visual
         * point; validate the point and legal boundary rather than guessing. */
        require(abs((int)x - inverse_x) <= 1 && y == inverse_y, "widget hit/caret disagree with captured visual position");
    }
    XmTextUnicodeMapFree(map);
}
static void check_wrapped(Widget widget) {
    const char *source = "abc \xd7\x90\xd7\x91\xd7\x92 \xd8\xa8\xd8\xad\xd8\xb1 e\xcc\x81 \xe4\xb8\xad\xe6\x96\x87 abc \xd7\x90\xd7\x91\xd7\x92 \xd8\xa8\xd8\xad\xd8\xb1 e\xcc\x81 \xe4\xb8\xad\xe6\x96\x87";
    XmTextUnicodeMap *map = XmTextUnicodeMapCreate(source, strlen(source));
    size_t g;
    Position x, y, last_y = -1;
    int rows = 0;
    require(map != NULL, "wrapped widget fixture mapping");
    {
        Dimension width;
        XtVaGetValues(widget, XmNwidth, &width, NULL);
        require(width <= 170, "fixture parent expanded the wrapped viewport");
    }
    XmTextSetString(widget, (char *)source);
    for (g = 0; g <= XmTextUnicodeGraphemeCount(map); ++g) {
        size_t scalar;
        XmTextPosition hit;
        XmTextUnicodeScalarAtGrapheme(map, g, &scalar);
        require(XmTextPosToXY(widget, (XmTextPosition)scalar, &x, &y), "wrapped caret geometry unavailable");
        if (y != last_y) { ++rows; last_y = y; }
        hit = XmTextXYToPos(widget, x, y);
        require(hit >= 0 && XmTextUnicodeIsGraphemeBoundary(map, (size_t)hit), "wrapped hit split a grapheme");
    }
    require(rows >= 3, "wrapped text did not produce multiple captured lines");
    /* Exercise source/cache invalidation and width-dependent line recapture. */
    XmTextReplace(widget, 0, 3, "xy");
    XtVaSetValues(widget, XmNwidth, 300, NULL);
    require(XmTextPosToXY(widget, 2, &x, &y), "wrapped cache failed after replacement/resize");
    XmTextUnicodeMapFree(map);
}
static void check_font_reset(Widget widget,int field) {
    Position x0,y0,x1,y1,new_x0,new_y0,new_x1,new_y1;
    Arg args[3];XmRendition rendition;XmFontList fonts;
    if(field) XmTextFieldSetString(widget,"abc");else XmTextSetString(widget,"abc");
    require(field?XmTextFieldPosToXY(widget,0,&x0,&y0):XmTextPosToXY(widget,0,&x0,&y0),"font reset old origin");
    require(field?XmTextFieldPosToXY(widget,3,&x1,&y1):XmTextPosToXY(widget,3,&x1,&y1),"font reset old caret");
    XtSetArg(args[0],XmNfontType,XmFONT_IS_XFT);XtSetArg(args[1],XmNfontName,"DejaVu Sans");XtSetArg(args[2],XmNfontSize,24);
    rendition=XmRenditionCreate(widget,XmFONTLIST_DEFAULT_TAG,args,3);
    require(rendition!=NULL,"replacement font rendition unavailable");
    fonts=XmRenderTableAddRenditions(NULL,&rendition,1,XmMERGE_REPLACE);XmRenditionFree(rendition);
    require(fonts!=NULL,"replacement font table unavailable");
    /* Changing cursor and font together exercises early cursor capture before
     * SetValues has installed the new render table's metrics. */
    XtVaSetValues(widget,XmNfontList,fonts,XmNcursorPosition,1,NULL);XmFontListFree(fonts);
    require(field?XmTextFieldPosToXY(widget,0,&new_x0,&new_y0):XmTextPosToXY(widget,0,&new_x0,&new_y0),"font reset new origin");
    require(field?XmTextFieldPosToXY(widget,3,&new_x1,&new_y1):XmTextPosToXY(widget,3,&new_x1,&new_y1),"font reset new caret");
    if(new_x1-new_x0<=x1-x0) fprintf(stderr,"font reset field=%d old-width=%d new-width=%d\n",field,(int)(x1-x0),(int)(new_x1-new_x0));
    require(new_x1-new_x0>x1-x0,"font change reused an old shaping/context capture");
    require(XmTextFontConfigurationChanged(),"font configuration notification");
    require(field?XmTextFieldPosToXY(widget,3,&x1,&y1):XmTextPosToXY(widget,3,&x1,&y1),"font configuration refresh caret");
    require(x1==new_x1&&y1==new_y1,"unchanged font configuration refresh changed geometry");
    check_widget(widget,field);
}
static void check_column_geometry(Widget parent) {
    const char *families[]={"DejaVu Sans","DejaVu Sans Mono"};
    const int sizes[]={10,18},columns[]={7,22};
    size_t family,size,column;int distinguished_maximum=0;
    for(family=0;family<2;++family) for(size=0;size<2;++size) {
        Arg args[3];XmRendition rendition;XmFontList fonts;
        XmFontContext context;XmFontListEntry entry;XmFontType type;
        XftFont *font;int cell;FcChar32 character=0x2003;XGlyphInfo extent;
        XtSetArg(args[0],XmNfontType,XmFONT_IS_XFT);
        XtSetArg(args[1],XmNfontName,families[family]);
        XtSetArg(args[2],XmNfontSize,sizes[size]);
        rendition=XmRenditionCreate(parent,XmFONTLIST_DEFAULT_TAG,args,3);
        require(rendition!=NULL,"column font rendition");
        fonts=XmRenderTableAddRenditions(NULL,&rendition,1,XmMERGE_REPLACE);XmRenditionFree(rendition);
        require(fonts&&XmFontListInitFontContext(&context,fonts),"column font table");
        entry=XmFontListNextEntry(context);require(entry!=NULL,"column font entry");
        font=(XftFont *)XmFontListEntryGetFont(entry,&type);
        require(font&&type==XmFONT_IS_XFT,"column font is not Xft");
        XmFontListFreeFontContext(context);
        if(!XftCharExists(XtDisplay(parent),font,character)) character='0';
        if(XftCharExists(XtDisplay(parent),font,character)) {
            XftTextExtents32(XtDisplay(parent),font,&character,1,&extent);cell=extent.xOff;
        } else cell=font->max_advance_width;
        require(cell>0,"column fixture lacks positive font cell");
        if(cell!=font->max_advance_width) distinguished_maximum=1;
        for(column=0;column<2;++column) {
            int field;
            for(field=0;field<3;++field) {
                WidgetClass kind=field==2?xmDataFieldWidgetClass:(field?xmTextFieldWidgetClass:xmTextWidgetClass);
                Widget widget=XtVaCreateWidget("columns",kind,parent,
                    XmNfontList,fonts,XmNcolumns,columns[column],XmNmarginWidth,3,
                    XmNshadowThickness,2,XmNhighlightThickness,1,NULL);
                XtWidgetGeometry preferred;
                Dimension actual;int expected=columns[column]*cell+12;
                XtRealizeWidget(widget);
                memset(&preferred,0,sizeof preferred);XtQueryGeometry(widget,NULL,&preferred);
                XtVaGetValues(widget,XmNwidth,&actual,NULL);
                if(!(preferred.request_mode&CWWidth)||preferred.width!=expected||actual!=expected)
                    fprintf(stderr,"column geometry field=%d family=%s size=%d columns=%d cell=%d maximum=%d expected=%d preferred=%d actual=%d\n",
                            field,families[family],sizes[size],columns[column],cell,font->max_advance_width,expected,(int)preferred.width,(int)actual);
                require((preferred.request_mode&CWWidth)&&preferred.width==expected&&actual==expected,
                        "Xft columns did not use documented em-space/zero font cell");
                XtDestroyWidget(widget);
            }
        }
        XmFontListFree(fonts);
    }
    require(distinguished_maximum,"column fixture did not distinguish representative and maximum font advance");
}
int main(int argc, char **argv) {
    XtAppContext application;
    Widget shell, row, field, text, wrapped, wrap_container;
    XmFontListEntry entry;
    XmFontList fonts;
    require(setlocale(LC_ALL, "C.UTF-8") != NULL, "UTF8 locale unavailable");
    XtSetLanguageProc(NULL, NULL, NULL);
    require(XmTextUnicodeAvailable(), "Unicode text support not enabled");
    check_map();
    shell = XtVaAppInitialize(&application, "UnicodeFixture", NULL, 0, &argc, argv, NULL, NULL);
    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    entry = XmFontListEntryLoad(XtDisplay(shell), "DejaVu Sans", XmFONT_IS_XFT, XmFONTLIST_DEFAULT_TAG);
    require(entry != NULL, "Xft text fixture font unavailable");
    fonts = XmFontListAppendEntry(NULL, entry); XmFontListEntryFree(&entry);
    field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, row, XmNfontList, fonts, NULL);
    text = XtVaCreateManagedWidget("text", xmTextWidgetClass, row, XmNfontList, fonts, XmNwordWrap, False, NULL);
    /* RowColumn stretches direct children to its widest child. A Form keeps
     * this editor's independent viewport narrow while the unwrapped controls
     * above it retain enough width for their separate round-trip checks. */
    wrap_container = XtVaCreateManagedWidget("wrapContainer", xmFormWidgetClass,
        row, XmNwidth, 170, XmNheight, 360, XmNresizePolicy, XmRESIZE_NONE, NULL);
    wrapped = XtVaCreateManagedWidget("wrapped", xmTextWidgetClass, wrap_container,
        XmNfontList, fonts, XmNwordWrap, True, XmNscrollHorizontal, False,
        XmNeditMode, XmMULTI_LINE_EDIT, XmNwidth, 170, XmNheight, 360,
        XmNresizeWidth, False, XmNresizeHeight, False, NULL);
    XmFontListFree(fonts);
    XtRealizeWidget(shell);
    check_widget(field, 1); check_widget(text, 0);
    check_complex(field, 1); check_complex(text, 0); check_wrapped(wrapped);
    check_font_reset(field,1);check_font_reset(text,0);
    check_column_geometry(row);
    XtDestroyWidget(shell); XtDestroyApplicationContext(application);
    puts("Unicode maps, preedit bounds and real Text/TextField grapheme actions passed");
    return 0;
}
