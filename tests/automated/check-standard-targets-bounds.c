/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <X11/Intrinsic.h>
#include <limits.h>
#include <stdio.h>

extern Atom *XmeStandardTargets(Widget, int, int *);

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } \
    ++checks; \
} while (0)

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Atom *targets;
    int count = -1;
    unsigned long checks = 0;

    shell = XtVaAppInitialize(&app, "StandardTargetsBoundsRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    targets = XmeStandardTargets(shell, -1, &count);
    CHECK(targets == NULL && count == 0);
    count = -1;
    targets = XmeStandardTargets(shell, INT_MAX, &count);
    CHECK(targets == NULL && count == 0);
    count = 0;
    targets = XmeStandardTargets(shell, 0, &count);
    CHECK(targets != NULL && count > 0);
    XtFree((char *)targets);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS standard-targets allocation bounds: %lu checks\n", checks);
    return 0;
}
