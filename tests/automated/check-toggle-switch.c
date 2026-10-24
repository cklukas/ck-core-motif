#include <Xm/Accessible.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleSwitch.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;
static unsigned callbacks;
static Boolean last_state;
static unsigned destroyed;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void changed(Widget widget, XtPointer closure, XtPointer call_data) {
    XmToggleSwitchCallbackStruct *callback =
        (XmToggleSwitchCallbackStruct *)call_data;
    (void)widget; (void)closure;
    ++callbacks;
    last_state = callback->set;
}

static void destroy_on_change(Widget widget, XtPointer closure,
                              XtPointer call_data) {
    (void)closure; (void)call_data;
    ++destroyed;
    XtDestroyWidget(widget);
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, row, switch_widget, doomed;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    XmString label;
    Dimension width = 0, height = 0;
    (void)argv;

    shell = XtVaAppInitialize(&app, "ToggleSwitchTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 360, XmNheight, 120, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0);
    XtManageChild(row);
    label = XmStringCreateLocalized((char *)"Sync");
    switch_widget = XmVaCreateManagedToggleSwitch(
        row, (char *)"switch", XmNlabelString, label, NULL);
    XmStringFree(label);
    XtAddCallback(switch_widget, XmNSwitchValueChangedCallback, changed, NULL);
    doomed = XmCreateToggleSwitch(row, (char *)"doomed", NULL, 0);
    XtManageChild(doomed);
    XtAddCallback(doomed, XmNSwitchValueChangedCallback, destroy_on_change, NULL);

    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    XtVaGetValues(switch_widget, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(width > 0 && height > 0);
    CHECK(!XmToggleSwitchGetState(switch_widget));
    XmToggleSwitchSetState(switch_widget, True, True);
    CHECK(XmToggleSwitchGetState(switch_widget) && callbacks == 1 && last_state);
    XmToggleSwitchSetState(switch_widget, True, True);
    CHECK(callbacks == 1);
    XmToggleSwitchSetState(switch_widget, False, False);
    CHECK(!XmToggleSwitchGetState(switch_widget) && callbacks == 1);
    CHECK(XmProcessTraversal(switch_widget, XmTRAVERSE_CURRENT));
    CHECK(XmGetFocusWidget(shell) == switch_widget);
    XtSetSensitive(switch_widget, False);
    CHECK(!XtIsSensitive(switch_widget));
    XmToggleSwitchSetState(switch_widget, True, False);
    CHECK(XmToggleSwitchGetState(switch_widget));
    XtSetSensitive(switch_widget, True);

    XmToggleSwitchSetState(doomed, True, True);
    CHECK(destroyed == 1);

    accessibility = XmAccessibilityCreate(shell, "ToggleSwitchTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, switch_widget);
    CHECK(id != XmACCESSIBLE_NONE);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_CHECK_BOX && info.action_count == 1);
    CHECK(info.states & XmACCESSIBLE_STATE(4));
    XmAccessibleInfoClear(&info);
    CHECK(XmAccessibilityAction(accessibility, id, 0));
    CHECK(!XmToggleSwitchGetState(switch_widget));

    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS toggle switch: %u checks\n", checks);
    return EXIT_SUCCESS;
}
