/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Form.h>
#include <Xm/XmP.h>
#include <stdio.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Cardinal maximum = (Cardinal)~(Cardinal)0;

    shell = XtVaAppInitialize(&app, "DialogCountRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    CHECK(XmeCreateClassDialog(xmFormWidgetClass, shell, "form", NULL,
                               maximum) == NULL);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS dialog constructor argument-count guard: %lu checks\n",
           checks);
    return 0;
}
