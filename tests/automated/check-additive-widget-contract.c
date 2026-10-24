/* Public API contract probe used before adding a new Motif widget class. */
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;
static unsigned activated;
static unsigned accessibility_changes;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void
destroy_on_activate(Widget widget, XtPointer closure, XtPointer call_data)
{
    (void)closure;
    (void)call_data;
    ++activated;
    XtDestroyWidget(widget);
}

static void
accessibility_changed(Widget widget, XtPointer closure, XtPointer call_data)
{
    (void)widget;
    (void)closure;
    (void)call_data;
    ++accessibility_changes;
}

int
main(int argc, char **argv)
{
    static String fallback_resources[] = {
        "AdditiveContractTest.width: 271",
        NULL
    };
    XtAppContext app;
    Widget shell;
    Widget row;
    Widget button;
    Widget list;
    XmString item;
    Dimension width = 0;
    Dimension shell_width = 0;

    shell = XtVaAppInitialize(&app, "AdditiveContractTest", NULL, 0,
                              &argc, argv, fallback_resources,
                              XmNwidth, 320, XmNheight, 180, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    button = XtVaCreateManagedWidget("probe", xmPushButtonWidgetClass, row,
                                     XmNwidth, 149,
                                     XmNheight, 32,
                                     XmNrecomputeSize, False,
                                     NULL);
    list = XtVaCreateManagedWidget("items", xmListWidgetClass, row, NULL);
    XtAddCallback(button, XmNactivateCallback, destroy_on_activate, NULL);
    XtAddCallback(list, XmNaccessibilityChangedCallback,
                  accessibility_changed, NULL);

    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);

    XtVaGetValues(shell, XmNwidth, &shell_width, NULL);
    CHECK(shell_width == 320);
    XtVaGetValues(button, XmNwidth, &width, NULL);
    CHECK(width != 0);
    CHECK(XmProcessTraversal(button, XmTRAVERSE_CURRENT));
    CHECK(XmGetFocusWidget(shell) == button);

    item = XmStringCreateLocalized((char *)"contract item");
    CHECK(item != NULL);
    XmListAddItem(list, item, 0);
    XmStringFree(item);
    CHECK(accessibility_changes != 0);

    XtCallCallbacks(button, XmNactivateCallback, NULL);
    CHECK(activated == 1);

    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS additive widget contract: %u checks\n", checks);
    return EXIT_SUCCESS;
}
