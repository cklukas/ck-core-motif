/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/DropDown.h>
#include <Xm/Xm.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget dropdown;

    shell = XtVaAppInitialize(&app, "DropDownRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    dropdown = XmCreateDropDown(shell, "dropdown", NULL, 0);
    XtManageChild(dropdown);
    XtRealizeWidget(shell);
    if (!XtIsRealized(dropdown))
        return 1;
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS DropDown selected-table allocation bounds");
    return 0;
}
