#include <Xm/Accessible.h>
#include <Xm/DatePicker.h>
#include <X11/keysym.h>
#include <assert.h>
#include <stdio.h>

static int changes, opens;

static void changed(Widget widget, XtPointer closure, XtPointer call_data) {
    XmDatePickerCallbackStruct *callback = (XmDatePickerCallbackStruct *)call_data;
    (void)widget; (void)closure;
    assert(callback && callback->reason == 1);
    ++changes;
}

static void open_changed(Widget widget, XtPointer closure, XtPointer call_data) {
    XmDatePickerCallbackStruct *callback = (XmDatePickerCallbackStruct *)call_data;
    (void)widget; (void)closure;
    assert(callback && (callback->reason == 3 || callback->reason == 4));
    ++opens;
}

static void key(Widget widget, KeySym keysym) {
    XEvent event = {0};
    event.xkey.type = KeyPress;
    event.xkey.display = XtDisplay(widget);
    event.xkey.window = XtWindow(widget);
    event.xkey.root = DefaultRootWindow(XtDisplay(widget));
    event.xkey.same_screen = True;
    event.xkey.keycode = XKeysymToKeycode(XtDisplay(widget), keysym);
    XtDispatchEvent(&event);
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, picker;
    XmDatePickerDate date, minimum, maximum;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId accessible_id;

    shell = XtVaAppInitialize(&app, "DatePickerFixture", NULL, 0,
                              &argc, argv, NULL, NULL);
    picker = XmCreateDatePicker(shell, (char *)"picker", NULL, 0);
    XtAddCallback(picker, XmNdatePickerDateChangedCallback, changed, NULL);
    XtAddCallback(picker, XmNdatePickerOpenChangedCallback, open_changed, NULL);
    XtManageChild(picker);
    assert(XmDatePickerSetDate(picker, &(XmDatePickerDate){2024, 2, 29}, True));
    assert(changes == 1);
    assert(XmDatePickerGetDate(picker, &date));
    assert(date.year == 2024 && date.month == 2 && date.day == 29);
    minimum = (XmDatePickerDate){2024, 2, 1};
    maximum = (XmDatePickerDate){2024, 3, 31};
    assert(XmDatePickerSetRange(picker, &minimum, &maximum));
    assert(!XmDatePickerSetDate(picker, &(XmDatePickerDate){2024, 4, 1}, True));
    XtRealizeWidget(shell);
    XtSetSensitive(picker, False);
    key(picker, XK_Return);
    assert(!XmDatePickerIsOpen(picker));
    assert(opens == 0);
    XtSetSensitive(picker, True);
    key(picker, XK_Return);
    assert(XmDatePickerIsOpen(picker));
    assert(opens == 1);
    XmDatePickerClose(picker);
    assert(!XmDatePickerIsOpen(picker));
    assert(opens == 2);
    XmDatePickerOpen(picker);
    assert(XmDatePickerIsOpen(picker));
    assert(opens == 3);
    XmDatePickerClose(picker);
    assert(!XmDatePickerIsOpen(picker));
    assert(opens == 4);
    accessibility = XmAccessibilityCreate(shell, "DatePickerFixture");
    assert(accessibility != NULL);
    accessible_id = XmAccessibilityWidget(accessibility, picker);
    assert(accessible_id != XmACCESSIBLE_NONE);
    assert(XmAccessibilityQuery(accessibility, accessible_id, &info));
    assert(info.role == XmACCESSIBLE_TEXT && info.has_value &&
           info.current_value == 2024);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    puts("PASS date picker: 15 checks");
    return 0;
}
