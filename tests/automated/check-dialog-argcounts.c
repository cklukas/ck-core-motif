/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Command.h>
#include <Xm/MessageB.h>
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
    Cardinal maximum = (Cardinal)~(Cardinal)0;

    shell = XtVaAppInitialize(&app, "DialogArgCountRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    CHECK(XmCreateCommand(shell, "command", NULL, maximum) == NULL);
    CHECK(XmCreateMessageDialog(shell, "message", NULL, maximum) == NULL);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS dialog argument-count overflow guards: %lu checks\n", checks);
    return 0;
}
