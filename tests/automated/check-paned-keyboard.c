/* Native sash batching: cancellation must not create a second timeout. */
#include <Xm/PanedWP.h>
#include <Xm/Label.h>
#include <X11/StringDefs.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", \
    __FILE__, __LINE__, #x); exit(1); } ++checks; } while (0)
static unsigned long checks;
static Widget watched;
static XtIntervalId watched_id;
static unsigned scheduled, removed;

XtIntervalId XtAppAddTimeOut(XtAppContext app, unsigned long delay,
                            XtTimerCallbackProc callback, XtPointer closure)
{
    typedef XtIntervalId (*AddProc)(XtAppContext, unsigned long,
                                   XtTimerCallbackProc, XtPointer);
    AddProc real = (AddProc)dlsym(RTLD_NEXT, "XtAppAddTimeOut");
    XtIntervalId id;
    if (!real) abort();
    id = real(app, delay, callback, closure);
    if (watched != NULL && closure == (XtPointer)watched) {
        ++scheduled;
        watched_id = id;
    }
    return id;
}

void XtRemoveTimeOut(XtIntervalId id)
{
    typedef void (*RemoveProc)(XtIntervalId);
    RemoveProc real = (RemoveProc)dlsym(RTLD_NEXT, "XtRemoveTimeOut");
    if (!real) abort();
    if (watched != NULL && id == watched_id) ++removed;
    real(id);
}

static void wake(XtPointer closure, XtIntervalId *id)
{
    (void)id;
    *(Boolean *)closure = True;
}

static void drain(XtAppContext app)
{
    Boolean done = False;
    XtAppAddTimeOut(app, 100, wake, &done);
    while (!done) XtAppProcessEvent(app, XtIMAll);
}

static void key(Widget sash, String increment, String direction)
{
    String params[] = {"Key", increment, direction};
    XEvent event;
    memset(&event, 0, sizeof(event));
    event.xkey.type = KeyPress;
    event.xkey.display = XtDisplay(sash);
    event.xkey.window = XtWindow(sash);
    XtCallActionProc(sash, "SashAction", &event, params, XtNumber(params));
}

static Dimension major(Widget pane, Boolean horizontal)
{
    Dimension size;
    XtVaGetValues(pane, horizontal ? XmNwidth : XmNheight, &size, NULL);
    return size;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Display *display;
    unsigned orientation, large, mode, repeat;
    shell = XtVaAppInitialize(&app, "PanedKeyboard", NULL, 0, &argc, argv,
                             NULL, XtNwidth, 500, XtNheight, 500, NULL);
    display = XtDisplay(shell);
    XtSetMultiClickTime(display, 20);
    XtRealizeWidget(shell);
    for (orientation = 0; orientation < 2; ++orientation) {
        for (large = 0; large < 2; ++large) {
            for (mode = 0; mode < 3; ++mode) {
                Boolean horizontal = orientation != 0;
                String increment = large ? "LargeIncr" : "DefaultIncr";
                String backward = horizontal ? "Left" : "Up";
                String forward = horizontal ? "Right" : "Down";
                int step = large ? 10 : 1;
                Widget paned = XtVaCreateManagedWidget("paned",
                    xmPanedWindowWidgetClass, shell, XmNorientation,
                    horizontal ? XmHORIZONTAL : XmVERTICAL, NULL);
                Widget upper = XtVaCreateWidget("upper",
                    xmLabelWidgetClass, paned, XmNwidth, 180, XmNheight, 180,
                    XmNpaneMinimum, 40, XmNpaneMaximum, 440, NULL);
                Widget lower = XtVaCreateWidget("lower",
                    xmLabelWidgetClass, paned, XmNwidth, 180, XmNheight, 180,
                    XmNpaneMinimum, 40, XmNpaneMaximum, 440, NULL);
                XmPanedWindowWidget pw = (XmPanedWindowWidget)paned;
                XmPanedWindowConstraintPtr constraint =
                    (XmPanedWindowConstraintPtr)upper->core.constraints;
                Widget sash = constraint->panedw.sash;
                Dimension before_upper, before_lower;
                XtIntervalId pending;
                Widget panes[] = {upper, lower};
                /* Manage together so the first pane is not stretched alone
                 * to its maximum before its neighbor is inserted. */
                XtManageChildren(panes, XtNumber(panes));
                CHECK(sash != NULL && XtIsManaged(sash));
                drain(app);
                before_upper = major(upper, horizontal);
                before_lower = major(lower, horizontal);
                CHECK(before_upper > 60 && before_upper < 420);
                CHECK(before_lower > 60 && before_lower < 420);
                watched = sash;
                watched_id = 0;
                scheduled = removed = 0;
                key(sash, increment, backward);
                pending = pw->paned_window.timer;
                CHECK(pending != 0);
                key(sash, increment, forward);
                for (repeat = 0; repeat < 25; ++repeat) {
                    CHECK(pw->paned_window.increment_count == 0);
                    CHECK(pw->paned_window.timer == pending);
                    key(sash, increment, backward);
                    key(sash, increment, forward);
                    CHECK(scheduled == 1);
                }
                CHECK(major(upper, horizontal) == before_upper);
                CHECK(major(lower, horizontal) == before_lower);
                if (mode != 0) key(sash, increment, forward);
                CHECK(scheduled == 1);
                CHECK(pw->paned_window.timer == pending);
                if (mode == 2) {
                    XtDestroyWidget(paned);
                    CHECK(removed == 1);
                    watched = NULL;
                    drain(app);
                    continue;
                }
                drain(app);
                CHECK(pw->paned_window.timer == 0);
                CHECK(pw->paned_window.increment_count == 0);
                CHECK((int)major(upper, horizontal) ==
                      (int)before_upper + (mode ? step : 0));
                CHECK((int)major(lower, horizontal) ==
                      (int)before_lower - (mode ? step : 0));
                CHECK(removed == 0);
                /* A completed batch must allow a fresh batch, including
                 * after cancellation without any movement. */
                key(sash, increment, mode ? backward : forward);
                CHECK(scheduled == 2);
                drain(app);
                CHECK(pw->paned_window.timer == 0);
                CHECK((int)major(upper, horizontal) ==
                      (int)before_upper + (mode ? 0 : step));
                CHECK((int)major(lower, horizontal) ==
                      (int)before_lower - (mode ? 0 : step));
                watched = NULL;
                XtDestroyWidget(paned);
            }
        }
    }
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS PanedWindow cancellation, movement and destruction: %lu checks\n", checks);
    return 0;
}
