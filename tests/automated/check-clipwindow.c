/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/ScrolledW.h>
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
    Widget scrolled;

    shell = XtVaAppInitialize(&app, "ClipWindowRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    scrolled = XmCreateScrolledWindow(shell, "scrolled", NULL, 0);
    XtManageChild(scrolled);
    XtRealizeWidget(shell);
    CHECK(XtIsRealized(scrolled));
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS ClipWindow translation construction: %lu checks\n", checks);
    return 0;
}
