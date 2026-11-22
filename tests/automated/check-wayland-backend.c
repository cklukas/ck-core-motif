/*
 * Automated qualification client for the XWayland-free libXm Wayland backend.
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrender.h>

#include <Xm/Appearance.h>
#include <Xm/CascadeB.h>
#include <Xm/Display.h>
#include <Xm/DrawingA.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Wayland.h>
#include <Xm/Xm.h>

#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void
fail_msg(const char *msg)
{
    fprintf(stderr, "FAIL: %s\n", msg);
    exit(1);
}

static uint64_t
mono_ms(void)
{
    struct timespec ts = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000L);
}

static void
pump_xt_ms(XtAppContext app, Display *dpy, uint64_t duration_ms)
{
    uint64_t deadline = mono_ms() + duration_ms;
    for (;;) {
        while (XtAppPending(app) & (XtIMXEvent | XtIMTimer | XtIMAlternateInput)) {
            XtAppProcessEvent(app, XtIMXEvent | XtIMTimer | XtIMAlternateInput);
        }
        XFlush(dpy);
        if (mono_ms() >= deadline)
            break;
        struct timespec req = {0, 10 * 1000 * 1000L};
        nanosleep(&req, NULL);
    }
}

static bool
verify_maps_has_libxm(void)
{
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp) return false;
    char line[1024];
    bool found = false;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "libXm.so")) {
            found = true;
            break;
        }
    }
    fclose(fp);
    return found;
}

static size_t
count_distinct_colors_in_window(Display *dpy, Window win, int w, int h, uint32_t *out_avg_luma)
{
    if (w < 4 || h < 4) return 0;
    XImage *img = XGetImage(dpy, win, 0, 0, (unsigned)w, (unsigned)h, AllPlanes, ZPixmap);
    if (!img) return 0;

    uint32_t seen[128];
    size_t nseen = 0;
    uint64_t luma_sum = 0;
    uint64_t px_cnt = 0;

    for (int y = 0; y < h; y += 2) {
        for (int x = 0; x < w; x += 2) {
            unsigned long px = XGetPixel(img, x, y) & 0x00ffffffUL;
            uint32_t r = (uint32_t)((px >> 16) & 0xffu);
            uint32_t g = (uint32_t)((px >> 8) & 0xffu);
            uint32_t b = (uint32_t)(px & 0xffu);
            luma_sum += (r * 299u + g * 587u + b * 114u) / 1000u;
            px_cnt++;

            bool dup = false;
            for (size_t i = 0; i < nseen; ++i) {
                if (seen[i] == (uint32_t)px) {
                    dup = true;
                    break;
                }
            }
            if (!dup && nseen < 128)
                seen[nseen++] = (uint32_t)px;
        }
    }
    XDestroyImage(img);
    if (out_avg_luma)
        *out_avg_luma = px_cnt ? (uint32_t)(luma_sum / px_cnt) : 0;
    return nseen;
}

/* ---------- Selection Owner / Requestor Helper State ---------- */

static const char *g_export_utf8 = "Motif Wayland Clipboard 日本語 42";
static const char *g_export_uris = "file:///tmp/wayland-test-alpha.txt\r\n";
static const uint8_t g_export_png[16] = {
    0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a,
    0x00, 0x00, 0x00, 0x0d, 'I', 'H', 'D', 'R'
};

static const char *g_peer_utf8 = "External Wayland Offer العربية 99";
static const char *g_peer_uris = "file:///tmp/from-peer-beta.txt\r\n";
static const uint8_t g_peer_png[16] = {
    0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a,
    0xde, 0xad, 0xbe, 0xef, 'I', 'E', 'N', 'D'
};

static void
answer_selection_request(Display *dpy, const XSelectionRequestEvent *sr, bool use_peer_payload)
{
    Atom a_targets = XInternAtom(dpy, "TARGETS", False);
    Atom a_utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    Atom a_text = XInternAtom(dpy, "TEXT", False);
    Atom a_uri = XInternAtom(dpy, "text/uri-list", False);
    Atom a_png = XInternAtom(dpy, "image/png", False);

    Atom out_prop = sr->property ? sr->property : sr->target;
    bool ok = true;

    const char *txt = use_peer_payload ? g_peer_utf8 : g_export_utf8;
    const char *uri = use_peer_payload ? g_peer_uris : g_export_uris;
    const uint8_t *png = use_peer_payload ? g_peer_png : g_export_png;

    if (sr->target == a_targets) {
        Atom list[5] = { a_targets, a_utf8, XA_STRING, a_uri, a_png };
        XChangeProperty(dpy, sr->requestor, out_prop, XA_ATOM, 32, PropModeReplace,
                        (const unsigned char *)list, 5);
    } else if (sr->target == a_utf8 || sr->target == XA_STRING || sr->target == a_text) {
        XChangeProperty(dpy, sr->requestor, out_prop, sr->target, 8, PropModeReplace,
                        (const unsigned char *)txt, (int)strlen(txt));
    } else if (sr->target == a_uri) {
        XChangeProperty(dpy, sr->requestor, out_prop, a_uri, 8, PropModeReplace,
                        (const unsigned char *)uri, (int)strlen(uri));
    } else if (sr->target == a_png) {
        XChangeProperty(dpy, sr->requestor, out_prop, a_png, 8, PropModeReplace,
                        png, (int)sizeof(g_export_png));
    } else {
        ok = false;
    }

    XEvent notify;
    memset(&notify, 0, sizeof(notify));
    notify.xselection.type = SelectionNotify;
    notify.xselection.requestor = sr->requestor;
    notify.xselection.selection = sr->selection;
    notify.xselection.target = sr->target;
    notify.xselection.property = ok ? out_prop : None;
    notify.xselection.time = sr->time;
    XSendEvent(dpy, sr->requestor, False, 0, &notify);
    XFlush(dpy);
}

static void
sel_owner_event_handler(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
    (void)w;
    (void)cont;
    bool peer_mode = (closure != NULL);
    if (event->type == SelectionRequest) {
        answer_selection_request(event->xany.display, &event->xselectionrequest, peer_mode);
    }
}

static uint8_t *
request_selection_bytes(XtAppContext app, Display *dpy, Window req_win,
                        Atom selection, Atom target, size_t *out_len)
{
    (void)app;
    *out_len = 0;
    Atom prop = XInternAtom(dpy, "_TEST_SEL_DEST", False);
    XDeleteProperty(dpy, req_win, prop);
    XConvertSelection(dpy, selection, target, prop, req_win, CurrentTime);
    XFlush(dpy);

    bool got_notify = false;
    Atom got_prop = None;
    uint64_t deadline = mono_ms() + 3000;
    while (mono_ms() < deadline) {
        while (XPending(dpy) > 0) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == SelectionNotify && ev.xselection.selection == selection) {
                got_notify = true;
                got_prop = ev.xselection.property;
                XtDispatchEvent(&ev);
                break;
            }
            XtDispatchEvent(&ev);
        }
        if (got_notify)
            break;
        struct timespec req = {0, 10 * 1000 * 1000L};
        nanosleep(&req, NULL);
    }
    if (!got_notify || got_prop == None)
        return NULL;

    Atom actual_type = None;
    int actual_fmt = 0;
    unsigned long nitems = 0, bytes_after = 0;
    unsigned char *prop_data = NULL;
    if (XGetWindowProperty(dpy, req_win, got_prop, 0, 65536, True, AnyPropertyType,
                           &actual_type, &actual_fmt, &nitems, &bytes_after, &prop_data) != Success ||
        !prop_data) {
        return NULL;
    }
    size_t blen = (size_t)nitems * (actual_fmt == 32 ? sizeof(long) : (size_t)(actual_fmt / 8));
    if (actual_fmt == 8)
        blen = (size_t)nitems;
    uint8_t *copy = (uint8_t *)malloc(blen + 1);
    if (copy) {
        memcpy(copy, prop_data, blen);
        copy[blen] = '\0';
        *out_len = blen;
    }
    XFree(prop_data);
    return copy;
}

static void
write_stage_file(const char *dir, const char *name)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", dir, name);
    FILE *fp = fopen(path, "w");
    if (fp) {
        fprintf(fp, "1\n");
        fclose(fp);
    }
}

static void
wait_for_stage_file(XtAppContext app, Display *dpy, const char *dir, const char *name, uint64_t timeout_ms)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", dir, name);
    uint64_t deadline = mono_ms() + timeout_ms;
    while (mono_ms() < deadline) {
        if (access(path, F_OK) == 0)
            return;
        pump_xt_ms(app, dpy, 40);
    }
    fprintf(stderr, "FAIL: timed out waiting for %s\n", path);
    exit(1);
}

/* ---------- Mode 1: Child Socket Isolation Check ---------- */

static int
run_child_check(const char *parent_dpy)
{
    const char *active = getenv("CK_MOTIF_WAYLAND_ACTIVE");
    const char *cur_dpy = getenv("DISPLAY");
    if (!active || strcmp(active, "1") != 0)
        fail_msg("child CK_MOTIF_WAYLAND_ACTIVE != 1");
    if (!cur_dpy || !strstr(cur_dpy, "/ck-wlx11-"))
        fail_msg("child DISPLAY is not a ck-wlx11 socket");
    if (strcmp(cur_dpy, parent_dpy) == 0)
        fail_msg("child reused parent's ck-wlx11 socket instead of isolating");

    XtToolkitInitialize();
    XtAppContext app = XtCreateApplicationContext();
    int argc = 1;
    char *argv[] = { (char *)"child-check", NULL };
    Display *dpy = XtOpenDisplay(app, NULL, "child", "Child", NULL, 0, &argc, argv);
    if (!dpy)
        fail_msg("child XtOpenDisplay failed");
    if (DisplayWidth(dpy, 0) < 320 || DisplayHeight(dpy, 0) < 200)
        fail_msg("child screen dimensions invalid");
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    return 0;
}

/* ---------- Mode 2: External Wayland Clipboard Peer Process ---------- */

static int
run_clipboard_peer(const char *sync_dir)
{
    XtToolkitInitialize();
    XtAppContext app = XtCreateApplicationContext();
    int argc = 1;
    char *argv[] = { (char *)"wl-peer", NULL };
    Display *dpy = XtOpenDisplay(app, NULL, "wlPeer", "WlPeer", NULL, 0, &argc, argv);
    if (!dpy)
        fail_msg("peer XtOpenDisplay failed");

    Widget shell = XtVaAppCreateShell("wlPeer", "WlPeer", applicationShellWidgetClass, dpy,
                                      XmNwidth, 180, XmNheight, 100,
                                      XmNtitle, "Peer", NULL);
    Widget lbl = XmCreateLabel(shell, (char *)"peerLabel", NULL, 0);
    XtManageChild(lbl);
    XtAddEventHandler(shell, 0, True, sel_owner_event_handler, (XtPointer)1);
    XtRealizeWidget(shell);
    pump_xt_ms(app, dpy, 300);

    Atom a_clip = XInternAtom(dpy, "CLIPBOARD", False);
    Atom a_utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    Atom a_uri = XInternAtom(dpy, "text/uri-list", False);
    Atom a_png = XInternAtom(dpy, "image/png", False);

    /* 1. Read CLIPBOARD & PRIMARY exported by the primary Motif process */
    size_t len = 0;
    uint8_t *got_utf8 = request_selection_bytes(app, dpy, XtWindow(shell), a_clip, a_utf8, &len);
    if (!got_utf8 || strcmp((const char *)got_utf8, g_export_utf8) != 0)
        fail_msg("peer failed to read UTF8_STRING from primary Motif process via wl_data_device");
    free(got_utf8);

    uint8_t *got_uri = request_selection_bytes(app, dpy, XtWindow(shell), a_clip, a_uri, &len);
    if (!got_uri || strcmp((const char *)got_uri, g_export_uris) != 0)
        fail_msg("peer failed to read text/uri-list from primary Motif process");
    free(got_uri);

    uint8_t *got_png = request_selection_bytes(app, dpy, XtWindow(shell), a_clip, a_png, &len);
    if (!got_png || len != sizeof(g_export_png) || memcmp(got_png, g_export_png, len) != 0)
        fail_msg("peer failed to read image/png from primary Motif process");
    free(got_png);

    uint8_t *got_prim = request_selection_bytes(app, dpy, XtWindow(shell), XA_PRIMARY, a_utf8, &len);
    if (!got_prim || strcmp((const char *)got_prim, g_export_utf8) != 0)
        fail_msg("peer failed to read PRIMARY UTF8_STRING via zwp_primary_selection_v1");
    free(got_prim);

    /* 2. Now claim CLIPBOARD and PRIMARY from the peer so main Motif app reads them back */
    XSetSelectionOwner(dpy, a_clip, XtWindow(shell), CurrentTime);
    XSetSelectionOwner(dpy, XA_PRIMARY, XtWindow(shell), CurrentTime);
    pump_xt_ms(app, dpy, 200);

    write_stage_file(sync_dir, "peer.ready");
    wait_for_stage_file(app, dpy, sync_dir, "peer.done", 8000);

    XtDestroyWidget(shell);
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    return 0;
}

/* ---------- Mode 3: Full Headless Compositor Qualification ---------- */

static void
switch_motif_theme_resources(Display *dpy, Widget root_shell, XmAppearanceProfile profile)
{
    XmApplyAppearanceProfile(dpy, profile);
    XClearArea(dpy, XtWindow(root_shell), 0, 0, 0, 0, True);
}

static int
run_compositor_check(const char *self_exe, const char *sync_dir)
{
    const char *active = getenv("CK_MOTIF_WAYLAND_ACTIVE");
    const char *dpy_env = getenv("DISPLAY");
    if (!active || strcmp(active, "1") != 0)
        fail_msg("CK_MOTIF_WAYLAND_ACTIVE was not activated automatically with DISPLAY unset");
    if (!dpy_env || !strstr(dpy_env, "/ck-wlx11-"))
        fail_msg("DISPLAY does not point to private ck-wlx11 runtime socket");
    if (!verify_maps_has_libxm())
        fail_msg("/proc/self/maps does not map libXm.so");

    struct stat st_sock, st_dir;
    if (stat(dpy_env, &st_sock) != 0 || (st_sock.st_mode & 0777) != 0600)
        fail_msg("ck-wlx11 socket does not exist or is not 0600");
    char dir_buf[512];
    snprintf(dir_buf, sizeof(dir_buf), "%s", dpy_env);
    char *slash = strrchr(dir_buf, '/');
    if (!slash) fail_msg("unexpected socket path");
    *slash = '\0';
    if (stat(dir_buf, &st_dir) != 0 || (st_dir.st_mode & 0777) != 0700)
        fail_msg("ck-wlx11 runtime directory is not 0700");

    pid_t cpid = fork();
    if (cpid == 0) {
        execl(self_exe, self_exe, "--child-check", dpy_env, (char *)NULL);
        _exit(127);
    }
    int cstatus = 0;
    if (waitpid(cpid, &cstatus, 0) < 0 || !WIFEXITED(cstatus) || WEXITSTATUS(cstatus) != 0)
        fail_msg("child-check failed");
    if (stat(dpy_env, &st_sock) != 0)
        fail_msg("child exit unlinked parent's ck-wlx11 socket");

    XtSetLanguageProc(NULL, NULL, NULL);
    XtToolkitInitialize();
    XtAppContext app = XtCreateApplicationContext();

    String fallback_res[] = {
        (String)"*appearanceProfile: classic",
        (String)"*renderTable: rt",
        (String)"*renderTable.rt.fontType: FONT_IS_XFT",
        (String)"*renderTable.rt.fontName: Noto Sans CJK JP",
        (String)"*renderTable.rt.fontSize: 11",
        NULL
    };
    XtAppSetFallbackResources(app, fallback_res);

    setenv("TT_SESSION", "01 4242 1342177281 1 1 501 127.0.0.1 4", 1);

    int argc = 1;
    char *argv[] = { (char *)"wayland-check", NULL };
    Display *dpy = XtOpenDisplay(app, NULL, "waylandCheck", "WaylandCheck", NULL, 0, &argc, argv);
    if (!dpy)
        fail_msg("XtOpenDisplay failed");

    /* Verify _SUN_TT_SESSION / TT_SESSION root properties use XA_STRING for libtt tt_X_session */
    {
        Atom a_sun_tt = XInternAtom(dpy, "_SUN_TT_SESSION", False);
        Atom actual_type = None;
        int actual_fmt = 0;
        unsigned long nitems = 0, bytes_after = 0;
        unsigned char *prop_data = NULL;
        if (XGetWindowProperty(dpy, DefaultRootWindow(dpy), a_sun_tt, 0, 128, False,
                               XA_STRING, &actual_type, &actual_fmt, &nitems,
                               &bytes_after, &prop_data) != Success ||
            actual_type != XA_STRING || actual_fmt != 8 || nitems == 0 || !prop_data) {
            fail_msg("_SUN_TT_SESSION not seeded as XA_STRING on virtual Wayland root window");
        }
        XFree(prop_data);
    }

    const char *exts[] = {
        "BIG-REQUESTS", "XC-MISC", "RENDER", "SHAPE", "XFIXES", "RANDR", "XINERAMA"
    };
    for (size_t i = 0; i < sizeof(exts) / sizeof(exts[0]); ++i) {
        int maj = 0, evb = 0, errb = 0;
        if (!XQueryExtension(dpy, exts[i], &maj, &evb, &errb))
            fail_msg("missing required extension on virtualizer");
    }
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(dpy, 0, 32, TrueColor, &vinfo))
        fail_msg("missing 32-bit ARGB TrueColor visual");
    if (XGetSelectionOwner(dpy, XInternAtom(dpy, "_NET_WM_CM_S0", False)) == None)
        fail_msg("_NET_WM_CM_S0 compositing selection owner not advertised");

    Widget shell = XtVaAppCreateShell(
        "waylandCheck", "WaylandCheck", applicationShellWidgetClass, dpy,
        XmNtitle, "Motif Native Wayland Runtime",
        XmNwidth, 620,
        XmNheight, 420,
        NULL);

    Pixmap icon_pix = XCreatePixmap(dpy, DefaultRootWindow(dpy), 24, 24, 24);
    Pixmap icon_mask = XCreatePixmap(dpy, DefaultRootWindow(dpy), 24, 24, 1);
    GC igc = XCreateGC(dpy, icon_pix, 0, NULL);
    XSetForeground(dpy, igc, 0x2a76c6);
    XFillRectangle(dpy, icon_pix, igc, 0, 0, 24, 24);
    XSetForeground(dpy, igc, 0xffffff);
    XFillRectangle(dpy, icon_pix, igc, 5, 5, 14, 14);
    XFreeGC(dpy, igc);
    GC mgc = XCreateGC(dpy, icon_mask, 0, NULL);
    XSetForeground(dpy, mgc, 1);
    XFillRectangle(dpy, icon_mask, mgc, 2, 2, 20, 20);
    XFreeGC(dpy, mgc);
    XtVaSetValues(shell, XmNiconPixmap, icon_pix, XmNiconMask, icon_mask, NULL);

    Widget main_w = XmCreateMainWindow(shell, (char *)"mainW", NULL, 0);
    XtManageChild(main_w);

    Widget menu_bar = XmCreateMenuBar(main_w, (char *)"menuBar", NULL, 0);
    XtManageChild(menu_bar);

    Widget file_pd = XmCreatePulldownMenu(menu_bar, (char *)"filePulldown", NULL, 0);
    XmString s_open = XmStringCreateLocalized((char *)"Open Document");
    XmString s_save = XmStringCreateLocalized((char *)"Save Snapshot");
    XmString s_quit = XmStringCreateLocalized((char *)"Quit");
    Widget mi_open = XtVaCreateManagedWidget("miOpen", xmPushButtonWidgetClass, file_pd, XmNlabelString, s_open, NULL);
    Widget mi_save = XtVaCreateManagedWidget("miSave", xmPushButtonWidgetClass, file_pd, XmNlabelString, s_save, NULL);
    Widget mi_quit = XtVaCreateManagedWidget("miQuit", xmPushButtonWidgetClass, file_pd, XmNlabelString, s_quit, NULL);
    (void)mi_open; (void)mi_save; (void)mi_quit;
    XmStringFree(s_open); XmStringFree(s_save); XmStringFree(s_quit);

    XmString s_file = XmStringCreateLocalized((char *)"File");
    Widget file_cb = XtVaCreateManagedWidget(
        "fileCascade", xmCascadeButtonWidgetClass, menu_bar,
        XmNlabelString, s_file, XmNsubMenuId, file_pd, NULL);
    (void)file_cb;
    XmStringFree(s_file);

    Widget form = XtVaCreateManagedWidget(
        "contentForm", xmFormWidgetClass, main_w,
        XmNmarginWidth, 14, XmNmarginHeight, 14, NULL);
    XmMainWindowSetAreas(main_w, menu_bar, NULL, NULL, NULL, form);

    Widget ctx_menu = XmCreatePopupMenu(form, (char *)"ctxPopup", NULL, 0);
    XmString sc1 = XmStringCreateLocalized((char *)"Open Document");
    XmString sc2 = XmStringCreateLocalized((char *)"Save Snapshot");
    XmString sc3 = XmStringCreateLocalized((char *)"Inspect Surface");
    XtVaCreateManagedWidget("ctxOpen", xmPushButtonWidgetClass, ctx_menu, XmNlabelString, sc1, NULL);
    XtVaCreateManagedWidget("ctxSave", xmPushButtonWidgetClass, ctx_menu, XmNlabelString, sc2, NULL);
    XtVaCreateManagedWidget("ctxInsp", xmPushButtonWidgetClass, ctx_menu, XmNlabelString, sc3, NULL);
    XmStringFree(sc1); XmStringFree(sc2); XmStringFree(sc3);

    XmString s_hdr = XmStringCreateLocalized((char *)"Native Wayland Rasterizer — 日本語 — Français");
    Widget hdr_lbl = XtVaCreateManagedWidget(
        "hdrLabel", xmLabelWidgetClass, form,
        XmNlabelString, s_hdr,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);
    XmStringFree(s_hdr);

    XmString s_btn = XmStringCreateLocalized((char *)"Execute Action (UTF-8 日本語)");
    Widget push_btn = XtVaCreateManagedWidget(
        "pushBtn", xmPushButtonWidgetClass, form,
        XmNlabelString, s_btn,
        XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, hdr_lbl, XmNtopOffset, 12,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);
    XmStringFree(s_btn);

    XmString s_tog = XmStringCreateLocalized((char *)"Enable Wayland Subsurface Sync");
    Widget tog_btn = XtVaCreateManagedWidget(
        "togBtn", xmToggleButtonWidgetClass, form,
        XmNlabelString, s_tog, XmNset, True,
        XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, push_btn, XmNtopOffset, 10,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);
    XmStringFree(s_tog);

    Widget scale_w = XtVaCreateManagedWidget(
        "demoScale", xmScaleWidgetClass, form,
        XmNorientation, XmHORIZONTAL,
        XmNminimum, 0, XmNmaximum, 100, XmNvalue, 65, XmNshowValue, True,
        XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, tog_btn, XmNtopOffset, 12,
        XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
        NULL);

    Widget text_f = XtVaCreateManagedWidget(
        "demoTextF", xmTextFieldWidgetClass, form,
        XmNvalue, "Editable XmTextField — かな漢字変換",
        XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, scale_w, XmNtopOffset, 12,
        XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
        NULL);

    Widget text_m = XtVaCreateManagedWidget(
        "demoText", xmTextWidgetClass, form,
        XmNeditMode, XmMULTI_LINE_EDIT,
        XmNrows, 4,
        XmNvalue, "Multi-line XmText rendered directly into wl_shm via pixman-1 & HarfBuzz.",
        XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, text_f, XmNtopOffset, 12,
        XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);
    (void)text_m;

    XtAddEventHandler(shell, 0, True, sel_owner_event_handler, NULL);
    XtRealizeWidget(shell);
    pump_xt_ms(app, dpy, 500);

    XWindowAttributes xwa;
    if (!XGetWindowAttributes(dpy, XtWindow(shell), &xwa) || xwa.map_state != IsViewable)
        fail_msg("top-level ApplicationShell is not viewable");

    /* Verify public <Xm/Wayland.h> embedding API */
    if (!XmWaylandIsActive(dpy))
        fail_msg("XmWaylandIsActive(dpy) returned False");
    struct wl_display *pub_wl_dpy = NULL;
    struct wl_compositor *pub_wl_comp = NULL;
    struct wl_subcompositor *pub_wl_sub = NULL;
    struct wl_shm *pub_wl_shm = NULL;
    if (!XmWaylandGetDisplayHandles(dpy, &pub_wl_dpy, &pub_wl_comp, &pub_wl_sub, &pub_wl_shm, NULL) ||
        !pub_wl_dpy || !pub_wl_comp || !pub_wl_shm) {
        fail_msg("XmWaylandGetDisplayHandles failed");
    }
    struct wl_surface *top_surf = NULL;
    int sub_x = -1, sub_y = -1, sub_w = 0, sub_h = 0;
    double sub_scale = 0.0;
    XmWaylandLock(dpy);
    Boolean got_surf = XmWaylandGetWidgetSurface(
        text_m, &top_surf, &sub_x, &sub_y, &sub_w, &sub_h, &sub_scale);
    XmWaylandUnlock(dpy);
    if (!got_surf || !top_surf || sub_x < 0 || sub_y <= 0 || sub_w <= 0 || sub_h <= 0 || sub_scale < 0.5)
        fail_msg("XmWaylandGetWidgetSurface returned invalid widget geometry");

    uint32_t classic_luma = 0;
    size_t classic_colors = count_distinct_colors_in_window(dpy, XtWindow(shell), xwa.width, xwa.height, &classic_luma);
    if (classic_colors < 4)
        fail_msg("classic theme rendered fewer than 4 distinct colors");

    write_stage_file(sync_dir, "stage_classic.ready");
    wait_for_stage_file(app, dpy, sync_dir, "stage_classic.done", 8000);

    /* Post xdg_popup context menu shell via XmMenuPosition + XtManageChild */
    XButtonPressedEvent bev;
    memset(&bev, 0, sizeof(bev));
    bev.type = ButtonPress;
    bev.display = dpy;
    bev.window = XtWindow(form);
    bev.root = DefaultRootWindow(dpy);
    bev.x = 60;
    bev.y = 70;
    bev.x_root = xwa.x + 60;
    bev.y_root = xwa.y + 96;
    bev.button = Button3;
    XmMenuPosition(ctx_menu, &bev);
    XtManageChild(ctx_menu);
    pump_xt_ms(app, dpy, 400);

    Widget popup_shell = XtParent(ctx_menu);
    XWindowAttributes pwa;
    if (!XGetWindowAttributes(dpy, XtWindow(popup_shell), &pwa) || pwa.map_state != IsViewable || pwa.width < 60)
        fail_msg("xdg_popup menu shell did not become viewable at natural size");
    size_t popup_colors = count_distinct_colors_in_window(dpy, XtWindow(popup_shell), pwa.width, pwa.height, NULL);
    if (popup_colors < 2)
        fail_msg("xdg_popup menu shell pixels are blank");

    write_stage_file(sync_dir, "stage_popup.ready");
    wait_for_stage_file(app, dpy, sync_dir, "stage_popup.done", 8000);
    XtUnmanageChild(ctx_menu);
    pump_xt_ms(app, dpy, 200);

    /* Switch to Modern Light and verify XRender rasterization */
    switch_motif_theme_resources(dpy, shell, XmAPPEARANCE_MODERN_LIGHT);
    pump_xt_ms(app, dpy, 450);
    uint32_t light_luma = 0;
    size_t light_colors = count_distinct_colors_in_window(dpy, XtWindow(shell), xwa.width, xwa.height, &light_luma);
    if (light_colors < 4 || light_luma < 140)
        fail_msg("modern_light theme failed luma/color verification");

    write_stage_file(sync_dir, "stage_modern_light.ready");
    wait_for_stage_file(app, dpy, sync_dir, "stage_modern_light.done", 8000);

    /* Switch to Modern Dark and verify dark luma contrast */
    switch_motif_theme_resources(dpy, shell, XmAPPEARANCE_MODERN_DARK);
    pump_xt_ms(app, dpy, 450);
    uint32_t dark_luma = 0;
    size_t dark_colors = count_distinct_colors_in_window(dpy, XtWindow(shell), xwa.width, xwa.height, &dark_luma);
    if (dark_colors < 4 || dark_luma >= light_luma)
        fail_msg("modern_dark theme failed luma shift verification");

    write_stage_file(sync_dir, "stage_modern_dark.ready");
    wait_for_stage_file(app, dpy, sync_dir, "stage_modern_dark.done", 8000);

    /* Live Fractional / HiDPI Scale Verification (compositor 2x buffer scale + explicit logical zoom) */
    write_stage_file(sync_dir, "stage_scale2x.request");
    wait_for_stage_file(app, dpy, sync_dir, "stage_scale2x.applied", 8000);
    pump_xt_ms(app, dpy, 500);

    double surf_scale = 1.0;
    if (!XmWaylandGetWidgetSurface(shell, NULL, NULL, NULL, NULL, NULL, &surf_scale))
        fail_msg("XmWaylandGetWidgetSurface failed on top-level shell");
    if (fabs(surf_scale - 2.0) > 0.05)
        fail_msg("compositor 2.0x output scale did not update XmWaylandGetWidgetSurface physical scale");

    double cur_scale = 1.0;
    XmGetWidgetScale(shell, &cur_scale);
    if (fabs(cur_scale - 1.0) > 0.05)
        fail_msg("compositor 2.0x output scale double-scaled logical XmGetWidgetScale / Xft.dpi");

    XFontStruct *fixed_fnt = XLoadQueryFont(dpy, "fixed");
    if (!fixed_fnt)
        fail_msg("XLoadQueryFont('fixed') failed under 2.0x Wayland output scale");
    int fixed_h = fixed_fnt->ascent + fixed_fnt->descent;
    XFreeFont(dpy, fixed_fnt);
    if (fixed_h > 20)
        fail_msg("compositor 2.0x output scale double-scaled core 'fixed' font metrics");

    /* Verify opened XLFD fonts are retained in font_buckets (distinct metrics + fast PolyText8) */
    {
        XFontStruct *xxl_fnt = XLoadQueryFont(
            dpy, "-dt-interface system-bold-r-normal-xxl-22-220-96-96-p-120-iso8859-1");
        if (!xxl_fnt)
            fail_msg("XLoadQueryFont failed for xxl XLFD font");
        int xxl_h = xxl_fnt->ascent + xxl_fnt->descent;
        if (xxl_h <= fixed_h)
            fail_msg("opened XLFD font fell back to 'fixed' instead of caching in font_buckets");

        Pixmap txt_pix = XCreatePixmap(dpy, DefaultRootWindow(dpy), 320, 48, 24);
        GC tgc = XCreateGC(dpy, txt_pix, 0, NULL);
        XSetFont(dpy, tgc, xxl_fnt->fid);
        XSetForeground(dpy, tgc, 0x000000);
        uint64_t t0 = mono_ms();
        for (int iter = 0; iter < 400; ++iter) {
            XDrawString(dpy, txt_pix, tgc, 4, 28, "Cached PolyText8 resize span", 28);
        }
        XSync(dpy, False);
        uint64_t dt_ms = mono_ms() - t0;
        XFreeGC(dpy, tgc);
        XFreePixmap(dpy, txt_pix);
        XFreeFont(dpy, xxl_fnt);
        if (dt_ms > 500)
            fail_msg("PolyText8 font cache regression: 400 spans exceeded 500 ms");
    }

    /* Verify XRender Trapezoids/Triangles/TriStrip/TriFan subwindow & source-origin coordinates */
    {
        int scr = DefaultScreen(dpy);
        Window sub = XCreateSimpleWindow(dpy, XtWindow(shell), 12, 41, 80, 64, 0, 0, 0x000000);
        XMapWindow(dpy, sub);
        pump_xt_ms(app, dpy, 100);

        Pixmap spm = XCreatePixmap(dpy, XtWindow(shell), 96, 96, 24);
        XImage *simg = XGetImage(dpy, spm, 0, 0, 96, 96, AllPlanes, ZPixmap);
        if (!simg)
            fail_msg("XGetImage failed on XRender test source pixmap");
        for (int sy = 0; sy < 96; ++sy)
            for (int sx = 0; sx < 96; ++sx)
                XPutPixel(simg, sx, sy, ((unsigned long)sx << 16) | ((unsigned long)sy << 8) | 0xaaUL);
        GC sgc = XCreateGC(dpy, spm, 0, NULL);
        XPutImage(dpy, spm, sgc, simg, 0, 0, 0, 0, 96, 96);
        XDestroyImage(simg);

        XRenderPictFormat *fmt24 = XRenderFindVisualFormat(dpy, DefaultVisual(dpy, scr));
        XRenderPictFormat *fmtA8 = XRenderFindStandardFormat(dpy, PictStandardA8);
        if (!fmt24 || !fmtA8)
            fail_msg("XRenderFindVisualFormat / PictStandardA8 failed");
        Picture psrc = XRenderCreatePicture(dpy, spm, fmt24, 0, NULL);
        Picture pdst = XRenderCreatePicture(dpy, sub, fmt24, 0, NULL);

        XSetForeground(dpy, sgc, 0x000000);
        XFillRectangle(dpy, sub, sgc, 0, 0, 80, 64);
        XTrapezoid traps[2] = {
            {
                .top = XDoubleToFixed(10.0),
                .bottom = XDoubleToFixed(20.0),
                .left = { { XDoubleToFixed(8.0), XDoubleToFixed(10.0) },
                          { XDoubleToFixed(8.0), XDoubleToFixed(20.0) } },
                .right = { { XDoubleToFixed(18.0), XDoubleToFixed(10.0) },
                           { XDoubleToFixed(18.0), XDoubleToFixed(20.0) } },
            },
            {
                .top = XDoubleToFixed(10.0),
                .bottom = XDoubleToFixed(20.0),
                .left = { { XDoubleToFixed(32.0), XDoubleToFixed(10.0) },
                          { XDoubleToFixed(32.0), XDoubleToFixed(20.0) } },
                .right = { { XDoubleToFixed(42.0), XDoubleToFixed(10.0) },
                           { XDoubleToFixed(42.0), XDoubleToFixed(20.0) } },
            },
        };
        XRenderCompositeTrapezoids(dpy, PictOpSrc, psrc, pdst, fmtA8, 40, 50, traps, 2);
        XSync(dpy, False);

        XImage *out = XGetImage(dpy, sub, 0, 0, 80, 64, AllPlanes, ZPixmap);
        if (!out)
            fail_msg("XGetImage failed on subwindow after XRenderCompositeTrapezoids");
        int min_x = 999, min_y = 999, max_x = -1, max_y = -1;
        for (int ry = 0; ry < 64; ++ry) {
            for (int rx = 0; rx < 80; ++rx) {
                if ((XGetPixel(out, rx, ry) & 0xffUL) == 0xaaUL) {
                    if (rx < min_x) min_x = rx;
                    if (ry < min_y) min_y = ry;
                    if (rx > max_x) max_x = rx;
                    if (ry > max_y) max_y = ry;
                }
            }
        }
        unsigned long p0 = XGetPixel(out, 8, 10);
        unsigned long p1 = XGetPixel(out, 32, 10);
        XDestroyImage(out);
        if (min_x != 8 || max_x != 41 || min_y != 10 || max_y != 19)
            fail_msg("XRenderCompositeTrapezoids subwindow destination coordinates mispositioned");
        if (((p0 >> 16) & 0xffUL) != 40 || ((p0 >> 8) & 0xffUL) != 50 ||
            ((p1 >> 16) & 0xffUL) != 64 || ((p1 >> 8) & 0xffUL) != 50)
            fail_msg("XRenderCompositeTrapezoids source origin offset mismatch");

        XFillRectangle(dpy, sub, sgc, 0, 0, 80, 64);
        XPointFixed strip[4] = {
            { XDoubleToFixed(6.0),  XDoubleToFixed(12.0) },
            { XDoubleToFixed(16.0), XDoubleToFixed(12.0) },
            { XDoubleToFixed(6.0),  XDoubleToFixed(22.0) },
            { XDoubleToFixed(16.0), XDoubleToFixed(22.0) },
        };
        XRenderCompositeTriStrip(dpy, PictOpSrc, psrc, pdst, fmtA8, 30, 35, strip, 4);
        XSync(dpy, False);

        out = XGetImage(dpy, sub, 0, 0, 80, 64, AllPlanes, ZPixmap);
        if (!out)
            fail_msg("XGetImage failed on subwindow after XRenderCompositeTriStrip");
        unsigned long ps = XGetPixel(out, 8, 14);
        unsigned long p_outside = XGetPixel(out, 20, 53);
        XDestroyImage(out);
        if ((ps & 0xffUL) != 0xaaUL || ((ps >> 16) & 0xffUL) != 32 || ((ps >> 8) & 0xffUL) != 37 ||
            (p_outside & 0xffUL) == 0xaaUL)
            fail_msg("XRenderCompositeTriStrip subwindow/source coordinates mispositioned");

        XRenderFreePicture(dpy, pdst);
        XRenderFreePicture(dpy, psrc);
        XFreeGC(dpy, sgc);
        XFreePixmap(dpy, spm);
        XDestroyWindow(dpy, sub);
    }

    char **missing_cs = NULL;
    int missing_cnt = 0;
    char *def_str = NULL;
    XFontSet fs = XCreateFontSet(
        dpy, "-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*",
        &missing_cs, &missing_cnt, &def_str);
    if (missing_cs)
        XFreeStringList(missing_cs);
    if (!fs)
        fail_msg("XCreateFontSet failed for wildcard XLFD pattern under UTF-8 locale");
    XFreeFontSet(dpy, fs);

    Atom a_test_sc = XInternAtom(dpy, "_MOTIF_WAYLAND_TEST_SCALE", False);
    XChangeProperty(dpy, DefaultRootWindow(dpy), a_test_sc, XA_STRING, 8,
                    PropModeReplace, (const unsigned char *)"2.0", 3);
    pump_xt_ms(app, dpy, 400);
    XmGetWidgetScale(shell, &cur_scale);
    if (fabs(cur_scale - 2.0) > 0.05)
        fail_msg("explicit 2.0x Wayland logical zoom did not update XmGetWidgetScale");

    write_stage_file(sync_dir, "stage_scale2x.ready");
    wait_for_stage_file(app, dpy, sync_dir, "stage_scale2x.done", 8000);

    XChangeProperty(dpy, DefaultRootWindow(dpy), a_test_sc, XA_STRING, 8,
                    PropModeReplace, (const unsigned char *)"1.0", 3);
    pump_xt_ms(app, dpy, 400);
    XmGetWidgetScale(shell, &cur_scale);
    if (fabs(cur_scale - 1.0) > 0.05)
        fail_msg("live 1.0x Wayland scale restore did not update XmGetWidgetScale");

    XFreePixmap(dpy, icon_pix);
    XFreePixmap(dpy, icon_mask);
    XtDestroyWidget(shell);
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    printf("PASS compositor-check (classic_colors=%zu light_luma=%u dark_luma=%u)\n",
           classic_colors, light_luma, dark_luma);
    return 0;
}

/* ---------- Mode 4: Deterministic zwp_text_input_v3 IME, Cross-Process Clipboard & DnD ---------- */

static char g_dropped_uri[256];
static bool g_drop_received = false;
static int g_resize_dispatches = 0;
static int g_resize_reversals = 0;
static int g_last_cfg_w = 0;
static int g_last_cfg_h = 0;

static void
resize_audit_eh(Widget w, XtPointer client_data, XEvent *ev, Boolean *cont)
{
    (void)client_data;
    (void)cont;
    if (ev->type == ConfigureNotify && ev->xconfigure.window == XtWindow(w)) {
        int nw = ev->xconfigure.width;
        int nh = ev->xconfigure.height;
        if (nw != g_last_cfg_w || nh != g_last_cfg_h) {
            if (g_last_cfg_w > 0 && (nw < g_last_cfg_w || nh < g_last_cfg_h))
                g_resize_reversals++;
            g_last_cfg_w = nw;
            g_last_cfg_h = nh;
            g_resize_dispatches++;
        }
    }
}

static void
drop_transfer_cb(Widget w, XtPointer closure, Atom *seltype, Atom *type,
                 XtPointer value, unsigned long *length, int *format)
{
    (void)w; (void)closure; (void)seltype; (void)type; (void)format;
    if (value && length && *length > 0) {
        size_t n = *length < sizeof(g_dropped_uri) - 1 ? *length : sizeof(g_dropped_uri) - 1;
        memcpy(g_dropped_uri, value, n);
        g_dropped_uri[n] = '\0';
        g_drop_received = true;
    }
    XtFree((char *)value);
}

static void
drop_site_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    (void)client_data;
    XmDropProcCallback dd = (XmDropProcCallback)call_data;
    Atom a_uri = XInternAtom(XtDisplay(w), "text/uri-list", False);
    XmDropTransferEntryRec entries[1];
    entries[0].target = a_uri;
    entries[0].client_data = NULL;

    Arg args[4];
    Cardinal n = 0;
    if (dd->dropAction != XmDROP || dd->operation == XmDROP_NOOP) {
        XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
        XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
    } else {
        dd->operation = XmDROP_COPY;
        dd->dropSiteStatus = XmVALID_DROP_SITE;
        XtSetArg(args[n], XmNdropTransfers, entries); n++;
        XtSetArg(args[n], XmNnumDropTransfers, 1); n++;
        XtSetArg(args[n], XmNtransferProc, drop_transfer_cb); n++;
    }
    XmDropTransferStart(dd->dragContext, args, n);
}

static int
run_ime_dnd_check(const char *self_exe, const char *sync_dir)
{
    XtSetLanguageProc(NULL, NULL, NULL);
    XtToolkitInitialize();
    XtAppContext app = XtCreateApplicationContext();
    int argc = 1;
    char *argv[] = { (char *)"ime-dnd-check", NULL };
    Display *dpy = XtOpenDisplay(app, NULL, "imeDnd", "ImeDnd", NULL, 0, &argc, argv);
    if (!dpy)
        fail_msg("ime-dnd XtOpenDisplay failed");

    Widget shell = XtVaAppCreateShell(
        "imeDnd", "ImeDnd", applicationShellWidgetClass, dpy,
        XmNwidth, 360, XmNheight, 220, XmNtitle, "IME & DnD Test", NULL);

    Widget form = XtVaCreateManagedWidget("form", xmFormWidgetClass, shell, NULL);
    Widget tf = XtVaCreateManagedWidget(
        "imeField", xmTextFieldWidgetClass, form,
        XmNvalue, "",
        XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 12,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 12,
        XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 12,
        NULL);

    XmString s_drop = XmStringCreateLocalized((char *)"Wayland Drop Target");
    Widget drop_lbl = XtVaCreateManagedWidget(
        "dropTarget", xmLabelWidgetClass, form,
        XmNlabelString, s_drop,
        XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, tf, XmNtopOffset, 16,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 12,
        XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 12,
        XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 12,
        NULL);
    XmStringFree(s_drop);

    Atom import_targets[1] = { XInternAtom(dpy, "text/uri-list", False) };
    Arg dargs[6];
    Cardinal dn = 0;
    XtSetArg(dargs[dn], XmNimportTargets, import_targets); dn++;
    XtSetArg(dargs[dn], XmNnumImportTargets, 1); dn++;
    XtSetArg(dargs[dn], XmNdropSiteOperations, XmDROP_COPY | XmDROP_MOVE); dn++;
    XtSetArg(dargs[dn], XmNdropProc, drop_site_proc); dn++;
    XmDropSiteRegister(drop_lbl, dargs, dn);

    XtAddEventHandler(shell, 0, True, sel_owner_event_handler, NULL);
    XtRealizeWidget(shell);
    pump_xt_ms(app, dpy, 300);
    XmProcessTraversal(tf, XmTRAVERSE_CURRENT);
    pump_xt_ms(app, dpy, 200);

    write_stage_file(sync_dir, "ime_client.ready");
    wait_for_stage_file(app, dpy, sync_dir, "ime_preedit1.sent", 8000);
    pump_xt_ms(app, dpy, 250);

    char *val1 = XmTextFieldGetString(tf);
    if (!val1 || strcmp(val1, "にほん") != 0) {
        fprintf(stderr, "FAIL: expected preedit1 'にほん', got '%s'\n", val1 ? val1 : "(null)");
        exit(1);
    }
    XtFree(val1);
    write_stage_file(sync_dir, "ime_preedit1.verified");

    wait_for_stage_file(app, dpy, sync_dir, "ime_commit.sent", 8000);
    pump_xt_ms(app, dpy, 250);

    char *val2 = XmTextFieldGetString(tf);
    if (!val2 || strcmp(val2, "日本語!") != 0) {
        fprintf(stderr, "FAIL: expected committed '日本語!', got '%s'\n", val2 ? val2 : "(null)");
        exit(1);
    }
    XtFree(val2);
    write_stage_file(sync_dir, "ime_commit.verified");

    /* Wait for incoming Wayland wl_data_device drag-and-drop onto drop_lbl */
    uint64_t deadline = mono_ms() + 8000;
    while (!g_drop_received && mono_ms() < deadline) {
        pump_xt_ms(app, dpy, 50);
    }
    if (!g_drop_received || !strstr(g_dropped_uri, "file:///wayland/dropped-item.txt")) {
        fprintf(stderr, "FAIL: Wayland DnD drop not received (got '%s')\n", g_dropped_uri);
        exit(1);
    }
    write_stage_file(sync_dir, "dnd_drop.verified");

    /* Cross-process Wayland CLIPBOARD & PRIMARY exchange with --clipboard-peer */
    Atom a_clip = XInternAtom(dpy, "CLIPBOARD", False);
    Atom a_utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    Atom a_uri = XInternAtom(dpy, "text/uri-list", False);
    Atom a_png = XInternAtom(dpy, "image/png", False);

    XSetSelectionOwner(dpy, a_clip, XtWindow(shell), CurrentTime);
    XSetSelectionOwner(dpy, XA_PRIMARY, XtWindow(shell), CurrentTime);
    pump_xt_ms(app, dpy, 200);

    pid_t peer_pid = fork();
    if (peer_pid == 0) {
        unsetenv("DISPLAY");
        unsetenv("CK_MOTIF_WAYLAND_ACTIVE");
        execl(self_exe, self_exe, "--clipboard-peer", sync_dir, (char *)NULL);
        _exit(127);
    }

    wait_for_stage_file(app, dpy, sync_dir, "peer.ready", 8000);
    pump_xt_ms(app, dpy, 200);

    size_t rlen = 0;
    uint8_t *from_peer_utf8 = request_selection_bytes(app, dpy, XtWindow(shell), a_clip, a_utf8, &rlen);
    if (!from_peer_utf8 || strcmp((const char *)from_peer_utf8, g_peer_utf8) != 0)
        fail_msg("failed to read external Wayland peer UTF8_STRING clipboard offer");
    free(from_peer_utf8);

    uint8_t *from_peer_uri = request_selection_bytes(app, dpy, XtWindow(shell), a_clip, a_uri, &rlen);
    if (!from_peer_uri || strcmp((const char *)from_peer_uri, g_peer_uris) != 0)
        fail_msg("failed to read external Wayland peer text/uri-list offer");
    free(from_peer_uri);

    uint8_t *from_peer_png = request_selection_bytes(app, dpy, XtWindow(shell), a_clip, a_png, &rlen);
    if (!from_peer_png || rlen != sizeof(g_peer_png) || memcmp(from_peer_png, g_peer_png, rlen) != 0)
        fail_msg("failed to read external Wayland peer image/png offer");
    free(from_peer_png);

    uint8_t *from_peer_prim = request_selection_bytes(app, dpy, XtWindow(shell), XA_PRIMARY, a_utf8, &rlen);
    if (!from_peer_prim || strcmp((const char *)from_peer_prim, g_peer_utf8) != 0)
        fail_msg("failed to read external Wayland peer PRIMARY selection offer");
    free(from_peer_prim);

    write_stage_file(sync_dir, "peer.done");
    int peer_status = 0;
    if (waitpid(peer_pid, &peer_status, 0) < 0 || !WIFEXITED(peer_status) || WEXITSTATUS(peer_status) != 0)
        fail_msg("external Wayland clipboard peer exited with failure");

    write_stage_file(sync_dir, "clipboard_exchange.verified");

    /* Verify 40-step rapid xdg_toplevel.configure resize burst coalesces cleanly
     * without accumulating 40 intermediate ConfigureNotify layouts or reversing. */
    Dimension cur_w = 0, cur_h = 0;
    XtVaGetValues(shell, XmNwidth, &cur_w, XmNheight, &cur_h, NULL);
    g_resize_dispatches = 0;
    g_resize_reversals = 0;
    g_last_cfg_w = (int)cur_w;
    g_last_cfg_h = (int)cur_h;
    XtAddEventHandler(shell, StructureNotifyMask, False, resize_audit_eh, NULL);

    struct timespec busy_sim = {0, 60 * 1000 * 1000L};
    nanosleep(&busy_sim, NULL);
    wait_for_stage_file(app, dpy, sync_dir, "resize_burst.sent", 5000);
    pump_xt_ms(app, dpy, 250);

    XtVaGetValues(shell, XmNwidth, &cur_w, XmNheight, &cur_h, NULL);
    if (cur_w != 600 || cur_h != 380) {
        fprintf(stderr, "FAIL: expected settled resize 600x380, got %ux%u\n",
                (unsigned)cur_w, (unsigned)cur_h);
        exit(1);
    }
    if (g_resize_reversals != 0) {
        fprintf(stderr, "FAIL: detected %d rubber-band size reversals during resize burst\n",
                g_resize_reversals);
        exit(1);
    }
    if (g_resize_dispatches < 1 || g_resize_dispatches > 6) {
        fprintf(stderr, "FAIL: resize burst accumulated %d ConfigureNotify dispatches (expected <= 6 out of 40)\n",
                g_resize_dispatches);
        exit(1);
    }
    write_stage_file(sync_dir, "resize_burst.verified");

    /* Verify unfocused hover-wheel scrolling on both:
     * 1) XmAUTOMATIC ScrolledWindow whose DrawingArea child wipes/overrides
     *    translations (<BtnDown>/<BtnUp>: DrawingAreaInput()), and
     * 2) XmAPPLICATION_DEFINED ScrolledWindow with a custom ScrollBar,
     * while keyboard focus stays on tf (XmEXPLICIT). */
    XtVaSetValues(drop_lbl, XmNbottomAttachment, XmATTACH_NONE, XmNheight, 44, NULL);

    Widget sw_auto = XtVaCreateManagedWidget(
        "swAuto", xmScrolledWindowWidgetClass, form,
        XmNscrollingPolicy, XmAUTOMATIC,
        XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 170,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 12,
        XmNwidth, 260, XmNheight, 180,
        NULL);
    Widget da_auto = XtVaCreateManagedWidget(
        "daAuto", xmDrawingAreaWidgetClass, sw_auto,
        XmNwidth, 220, XmNheight, 640,
        NULL);
    XtUninstallTranslations(da_auto);
    XtOverrideTranslations(
        da_auto,
        XtParseTranslationTable("<BtnDown>: DrawingAreaInput()\n<BtnUp>: DrawingAreaInput()"));

    Widget sw_app = XtVaCreateManagedWidget(
        "swApp", xmScrolledWindowWidgetClass, form,
        XmNscrollingPolicy, XmAPPLICATION_DEFINED,
        XmNvisualPolicy, XmVARIABLE,
        XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 170,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 310,
        XmNwidth, 260, XmNheight, 180,
        NULL);
    Widget sb_app = XtVaCreateManagedWidget(
        "sbApp", xmScrollBarWidgetClass, sw_app,
        XmNorientation, XmVERTICAL,
        XmNminimum, 0, XmNmaximum, 200,
        XmNsliderSize, 40, XmNincrement, 10, XmNpageIncrement, 40,
        XmNvalue, 0,
        NULL);
    Widget da_app = XtVaCreateManagedWidget(
        "daApp", xmDrawingAreaWidgetClass, sw_app,
        XmNwidth, 220, XmNheight, 170,
        NULL);
    XmScrolledWindowSetAreas(sw_app, NULL, sb_app, da_app);

    XmProcessTraversal(tf, XmTRAVERSE_CURRENT);
    pump_xt_ms(app, dpy, 200);
    write_stage_file(sync_dir, "wheel_test.ready");
    wait_for_stage_file(app, dpy, sync_dir, "wheel_subnotch.sent", 5000);
    pump_xt_ms(app, dpy, 220);

    Widget sb_auto = NULL;
    XtVaGetValues(sw_auto, XmNverticalScrollBar, &sb_auto, NULL);
    int subnotch_val = 0;
    if (sb_auto)
        XtVaGetValues(sb_auto, XmNvalue, &subnotch_val, NULL);
    if (subnotch_val <= 0 || subnotch_val >= 30) {
        fprintf(stderr, "FAIL: sub-notch smooth scroll expected 1..29 px, got %d\n", subnotch_val);
        exit(1);
    }
    write_stage_file(sync_dir, "wheel_subnotch.verified");

    wait_for_stage_file(app, dpy, sync_dir, "wheel_auto.sent", 5000);
    /* Sample intermediate glide frames on sw_auto to verify smooth multi-frame interpolation */
    int distinct_auto_frames = 0, last_sampled_auto = subnotch_val;
    for (int f = 0; f < 22; ++f) {
        pump_xt_ms(app, dpy, 12);
        int cur_auto = 0;
        if (sb_auto)
            XtVaGetValues(sb_auto, XmNvalue, &cur_auto, NULL);
        if (cur_auto != last_sampled_auto) {
            distinct_auto_frames++;
            last_sampled_auto = cur_auto;
        }
    }
    int auto_val = 0, app_val = 0;
    if (sb_auto)
        XtVaGetValues(sb_auto, XmNvalue, &auto_val, NULL);
    if (auto_val <= subnotch_val + 60 || distinct_auto_frames < 2) {
        fprintf(stderr, "FAIL: unfocused XmAUTOMATIC smooth wheel failed (val=%d frames=%d)\n",
                auto_val, distinct_auto_frames);
        exit(1);
    }
    write_stage_file(sync_dir, "wheel_auto.verified");

    wait_for_stage_file(app, dpy, sync_dir, "wheel_test.sent", 5000);
    pump_xt_ms(app, dpy, 260);
    XtVaGetValues(sb_app, XmNvalue, &app_val, NULL);
    if (app_val != 60) {
        fprintf(stderr, "FAIL: unfocused XmAPPLICATION_DEFINED wheel expected 60, got %d\n", app_val);
        exit(1);
    }
    write_stage_file(sync_dir, "wheel_mid.verified");

    /* Verify rapid continuous scrolling all the way to the end (160) and back to start (0) */
    wait_for_stage_file(app, dpy, sync_dir, "wheel_to_end.sent", 5000);
    pump_xt_ms(app, dpy, 300);
    int app_end = 0;
    XtVaGetValues(sb_app, XmNvalue, &app_end, NULL);
    if (app_end != 160) {
        fprintf(stderr, "FAIL: continuous scroll to end expected 160, got %d\n", app_end);
        exit(1);
    }
    write_stage_file(sync_dir, "wheel_to_end.verified");

    wait_for_stage_file(app, dpy, sync_dir, "wheel_to_start.sent", 5000);
    pump_xt_ms(app, dpy, 300);
    int app_start = -1;
    XtVaGetValues(sb_app, XmNvalue, &app_start, NULL);
    if (app_start != 0) {
        fprintf(stderr, "FAIL: continuous scroll back to start expected 0, got %d\n", app_start);
        exit(1);
    }
    if (XmGetFocusWidget(shell) != tf) {
        fail_msg("hover mouse-wheel unexpectedly stole keyboard focus from XmTextField");
    }
    write_stage_file(sync_dir, "wheel_test.verified");

    XtDestroyWidget(shell);
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    printf("PASS ime-dnd-clipboard-resize-wheel-check (ime='日本語!' dnd='file:///wayland/dropped-item.txt' clipboard=OK resize_dispatches=%d/40 wheel_subnotch=%d wheel_auto=%d[frames=%d] wheel_app=0->%d->%d->%d)\n",
           g_resize_dispatches, subnotch_val, auto_val, distinct_auto_frames, app_val, app_end, app_start);
    return 0;
}

int
main(int argc, char **argv)
{
    if (argc >= 3 && strcmp(argv[1], "--child-check") == 0)
        return run_child_check(argv[2]);
    if (argc >= 3 && strcmp(argv[1], "--clipboard-peer") == 0)
        return run_clipboard_peer(argv[2]);
    if (argc >= 3 && strcmp(argv[1], "--compositor-check") == 0)
        return run_compositor_check(argv[0], argv[2]);
    if (argc >= 3 && strcmp(argv[1], "--ime-dnd-check") == 0)
        return run_ime_dnd_check(argv[0], argv[2]);

    fprintf(stderr, "Usage: %s [--compositor-check|--ime-dnd-check|--clipboard-peer|--child-check] <arg>\n", argv[0]);
    return 2;
}
