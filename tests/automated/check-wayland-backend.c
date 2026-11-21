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

#include <Xm/Appearance.h>
#include <Xm/CascadeB.h>
#include <Xm/Display.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
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

    int argc = 1;
    char *argv[] = { (char *)"wayland-check", NULL };
    Display *dpy = XtOpenDisplay(app, NULL, "waylandCheck", "WaylandCheck", NULL, 0, &argc, argv);
    if (!dpy)
        fail_msg("XtOpenDisplay failed");

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

    /* Live Fractional / HiDPI Scale Reflow (1.0 -> 2.0 -> 1.0) */
    write_stage_file(sync_dir, "stage_scale2x.request");
    wait_for_stage_file(app, dpy, sync_dir, "stage_scale2x.applied", 8000);
    pump_xt_ms(app, dpy, 500);

    double cur_scale = 1.0;
    XmGetWidgetScale(shell, &cur_scale);
    if (fabs(cur_scale - 2.0) > 0.05) {
        Atom a_test_sc = XInternAtom(dpy, "_MOTIF_WAYLAND_TEST_SCALE", False);
        XChangeProperty(dpy, DefaultRootWindow(dpy), a_test_sc, XA_STRING, 8,
                        PropModeReplace, (const unsigned char *)"2.0", 3);
        pump_xt_ms(app, dpy, 400);
        XmGetWidgetScale(shell, &cur_scale);
    }
    if (fabs(cur_scale - 2.0) > 0.05)
        fail_msg("live 2.0x Wayland scale reflow did not update XmGetWidgetScale");

    write_stage_file(sync_dir, "stage_scale2x.ready");
    wait_for_stage_file(app, dpy, sync_dir, "stage_scale2x.done", 8000);

    Atom a_test_sc = XInternAtom(dpy, "_MOTIF_WAYLAND_TEST_SCALE", False);
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

    XtDestroyWidget(shell);
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    printf("PASS ime-dnd-clipboard-check (ime='日本語!' dnd='file:///wayland/dropped-item.txt' clipboard=UTF8+URI+PNG+PRIMARY)\n");
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
