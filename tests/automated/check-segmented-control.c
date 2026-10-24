#include <Xm/Accessible.h>
#include <Xm/RowColumn.h>
#include <Xm/SegmentedControl.h>
#include <Xm/ToggleB.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, callbacks, destroyed;
static int callback_index;

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return EXIT_FAILURE; } \
    ++checks; \
} while (0)

static void selected(Widget widget, XtPointer closure, XtPointer data) {
    XmSegmentedControlCallbackStruct *callback =
        (XmSegmentedControlCallbackStruct *)data;
    (void)widget; (void)closure;
    ++callbacks;
    callback_index = callback->selected_index;
}

static void destroy_on_selection(Widget widget, XtPointer closure, XtPointer data) {
    (void)closure; (void)data;
    ++destroyed;
    XtDestroyWidget(widget);
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, row, control, doomed, child;
    XmSegmentedControlItem items[3];
    XmString labels[3];
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    Dimension width = 0;
    unsigned i;
    (void)argv;

    shell = XtVaAppInitialize(&app, "SegmentedControlTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 260, XmNheight, 100, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    for (i = 0; i < 3; ++i) {
        char text[8];
        snprintf(text, sizeof(text), "Item%u", i);
        labels[i] = XmStringCreateLocalized(text);
        items[i].id = i == 0 ? "one" : i == 1 ? "two" : "three";
        items[i].label = labels[i];
        items[i].pixmap = XmUNSPECIFIED_PIXMAP;
        items[i].enabled = i != 2;
    }
    control = XmVaCreateManagedSegmentedControl(
        row, (char *)"segments", XmNSegmentedAllowClear, True,
        XmNSegmentedSpacing, 2, XmNwidth, 80, NULL);
    XtAddCallback(control, XmNSegmentedSelectionCallback, selected, NULL);
    CHECK(XmSegmentedControlSetItems(control, items, 3));
    CHECK(XmSegmentedControlGetItemCount(control) == 3);
    CHECK(!strcmp(XmSegmentedControlGetItemId(control, 1), "two"));
    CHECK(XmSegmentedControlSetSelectedIndex(control, 1, True));
    CHECK(XmSegmentedControlGetSelectedIndex(control) == 1 && callbacks == 1 && callback_index == 1);
    CHECK(!XmSegmentedControlSetSelectedIndex(control, 2, True));
    CHECK(XmSegmentedControlGetSelectedIndex(control) == 1 && callbacks == 1);
    CHECK(XmSegmentedControlSetSelectedIndex(control, -1, True));
    CHECK(XmSegmentedControlGetSelectedIndex(control) == -1 && callbacks == 2);

    child = XtNameToWidget(control, (char *)"segment0");
    CHECK(child != NULL);
    XtVaGetValues(child, XmNwidth, &width, NULL);
    CHECK(width > 0);
    XmToggleButtonSetState(child, True, True);
    CHECK(XmSegmentedControlGetSelectedIndex(control) == 0);
    XmToggleButtonSetState(child, False, True);
    CHECK(XmSegmentedControlGetSelectedIndex(control) == -1);
    XtVaSetValues(control, XmNSegmentedRtl, True, NULL);
    CHECK(XmSegmentedControlSetSelectedIndex(control, 0, False));

    accessibility = XmAccessibilityCreate(shell, "SegmentedControlTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, control);
    CHECK(id != XmACCESSIBLE_NONE && XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_LIST && info.has_selection && info.current_value == 0);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);

    doomed = XmVaCreateManagedSegmentedControl(row, (char *)"doomed", NULL);
    XtAddCallback(doomed, XmNSegmentedSelectionCallback, destroy_on_selection, NULL);
    CHECK(XmSegmentedControlSetItems(doomed, items, 2));
    CHECK(XmSegmentedControlSetSelectedIndex(doomed, 0, True));
    CHECK(destroyed == 1);
    for (i = 0; i < 3; ++i) XmStringFree(labels[i]);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS segmented control: %u checks\n", checks);
    return EXIT_SUCCESS;
}
