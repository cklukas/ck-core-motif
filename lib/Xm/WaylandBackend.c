/*
 * Native Wayland runtime backend for libXm: display seam, Wayland registry,
 * xdg_toplevel / xdg_popup lifecycle, wl_shm buffers, xkbcommon keymaps,
 * fractional scaling, and session IPC synchronization.
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Wayland.h"

#ifdef HAVE_WAYLAND_BACKEND

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "WaylandBackendI.h"

#include <X11/IntrinsicP.h>
#include <Xm/Display.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <math.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

static XmWlServer *g_wl_srv = NULL;
static pid_t g_wl_owner_pid = 0;
static pthread_mutex_t g_wl_init_lock = PTHREAD_MUTEX_INITIALIZER;

uint32_t
_XmWlNowMs(void)
{
    struct timespec ts = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint32_t ms = (uint32_t)((uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000L));
    return ms ? ms : 1u;
}

static uint64_t
now_mono_ms64(void)
{
    struct timespec ts = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000L);
}

XmWlServer *
_XmWaylandGetServer(void)
{
    return (g_wl_srv && g_wl_owner_pid == getpid()) ? g_wl_srv : NULL;
}

void
_XmWlWakeServer(XmWlServer *srv)
{
    if (!srv || srv->wake_pipe[1] < 0) return;
    uint8_t b = 1;
    ssize_t r = write(srv->wake_pipe[1], &b, 1);
    (void)r;
}

void
_XmWaylandNotifyMainThread(XmWlServer *srv)
{
    if (!srv || srv->xt_pipe[1] < 0) return;
    uint8_t b = 1;
    ssize_t r = write(srv->xt_pipe[1], &b, 1);
    (void)r;
}

static int
create_shm_fd(size_t size)
{
    int fd = -1;
#ifdef MFD_CLOEXEC
    fd = memfd_create("ckmotif-wl-shm", MFD_CLOEXEC);
#endif
    if (fd < 0) {
        const char *rt = getenv("XDG_RUNTIME_DIR");
        char tmpl[512];
        snprintf(tmpl, sizeof(tmpl), "%s/ckmotif-shm-XXXXXX", (rt && rt[0]) ? rt : "/dev/shm");
        fd = mkostemp(tmpl, O_CLOEXEC);
        if (fd >= 0) unlink(tmpl);
    }
    if (fd < 0) return -1;
    if (ftruncate(fd, (off_t)size) != 0) {
        close(fd);
        return -1;
    }
    return fd;
}

static void
wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
    (void)wl_buffer;
    XmWlShmBuf *sb = (XmWlShmBuf *)data;
    if (sb) sb->busy = false;
}

static const struct wl_buffer_listener g_wl_buffer_listener = {
    .release = wl_buffer_release,
};

static void
free_shm_buf(XmWlShmBuf *sb)
{
    if (!sb) return;
    if (sb->pix_img) {
        pixman_image_unref(sb->pix_img);
        sb->pix_img = NULL;
    }
    if (sb->wl_buf) {
        wl_buffer_destroy(sb->wl_buf);
        sb->wl_buf = NULL;
    }
    if (sb->pixels && sb->byte_size > 0) {
        munmap(sb->pixels, sb->byte_size);
        sb->pixels = NULL;
    }
    if (sb->fd >= 0) {
        close(sb->fd);
        sb->fd = -1;
    }
    memset(sb, 0, sizeof(*sb));
    sb->fd = -1;
}

static bool
ensure_shm_buf(XmWlServer *srv, XmWlShmBuf *sb, int w, int h, bool has_alpha)
{
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    if (sb->wl_buf && sb->width == w && sb->height == h)
        return true;

    free_shm_buf(sb);
    if (!srv->shm) return false;

    int stride = w * 4;
    size_t size = (size_t)stride * (size_t)h;
    int fd = create_shm_fd(size);
    if (fd < 0) return false;

    void *map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        return false;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(srv->shm, fd, (int32_t)size);
    if (!pool) {
        munmap(map, size);
        close(fd);
        return false;
    }
    uint32_t fmt = has_alpha ? WL_SHM_FORMAT_ARGB8888 : WL_SHM_FORMAT_XRGB8888;
    struct wl_buffer *buf = wl_shm_pool_create_buffer(pool, 0, w, h, stride, fmt);
    wl_shm_pool_destroy(pool);
    if (!buf) {
        munmap(map, size);
        close(fd);
        return false;
    }

    sb->wl_buf = buf;
    sb->pixels = (uint32_t *)map;
    sb->byte_size = size;
    sb->width = w;
    sb->height = h;
    sb->stride = stride;
    sb->fd = fd;
    sb->busy = false;
    sb->pix_img = pixman_image_create_bits(
        has_alpha ? PIXMAN_a8r8g8b8 : PIXMAN_x8r8g8b8,
        w, h, sb->pixels, stride);
    wl_buffer_add_listener(buf, &g_wl_buffer_listener, sb);
    return true;
}

void
_XmWlEnsureTopCanvas(XmWlServer *srv, XmWlWindow *top)
{
    (void)srv;
    if (!top) return;
    int w = top->width > 0 ? top->width : 1;
    int h = top->height > 0 ? top->height : 1;
    if (top->canvas && top->canvas_w == w && top->canvas_h == h)
        return;

    int new_stride = w * 4;
    uint32_t *new_bits = (uint32_t *)calloc((size_t)w * (size_t)h, sizeof(uint32_t));
    if (!new_bits) return;
    pixman_image_t *new_img = pixman_image_create_bits(PIXMAN_a8r8g8b8, w, h, new_bits, new_stride);
    if (!new_img) {
        free(new_bits);
        return;
    }
    uint32_t fill = top->has_bg_pixel ? (0xff000000u | (top->bg_pixel & 0x00ffffffu)) :
                    (top->depth == 32 ? 0x00000000u : 0xffd8d8d8u);
    for (int i = 0; i < w * h; ++i)
        new_bits[i] = fill;

    if (top->canvas && top->canvas_bits) {
        int cw = w < top->canvas_w ? w : top->canvas_w;
        int ch = h < top->canvas_h ? h : top->canvas_h;
        for (int row = 0; row < ch; ++row) {
            memcpy(new_bits + (size_t)row * (size_t)w,
                   top->canvas_bits + (size_t)row * (size_t)top->canvas_w,
                   (size_t)cw * 4u);
        }
        pixman_image_unref(top->canvas);
        free(top->canvas_bits);
    }
    top->canvas = new_img;
    top->canvas_bits = new_bits;
    top->canvas_w = w;
    top->canvas_h = h;
    top->canvas_stride = new_stride;
    _XmWlDamageTop(top, 0, 0, w, h);
}

void
_XmWlDamageTop(XmWlWindow *top, int x, int y, int w, int h)
{
    if (!top || w <= 0 || h <= 0) return;
    int x1 = x < 0 ? 0 : x;
    int y1 = y < 0 ? 0 : y;
    int x2 = x + w;
    int y2 = y + h;
    if (top->canvas_w > 0 && x2 > top->canvas_w) x2 = top->canvas_w;
    if (top->canvas_h > 0 && y2 > top->canvas_h) y2 = top->canvas_h;
    if (x2 <= x1 || y2 <= y1) return;
    if (!top->dirty) {
        top->dmg_x1 = x1;
        top->dmg_y1 = y1;
        top->dmg_x2 = x2;
        top->dmg_y2 = y2;
        top->dirty = true;
    } else {
        if (x1 < top->dmg_x1) top->dmg_x1 = x1;
        if (y1 < top->dmg_y1) top->dmg_y1 = y1;
        if (x2 > top->dmg_x2) top->dmg_x2 = x2;
        if (y2 > top->dmg_y2) top->dmg_y2 = y2;
    }
}

/* ---------- Wayland Listeners ---------- */

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *wm_base, uint32_t serial)
{
    (void)data;
    xdg_wm_base_pong(wm_base, serial);
}

static const struct xdg_wm_base_listener g_xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static void
xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    XmWlWindow *win = (XmWlWindow *)data;
    XmWlServer *srv = g_wl_srv;
    if (!win || !srv) {
        xdg_surface_ack_configure(xdg_surface, serial);
        return;
    }
    win->pending_configure_serial = serial;
    win->xdg_configured = true;

    int cw = win->pending_cfg_w > 0 ? win->pending_cfg_w : win->width;
    int ch = win->pending_cfg_h > 0 ? win->pending_cfg_h : win->height;
    win->pending_cfg_w = 0;
    win->pending_cfg_h = 0;
    if (cw < 1) cw = 1;
    if (ch < 1) ch = 1;

    bool size_changed = (cw != (int)win->width || ch != (int)win->height);
    if (size_changed) {
        win->width = (uint16_t)cw;
        win->height = (uint16_t)ch;
        _XmWlEnsureTopCanvas(srv, win);
        _XmWlDeliverConfigureAndExpose(srv, win, cw, ch);
    } else {
        _XmWlEnsureTopCanvas(srv, win);
        _XmWlDamageTop(win, 0, 0, win->width, win->height);
    }
}

static const struct xdg_surface_listener g_xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static void
xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel,
                       int32_t width, int32_t height, struct wl_array *states)
{
    (void)xdg_toplevel;
    (void)states;
    XmWlWindow *win = (XmWlWindow *)data;
    if (!win) return;
    if (width > 0 && height > 0) {
        win->pending_cfg_w = width;
        win->pending_cfg_h = height;
    }
}

static void
xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
    (void)xdg_toplevel;
    XmWlWindow *win = (XmWlWindow *)data;
    if (!win || !g_wl_srv) return;
    _XmWlDeliverCloseRequest(g_wl_srv, win);
}

static void
xdg_toplevel_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel,
                              int32_t width, int32_t height)
{
    (void)data;
    (void)xdg_toplevel;
    (void)width;
    (void)height;
}

static void
xdg_toplevel_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel,
                             struct wl_array *capabilities)
{
    (void)data;
    (void)xdg_toplevel;
    (void)capabilities;
}

static const struct xdg_toplevel_listener g_xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
    .configure_bounds = xdg_toplevel_configure_bounds,
    .wm_capabilities = xdg_toplevel_wm_capabilities,
};

static void
xdg_deco_configure(void *data, struct zxdg_toplevel_decoration_v1 *deco, uint32_t mode)
{
    (void)data;
    (void)deco;
    (void)mode;
}

static const struct zxdg_toplevel_decoration_v1_listener g_xdg_deco_listener = {
    .configure = xdg_deco_configure,
};

static void
xdg_popup_configure(void *data, struct xdg_popup *xdg_popup,
                    int32_t x, int32_t y, int32_t width, int32_t height)
{
    (void)xdg_popup;
    (void)x;
    (void)y;
    XmWlWindow *win = (XmWlWindow *)data;
    if (!win) return;
    if (width > 0 && height > 0) {
        win->pending_cfg_w = width;
        win->pending_cfg_h = height;
    }
}

static void
xdg_popup_done(void *data, struct xdg_popup *xdg_popup)
{
    (void)xdg_popup;
    XmWlWindow *win = (XmWlWindow *)data;
    XmWlServer *srv = g_wl_srv;
    if (!win || !srv) return;
    /* Synthesize an outside click to pop down active Motif menu shells cleanly */
    if (srv->ptr_grab_active) {
        _XmWlDispatchPointerButton(srv, 1, true, _XmWlNowMs());
        _XmWlDispatchPointerButton(srv, 1, false, _XmWlNowMs());
    }
}

static void
xdg_popup_repositioned(void *data, struct xdg_popup *xdg_popup, uint32_t token)
{
    (void)data;
    (void)xdg_popup;
    (void)token;
}

static const struct xdg_popup_listener g_xdg_popup_listener = {
    .configure = xdg_popup_configure,
    .popup_done = xdg_popup_done,
    .repositioned = xdg_popup_repositioned,
};

static void
frac_scale_preferred(void *data, struct wp_fractional_scale_v1 *frac, uint32_t scale_120)
{
    (void)frac;
    XmWlWindow *win = (XmWlWindow *)data;
    XmWlServer *srv = g_wl_srv;
    if (!win || !srv || scale_120 == 0) return;
    double sc = (double)scale_120 / 120.0;
    if (sc < 0.5) sc = 1.0;
    if (sc > 4.0) sc = 4.0;
    win->preferred_scale = sc;
    _XmWlDamageTop(win, 0, 0, win->width, win->height);
    if (fabs(sc - srv->active_scale) > 0.01) {
        _XmWlApplyScaleUpdate(srv, sc);
    }
}

static const struct wp_fractional_scale_v1_listener g_frac_scale_listener = {
    .preferred_scale = frac_scale_preferred,
};

/* ---------- Pointer & Keyboard Listeners ---------- */

static XmWlWindow *
find_top_by_wl_surface(XmWlServer *srv, struct wl_surface *surf)
{
    if (!srv || !surf || !srv->root) return NULL;
    for (XmWlWindow *w = srv->root->first_child; w; w = w->next_sibling) {
        if (w->wl_surf == surf) return w;
    }
    return NULL;
}

static void
pointer_enter(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
              struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy)
{
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    XmWlWindow *top = find_top_by_wl_surface(srv, surface);
    srv->pointer_top = top;

    /* Set default left_ptr cursor so pointer is visible on Wayland */
    if (srv->cursor_theme && srv->cursor_surface) {
        struct wl_cursor *cur = wl_cursor_theme_get_cursor(srv->cursor_theme, "left_ptr");
        if (!cur) cur = wl_cursor_theme_get_cursor(srv->cursor_theme, "default");
        if (cur && cur->image_count > 0) {
            struct wl_cursor_image *img = cur->images[0];
            struct wl_buffer *cbuf = wl_cursor_image_get_buffer(img);
            if (cbuf) {
                wl_pointer_set_cursor(wl_pointer, serial, srv->cursor_surface,
                                      (int32_t)img->hotspot_x, (int32_t)img->hotspot_y);
                wl_surface_attach(srv->cursor_surface, cbuf, 0, 0);
                wl_surface_damage(srv->cursor_surface, 0, 0, (int32_t)img->width, (int32_t)img->height);
                wl_surface_commit(srv->cursor_surface);
            }
        }
    }

    if (top) {
        int16_t ix = (int16_t)wl_fixed_to_int(sx);
        int16_t iy = (int16_t)wl_fixed_to_int(sy);
        _XmWlDispatchPointerMotion(srv, top, ix, iy, _XmWlNowMs());
    }
}

static void
pointer_leave(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
              struct wl_surface *surface)
{
    (void)wl_pointer;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    XmWlWindow *top = find_top_by_wl_surface(srv, surface);
    if (top && srv->pointer_top == top) {
        _XmWlDispatchPointerLeaveTop(srv, top, _XmWlNowMs());
        srv->pointer_top = NULL;
    }
}

static void
pointer_motion(void *data, struct wl_pointer *wl_pointer, uint32_t time,
               wl_fixed_t sx, wl_fixed_t sy)
{
    (void)wl_pointer;
    XmWlServer *srv = (XmWlServer *)data;
    srv->server_time_ms = time ? time : _XmWlNowMs();
    if (srv->pointer_top) {
        int16_t ix = (int16_t)wl_fixed_to_int(sx);
        int16_t iy = (int16_t)wl_fixed_to_int(sy);
        _XmWlDispatchPointerMotion(srv, srv->pointer_top, ix, iy, srv->server_time_ms);
    }
}

static void
pointer_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
               uint32_t time, uint32_t button, uint32_t state)
{
    (void)wl_pointer;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    srv->last_button_serial = serial;
    srv->server_time_ms = time ? time : _XmWlNowMs();

    uint8_t xbtn = 1;
    if (button == BTN_LEFT) xbtn = 1;
    else if (button == BTN_MIDDLE) xbtn = 2;
    else if (button == BTN_RIGHT) xbtn = 3;
    else if (button == BTN_SIDE) xbtn = 8;
    else if (button == BTN_EXTRA) xbtn = 9;
    else return;

    _XmWlDispatchPointerButton(srv, xbtn, state == WL_POINTER_BUTTON_STATE_PRESSED, srv->server_time_ms);
}

static void
pointer_axis(void *data, struct wl_pointer *wl_pointer, uint32_t time,
             uint32_t axis, wl_fixed_t value)
{
    (void)wl_pointer;
    XmWlServer *srv = (XmWlServer *)data;
    double v = wl_fixed_to_double(value);
    if (fabs(v) < 0.01) return;
    uint8_t xbtn = 0;
    if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
        xbtn = (v < 0.0) ? 4 : 5;
    else if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL)
        xbtn = (v < 0.0) ? 6 : 7;
    if (xbtn) {
        uint32_t t = time ? time : _XmWlNowMs();
        _XmWlDispatchPointerButton(srv, xbtn, true, t);
        _XmWlDispatchPointerButton(srv, xbtn, false, t);
    }
}

static void pointer_frame(void *data, struct wl_pointer *p) { (void)data; (void)p; }
static void pointer_axis_source(void *data, struct wl_pointer *p, uint32_t s) { (void)data; (void)p; (void)s; }
static void pointer_axis_stop(void *data, struct wl_pointer *p, uint32_t t, uint32_t a) { (void)data; (void)p; (void)t; (void)a; }
static void pointer_axis_discrete(void *data, struct wl_pointer *p, uint32_t a, int32_t d) { (void)data; (void)p; (void)a; (void)d; }
static void pointer_axis_value120(void *data, struct wl_pointer *p, uint32_t a, int32_t v) { (void)data; (void)p; (void)a; (void)v; }
static void pointer_axis_rel_dir(void *data, struct wl_pointer *p, uint32_t a, uint32_t d) { (void)data; (void)p; (void)a; (void)d; }

static const struct wl_pointer_listener g_pointer_listener = {
    .enter = pointer_enter,
    .leave = pointer_leave,
    .motion = pointer_motion,
    .button = pointer_button,
    .axis = pointer_axis,
    .frame = pointer_frame,
    .axis_source = pointer_axis_source,
    .axis_stop = pointer_axis_stop,
    .axis_discrete = pointer_axis_discrete,
    .axis_value120 = pointer_axis_value120,
    .axis_relative_direction = pointer_axis_rel_dir,
};

static void
keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format,
                int32_t fd, uint32_t size)
{
    (void)wl_keyboard;
    XmWlServer *srv = (XmWlServer *)data;
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 || fd < 0 || size == 0) {
        if (fd >= 0) close(fd);
        return;
    }
    char *map_str = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (map_str == MAP_FAILED) return;

    if (!srv->xkb_ctx)
        srv->xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (srv->xkb_ctx) {
        struct xkb_keymap *km = xkb_keymap_new_from_string(
            srv->xkb_ctx, map_str, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
        if (km) {
            if (srv->xkb_state) xkb_state_unref(srv->xkb_state);
            if (srv->xkb_keymap) xkb_keymap_unref(srv->xkb_keymap);
            srv->xkb_keymap = km;
            srv->xkb_state = xkb_state_new(km);
            _XmWlRebuildKeymapFromXkb(srv);
        }
    }
    munmap(map_str, size);
}

static void
keyboard_enter(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
               struct wl_surface *surface, struct wl_array *keys)
{
    (void)wl_keyboard;
    (void)keys;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    XmWlWindow *top = find_top_by_wl_surface(srv, surface);
    srv->wl_focus_top = top;
    srv->held_keycode = 0;
    if (top) {
        _XmWlDispatchFocusTop(srv, top, true);
        _XmWlImeSyncFocus(srv);
    }
}

static void
keyboard_leave(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
               struct wl_surface *surface)
{
    (void)wl_keyboard;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    srv->held_keycode = 0;
    XmWlWindow *top = find_top_by_wl_surface(srv, surface);
    if (top && srv->wl_focus_top == top) {
        _XmWlDispatchFocusTop(srv, top, false);
        srv->wl_focus_top = NULL;
        _XmWlImeSyncFocus(srv);
    }
}

static void
keyboard_key(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
             uint32_t time, uint32_t key, uint32_t state)
{
    (void)wl_keyboard;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    srv->server_time_ms = time ? time : _XmWlNowMs();
    uint32_t xkc = key + 8u;
    if (xkc > 255u) return;

    bool pressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
    _XmWlDispatchKey(srv, (uint8_t)xkc, pressed, srv->server_time_ms);

    if (pressed && srv->repeat_rate > 0 && srv->repeat_delay > 0 &&
        (!srv->xkb_keymap || xkb_keymap_key_repeats(srv->xkb_keymap, xkc))) {
        srv->held_keycode = xkc;
        srv->next_repeat_ms = now_mono_ms64() + (uint64_t)srv->repeat_delay;
    } else if (!pressed && srv->held_keycode == xkc) {
        srv->held_keycode = 0;
    }
}

static void
keyboard_modifiers(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
                   uint32_t mods_depressed, uint32_t mods_latched,
                   uint32_t mods_locked, uint32_t group)
{
    (void)wl_keyboard;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    if (srv->xkb_state) {
        xkb_state_update_mask(srv->xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
        uint16_t m = 0;
        if (xkb_state_mod_name_is_active(srv->xkb_state, XKB_MOD_NAME_SHIFT, XKB_STATE_MODS_EFFECTIVE) > 0)
            m |= (1u << 0);
        if (xkb_state_mod_name_is_active(srv->xkb_state, XKB_MOD_NAME_CAPS, XKB_STATE_MODS_EFFECTIVE) > 0)
            m |= (1u << 1);
        if (xkb_state_mod_name_is_active(srv->xkb_state, XKB_MOD_NAME_CTRL, XKB_STATE_MODS_EFFECTIVE) > 0)
            m |= (1u << 2);
        if (xkb_state_mod_name_is_active(srv->xkb_state, XKB_MOD_NAME_ALT, XKB_STATE_MODS_EFFECTIVE) > 0)
            m |= (1u << 3);
        if (xkb_state_mod_name_is_active(srv->xkb_state, XKB_MOD_NAME_NUM, XKB_STATE_MODS_EFFECTIVE) > 0)
            m |= (1u << 4);
        if (xkb_state_mod_name_is_active(srv->xkb_state, XKB_MOD_NAME_LOGO, XKB_STATE_MODS_EFFECTIVE) > 0)
            m |= (1u << 6);
        srv->mod_mask = m;
    } else {
        srv->mod_mask = (uint16_t)(mods_depressed & 0xffu);
    }
}

static void
keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard,
                     int32_t rate, int32_t delay)
{
    (void)wl_keyboard;
    XmWlServer *srv = (XmWlServer *)data;
    srv->repeat_rate = rate;
    srv->repeat_delay = delay;
}

static const struct wl_keyboard_listener g_keyboard_listener = {
    .keymap = keyboard_keymap,
    .enter = keyboard_enter,
    .leave = keyboard_leave,
    .key = keyboard_key,
    .modifiers = keyboard_modifiers,
    .repeat_info = keyboard_repeat_info,
};

static void
seat_capabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
    XmWlServer *srv = (XmWlServer *)data;
    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !srv->pointer) {
        srv->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(srv->pointer, &g_pointer_listener, srv);
    } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && srv->pointer) {
        wl_pointer_destroy(srv->pointer);
        srv->pointer = NULL;
    }
    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !srv->keyboard) {
        srv->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(srv->keyboard, &g_keyboard_listener, srv);
    } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && srv->keyboard) {
        wl_keyboard_destroy(srv->keyboard);
        srv->keyboard = NULL;
    }
    _XmWlDataBindSeat(srv);
    _XmWlImeBindSeat(srv);
}

static void seat_name(void *data, struct wl_seat *seat, const char *name)
{
    (void)data; (void)seat; (void)name;
}

static const struct wl_seat_listener g_seat_listener = {
    .capabilities = seat_capabilities,
    .name = seat_name,
};

/* ---------- Output Listener ---------- */

static void
recompute_screen_metrics(XmWlServer *srv)
{
    int max_w = 0, max_h = 0, mm_w = 0, mm_h = 0, max_scale = 1;
    for (int i = 0; i < srv->output_count; ++i) {
        const XmWlOutputInfo *o = &srv->outputs[i];
        int sc = o->scale > 0 ? o->scale : 1;
        int lw = o->width > 0 ? (o->width / sc) : 1280;
        int lh = o->height > 0 ? (o->height / sc) : 800;
        if (o->x + lw > max_w) { max_w = o->x + lw; mm_w = o->phys_w_mm; }
        if (o->y + lh > max_h) { max_h = o->y + lh; mm_h = o->phys_h_mm; }
        if (sc > max_scale) max_scale = sc;
    }
    if (max_w < 320) max_w = 1280;
    if (max_h < 240) max_h = 800;
    srv->screen_w = max_w;
    srv->screen_h = max_h;
    srv->screen_mm_w = mm_w > 0 ? mm_w : (int)((double)max_w * 25.4 / (96.0 * srv->active_scale));
    srv->screen_mm_h = mm_h > 0 ? mm_h : (int)((double)max_h * 25.4 / (96.0 * srv->active_scale));
    if (srv->root) {
        srv->root->width = (uint16_t)srv->screen_w;
        srv->root->height = (uint16_t)srv->screen_h;
    }
}

static void
output_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y,
                int32_t physical_width, int32_t physical_height, int32_t subpixel,
                const char *make, const char *model, int32_t transform)
{
    (void)wl_output; (void)subpixel; (void)make; (void)model; (void)transform;
    XmWlOutputInfo *o = (XmWlOutputInfo *)data;
    o->x = x;
    o->y = y;
    o->phys_w_mm = physical_width;
    o->phys_h_mm = physical_height;
}

static void
output_mode(void *data, struct wl_output *wl_output, uint32_t flags,
            int32_t width, int32_t height, int32_t refresh)
{
    (void)wl_output; (void)refresh;
    XmWlOutputInfo *o = (XmWlOutputInfo *)data;
    if ((flags & WL_OUTPUT_MODE_CURRENT) || o->width == 0) {
        o->width = width;
        o->height = height;
    }
}

static void
output_done(void *data, struct wl_output *wl_output)
{
    (void)wl_output;
    XmWlOutputInfo *o = (XmWlOutputInfo *)data;
    o->done = true;
    if (g_wl_srv) {
        recompute_screen_metrics(g_wl_srv);
        if (o->scale > 0 && !g_wl_srv->frac_scale_mgr &&
            fabs((double)o->scale - g_wl_srv->active_scale) > 0.01) {
            _XmWlApplyScaleUpdate(g_wl_srv, (double)o->scale);
        } else {
            _XmWlBroadcastRandRChange(g_wl_srv);
        }
    }
}

static void
output_scale(void *data, struct wl_output *wl_output, int32_t factor)
{
    (void)wl_output;
    XmWlOutputInfo *o = (XmWlOutputInfo *)data;
    o->scale = factor > 0 ? factor : 1;
}

static void output_name(void *d, struct wl_output *o, const char *n) { (void)d; (void)o; (void)n; }
static void output_desc(void *d, struct wl_output *o, const char *s) { (void)d; (void)o; (void)s; }

static const struct wl_output_listener g_output_listener = {
    .geometry = output_geometry,
    .mode = output_mode,
    .done = output_done,
    .scale = output_scale,
    .name = output_name,
    .description = output_desc,
};

static void
registry_global(void *data, struct wl_registry *registry, uint32_t name,
                const char *interface, uint32_t version)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (!strcmp(interface, wl_compositor_interface.name)) {
        uint32_t v = version < 4 ? version : 4;
        srv->compositor = (struct wl_compositor *)wl_registry_bind(registry, name, &wl_compositor_interface, v);
    } else if (!strcmp(interface, wl_subcompositor_interface.name)) {
        srv->subcompositor = (struct wl_subcompositor *)wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    } else if (!strcmp(interface, wl_shm_interface.name)) {
        srv->shm = (struct wl_shm *)wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (!strcmp(interface, xdg_wm_base_interface.name)) {
        uint32_t v = version < 5 ? version : 5;
        srv->xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(registry, name, &xdg_wm_base_interface, v);
        xdg_wm_base_add_listener(srv->xdg_wm_base, &g_xdg_wm_base_listener, srv);
    } else if (!strcmp(interface, zxdg_decoration_manager_v1_interface.name)) {
        srv->deco_mgr = (struct zxdg_decoration_manager_v1 *)wl_registry_bind(
            registry, name, &zxdg_decoration_manager_v1_interface, 1);
    } else if (!strcmp(interface, xdg_toplevel_icon_manager_v1_interface.name)) {
        srv->icon_mgr = (struct xdg_toplevel_icon_manager_v1 *)wl_registry_bind(
            registry, name, &xdg_toplevel_icon_manager_v1_interface, 1);
    } else if (!strcmp(interface, wl_seat_interface.name)) {
        if (!srv->seat) {
            uint32_t v = version < 7 ? version : 7;
            srv->seat = (struct wl_seat *)wl_registry_bind(registry, name, &wl_seat_interface, v);
            wl_seat_add_listener(srv->seat, &g_seat_listener, srv);
            _XmWlDataBindSeat(srv);
            _XmWlImeBindSeat(srv);
        }
    } else if (!strcmp(interface, wl_data_device_manager_interface.name)) {
        uint32_t v = version < 3 ? version : 3;
        srv->data_mgr = (struct wl_data_device_manager *)wl_registry_bind(
            registry, name, &wl_data_device_manager_interface, v);
        _XmWlDataBindSeat(srv);
    } else if (!strcmp(interface, zwp_primary_selection_device_manager_v1_interface.name)) {
        srv->prim_mgr = (struct zwp_primary_selection_device_manager_v1 *)wl_registry_bind(
            registry, name, &zwp_primary_selection_device_manager_v1_interface, 1);
        _XmWlDataBindSeat(srv);
    } else if (!strcmp(interface, zwp_text_input_manager_v3_interface.name)) {
        srv->text_input_mgr = (struct zwp_text_input_manager_v3 *)wl_registry_bind(
            registry, name, &zwp_text_input_manager_v3_interface, 1);
        _XmWlImeBindSeat(srv);
    } else if (!strcmp(interface, wp_fractional_scale_manager_v1_interface.name)) {
        srv->frac_scale_mgr = (struct wp_fractional_scale_manager_v1 *)wl_registry_bind(
            registry, name, &wp_fractional_scale_manager_v1_interface, 1);
    } else if (!strcmp(interface, wp_viewporter_interface.name)) {
        srv->viewporter = (struct wp_viewporter *)wl_registry_bind(
            registry, name, &wp_viewporter_interface, 1);
    } else if (!strcmp(interface, wl_output_interface.name)) {
        if (srv->output_count < XM_WL_MAX_OUTPUTS) {
            uint32_t v = version < 4 ? version : 4;
            XmWlOutputInfo *o = &srv->outputs[srv->output_count++];
            memset(o, 0, sizeof(*o));
            o->global_name = name;
            o->scale = 1;
            o->wl_out = (struct wl_output *)wl_registry_bind(registry, name, &wl_output_interface, v);
            wl_output_add_listener(o->wl_out, &g_output_listener, o);
        }
    }
}

static void
registry_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
    (void)data; (void)registry; (void)name;
}

static const struct wl_registry_listener g_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

/* ---------- Top-Level & Popup Role Synchronization ---------- */

static void
pop_popup_stack_from(XmWlServer *srv, int idx)
{
    for (int i = srv->popup_depth - 1; i >= idx; --i) {
        XmWlWindow *pw = srv->popup_stack[i];
        srv->popup_stack[i] = NULL;
        if (!pw) continue;
        if (pw->xdg_pop) {
            xdg_popup_destroy(pw->xdg_pop);
            pw->xdg_pop = NULL;
        }
        if (pw->xdg_surf) {
            xdg_surface_destroy(pw->xdg_surf);
            pw->xdg_surf = NULL;
        }
        if (pw->frac_scale_obj) {
            wp_fractional_scale_v1_destroy(pw->frac_scale_obj);
            pw->frac_scale_obj = NULL;
        }
        if (pw->viewport) {
            wp_viewport_destroy(pw->viewport);
            pw->viewport = NULL;
        }
        if (pw->wl_surf) {
            wl_surface_destroy(pw->wl_surf);
            pw->wl_surf = NULL;
        }
        pw->xdg_configured = false;
        pw->is_popup = false;
    }
    if (idx < srv->popup_depth)
        srv->popup_depth = idx;
}

static void
sync_toplevel_metadata_all(XmWlServer *srv, XmWlWindow *win)
{
    if (!win || !win->xdg_top) return;

    uint32_t a_net_name = _XmWlInternAtom(srv, "_NET_WM_NAME", 12, true);
    uint32_t a_wm_name = 39; /* XA_WM_NAME */
    XmWlProperty *p = a_net_name ? _XmWlGetProperty(win, a_net_name) : NULL;
    if (!p || !p->data || p->byte_len == 0)
        p = _XmWlGetProperty(win, a_wm_name);
    if (p && p->data && p->byte_len > 0) {
        char title[256];
        size_t n = p->byte_len < sizeof(title) - 1 ? p->byte_len : sizeof(title) - 1;
        memcpy(title, p->data, n);
        title[n] = '\0';
        xdg_toplevel_set_title(win->xdg_top, title);
    }

    uint32_t a_wm_class = 67; /* XA_WM_CLASS */
    p = _XmWlGetProperty(win, a_wm_class);
    if (p && p->data && p->byte_len > 0) {
        char app_id[128];
        size_t n = strnlen((const char *)p->data, p->byte_len);
        if (n >= sizeof(app_id)) n = sizeof(app_id) - 1;
        memcpy(app_id, p->data, n);
        app_id[n] = '\0';
        if (app_id[0]) xdg_toplevel_set_app_id(win->xdg_top, app_id);
    }

    uint32_t a_transient = 68; /* XA_WM_TRANSIENT_FOR */
    p = _XmWlGetProperty(win, a_transient);
    if (p && p->data && p->byte_len >= 4) {
        uint32_t parent_xid = 0;
        memcpy(&parent_xid, p->data, 4);
        XmWlWindow *ptop = _XmWlFindWindow(srv, parent_xid);
        if (ptop && ptop->xdg_top)
            xdg_toplevel_set_parent(win->xdg_top, ptop->xdg_top);
    }

    uint32_t a_normal = 40; /* XA_WM_NORMAL_HINTS */
    p = _XmWlGetProperty(win, a_normal);
    if (p && p->data && p->byte_len >= 9 * 4) {
        uint32_t flags = 0;
        int32_t min_w = 0, min_h = 0, max_w = 0, max_h = 0;
        memcpy(&flags, p->data + 0, 4);
        memcpy(&min_w, p->data + 5 * 4, 4);
        memcpy(&min_h, p->data + 6 * 4, 4);
        memcpy(&max_w, p->data + 7 * 4, 4);
        memcpy(&max_h, p->data + 8 * 4, 4);
        if ((flags & (1u << 4)) && min_w > 0 && min_h > 0)
            xdg_toplevel_set_min_size(win->xdg_top, min_w, min_h);
        if ((flags & (1u << 5)) && max_w > 0 && max_h > 0)
            xdg_toplevel_set_max_size(win->xdg_top, max_w, max_h);
    }

    uint32_t a_mwm = _XmWlInternAtom(srv, "_MOTIF_WM_HINTS", 15, true);
    p = a_mwm ? _XmWlGetProperty(win, a_mwm) : NULL;
    bool want_deco = true;
    if (p && p->data && p->byte_len >= 12) {
        uint32_t mflags = 0, mdeco = 1;
        memcpy(&mflags, p->data + 0, 4);
        memcpy(&mdeco, p->data + 8, 4);
        if ((mflags & 2u) && mdeco == 0)
            want_deco = false;
    }
    win->wants_decorations = want_deco;
    if (win->xdg_deco) {
        zxdg_toplevel_decoration_v1_set_mode(
            win->xdg_deco,
            want_deco ? ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE
                      : ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
    }

    /* Dynamic window icon via xdg_toplevel_icon_v1 */
    if (srv->icon_mgr && srv->shm) {
        uint32_t a_wm_hints = 35; /* XA_WM_HINTS */
        XmWlProperty *hp = _XmWlGetProperty(win, a_wm_hints);
        if (hp && hp->data && hp->byte_len >= 24) {
            uint32_t hflags = 0, icon_pix = 0, icon_mask = 0;
            memcpy(&hflags, hp->data + 0, 4);
            memcpy(&icon_pix, hp->data + 8, 4);
            memcpy(&icon_mask, hp->data + 20, 4);
            if ((hflags & 4u) && icon_pix != 0) {
                uint32_t *argb = NULL;
                int iw = 0, ih = 0;
                if (_XmWlBuildIconPixels(srv, icon_pix, (hflags & 32u) ? icon_mask : 0, &argb, &iw, &ih)) {
                    size_t isz = (size_t)iw * (size_t)ih * 4u;
                    int ifd = create_shm_fd(isz);
                    if (ifd >= 0) {
                        void *imap = mmap(NULL, isz, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0);
                        if (imap != MAP_FAILED) {
                            memcpy(imap, argb, isz);
                            struct wl_shm_pool *ipool = wl_shm_create_pool(srv->shm, ifd, (int32_t)isz);
                            struct wl_buffer *ibuf = wl_shm_pool_create_buffer(
                                ipool, 0, iw, ih, iw * 4, WL_SHM_FORMAT_ARGB8888);
                            wl_shm_pool_destroy(ipool);
                            struct xdg_toplevel_icon_v1 *ico =
                                xdg_toplevel_icon_manager_v1_create_icon(srv->icon_mgr);
                            xdg_toplevel_icon_v1_add_buffer(ico, ibuf, 1);
                            xdg_toplevel_icon_manager_v1_set_icon(srv->icon_mgr, win->xdg_top, ico);
                            xdg_toplevel_icon_v1_destroy(ico);
                            wl_buffer_destroy(ibuf);
                            munmap(imap, isz);
                        }
                        close(ifd);
                    }
                    free(argb);
                }
            }
        }
    }
}

void
_XmWlUpdateWindowMetadata(XmWlServer *srv, XmWlWindow *win, uint32_t changed_atom)
{
    if (!srv || !win) return;
    if (win == srv->root) {
        const char *aname = _XmWlAtomName(srv, changed_atom);
        XmWlProperty *rp = _XmWlGetProperty(win, changed_atom);
        if (aname && rp && rp->format == 8) {
            if (!strcmp(aname, "_MOTIF_WAYLAND_TEST_SCALE") && rp->data && rp->byte_len > 0) {
                char buf[32];
                size_t n = rp->byte_len < sizeof(buf) - 1 ? rp->byte_len : sizeof(buf) - 1;
                memcpy(buf, rp->data, n);
                buf[n] = '\0';
                double sc = strtod(buf, NULL);
                if (sc >= 0.5 && sc <= 4.0)
                    _XmWlApplyScaleUpdate(srv, sc);
            } else if (!strncmp(aname, "RESOURCE_MANAGER", 16) ||
                       !strncmp(aname, "_DT_", 4) ||
                       !strncmp(aname, "_DTWM_", 6)) {
                _XmWlIpcPublishRootProp(srv, aname, rp->data, rp->byte_len);
            }
        }
        return;
    }
    if (win->parent == srv->root && win->xdg_top) {
        sync_toplevel_metadata_all(srv, win);
        if (win->wl_surf && win->xdg_configured)
            win->dirty = true;
    }
}

void
_XmWlSyncWindowRole(XmWlServer *srv, XmWlWindow *win)
{
    if (!srv || !win || win->parent != srv->root || !win->mapped || win->win_class != 1)
        return;
    if (win->wl_surf)
        return;
    if (!srv->compositor || !srv->xdg_wm_base)
        return;

    _XmWlEnsureTopCanvas(srv, win);
    win->preferred_scale = srv->active_scale > 0.0 ? srv->active_scale : 1.0;
    win->wl_surf = wl_compositor_create_surface(srv->compositor);
    if (!win->wl_surf) return;

    if (srv->frac_scale_mgr) {
        win->frac_scale_obj = wp_fractional_scale_manager_v1_get_fractional_scale(
            srv->frac_scale_mgr, win->wl_surf);
        wp_fractional_scale_v1_add_listener(win->frac_scale_obj, &g_frac_scale_listener, win);
    }
    if (srv->viewporter) {
        win->viewport = wp_viewporter_get_viewport(srv->viewporter, win->wl_surf);
    }

    /* If a Wayland drag is starting and this is an override-redirect depth-32 ARGB drag window */
    if (win->override_redirect && srv->sel_xdnd_owner != 0 && win->depth == 32) {
        win->is_drag_icon = true;
        win->xdg_configured = true;
        _XmWlDataAttachDragIcon(srv, win);
        _XmWlDamageTop(win, 0, 0, win->width, win->height);
        return;
    }

    if (!win->override_redirect) {
        win->is_popup = false;
        win->xdg_surf = xdg_wm_base_get_xdg_surface(srv->xdg_wm_base, win->wl_surf);
        xdg_surface_add_listener(win->xdg_surf, &g_xdg_surface_listener, win);
        win->xdg_top = xdg_surface_get_toplevel(win->xdg_surf);
        xdg_toplevel_add_listener(win->xdg_top, &g_xdg_toplevel_listener, win);
        if (srv->deco_mgr) {
            win->xdg_deco = zxdg_decoration_manager_v1_get_toplevel_decoration(
                srv->deco_mgr, win->xdg_top);
            zxdg_toplevel_decoration_v1_add_listener(win->xdg_deco, &g_xdg_deco_listener, win);
        }
        sync_toplevel_metadata_all(srv, win);
        wl_surface_commit(win->wl_surf);
        wl_display_roundtrip(srv->wl_dpy);
        _XmWlDamageTop(win, 0, 0, win->width, win->height);
    } else {
        /* Override-redirect menu / dropdown / tooltip -> xdg_popup */
        XmWlWindow *parent_win = NULL;
        if (srv->popup_depth > 0) {
            parent_win = srv->popup_stack[srv->popup_depth - 1];
        }
        if (!parent_win) parent_win = srv->pointer_top;
        if (!parent_win) parent_win = srv->wl_focus_top;
        if (!parent_win || !parent_win->xdg_surf) {
            for (XmWlWindow *cand = srv->root->first_child; cand; cand = cand->next_sibling) {
                if (cand != win && cand->mapped && cand->xdg_surf && !cand->is_popup) {
                    parent_win = cand;
                    break;
                }
            }
        }
        if (!parent_win || !parent_win->xdg_surf || srv->popup_depth >= XM_WL_MAX_POPUPS) {
            /* Fallback: promote orphan override-redirect window to undecorated xdg_toplevel */
            win->is_popup = false;
            win->xdg_surf = xdg_wm_base_get_xdg_surface(srv->xdg_wm_base, win->wl_surf);
            xdg_surface_add_listener(win->xdg_surf, &g_xdg_surface_listener, win);
            win->xdg_top = xdg_surface_get_toplevel(win->xdg_surf);
            xdg_toplevel_add_listener(win->xdg_top, &g_xdg_toplevel_listener, win);
            if (srv->deco_mgr) {
                win->xdg_deco = zxdg_decoration_manager_v1_get_toplevel_decoration(
                    srv->deco_mgr, win->xdg_top);
                zxdg_toplevel_decoration_v1_add_listener(win->xdg_deco, &g_xdg_deco_listener, win);
                zxdg_toplevel_decoration_v1_set_mode(
                    win->xdg_deco, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
            }
            wl_surface_commit(win->wl_surf);
            wl_display_roundtrip(srv->wl_dpy);
            _XmWlDamageTop(win, 0, 0, win->width, win->height);
            return;
        }

        int rel_x = (int)win->x - (int)parent_win->x;
        int rel_y = (int)win->y - (int)parent_win->y;
        if (rel_x < 0) rel_x = 0;
        if (rel_y < 0) rel_y = 0;
        if (rel_x >= (int)parent_win->width) rel_x = (int)parent_win->width - 1;
        if (rel_y >= (int)parent_win->height) rel_y = (int)parent_win->height - 1;
        if (rel_x < 0) rel_x = 0;
        if (rel_y < 0) rel_y = 0;

        struct xdg_positioner *pos = xdg_wm_base_create_positioner(srv->xdg_wm_base);
        xdg_positioner_set_size(pos, win->width > 0 ? win->width : 1, win->height > 0 ? win->height : 1);
        xdg_positioner_set_anchor_rect(pos, rel_x, rel_y, 1, 1);
        xdg_positioner_set_anchor(pos, XDG_POSITIONER_ANCHOR_TOP_LEFT);
        xdg_positioner_set_gravity(pos, XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT);
        xdg_positioner_set_constraint_adjustment(
            pos,
            XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X |
            XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y |
            XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X |
            XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y |
            XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_X |
            XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_Y);

        win->is_popup = true;
        win->xdg_surf = xdg_wm_base_get_xdg_surface(srv->xdg_wm_base, win->wl_surf);
        xdg_surface_add_listener(win->xdg_surf, &g_xdg_surface_listener, win);
        win->xdg_pop = xdg_surface_get_popup(win->xdg_surf, parent_win->xdg_surf, pos);
        xdg_positioner_destroy(pos);
        xdg_popup_add_listener(win->xdg_pop, &g_xdg_popup_listener, win);

        if (srv->seat && srv->last_button_serial != 0 && srv->ptr_grab_active && srv->popup_depth == 0) {
            xdg_popup_grab(win->xdg_pop, srv->seat, srv->last_button_serial);
        }
        srv->popup_stack[srv->popup_depth++] = win;

        wl_surface_commit(win->wl_surf);
        wl_display_roundtrip(srv->wl_dpy);
        _XmWlDamageTop(win, 0, 0, win->width, win->height);
    }
}

void
_XmWlUnmapWindowRole(XmWlServer *srv, XmWlWindow *win)
{
    if (!srv || !win || !win->wl_surf) return;

    if (srv->pointer_top == win) srv->pointer_top = NULL;
    if (srv->wl_focus_top == win) srv->wl_focus_top = NULL;
    if (srv->wl_dnd_target_top == win) srv->wl_dnd_target_top = NULL;

    if (win->is_popup) {
        for (int i = 0; i < srv->popup_depth; ++i) {
            if (srv->popup_stack[i] == win) {
                pop_popup_stack_from(srv, i);
                return;
            }
        }
    }

    /* Unmap any child popups if a parent top-level unmaps */
    if (!win->is_popup && !win->is_drag_icon && srv->popup_depth > 0) {
        pop_popup_stack_from(srv, 0);
    }

    if (win->xdg_deco) {
        zxdg_toplevel_decoration_v1_destroy(win->xdg_deco);
        win->xdg_deco = NULL;
    }
    if (win->xdg_top) {
        xdg_toplevel_destroy(win->xdg_top);
        win->xdg_top = NULL;
    }
    if (win->xdg_pop) {
        xdg_popup_destroy(win->xdg_pop);
        win->xdg_pop = NULL;
    }
    if (win->xdg_surf) {
        xdg_surface_destroy(win->xdg_surf);
        win->xdg_surf = NULL;
    }
    if (win->frac_scale_obj) {
        wp_fractional_scale_v1_destroy(win->frac_scale_obj);
        win->frac_scale_obj = NULL;
    }
    if (win->viewport) {
        wp_viewport_destroy(win->viewport);
        win->viewport = NULL;
    }
    if (win->wl_surf) {
        wl_surface_destroy(win->wl_surf);
        win->wl_surf = NULL;
    }
    win->xdg_configured = false;
    win->is_popup = false;
    win->is_drag_icon = false;
}

void
_XmWlDestroyWindowRole(XmWlServer *srv, XmWlWindow *win)
{
    if (!win) return;
    _XmWlUnmapWindowRole(srv, win);
    free_shm_buf(&win->shm_bufs[0]);
    free_shm_buf(&win->shm_bufs[1]);
    if (win->canvas) {
        pixman_image_unref(win->canvas);
        win->canvas = NULL;
    }
    free(win->canvas_bits);
    win->canvas_bits = NULL;
}

void
_XmWlFlushDirtySurfaces(XmWlServer *srv)
{
    if (!srv || !srv->root || !srv->wl_dpy) return;

    for (XmWlWindow *w = srv->root->first_child; w; w = w->next_sibling) {
        if (!w->mapped || !w->wl_surf || !w->xdg_configured || !w->dirty || !w->canvas)
            continue;

        double sc = w->preferred_scale > 0.0 ? w->preferred_scale : srv->active_scale;
        if (sc < 1.0 || !srv->viewporter) sc = 1.0;
        int log_w = w->canvas_w > 0 ? w->canvas_w : 1;
        int log_h = w->canvas_h > 0 ? w->canvas_h : 1;
        int buf_w = (int)lround((double)log_w * sc);
        int buf_h = (int)lround((double)log_h * sc);
        if (buf_w < 1) buf_w = 1;
        if (buf_h < 1) buf_h = 1;

        bool has_alpha = (w->depth == 32) || w->has_shape || w->is_drag_icon;
        XmWlShmBuf *sb = !w->shm_bufs[0].busy ? &w->shm_bufs[0] :
                         (!w->shm_bufs[1].busy ? &w->shm_bufs[1] : &w->shm_bufs[0]);
        if (!ensure_shm_buf(srv, sb, buf_w, buf_h, has_alpha))
            continue;

        if (buf_w == log_w && buf_h == log_h) {
            memcpy(sb->pixels, w->canvas_bits, (size_t)log_w * (size_t)log_h * 4u);
        } else {
            pixman_transform_t xform;
            pixman_transform_init_scale(
                &xform,
                pixman_double_to_fixed((double)log_w / (double)buf_w),
                pixman_double_to_fixed((double)log_h / (double)buf_h));
            pixman_image_set_transform(w->canvas, &xform);
            pixman_image_set_filter(w->canvas, PIXMAN_FILTER_BILINEAR, NULL, 0);
            pixman_image_composite32(PIXMAN_OP_SRC, w->canvas, NULL, sb->pix_img,
                                     0, 0, 0, 0, 0, 0, buf_w, buf_h);
            pixman_transform_t ident;
            pixman_transform_init_identity(&ident);
            pixman_image_set_transform(w->canvas, &ident);
        }

        /* Apply non-rectangular SHAPE alpha + input region cutouts when present */
        if (w->has_shape && srv->compositor) {
            struct wl_region *ireg = wl_compositor_create_region(srv->compositor);
            uint8_t *inside = (uint8_t *)calloc((size_t)buf_w * (size_t)buf_h, 1);
            for (size_t ri = 0; ri < w->shape_rect_count; ++ri) {
                const XmWlRect *r = &w->shape_rects[ri];
                wl_region_add(ireg, r->x, r->y, r->width, r->height);
                if (inside) {
                    int sx1 = (int)floor((double)r->x * sc);
                    int sy1 = (int)floor((double)r->y * sc);
                    int sx2 = (int)ceil((double)(r->x + (int)r->width) * sc);
                    int sy2 = (int)ceil((double)(r->y + (int)r->height) * sc);
                    if (sx1 < 0) sx1 = 0;
                    if (sy1 < 0) sy1 = 0;
                    if (sx2 > buf_w) sx2 = buf_w;
                    if (sy2 > buf_h) sy2 = buf_h;
                    for (int py = sy1; py < sy2; ++py)
                        memset(inside + (size_t)py * (size_t)buf_w + (size_t)sx1, 1, (size_t)(sx2 - sx1));
                }
            }
            if (inside) {
                for (int i = 0; i < buf_w * buf_h; ++i) {
                    if (!inside[i]) sb->pixels[i] = 0x00000000u;
                }
                free(inside);
            }
            wl_surface_set_input_region(w->wl_surf, ireg);
            wl_region_destroy(ireg);
        }

        if (w->pending_configure_serial != 0 && w->xdg_surf) {
            xdg_surface_ack_configure(w->xdg_surf, w->pending_configure_serial);
            w->pending_configure_serial = 0;
        }
        if (w->viewport) {
            wp_viewport_set_destination(w->viewport, log_w, log_h);
        }
        sb->busy = true;
        wl_surface_attach(w->wl_surf, sb->wl_buf, 0, 0);
        wl_surface_damage_buffer(w->wl_surf, 0, 0, buf_w, buf_h);
        wl_surface_commit(w->wl_surf);
        w->dirty = false;
    }
    wl_display_flush(srv->wl_dpy);
}

/* ---------- Scale & DPI Live Updates ---------- */

void
_XmWlApplyScaleUpdate(XmWlServer *srv, double new_scale)
{
    if (!srv || new_scale < 0.5 || new_scale > 4.0) return;
    srv->active_scale = new_scale;
    recompute_screen_metrics(srv);

    /* Update RESOURCE_MANAGER Xft.dpi on virtualized root window */
    uint32_t rm_atom = 23; /* XA_RESOURCE_MANAGER */
    XmWlProperty *rm = _XmWlGetProperty(srv->root, rm_atom);
    int dpi = (int)lround(96.0 * new_scale);
    char new_rm[8192];
    new_rm[0] = '\0';
    if (rm && rm->data && rm->byte_len > 0) {
        /* Copy existing lines except Xft.dpi */
        const char *p = (const char *)rm->data;
        const char *end = p + rm->byte_len;
        size_t out = 0;
        while (p < end && *p) {
            const char *nl = (const char *)memchr(p, '\n', (size_t)(end - p));
            size_t llen = nl ? (size_t)(nl - p + 1) : (size_t)(end - p);
            if (llen < 8 || strncmp(p, "Xft.dpi:", 8) != 0) {
                if (out + llen + 1 < sizeof(new_rm)) {
                    memcpy(new_rm + out, p, llen);
                    out += llen;
                    new_rm[out] = '\0';
                }
            }
            p += llen;
        }
        if (out > 0 && new_rm[out - 1] != '\n' && out + 2 < sizeof(new_rm)) {
            new_rm[out++] = '\n';
            new_rm[out] = '\0';
        }
    }
    size_t cur_len = strlen(new_rm);
    snprintf(new_rm + cur_len, sizeof(new_rm) - cur_len, "Xft.dpi:\t%d\n", dpi);
    _XmWlSetProperty(srv, srv->root, rm_atom, 31 /* XA_STRING */, 8, 0 /* Replace */,
                     (const uint8_t *)new_rm, strlen(new_rm), true);

    _XmWlBroadcastRandRChange(srv);

    srv->pending_xt_scale = new_scale;
    srv->has_pending_xt_scale = true;
    _XmWaylandNotifyMainThread(srv);
}

/* ---------- Compositor Root-Property UNIX IPC ---------- */

static void
connect_compositor_ipc(XmWlServer *srv)
{
    srv->ipc_fd = -1;
    const char *rt = getenv("XDG_RUNTIME_DIR");
    const char *wd = getenv("WAYLAND_DISPLAY");
    if (!rt || !rt[0] || !wd || !wd[0]) return;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    int n = snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/dtwm-wayland-%s.ipc", rt, wd);
    if (n <= 0 || (size_t)n >= sizeof(addr.sun_path)) return;

    int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
    if (fd < 0) return;
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 && errno != EINPROGRESS) {
        close(fd);
        return;
    }
    srv->ipc_fd = fd;
}

static size_t
hex_encode(const uint8_t *in, size_t len, char *out, size_t cap)
{
    static const char h[] = "0123456789abcdef";
    size_t o = 0;
    for (size_t i = 0; i < len && o + 2 < cap; ++i) {
        out[o++] = h[in[i] >> 4];
        out[o++] = h[in[i] & 0x0fu];
    }
    out[o] = '\0';
    return o;
}

static int
hex_nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

void
_XmWlIpcPublishRootProp(XmWlServer *srv, const char *atom_name, const uint8_t *data, size_t len)
{
    if (!srv || srv->ipc_fd < 0 || !atom_name || len > 16384) return;
    size_t hex_cap = len * 2 + 4;
    char *hex = (char *)malloc(hex_cap);
    if (!hex) return;
    hex_encode(data, len, hex, hex_cap);
    char hdr[320];
    int hn = snprintf(hdr, sizeof(hdr), "SET %s ", atom_name);
    if (hn > 0 && (size_t)hn < sizeof(hdr)) {
        (void)send(srv->ipc_fd, hdr, (size_t)hn, MSG_NOSIGNAL);
        (void)send(srv->ipc_fd, hex, strlen(hex), MSG_NOSIGNAL);
        (void)send(srv->ipc_fd, "\n", 1, MSG_NOSIGNAL);
    }
    free(hex);
}

void
_XmWlIpcHandleReadable(XmWlServer *srv)
{
    if (!srv || srv->ipc_fd < 0) return;
    static char line_buf[65536];
    static size_t line_len = 0;
    ssize_t n = recv(srv->ipc_fd, line_buf + line_len, sizeof(line_buf) - 1 - line_len, 0);
    if (n <= 0) {
        if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)) {
            close(srv->ipc_fd);
            srv->ipc_fd = -1;
        }
        return;
    }
    line_len += (size_t)n;
    line_buf[line_len] = '\0';

    char *start = line_buf;
    char *nl = NULL;
    while ((nl = strchr(start, '\n')) != NULL) {
        *nl = '\0';
        if (!strncmp(start, "PROP ", 5)) {
            char *p = start + 5;
            char *sp = strchr(p, ' ');
            if (sp) {
                *sp = '\0';
                const char *aname = p;
                const char *hex = sp + 1;
                size_t hlen = strlen(hex);
                uint8_t *raw = (uint8_t *)malloc(hlen / 2 + 1);
                if (raw) {
                    size_t rlen = 0;
                    for (size_t i = 0; i + 1 < hlen; i += 2) {
                        int hi = hex_nibble(hex[i]);
                        int lo = hex_nibble(hex[i + 1]);
                        if (hi < 0 || lo < 0) break;
                        raw[rlen++] = (uint8_t)((hi << 4) | lo);
                    }
                    uint32_t atom = _XmWlInternAtom(srv, aname, strlen(aname), false);
                    uint32_t utf8 = _XmWlInternAtom(srv, "UTF8_STRING", 11, false);
                    uint32_t ptype = !strcmp(aname, "RESOURCE_MANAGER") ? 31u : utf8;
                    _XmWlSetProperty(srv, srv->root, atom, ptype, 8, 0, raw, rlen, true);
                    free(raw);
                }
            }
        }
        start = nl + 1;
    }
    size_t rem = strlen(start);
    if (rem > 0 && start != line_buf)
        memmove(line_buf, start, rem);
    line_len = rem;
}

/* ---------- XKB -> Core X11 Keymap Builder ---------- */

static void
init_default_us_keymap(XmWlServer *srv)
{
    memset(srv->keysym_table, 0, sizeof(srv->keysym_table));
    memset(srv->mod_map, 0, sizeof(srv->mod_map));

    static const struct { uint8_t evkey; uint32_t norm; uint32_t shft; } us_keys[] = {
        { KEY_ESC, 0xff1b, 0xff1b },
        { KEY_1, '1', '!' }, { KEY_2, '2', '@' }, { KEY_3, '3', '#' }, { KEY_4, '4', '$' },
        { KEY_5, '5', '%' }, { KEY_6, '6', '^' }, { KEY_7, '7', '&' }, { KEY_8, '8', '*' },
        { KEY_9, '9', '(' }, { KEY_0, '0', ')' }, { KEY_MINUS, '-', '_' }, { KEY_EQUAL, '=', '+' },
        { KEY_BACKSPACE, 0xff08, 0xff08 }, { KEY_TAB, 0xff09, 0xfe20 },
        { KEY_Q, 'q', 'Q' }, { KEY_W, 'w', 'W' }, { KEY_E, 'e', 'E' }, { KEY_R, 'r', 'R' },
        { KEY_T, 't', 'T' }, { KEY_Y, 'y', 'Y' }, { KEY_U, 'u', 'U' }, { KEY_I, 'i', 'I' },
        { KEY_O, 'o', 'O' }, { KEY_P, 'p', 'P' }, { KEY_LEFTBRACE, '[', '{' }, { KEY_RIGHTBRACE, ']', '}' },
        { KEY_ENTER, 0xff0d, 0xff0d },
        { KEY_A, 'a', 'A' }, { KEY_S, 's', 'S' }, { KEY_D, 'd', 'D' }, { KEY_F, 'f', 'F' },
        { KEY_G, 'g', 'G' }, { KEY_H, 'h', 'H' }, { KEY_J, 'j', 'J' }, { KEY_K, 'k', 'K' },
        { KEY_L, 'l', 'L' }, { KEY_SEMICOLON, ';', ':' }, { KEY_APOSTROPHE, '\'', '"' }, { KEY_GRAVE, '`', '~' },
        { KEY_BACKSLASH, '\\', '|' },
        { KEY_Z, 'z', 'Z' }, { KEY_X, 'x', 'X' }, { KEY_C, 'c', 'C' }, { KEY_V, 'v', 'V' },
        { KEY_B, 'b', 'B' }, { KEY_N, 'n', 'N' }, { KEY_M, 'm', 'M' },
        { KEY_COMMA, ',', '<' }, { KEY_DOT, '.', '>' }, { KEY_SLASH, '/', '?' },
        { KEY_SPACE, ' ', ' ' },
        { KEY_F1, 0xffbe, 0xffbe }, { KEY_F2, 0xffbf, 0xffbf }, { KEY_F3, 0xffc0, 0xffc0 },
        { KEY_F4, 0xffc1, 0xffc1 }, { KEY_F5, 0xffc2, 0xffc2 }, { KEY_F6, 0xffc3, 0xffc3 },
        { KEY_F7, 0xffc4, 0xffc4 }, { KEY_F8, 0xffc5, 0xffc5 }, { KEY_F9, 0xffc6, 0xffc6 },
        { KEY_F10, 0xffc7, 0xffc7 }, { KEY_F11, 0xffc8, 0xffc8 }, { KEY_F12, 0xffc9, 0xffc9 },
        { KEY_HOME, 0xff50, 0xff50 }, { KEY_LEFT, 0xff51, 0xff51 }, { KEY_UP, 0xff52, 0xff52 },
        { KEY_RIGHT, 0xff53, 0xff53 }, { KEY_DOWN, 0xff54, 0xff54 },
        { KEY_PAGEUP, 0xff55, 0xff55 }, { KEY_PAGEDOWN, 0xff56, 0xff56 },
        { KEY_END, 0xff57, 0xff57 }, { KEY_INSERT, 0xff63, 0xff63 }, { KEY_DELETE, 0xffff, 0xffff },
        { KEY_LEFTSHIFT, 0xffe1, 0xffe1 }, { KEY_RIGHTSHIFT, 0xffe2, 0xffe2 },
        { KEY_LEFTCTRL, 0xffe3, 0xffe3 }, { KEY_RIGHTCTRL, 0xffe4, 0xffe4 },
        { KEY_CAPSLOCK, 0xffe5, 0xffe5 },
        { KEY_LEFTALT, 0xffe9, 0xffe9 }, { KEY_RIGHTALT, 0xffea, 0xffea },
        { KEY_LEFTMETA, 0xffeb, 0xffeb }, { KEY_RIGHTMETA, 0xffec, 0xffec },
    };
    for (size_t i = 0; i < sizeof(us_keys) / sizeof(us_keys[0]); ++i) {
        uint8_t kc = (uint8_t)(us_keys[i].evkey + 8u);
        srv->keysym_table[kc][0] = us_keys[i].norm;
        srv->keysym_table[kc][1] = us_keys[i].shft;
        srv->keysym_table[kc][2] = us_keys[i].norm;
        srv->keysym_table[kc][3] = us_keys[i].shft;
    }
    srv->mod_map[0][0] = KEY_LEFTSHIFT + 8;
    srv->mod_map[0][1] = KEY_RIGHTSHIFT + 8;
    srv->mod_map[1][0] = KEY_CAPSLOCK + 8;
    srv->mod_map[2][0] = KEY_LEFTCTRL + 8;
    srv->mod_map[2][1] = KEY_RIGHTCTRL + 8;
    srv->mod_map[3][0] = KEY_LEFTALT + 8;
    srv->mod_map[3][1] = KEY_RIGHTALT + 8;
    srv->mod_map[6][0] = KEY_LEFTMETA + 8;
    srv->mod_map[6][1] = KEY_RIGHTMETA + 8;
}

void
_XmWlRebuildKeymapFromXkb(XmWlServer *srv)
{
    if (!srv || !srv->xkb_keymap) return;
    xkb_keycode_t min_kc = xkb_keymap_min_keycode(srv->xkb_keymap);
    xkb_keycode_t max_kc = xkb_keymap_max_keycode(srv->xkb_keymap);
    if (max_kc > 255) max_kc = 255;

    for (xkb_keycode_t kc = min_kc; kc <= max_kc; ++kc) {
        xkb_layout_index_t nlay = xkb_keymap_num_layouts_for_key(srv->xkb_keymap, kc);
        if (nlay == 0) continue;
        xkb_level_index_t nlev = xkb_keymap_num_levels_for_key(srv->xkb_keymap, kc, 0);
        for (uint32_t lv = 0; lv < 4; ++lv) {
            const xkb_keysym_t *syms = NULL;
            int n = (lv < nlev) ? xkb_keymap_key_get_syms_by_level(srv->xkb_keymap, kc, 0, lv, &syms) : 0;
            if (n > 0 && syms)
                srv->keysym_table[kc][lv] = syms[0];
        }
    }
}

/* ---------- Server Event Loop Thread ---------- */

static void *
wayland_server_thread(void *arg)
{
    XmWlServer *srv = (XmWlServer *)arg;

    while (srv->running) {
        if (wl_display_prepare_read(srv->wl_dpy) != 0) {
            wl_display_dispatch_pending(srv->wl_dpy);
            continue;
        }
        wl_display_flush(srv->wl_dpy);

        struct pollfd pfds[4 + XM_WL_MAX_CLIENTS];
        int client_map[4 + XM_WL_MAX_CLIENTS];
        nfds_t nfds = 0;

        int wl_idx = (int)nfds;
        pfds[nfds] = (struct pollfd){ .fd = wl_display_get_fd(srv->wl_dpy), .events = POLLIN };
        client_map[nfds++] = -1;

        int listen_idx = (int)nfds;
        pfds[nfds] = (struct pollfd){ .fd = srv->listen_fd, .events = POLLIN };
        client_map[nfds++] = -1;

        int wake_idx = (int)nfds;
        pfds[nfds] = (struct pollfd){ .fd = srv->wake_pipe[0], .events = POLLIN };
        client_map[nfds++] = -1;

        int ipc_idx = -1;
        if (srv->ipc_fd >= 0) {
            ipc_idx = (int)nfds;
            pfds[nfds] = (struct pollfd){ .fd = srv->ipc_fd, .events = POLLIN };
            client_map[nfds++] = -1;
        }

        pthread_mutex_lock(&srv->lock);
        for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
            if (srv->clients[i].active && srv->clients[i].fd >= 0) {
                short ev = POLLIN;
                if (srv->clients[i].tx_len > 0) ev |= POLLOUT;
                pfds[nfds] = (struct pollfd){ .fd = srv->clients[i].fd, .events = ev };
                client_map[nfds++] = i;
            }
        }
        int timeout_ms = -1;
        if (srv->held_keycode != 0 && srv->repeat_rate > 0) {
            uint64_t now = now_mono_ms64();
            timeout_ms = (srv->next_repeat_ms > now) ? (int)(srv->next_repeat_ms - now) : 0;
        }
        pthread_mutex_unlock(&srv->lock);

        int prc = poll(pfds, nfds, timeout_ms);
        if (!srv->running) {
            wl_display_cancel_read(srv->wl_dpy);
            break;
        }

        if (prc > 0 && (pfds[wl_idx].revents & (POLLIN | POLLERR | POLLHUP))) {
            if (wl_display_read_events(srv->wl_dpy) < 0) {
                srv->running = false;
                break;
            }
        } else {
            wl_display_cancel_read(srv->wl_dpy);
        }

        pthread_mutex_lock(&srv->lock);
        wl_display_dispatch_pending(srv->wl_dpy);

        if (prc > 0 && (pfds[wake_idx].revents & POLLIN)) {
            uint8_t drain[64];
            while (read(srv->wake_pipe[0], drain, sizeof(drain)) > 0) {}
        }

        if (ipc_idx >= 0 && prc > 0 && (pfds[ipc_idx].revents & (POLLIN | POLLHUP | POLLERR))) {
            _XmWlIpcHandleReadable(srv);
        }

        if (prc > 0 && (pfds[listen_idx].revents & POLLIN)) {
            int cfd = accept(srv->listen_fd, NULL, NULL);
            if (cfd >= 0) {
                fcntl(cfd, F_SETFD, FD_CLOEXEC);
                int fl = fcntl(cfd, F_GETFL, 0);
                if (fl >= 0) fcntl(cfd, F_SETFL, fl | O_NONBLOCK);
                int slot = -1;
                for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
                    if (!srv->clients[i].active) { slot = i; break; }
                }
                if (slot >= 0) {
                    XmWlClient *c = &srv->clients[slot];
                    memset(c, 0, sizeof(*c));
                    c->active = true;
                    c->fd = cfd;
                    c->index = slot;
                    c->resource_base = UINT32_C(0x00200000) * (uint32_t)(slot + 1);
                } else {
                    close(cfd);
                }
            }
        }

        for (nfds_t k = 0; k < nfds; ++k) {
            int ci = client_map[k];
            if (ci < 0 || !srv->clients[ci].active) continue;
            if (pfds[k].revents & (POLLERR | POLLHUP)) {
                if (pfds[k].revents & POLLIN)
                    _XmWlHandleClientReadable(srv, ci);
                if (srv->clients[ci].active)
                    _XmWlDisconnectClient(srv, ci);
                continue;
            }
            if (pfds[k].revents & POLLIN) {
                _XmWlHandleClientReadable(srv, ci);
            }
        }

        if (srv->held_keycode != 0 && srv->repeat_rate > 0) {
            uint64_t now = now_mono_ms64();
            if (now >= srv->next_repeat_ms) {
                uint32_t t = _XmWlNowMs();
                _XmWlDispatchKey(srv, (uint8_t)srv->held_keycode, false, t);
                _XmWlDispatchKey(srv, (uint8_t)srv->held_keycode, true, t);
                uint64_t step = (uint64_t)(1000 / srv->repeat_rate);
                if (step < 10) step = 10;
                srv->next_repeat_ms = now + step;
            }
        }

        _XmWlFlushDirtySurfaces(srv);
        _XmWlFlushClientWrites(srv);
        pthread_mutex_unlock(&srv->lock);
    }
    return NULL;
}

/* ---------- Main-Thread XtAppAddInput Wakeup ---------- */

static void
xt_wayland_wakeup_cb(XtPointer closure, int *source, XtInputId *id)
{
    (void)source;
    (void)id;
    XmWlServer *srv = (XmWlServer *)closure;
    if (!srv) return;
    uint8_t drain[64];
    while (read(srv->xt_pipe[0], drain, sizeof(drain)) > 0) {}

    double new_scale = 0.0;
    bool apply_scale = false;
    pthread_mutex_lock(&srv->lock);
    if (srv->has_pending_xt_scale) {
        new_scale = srv->pending_xt_scale;
        srv->has_pending_xt_scale = false;
        apply_scale = true;
    }
    pthread_mutex_unlock(&srv->lock);

    if (apply_scale && srv->attached_xm_display) {
        XmSetDisplayScaleOverride(XtDisplay(srv->attached_xm_display), new_scale);
    }
    _XmWlImeDrainMainThread(srv);
}

void
_XmWaylandAttachDisplay(Widget xm_display)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !xm_display) return;
    pthread_mutex_lock(&srv->lock);
    if (!srv->attached_xm_display && srv->xt_pipe[0] >= 0) {
        srv->attached_xm_display = xm_display;
        XtAppContext app = XtWidgetToApplicationContext(xm_display);
        srv->xt_input_id = XtAppAddInput(
            app, srv->xt_pipe[0], (XtPointer)XtInputReadMask, xt_wayland_wakeup_cb, srv);
    }
    pthread_mutex_unlock(&srv->lock);
}

void
_XmWaylandDetachDisplay(Widget xm_display)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !xm_display) return;
    pthread_mutex_lock(&srv->lock);
    if (srv->attached_xm_display == xm_display) {
        if (srv->xt_input_id) {
            XtRemoveInput(srv->xt_input_id);
            srv->xt_input_id = 0;
        }
        srv->attached_xm_display = NULL;
    }
    pthread_mutex_unlock(&srv->lock);
}

/* ---------- Cleanup & Initialization Seam ---------- */

static void
cleanup_wayland_backend(void)
{
    if (!g_wl_srv || g_wl_owner_pid != getpid())
        return;
    XmWlServer *srv = g_wl_srv;
    g_wl_srv = NULL;

    srv->running = false;
    _XmWlWakeServer(srv);
    pthread_join(srv->thread, NULL);

    if (srv->listen_fd >= 0) close(srv->listen_fd);
    if (srv->socket_path[0]) unlink(srv->socket_path);
    if (srv->runtime_dir[0]) rmdir(srv->runtime_dir);
    if (srv->ipc_fd >= 0) close(srv->ipc_fd);
    if (srv->wake_pipe[0] >= 0) close(srv->wake_pipe[0]);
    if (srv->wake_pipe[1] >= 0) close(srv->wake_pipe[1]);
    if (srv->xt_pipe[0] >= 0) close(srv->xt_pipe[0]);
    if (srv->xt_pipe[1] >= 0) close(srv->xt_pipe[1]);
    _XmWlRenderDestroy(srv);
    if (srv->wl_dpy) wl_display_disconnect(srv->wl_dpy);
    pthread_mutex_destroy(&srv->lock);
    free(srv);
}

Boolean
_XmWaylandEnsureDisplay(void)
{
    pthread_mutex_lock(&g_wl_init_lock);
    if (g_wl_srv && g_wl_owner_pid == getpid()) {
        pthread_mutex_unlock(&g_wl_init_lock);
        return True;
    }

    const char *wl_disp = getenv("WAYLAND_DISPLAY");
    if (!wl_disp || !wl_disp[0]) {
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }

    const char *xdg_rt = getenv("XDG_RUNTIME_DIR");
    if (!xdg_rt || xdg_rt[0] != '/') {
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }

    struct wl_display *wl_dpy = wl_display_connect(NULL);
    if (!wl_dpy) {
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }

    XmWlServer *srv = (XmWlServer *)calloc(1, sizeof(*srv));
    if (!srv) {
        wl_display_disconnect(wl_dpy);
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }

    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&srv->lock, &mattr);
    pthread_mutexattr_destroy(&mattr);
    srv->wl_dpy = wl_dpy;
    srv->listen_fd = -1;
    srv->ipc_fd = -1;
    srv->wake_pipe[0] = srv->wake_pipe[1] = -1;
    srv->xt_pipe[0] = srv->xt_pipe[1] = -1;
    srv->screen_w = 1280;
    srv->screen_h = 800;
    srv->screen_mm_w = 338;
    srv->screen_mm_h = 211;
    srv->active_scale = 1.0;
    srv->repeat_rate = 25;
    srv->repeat_delay = 400;
    srv->x11_focus_win = 1u; /* PointerRoot */
    srv->x11_focus_revert = 1u;

    const char *env_scale = getenv("CK_MOTIF_WAYLAND_SCALE");
    if (env_scale && env_scale[0]) {
        double sc = strtod(env_scale, NULL);
        if (sc >= 0.5 && sc <= 4.0)
            srv->active_scale = sc;
    }

    if (pipe2(srv->wake_pipe, O_CLOEXEC | O_NONBLOCK) != 0 ||
        pipe2(srv->xt_pipe, O_CLOEXEC | O_NONBLOCK) != 0) {
        if (srv->wake_pipe[0] >= 0) { close(srv->wake_pipe[0]); close(srv->wake_pipe[1]); }
        wl_display_disconnect(wl_dpy);
        free(srv);
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }

    snprintf(srv->runtime_dir, sizeof(srv->runtime_dir),
             "%s/ck-wlx11-%d-XXXXXX", xdg_rt, (int)getpid());
    if (!mkdtemp(srv->runtime_dir)) {
        close(srv->wake_pipe[0]); close(srv->wake_pipe[1]);
        close(srv->xt_pipe[0]); close(srv->xt_pipe[1]);
        wl_display_disconnect(wl_dpy);
        free(srv);
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }
    chmod(srv->runtime_dir, 0700);
    snprintf(srv->socket_path, sizeof(srv->socket_path), "%s/S:0", srv->runtime_dir);
    snprintf(srv->display_spec, sizeof(srv->display_spec), "%s/S:0", srv->runtime_dir);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (strlen(srv->socket_path) >= sizeof(addr.sun_path)) {
        rmdir(srv->runtime_dir);
        close(srv->wake_pipe[0]); close(srv->wake_pipe[1]);
        close(srv->xt_pipe[0]); close(srv->xt_pipe[1]);
        wl_display_disconnect(wl_dpy);
        free(srv);
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }
    memcpy(addr.sun_path, srv->socket_path, strlen(srv->socket_path) + 1);

    int lfd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
    if (lfd < 0 || bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(lfd, 16) != 0) {
        if (lfd >= 0) close(lfd);
        unlink(srv->socket_path);
        rmdir(srv->runtime_dir);
        close(srv->wake_pipe[0]); close(srv->wake_pipe[1]);
        close(srv->xt_pipe[0]); close(srv->xt_pipe[1]);
        wl_display_disconnect(wl_dpy);
        free(srv);
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }
    chmod(srv->socket_path, 0600);
    srv->listen_fd = lfd;

    init_default_us_keymap(srv);
    _XmWlRenderInit(srv);
    _XmWlInitAtomsAndRoot(srv);

    srv->registry = wl_display_get_registry(wl_dpy);
    wl_registry_add_listener(srv->registry, &g_registry_listener, srv);
    wl_display_roundtrip(wl_dpy);
    wl_display_roundtrip(wl_dpy);

    if (!srv->compositor || !srv->shm || !srv->xdg_wm_base) {
        close(srv->listen_fd);
        unlink(srv->socket_path);
        rmdir(srv->runtime_dir);
        close(srv->wake_pipe[0]); close(srv->wake_pipe[1]);
        close(srv->xt_pipe[0]); close(srv->xt_pipe[1]);
        _XmWlRenderDestroy(srv);
        wl_display_disconnect(wl_dpy);
        free(srv);
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }

    if (srv->shm && srv->compositor) {
        srv->cursor_theme = wl_cursor_theme_load(NULL, 24, srv->shm);
        srv->cursor_surface = wl_compositor_create_surface(srv->compositor);
    }

    connect_compositor_ipc(srv);
    if (srv->active_scale != 1.0)
        _XmWlApplyScaleUpdate(srv, srv->active_scale);

    g_wl_srv = srv;
    g_wl_owner_pid = getpid();
    srv->running = true;
    if (pthread_create(&srv->thread, NULL, wayland_server_thread, srv) != 0) {
        srv->running = false;
        g_wl_srv = NULL;
        close(srv->listen_fd);
        unlink(srv->socket_path);
        rmdir(srv->runtime_dir);
        close(srv->wake_pipe[0]); close(srv->wake_pipe[1]);
        close(srv->xt_pipe[0]); close(srv->xt_pipe[1]);
        _XmWlRenderDestroy(srv);
        wl_display_disconnect(wl_dpy);
        free(srv);
        pthread_mutex_unlock(&g_wl_init_lock);
        return False;
    }

    const char *prev_dpy = getenv("DISPLAY");
    if (prev_dpy && prev_dpy[0] && !strstr(prev_dpy, "/ck-wlx11-")) {
        setenv("CK_MOTIF_ORIGINAL_DISPLAY", prev_dpy, 1);
    }
    setenv("DISPLAY", srv->display_spec, 1);
    setenv("CK_MOTIF_WAYLAND_ACTIVE", "1", 1);
    setenv("CK_MOTIF_WAYLAND_DISPLAY", wl_disp, 1);
    atexit(cleanup_wayland_backend);
    pthread_mutex_unlock(&g_wl_init_lock);
    return True;
}

static bool
display_matches_wayland_server(Display *display, XmWlServer *srv)
{
    if (!display || !srv)
        return false;
    const char *dstr = DisplayString(display);
    return dstr && strcmp(dstr, srv->display_spec) == 0;
}

Boolean
XmWaylandIsActive(Display *display)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv)
        return False;
    if (!display)
        return True;
    return display_matches_wayland_server(display, srv) ? True : False;
}

void
XmWaylandLock(Display *display)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || (display && !display_matches_wayland_server(display, srv)))
        return;
    pthread_mutex_lock(&srv->lock);
}

void
XmWaylandUnlock(Display *display)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || (display && !display_matches_wayland_server(display, srv)))
        return;
    pthread_mutex_unlock(&srv->lock);
}

Boolean
XmWaylandGetDisplayHandles(
    Display *display,
    struct wl_display **wl_display_return,
    struct wl_compositor **wl_compositor_return,
    struct wl_subcompositor **wl_subcompositor_return,
    struct wl_shm **wl_shm_return,
    struct wp_viewporter **wp_viewporter_return)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(display, srv))
        return False;
    if (wl_display_return) *wl_display_return = srv->wl_dpy;
    if (wl_compositor_return) *wl_compositor_return = srv->compositor;
    if (wl_subcompositor_return) *wl_subcompositor_return = srv->subcompositor;
    if (wl_shm_return) *wl_shm_return = srv->shm;
    if (wp_viewporter_return) *wp_viewporter_return = srv->viewporter;
    return True;
}

Boolean
XmWaylandGetWidgetSurface(
    Widget widget,
    struct wl_surface **toplevel_surface_return,
    int *x_return,
    int *y_return,
    int *width_return,
    int *height_return,
    double *scale_return)
{
    if (!widget || !XtIsRealized(widget))
        return False;
    Display *dpy = XtDisplayOfObject(widget);
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(dpy, srv))
        return False;

    Window xwin = XtWindowOfObject(widget);
    if (xwin == None)
        return False;

    Boolean ok = False;
    pthread_mutex_lock(&srv->lock);
    XmWlWindow *win = _XmWlFindWindow(srv, (uint32_t)xwin);
    int dx = 0, dy = 0;
    XmWlWindow *top = win ? _XmWlEnclosingTopLevel(srv, win, &dx, &dy) : NULL;
    if (top && top->wl_surf) {
        if (!XtIsWidget(widget)) {
            dx += (int)widget->core.x;
            dy += (int)widget->core.y;
        }
        if (toplevel_surface_return) *toplevel_surface_return = top->wl_surf;
        if (x_return) *x_return = dx;
        if (y_return) *y_return = dy;
        if (width_return) *width_return = (int)widget->core.width;
        if (height_return) *height_return = (int)widget->core.height;
        if (scale_return)
            *scale_return = top->preferred_scale > 0.0 ? top->preferred_scale : srv->active_scale;
        ok = True;
    }
    pthread_mutex_unlock(&srv->lock);
    return ok;
}

__attribute__((constructor))
static void
xm_wl_library_ctor(void)
{
    const char *backend = getenv("CK_MOTIF_BACKEND");
    if (backend && (!strcasecmp(backend, "x11") || !strcmp(backend, "0") || !strcasecmp(backend, "off")))
        return;

    bool force_wl = backend && (!strcasecmp(backend, "wayland") || !strcmp(backend, "1"));
    const char *wl_disp = getenv("WAYLAND_DISPLAY");
    const char *x_disp = getenv("DISPLAY");

    if (!wl_disp || !wl_disp[0])
        return;

    bool inherited_virtual = x_disp && strstr(x_disp, "/ck-wlx11-") != NULL;
    bool has_real_x11 = x_disp && x_disp[0] && !inherited_virtual;

#if defined(__GNUC__) || defined(__clang__)
    if (&ck_motif_prefer_x11 != NULL && ck_motif_prefer_x11 != 0 && has_real_x11)
        return;
#endif

    /* Activate automatically when DISPLAY is unset/empty OR inherited from a parent ck-wlx11 process */
    if (force_wl || !has_real_x11) {
        (void)_XmWaylandEnsureDisplay();
    }
}

#else /* !HAVE_WAYLAND_BACKEND */

Boolean
XmWaylandIsActive(Display *display)
{
    (void)display;
    return False;
}

void
XmWaylandLock(Display *display)
{
    (void)display;
}

void
XmWaylandUnlock(Display *display)
{
    (void)display;
}

Boolean
XmWaylandGetDisplayHandles(
    Display *display,
    struct wl_display **wl_display_return,
    struct wl_compositor **wl_compositor_return,
    struct wl_subcompositor **wl_subcompositor_return,
    struct wl_shm **wl_shm_return,
    struct wp_viewporter **wp_viewporter_return)
{
    (void)display;
    (void)wl_display_return;
    (void)wl_compositor_return;
    (void)wl_subcompositor_return;
    (void)wl_shm_return;
    (void)wp_viewporter_return;
    return False;
}

Boolean
XmWaylandGetWidgetSurface(
    Widget widget,
    struct wl_surface **toplevel_surface_return,
    int *x_return,
    int *y_return,
    int *width_return,
    int *height_return,
    double *scale_return)
{
    (void)widget;
    (void)toplevel_surface_return;
    (void)x_return;
    (void)y_return;
    (void)width_return;
    (void)height_return;
    (void)scale_return;
    return False;
}

#endif /* HAVE_WAYLAND_BACKEND */
