/* Installed-consumer proof for the deterministic display-scale override. */
#include <Xm/Display.h>
#include <Xm/ProgressRing.h>
#include <Xm/SpinB.h>
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

static Dimension
scaled_dimension(Dimension baseline, double scale)
{
    return (Dimension)(baseline * scale + 0.5);
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget baseline_toggle;
    Widget scaled_toggle;
    Widget explicit_toggle;
    Widget baseline_spin;
    Widget explicit_spin;
    Widget baseline_ring;
    Widget explicit_ring;
    Display *display;
    XrmDatabase database;
    double scale;
    Dimension baseline_indicator;
    Dimension live_indicator;
    Dimension scaled_indicator;
    Dimension explicit_indicator;
    Dimension baseline_arrow;
    Dimension live_arrow;
    Dimension explicit_arrow;
    Dimension baseline_ring_height;
    Dimension live_ring_height;
    Dimension explicit_ring_height;
    const double gallery_scales[] = {1.28, 1.92, 2.56, 3.84};
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

    baseline_spin = XmCreateSpinBox(shell, "baselineSpin", NULL, 0);
    XtManageChild(baseline_spin);
    baseline_ring = XmCreateProgressRing(shell, "baselineRing", NULL, 0);
    XtManageChild(baseline_ring);
    XtRealizeWidget(shell);

    XtVaGetValues(scaled_toggle, XmNindicatorSize, &scaled_indicator, NULL);
    XtVaGetValues(explicit_toggle, XmNindicatorSize, &explicit_indicator, NULL);
    XtVaGetValues(baseline_spin, XmNarrowSize, &baseline_arrow, NULL);
    XtVaGetValues(baseline_ring, XtNheight, &baseline_ring_height, NULL);
    if (baseline_indicator == 0 || live_indicator <= baseline_indicator ||
        scaled_indicator <= baseline_indicator)
        return fail("scaled default metric did not increase");
    if (explicit_indicator != 7)
        return fail("explicit pixel metric was changed by scale override");
    if (baseline_arrow == 0)
        return fail("spin box default metric was not initialized");
    if (baseline_ring_height == 0)
        return fail("progress ring default metric was not initialized");

    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("one-times live override was not retained for spin box");
    XSync(display, False);
    XtVaGetValues(baseline_spin, XmNarrowSize, &live_arrow, NULL);
    if (live_arrow >= baseline_arrow)
        return fail("spin box default metric did not reflow down");
    XtVaGetValues(baseline_ring, XtNheight, &live_ring_height, NULL);
    if (live_ring_height >= baseline_ring_height)
        return fail("progress ring default metric did not reflow down");

    explicit_ring = XmCreateProgressRing(shell, "explicitRing", NULL, 0);
    XtVaSetValues(explicit_ring, XtNheight, (Dimension)100, NULL);
    XtManageChild(explicit_ring);
    XtVaGetValues(explicit_ring, XtNheight, &explicit_ring_height, NULL);
    if (explicit_ring_height != 100)
        return fail("progress ring explicit height was not retained");

    explicit_spin = XmCreateSpinBox(shell, "explicitSpin", NULL, 0);
    XtVaSetValues(explicit_spin, XmNarrowSize, (Dimension)16, NULL);
    XtManageChild(explicit_spin);
    XtVaGetValues(explicit_spin, XmNarrowSize, &explicit_arrow, NULL);
    if (explicit_arrow != 16)
        return fail("spin box explicit pixel metric was not retained");

    if (!XmSetDisplayScaleOverride(display, 2.0))
        return fail("two-times live override was not retained for spin box");
    XSync(display, False);
    XtVaGetValues(explicit_ring, XtNheight, &explicit_ring_height, NULL);
    if (explicit_ring_height != 100)
        return fail("progress ring explicit height changed by scale override");
    XtVaGetValues(explicit_spin, XmNarrowSize, &explicit_arrow, NULL);
    if (explicit_arrow != 16)
        return fail("spin box explicit pixel metric changed by scale override");

    for (size_t i = 0; i < XtNumber(gallery_scales); ++i) {
        Dimension indicator, arrow, ring_height;
        Dimension expected_indicator = scaled_dimension(9, gallery_scales[i]);
        Dimension expected_arrow = scaled_dimension(16, gallery_scales[i]);
        Dimension expected_ring = scaled_dimension(64, gallery_scales[i]) + 16;
        if (!XmSetDisplayScaleOverride(display, gallery_scales[i]))
            return fail("gallery DPI override was not retained");
        XSync(display, False);
        XtVaGetValues(baseline_toggle, XmNindicatorSize, &indicator, NULL);
        XtVaGetValues(baseline_spin, XmNarrowSize, &arrow, NULL);
        XtVaGetValues(baseline_ring, XtNheight, &ring_height, NULL);
        if (indicator != expected_indicator || arrow != expected_arrow ||
            ring_height != expected_ring)
            return fail("gallery DPI matrix produced an unexpected default");
    }

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
