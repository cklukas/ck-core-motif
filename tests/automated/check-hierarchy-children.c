/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Hierarchy.h>
#include <Xm/HierarchyP.h>
#include <Xm/Xm.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget hierarchy;
    WidgetList children;

    shell = XtVaAppInitialize(&app, "HierarchyChildrenRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    hierarchy = XtCreateWidget("hierarchy", xmHierarchyWidgetClass, shell,
                               NULL, 0);
    children = XmHierarchyGetChildNodes(hierarchy);
    if (children != NULL) {
        XtFree((char *)children);
        return 1;
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS hierarchy child-list allocation bounds");
    return 0;
}
