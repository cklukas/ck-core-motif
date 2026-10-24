/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/RowColumn.h>
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
    Widget menu;

    shell = XtVaAppInitialize(&app, "AccessibilityMenuRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    menu = XmCreatePopupMenu(shell, "popup", NULL, 0);
    XtManageChild(menu);
    XtRealizeWidget(shell);
    CHECK(XtIsRealized(menu));
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS accessibility menu lifecycle: %lu checks\n", checks);
    return 0;
}
