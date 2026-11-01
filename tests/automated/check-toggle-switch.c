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
    Widget shell, row, switch_widget, doomed, sizing;
    Display *display;
    XEvent event = {0};
    XtWidgetGeometry preferred = {0};
    Dimension short_width;
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
    XtAddCallback(switch_widget, XmNvalueChangedCallback, changed, NULL);
    doomed = XmCreateToggleSwitch(row, (char *)"doomed", NULL, 0);
    XtManageChild(doomed);
    XtAddCallback(doomed, XmNvalueChangedCallback, destroy_on_change, NULL);

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

    /* A release must belong to an armed press, and cancellation is sticky. */
    XmToggleSwitchSetState(switch_widget, False, False);
    event.xbutton.type = ButtonRelease;
    event.xbutton.x = 2; event.xbutton.y = 2;
    XtCallActionProc(switch_widget, "SwitchRelease", &event, NULL, 0);
    CHECK(!XmToggleSwitchGetState(switch_widget));
    event.xbutton.type = ButtonPress;
    XtCallActionProc(switch_widget, "SwitchArm", &event, NULL, 0);
    event.xbutton.type = ButtonRelease;
    event.xbutton.x = -1;
    XtCallActionProc(switch_widget, "SwitchRelease", &event, NULL, 0);
    CHECK(!XmToggleSwitchGetState(switch_widget));
    event.xbutton.type = ButtonPress; event.xbutton.x = 2;
    XtCallActionProc(switch_widget, "SwitchArm", &event, NULL, 0);
    XtSetSensitive(switch_widget, False);
    XtSetSensitive(switch_widget, True);
    event.xbutton.type = ButtonRelease;
    XtCallActionProc(switch_widget, "SwitchRelease", &event, NULL, 0);
    CHECK(!XmToggleSwitchGetState(switch_widget));
    event.xbutton.type = ButtonPress;
    XtCallActionProc(switch_widget, "SwitchArm", &event, NULL, 0);
    event.xbutton.type = ButtonRelease;
    XtCallActionProc(switch_widget, "SwitchRelease", &event, NULL, 0);
    CHECK(XmToggleSwitchGetState(switch_widget));
    XtSetSensitive(switch_widget, False);
    XtCallActionProc(switch_widget, "ToggleSwitch", NULL, NULL, 0);
    CHECK(XmToggleSwitchGetState(switch_widget));
    XtSetSensitive(switch_widget, True);

    /* Independent automatic axes must survive label changes and reset. */
    sizing = XmCreateToggleSwitch(row, "sizing", NULL, 0);
    XtQueryGeometry(sizing, NULL, &preferred);
    short_width = preferred.width;
    label = XmStringCreateLocalized("A substantially longer switch label");
    XtVaSetValues(sizing, XmNlabelString, label, NULL);
    XmStringFree(label);
    XtQueryGeometry(sizing, NULL, &preferred);
    CHECK(preferred.width > short_width);
    XtVaSetValues(sizing, XmNwidth, 300, XmNheight, 80, NULL);
    XtQueryGeometry(sizing, NULL, &preferred);
    CHECK(preferred.width == 300 && preferred.height == 80);
    XtVaSetValues(sizing, XmNwidth, 0, NULL);
    XtQueryGeometry(sizing, NULL, &preferred);
    CHECK(preferred.width > short_width && preferred.height == 80);
    XtVaSetValues(sizing, XmNheight, 0, XmNlabelString, NULL, NULL);
    XtQueryGeometry(sizing, NULL, &preferred);
    CHECK(preferred.width == short_width && preferred.height < 80);
    XtDestroyWidget(sizing);

    doomed = XmCreateToggleSwitch(row, "actionDestroyed", NULL, 0);
    XtAddCallback(doomed, XmNvalueChangedCallback, destroy_on_change, NULL);
    XtCallActionProc(doomed, "ToggleSwitch", NULL, NULL, 0);
    CHECK(destroyed == 2);

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
    display = XtDisplay(shell);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS toggle switch: %u checks\n", checks);
    return EXIT_SUCCESS;
}
