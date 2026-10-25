/* Exercise undersized ToggleButton drawing through the installed public API. */
#include <Xm/ToggleB.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <stdio.h>

static int x_errors;

static int
record_x_error(Display *display, XErrorEvent *event)
{
    (void)display;
    (void)event;
    x_errors++;
    return 0;
}

static void
drain_events(XtAppContext app)
{
    int guard = 100;

    while (guard-- > 0 && XtAppPending(app))
        XtAppProcessEvent(app, XtIMAll);
}

static int
fail(const char *message)
{
    fprintf(stderr, "toggle geometry: %s\n", message);
    return 1;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget shadow;
    Widget fill;
    Display *display;
    XmString label;

    shell = XtVaAppInitialize(&app, "ToggleGeometry", NULL, 0,
                              &argc, argv,
                              NULL,
                              XmNwidth, (Dimension)4,
                              XmNheight, (Dimension)4,
                              NULL);
    display = XtDisplay(shell);
    XSetErrorHandler(record_x_error);

    label = XmStringCreateLocalized("");
    shadow = XtVaCreateManagedWidget(
        "shadow", xmToggleButtonWidgetClass, shell,
        XmNlabelString, label,
        XmNwidth, (Dimension)1,
        XmNheight, (Dimension)1,
        XmNhighlightThickness, (Dimension)2,
        XmNshadowThickness, (Dimension)2,
        XmNindicatorOn, True,
        NULL);
    XmStringFree(label);

    label = XmStringCreateLocalized("");
    fill = XtVaCreateManagedWidget(
        "fill", xmToggleButtonWidgetClass, shell,
        XmNlabelString, label,
        XmNwidth, (Dimension)1,
        XmNheight, (Dimension)1,
        XmNhighlightThickness, (Dimension)2,
        XmNshadowThickness, (Dimension)2,
        XmNindicatorOn, False,
        XmNfillOnSelect, True,
        NULL);
    XmStringFree(label);

    XtRealizeWidget(shell);
    XmToggleButtonSetState(fill, True, False);
    XClearArea(display, XtWindow(shadow), 0, 0, 0, 0, True);
    XClearArea(display, XtWindow(fill), 0, 0, 0, 0, True);
    XSync(display, False);
    drain_events(app);
    XSync(display, False);

    if (x_errors != 0)
        return fail("undersized drawing generated an X protocol error");

    printf("PASS ToggleButton undersized geometry\n");
    return 0;
}
