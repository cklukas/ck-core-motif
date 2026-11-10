#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/Accessible.h>
#include <Xm/List.h>
#include <Xm/RowActions.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks;

#define CHECK(x) do { \
    if (!(x)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void invoked(Widget widget, XtPointer data, XtPointer call)
{
    const char **seen = (const char **)data;
    XmRowActionsCallbackStruct *callback =
        (XmRowActionsCallbackStruct *)call;
    (void)widget;
    if (callback && callback->id)
        *seen = callback->id;
}

static void key(Widget widget, KeySym keysym)
{
    XEvent event = {0};
    event.xkey.type = KeyPress;
    event.xkey.display = XtDisplay(widget);
    event.xkey.window = XtWindow(widget);
    event.xkey.keycode = XKeysymToKeycode(XtDisplay(widget), keysym);
    XtDispatchEvent(&event);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, list, actions, rtl_actions, first_action;
    XmString item, edit, remove, shortcut;
    XmRowActionItem items[2];
    int *positions = NULL, count = 0;
    const char *seen = NULL;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;

    (void)argv;
    shell = XtVaAppInitialize(&app, "RowActionsTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 420, XmNheight, 160, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    list = XmCreateList(row, (char *)"items", NULL, 0);
    item = XmStringCreateLocalized((char *)"Row one");
    XmListAddItem(list, item, 0);
    XmStringFree(item);
    item = XmStringCreateLocalized((char *)"Row two");
    XmListAddItem(list, item, 0);
    XmStringFree(item);
    XtManageChild(list);
    XmListSelectPos(list, 2, False);

    edit = XmStringCreateLocalized((char *)"Edit");
    remove = XmStringCreateLocalized((char *)"Remove");
    shortcut = XmStringCreateLocalized((char *)"Del");
    items[0] = (XmRowActionItem){"edit", edit, NULL, NULL};
    items[1] = (XmRowActionItem){"remove", remove, shortcut, NULL};
    actions = XmVaCreateManagedRowActions(row, (char *)"actions", NULL);
    rtl_actions = XmVaCreateManagedRowActions(
        row, (char *)"rtlActions", XmNrowActionsSide,
        XmROW_ACTIONS_SIDE_START, XmNstringDirection,
        XmSTRING_DIRECTION_R_TO_L, NULL);
    XtAddCallback(actions, XmNrowActionsInvokeCallback, invoked, &seen);
    XmRowActionsSetItems(actions, items, 2);
    XmRowActionsSetItems(rtl_actions, items, 2);
    XmStringFree(edit);
    XmStringFree(remove);
    XmStringFree(shortcut);

    CHECK(XmRowActionsGetItemCount(actions) == 2);
    CHECK(!XmRowActionsIsRevealed(actions));
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    XtSetSensitive(actions, False);
    key(actions, XK_space);
    CHECK(!XmRowActionsIsRevealed(actions));
    XtSetSensitive(actions, True);
    key(actions, XK_space);
    CHECK(XmRowActionsIsRevealed(actions));
    XmRowActionsReveal(actions, False);
    CHECK(XmListGetSelectedPos(list, &positions, &count) && count == 1 &&
          positions[0] == 2);
    XtFree((char *)positions);

    XmRowActionsReveal(actions, True);
    XmRowActionsReveal(rtl_actions, True);
    CHECK(XmRowActionsIsRevealed(actions));
    XtResizeWidget(actions, 360, 32, 0);
    first_action = XtNameToWidget(actions, (char *)"action");
    CHECK(first_action != NULL);
    {
        Position x;
        XtVaGetValues(first_action, XmNx, &x, NULL);
        CHECK(x > 0); /* Default logical END is trailing in LTR. */
        XtVaSetValues(actions, XmNrowActionsSide, XmROW_ACTIONS_SIDE_START,
                      NULL);
        XtVaGetValues(first_action, XmNx, &x, NULL);
        CHECK(x == 0);
    }
    XtResizeWidget(rtl_actions, 360, 32, 0);
    first_action = XtNameToWidget(rtl_actions, (char *)"action");
    CHECK(first_action != NULL);
    {
        Position x;
        XtVaGetValues(first_action, XmNx, &x, NULL);
        CHECK(x > 0); /* Logical START mirrors to physical right in RTL. */
    }
    XmRowActionsInvoke(actions, 1, NULL);
    CHECK(seen && strcmp(seen, "remove") == 0);
    CHECK(XmListGetSelectedPos(list, &positions, &count) && count == 1 &&
          positions[0] == 2);
    XtFree((char *)positions);

    accessibility = XmAccessibilityCreate(shell, "RowActionsTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, actions);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL &&
          strcmp(info.name, "Row actions") == 0 && info.action_count == 2);
    XmAccessibleInfoClear(&info);
    CHECK(XmAccessibilityAction(accessibility, id, 0));
    CHECK(seen && strcmp(seen, "edit") == 0);
    XmAccessibilityDestroy(accessibility);

    XmRowActionsReveal(actions, False);
    CHECK(!XmRowActionsIsRevealed(actions));
    XmRowActionsInvoke(actions, 0, NULL);
    CHECK(seen && strcmp(seen, "edit") == 0);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS row actions: %u checks\n", checks);
    return EXIT_SUCCESS;
}
