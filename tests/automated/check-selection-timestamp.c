/* Regression: format-32 selection lengths count logical items, not bytes.
 * Build against an installed toolkit:
 *   cc check-selection-timestamp.c -I$prefix/include -L$prefix/lib \
 *      -Wl,-rpath,$prefix/lib -lXm -lXt -lX11 -o check-selection-timestamp
 * Run with an X server available, for example via xvfb-run.
 */
#include <Xm/RowColumn.h>
#include <Xm/DataF.h>
#include <Xm/DataFSelP.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const char *name;
    Time expected;
    Boolean done;
    int failed;
} Check;

static Bool
timestamp_event(Display *display, XEvent *event, XPointer data)
{
    XPropertyEvent *wanted = (XPropertyEvent *)data;
    (void)display;
    return event->type == PropertyNotify &&
           event->xproperty.window == wanted->window &&
           event->xproperty.atom == wanted->atom;
}

static Time
server_time(Widget shell)
{
    Display *display = XtDisplay(shell);
    XWindowAttributes attributes;
    XEvent event;
    XPropertyEvent wanted;
    unsigned char value = 0;

    wanted.window = XtWindow(shell);
    wanted.atom = XInternAtom(display, "_XM_TEST_TIMESTAMP", False);
    XGetWindowAttributes(display, wanted.window, &attributes);
    XSelectInput(display, wanted.window,
                 attributes.your_event_mask | PropertyChangeMask);
    XChangeProperty(display, wanted.window, wanted.atom, XA_INTEGER,
                    8, PropModeReplace, &value, 1);
    XIfEvent(display, &event, timestamp_event, (XPointer)&wanted);
    return event.xproperty.time;
}

static void
received(Widget widget, XtPointer client_data, Atom *selection,
         Atom *type, XtPointer value, unsigned long *length, int *format)
{
    Check *check = (Check *)client_data;
    (void)widget;
    (void)selection;
    if (*type != XA_INTEGER || *format != 32 || *length != 1 ||
        value == NULL || *(Time *)value != check->expected) {
        fprintf(stderr, "%s TIMESTAMP: type=%lu format=%d length=%lu "
                        "expected one INTEGER/32 item with time=%lu\n",
                check->name, *type, *format, *length, check->expected);
        check->failed = 1;
    }
    XtFree((char *)value);
    check->done = True;
}

static void
timed_out(XtPointer client_data, XtIntervalId *id)
{
    Check *check = (Check *)client_data;
    (void)id;
    fprintf(stderr, "%s TIMESTAMP request timed out\n", check->name);
    /* Do not leave a pending selection callback pointing to expired state. */
    exit(1);
}

static int
check_widget(XtAppContext app, Widget shell, Widget widget,
             const char *name, int kind)
{
    Display *display = XtDisplay(widget);
    Check check = {name, server_time(shell), False, 0};
    XtIntervalId timeout;
    Atom timestamp = XInternAtom(display, "TIMESTAMP", False);

    if (kind == 2)
        XmDataFieldSetSelection(widget, 0, 3, check.expected);
    else if (kind == 1)
        XmTextFieldSetSelection(widget, 0, 3, check.expected);
    else
        XmTextSetSelection(widget, 0, 3, check.expected);
    if (XGetSelectionOwner(display, XA_PRIMARY) != XtWindow(widget)) {
        fprintf(stderr, "%s did not acquire PRIMARY\n", name);
        return 1;
    }
    timeout = XtAppAddTimeOut(app, 5000, timed_out, &check);
    XtGetSelectionValue(widget, XA_PRIMARY, timestamp, received, &check,
                        check.expected);
    while (!check.done)
        XtAppProcessEvent(app, XtIMAll);
    XtRemoveTimeOut(timeout);
    if (!check.failed)
        printf("PASS %s PRIMARY TIMESTAMP: one INTEGER/32 item, time=%lu\n",
               name, check.expected);
    return check.failed;
}

/* Exercise the converter directly so SECONDARY coverage does not depend on
 * simulated pointer gestures. PRIMARY timestamps above use real ownership and
 * Xt's selection request API for each widget class.
 */
static int
check_data_targets(Widget field)
{
    Display *display = XtDisplay(field);
    Atom target = XInternAtom(display, "TARGETS", False);
    Atom insert = XInternAtom(display, "INSERT_SELECTION", False);
    Atom selections[] = {XA_PRIMARY, XA_SECONDARY};
    unsigned long expected[] = {10, 8};
    int failed = 0;
    unsigned int i;

    for (i = 0; i < XtNumber(selections); ++i) {
        Atom type = None;
        XtPointer value = NULL;
        unsigned long length = 0;
        int format = 0;
        unsigned long j;
        int found_insert = 0;
        Boolean converted = _XmDataFieldConvert(field, &selections[i], &target,
                                                &type, &value, &length, &format);
        if (!converted || type != XA_ATOM || format != 32 ||
            length != expected[i] || value == NULL) {
            fprintf(stderr, "DataField %s TARGETS: converted=%d type=%lu "
                            "format=%d length=%lu, expected %lu atoms\n",
                    i == 0 ? "PRIMARY" : "SECONDARY", converted, type,
                    format, length, expected[i]);
            failed = 1;
        } else {
            for (j = 0; j < length; ++j)
                if (((Atom *)value)[j] == insert)
                    ++found_insert;
            if (found_insert != (i == 0 ? 1 : 0)) {
                fprintf(stderr, "DataField TARGETS has incorrect "
                                "INSERT_SELECTION membership\n");
                failed = 1;
            } else {
                printf("PASS DataField %s TARGETS: %lu initialized atoms\n",
                       i == 0 ? "PRIMARY" : "SECONDARY", length);
            }
        }
        XtFree((char *)value);
    }
    return failed;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "SelectionTimestamp", NULL, 0,
                                    &argc, argv, NULL, NULL);
    Widget row = XmCreateRowColumn(shell, "row", NULL, 0);
    Widget text = XmCreateText(row, "text", NULL, 0);
    Widget field = XmCreateTextField(row, "field", NULL, 0);
    Widget data = XmCreateDataField(row, "data", NULL, 0);
    int failed;

    XmTextSetString(text, "abc");
    XmTextFieldSetString(field, "abc");
    XmDataFieldSetString(data, "abc");
    XtManageChild(text);
    XtManageChild(field);
    XtManageChild(data);
    XtManageChild(row);
    XtRealizeWidget(shell);
    failed = check_widget(app, shell, text, "Text", 0);
    failed |= check_widget(app, shell, field, "TextField", 1);
    failed |= check_widget(app, shell, data, "DataField", 2);
    failed |= check_data_targets(data);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    return failed ? 1 : 0;
}
