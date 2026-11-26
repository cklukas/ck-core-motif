/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Qualify multi-head RandR output-DPI shell resolution, cross-monitor window
 * movement (ConfigureNotify), and per-output DPI property updates. */
#define _POSIX_C_SOURCE 200809L
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <Xm/Display.h>
#include <Xm/Form.h>
#include <Xm/ScrollBar.h>
#include <Xm/SelectioB.h>
#include <Xm/ToggleB.h>

#include "XlibreDpiI.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int count;
    double old_scale;
    double new_scale;
    RROutput old_output;
    RROutput new_output;
} ShellCbLog;

static void
on_shell_scale(Widget shell, double old_scale, double new_scale,
               RROutput old_output, RROutput new_output, XtPointer closure)
{
    ShellCbLog *log = (ShellCbLog *)closure;
    (void)shell;
    if (!log)
        return;
    log->count++;
    log->old_scale = old_scale;
    log->new_scale = new_scale;
    log->old_output = old_output;
    log->new_output = new_output;
}

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        return 1; \
    } \
} while (0)

typedef struct {
    RROutput output;
    int x, y, w, h;
} ConnectedHead;

static int
discover_heads(Display *dpy, ConnectedHead *heads, int max_heads)
{
    XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, DefaultRootWindow(dpy));
    int n = 0;
    if (!res)
        return 0;
    for (int i = 0; i < res->noutput && n < max_heads; ++i) {
        XRROutputInfo *oi = XRRGetOutputInfo(dpy, res, res->outputs[i]);
        if (!oi)
            continue;
        if (oi->connection == RR_Connected && oi->crtc != None) {
            XRRCrtcInfo *ci = XRRGetCrtcInfo(dpy, res, oi->crtc);
            if (ci && ci->width > 0 && ci->height > 0) {
                heads[n].output = res->outputs[i];
                heads[n].x = ci->x;
                heads[n].y = ci->y;
                heads[n].w = (int)ci->width;
                heads[n].h = (int)ci->height;
                ++n;
            }
            if (ci)
                XRRFreeCrtcInfo(ci);
        }
        XRRFreeOutputInfo(oi);
    }
    XRRFreeScreenResources(res);
    /* Sort left-to-right by CRTC x coordinate */
    for (int a = 0; a < n; ++a) {
        for (int b = a + 1; b < n; ++b) {
            if (heads[b].x < heads[a].x ||
                (heads[b].x == heads[a].x && heads[b].y < heads[a].y)) {
                ConnectedHead tmp = heads[a];
                heads[a] = heads[b];
                heads[b] = tmp;
            }
        }
    }
    return n;
}

static void
noop_timer(XtPointer closure, XtIntervalId *id)
{
    (void)closure;
    (void)id;
}

static void
pump_until(XtAppContext app, Display *dpy, const int *counter, int target, int timeout_ms)
{
    struct timespec start, now, step = {0, 10000000L};
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (;;) {
        XFlush(dpy);
        (void)XtAppAddTimeOut(app, 25, noop_timer, NULL);
        XtAppProcessEvent(app, XtIMAll);
        (void)XtAppAddTimeOut(app, 25, noop_timer, NULL);
        XtAppProcessEvent(app, XtIMAll);
        while (XtAppPending(app))
            XtAppProcessEvent(app, XtIMAll);
        if (counter && *counter >= target)
            break;
        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed = (now.tv_sec - start.tv_sec) * 1000L +
                       (now.tv_nsec - start.tv_nsec) / 1000000L;
        if (elapsed >= timeout_ms)
            break;
        nanosleep(&step, NULL);
    }
}

static Dimension
dim_of(Widget w, const char *res)
{
    Dimension v = 0;
    XtVaGetValues(w, res, &v, NULL);
    return v;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget root_shell, shell_a, shell_b;
    Widget form_a, form_b, tog_a, tog_b, sb_a, sb_b, sel_a, sel_b;
    Display *dpy;
    ConnectedHead heads[8];
    int head_count;
    Atom opt_in_atom, dpi_atom;
    long opt_in_val = 1L, dpi_left = 96L, dpi_right = 192L, dpi_mid = 144L;
    ShellCbLog log_a = {0}, log_b = {0};
    double sc_a = 0.0, sc_b = 0.0;

    root_shell = XtVaAppInitialize(&app, "XlibreMultihead", NULL, 0,
                                   &argc, argv, NULL, NULL);
    dpy = XtDisplay(root_shell);

    head_count = discover_heads(dpy, heads, 8);
    if (head_count < 2) {
        XtDestroyWidget(root_shell);
        XtDestroyApplicationContext(app);
        puts("SKIP XLibre multihead fixture requires >= 2 connected RandR CRTCs");
        return 77;
    }

    /* Opt in to the RandR output-DPI contract if server vendor is Xwayland/Xorg */
    if (!_XmXlibreDpiServerSupported(dpy)) {
        opt_in_atom = XInternAtom(dpy, "_MOTIF_RANDR_OUTPUT_DPI", False);
        XChangeProperty(dpy, DefaultRootWindow(dpy), opt_in_atom, XA_CARDINAL, 32,
                        PropModeReplace, (const unsigned char *)&opt_in_val, 1);
        XSync(dpy, False);
    }
    CHECK(_XmXlibreDpiServerSupported(dpy));

    dpi_atom = XInternAtom(dpy, "DPI", False);
    XRRChangeOutputProperty(dpy, heads[0].output, dpi_atom, XA_INTEGER, 32,
                            PropModeReplace, (const unsigned char *)&dpi_left, 1);
    XRRChangeOutputProperty(dpy, heads[1].output, dpi_atom, XA_INTEGER, 32,
                            PropModeReplace, (const unsigned char *)&dpi_right, 1);
    XSync(dpy, False);

    /* Create Shell A on Head 0 (96 DPI -> 1.0x) and Shell B on Head 1 (192 DPI -> 2.0x) */
    XmString empty_label = XmStringCreateLocalized("");
    shell_a = XtVaAppCreateShell("shellA", "XlibreMultihead",
                                 overrideShellWidgetClass, dpy,
                                 XmNx, heads[0].x + 80, XmNy, heads[0].y + 80,
                                 XmNwidth, 320, XmNheight, 260, NULL);
    form_a = XtVaCreateManagedWidget("formA", xmFormWidgetClass, shell_a, NULL);
    tog_a = XtVaCreateManagedWidget("togA", xmToggleButtonWidgetClass, form_a,
                                    XmNlabelString, empty_label, NULL);
    sb_a = XtVaCreateManagedWidget("sbA", xmScrollBarWidgetClass, form_a, NULL);
    sel_a = XtVaCreateManagedWidget("selA", xmSelectionBoxWidgetClass, form_a, NULL);

    shell_b = XtVaAppCreateShell("shellB", "XlibreMultihead",
                                 overrideShellWidgetClass, dpy,
                                 XmNx, heads[1].x + 80, XmNy, heads[1].y + 80,
                                 XmNwidth, 320, XmNheight, 260, NULL);
    form_b = XtVaCreateManagedWidget("formB", xmFormWidgetClass, shell_b, NULL);
    tog_b = XtVaCreateManagedWidget("togB", xmToggleButtonWidgetClass, form_b,
                                    XmNlabelString, empty_label, NULL);
    XmStringFree(empty_label);
    sb_b = XtVaCreateManagedWidget("sbB", xmScrollBarWidgetClass, form_b, NULL);
    sel_b = XtVaCreateManagedWidget("selB", xmSelectionBoxWidgetClass, form_b, NULL);

    XtRealizeWidget(shell_a);
    XtRealizeWidget(shell_b);
    XtPopup(shell_a, XtGrabNone);
    XtPopup(shell_b, XtGrabNone);
    pump_until(app, dpy, NULL, 0, 60);

    CHECK(_XmXlibreDpiInstallShell(shell_a, on_shell_scale, &log_a));
    CHECK(_XmXlibreDpiInstallShell(shell_b, on_shell_scale, &log_b));

    /* Trigger initial scale sync on shell_b (constructed before observer install at 1x)
     * by verify-querying both shells' resolved RandR output scales: */
    CHECK(_XmXlibreDpiGetWidgetScale(shell_a, &sc_a));
    CHECK(_XmXlibreDpiGetWidgetScale(shell_b, &sc_b));
    CHECK(fabs(sc_a - 1.0) < 0.001);
    CHECK(fabs(sc_b - 2.0) < 0.001);
    CHECK(XmGetWidgetScale(tog_a, &sc_a) && fabs(sc_a - 1.0) < 0.001);
    CHECK(XmGetWidgetScale(tog_b, &sc_b) && fabs(sc_b - 2.0) < 0.001);

    Dimension tog_a_1x = dim_of(tog_a, XmNindicatorSize);
    Dimension sb_a_1x = dim_of(sb_a, XmNwidth);
    Dimension sel_a_1x = dim_of(sel_a, XmNmarginWidth);

    /* Move Shell A onto Head 1 (192 DPI -> 2.0x) via XtVaSetValues -> ConfigureNotify */
    log_a.count = 0;
    log_b.count = 0;
    XtVaSetValues(shell_a, XmNx, heads[1].x + 120, XmNy, heads[1].y + 100, NULL);
    pump_until(app, dpy, &log_a.count, 1, 2000);

    CHECK(log_a.count == 1);
    CHECK(log_b.count == 0);
    CHECK(log_a.old_output == heads[0].output && log_a.new_output == heads[1].output);
    CHECK(fabs(log_a.old_scale - 1.0) < 0.001 && fabs(log_a.new_scale - 2.0) < 0.001);
    CHECK(dim_of(tog_a, XmNindicatorSize) > tog_a_1x);
    CHECK(dim_of(sb_a, XmNwidth) > sb_a_1x);
    CHECK(dim_of(sel_a, XmNmarginWidth) == (Dimension)(2 * sel_a_1x));

    /* Live XRRChangeOutputProperty on Head 1: 192 -> 144 (1.5x) updates BOTH shells on Head 1 */
    XRRChangeOutputProperty(dpy, heads[1].output, dpi_atom, XA_INTEGER, 32,
                            PropModeReplace, (const unsigned char *)&dpi_mid, 1);
    pump_until(app, dpy, &log_b.count, 1, 2000);
    pump_until(app, dpy, &log_a.count, 2, 2000);
    CHECK(log_a.count == 2 && log_b.count == 1);
    CHECK(fabs(log_a.new_scale - 1.5) < 0.001 && fabs(log_b.new_scale - 1.5) < 0.001);
    CHECK(dim_of(tog_b, XmNindicatorSize) > tog_a_1x);
    CHECK(dim_of(sb_b, XmNwidth) > sb_a_1x);
    CHECK(dim_of(sel_b, XmNmarginWidth) > sel_a_1x);

    /* Move Shell A back to Head 0 (96 DPI -> 1.0x): Shell A restores 1x while Shell B stays 1.5x */
    XtVaSetValues(shell_a, XmNx, heads[0].x + 80, XmNy, heads[0].y + 80, NULL);
    pump_until(app, dpy, &log_a.count, 3, 2000);
    CHECK(log_a.count == 3 && log_b.count == 1);
    CHECK(fabs(log_a.new_scale - 1.0) < 0.001);
    CHECK(dim_of(tog_a, XmNindicatorSize) == tog_a_1x);
    CHECK(dim_of(sb_a, XmNwidth) == sb_a_1x);
    CHECK(dim_of(sel_a, XmNmarginWidth) == sel_a_1x);
    CHECK(_XmXlibreDpiGetWidgetScale(shell_b, &sc_b) && fabs(sc_b - 1.5) < 0.001);

    _XmXlibreDpiUninstallShell(shell_b);
    _XmXlibreDpiUninstallShell(shell_a);
    XtDestroyWidget(shell_b);
    XtDestroyWidget(shell_a);
    XtDestroyWidget(root_shell);
    XtDestroyApplicationContext(app);
    puts("PASS XLibre multi-head RandR DPI per-CRTC resolution and cross-monitor reflow");
    return 0;
}
