/* Run with the freshly compiled demo UID files on an isolated X display.
 * Include the demo so the regression exercises its actual callbacks and loop. */
#include <assert.h>
#include <stdio.h>
#include <Xm/Xm.h>
static void check_animation(XtAppContext context);
#define XtAppMainLoop check_animation
#include "../../demos/programs/animate/xmanimate.c"
#undef XtAppMainLoop

static void check_animation(XtAppContext context)
{
    Widget shell = drawingArea;
    Widget scale, start, halt, step;
    int minimum, maximum, value, before, i;
    XmScaleCallbackStruct change = {0};
    (void)context;
    while (XtParent(shell)) shell = XtParent(shell);
    scale = XtNameToWidget(shell, "*motifanim_speed");
    start = XtNameToWidget(shell, "*motifanim_start");
    halt = XtNameToWidget(shell, "*motifanim_stop");
    step = XtNameToWidget(shell, "*motifanim_step");
    assert(scale && start && halt && step);
    XtVaGetValues(scale, XmNminimum, &minimum, XmNmaximum, &maximum,
                  XmNvalue, &value, NULL);
    assert(minimum == 1 && maximum == 100 && value == minimum);
    assert(scalespeed == minimum);
    XtCallCallbacks(start, XmNactivateCallback, NULL);
    assert(!stop);
    before = xanim;
    for (i = 0; i < maximum * speed_factor; ++i) fstep(&scalespeed);
    assert(xanim != before);
    /* Return to the left endpoint after changing speed through the callback. */
    change.value = maximum;
    XtCallCallbacks(scale, XmNvalueChangedCallback, &change);
    assert(scalespeed == maximum);
    XmScaleSetValue(scale, minimum);
    change.value = minimum;
    XtCallCallbacks(scale, XmNdragCallback, &change);
    before = xanim;
    for (i = 0; i < maximum * speed_factor; ++i) fstep(&scalespeed);
    assert(xanim != before);
    XtCallCallbacks(halt, XmNactivateCallback, NULL);
    assert(stop);
    assert(fstep(&scalespeed)); /* Work procedure retires on Stop. */
    before = xanim;
    XtCallCallbacks(step, XmNactivateCallback, NULL);
    assert(xanim != before && stop);
    puts("PASS: initial/minimum speed advances; speed changes, Stop and Step work");
}
