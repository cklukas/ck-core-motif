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

static Boolean convert(Widget widget, const char *text, XmStringTable *table)
{
    XrmValue from, to;
    from.addr = (XtPointer)text;
    from.size = (unsigned int)strlen(text) + 1;
    to.addr = (XPointer)table;
    to.size = sizeof(*table);
    return XtConvertAndStore(widget, XmRString, &from, XmRXmStringTable, &to);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    XmStringTable table = NULL;
    char *text = NULL;

    shell = XtVaAppInitialize(&app, "XmStringTableConversionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    XmRegisterConverters();
    CHECK(convert(shell, "alpha,,omega,", &table));
    CHECK(table != NULL && XmStringGetLtoR(table[0], XmFONTLIST_DEFAULT_TAG, &text));
    CHECK(strcmp(text, "alpha") == 0);
    XtFree(text); text = NULL;
    CHECK(XmStringGetLtoR(table[1], XmFONTLIST_DEFAULT_TAG, &text));
    CHECK(text[0] == '\0');
    XtFree(text); text = NULL;
    CHECK(XmStringGetLtoR(table[2], XmFONTLIST_DEFAULT_TAG, &text));
    CHECK(strcmp(text, "omega") == 0);
    XtFree(text); text = NULL;
    CHECK(XmStringGetLtoR(table[3], XmFONTLIST_DEFAULT_TAG, &text));
    CHECK(text[0] == '\0');
    XtFree(text);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS XmString-table trailing empty-slot preservation: %lu checks\n", checks);
    return 0;
}
