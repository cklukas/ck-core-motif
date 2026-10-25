#include <Xm/Accessible.h>
#include <Xm/DropHighlight.h>
#include <Xm/List.h>
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

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, list, highlight;
    XmString item, label;
    int *positions = NULL, count = 0;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;

    (void)argv;
    shell = XtVaAppInitialize(&app, "DropHighlightTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 360, XmNheight, 140, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    list = XmCreateList(row, (char *)"items", NULL, 0);
    item = XmStringCreateLocalized((char *)"Keep selected");
    XmListAddItem(list, item, 0);
    XmStringFree(item);
    XtManageChild(list);
    XmListSelectPos(list, 1, False);

    label = XmStringCreateLocalized((char *)"Drop target");
    highlight = XmVaCreateManagedDropHighlight(
        row, (char *)"highlight", XmNdropHighlightLabel, label,
        XmNwidth, 260, XmNheight, 42, NULL);
    XmStringFree(label);

    CHECK(XmDropHighlightGetState(highlight) == XmDROP_HIGHLIGHT_NONE);
    CHECK(!XmDropHighlightSetState(highlight, 99));
    CHECK(XmDropHighlightSetState(highlight, XmDROP_HIGHLIGHT_VALID));
    CHECK(XmDropHighlightGetState(highlight) == XmDROP_HIGHLIGHT_VALID);
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    CHECK(XmDropHighlightSetState(highlight, XmDROP_HIGHLIGHT_INVALID));
    CHECK(XmDropHighlightGetState(highlight) == XmDROP_HIGHLIGHT_INVALID);

    accessibility = XmAccessibilityCreate(shell, "DropHighlightTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, highlight);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL &&
          strcmp(info.name, "Drop target") == 0);
    CHECK(info.action_count == 0);
    XmAccessibleInfoClear(&info);
    CHECK(!XmAccessibilityAction(accessibility, id, 0));
    XmAccessibilityDestroy(accessibility);

    CHECK(XmDropHighlightSetState(highlight, XmDROP_HIGHLIGHT_NONE));
    CHECK(XmDropHighlightGetState(highlight) == XmDROP_HIGHLIGHT_NONE);
    CHECK(XmListGetSelectedPos(list, &positions, &count) && count == 1 &&
          positions[0] == 1);
    XtFree((char *)positions);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS drop highlight: %u checks\n", checks);
    return EXIT_SUCCESS;
}
