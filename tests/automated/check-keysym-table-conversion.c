/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <X11/keysym.h>
#include <Xm/RowColumn.h>
#include <Xm/Xm.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

static Boolean convert(Widget widget, const char *text, XmKeySymTable *table)
{
    XrmValue from, to;
    from.addr = (XtPointer)text;
    from.size = (unsigned int)strlen(text) + 1;
    to.addr = (XPointer)table;
    to.size = sizeof(*table);
    return XtConvertAndStore(widget, XmRString, &from, XmRKeySymTable, &to);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    XmKeySymTable table = NULL;

    shell = XtVaAppInitialize(&app, "KeysymTableConversionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    CHECK(convert(shell, "Return,,Escape,", &table));
    CHECK(table != NULL && table[0] == XK_Return);
    CHECK(table[1] == NoSymbol);
    CHECK(table[2] == XK_Escape);
    CHECK(table[3] == NoSymbol && table[4] == NoSymbol);
    XtFree((char *)table);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS keysym-table empty-slot preservation: %lu checks\n", checks);
    return 0;
}
