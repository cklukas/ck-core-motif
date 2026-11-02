/* Native regression for the shared demo help manager's geometry contract. */
#include <assert.h>
#include <stdio.h>
#include "../../demos/lib/Xmd/Help.c"

static void check_size(Widget help)
{
    XmdHelpWidget hw = (XmdHelpWidget)help;
    Widget button = hw->help.dismiss;
    assert(XtWidth(help) >= XtX(button) + XtWidth(button) +
           2 * XtBorderWidth(button) + hw->help.spacing);
    assert(XtWidth(help) < 1000 && XtHeight(help) < 800);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "HelpGeometry", NULL, 0,
                                     &argc, argv, NULL, NULL);
    Arg args[2];
    XtSetArg(args[0], XmdNhelpFile, "filemanager");
    XtSetArg(args[1], XmdNhelpPath, argv[1]);
    Widget help = XmdCreateHelpDialog(shell, "help_manager", args, 2);
    XmdHelpWidget hw = (XmdHelpWidget)help;
    Widget notebook = hw->help.notebook, button = hw->help.dismiss;
    XtWidgetGeometry request = {0}, reply = {0};
    Dimension old_width, old_height, old_border, parent_width;
    XtManageChild(help);
    check_size(help);
    old_width = XtWidth(notebook);
    old_height = XtHeight(notebook);
    old_border = XtBorderWidth(notebook);
    parent_width = XtWidth(help);
    request.request_mode = CWWidth | CWHeight | CWBorderWidth | XtCWQueryOnly;
    request.width = old_width + 40;
    request.height = old_height + 20;
    request.border_width = 2;
    assert(XtMakeGeometryRequest(notebook, &request, &reply) == XtGeometryYes);
    assert(XtWidth(notebook) == old_width && XtHeight(notebook) == old_height);
    assert(XtBorderWidth(notebook) == old_border && XtWidth(help) == parent_width);
    request.request_mode &= ~XtCWQueryOnly;
    assert(XtMakeGeometryRequest(notebook, &request, &reply) == XtGeometryYes);
    assert(XtWidth(notebook) == request.width);
    assert(XtHeight(notebook) == request.height);
    assert(XtBorderWidth(notebook) == request.border_width);
    assert(XtX(button) == XtX(notebook) + XtWidth(notebook) +
           2 * XtBorderWidth(notebook) + hw->help.spacing);
    check_size(help);
    old_width = XtWidth(notebook);
    parent_width = XtWidth(help);
    XtVaSetValues(XtParent(help), XtNallowShellResize, False, NULL);
    request.request_mode = CWWidth;
    request.width = old_width + 10;
    assert(XtMakeGeometryRequest(notebook, &request, &reply) == XtGeometryNo);
    assert(XtWidth(notebook) == old_width && XtWidth(help) == parent_width);
    XtVaSetValues(XtParent(help), XtNallowShellResize, True, NULL);
    request.request_mode = CWX;
    request.x = 500;
    assert(XtMakeGeometryRequest(notebook, &request, &reply) == XtGeometryNo);
    check_size(help);
    XtUnmanageChild(help);
    XtManageChild(help);
    check_size(help);
    XtDestroyWidget(shell);
    puts("PASS: bounded help geometry, child resize, borders, query-only, denied position, reopen");
    return 0;
}
