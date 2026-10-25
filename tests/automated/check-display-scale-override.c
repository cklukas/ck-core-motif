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
    Widget explicit_toggle;
    Display *display;
    XrmDatabase database;
    double scale;
    Dimension baseline_indicator;
    Dimension live_indicator;
    Dimension scaled_indicator;
    Dimension explicit_indicator;
    XmString empty_label;
    Arg empty_args[1];
    Arg explicit_args[2];

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

    XtRealizeWidget(shell);
    XtVaGetValues(baseline_toggle, XmNindicatorSize, &baseline_indicator,
                  NULL);
    XmToggleButtonSetState(baseline_toggle, True, False);
    if (!XmToggleButtonGetState(baseline_toggle))
        return fail("toggle state could not be established before reflow");

    if (!XmSetDisplayScaleOverride(display, 2.0) ||
        !XmGetDisplayScaleOverride(display, &scale) || scale != 2.0)
        return fail("live override was not retained");
    XSync(display, False);
    XtVaGetValues(baseline_toggle, XmNindicatorSize, &live_indicator, NULL);
    if (live_indicator <= baseline_indicator)
        return fail("realized default metric did not reflow");
    if (!XmToggleButtonGetState(baseline_toggle))
        return fail("toggle state changed during reflow");

    empty_label = XmStringCreateLocalized("");
    XtSetArg(empty_args[0], XmNlabelString, empty_label);
    scaled_toggle = XmCreateToggleButton(shell, "scaled", empty_args, 1);
    XmStringFree(empty_label);
    XtManageChild(scaled_toggle);
    empty_label = XmStringCreateLocalized("");
    XtSetArg(explicit_args[0], XmNlabelString, empty_label);
    XtSetArg(explicit_args[1], XmNindicatorSize, (Dimension)7);
    explicit_toggle = XmCreateToggleButton(shell, "explicit", explicit_args, 2);
    XmStringFree(empty_label);
    XtManageChild(explicit_toggle);
    XtRealizeWidget(shell);

    XtVaGetValues(scaled_toggle, XmNindicatorSize, &scaled_indicator, NULL);
    XtVaGetValues(explicit_toggle, XmNindicatorSize, &explicit_indicator, NULL);
    if (baseline_indicator == 0 || live_indicator <= baseline_indicator ||
        scaled_indicator <= baseline_indicator)
        return fail("scaled default metric did not increase");
    if (explicit_indicator != 7)
        return fail("explicit pixel metric was changed by scale override");

    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("one-times live override was not retained");
    XSync(display, False);
    XtVaGetValues(baseline_toggle, XmNindicatorSize, &live_indicator, NULL);
    if (live_indicator != baseline_indicator)
        return fail("live scale reflow was not reversible");
    if (!XmClearDisplayScaleOverride(display) ||
        XmGetDisplayScaleOverride(display, &scale))
        return fail("override did not clear");
    XtDestroyWidget(shell);
    puts("PASS WP-05.2 display-scale override");
    return 0;
}
