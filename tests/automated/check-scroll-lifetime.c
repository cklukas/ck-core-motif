/* Native registered drag-procedure and timer-lifetime regression. */
#include <Xm/Xm.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/DrawingA.h>
#include <Xm/DropSMgr.h>
#include <Xm/ScrolledWP.h>
#include <Xm/TraitP.h>
#include <Xm/ScrollFrameT.h>
#include <Xm/NavigatorT.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", \
    __FILE__, __LINE__, #x); exit(1); } ++checks; } while (0)
static unsigned checks;
static Widget scroll, scrollbar;
static XtCallbackProc drag;
static unsigned changed;
static int callback_action;

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

static void leave(void)
{
    XmDragProcCallbackStruct data;
    memset(&data, 0, sizeof(data));
    data.reason = XmCR_DROP_SITE_LEAVE_MESSAGE;
    drag(scrollbar, NULL, &data);
}

static void value_changed(Widget w, XtPointer closure, XtPointer data)
{
    (void)w;
    (void)closure;
    CHECK(data != NULL);
    ++changed;
    if (changed != 1) return;
    if (callback_action == 1)
        XtVaSetValues(scroll, XmNautoDragModel, XmAUTO_DRAG_DISABLED, NULL);
    else if (callback_action == 2)
        leave();
    else if (callback_action == 3)
        XtDestroyWidget(scroll);
}

int main(int argc, char **argv)
{
    unsigned scenario;
    /* Keep the isolated server alive while independent Xt displays close;
     * otherwise Xvfb's automatic server reset races the next connection. */
    Display *anchor = XOpenDisplay(NULL);
    CHECK(anchor != NULL);
    for (scenario = 0; scenario < 8; ++scenario) {
        XtAppContext app;
        Widget shell, horizontal;
        XmScrollFrameTrait frame;
        Display *display;
        Arg arg;
        XmDragProcCallbackStruct data;
        XmScrolledWindowWidget sw;
        int value = 0;
        fprintf(stderr, "autoscroll scenario %u\n", scenario);
        shell = XtVaAppInitialize(&app, "ScrollLifetime", NULL, 0, &argc, argv,
                    NULL, XmNwidth, 300, XmNheight, 200, NULL);
        display = XtDisplay(shell);
        scroll = XtVaCreateManagedWidget("scroll", xmScrolledWindowWidgetClass,
                    shell, XmNscrollingPolicy, XmAUTOMATIC,
                    XmNautoDragModel, XmAUTO_DRAG_ENABLED, NULL);
        XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass, scroll,
                    XmNwidth, 1000, XmNheight, 1000, NULL);
        XtRealizeWidget(shell);
        XtVaGetValues(scroll, XmNverticalScrollBar, &scrollbar, NULL);
        CHECK(scrollbar != NULL);
        XtVaGetValues(scroll, XmNhorizontalScrollBar, &horizontal, NULL);
        CHECK(horizontal != NULL);
        frame = (XmScrollFrameTrait)XmeTraitGet((XtPointer)XtClass(scroll), XmQTscrollFrame);
        CHECK(frame != NULL);
        /* Notify the active vertical navigator first. A callback destroying
         * the frame must not leave a later borrowed-list entry to visit. */
        frame->removeNavigator(scroll, horizontal);
        frame->addNavigator(scroll, horizontal, NavigDimensionX);
        XtVaSetValues(scrollbar, XmNinitialDelay, 10, XmNrepeatDelay, 10, NULL);
        drag = NULL;
        XtSetArg(arg, XmNdragProc, &drag);
        XmDropSiteRetrieve(scrollbar, &arg, 1);
        CHECK(drag != NULL);
        changed = 0;
        callback_action = scenario >= 5 ? (int)scenario - 4 : 0;
        XtAddCallback(scrollbar, XmNvalueChangedCallback, value_changed, NULL);
        memset(&data, 0, sizeof(data));
        data.reason = XmCR_DROP_SITE_ENTER_MESSAGE;
        data.x = 10;
        data.y = 100;
        drag(scrollbar, NULL, &data);
        sw = (XmScrolledWindowWidget)scroll;
        CHECK(sw->swindow.auto_drag_timer != 0);
        CHECK(sw->swindow.auto_drag_closure != NULL);
        switch (scenario) {
        case 0: XtDestroyWidget(scroll); break;
        case 1: XtDestroyWidget(scrollbar); break;
        case 2: XtVaSetValues(scroll, XmNautoDragModel, XmAUTO_DRAG_DISABLED, NULL); break;
        case 3: leave(); break;
        default: break;
        }
        drain(app);
        if (scenario != 0 && scenario != 7) {
            if (scenario == 4) {
                CHECK(changed > 0);
                XtVaGetValues(scrollbar, XmNvalue, &value, NULL);
                CHECK(value > 0);
                leave();
            } else if (scenario >= 5) CHECK(changed == 1);
            else CHECK(changed == 0);
            CHECK(sw->swindow.auto_drag_timer == 0);
            CHECK(sw->swindow.auto_drag_closure == NULL);
        } else if (scenario == 7) CHECK(changed == 1);
        XtDestroyWidget(shell);
        XtCloseDisplay(display);
        XtDestroyApplicationContext(app);
    }
    XCloseDisplay(anchor);
    printf("PASS autoscroll destruction, disable, leave and callback retirement: %u checks\n", checks);
    return 0;
}
