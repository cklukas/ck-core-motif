/* Installed-consumer proof for the deterministic display-scale override. */
#include <Xm/Display.h>
#include <Xm/ToggleB.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <math.h>
#include <stdio.h>

static int
fail(const char *message)
{
    fprintf(stderr, "display-scale override: %s\n", message);
    return 1;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget baseline_toggle;
    Widget scaled_toggle;
    Display *display;
    XrmDatabase database;
    double scale;
    Dimension baseline_indicator;
    Dimension scaled_indicator;
    XmString empty_label;
    Arg empty_args[1];

    shell = XtVaAppInitialize(&app, "DisplayScaleOverride", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    database = XrmGetDatabase(display);

    if (XmGetDisplayScaleOverride(display, &scale))
        return fail("override unexpectedly existed before selection");
    if (XmSetDisplayScaleOverride(display, 0.0) ||
        XmSetDisplayScaleOverride(display, -1.0) ||
        XmSetDisplayScaleOverride(display, NAN) ||
        XmSetDisplayScaleOverride(display, INFINITY))
        return fail("invalid override was accepted");
    if (!XmSetDisplayScaleOverride(display, 1.0) ||
        !XmGetDisplayScaleOverride(display, &scale) || scale != 1.0)
        return fail("one-times override was not retained");
    if (XrmGetDatabase(display) != database)
        return fail("override mutated the X resource database");

    empty_label = XmStringCreateLocalized("");
    XtSetArg(empty_args[0], XmNlabelString, empty_label);
    baseline_toggle = XmCreateToggleButton(shell, "baseline", empty_args, 1);
    XmStringFree(empty_label);
    XtManageChild(baseline_toggle);

    if (!XmSetDisplayScaleOverride(display, 2.0) ||
        !XmGetDisplayScaleOverride(display, &scale) || scale != 2.0)
        return fail("updated override was not retained");
    empty_label = XmStringCreateLocalized("");
    XtSetArg(empty_args[0], XmNlabelString, empty_label);
    scaled_toggle = XmCreateToggleButton(shell, "scaled", empty_args, 1);
    XmStringFree(empty_label);
    XtManageChild(scaled_toggle);
    XtRealizeWidget(shell);

    XtVaGetValues(baseline_toggle, XmNindicatorSize, &baseline_indicator, NULL);
    XtVaGetValues(scaled_toggle, XmNindicatorSize, &scaled_indicator, NULL);
    if (baseline_indicator == 0 || scaled_indicator <= baseline_indicator)
        return fail("scaled default metric did not increase");

    if (!XmClearDisplayScaleOverride(display) ||
        XmGetDisplayScaleOverride(display, &scale))
        return fail("override did not clear");
    XtDestroyWidget(shell);
    puts("PASS WP-05.2 display-scale override");
    return 0;
}
