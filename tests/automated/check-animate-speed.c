/* Exercise the actual demo timer callbacks on an isolated X display. */
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <Xm/Xm.h>
static void check_animation(XtAppContext context);
#define XtAppMainLoop check_animation
#ifdef TEST_LEGACY_ANIMATE
#include "../../demos/programs/animate/animate.c"
#else
#include "../../demos/programs/animate/xmanimate.c"
#endif
#undef XtAppMainLoop

static void end_observation(XtPointer data, XtIntervalId *id)
{
    (void)id;
    *(Boolean *)data = True;
}

static int observe(unsigned long milliseconds)
{
    Boolean done = False;
    int count = 0, before;
    XtAppAddTimeOut(app_context, milliseconds, end_observation, &done);
    while (!done) {
        before = xanim;
        XtAppProcessEvent(app_context, XtIMTimer);
        if (xanim != before) ++count;
    }
    return count;
}

static void check_animation(XtAppContext context)
{
    Widget shell = drawingArea;
    Widget scale, start, halt, step;
    int minimum, maximum, value, before, frames;
    clock_t cpu;
    XtIntervalId first;
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
    assert(scalespeed == minimum && frame_interval() == 500);
    speed_factor = 1;
    assert(frame_interval() == 25);
    speed_factor = 100;
    assert(frame_interval() == 2500);
    speed_factor = 20;
    assert(stop && !timer_pending && observe(100) == 0);
    XtCallCallbacks(start, XmNactivateCallback, NULL);
    assert(!stop && timer_pending);
    first = animation_timer;
    XtCallCallbacks(start, XmNactivateCallback, NULL);
    assert(animation_timer == first); /* Start is idempotent. */
    cpu = clock();
    frames = observe(1150);
    assert(frames == 2);
    assert((double)(clock() - cpu) / CLOCKS_PER_SEC < 0.15);
    printf("minimum: %d frames in 1150 ms; no busy loop\n", frames);
    change.value = maximum;
    XtCallCallbacks(scale, XmNvalueChangedCallback, &change);
    assert(scalespeed == maximum && frame_interval() == 20);
    frames = observe(250);
    assert(frames >= 8 && frames <= 13);
    printf("maximum: %d frames in 250 ms\n", frames);
    /* A delayed event loop must not produce a burst of overdue frames. */
    {
        struct timespec delay = {0, 120000000};
        nanosleep(&delay, NULL);
        XtAppProcessEvent(app_context, XtIMTimer);
        assert(observe(5) == 0);
    }
    XmScaleSetValue(scale, minimum);
    change.value = minimum;
    XtCallCallbacks(scale, XmNdragCallback, &change);
    assert(frame_interval() == 500 && observe(200) == 0);
    assert(observe(400) == 1);
    XtCallCallbacks(halt, XmNactivateCallback, NULL);
    assert(stop && !timer_pending && observe(550) == 0);
    before = xanim;
    XtCallCallbacks(step, XmNactivateCallback, NULL);
    assert(xanim != before && stop && !timer_pending);
    assert(observe(100) == 0);
    XtCallCallbacks(start, XmNactivateCallback, NULL);
    assert(observe(550) == 1);
    XtDestroyWidget(shell);
    assert(stop && !timer_pending && observe(550) == 0);
    puts("PASS: timed playback, repeated Start, speed changes, Stop, Step and destruction");
}
