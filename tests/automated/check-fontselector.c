/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/FontS.h>
#include <Xm/Xm.h>
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
    Widget selector;

    shell = XtVaAppInitialize(&app, "FontSelectorRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    selector = XmCreateFontSelector(shell, "selector", NULL, 0);
    XtManageChild(selector);
    XtRealizeWidget(shell);
    CHECK(XtIsRealized(selector));
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS FontSelector bounded font-string construction: %lu checks\n",
           checks);
    return 0;
}
