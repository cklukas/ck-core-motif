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

extern void XmRegisterConverters(void);

static Boolean convert(Widget widget, const char *text, String **table)
{
    XrmValue from, to;
    from.addr = (XtPointer)text;
    from.size = (unsigned int)strlen(text) + 1;
    to.addr = (XPointer)table;
    to.size = sizeof(*table);
    return XtConvertAndStore(widget, XmRString, &from, XmRStringTable, &to);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    String *table = NULL;

    shell = XtVaAppInitialize(&app, "StringTableConversionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    XmRegisterConverters();
    CHECK(convert(shell, "alpha,,omega,", &table));
    CHECK(table != NULL && strcmp(table[0], "alpha") == 0);
    CHECK(table[1][0] == '\0');
    CHECK(strcmp(table[2], "omega") == 0);
    CHECK(table[3] != NULL && table[3][0] == '\0' && table[4] == NULL);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS string-table trailing empty-slot preservation: %lu checks\n", checks);
    return 0;
}
