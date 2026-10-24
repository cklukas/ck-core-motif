/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/List.h>
#include <Xm/Xm.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget list;
    int *positions = NULL;
    int count = -1;

    shell = XtVaAppInitialize(&app, "ListSelectionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    list = XmCreateList(shell, "list", NULL, 0);
    if (XmListGetSelectedPos(list, &positions, &count) ||
        positions != NULL || count != 0) {
        XtFree((char *)positions);
        return 1;
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS List selection allocation bounds");
    return 0;
}
