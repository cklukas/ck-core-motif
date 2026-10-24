#include <Xm/Calendar.h>
#include <Xm/Accessible.h>
#include <assert.h>
#include <stdio.h>

static int changes;
static int activations;

static void changed(Widget widget, XtPointer closure, XtPointer call_data) {
    XmCalendarCallbackStruct *callback = (XmCalendarCallbackStruct *)call_data;
    (void)widget; (void)closure;
    assert(callback->reason == 1);
    ++changes;
}

static void activated(Widget widget, XtPointer closure, XtPointer call_data) {
    XmCalendarCallbackStruct *callback = (XmCalendarCallbackStruct *)call_data;
    (void)widget; (void)closure;
    assert(callback->reason == 2);
    ++activations;
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, calendar;
    XmCalendarDate date, minimum, maximum;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId accessible_id;
    int year, month;

    shell = XtVaAppInitialize(&app, "CalendarFixture", NULL, 0,
                              &argc, argv, NULL, NULL);
    calendar = XmCreateCalendar(shell, "calendar", NULL, 0);
    XtAddCallback(calendar, XmNcalendarDateChangedCallback, changed, NULL);
    XtAddCallback(calendar, XmNcalendarActivateCallback, activated, NULL);
    XtManageChild(calendar);

    assert(XmCalendarIsValidDate(&(XmCalendarDate){2024, 2, 29}));
    assert(!XmCalendarIsValidDate(&(XmCalendarDate){2023, 2, 29}));
    assert(XmCalendarSetDate(calendar, &(XmCalendarDate){2024, 2, 29}, True));
    assert(changes == 1);
    assert(XmCalendarGetDate(calendar, &date));
    assert(date.year == 2024 && date.month == 2 && date.day == 29);
    assert(XmCalendarSetVisibleMonth(calendar, 2024, 2));
    assert(XmCalendarGetVisibleMonth(calendar, &year, &month));
    assert(year == 2024 && month == 2);

    minimum = (XmCalendarDate){2024, 2, 1};
    maximum = (XmCalendarDate){2024, 3, 31};
    assert(XmCalendarSetRange(calendar, &minimum, &maximum));
    assert(!XmCalendarSetDate(calendar, &(XmCalendarDate){2024, 1, 31}, True));
    assert(changes == 1);
    assert(!XmCalendarSetRange(calendar, &(XmCalendarDate){2024, 4, 1},
                               &maximum));
    assert(XmCalendarGetRange(calendar, &minimum, &maximum));
    assert(minimum.month == 2 && maximum.month == 3);
    assert(XmCalendarSetDate(calendar, &(XmCalendarDate){2024, 3, 31}, True));
    assert(changes == 2);

    XtRealizeWidget(shell);
    accessibility = XmAccessibilityCreate(shell, "CalendarFixture");
    assert(accessibility != NULL);
    accessible_id = XmAccessibilityWidget(accessibility, calendar);
    assert(accessible_id != XmACCESSIBLE_NONE);
    assert(XmAccessibilityQuery(accessibility, accessible_id, &info));
    assert(info.role == XmACCESSIBLE_CALENDAR && info.has_selection &&
           info.has_value && info.current_value == 2024);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);

    XtDestroyWidget(shell);
    puts("PASS calendar: 16 checks");
    return 0;
}
