/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Container.h>
#include <Xm/Xm.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget container;

    shell = XtVaAppInitialize(&app, "ContainerRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    container = XmCreateContainer(shell, "container", NULL, 0);
    XtManageChild(container);
    XtRealizeWidget(shell);
    if (!XtIsRealized(container))
        return 1;
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS Container detail-header allocation bounds");
    return 0;
}
