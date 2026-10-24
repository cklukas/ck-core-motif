/* SPDX-License-Identifier: LGPL-2.0-or-later */
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

static Boolean convert(Widget widget, const char *text,
                       XmStringCharSetTable *table)
{
    XrmValue from, to;
    from.addr = (XtPointer)text;
    from.size = (unsigned int)strlen(text) + 1;
    to.addr = (XPointer)table;
    to.size = sizeof(*table);
    return XtConvertAndStore(widget, XmRString, &from, XmRCharSetTable, &to);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    XmStringCharSetTable table = NULL;

    shell = XtVaAppInitialize(&app, "CharsetTableConversionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    CHECK(convert(shell, "UTF-8,,ISO-8859-1,", &table));
    CHECK(table != NULL && strcmp(table[0], "UTF-8") == 0);
    CHECK(table[1] == NULL);
    CHECK(strcmp(table[2], "ISO-8859-1") == 0);
    CHECK(table[3] == NULL && table[4] == NULL);
    XtFree((char *)table);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS charset-table empty-slot preservation: %lu checks\n", checks);
    return 0;
}
