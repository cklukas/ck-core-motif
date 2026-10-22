/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/TextUnicode.h>
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
int main(int argc, char **argv) {
    XtAppContext application;
    Widget shell, row, field, text;
    require(setlocale(LC_ALL, "C.UTF-8") != NULL, "UTF8 locale unavailable");
    XtSetLanguageProc(NULL, NULL, NULL);
    require(XmTextUnicodeAvailable(), "Unicode text support not enabled");
    check_map();
    shell = XtVaAppInitialize(&application, "UnicodeFixture", NULL, 0, &argc, argv, NULL, NULL);
    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, row, NULL);
    text = XtVaCreateManagedWidget("text", xmTextWidgetClass, row, NULL);
    XtRealizeWidget(shell);
    check_widget(field, 1); check_widget(text, 0);
    XtDestroyWidget(shell); XtDestroyApplicationContext(application);
    puts("Unicode maps, preedit bounds and real Text/TextField grapheme actions passed");
    return 0;
}
