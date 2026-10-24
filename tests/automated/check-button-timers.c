/* Repeated keyboard activation must own at most one release animation timer.
 * Build against an installed prefix, then run under an isolated Xvfb.
 */
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/DrawnB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/GadgetP.h>
#include <X11/StringDefs.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", \
    __FILE__, __LINE__, #x); exit(1); } ++checks; } while (0)
static unsigned long checks;
static unsigned activated;
static Widget watched;
static unsigned scheduled;

XtIntervalId XtAppAddTimeOut(XtAppContext app, unsigned long delay,
                            XtTimerCallbackProc callback, XtPointer closure)
{
    typedef XtIntervalId (*AddProc)(XtAppContext, unsigned long,
                                   XtTimerCallbackProc, XtPointer);
    AddProc real = (AddProc)dlsym(RTLD_NEXT, "XtAppAddTimeOut");
    if (!real) abort();
    if (watched != NULL && closure == (XtPointer)watched) ++scheduled;
    return real(app, delay, callback, closure);
}

static void activation(Widget w, XtPointer closure, XtPointer data)
{
    (void)w;
    (void)closure;
    CHECK(data != NULL);
    CHECK(((XmAnyCallbackStruct *)data)->reason == XmCR_ACTIVATE);
    ++activated;
}

static void wake(XtPointer closure, XtIntervalId *id)
{
    (void)id;
    *(Boolean *)closure = True;
}

static void drain(XtAppContext app)
{
    Boolean done = False;
    XtAppAddTimeOut(app, 400, wake, &done);
    while (!done) XtAppProcessEvent(app, XtIMAll);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row;
    Display *display;
    WidgetClass classes[] = {xmArrowButtonWidgetClass, xmArrowButtonGadgetClass,
        xmDrawnButtonWidgetClass, xmPushButtonWidgetClass, xmPushButtonGadgetClass};
    unsigned type, cycle, repeat;
    shell = XtVaAppInitialize(&app, "ButtonTimers", NULL, 0, &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    XtRealizeWidget(shell);
    for (type = 0; type < XtNumber(classes); ++type) {
        for (cycle = 0; cycle < 3; ++cycle) {
            XEvent event;
            Widget button = XtVaCreateManagedWidget("button", classes[type], row,
                                XmNpushButtonEnabled, True, NULL);
            unsigned before = activated;
            watched = button;
            scheduled = 0;
            XtAddCallback(button, XmNactivateCallback, activation, NULL);
            memset(&event, 0, sizeof(event));
            event.xkey.type = KeyPress;
            event.xkey.display = display;
            event.xkey.window = XtWindow(XtParent(button));
            event.xkey.time = XtLastTimestampProcessed(display);
            for (repeat = 0; repeat < 20; ++repeat) {
                if (XmIsGadget(button)) {
                    XmGadgetClass klass = (XmGadgetClass)XtClass(button);
                    klass->gadget_class.arm_and_activate(button, &event, NULL, NULL);
                } else XtCallActionProc(button, "ArmAndActivate", &event, NULL, 0);
            }
            CHECK(activated == before + 20);
            CHECK(scheduled == 1);
            XtDestroyWidget(button);
            watched = NULL;
            /* Real timers dispatch after the owning allocation has retired. */
            drain(app);
        }
    }
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS repeated activation/destruction in five button classes: %lu checks\n", checks);
    return 0;
}
