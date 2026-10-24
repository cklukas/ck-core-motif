#include <Xm/Accessible.h>
#include <Xm/TimePicker.h>
#include <assert.h>
#include <stdio.h>

static int changes, activations;
static void changed(Widget w, XtPointer c, XtPointer d) {
    XmTimePickerCallbackStruct *cb = (XmTimePickerCallbackStruct *)d;
    (void)w; (void)c; assert(cb && cb->reason == 1); ++changes;
}
static void activated(Widget w, XtPointer c, XtPointer d) {
    XmTimePickerCallbackStruct *cb = (XmTimePickerCallbackStruct *)d;
    (void)w; (void)c; assert(cb && cb->reason == 2); ++activations;
}

int main(int argc, char **argv) {
    XtAppContext app; Widget shell, picker; XmTimePickerValue value, minimum, maximum;
    XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id;
    shell = XtVaAppInitialize(&app, "TimePickerFixture", NULL, 0,
                              &argc, argv, NULL, NULL);
    picker = XmCreateTimePicker(shell, (char *)"picker", NULL, 0);
    XtAddCallback(picker, XmNtimePickerValueChangedCallback, changed, NULL);
    XtAddCallback(picker, XmNtimePickerActivateCallback, activated, NULL);
    XtManageChild(picker);
    assert(XmTimePickerIsValid(&(XmTimePickerValue){23, 59, 59}));
    assert(!XmTimePickerIsValid(&(XmTimePickerValue){24, 0, 0}));
    assert(XmTimePickerSetValue(picker, &(XmTimePickerValue){12, 34, 56}, True));
    assert(changes == 1);
    assert(XmTimePickerGetValue(picker, &value));
    assert(value.hour == 12 && value.minute == 34 && value.second == 56);
    minimum = (XmTimePickerValue){12, 0, 0};
    maximum = (XmTimePickerValue){13, 0, 0};
    assert(XmTimePickerSetRange(picker, &minimum, &maximum));
    assert(!XmTimePickerSetValue(picker, &(XmTimePickerValue){14, 0, 0}, True));
    XtRealizeWidget(shell);
    accessibility = XmAccessibilityCreate(shell, "TimePickerFixture");
    assert(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, picker);
    assert(id != XmACCESSIBLE_NONE);
    assert(XmAccessibilityQuery(accessibility, id, &info));
    assert(info.role == XmACCESSIBLE_TEXT && info.has_value &&
           info.current_value == 12 * 3600 + 34 * 60 + 56);
    XmAccessibleInfoClear(&info); XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    puts("PASS time picker: 11 checks");
    return 0;
}
