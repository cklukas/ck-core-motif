#include <X11/IntrinsicP.h>
#include <Xm/Accessible.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/StatusBar.h>
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

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, list, status;
    XmString item, primary, progress, secondary;
    XmStatusBarItem items[3], replacement;
    int *positions = NULL, count = 0;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;

    (void)argv;
    shell = XtVaAppInitialize(&app, "StatusBarTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 360, XmNheight, 140, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    list = XmCreateList(row, (char *)"items", NULL, 0);
    item = XmStringCreateLocalized((char *)"Keep selected");
    XmListAddItem(list, item, 0);
    XmStringFree(item);
    XtManageChild(list);
    XmListSelectPos(list, 1, False);

    primary = XmStringCreateLocalized((char *)"Primary");
    progress = XmStringCreateLocalized((char *)"Progress");
    secondary = XmStringCreateLocalized((char *)"Secondary information");
    items[0] = (XmStatusBarItem){"primary", primary, XmSTATUS_BAR_ITEM_TEXT,
                                 10, 0, 0, NULL};
    items[1] = (XmStatusBarItem){"progress", progress,
                                 XmSTATUS_BAR_ITEM_PROGRESS, 5, 4, 10, NULL};
    items[2] = (XmStatusBarItem){"secondary", secondary,
                                 XmSTATUS_BAR_ITEM_TEXT, 1, 0, 0, NULL};
    status = XmVaCreateManagedStatusBar(row, (char *)"status",
                                        XmNwidth, 120, XmNheight, 32, NULL);
    XmStatusBarSetItems(status, items, 3);
    XmStringFree(primary);
    XmStringFree(progress);
    XmStringFree(secondary);

    CHECK(XmStatusBarGetItemCount(status) == 3);
    CHECK(XmStatusBarGetVisibleItemCount(status) <= 3);
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    XtResizeWidget(status, 120, 32, 0);
    XSync(XtDisplay(shell), False);
    CHECK(XmStatusBarGetVisibleItemCount(status) >= 1);
    CHECK(XmStatusBarGetOverflowCount(status) > 0);
    CHECK(XmStatusBarGetVisibleItemCount(status) +
              XmStatusBarGetOverflowCount(status) == 3);

    XtResizeWidget(status, 900, 32, 0);
    XSync(XtDisplay(shell), False);
    CHECK(XmStatusBarGetVisibleItemCount(status) == 3);
    CHECK(XmStatusBarGetOverflowCount(status) == 0);

    accessibility = XmAccessibilityCreate(shell, "StatusBarTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, status);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL &&
          strcmp(info.name, "Status bar") == 0 && info.action_count == 0);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);

    replacement = (XmStatusBarItem){"replacement", NULL,
                                    XmSTATUS_BAR_ITEM_TEXT, 1, 0, 0, NULL};
    primary = XmStringCreateLocalized((char *)"Replacement");
    replacement.label = primary;
    XmStatusBarSetItems(status, &replacement, 1);
    XmStringFree(primary);
    CHECK(XmStatusBarGetItemCount(status) == 1);
    CHECK(XmStatusBarGetOverflowCount(status) == 0);
    CHECK(XmListGetSelectedPos(list, &positions, &count) && count == 1 &&
          positions[0] == 1);
    XtFree((char *)positions);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS status bar: %u checks\n", checks);
    return EXIT_SUCCESS;
}
