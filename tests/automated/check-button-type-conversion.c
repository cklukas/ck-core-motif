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

static Boolean convert(Widget widget, const char *text, XmButtonTypeTable *table)
{
    XrmValue from, to;
    from.addr = (XtPointer)text;
    from.size = (unsigned int)strlen(text) + 1;
    to.addr = (XPointer)table;
    to.size = sizeof(*table);
    return XtConvertAndStore(widget, XmRString, &from, XmRButtonType, &to);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    XmButtonTypeTable table = NULL;

    shell = XtVaAppInitialize(&app, "ButtonTypeConversionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    CHECK(convert(shell, "pushbutton,togglebutton", &table));
    CHECK(table != NULL && table[0] == XmPUSHBUTTON &&
          table[1] == XmTOGGLEBUTTON && table[2] == 0);
    XtFree((char *)table);
    table = NULL;
    CHECK(!convert(shell, "pushbutton,", &table));
    CHECK(!convert(shell, ",togglebutton", &table));
    CHECK(!convert(shell, "pushbutton,,togglebutton", &table));
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS button-type converter rejects empty tokens: %lu checks\n", checks);
    return 0;
}
