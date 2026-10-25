#include <X11/IntrinsicP.h>
#include <Xm/Accessible.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/Toolbar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks;
static unsigned callbacks;
static const char *last_id;

#define CHECK(x) do { \
    if (!(x)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void activated(Widget widget, XtPointer client, XtPointer call)
{
    XmToolbarCallbackStruct *callback = (XmToolbarCallbackStruct *)call;
    (void)widget;
    (void)client;
    ++callbacks;
    last_id = callback ? callback->id : NULL;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, list, toolbar;
    XmString fixed, important, secondary, seldom;
    XmToolbarAction actions[4], replacement;
    int *positions = NULL, count = 0;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;

    (void)argv;
    shell = XtVaAppInitialize(&app, "ToolbarTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 360, XmNheight, 140, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    list = XmCreateList(row, (char *)"items", NULL, 0);
    fixed = XmStringCreateLocalized((char *)"Keep selected");
    XmListAddItem(list, fixed, 0);
    XmStringFree(fixed);
    XtManageChild(list);
    XmListSelectPos(list, 1, False);

    fixed = XmStringCreateLocalized((char *)"Fixed");
    important = XmStringCreateLocalized((char *)"Important");
    secondary = XmStringCreateLocalized((char *)"Secondary action");
    seldom = XmStringCreateLocalized((char *)"Seldom used action");
    actions[0] = (XmToolbarAction){"fixed", fixed, NULL, False, 0, NULL};
    actions[1] = (XmToolbarAction){"important", important, NULL, True, 10, NULL};
    actions[2] = (XmToolbarAction){"secondary", secondary, NULL, True, 5, NULL};
    actions[3] = (XmToolbarAction){"seldom", seldom, NULL, True, 1, NULL};
    toolbar = XmVaCreateManagedToolbar(row, (char *)"toolbar",
                                      XmNwidth, 120, XmNheight, 32, NULL);
    XtAddCallback(toolbar, XmNtoolbarActivateCallback, activated, NULL);
    XmToolbarSetActions(toolbar, actions, 4);
    {
        Pixmap icons[4] = {None, None, None, None};
        XmToolbarSetActionIcons(toolbar, icons, 4);
    }
    XmStringFree(fixed);
    XmStringFree(important);
    XmStringFree(secondary);
    XmStringFree(seldom);

    CHECK(XmToolbarGetActionCount(toolbar) == 4);
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    XtResizeWidget(toolbar, 120, 32, 0);
    XSync(XtDisplay(shell), False);
    CHECK(XmToolbarGetVisibleActionCount(toolbar) >= 1);
    CHECK(XmToolbarGetOverflowCount(toolbar) > 0);
    CHECK(XmToolbarGetVisibleActionCount(toolbar) +
              XmToolbarGetOverflowCount(toolbar) == 4);

    XmToolbarInvoke(toolbar, 1, NULL);
    CHECK(callbacks == 1 && last_id && strcmp(last_id, "important") == 0);

    XtResizeWidget(toolbar, 900, 32, 0);
    XSync(XtDisplay(shell), False);
    CHECK(XmToolbarGetVisibleActionCount(toolbar) == 4);
    CHECK(XmToolbarGetOverflowCount(toolbar) == 0);

    accessibility = XmAccessibilityCreate(shell, "ToolbarTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, toolbar);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL &&
          strcmp(info.name, "Toolbar") == 0);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);

    replacement = (XmToolbarAction){"replacement", NULL, NULL, True, 1, NULL};
    important = XmStringCreateLocalized((char *)"Replacement");
    replacement.label = important;
    XmToolbarSetActions(toolbar, &replacement, 1);
    XmStringFree(important);
    CHECK(XmToolbarGetActionCount(toolbar) == 1);
    CHECK(XmToolbarGetOverflowCount(toolbar) == 0);
    CHECK(XmListGetSelectedPos(list, &positions, &count) && count == 1 &&
          positions[0] == 1);
    XtFree((char *)positions);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS toolbar: %u checks\n", checks);
    return EXIT_SUCCESS;
}
