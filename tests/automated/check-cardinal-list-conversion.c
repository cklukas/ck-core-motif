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

static Boolean convert(Widget widget, const char *text, Cardinal **values)
{
    XrmValue from, to;
    from.addr = (XtPointer)text;
    from.size = (unsigned int)strlen(text) + 1;
    to.addr = (XPointer)values;
    to.size = sizeof(*values);
    return XtConvertAndStore(widget, XmRString, &from, XmRCardinalList, &to);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Cardinal *values = NULL;

    shell = XtVaAppInitialize(&app, "CardinalListConversionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    XmRegisterConverters();
    CHECK(convert(shell, "1, 42, 4294967295", &values));
    CHECK(values != NULL && values[0] == 1 && values[1] == 42 &&
          values[2] == (Cardinal)~0U);
    XtFree((char *)values);
    values = NULL;
    CHECK(!convert(shell, "4294967296", &values));
    CHECK(!convert(shell, "-1", &values));
    CHECK(!convert(shell, "12oops", &values));
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS cardinal-list range validation: %lu checks\n", checks);
    return 0;
}
