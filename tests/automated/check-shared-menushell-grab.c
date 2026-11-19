#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeBG.h>
#include <Xm/PushBG.h>

static int g_activated = 0;

static void on_activate(Widget w, XtPointer client_data, XtPointer call_data)
{
    (void)w;
    (void)client_data;
    (void)call_data;
    g_activated++;
}

static void pump_events(XtAppContext app, Display *dpy, int ms)
{
    int loops = ms / 10;
    if (loops < 1) loops = 1;
    struct timespec ts = { 0, 10000000L };
    for (int i = 0; i < loops; ++i) {
        XSync(dpy, False);
        while (XtAppPending(app)) {
            XEvent ev;
            XtAppNextEvent(app, &ev);
            XtDispatchEvent(&ev);
        }
        nanosleep(&ts, NULL);
    }
    XSync(dpy, False);
    while (XtAppPending(app)) {
        XEvent ev;
        XtAppNextEvent(app, &ev);
        XtDispatchEvent(&ev);
    }
}

static void widget_center_root(Widget w, int *rx, int *ry)
{
    Position x = 0, y = 0;
    Dimension width = 0, height = 0;
    Position root_x = 0, root_y = 0;
    Widget ref = XtIsWidget(w) ? w : XtParent(w);
    XtVaGetValues(w, XmNx, &x, XmNy, &y, XmNwidth, &width, XmNheight, &height, NULL);
    if (XtIsWidget(w)) {
        XtTranslateCoords(w, (Position)(width / 2), (Position)(height / 2), &root_x, &root_y);
    } else {
        XtTranslateCoords(ref, (Position)(x + (Position)(width / 2)),
                          (Position)(y + (Position)(height / 2)), &root_x, &root_y);
    }
    *rx = (int)root_x;
    *ry = (int)root_y;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell1, shell2, mw1, mw2, mbar1, mbar2;
    Widget shared_pane, item1, item2, cb1, cb2;
    Display *dpy;
    int ev_base = 0, err_base = 0, maj = 0, min = 0;
    int cx = 0, cy = 0, ix = 0, iy = 0;

    XtSetLanguageProc(NULL, NULL, NULL);
    shell1 = XtVaAppInitialize(&app, "CheckSharedMenuGrab", NULL, 0, &argc, argv, NULL,
                               XmNx, 40, XmNy, 40, XmNwidth, 260, XmNheight, 180, NULL);
    dpy = XtDisplay(shell1);
    if (!XTestQueryExtension(dpy, &ev_base, &err_base, &maj, &min)) {
        fprintf(stderr, "FAIL: XTest extension unavailable\n");
        return 1;
    }

    mw1 = XmCreateMainWindow(shell1, "mw1", NULL, 0);
    XtManageChild(mw1);
    mbar1 = XmCreateMenuBar(mw1, "mbar1", NULL, 0);
    XtManageChild(mbar1);

    /* Shared pulldown menu parented under mbar1 (Window #1), as in CDE dtfile */
    shared_pane = XmCreatePulldownMenu(mbar1, "sharedFileMenu", NULL, 0);
    item1 = XtVaCreateManagedWidget("newFolder", xmPushButtonGadgetClass, shared_pane, NULL);
    item2 = XtVaCreateManagedWidget("connectServer", xmPushButtonGadgetClass, shared_pane, NULL);
    XtAddCallback(item2, XmNactivateCallback, on_activate, NULL);

    cb1 = XtVaCreateManagedWidget("File", xmCascadeButtonGadgetClass, mbar1,
                                  XmNsubMenuId, shared_pane, NULL);
    (void)cb1;
    XtRealizeWidget(shell1);

    /* Second independent TopLevelShell sharing the exact same pulldown pane */
    shell2 = XtVaAppCreateShell("win2", "CheckSharedMenuGrab",
                                topLevelShellWidgetClass, dpy,
                                XmNx, 360, XmNy, 80, XmNwidth, 280, XmNheight, 180, NULL);
    mw2 = XmCreateMainWindow(shell2, "mw2", NULL, 0);
    XtManageChild(mw2);
    mbar2 = XmCreateMenuBar(mw2, "mbar2", NULL, 0);
    XtManageChild(mbar2);
    cb2 = XtVaCreateManagedWidget("File", xmCascadeButtonGadgetClass, mbar2,
                                  XmNsubMenuId, shared_pane, NULL);
    XtRealizeWidget(shell2);

    pump_events(app, dpy, 150);

    /* 1. Press-drag-release on shell2's File menu -> connectServer */
    widget_center_root(cb2, &cx, &cy);
    XTestFakeMotionEvent(dpy, -1, cx, cy, CurrentTime);
    pump_events(app, dpy, 40);
    XTestFakeButtonEvent(dpy, Button1, True, CurrentTime);
    pump_events(app, dpy, 120);

    widget_center_root(item2, &ix, &iy);
    XTestFakeMotionEvent(dpy, -1, ix, iy, CurrentTime);
    pump_events(app, dpy, 80);
    XTestFakeButtonEvent(dpy, Button1, False, CurrentTime);
    pump_events(app, dpy, 120);

    if (g_activated != 1) {
        fprintf(stderr, "FAIL: press-drag-release on secondary shell shared menu did not activate item (count=%d)\n",
                g_activated);
        return 1;
    }

    /* 2. Two-click (click File, release, move to item2, click item2) on shell2 */
    widget_center_root(cb2, &cx, &cy);
    XTestFakeMotionEvent(dpy, -1, cx, cy, CurrentTime);
    pump_events(app, dpy, 40);
    XTestFakeButtonEvent(dpy, Button1, True, CurrentTime);
    pump_events(app, dpy, 40);
    XTestFakeButtonEvent(dpy, Button1, False, CurrentTime);
    pump_events(app, dpy, 120);

    widget_center_root(item2, &ix, &iy);
    XTestFakeMotionEvent(dpy, -1, ix, iy, CurrentTime);
    pump_events(app, dpy, 60);
    XTestFakeButtonEvent(dpy, Button1, True, CurrentTime);
    pump_events(app, dpy, 40);
    XTestFakeButtonEvent(dpy, Button1, False, CurrentTime);
    pump_events(app, dpy, 120);

    if (g_activated != 2) {
        fprintf(stderr, "FAIL: click-to-select on secondary shell shared menu did not activate item (count=%d)\n",
                g_activated);
        return 1;
    }

    (void)item1;
    printf("PASS shared XmMenuShell press-drag-release and click activation across secondary shells\n");
    return 0;
}
