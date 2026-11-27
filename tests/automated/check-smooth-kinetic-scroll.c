/*
 * Regression qualification for XInput2 smooth-scroll valuator dispatch and
 * sub-line / pixel-precise scrolling across XmScrolledWindow, XmDataGrid,
 * XmList, and XmText.
 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <Xm/XmAll.h>
#include <Xm/DataGrid.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); exit(1); } } while (0)

extern Boolean _XmDispatchSmoothScrollValuator(Widget shell,
                                               double event_x,
                                               double event_y,
                                               unsigned int state,
                                               double vert_v120,
                                               double horiz_v120);

/* Mock XIScrollClassInfo device descriptor for Xvfb GenericEvent verification */
typedef struct {
    int type;
    int sourceid;
} TestXiAnyClass;

typedef struct {
    int type;
    int sourceid;
    int number;
    int scroll_type;
    double increment;
    int flags;
} TestXiScrollClass;

typedef struct {
    int deviceid;
    char *name;
    int use;
    int attachment;
    Bool enabled;
    int num_classes;
    TestXiAnyClass **classes;
} TestXiDeviceInfo;

typedef struct {
    int deviceid;
    int mask_len;
    unsigned char *mask;
} TestXiEventMask;

typedef struct {
    int base, latched, locked, effective;
} TestXiMods;

typedef struct {
    int mask_len;
    unsigned char *mask;
} TestXiButtons;

typedef struct {
    int mask_len;
    unsigned char *mask;
    double *values;
} TestXiValuators;

typedef struct {
    int type;
    unsigned long serial;
    Bool send_event;
    Display *display;
    int extension;
    int evtype;
    Time time;
    int deviceid;
    int sourceid;
    int detail;
    Window root;
    Window event;
    Window child;
    double root_x;
    double root_y;
    double event_x;
    double event_y;
    int flags;
    TestXiButtons buttons;
    TestXiValuators valuators;
    TestXiMods mods;
    TestXiMods group;
} TestXiDeviceEvent;

static TestXiScrollClass g_mock_vscroll = { 3 /* XIScrollClass */, 2, 2, 1 /* Vertical */, 15.0, 0 };
static TestXiScrollClass g_mock_hscroll = { 3 /* XIScrollClass */, 2, 3, 2 /* Horizontal */, 15.0, 0 };
static TestXiAnyClass *g_mock_classes[2] = {
    (TestXiAnyClass *)&g_mock_vscroll,
    (TestXiAnyClass *)&g_mock_hscroll,
};
static int g_xi_select_calls = 0;

Status XIQueryVersion(Display *dpy, int *maj, int *min) {
    (void)dpy;
    *maj = 2;
    *min = 2;
    return Success;
}

TestXiDeviceInfo *XIQueryDevice(Display *dpy, int deviceid, int *ndevices_return) {
    (void)dpy;
    (void)deviceid;
    TestXiDeviceInfo *dev = (TestXiDeviceInfo *)calloc(1, sizeof(*dev));
    if (!dev) return NULL;
    dev->deviceid = 2;
    dev->name = (char *)"Virtual Smooth Touchpad";
    dev->enabled = True;
    dev->num_classes = 2;
    dev->classes = g_mock_classes;
    *ndevices_return = 1;
    return dev;
}

void XIFreeDeviceInfo(TestXiDeviceInfo *info) {
    free(info);
}

int XISelectEvents(Display *dpy, Window win, TestXiEventMask *masks, int num_masks) {
    (void)dpy;
    (void)win;
    (void)masks;
    (void)num_masks;
    g_xi_select_calls++;
    return 0;
}

static void settle(XtAppContext app, Display *dpy) {
    for (int pass = 0; pass < 6; ++pass) {
        XSync(dpy, False);
        while (XtAppPending(app))
            XtAppProcessEvent(app, XtIMAll);
    }
}

static uint64_t hash_region(Display *dpy, Window win, int x, int y, unsigned int w, unsigned int h) {
    XImage *img = XGetImage(dpy, win, x, y, w, h, AllPlanes, ZPixmap);
    CHECK(img != NULL);
    uint64_t fnv = UINT64_C(14695981039346656037);
    for (unsigned int r = 0; r < h; ++r) {
        for (unsigned int c = 0; c < w; ++c) {
            unsigned long px = XGetPixel(img, (int)c, (int)r) & 0x00ffffffUL;
            fnv ^= (uint64_t)px;
            fnv *= UINT64_C(1099511628211);
        }
    }
    XDestroyImage(img);
    return fnv;
}

static void send_xi2_scroll_motion(Display *dpy, int xi_op, Window shell_win,
                                   double ex, double ey, double v_val, double h_val) {
    unsigned char vmask[1] = { (unsigned char)((1u << 2) | (1u << 3)) };
    double vals[2] = { v_val, h_val };
    TestXiDeviceEvent de;
    memset(&de, 0, sizeof(de));
    de.type = GenericEvent;
    de.display = dpy;
    de.extension = xi_op;
    de.evtype = 6; /* XI_Motion */
    de.deviceid = 2;
    de.sourceid = 2;
    de.event = shell_win;
    de.event_x = ex;
    de.event_y = ey;
    de.valuators.mask_len = 1;
    de.valuators.mask = vmask;
    de.valuators.values = vals;

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.xcookie.type = GenericEvent;
    xev.xcookie.display = dpy;
    xev.xcookie.extension = xi_op;
    xev.xcookie.evtype = 6;
    xev.xcookie.data = &de;
    (void)XtDispatchEvent(&xev);
}

int main(int argc, char **argv) {
    XtAppContext app;
    XtToolkitInitialize();
    app = XtCreateApplicationContext();
    Display *dpy = XtOpenDisplay(app, NULL, "smoothScroll", "SmoothScroll", NULL, 0, &argc, argv);
    CHECK(dpy != NULL);

    int xi_op = 0, xi_ev = 0, xi_err = 0;
    CHECK(XQueryExtension(dpy, "XInputExtension", &xi_op, &xi_ev, &xi_err));

    Widget shell = XtVaAppCreateShell("smoothScroll", "SmoothScroll",
                                      applicationShellWidgetClass, dpy,
                                      XmNwidth, 720, XmNheight, 560, NULL);
    Widget form = XtVaCreateManagedWidget("form", xmFormWidgetClass, shell, NULL);

    /* 1. XmScrolledWindow (XmAUTOMATIC) */
    Widget sw = XtVaCreateManagedWidget(
        "sw", xmScrolledWindowWidgetClass, form,
        XmNscrollingPolicy, XmAUTOMATIC,
        XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 8,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 8,
        XmNwidth, 320, XmNheight, 180, NULL);
    Widget canvas = XtVaCreateManagedWidget(
        "canvas", xmDrawingAreaWidgetClass, sw,
        XmNwidth, 900, XmNheight, 900, NULL);
    (void)canvas;
    Widget sw_vbar = NULL, sw_hbar = NULL;
    XtVaGetValues(sw, XmNverticalScrollBar, &sw_vbar, XmNhorizontalScrollBar, &sw_hbar, NULL);
    CHECK(sw_vbar && sw_hbar);

    /* 2. XmDataGrid with 30 rows and wide columns */
    Widget grid = XmCreateDataGrid(form, "grid", NULL, 0);
    XtVaSetValues(grid,
                  XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 8,
                  XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 350,
                  XmNwidth, 320, XmNheight, 180,
                  XmNdataGridHeaderHeight, 24,
                  XmNdataGridRowHeight, 22,
                  NULL);
    XmDataGridColumn cols[3] = {
        { (XtPointer)1, "Column Alpha", False, 180 },
        { (XtPointer)2, "Column Beta",  False, 180 },
        { (XtPointer)3, "Column Gamma", False, 180 },
    };
    XmDataGridRow rows[30];
    char row_cells[30][3][32];
    const char *row_ptrs[30][3];
    for (int r = 0; r < 30; ++r) {
        for (int c = 0; c < 3; ++c) {
            snprintf(row_cells[r][c], sizeof(row_cells[r][c]), "Cell R%02d-C%d", r, c);
            row_ptrs[r][c] = row_cells[r][c];
        }
        rows[r].id = (XtPointer)(uintptr_t)(r + 1);
        rows[r].values = row_ptrs[r];
    }
    CHECK(XmDataGridSetModel(grid, cols, 3, rows, 30));
    XtManageChild(grid);

    /* 3. Scrolled XmList & Scrolled XmText */
    Arg list_args[4];
    Cardinal ln = 0;
    XtSetArg(list_args[ln], XmNscrollBarDisplayPolicy, XmSTATIC); ln++;
    XtSetArg(list_args[ln], XmNlistSizePolicy, XmCONSTANT); ln++;
    Widget list = XmCreateScrolledList(form, "list", list_args, ln);
    XtVaSetValues(XtParent(list),
                  XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 210,
                  XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 8,
                  XmNwidth, 320, XmNheight, 180, NULL);
    for (int i = 0; i < 40; ++i) {
        char buf[96];
        snprintf(buf, sizeof(buf), "List Entry %02d — wide horizontal content for smooth pixel scroll check", i);
        XmString xs = XmStringCreateLocalized(buf);
        XmListAddItem(list, xs, 0);
        XmStringFree(xs);
    }
    XtManageChild(list);

    Widget text = XmCreateScrolledText(form, "text", NULL, 0);
    XtVaSetValues(XtParent(text),
                  XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 210,
                  XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 350,
                  XmNwidth, 320, XmNheight, 180, NULL);
    XtVaSetValues(text, XmNeditMode, XmMULTI_LINE_EDIT, XmNscrollHorizontal, True, XmNwordWrap, False, NULL);
    char big_txt[4096];
    size_t off = 0;
    for (int i = 0; i < 40; ++i) {
        off += (size_t)snprintf(big_txt + off, sizeof(big_txt) - off,
                                "Line %02d: horizontal smooth text content across columns 0123456789abcdef\n", i);
    }
    XmTextSetString(text, big_txt);
    XtManageChild(text);

    XtRealizeWidget(shell);
    XMapRaised(dpy, XtWindow(shell));
    {
        XWindowAttributes attr;
        memset(&attr, 0, sizeof(attr));
        for (int i = 0; i < 100; ++i) {
            settle(app, dpy);
            if (XGetWindowAttributes(dpy, XtWindow(grid), &attr) &&
                attr.map_state == IsViewable)
                break;
            struct timespec ts = { 0, 10 * 1000 * 1000 };
            nanosleep(&ts, NULL);
        }
        struct timespec ts = { 0, 20 * 1000 * 1000 };
        nanosleep(&ts, NULL);
        settle(app, dpy);
    }
    CHECK(g_xi_select_calls >= 2);

    /* Test A: Real XInput2 GenericEvent XI_Motion valuator deltas + XIPointerEmulated suppression on sw */
    send_xi2_scroll_motion(dpy, xi_op, XtWindow(shell), 60.0, 60.0, 100.0, 200.0); /* baseline sample */
    int v0 = 0, v1 = 0, h1 = 0;
    XtVaGetValues(sw_vbar, XmNvalue, &v0, NULL);
    CHECK(v0 == 0);
    /* Advance vertical valuator by +3.75 (3.75 / 15.0 * 120 = 30 v120 = 1/4 notch) and horizontal by +7.5 */
    send_xi2_scroll_motion(dpy, xi_op, XtWindow(shell), 60.0, 60.0, 103.75, 207.5);
    settle(app, dpy);
    XtVaGetValues(sw_vbar, XmNvalue, &v1, NULL);
    XtVaGetValues(sw_hbar, XmNvalue, &h1, NULL);
    CHECK(v1 > 0 && v1 < 40);
    CHECK(h1 > 0 && h1 < 60);

    /* Emit XI_ButtonPress with XIPointerEmulated (1<<16) for Button5, then fake legacy Button5: must be suppressed */
    {
        TestXiDeviceEvent bde;
        memset(&bde, 0, sizeof(bde));
        bde.type = GenericEvent;
        bde.display = dpy;
        bde.extension = xi_op;
        bde.evtype = 4; /* XI_ButtonPress */
        bde.deviceid = 2;
        bde.detail = 5;
        bde.event = XtWindow(shell);
        bde.flags = (1 << 16); /* XIPointerEmulated */
        XEvent xev;
        memset(&xev, 0, sizeof(xev));
        xev.xcookie.type = GenericEvent;
        xev.xcookie.display = dpy;
        xev.xcookie.extension = xi_op;
        xev.xcookie.evtype = 4;
        xev.xcookie.data = &bde;
        (void)XtDispatchEvent(&xev);

        Position rx = 0, ry = 0;
        XtTranslateCoords(sw, 40, 40, &rx, &ry);
        XTestFakeMotionEvent(dpy, DefaultScreen(dpy), rx, ry, CurrentTime);
        XTestFakeButtonEvent(dpy, 5, True, CurrentTime);
        XTestFakeButtonEvent(dpy, 5, False, CurrentTime);
        settle(app, dpy);
        int v_after_emu = 0;
        XtVaGetValues(sw_vbar, XmNvalue, &v_after_emu, NULL);
        CHECK(v_after_emu == v1);
    }

    /* Test B: XmDataGrid sub-row vertical pixel scroll & sticky header preservation + horizontal pixel scroll */
    uint64_t hdr_h0 = hash_region(dpy, XtWindow(grid), 0, 0, 300, 22);
    uint64_t body_h0 = hash_region(dpy, XtWindow(grid), 0, 26, 300, 140);
    CHECK(XmDataGridGetTopRow(grid) == 0);

    /* 20 v120 = 1/6 notch = 11 px (< 22px row_height): top_row stays 0, body pixels shift, header unchanged! */
    CHECK(_XmDispatchSmoothScrollValuator(shell, 420.0, 80.0, 0, 20.0, 0.0));
    settle(app, dpy);
    CHECK(XmDataGridGetTopRow(grid) == 0);
    uint64_t hdr_h1 = hash_region(dpy, XtWindow(grid), 0, 0, 300, 22);
    uint64_t body_h1 = hash_region(dpy, XtWindow(grid), 0, 26, 300, 140);
    CHECK(hdr_h1 == hdr_h0);
    CHECK(body_h1 != body_h0);

    /* Another 30 v120 crosses 22px row boundary -> top_row advances to 1 */
    CHECK(_XmDispatchSmoothScrollValuator(shell, 420.0, 80.0, 0, 30.0, 0.0));
    settle(app, dpy);
    CHECK(XmDataGridGetTopRow(grid) >= 1);

    /* Horizontal pixel scroll on DataGrid shifts both header and body horizontally */
    CHECK(_XmDispatchSmoothScrollValuator(shell, 420.0, 80.0, 0, 0.0, 45.0));
    settle(app, dpy);
    uint64_t hdr_h2 = hash_region(dpy, XtWindow(grid), 0, 0, 300, 22);
    CHECK(hdr_h2 != hdr_h1);

    /* Test C: XmList & XmText horizontal + vertical smooth scroll */
    Widget list_hbar = NULL;
    XtVaGetValues(list, XmNhorizontalScrollBar, &list_hbar, NULL);
    CHECK(list_hbar != NULL);
    CHECK(_XmDispatchSmoothScrollValuator(shell, 80.0, 280.0, 0, 60.0, 60.0));
    settle(app, dpy);
    int list_top = 1, list_hval = 0;
    XtVaGetValues(list, XmNtopItemPosition, &list_top, NULL);
    XtVaGetValues(list_hbar, XmNvalue, &list_hval, NULL);
    CHECK(list_top >= 2);
    CHECK(list_hval > 0);

    Widget text_hbar = NULL;
    XtVaGetValues(XtParent(text), XmNhorizontalScrollBar, &text_hbar, NULL);
    CHECK(text_hbar != NULL);
    CHECK(_XmDispatchSmoothScrollValuator(shell, 420.0, 280.0, 0, 80.0, 60.0));
    settle(app, dpy);
    int text_hval = 0;
    XtVaGetValues(text_hbar, XmNvalue, &text_hval, NULL);
    CHECK(XmTextGetTopCharacter(text) > 0);
    CHECK(text_hval > 0);

    XtDestroyWidget(shell);
    settle(app, dpy);
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    puts("PASS smooth/kinetic XInput2 + sub-line pixel scrolling (ScrolledWindow, DataGrid, List, Text)");
    return 0;
}
