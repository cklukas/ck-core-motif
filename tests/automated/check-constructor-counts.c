/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/List.h>
#include <Xm/Text.h>
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

    shell = XtVaAppInitialize(&app, "ConstructorCountRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    CHECK(XmCreateScrolledText(shell, "text", NULL, maximum) == NULL);
    CHECK(XmCreateScrolledList(shell, "list", NULL, maximum) == NULL);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS constructor argument-count overflow guards: %lu checks\n",
           checks);
    return 0;
}
