/*
 * Motif
 *
 * Copyright (c) 2026 Combined Motif Validation Suite
 *
 * Verify MWM frame geometry, active/inactive titlebar surfaces, flat modern
 * border strokes vs classic 3D bevels, titlebar button/text foreground
 * rendering, and explicit resource overrides under Xvfb.
 */

#define _POSIX_C_SOURCE 200809L

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    Window client;
    Window base;
    Window frame;
    int base_x;
    int base_y;
    unsigned int frame_w;
    unsigned int frame_h;
    int upper_border;
    int title_height;
} FrameGeometry;

static void sleep_ms(int ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static void pump_events(Display *dpy)
{
    XEvent ev;
    while (XPending(dpy) > 0) {
        XNextEvent(dpy, &ev);
    }
}

static Window create_test_client(Display *dpy, int x, int y,
                                 unsigned int w, unsigned int h,
                                 const char *title)
{
    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    XSetWindowAttributes attrs;
    XWMHints wm_hints;
    XSizeHints size_hints;
    XClassHint class_hint;
    Window win;

    memset(&attrs, 0, sizeof(attrs));
    attrs.background_pixel = WhitePixel(dpy, screen);
    attrs.event_mask = ExposureMask | StructureNotifyMask | FocusChangeMask;

    win = XCreateWindow(dpy, root, x, y, w, h, 0,
                        CopyFromParent, InputOutput, CopyFromParent,
                        CWBackPixel | CWEventMask, &attrs);
    if (win == None) {
        fprintf(stderr, "FAIL: XCreateWindow failed for %s\n", title);
        exit(1);
    }

    XStoreName(dpy, win, title);
    memset(&wm_hints, 0, sizeof(wm_hints));
    wm_hints.flags = InputHint | StateHint;
    wm_hints.input = True;
    wm_hints.initial_state = NormalState;
    XSetWMHints(dpy, win, &wm_hints);

    memset(&size_hints, 0, sizeof(size_hints));
    size_hints.flags = USPosition | USSize;
    size_hints.x = x;
    size_hints.y = y;
    size_hints.width = (int)w;
    size_hints.height = (int)h;
    XSetWMNormalHints(dpy, win, &size_hints);

    class_hint.res_name = (char *)"mwmclient";
    class_hint.res_class = (char *)"MwmClient";
    XSetClassHint(dpy, win, &class_hint);

    return win;
}

static int resolve_frame(Display *dpy, Window client, FrameGeometry *out)
{
    Window root = RootWindow(dpy, DefaultScreen(dpy));
    Window r = None, parent = None, *children = NULL;
    unsigned int nchildren = 0;
    Window base = None, frame = None;
    int x = 0, y = 0;
    unsigned int w = 0, h = 0, bw = 0, depth = 0;

    if (!XQueryTree(dpy, client, &r, &parent, &children, &nchildren))
        return 0;
    if (children)
        XFree(children);
    if (parent == None || parent == root)
        return 0;
    base = parent;

    children = NULL;
    if (!XQueryTree(dpy, base, &r, &parent, &children, &nchildren))
        return 0;
    if (children)
        XFree(children);
    if (parent == None || parent == root)
        return 0;
    frame = parent;

    if (!XGetGeometry(dpy, base, &r, &x, &y, &w, &h, &bw, &depth))
        return 0;
    out->client = client;
    out->base = base;
    out->frame = frame;
    out->base_x = x;
    out->base_y = y;

    if (!XGetGeometry(dpy, frame, &r, &x, &y, &w, &h, &bw, &depth))
        return 0;
    out->frame_w = w;
    out->frame_h = h;
    out->upper_border = out->base_x;
    out->title_height = (out->base_y > out->base_x) ? (out->base_y - out->base_x) : 0;

    /* Find clientTitleWin among frame's children if present */
    children = NULL;
    nchildren = 0;
    if (XQueryTree(dpy, frame, &r, &parent, &children, &nchildren) && children) {
        unsigned int i;
        for (i = 0; i < nchildren; i++) {
            int cx = 0, cy = 0;
            unsigned int cw = 0, ch = 0, cbw = 0, cdepth = 0;
            if (children[i] == base)
                continue;
            if (XGetGeometry(dpy, children[i], &r, &cx, &cy, &cw, &ch, &cbw, &cdepth)) {
                if (cx > 0 && cx == cy &&
                    cw == out->frame_w - 2 * (unsigned int)cx &&
                    cy + (int)ch == out->base_y) {
                    out->upper_border = cx;
                    out->title_height = (int)ch;
                    break;
                }
            }
        }
        XFree(children);
    }
    return 1;
}

static int wait_for_reparent(Display *dpy, Window client, FrameGeometry *out)
{
    int attempt;
    for (attempt = 0; attempt < 100; attempt++) {
        pump_events(dpy);
        if (resolve_frame(dpy, client, out) && out->frame_w > 0 && out->frame_h > 0)
            return 1;
        sleep_ms(30);
    }
    return 0;
}

static unsigned long parse_pixel(Display *dpy, const char *spec)
{
    Colormap cmap = DefaultColormap(dpy, DefaultScreen(dpy));
    XColor color;
    if (!XParseColor(dpy, cmap, spec, &color) || !XAllocColor(dpy, cmap, &color)) {
        fprintf(stderr, "FAIL: unable to parse/alloc color %s\n", spec);
        exit(1);
    }
    return color.pixel;
}

static unsigned long dominant_pixel_in_rect(XImage *img, int x0, int y0, int w, int h)
{
    unsigned long pixels[64];
    int counts[64];
    int n = 0;
    int x, y;
    int best_idx = 0;

    for (y = y0; y < y0 + h; y++) {
        if (y < 0 || y >= img->height)
            continue;
        for (x = x0; x < x0 + w; x++) {
            unsigned long p;
            int i, found = 0;
            if (x < 0 || x >= img->width)
                continue;
            p = XGetPixel(img, x, y);
            for (i = 0; i < n; i++) {
                if (pixels[i] == p) {
                    counts[i]++;
                    found = 1;
                    break;
                }
            }
            if (!found && n < 64) {
                pixels[n] = p;
                counts[n] = 1;
                n++;
            }
        }
    }
    if (n == 0)
        return 0;
    for (x = 1; x < n; x++) {
        if (counts[x] > counts[best_idx])
            best_idx = x;
    }
    return pixels[best_idx];
}

static int count_non_bg_pixels(XImage *img, int x0, int y0, int w, int h,
                               unsigned long bg)
{
    int count = 0;
    int x, y;
    for (y = y0; y < y0 + h; y++) {
        if (y < 0 || y >= img->height)
            continue;
        for (x = x0; x < x0 + w; x++) {
            if (x < 0 || x >= img->width)
                continue;
            if (XGetPixel(img, x, y) != bg)
                count++;
        }
    }
    return count;
}

int main(int argc, char **argv)
{
    const char *profile = "modern-light";
    const char *expect_active_bg = NULL;
    const char *expect_inactive_bg = NULL;
    const char *expect_active_border = NULL;
    int expect_no_title = 0;
    int i;
    Display *dpy;
    Window win1, win2;
    FrameGeometry fg1, fg2;
    XImage *img1, *img2;
    unsigned long active_bg, inactive_bg;
    unsigned long top_border, bot_border;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--profile") == 0 && i + 1 < argc) {
            profile = argv[++i];
        } else if (strcmp(argv[i], "--expect-active-bg") == 0 && i + 1 < argc) {
            expect_active_bg = argv[++i];
        } else if (strcmp(argv[i], "--expect-inactive-bg") == 0 && i + 1 < argc) {
            expect_inactive_bg = argv[++i];
        } else if (strcmp(argv[i], "--expect-active-border") == 0 && i + 1 < argc) {
            expect_active_border = argv[++i];
        } else if (strcmp(argv[i], "--expect-no-title") == 0) {
            expect_no_title = 1;
        } else {
            fprintf(stderr, "Usage: %s [--profile P] [--expect-active-bg HEX] "
                            "[--expect-inactive-bg HEX] [--expect-active-border HEX] "
                            "[--expect-no-title]\n", argv[0]);
            return 2;
        }
    }

    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "FAIL: cannot open DISPLAY\n");
        return 1;
    }

    win1 = create_test_client(dpy, 80, 100, 320, 200, "Inactive Window");
    XMapRaised(dpy, win1);
    XFlush(dpy);
    if (!wait_for_reparent(dpy, win1, &fg1)) {
        fprintf(stderr, "FAIL: MWM did not reparent win1\n");
        return 1;
    }

    win2 = create_test_client(dpy, 500, 100, 320, 200, "Active Window");
    XMapRaised(dpy, win2);
    XFlush(dpy);
    if (!wait_for_reparent(dpy, win2, &fg2)) {
        fprintf(stderr, "FAIL: MWM did not reparent win2\n");
        return 1;
    }

    /* Explicitly focus win2 so win2 is active and win1 is inactive */
    XSetInputFocus(dpy, win2, RevertToParent, CurrentTime);
    XRaiseWindow(dpy, fg2.frame);
    XSync(dpy, False);
    sleep_ms(200);
    pump_events(dpy);
    resolve_frame(dpy, win1, &fg1);
    resolve_frame(dpy, win2, &fg2);

    if (expect_no_title) {
        if (fg2.title_height != 0 || fg2.base_y != fg2.base_x) {
            fprintf(stderr,
                    "FAIL: expected no titlebar under clientDecoration override, "
                    "got base_x=%d base_y=%d title_height=%d\n",
                    fg2.base_x, fg2.base_y, fg2.title_height);
            return 1;
        }
        printf("METRICS profile=%s resize_border=%d upper_border=%d title_height=%d base_y=%d\n",
               profile, fg2.base_x, fg2.upper_border, fg2.title_height, fg2.base_y);
        XDestroyWindow(dpy, win2);
        XDestroyWindow(dpy, win1);
        XCloseDisplay(dpy);
        return 0;
    }

    if (fg2.title_height <= 0 || fg2.base_x <= 0) {
        fprintf(stderr, "FAIL: invalid decorated frame geometry: base_x=%d title_height=%d\n",
                fg2.base_x, fg2.title_height);
        return 1;
    }

    img1 = XGetImage(dpy, fg1.frame, 0, 0, fg1.frame_w, fg1.frame_h, AllPlanes, ZPixmap);
    img2 = XGetImage(dpy, fg2.frame, 0, 0, fg2.frame_w, fg2.frame_h, AllPlanes, ZPixmap);
    if (!img1 || !img2) {
        fprintf(stderr, "FAIL: XGetImage on MWM frame windows failed\n");
        return 1;
    }

    /* Sample titlebar background in the title area */
    inactive_bg = dominant_pixel_in_rect(
        img1, fg1.upper_border + fg1.title_height + 4, fg1.upper_border + 2,
        (int)fg1.frame_w - 2 * fg1.upper_border - 3 * fg1.title_height - 8,
        fg1.title_height - 4);
    active_bg = dominant_pixel_in_rect(
        img2, fg2.upper_border + fg2.title_height + 4, fg2.upper_border + 2,
        (int)fg2.frame_w - 2 * fg2.upper_border - 3 * fg2.title_height - 8,
        fg2.title_height - 4);

    if (active_bg == inactive_bg) {
        fprintf(stderr, "FAIL: active_bg (0x%lx) == inactive_bg (0x%lx) in profile %s\n",
                active_bg, inactive_bg, profile);
        return 1;
    }

    if (expect_active_bg) {
        unsigned long exp = parse_pixel(dpy, expect_active_bg);
        if (active_bg != exp) {
            fprintf(stderr, "FAIL: profile %s active_bg 0x%lx != expected %s (0x%lx)\n",
                    profile, active_bg, expect_active_bg, exp);
            return 1;
        }
    }
    if (expect_inactive_bg) {
        unsigned long exp = parse_pixel(dpy, expect_inactive_bg);
        if (inactive_bg != exp) {
            fprintf(stderr, "FAIL: profile %s inactive_bg 0x%lx != expected %s (0x%lx)\n",
                    profile, inactive_bg, expect_inactive_bg, exp);
            return 1;
        }
    }

    top_border = XGetPixel(img2, (int)fg2.frame_w / 2, 0);
    bot_border = XGetPixel(img2, (int)fg2.frame_w / 2, (int)fg2.frame_h - 1);

    if (strcmp(profile, "classic") == 0) {
        if (top_border == bot_border) {
            fprintf(stderr, "FAIL: classic profile top bevel (0x%lx) == bottom bevel (0x%lx)\n",
                    top_border, bot_border);
            return 1;
        }
    } else {
        if (top_border != bot_border) {
            fprintf(stderr, "FAIL: modern profile %s top border (0x%lx) != bottom border (0x%lx)\n",
                    profile, top_border, bot_border);
            return 1;
        }
        if (expect_active_border) {
            unsigned long exp_b = parse_pixel(dpy, expect_active_border);
            if (top_border != exp_b) {
                fprintf(stderr,
                        "FAIL: modern profile %s active border 0x%lx != expected %s (0x%lx)\n",
                        profile, top_border, expect_active_border, exp_b);
                return 1;
            }
        }
    }

    /* Verify rendered foreground pixels in system button, title text, and min/max buttons */
    {
        int sys_fg = count_non_bg_pixels(
            img2, fg2.upper_border + 1, fg2.upper_border + 1,
            fg2.title_height - 2, fg2.title_height - 2, active_bg);
        int title_fg = count_non_bg_pixels(
            img2, fg2.upper_border + fg2.title_height + 2, fg2.upper_border + 2,
            (int)fg2.frame_w - 2 * fg2.upper_border - 3 * fg2.title_height - 4,
            fg2.title_height - 4, active_bg);
        int minmax_fg = count_non_bg_pixels(
            img2, (int)fg2.frame_w - fg2.upper_border - 2 * fg2.title_height + 1,
            fg2.upper_border + 1,
            2 * fg2.title_height - 2, fg2.title_height - 2, active_bg);

        if (sys_fg <= 0 || title_fg <= 0 || minmax_fg <= 0) {
            fprintf(stderr,
                    "FAIL: missing rendered foreground pixels in profile %s "
                    "(sys_fg=%d title_fg=%d minmax_fg=%d)\n",
                    profile, sys_fg, title_fg, minmax_fg);
            return 1;
        }
    }

    printf("METRICS profile=%s resize_border=%d upper_border=%d title_height=%d "
           "base_y=%d active_bg=0x%lx inactive_bg=0x%lx top_border=0x%lx bot_border=0x%lx\n",
           profile, fg2.base_x, fg2.upper_border, fg2.title_height, fg2.base_y,
           active_bg, inactive_bg, top_border, bot_border);

    XDestroyImage(img1);
    XDestroyImage(img2);
    XDestroyWindow(dpy, win2);
    XDestroyWindow(dpy, win1);
    XCloseDisplay(dpy);
    return 0;
}
