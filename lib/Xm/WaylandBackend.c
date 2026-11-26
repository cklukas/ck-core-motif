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
#include "XmI.h"
#include <ctype.h>
#include <dlfcn.h>
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

static _Atomic(XmWlServer *) g_wl_srv = NULL;
static _Atomic pid_t g_wl_owner_pid = 0;
static pthread_mutex_t g_wl_init_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t g_wl_atfork_once = PTHREAD_ONCE_INIT;

static void
wayland_atfork_child(void)
{
    XmWlServer *srv = g_wl_srv;
    g_wl_srv = NULL;
    g_wl_owner_pid = 0;
    pthread_mutex_init(&g_wl_init_lock, NULL);
    if (!srv)
        return;
    /* In a forked child, wayland_server_thread does not exist; release
     * inherited server/IPC descriptors so daemonized or worker children
     * (e.g. ttsession, dtfile directory readers) never hold zombie IPC pipes. */
    if (srv->ipc_fd >= 0) {
        close(srv->ipc_fd);
        srv->ipc_fd = -1;
    }
    if (srv->listen_fd >= 0) {
        close(srv->listen_fd);
        srv->listen_fd = -1;
    }
    if (srv->wake_pipe[0] >= 0) close(srv->wake_pipe[0]);
    if (srv->wake_pipe[1] >= 0) close(srv->wake_pipe[1]);
    if (srv->xt_pipe[0] >= 0) close(srv->xt_pipe[0]);
    if (srv->xt_pipe[1] >= 0) close(srv->xt_pipe[1]);
    if (srv->wl_dpy) {
        int wfd = wl_display_get_fd(srv->wl_dpy);
        if (wfd >= 0)
            close(wfd);
    }
}

static void
register_wayland_atfork(void)
{
    pthread_atfork(NULL, NULL, wayland_atfork_child);
}

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
    if (sb->pool) {
        wl_shm_pool_destroy(sb->pool);
        sb->pool = NULL;
    }
    if (sb->pixels && sb->pool_cap > 0) {
        munmap(sb->pixels, sb->pool_cap);
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
    if (sb->wl_buf && sb->width == w && sb->height == h && sb->has_alpha == has_alpha)
        return true;

    if (!srv->shm) return false;
    if (sb->pix_img) {
        pixman_image_unref(sb->pix_img);
        sb->pix_img = NULL;
    }
    if (sb->wl_buf) {
        wl_buffer_destroy(sb->wl_buf);
        sb->wl_buf = NULL;
    }

    int stride = w * 4;
    size_t size = (size_t)stride * (size_t)h;
    size_t min_cap = (size + 0x3fffffu) & ~(size_t)0x3fffffu;
    if (min_cap < 0x400000u)
        min_cap = 0x400000u;

    if (sb->fd < 0 || !sb->pool || !sb->pixels) {
        free_shm_buf(sb);
        int fd = create_shm_fd(min_cap);
        if (fd < 0) return false;
        void *map = mmap(NULL, min_cap, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
            close(fd);
            return false;
        }
        struct wl_shm_pool *pool = wl_shm_create_pool(srv->shm, fd, (int32_t)min_cap);
        if (!pool) {
            munmap(map, min_cap);
            close(fd);
            return false;
        }
        sb->fd = fd;
        sb->pixels = (uint32_t *)map;
        sb->pool = pool;
        sb->pool_cap = min_cap;
    } else if (size > sb->pool_cap) {
        if (ftruncate(sb->fd, (off_t)min_cap) != 0) {
            free_shm_buf(sb);
            return false;
        }
        void *map = mmap(NULL, min_cap, PROT_READ | PROT_WRITE, MAP_SHARED, sb->fd, 0);
        if (map == MAP_FAILED) {
            free_shm_buf(sb);
            return false;
        }
        munmap(sb->pixels, sb->pool_cap);
        wl_shm_pool_resize(sb->pool, (int32_t)min_cap);
        sb->pixels = (uint32_t *)map;
        sb->pool_cap = min_cap;
    }

    uint32_t fmt = has_alpha ? WL_SHM_FORMAT_ARGB8888 : WL_SHM_FORMAT_XRGB8888;
    struct wl_buffer *buf = wl_shm_pool_create_buffer(sb->pool, 0, w, h, stride, fmt);
    if (!buf) {
        return false;
    }

    sb->wl_buf = buf;
    sb->byte_size = size;
    sb->width = w;
    sb->height = h;
    sb->stride = stride;
    sb->has_alpha = has_alpha;
    sb->busy = false;
    sb->valid_w = 0;
    sb->valid_h = 0;
    sb->valid_gen = 0;
    sb->pix_img = pixman_image_create_bits(
        has_alpha ? PIXMAN_a8r8g8b8 : PIXMAN_x8r8g8b8,
        w, h, sb->pixels, stride);
    wl_buffer_add_listener(buf, &g_wl_buffer_listener, sb);
    return true;
}

static void
fill_pixman_rect_u32(pixman_image_t *img, int x, int y, int w, int h, uint32_t argb)
{
    if (!img || w <= 0 || h <= 0)
        return;
    pixman_color_t pc;
    uint32_t a = (argb >> 24) & 0xffu;
    uint32_t r = (argb >> 16) & 0xffu;
    uint32_t g = (argb >> 8) & 0xffu;
    uint32_t b = argb & 0xffu;
    pc.alpha = (uint16_t)(a * 0x0101u);
    pc.red   = (uint16_t)(r * 0x0101u);
    pc.green = (uint16_t)(g * 0x0101u);
    pc.blue  = (uint16_t)(b * 0x0101u);
    pixman_box32_t box = { x, y, x + w, y + h };
    pixman_image_fill_boxes(PIXMAN_OP_SRC, img, &pc, 1, &box);
}

void
_XmWlEnsureTopCanvas(XmWlServer *srv, XmWlWindow *top)
{
    (void)srv;
    if (!top) return;
    int w = top->width > 0 ? top->width : 1;
    int h = top->height > 0 ? top->height : 1;
    if (w > 8192) w = 8192;
    if (h > 8192) h = 8192;
    if (top->canvas && top->canvas_w == w && top->canvas_h == h)
        return;

    uint32_t fill = top->has_bg_pixel ? (0xff000000u | (top->bg_pixel & 0x00ffffffu)) :
                    (top->depth == 32 ? 0x00000000u : 0xffd8d8d8u);

    /* Fast path: reuse padded backing buffer in-place without malloc/free/memcpy */
    if (top->canvas_bits && w <= top->canvas_cap_w && h <= top->canvas_cap_h) {
        int old_w = top->canvas_w;
        int old_h = top->canvas_h;
        pixman_image_t *new_img = pixman_image_create_bits(
            PIXMAN_a8r8g8b8, w, h, top->canvas_bits, top->canvas_stride);
        if (!new_img)
            return;
        if (top->canvas)
            pixman_image_unref(top->canvas);
        top->canvas = new_img;
        if (w > old_w) {
            int ch = h < old_h ? h : old_h;
            fill_pixman_rect_u32(new_img, old_w, 0, w - old_w, ch, fill);
        }
        if (h > old_h) {
            fill_pixman_rect_u32(new_img, 0, old_h, w, h - old_h, fill);
        }
        top->canvas_w = w;
        top->canvas_h = h;
        _XmWlDamageTop(top, 0, 0, w, h);
        return;
    }

    int cap_w = (w + 255) & ~255;
    int cap_h = (h + 255) & ~255;
    if (cap_w < top->canvas_cap_w) cap_w = top->canvas_cap_w;
    if (cap_h < top->canvas_cap_h) cap_h = top->canvas_cap_h;
    int new_stride = cap_w * 4;
    uint32_t *new_bits = (uint32_t *)malloc((size_t)new_stride * (size_t)cap_h);
    if (!new_bits) return;
    pixman_image_t *new_img = pixman_image_create_bits(PIXMAN_a8r8g8b8, w, h, new_bits, new_stride);
    if (!new_img) {
        free(new_bits);
        return;
    }

    if (top->canvas && top->canvas_bits) {
        int cw = w < top->canvas_w ? w : top->canvas_w;
        int ch = h < top->canvas_h ? h : top->canvas_h;
        for (int row = 0; row < ch; ++row) {
            memcpy((uint8_t *)new_bits + (size_t)row * (size_t)new_stride,
                   (const uint8_t *)top->canvas_bits + (size_t)row * (size_t)top->canvas_stride,
                   (size_t)cw * 4u);
        }
        if (w > cw)
            fill_pixman_rect_u32(new_img, cw, 0, w - cw, ch, fill);
        if (h > ch)
            fill_pixman_rect_u32(new_img, 0, ch, w, h - ch, fill);
        pixman_image_unref(top->canvas);
        free(top->canvas_bits);
    } else {
        fill_pixman_rect_u32(new_img, 0, 0, w, h, fill);
    }
    top->canvas = new_img;
    top->canvas_bits = new_bits;
    top->canvas_w = w;
    top->canvas_h = h;
    top->canvas_cap_w = cap_w;
    top->canvas_cap_h = cap_h;
    top->canvas_stride = new_stride;
    _XmWlDamageTop(top, 0, 0, w, h);
}

static inline void
ensure_win_dmg_regions(XmWlWindow *w)
{
    if (!w || w->dmg_regions_inited) return;
    pixman_region32_init(&w->dirty_region);
    for (int i = 0; i < XM_WL_SHM_SLOTS; ++i)
        pixman_region32_init(&w->dmg_hist_reg[i]);
    w->dmg_regions_inited = true;
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

    ensure_win_dmg_regions(top);
    pixman_region32_union_rect(&top->dirty_region, &top->dirty_region,
                               x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
    if (pixman_region32_n_rects(&top->dirty_region) > 64) {
        const pixman_box32_t *ext = pixman_region32_extents(&top->dirty_region);
        if (ext && ext->x2 > ext->x1 && ext->y2 > ext->y1)
            pixman_region32_reset(&top->dirty_region, ext);
    }

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

    int fallback_w = win->has_queued_wl_cfg ? win->queued_wl_w : (int)win->width;
    int fallback_h = win->has_queued_wl_cfg ? win->queued_wl_h : (int)win->height;
    int cw = win->pending_cfg_w > 0 ? win->pending_cfg_w : fallback_w;
    int ch = win->pending_cfg_h > 0 ? win->pending_cfg_h : fallback_h;
    win->pending_cfg_w = 0;
    win->pending_cfg_h = 0;
    if (cw < 1) cw = 1;
    if (ch < 1) ch = 1;

    /* Initial map configure runs synchronously during _XmWlSyncWindowRole's roundtrip */
    if (!win->xdg_configured) {
        win->pending_configure_serial = serial;
        win->xdg_configured = true;
        bool size_changed = (cw != (int)win->width || ch != (int)win->height);
        if (size_changed) {
            int old_w = win->width;
            int old_h = win->height;
            win->width = (uint16_t)cw;
            win->height = (uint16_t)ch;
            _XmWlEnsureTopCanvas(srv, win);
            _XmWlDeliverConfigureAndExpose(srv, win, cw, ch, old_w, old_h);
        } else {
            _XmWlEnsureTopCanvas(srv, win);
            _XmWlDamageTop(win, 0, 0, win->width, win->height);
        }
        return;
    }

    /* Coalesce subsequent configures so only the latest size/serial is retained */
    win->has_queued_wl_cfg = true;
    win->queued_wl_serial = serial;
    win->queued_wl_w = cw;
    win->queued_wl_h = ch;
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
    /* Synthesize an outside click away from all popup items to pop down active Motif menu shells cleanly */
    if (srv->ptr_grab_active) {
        uint32_t ts = _XmWlNowMs();
        srv->pointer_top = NULL;
        srv->ptr_surf_x = -500;
        srv->ptr_surf_y = -500;
        srv->ptr_root_x = -500;
        srv->ptr_root_y = -500;
        _XmWlDispatchPointerMotion(srv, win, -500, -500, ts);
        _XmWlDispatchPointerButton(srv, 1, true, ts);
        _XmWlDispatchPointerButton(srv, 1, false, ts);
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
    if (!win || !srv || scale_120 == 0 || srv->compositor_native_dpi) return;
    double sc = (double)scale_120 / 120.0;
    if (sc < 0.5) sc = 1.0;
    if (sc > 4.0) sc = 4.0;
    win->preferred_scale = sc;
    srv->active_scale = sc;
    _XmWlDamageTop(win, 0, 0, win->width, win->height);
    _XmWlBroadcastRandRChange(srv);
}

static const struct wp_fractional_scale_v1_listener g_frac_scale_listener = {
    .preferred_scale = frac_scale_preferred,
};

static void
emit_top_configure_notify(XmWlServer *srv, XmWlWindow *win)
{
    if (!srv || !win) return;
    uint8_t cfg[32];
    memset(cfg, 0, sizeof(cfg));
    cfg[0] = 22; /* ConfigureNotify */
    uint32_t wid = win->id;
    uint32_t sib = win->prev_sibling ? win->prev_sibling->id : 0;
    memcpy(cfg + 4, &wid, 4);
    memcpy(cfg + 8, &wid, 4);
    memcpy(cfg + 12, &sib, 4);
    memcpy(cfg + 16, &win->x, 2);
    memcpy(cfg + 18, &win->y, 2);
    memcpy(cfg + 20, &win->width, 2);
    memcpy(cfg + 22, &win->height, 2);
    memcpy(cfg + 24, &win->border_width, 2);
    cfg[26] = win->override_redirect ? 1 : 0;
    _XmWlSendEventToWindow(srv, win, 1u << 17 /* StructureNotifyMask */, cfg);
}

static void
sync_win_single_output_transition(XmWlServer *srv, XmWlWindow *win)
{
    if (!srv || !win || win->is_popup || win->is_drag_icon || win->wl_output_count != 1)
        return;

    struct wl_output *output = win->wl_outputs[0];
    const XmWlOutputInfo *oinfo = NULL;
    for (int i = 0; i < srv->output_count; ++i) {
        if (srv->outputs[i].wl_out == output) {
            oinfo = &srv->outputs[i];
            break;
        }
    }
    if (!oinfo)
        return;

    if (!srv->compositor_native_dpi) {
        if (!srv->frac_scale_mgr && oinfo->scale > 0) {
            double sc = (double)oinfo->scale;
            if (sc < 0.5) sc = 1.0;
            if (sc > 4.0) sc = 4.0;
            if (fabs(win->preferred_scale - sc) > 1e-4) {
                win->preferred_scale = sc;
                srv->active_scale = sc;
                _XmWlDamageTop(win, 0, 0, win->width, win->height);
                _XmWlBroadcastRandRChange(srv);
            }
        }
        return;
    }

    if (win->has_authoritative_pos || !oinfo->name[0])
        return;

    for (int i = 0; i < srv->ipc_output_count; ++i) {
        const XmWlIpcOutput *io = &srv->ipc_outputs[i];
        if (!io->enabled || strcmp(io->name, oinfo->name) != 0)
            continue;
        int cx = (int)win->x + (int)win->width / 2;
        int cy = (int)win->y + (int)win->height / 2;
        if (cx < io->x || cx >= io->x + io->width ||
            cy < io->y || cy >= io->y + io->height) {
            win->x = (int16_t)(io->x + 32);
            win->y = (int16_t)(io->y + 32);
            emit_top_configure_notify(srv, win);
        }
        break;
    }
}

static void
surf_enter(void *data, struct wl_surface *wl_surface, struct wl_output *output)
{
    (void)wl_surface;
    XmWlWindow *win = (XmWlWindow *)data;
    XmWlServer *srv = g_wl_srv;
    if (!win || !srv || !output) return;
    bool present = false;
    for (int i = 0; i < win->wl_output_count; ++i) {
        if (win->wl_outputs[i] == output) {
            present = true;
            break;
        }
    }
    if (!present && win->wl_output_count < XM_WL_MAX_OUTPUTS)
        win->wl_outputs[win->wl_output_count++] = output;
    win->current_wl_output = output;
    sync_win_single_output_transition(srv, win);
}

static void
surf_leave(void *data, struct wl_surface *wl_surface, struct wl_output *output)
{
    (void)wl_surface;
    XmWlWindow *win = (XmWlWindow *)data;
    XmWlServer *srv = g_wl_srv;
    if (!win || !output) return;
    for (int i = 0; i < win->wl_output_count; ++i) {
        if (win->wl_outputs[i] == output) {
            for (int j = i; j + 1 < win->wl_output_count; ++j)
                win->wl_outputs[j] = win->wl_outputs[j + 1];
            win->wl_outputs[--win->wl_output_count] = NULL;
            break;
        }
    }
    if (win->current_wl_output == output) {
        win->current_wl_output =
            (win->wl_output_count > 0) ? win->wl_outputs[win->wl_output_count - 1] : NULL;
    }
    if (srv)
        sync_win_single_output_transition(srv, win);
}

static const struct wl_surface_listener g_wl_surface_listener = {
    .enter = surf_enter,
    .leave = surf_leave,
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

XmWlCursor *
_XmWlFindCursor(XmWlServer *srv, uint32_t id)
{
    if (!srv || id == 0) return NULL;
    for (XmWlCursor *c = srv->cursor_buckets[id & 127u]; c; c = c->hash_next) {
        if (c->id == id)
            return c;
    }
    return NULL;
}

void
_XmWlFreeCursor(XmWlServer *srv, uint32_t id)
{
    if (!srv || id == 0) return;
    uint32_t b = id & 127u;
    XmWlCursor **pp = &srv->cursor_buckets[b];
    while (*pp) {
        if ((*pp)->id == id) {
            XmWlCursor *cur = *pp;
            *pp = cur->hash_next;
            free_shm_buf(&cur->shm_buf);
            free(cur->argb_pixels);
            free(cur);
            if (srv->last_applied_cursor_id == id) {
                srv->last_applied_cursor_id = UINT32_MAX;
                _XmWlUpdatePointerCursor(srv, true);
            }
            return;
        }
        pp = &(*pp)->hash_next;
    }
}

void
_XmWlRegisterGlyphCursor(XmWlServer *srv, uint32_t id, uint16_t glyph_char)
{
    if (!srv || id == 0) return;
    _XmWlFreeCursor(srv, id);
    XmWlCursor *cur = (XmWlCursor *)calloc(1, sizeof(*cur));
    if (!cur) return;
    cur->id = id;
    cur->is_glyph = true;
    cur->glyph_char = glyph_char;
    cur->shm_buf.fd = -1;
    uint32_t b = id & 127u;
    cur->hash_next = srv->cursor_buckets[b];
    srv->cursor_buckets[b] = cur;
}

void
_XmWlRegisterBitmapCursor(XmWlServer *srv, uint32_t id, uint16_t w, uint16_t h,
                          int16_t hot_x, int16_t hot_y, const uint32_t *argb)
{
    if (!srv || id == 0 || w == 0 || h == 0 || !argb) return;
    _XmWlFreeCursor(srv, id);
    XmWlCursor *cur = (XmWlCursor *)calloc(1, sizeof(*cur));
    if (!cur) return;
    size_t sz = (size_t)w * (size_t)h * sizeof(uint32_t);
    cur->argb_pixels = (uint32_t *)malloc(sz);
    if (!cur->argb_pixels) {
        free(cur);
        return;
    }
    memcpy(cur->argb_pixels, argb, sz);
    cur->id = id;
    cur->is_glyph = false;
    cur->width = w;
    cur->height = h;
    cur->hot_x = hot_x;
    cur->hot_y = hot_y;
    cur->shm_buf.fd = -1;
    uint32_t b = id & 127u;
    cur->hash_next = srv->cursor_buckets[b];
    srv->cursor_buckets[b] = cur;
}

static uint32_t
xc_glyph_to_wl_cursor_shape(uint16_t glyph, const char **out_xcursor_name)
{
    /* Standard <X11/cursorfont.h> XC_* even-numbered source glyph codes */
    switch (glyph & ~1u) {
    case 152: /* XC_xterm */
        if (out_xcursor_name) *out_xcursor_name = "xterm";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_TEXT;
    case 58:  /* XC_hand1 */
    case 60:  /* XC_hand2 */
        if (out_xcursor_name) *out_xcursor_name = "hand2";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_POINTER;
    case 26:  /* XC_clock */
    case 150: /* XC_watch */
        if (out_xcursor_name) *out_xcursor_name = "watch";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_WAIT;
    case 92:  /* XC_question_arrow */
        if (out_xcursor_name) *out_xcursor_name = "question_arrow";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_HELP;
    case 30:  /* XC_cross */
    case 32:  /* XC_cross_reverse */
    case 34:  /* XC_crosshair */
    case 130: /* XC_tcross */
        if (out_xcursor_name) *out_xcursor_name = "crosshair";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_CROSSHAIR;
    case 52:  /* XC_fleur */
        if (out_xcursor_name) *out_xcursor_name = "fleur";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_MOVE;
    case 88:  /* XC_pirate */
        if (out_xcursor_name) *out_xcursor_name = "pirate";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NOT_ALLOWED;
    case 40:  /* XC_draped_box */
    case 90:  /* XC_plus */
        if (out_xcursor_name) *out_xcursor_name = "plus";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_CELL;
    case 44:  /* XC_double_arrow */
    case 108: /* XC_sb_h_double_arrow */
        if (out_xcursor_name) *out_xcursor_name = "sb_h_double_arrow";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_EW_RESIZE;
    case 116: /* XC_sb_v_double_arrow */
        if (out_xcursor_name) *out_xcursor_name = "sb_v_double_arrow";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NS_RESIZE;
    case 70:  /* XC_left_side */
        if (out_xcursor_name) *out_xcursor_name = "left_side";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_W_RESIZE;
    case 96:  /* XC_right_side */
        if (out_xcursor_name) *out_xcursor_name = "right_side";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_E_RESIZE;
    case 138: /* XC_top_side */
        if (out_xcursor_name) *out_xcursor_name = "top_side";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_N_RESIZE;
    case 16:  /* XC_bottom_side */
        if (out_xcursor_name) *out_xcursor_name = "bottom_side";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_S_RESIZE;
    case 134: /* XC_top_left_corner */
        if (out_xcursor_name) *out_xcursor_name = "top_left_corner";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NW_RESIZE;
    case 136: /* XC_top_right_corner */
        if (out_xcursor_name) *out_xcursor_name = "top_right_corner";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NE_RESIZE;
    case 12:  /* XC_bottom_left_corner */
        if (out_xcursor_name) *out_xcursor_name = "bottom_left_corner";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_SW_RESIZE;
    case 14:  /* XC_bottom_right_corner */
    case 120: /* XC_sizing */
        if (out_xcursor_name) *out_xcursor_name = "bottom_right_corner";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_SE_RESIZE;
    default:
        if (out_xcursor_name) *out_xcursor_name = "left_ptr";
        return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT;
    }
}

void
_XmWlUpdatePointerCursor(XmWlServer *srv, bool force)
{
    if (!srv || !srv->pointer || !srv->pointer_top || srv->last_ptr_enter_serial == 0)
        return;

    uint32_t eff_cid = 0;
    if (srv->ptr_grab_active && srv->ptr_grab_cursor_id != 0) {
        eff_cid = srv->ptr_grab_cursor_id;
    } else {
        for (XmWlWindow *cw = srv->hovered_win; cw && cw != srv->root; cw = cw->parent) {
            if (cw->cursor_id != 0) {
                eff_cid = cw->cursor_id;
                break;
            }
        }
    }

    if (!force && eff_cid == srv->last_applied_cursor_id)
        return;

    XmWlCursor *xcur = _XmWlFindCursor(srv, eff_cid);
    if (xcur && !xcur->is_glyph && xcur->argb_pixels && xcur->width > 0 && xcur->height > 0 &&
        srv->cursor_surface) {
        if (ensure_shm_buf(srv, &xcur->shm_buf, xcur->width, xcur->height, true)) {
            for (int y = 0; y < xcur->height; ++y) {
                memcpy((uint8_t *)xcur->shm_buf.pixels + (size_t)y * (size_t)xcur->shm_buf.stride,
                       xcur->argb_pixels + (size_t)y * (size_t)xcur->width,
                       (size_t)xcur->width * 4u);
            }
            wl_pointer_set_cursor(srv->pointer, srv->last_ptr_enter_serial, srv->cursor_surface,
                                  xcur->hot_x, xcur->hot_y);
            wl_surface_attach(srv->cursor_surface, xcur->shm_buf.wl_buf, 0, 0);
            wl_surface_damage_buffer(srv->cursor_surface, 0, 0, xcur->width, xcur->height);
            wl_surface_commit(srv->cursor_surface);
            srv->last_applied_cursor_id = eff_cid;
            srv->last_applied_cursor_shape = 0;
            wl_display_flush(srv->wl_dpy);
            return;
        }
    }

    const char *xc_name = "left_ptr";
    uint32_t shape = WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT;
    if (xcur && xcur->is_glyph) {
        shape = xc_glyph_to_wl_cursor_shape(xcur->glyph_char, &xc_name);
    }

    if (srv->cursor_shape_dev) {
        if (force || shape != srv->last_applied_cursor_shape) {
            wp_cursor_shape_device_v1_set_shape(
                srv->cursor_shape_dev, srv->last_ptr_enter_serial, shape);
            srv->last_applied_cursor_shape = shape;
            wl_display_flush(srv->wl_dpy);
        }
        srv->last_applied_cursor_id = eff_cid;
        return;
    }

    if (srv->cursor_theme && srv->cursor_surface) {
        struct wl_cursor *cur = wl_cursor_theme_get_cursor(srv->cursor_theme, xc_name);
        if (!cur) cur = wl_cursor_theme_get_cursor(srv->cursor_theme, "left_ptr");
        if (!cur) cur = wl_cursor_theme_get_cursor(srv->cursor_theme, "default");
        if (cur && cur->image_count > 0) {
            struct wl_cursor_image *img = cur->images[0];
            struct wl_buffer *cbuf = wl_cursor_image_get_buffer(img);
            if (cbuf) {
                wl_pointer_set_cursor(srv->pointer, srv->last_ptr_enter_serial, srv->cursor_surface,
                                      (int32_t)img->hotspot_x, (int32_t)img->hotspot_y);
                wl_surface_attach(srv->cursor_surface, cbuf, 0, 0);
                wl_surface_damage(srv->cursor_surface, 0, 0, (int32_t)img->width, (int32_t)img->height);
                wl_surface_commit(srv->cursor_surface);
                wl_display_flush(srv->wl_dpy);
            }
        }
    }
    srv->last_applied_cursor_id = eff_cid;
}

static void
pointer_enter(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
              struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy)
{
    (void)wl_pointer;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    srv->last_ptr_enter_serial = serial;
    XmWlWindow *top = find_top_by_wl_surface(srv, surface);
    srv->pointer_top = top;

    if (top) {
        int16_t ix = (int16_t)wl_fixed_to_int(sx);
        int16_t iy = (int16_t)wl_fixed_to_int(sy);
        _XmWlDispatchPointerMotion(srv, top, ix, iy, _XmWlNowMs());
    }
    _XmWlUpdatePointerCursor(srv, true);
}

static void
reset_wheel_axis_state(XmWlServer *srv, int ax)
{
    if (!srv || ax < 0 || ax > 1) return;
    srv->wheel_frame_px[ax] = 0.0;
    srv->wheel_frame_v120[ax] = 0;
    srv->wheel_frame_has_px[ax] = false;
    srv->wheel_frame_has_v120[ax] = false;
    srv->wheel_accum_v120[ax] = 0;
    srv->wheel_gesture_active[ax] = false;
    srv->wheel_last_dir[ax] = 0;
    srv->wheel_last_ms[ax] = 0;
}

static void
emit_wheel_tick(XmWlServer *srv, uint8_t xbtn, uint64_t now_ms)
{
    uint32_t btn_time = srv->wheel_frame_time_ms ? srv->wheel_frame_time_ms : (uint32_t)now_ms;
    if (srv->last_button_time_ms != 0 &&
        (int32_t)(btn_time - srv->last_button_time_ms) <= 0) {
        btn_time = srv->last_button_time_ms + 1u;
    }
    srv->last_button_time_ms = btn_time;
    srv->server_time_ms = btn_time;
    _XmWlDispatchPointerButton(srv, xbtn, true, btn_time);

    btn_time += 1u;
    srv->last_button_time_ms = btn_time;
    srv->server_time_ms = btn_time;
    _XmWlDispatchPointerButton(srv, xbtn, false, btn_time);
}

static void
emit_discrete_wheel_v120(XmWlServer *srv, int ax, int32_t step_v120, uint64_t now_ms)
{
    if (!srv || ax < 0 || ax > 1 || step_v120 == 0)
        return;

    int dir = (step_v120 < 0) ? -1 : 1;
    if (srv->wheel_last_ms[ax] == 0 ||
        (now_ms - srv->wheel_last_ms[ax]) > 180ULL ||
        (srv->wheel_last_dir[ax] != 0 && srv->wheel_last_dir[ax] != dir)) {
        srv->wheel_accum_v120[ax] = 0;
        srv->wheel_gesture_active[ax] = false;
    }
    srv->wheel_last_dir[ax] = dir;
    srv->wheel_last_ms[ax] = now_ms;
    srv->wheel_accum_v120[ax] += step_v120;

    uint8_t xbtn = (ax == 0) ? ((dir < 0) ? 4 : 5) : ((dir < 0) ? 6 : 7);
    int ticks = 0;
    bool is_full_detent = (step_v120 % 120 == 0) &&
                          (step_v120 >= 120 || step_v120 <= -120);
    int32_t stride_v120 = is_full_detent ? 120 : 60;

    if (!srv->wheel_gesture_active[ax]) {
        int32_t mag = srv->wheel_accum_v120[ax] < 0 ? -srv->wheel_accum_v120[ax] : srv->wheel_accum_v120[ax];
        int32_t lead_v120 = is_full_detent ? 120 : 12;
        if (mag >= lead_v120) {
            srv->wheel_gesture_active[ax] = true;
            ticks++;
            if (mag <= stride_v120)
                srv->wheel_accum_v120[ax] = 0;
            else
                srv->wheel_accum_v120[ax] -= dir * stride_v120;
        }
    }
    while (srv->wheel_gesture_active[ax] && ticks < 16) {
        int32_t mag = srv->wheel_accum_v120[ax] < 0 ? -srv->wheel_accum_v120[ax] : srv->wheel_accum_v120[ax];
        if (mag < stride_v120)
            break;
        ticks++;
        srv->wheel_accum_v120[ax] -= dir * stride_v120;
    }

    for (int i = 0; i < ticks; ++i)
        emit_wheel_tick(srv, xbtn, now_ms);
}

static void
flush_wheel_axis(XmWlServer *srv, int ax)
{
    if (!srv || ax < 0 || ax > 1) return;
    if (!srv->wheel_frame_has_v120[ax] && !srv->wheel_frame_has_px[ax]) return;

    double step_v120_f = 0.0;
    if (srv->wheel_frame_has_v120[ax] && srv->wheel_frame_v120[ax] != 0) {
        step_v120_f = (double)srv->wheel_frame_v120[ax];
    } else if (srv->wheel_frame_has_px[ax] && fabs(srv->wheel_frame_px[ax]) >= 0.01) {
        step_v120_f = srv->wheel_frame_px[ax] * (120.0 / 15.0);
    }

    srv->wheel_frame_px[ax] = 0.0;
    srv->wheel_frame_v120[ax] = 0;
    srv->wheel_frame_has_px[ax] = false;
    srv->wheel_frame_has_v120[ax] = false;

    if (fabs(step_v120_f) < 0.05) {
        srv->wheel_frame_time_ms = 0;
        return;
    }

    /* Native ~83 Hz smooth scrolling on the main Xt thread when an XmDisplay
     * is attached and no Ctrl/Alt/Super modifier or primary button drag is active.
     * Shift + vertical wheel routes to horizontal smooth scrolling. */
    if (srv->attached_xm_display != NULL && srv->xt_input_id != 0 &&
        (srv->mod_mask & (ControlMask | Mod1Mask | Mod4Mask)) == 0 &&
        (srv->button_mask & ((1u << 8) | (1u << 9) | (1u << 10))) == 0) {
        uint32_t xids[8];
        if (_XmWlHitTestWindowChain(srv, xids) > 0) {
            int eff_ax = (ax == 0 && (srv->mod_mask & ShiftMask) != 0) ? 1 : ax;
            memcpy(srv->smooth_xid_chain, xids, sizeof(xids));
            srv->smooth_pending_v120[eff_ax] += step_v120_f;
            srv->has_smooth_pending = true;
            srv->wheel_frame_time_ms = 0;
            _XmWaylandNotifyMainThread(srv);
            return;
        }
    }

    int32_t step_v120 = (int32_t)llround(step_v120_f);
    if (step_v120 == 0)
        step_v120 = (step_v120_f < 0.0) ? -1 : 1;
    emit_discrete_wheel_v120(srv, ax, step_v120, _XmWlNowMs());
    srv->wheel_frame_time_ms = 0;
}

static void
pointer_leave(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
              struct wl_surface *surface)
{
    (void)wl_pointer;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    reset_wheel_axis_state(srv, 0);
    reset_wheel_axis_state(srv, 1);
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
    srv->last_activation_serial = serial;
    srv->last_button_serial = serial;
    uint32_t btn_time = time ? time : _XmWlNowMs();
    if (srv->last_button_time_ms != 0 &&
        (int32_t)(btn_time - srv->last_button_time_ms) <= 0) {
        btn_time = srv->last_button_time_ms + 1u;
    }
    srv->last_button_time_ms = btn_time;
    srv->server_time_ms = btn_time;

    uint8_t xbtn = 1;
    if (button == BTN_LEFT) xbtn = 1;
    else if (button == BTN_MIDDLE) xbtn = 2;
    else if (button == BTN_RIGHT) xbtn = 3;
    else if (button == BTN_SIDE) xbtn = 8;
    else if (button == BTN_EXTRA) xbtn = 9;
    else return;

    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        srv->smooth_cancel_requested = true;
        srv->smooth_pending_v120[0] = 0.0;
        srv->smooth_pending_v120[1] = 0.0;
        _XmWaylandNotifyMainThread(srv);
    }

    _XmWlDispatchPointerButton(srv, xbtn, state == WL_POINTER_BUTTON_STATE_PRESSED, srv->server_time_ms);
}

static int
wheel_axis_index(uint32_t axis)
{
    if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) return 0;
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) return 1;
    return -1;
}

static void
pointer_axis(void *data, struct wl_pointer *wl_pointer, uint32_t time,
             uint32_t axis, wl_fixed_t value)
{
    XmWlServer *srv = (XmWlServer *)data;
    int ax = wheel_axis_index(axis);
    if (ax < 0) return;
    if (time) srv->wheel_frame_time_ms = time;
    srv->wheel_frame_px[ax] += wl_fixed_to_double(value);
    srv->wheel_frame_has_px[ax] = true;
    if (!wl_pointer || wl_pointer_get_version(wl_pointer) < WL_POINTER_FRAME_SINCE_VERSION)
        flush_wheel_axis(srv, ax);
}

static void
pointer_frame(void *data, struct wl_pointer *p)
{
    (void)p;
    XmWlServer *srv = (XmWlServer *)data;
    flush_wheel_axis(srv, 0);
    flush_wheel_axis(srv, 1);
}

static void pointer_axis_source(void *data, struct wl_pointer *p, uint32_t s) { (void)data; (void)p; (void)s; }

static void
pointer_axis_stop(void *data, struct wl_pointer *p, uint32_t t, uint32_t a)
{
    (void)p; (void)t;
    XmWlServer *srv = (XmWlServer *)data;
    int ax = wheel_axis_index(a);
    if (ax >= 0) {
        flush_wheel_axis(srv, ax);
        reset_wheel_axis_state(srv, ax);
    }
}

static void
pointer_axis_discrete(void *data, struct wl_pointer *p, uint32_t a, int32_t d)
{
    (void)p;
    XmWlServer *srv = (XmWlServer *)data;
    int ax = wheel_axis_index(a);
    if (ax >= 0 && d != 0 && !srv->wheel_frame_has_v120[ax]) {
        srv->wheel_frame_v120[ax] += d * 120;
        srv->wheel_frame_has_v120[ax] = true;
    }
}

static void
pointer_axis_value120(void *data, struct wl_pointer *p, uint32_t a, int32_t v)
{
    (void)p;
    XmWlServer *srv = (XmWlServer *)data;
    int ax = wheel_axis_index(a);
    if (ax >= 0 && v != 0) {
        srv->wheel_frame_v120[ax] += v;
        srv->wheel_frame_has_v120[ax] = true;
    }
}

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
    if (pressed)
        srv->last_activation_serial = serial;
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

static XmWlTouchPoint *
find_or_alloc_touch_pt(XmWlServer *srv, int32_t id, bool create)
{
    XmWlTouchPoint *free_slot = NULL;
    for (int i = 0; i < XM_WL_MAX_TOUCH_POINTS; ++i) {
        if (srv->touch_pts[i].active && srv->touch_pts[i].id == id)
            return &srv->touch_pts[i];
        if (!srv->touch_pts[i].active && !free_slot)
            free_slot = &srv->touch_pts[i];
    }
    if (create && free_slot) {
        memset(free_slot, 0, sizeof(*free_slot));
        free_slot->active = true;
        free_slot->id = id;
        srv->active_touch_count++;
        return free_slot;
    }
    return NULL;
}

static bool
compute_two_finger_centroid(const XmWlServer *srv, double *out_cx, double *out_cy)
{
    double sx = 0.0, sy = 0.0;
    int cnt = 0;
    for (int i = 0; i < XM_WL_MAX_TOUCH_POINTS && cnt < 2; ++i) {
        if (srv->touch_pts[i].active) {
            sx += srv->touch_pts[i].x;
            sy += srv->touch_pts[i].y;
            cnt++;
        }
    }
    if (cnt < 2)
        return false;
    *out_cx = sx * 0.5;
    *out_cy = sy * 0.5;
    return true;
}

static uint32_t
next_monotonic_btn_time(XmWlServer *srv, uint32_t time_ms)
{
    uint32_t btn_time = time_ms ? time_ms : _XmWlNowMs();
    if (srv->last_button_time_ms != 0 &&
        (int32_t)(btn_time - srv->last_button_time_ms) <= 0) {
        btn_time = srv->last_button_time_ms + 1u;
    }
    srv->last_button_time_ms = btn_time;
    srv->server_time_ms = btn_time;
    return btn_time;
}

static void
touch_down(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time,
           struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    (void)wl_touch;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    srv->last_activation_serial = serial;
    srv->last_button_serial = serial;

    XmWlTouchPoint *tp = find_or_alloc_touch_pt(srv, id, true);
    if (!tp) return;
    tp->x = wl_fixed_to_double(x);
    tp->y = wl_fixed_to_double(y);
    tp->surface = surface;

    XmWlWindow *top = find_top_by_wl_surface(srv, surface);
    if (top)
        srv->pointer_top = top;

    uint32_t t_ms = next_monotonic_btn_time(srv, time);
    if (srv->active_touch_count == 1 && srv->pointer_top) {
        srv->touch_two_finger_pan = false;
        srv->smooth_cancel_requested = true;
        srv->smooth_pending_v120[0] = 0.0;
        srv->smooth_pending_v120[1] = 0.0;
        _XmWaylandNotifyMainThread(srv);

        int16_t ix = (int16_t)lround(tp->x);
        int16_t iy = (int16_t)lround(tp->y);
        _XmWlDispatchPointerMotion(srv, srv->pointer_top, ix, iy, t_ms);
        _XmWlDispatchPointerButton(srv, 1, true, t_ms);
        srv->touch_btn1_down = true;
    } else if (srv->active_touch_count == 2) {
        if (srv->touch_btn1_down && srv->pointer_top) {
            /* Move outside armed widget bounds before releasing Button1 so starting a
             * 2-finger scroll gesture never accidentally activates a button underneath. */
            _XmWlDispatchPointerMotion(srv, srv->pointer_top, -16, -16, t_ms);
            uint32_t rel_t = next_monotonic_btn_time(srv, t_ms + 1u);
            _XmWlDispatchPointerButton(srv, 1, false, rel_t);
            srv->touch_btn1_down = false;
        }
        double cx = 0.0, cy = 0.0;
        if (compute_two_finger_centroid(srv, &cx, &cy)) {
            srv->touch_two_finger_pan = true;
            srv->touch_pan_last_cx = cx;
            srv->touch_pan_last_cy = cy;
            if (srv->pointer_top) {
                uint32_t pos_t = next_monotonic_btn_time(srv, t_ms + 1u);
                _XmWlDispatchPointerMotion(
                    srv, srv->pointer_top, (int16_t)lround(cx), (int16_t)lround(cy), pos_t);
            }
        }
    }
}

static void
touch_up(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
    (void)wl_touch;
    XmWlServer *srv = (XmWlServer *)data;
    srv->last_input_serial = serial;
    srv->last_activation_serial = serial;

    XmWlTouchPoint *tp = find_or_alloc_touch_pt(srv, id, false);
    if (tp) {
        tp->active = false;
        if (srv->active_touch_count > 0)
            srv->active_touch_count--;
    }

    uint32_t t_ms = next_monotonic_btn_time(srv, time);
    if (srv->active_touch_count == 0) {
        if (srv->touch_btn1_down) {
            _XmWlDispatchPointerButton(srv, 1, false, t_ms);
            srv->touch_btn1_down = false;
        }
        srv->touch_two_finger_pan = false;
        reset_wheel_axis_state(srv, 0);
        reset_wheel_axis_state(srv, 1);
    }
}

static void
touch_motion(void *data, struct wl_touch *wl_touch, uint32_t time,
             int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    (void)wl_touch;
    XmWlServer *srv = (XmWlServer *)data;
    XmWlTouchPoint *tp = find_or_alloc_touch_pt(srv, id, false);
    if (!tp) return;
    tp->x = wl_fixed_to_double(x);
    tp->y = wl_fixed_to_double(y);

    uint32_t t_ms = time ? time : _XmWlNowMs();
    srv->server_time_ms = t_ms;
    if (srv->active_touch_count == 1 && !srv->touch_two_finger_pan && srv->pointer_top) {
        _XmWlDispatchPointerMotion(
            srv, srv->pointer_top, (int16_t)lround(tp->x), (int16_t)lround(tp->y), t_ms);
    } else if (srv->active_touch_count >= 2 && srv->touch_two_finger_pan) {
        double cx = 0.0, cy = 0.0;
        if (compute_two_finger_centroid(srv, &cx, &cy)) {
            double dx = cx - srv->touch_pan_last_cx;
            double dy = cy - srv->touch_pan_last_cy;
            srv->touch_pan_last_cx = cx;
            srv->touch_pan_last_cy = cy;
            srv->wheel_frame_time_ms = t_ms;
            if (fabs(dy) >= 0.05) {
                srv->wheel_frame_px[0] += -dy;
                srv->wheel_frame_has_px[0] = true;
            }
            if (fabs(dx) >= 0.05) {
                srv->wheel_frame_px[1] += -dx;
                srv->wheel_frame_has_px[1] = true;
            }
        }
    }
}

static void
touch_frame(void *data, struct wl_touch *wl_touch)
{
    (void)wl_touch;
    XmWlServer *srv = (XmWlServer *)data;
    flush_wheel_axis(srv, 0);
    flush_wheel_axis(srv, 1);
}

static void
touch_cancel(void *data, struct wl_touch *wl_touch)
{
    (void)wl_touch;
    XmWlServer *srv = (XmWlServer *)data;
    for (int i = 0; i < XM_WL_MAX_TOUCH_POINTS; ++i)
        srv->touch_pts[i].active = false;
    srv->active_touch_count = 0;
    if (srv->touch_btn1_down) {
        uint32_t t_ms = next_monotonic_btn_time(srv, _XmWlNowMs());
        if (srv->pointer_top)
            _XmWlDispatchPointerMotion(srv, srv->pointer_top, -16, -16, t_ms);
        uint32_t rel_t = next_monotonic_btn_time(srv, t_ms + 1u);
        _XmWlDispatchPointerButton(srv, 1, false, rel_t);
        srv->touch_btn1_down = false;
    }
    srv->touch_two_finger_pan = false;
    reset_wheel_axis_state(srv, 0);
    reset_wheel_axis_state(srv, 1);
}

static void
touch_shape(void *data, struct wl_touch *wl_touch, int32_t id, wl_fixed_t major, wl_fixed_t minor)
{
    (void)data; (void)wl_touch; (void)id; (void)major; (void)minor;
}

static void
touch_orientation(void *data, struct wl_touch *wl_touch, int32_t id, wl_fixed_t orientation)
{
    (void)data; (void)wl_touch; (void)id; (void)orientation;
}

static const struct wl_touch_listener g_touch_listener = {
    .down = touch_down,
    .up = touch_up,
    .motion = touch_motion,
    .frame = touch_frame,
    .cancel = touch_cancel,
    .shape = touch_shape,
    .orientation = touch_orientation,
};

static void
sync_cursor_shape_device(XmWlServer *srv)
{
    if (!srv) return;
    if (srv->pointer && srv->cursor_shape_mgr && !srv->cursor_shape_dev) {
        srv->cursor_shape_dev = wp_cursor_shape_manager_v1_get_pointer(
            srv->cursor_shape_mgr, srv->pointer);
    } else if ((!srv->pointer || !srv->cursor_shape_mgr) && srv->cursor_shape_dev) {
        wp_cursor_shape_device_v1_destroy(srv->cursor_shape_dev);
        srv->cursor_shape_dev = NULL;
    }
}

static void
seat_capabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
    XmWlServer *srv = (XmWlServer *)data;
    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !srv->pointer) {
        srv->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(srv->pointer, &g_pointer_listener, srv);
        sync_cursor_shape_device(srv);
    } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && srv->pointer) {
        if (srv->cursor_shape_dev) {
            wp_cursor_shape_device_v1_destroy(srv->cursor_shape_dev);
            srv->cursor_shape_dev = NULL;
        }
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
    if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !srv->touch) {
        srv->touch = wl_seat_get_touch(seat);
        wl_touch_add_listener(srv->touch, &g_touch_listener, srv);
    } else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && srv->touch) {
        wl_touch_destroy(srv->touch);
        srv->touch = NULL;
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
    int max_w = 0, max_h = 0;
    if (srv->compositor_native_dpi && srv->ipc_output_count > 0) {
        for (int i = 0; i < srv->ipc_output_count; ++i) {
            const XmWlIpcOutput *io = &srv->ipc_outputs[i];
            if (!io->enabled) continue;
            if (io->x + io->width > max_w) max_w = io->x + io->width;
            if (io->y + io->height > max_h) max_h = io->y + io->height;
        }
    }
    if (max_w == 0 || max_h == 0) {
        for (int i = 0; i < srv->output_count; ++i) {
            const XmWlOutputInfo *o = &srv->outputs[i];
            int sc = (!srv->compositor_native_dpi && o->scale > 0) ? o->scale : 1;
            int lw = o->width > 0 ? (o->width / sc) : 1280;
            int lh = o->height > 0 ? (o->height / sc) : 800;
            if (o->x + lw > max_w) max_w = o->x + lw;
            if (o->y + lh > max_h) max_h = o->y + lh;
        }
    }
    if (max_w < 320) max_w = 1280;
    if (max_h < 240) max_h = 800;
    double eff_dpi = (srv->compositor_native_dpi && srv->primary_dpi >= 48)
                         ? (double)srv->primary_dpi
                         : (96.0 * (srv->logical_zoom > 0.0 ? srv->logical_zoom : 1.0));
    srv->screen_w = max_w;
    srv->screen_h = max_h;
    srv->screen_mm_w = (int)lround((double)max_w * 25.4 / eff_dpi);
    srv->screen_mm_h = (int)lround((double)max_h * 25.4 / eff_dpi);
    if (srv->screen_mm_w < 1) srv->screen_mm_w = 1;
    if (srv->screen_mm_h < 1) srv->screen_mm_h = 1;
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
        if (!g_wl_srv->compositor_native_dpi && o->scale > 0 && !g_wl_srv->frac_scale_mgr) {
            bool any_bound_elsewhere = false;
            if (g_wl_srv->root) {
                for (XmWlWindow *tw = g_wl_srv->root->first_child; tw; tw = tw->next_sibling) {
                    if (tw->wl_output_count == 1 && tw->wl_outputs[0] == o->wl_out) {
                        tw->preferred_scale = (double)o->scale;
                        g_wl_srv->active_scale = (double)o->scale;
                        _XmWlDamageTop(tw, 0, 0, tw->width, tw->height);
                    } else if (tw->wl_output_count == 0) {
                        tw->preferred_scale = (double)o->scale;
                        _XmWlDamageTop(tw, 0, 0, tw->width, tw->height);
                    } else {
                        any_bound_elsewhere = true;
                    }
                }
            }
            if (!any_bound_elsewhere)
                g_wl_srv->active_scale = (double)o->scale;
        }
        _XmWlBroadcastRandRChange(g_wl_srv);
    }
}

static void
output_scale(void *data, struct wl_output *wl_output, int32_t factor)
{
    (void)wl_output;
    XmWlOutputInfo *o = (XmWlOutputInfo *)data;
    o->scale = factor > 0 ? factor : 1;
}

static void
output_name(void *d, struct wl_output *wl_out, const char *n)
{
    (void)wl_out;
    XmWlOutputInfo *o = (XmWlOutputInfo *)d;
    if (o)
        snprintf(o->name, sizeof(o->name), "%s", n ? n : "");
}
static void output_desc(void *d, struct wl_output *o, const char *s) { (void)d; (void)o; (void)s; }

static const struct wl_output_listener g_output_listener = {
    .geometry = output_geometry,
    .mode = output_mode,
    .done = output_done,
    .scale = output_scale,
    .name = output_name,
    .description = output_desc,
};

#ifdef HAVE_WAYLAND_DMABUF
#define XM_WL_DRM_FORMAT_MOD_INVALID UINT64_C(0x00ffffffffffffff)

static void
dmabuf_append_format(XmWlDmabufFormatEntry **arr, size_t *cnt, size_t *cap,
                     uint32_t format, uint64_t modifier)
{
    if (!arr || !cnt || !cap)
        return;
    for (size_t i = 0; i < *cnt; ++i) {
        if ((*arr)[i].format == format && (*arr)[i].modifier == modifier)
            return;
    }
    if (*cnt >= 4096u)
        return;
    if (*cnt >= *cap) {
        size_t ncap = (*cap == 0) ? 32u : (*cap * 2u);
        if (ncap > 4096u)
            ncap = 4096u;
        XmWlDmabufFormatEntry *next = (XmWlDmabufFormatEntry *)realloc(
            *arr, ncap * sizeof(XmWlDmabufFormatEntry));
        if (!next)
            return;
        *arr = next;
        *cap = ncap;
    }
    (*arr)[*cnt].format = format;
    (*arr)[*cnt].pad = 0;
    (*arr)[*cnt].modifier = modifier;
    (*cnt)++;
}

static void
dmabuf_handle_format(void *data, struct zwp_linux_dmabuf_v1 *dmabuf, uint32_t format)
{
    (void)dmabuf;
    XmWlServer *srv = (XmWlServer *)data;
    if (srv && srv->dmabuf_version < 3) {
        dmabuf_append_format(&srv->dmabuf_formats, &srv->dmabuf_format_count,
                             &srv->dmabuf_format_cap, format, XM_WL_DRM_FORMAT_MOD_INVALID);
    }
}

static void
dmabuf_handle_modifier(void *data, struct zwp_linux_dmabuf_v1 *dmabuf,
                       uint32_t format, uint32_t mod_hi, uint32_t mod_lo)
{
    (void)dmabuf;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv)
        return;
    uint64_t mod = ((uint64_t)mod_hi << 32) | (uint64_t)mod_lo;
    dmabuf_append_format(&srv->dmabuf_formats, &srv->dmabuf_format_count,
                         &srv->dmabuf_format_cap, format, mod);
}

static const struct zwp_linux_dmabuf_v1_listener g_dmabuf_listener = {
    .format = dmabuf_handle_format,
    .modifier = dmabuf_handle_modifier,
};

#ifdef ZWP_LINUX_DMABUF_V1_GET_DEFAULT_FEEDBACK_SINCE_VERSION
static void
dmabuf_fb_done(void *data, struct zwp_linux_dmabuf_feedback_v1 *fb)
{
    (void)fb;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv)
        return;
    if (srv->dmabuf_staging_count > 0) {
        free(srv->dmabuf_formats);
        srv->dmabuf_formats = srv->dmabuf_staging;
        srv->dmabuf_format_count = srv->dmabuf_staging_count;
        srv->dmabuf_format_cap = srv->dmabuf_staging_cap;
        srv->dmabuf_staging = NULL;
        srv->dmabuf_staging_count = 0;
        srv->dmabuf_staging_cap = 0;
    }
}

static void
dmabuf_fb_format_table(void *data, struct zwp_linux_dmabuf_feedback_v1 *fb,
                       int32_t fd, uint32_t size)
{
    (void)fb;
    XmWlServer *srv = (XmWlServer *)data;
    if (fd < 0)
        return;
    if (!srv || size < sizeof(XmWlDmabufFormatEntry) ||
        (size % sizeof(XmWlDmabufFormatEntry)) != 0 ||
        size > 16384u * sizeof(XmWlDmabufFormatEntry)) {
        close(fd);
        return;
    }
    void *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (map == MAP_FAILED)
        return;
    size_t count = size / sizeof(XmWlDmabufFormatEntry);
    XmWlDmabufFormatEntry *copy = (XmWlDmabufFormatEntry *)malloc(size);
    if (copy) {
        memcpy(copy, map, size);
        free(srv->dmabuf_table);
        srv->dmabuf_table = copy;
        srv->dmabuf_table_count = count;
    }
    munmap(map, size);
}

static void
dmabuf_fb_main_device(void *data, struct zwp_linux_dmabuf_feedback_v1 *fb, struct wl_array *dev)
{
    (void)data; (void)fb; (void)dev;
}

static void
dmabuf_fb_tranche_done(void *data, struct zwp_linux_dmabuf_feedback_v1 *fb)
{
    (void)data; (void)fb;
}

static void
dmabuf_fb_tranche_target_device(void *data, struct zwp_linux_dmabuf_feedback_v1 *fb, struct wl_array *dev)
{
    (void)data; (void)fb; (void)dev;
}

static void
dmabuf_fb_tranche_formats(void *data, struct zwp_linux_dmabuf_feedback_v1 *fb, struct wl_array *indices)
{
    (void)fb;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || !indices || !srv->dmabuf_table || srv->dmabuf_table_count == 0)
        return;
    const uint16_t *idxs = (const uint16_t *)indices->data;
    size_t n = indices->size / sizeof(uint16_t);
    for (size_t i = 0; i < n; ++i) {
        uint16_t idx = idxs[i];
        if ((size_t)idx < srv->dmabuf_table_count) {
            dmabuf_append_format(&srv->dmabuf_staging, &srv->dmabuf_staging_count,
                                 &srv->dmabuf_staging_cap,
                                 srv->dmabuf_table[idx].format,
                                 srv->dmabuf_table[idx].modifier);
        }
    }
}

static void
dmabuf_fb_tranche_flags(void *data, struct zwp_linux_dmabuf_feedback_v1 *fb, uint32_t flags)
{
    (void)data; (void)fb; (void)flags;
}

static const struct zwp_linux_dmabuf_feedback_v1_listener g_dmabuf_feedback_listener = {
    .done = dmabuf_fb_done,
    .format_table = dmabuf_fb_format_table,
    .main_device = dmabuf_fb_main_device,
    .tranche_done = dmabuf_fb_tranche_done,
    .tranche_target_device = dmabuf_fb_tranche_target_device,
    .tranche_formats = dmabuf_fb_tranche_formats,
    .tranche_flags = dmabuf_fb_tranche_flags,
};
#endif
#endif /* HAVE_WAYLAND_DMABUF */

static void
icon_mgr_icon_size(void *data, struct xdg_toplevel_icon_manager_v1 *mgr, int32_t size)
{
    (void)mgr;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || size <= 0 || size > 512)
        return;
    for (int i = 0; i < srv->icon_pref_count; ++i) {
        if (srv->icon_pref_sizes[i] == size)
            return;
    }
    if (srv->icon_pref_count < 8)
        srv->icon_pref_sizes[srv->icon_pref_count++] = size;
}

static void
icon_mgr_done(void *data, struct xdg_toplevel_icon_manager_v1 *mgr)
{
    (void)data;
    (void)mgr;
}

static const struct xdg_toplevel_icon_manager_v1_listener g_icon_mgr_listener = {
    .icon_size = icon_mgr_icon_size,
    .done = icon_mgr_done,
};

#ifdef HAVE_WAYLAND_COLOR_MANAGEMENT
static void
cm_mgr_supported_intent(void *data, struct wp_color_manager_v1 *mgr, uint32_t render_intent)
{
    (void)data;
    (void)mgr;
    (void)render_intent;
}

static void
cm_mgr_supported_feature(void *data, struct wp_color_manager_v1 *mgr, uint32_t feature)
{
    (void)mgr;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv)
        return;
    if (feature == WP_COLOR_MANAGER_V1_FEATURE_PARAMETRIC)
        srv->cm_feat_parametric = true;
    else if (feature == WP_COLOR_MANAGER_V1_FEATURE_SET_MASTERING_DISPLAY_PRIMARIES)
        srv->cm_feat_mastering = true;
}

static void
cm_mgr_supported_tf_named(void *data, struct wp_color_manager_v1 *mgr, uint32_t tf)
{
    (void)mgr;
    XmWlServer *srv = (XmWlServer *)data;
    if (srv && tf < 32u)
        srv->cm_tf_mask |= (1u << tf);
}

static void
cm_mgr_supported_primaries_named(void *data, struct wp_color_manager_v1 *mgr, uint32_t primaries)
{
    (void)mgr;
    XmWlServer *srv = (XmWlServer *)data;
    if (srv && primaries < 32u)
        srv->cm_primaries_mask |= (1u << primaries);
}

static void
cm_mgr_done(void *data, struct wp_color_manager_v1 *mgr)
{
    (void)data;
    (void)mgr;
}

static const struct wp_color_manager_v1_listener g_cm_mgr_listener = {
    .supported_intent = cm_mgr_supported_intent,
    .supported_feature = cm_mgr_supported_feature,
    .supported_tf_named = cm_mgr_supported_tf_named,
    .supported_primaries_named = cm_mgr_supported_primaries_named,
    .done = cm_mgr_done,
};

static void
cm_img_desc_failed(void *data, struct wp_image_description_v1 *desc,
                   uint32_t cause, const char *msg)
{
    (void)cause;
    (void)msg;
    XmWlSurfaceExt *ext = (XmWlSurfaceExt *)data;
    if (ext && ext->cm_pending_desc == desc)
        ext->cm_pending_desc = NULL;
    wp_image_description_v1_destroy(desc);
}

static void
cm_img_desc_ready(void *data, struct wp_image_description_v1 *desc, uint32_t identity)
{
    (void)identity;
    XmWlSurfaceExt *ext = (XmWlSurfaceExt *)data;
    if (!ext || ext->cm_pending_desc != desc || !ext->cm_surf) {
        wp_image_description_v1_destroy(desc);
        return;
    }
    ext->cm_pending_desc = NULL;
    if (ext->cm_desc)
        wp_image_description_v1_destroy(ext->cm_desc);
    ext->cm_desc = desc;
    ext->cm_desc_applied = true;
    wp_color_management_surface_v1_set_image_description(
        ext->cm_surf, desc, WP_COLOR_MANAGER_V1_RENDER_INTENT_PERCEPTUAL);
    if (ext->cm_srv && ext->surf) {
        XmWlWindow *top = find_top_by_wl_surface(ext->cm_srv, ext->surf);
        if (!top || (top->xdg_configured && !top->resize_awaiting_client && !top->dirty)) {
            wl_surface_commit(ext->surf);
            wl_display_flush(ext->cm_srv->wl_dpy);
        }
    }
}

static const struct wp_image_description_v1_listener g_cm_img_desc_listener = {
    .failed = cm_img_desc_failed,
    .ready = cm_img_desc_ready,
};
#endif /* HAVE_WAYLAND_COLOR_MANAGEMENT */

static XmWlSurfaceExt *
find_or_create_surface_ext(XmWlServer *srv, struct wl_surface *surf, bool create)
{
    if (!srv || !surf)
        return NULL;
    for (XmWlSurfaceExt *ext = srv->surf_exts; ext; ext = ext->next) {
        if (ext->surf == surf)
            return ext;
    }
    if (!create)
        return NULL;
    XmWlSurfaceExt *ext = (XmWlSurfaceExt *)calloc(1, sizeof(*ext));
    if (!ext)
        return NULL;
    ext->surf = surf;
    ext->next = srv->surf_exts;
    srv->surf_exts = ext;
    return ext;
}

static void
release_surface_ext(XmWlServer *srv, struct wl_surface *surf)
{
    if (!srv || !surf)
        return;
    XmWlSurfaceExt **pp = &srv->surf_exts;
    while (*pp) {
        XmWlSurfaceExt *cur = *pp;
        if (cur->surf == surf) {
            *pp = cur->next;
#ifdef HAVE_WAYLAND_TEARING_CONTROL
            if (cur->tearing)
                wp_tearing_control_v1_destroy(cur->tearing);
#endif
#ifdef HAVE_WAYLAND_FIFO
            if (cur->fifo)
                wp_fifo_v1_destroy(cur->fifo);
#endif
#ifdef HAVE_WAYLAND_COLOR_MANAGEMENT
            if (cur->cm_pending_desc)
                wp_image_description_v1_destroy(cur->cm_pending_desc);
            if (cur->cm_desc)
                wp_image_description_v1_destroy(cur->cm_desc);
            if (cur->cm_surf)
                wp_color_management_surface_v1_destroy(cur->cm_surf);
#endif
            free(cur);
            return;
        }
        pp = &cur->next;
    }
}

static bool
apply_surface_tearing_locked(XmWlServer *srv, struct wl_surface *surf, bool async_tearing)
{
#ifdef HAVE_WAYLAND_TEARING_CONTROL
    if (!srv || !surf || !srv->tearing_mgr)
        return false;
    XmWlSurfaceExt *ext = find_or_create_surface_ext(srv, surf, true);
    if (!ext)
        return false;
    if (!ext->tearing) {
        ext->tearing = wp_tearing_control_manager_v1_get_tearing_control(srv->tearing_mgr, surf);
        if (!ext->tearing)
            return false;
    }
    wp_tearing_control_v1_set_presentation_hint(
        ext->tearing,
        async_tearing ? WP_TEARING_CONTROL_V1_PRESENTATION_HINT_ASYNC
                      : WP_TEARING_CONTROL_V1_PRESENTATION_HINT_VSYNC);
    return true;
#else
    (void)srv; (void)surf; (void)async_tearing;
    return false;
#endif
}

static bool
apply_surface_fifo_locked(XmWlServer *srv, struct wl_surface *surf, bool set_barrier, bool wait_barrier)
{
#ifdef HAVE_WAYLAND_FIFO
    if (!srv || !surf || !srv->fifo_mgr)
        return false;
    XmWlSurfaceExt *ext = find_or_create_surface_ext(srv, surf, true);
    if (!ext)
        return false;
    if (!ext->fifo) {
        ext->fifo = wp_fifo_manager_v1_get_fifo(srv->fifo_mgr, surf);
        if (!ext->fifo)
            return false;
    }
    if (set_barrier)
        wp_fifo_v1_set_barrier(ext->fifo);
    if (wait_barrier)
        wp_fifo_v1_wait_barrier(ext->fifo);
    return true;
#else
    (void)srv; (void)surf; (void)set_barrier; (void)wait_barrier;
    return false;
#endif
}

static bool
apply_surface_hdr_locked(XmWlServer *srv, struct wl_surface *surf, const XmWaylandHdrMetadata *meta)
{
#ifdef HAVE_WAYLAND_COLOR_MANAGEMENT
    if (!srv || !surf || !srv->color_mgr)
        return false;
    if (!meta) {
        XmWlSurfaceExt *ext = find_or_create_surface_ext(srv, surf, false);
        if (!ext || !ext->cm_surf)
            return true;
        if (ext->cm_pending_desc) {
            wp_image_description_v1_destroy(ext->cm_pending_desc);
            ext->cm_pending_desc = NULL;
        }
        if (ext->cm_desc) {
            wp_image_description_v1_destroy(ext->cm_desc);
            ext->cm_desc = NULL;
        }
        if (ext->has_hdr_meta || ext->cm_desc_applied)
            wp_color_management_surface_v1_unset_image_description(ext->cm_surf);
        memset(&ext->hdr_meta, 0, sizeof(ext->hdr_meta));
        ext->has_hdr_meta = false;
        ext->cm_desc_applied = false;
        return true;
    }

    uint32_t wl_tf = 0;
    switch (meta->transfer_function) {
    case XM_WAYLAND_COLOR_TF_SRGB:      wl_tf = WP_COLOR_MANAGER_V1_TRANSFER_FUNCTION_SRGB; break;
    case XM_WAYLAND_COLOR_TF_BT1886:    wl_tf = WP_COLOR_MANAGER_V1_TRANSFER_FUNCTION_BT1886; break;
    case XM_WAYLAND_COLOR_TF_ST2084_PQ: wl_tf = WP_COLOR_MANAGER_V1_TRANSFER_FUNCTION_ST2084_PQ; break;
    case XM_WAYLAND_COLOR_TF_HLG:       wl_tf = WP_COLOR_MANAGER_V1_TRANSFER_FUNCTION_HLG; break;
    default: return false;
    }

    uint32_t wl_prim = 0;
    switch (meta->primaries) {
    case XM_WAYLAND_COLOR_PRIMARIES_SRGB:       wl_prim = WP_COLOR_MANAGER_V1_PRIMARIES_SRGB; break;
    case XM_WAYLAND_COLOR_PRIMARIES_DISPLAY_P3: wl_prim = WP_COLOR_MANAGER_V1_PRIMARIES_DISPLAY_P3; break;
    case XM_WAYLAND_COLOR_PRIMARIES_BT2020:     wl_prim = WP_COLOR_MANAGER_V1_PRIMARIES_BT2020; break;
    default: return false;
    }

    if (srv->cm_tf_mask != 0 && !(srv->cm_tf_mask & (1u << wl_tf)))
        return false;
    if (srv->cm_primaries_mask != 0 && !(srv->cm_primaries_mask & (1u << wl_prim)))
        return false;
    if (!srv->cm_feat_parametric && srv->cm_tf_mask != 0)
        return false;

    XmWlSurfaceExt *ext = find_or_create_surface_ext(srv, surf, true);
    if (!ext)
        return false;
    ext->cm_srv = srv;
    if (ext->has_hdr_meta && (ext->cm_desc_applied || ext->cm_pending_desc != NULL) &&
        memcmp(&ext->hdr_meta, meta, sizeof(*meta)) == 0) {
        return true;
    }
    if (!ext->cm_surf) {
        ext->cm_surf = wp_color_manager_v1_get_surface(srv->color_mgr, surf);
        if (!ext->cm_surf)
            return false;
    }
    if (ext->cm_pending_desc) {
        wp_image_description_v1_destroy(ext->cm_pending_desc);
        ext->cm_pending_desc = NULL;
    }

    struct wp_image_description_creator_params_v1 *creator =
        wp_color_manager_v1_create_parametric_creator(srv->color_mgr);
    if (!creator)
        return false;

    wp_image_description_creator_params_v1_set_tf_named(creator, wl_tf);
    wp_image_description_creator_params_v1_set_primaries_named(creator, wl_prim);
    if (srv->cm_feat_mastering) {
        if (meta->has_mastering_display_primaries) {
            wp_image_description_creator_params_v1_set_mastering_display_primaries(
                creator,
                meta->display_primaries_rx, meta->display_primaries_ry,
                meta->display_primaries_gx, meta->display_primaries_gy,
                meta->display_primaries_bx, meta->display_primaries_by,
                meta->white_point_x, meta->white_point_y);
        }
        if (meta->has_mastering_luminance && meta->max_luminance > 0) {
            wp_image_description_creator_params_v1_set_mastering_luminance(
                creator, meta->min_luminance_10000, meta->max_luminance);
        }
    }
    if (meta->max_cll > 0)
        wp_image_description_creator_params_v1_set_max_cll(creator, meta->max_cll);
    if (meta->max_fall > 0)
        wp_image_description_creator_params_v1_set_max_fall(creator, meta->max_fall);

    struct wp_image_description_v1 *desc =
        wp_image_description_creator_params_v1_create(creator);
    if (!desc)
        return false;
    ext->hdr_meta = *meta;
    ext->has_hdr_meta = true;
    ext->cm_desc_applied = false;
    ext->cm_pending_desc = desc;
    wp_image_description_v1_add_listener(desc, &g_cm_img_desc_listener, ext);
    wl_display_flush(srv->wl_dpy);
    return true;
#else
    (void)srv; (void)surf; (void)meta;
    return false;
#endif
}

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
        if (srv->icon_mgr)
            xdg_toplevel_icon_manager_v1_add_listener(srv->icon_mgr, &g_icon_mgr_listener, srv);
    } else if (!strcmp(interface, xdg_activation_v1_interface.name)) {
        srv->activation_mgr = (struct xdg_activation_v1 *)wl_registry_bind(
            registry, name, &xdg_activation_v1_interface, 1);
    } else if (!strcmp(interface, wp_cursor_shape_manager_v1_interface.name)) {
        srv->cursor_shape_mgr = (struct wp_cursor_shape_manager_v1 *)wl_registry_bind(
            registry, name, &wp_cursor_shape_manager_v1_interface, 1);
        sync_cursor_shape_device(srv);
    } else if (!strcmp(interface, wl_seat_interface.name)) {
        if (!srv->seat) {
            uint32_t max_v = (uint32_t)wl_seat_interface.version;
            if (max_v > 9u) max_v = 9u;
            uint32_t v = version < max_v ? version : max_v;
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
#ifdef HAVE_WAYLAND_DMABUF
    } else if (!strcmp(interface, zwp_linux_dmabuf_v1_interface.name)) {
        if (!srv->dmabuf && version >= 2) {
            uint32_t max_v = (uint32_t)zwp_linux_dmabuf_v1_interface.version;
            if (max_v > 4u) max_v = 4u;
            uint32_t v = version < max_v ? version : max_v;
            srv->dmabuf = (struct zwp_linux_dmabuf_v1 *)wl_registry_bind(
                registry, name, &zwp_linux_dmabuf_v1_interface, v);
            srv->dmabuf_version = v;
            if (srv->dmabuf) {
                if (v < 4u) {
                    zwp_linux_dmabuf_v1_add_listener(srv->dmabuf, &g_dmabuf_listener, srv);
                }
#ifdef ZWP_LINUX_DMABUF_V1_GET_DEFAULT_FEEDBACK_SINCE_VERSION
                else {
                    srv->dmabuf_feedback = zwp_linux_dmabuf_v1_get_default_feedback(srv->dmabuf);
                    if (srv->dmabuf_feedback) {
                        zwp_linux_dmabuf_feedback_v1_add_listener(
                            srv->dmabuf_feedback, &g_dmabuf_feedback_listener, srv);
                    }
                }
#endif
            }
        }
#endif
#ifdef HAVE_WAYLAND_XDG_DIALOG
    } else if (!strcmp(interface, xdg_wm_dialog_v1_interface.name)) {
        if (!srv->xdg_dialog_mgr) {
            srv->xdg_dialog_mgr = (struct xdg_wm_dialog_v1 *)wl_registry_bind(
                registry, name, &xdg_wm_dialog_v1_interface, 1);
        }
#endif
#ifdef HAVE_WAYLAND_TEARING_CONTROL
    } else if (!strcmp(interface, wp_tearing_control_manager_v1_interface.name)) {
        if (!srv->tearing_mgr) {
            srv->tearing_mgr = (struct wp_tearing_control_manager_v1 *)wl_registry_bind(
                registry, name, &wp_tearing_control_manager_v1_interface, 1);
        }
#endif
#ifdef HAVE_WAYLAND_FIFO
    } else if (!strcmp(interface, wp_fifo_manager_v1_interface.name)) {
        if (!srv->fifo_mgr) {
            srv->fifo_mgr = (struct wp_fifo_manager_v1 *)wl_registry_bind(
                registry, name, &wp_fifo_manager_v1_interface, 1);
        }
#endif
#ifdef HAVE_WAYLAND_COLOR_MANAGEMENT
    } else if (!strcmp(interface, wp_color_manager_v1_interface.name)) {
        if (!srv->color_mgr) {
            srv->color_mgr = (struct wp_color_manager_v1 *)wl_registry_bind(
                registry, name, &wp_color_manager_v1_interface, 1);
            if (srv->color_mgr)
                wp_color_manager_v1_add_listener(srv->color_mgr, &g_cm_mgr_listener, srv);
        }
#endif
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

/* ---------- xdg_activation_v1 Token Minting & Consumption ---------- */

struct XmWlPendingTokenCtx {
    XmWlServer *srv;
    struct xdg_activation_token_v1 *tok_obj;
    uint64_t req_seq;
    uint32_t target_xid;
    bool is_sync_req;
    struct XmWlPendingTokenCtx *next;
};

static void
unlink_pending_token_ctx(XmWlServer *srv, XmWlPendingTokenCtx *ctx)
{
    if (!srv || !ctx)
        return;
    XmWlPendingTokenCtx **pp = &srv->pending_act_tokens;
    while (*pp) {
        if (*pp == ctx) {
            *pp = ctx->next;
            break;
        }
        pp = &(*pp)->next;
    }
}

static void
activation_token_done(void *data, struct xdg_activation_token_v1 *tok, const char *token)
{
    XmWlPendingTokenCtx *ctx = (XmWlPendingTokenCtx *)data;
    if (!ctx) {
        if (tok)
            xdg_activation_token_v1_destroy(tok);
        return;
    }
    XmWlServer *srv = ctx->srv;
    if (srv) {
        unlink_pending_token_ctx(srv, ctx);
        if (ctx->is_sync_req) {
            pthread_mutex_lock(&srv->act_lock);
            if (srv->act_req_in_flight && !srv->act_req_done && ctx->req_seq == srv->act_req_seq) {
                if (token && token[0])
                    snprintf(srv->act_req_result, sizeof(srv->act_req_result), "%s", token);
                else
                    srv->act_req_result[0] = '\0';
                srv->act_req_done = true;
                pthread_cond_broadcast(&srv->act_cond);
            }
            pthread_mutex_unlock(&srv->act_lock);
        } else if (ctx->target_xid != 0 && token && token[0] && srv->activation_mgr) {
            XmWlWindow *tw = _XmWlFindWindow(srv, ctx->target_xid);
            int dx = 0, dy = 0;
            XmWlWindow *top = tw ? _XmWlEnclosingTopLevel(srv, tw, &dx, &dy) : NULL;
            if (top && top->mapped && top->wl_surf) {
                if (top->xdg_configured && !top->dirty) {
                    xdg_activation_v1_activate(srv->activation_mgr, token, top->wl_surf);
                    wl_display_flush(srv->wl_dpy);
                } else {
                    snprintf(top->pending_activation_token, sizeof(top->pending_activation_token),
                             "%s", token);
                }
            }
        }
    }
    if (tok)
        xdg_activation_token_v1_destroy(tok);
    free(ctx);
}

static const struct xdg_activation_token_v1_listener g_activation_token_listener = {
    .done = activation_token_done,
};

static void
submit_activation_token_on_server(XmWlServer *srv, uint32_t source_xid, const char *app_id,
                                  bool is_sync_req, uint64_t req_seq, uint32_t target_xid)
{
    if (!srv || !srv->activation_mgr || !srv->wl_dpy)
        return;
    struct xdg_activation_token_v1 *tok =
        xdg_activation_v1_get_activation_token(srv->activation_mgr);
    if (!tok)
        return;

    XmWlPendingTokenCtx *ctx = (XmWlPendingTokenCtx *)calloc(1, sizeof(*ctx));
    if (!ctx) {
        xdg_activation_token_v1_destroy(tok);
        return;
    }
    ctx->srv = srv;
    ctx->tok_obj = tok;
    ctx->req_seq = req_seq;
    ctx->target_xid = target_xid;
    ctx->is_sync_req = is_sync_req;
    ctx->next = srv->pending_act_tokens;
    srv->pending_act_tokens = ctx;

    xdg_activation_token_v1_add_listener(tok, &g_activation_token_listener, ctx);

    XmWlWindow *src_top = NULL;
    if (source_xid != 0) {
        XmWlWindow *sw = _XmWlFindWindow(srv, source_xid);
        int dx = 0, dy = 0;
        src_top = sw ? _XmWlEnclosingTopLevel(srv, sw, &dx, &dy) : NULL;
    }
    if (!src_top || !src_top->wl_surf ||
        (src_top != srv->wl_focus_top && src_top != srv->pointer_top)) {
        if (srv->wl_focus_top && srv->wl_focus_top->wl_surf)
            src_top = srv->wl_focus_top;
        else if (srv->pointer_top && srv->pointer_top->wl_surf)
            src_top = srv->pointer_top;
    }

    bool has_seat_focus =
        (src_top != NULL && src_top->wl_surf != NULL &&
         (src_top == srv->wl_focus_top || src_top == srv->pointer_top));
    if (has_seat_focus) {
        if (srv->seat && srv->last_activation_serial != 0)
            xdg_activation_token_v1_set_serial(tok, srv->last_activation_serial, srv->seat);
        xdg_activation_token_v1_set_surface(tok, src_top->wl_surf);
    }
    if (app_id && app_id[0])
        xdg_activation_token_v1_set_app_id(tok, app_id);

    xdg_activation_token_v1_commit(tok);
    wl_display_flush(srv->wl_dpy);
}

static void
process_pending_activation_requests(XmWlServer *srv)
{
    if (!srv)
        return;
    bool submit = false;
    uint64_t seq = 0;
    uint32_t src_xid = 0;
    char app_id[128] = {0};

    pthread_mutex_lock(&srv->act_lock);
    if (srv->act_req_in_flight && !srv->act_req_submitted) {
        srv->act_req_submitted = true;
        if (!srv->activation_mgr) {
            srv->act_req_result[0] = '\0';
            srv->act_req_done = true;
            pthread_cond_broadcast(&srv->act_cond);
        } else {
            submit = true;
            seq = srv->act_req_seq;
            src_xid = srv->act_req_source_xid;
            snprintf(app_id, sizeof(app_id), "%s", srv->act_req_app_id);
        }
    }
    pthread_mutex_unlock(&srv->act_lock);

    if (submit)
        submit_activation_token_on_server(srv, src_xid, app_id, true, seq, 0);
}

void
_XmWlRequestSelfActivate(XmWlServer *srv, XmWlWindow *top)
{
    if (!srv || !top || !srv->activation_mgr || top->is_popup || top->is_drag_icon)
        return;
    const char *ext_tok = getenv("XDG_ACTIVATION_TOKEN");
    if (!ext_tok || !ext_tok[0])
        ext_tok = getenv("DESKTOP_STARTUP_ID");
    if (ext_tok && ext_tok[0]) {
        char tok_copy[256];
        snprintf(tok_copy, sizeof(tok_copy), "%s", ext_tok);
        unsetenv("XDG_ACTIVATION_TOKEN");
        unsetenv("DESKTOP_STARTUP_ID");
        if (top->mapped && top->wl_surf && top->xdg_configured && !top->dirty) {
            xdg_activation_v1_activate(srv->activation_mgr, tok_copy, top->wl_surf);
            wl_display_flush(srv->wl_dpy);
        } else {
            snprintf(top->pending_activation_token, sizeof(top->pending_activation_token),
                     "%s", tok_copy);
        }
        return;
    }
    submit_activation_token_on_server(
        srv, top->id, top->sent_app_id[0] ? top->sent_app_id : NULL,
        false, 0, top->id);
}

static uint64_t
fnv1a_update64(uint64_t h, const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t *)data;
    for (size_t i = 0; i < len; ++i) {
        h ^= (uint64_t)p[i];
        h *= UINT64_C(1099511628211);
    }
    return h;
}

static bool
is_valid_xdg_icon_name(const char *s, size_t len, char out_name[128])
{
    if (!s || len == 0)
        return false;
    size_t n = strnlen(s, len);
    if (n == 0 || n >= 128)
        return false;
    /* XDG icon theme names are non-path tokens of [A-Za-z0-9._-] */
    if (s[0] == '.' || s[0] == '-')
        return false;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)s[i];
        if (!(isalnum(c) || c == '-' || c == '_' || c == '.'))
            return false;
    }
    memcpy(out_name, s, n);
    out_name[n] = '\0';
    return true;
}

static uint32_t *
extract_net_wm_icon_square(const uint8_t *raw, size_t byte_len, int target_side,
                           int min_side, int *out_side)
{
    if (!raw || byte_len < 12 || !out_side)
        return NULL;
    size_t nwords = byte_len / 4u;
    size_t best_off = 0;
    uint32_t best_w = 0, best_h = 0;
    int best_dist = 0x7fffffff;

    size_t pos = 0;
    while (pos + 2 <= nwords) {
        uint32_t fw = 0, fh = 0;
        memcpy(&fw, raw + pos * 4u, 4);
        memcpy(&fh, raw + (pos + 1u) * 4u, 4);
        if (fw == 0 || fh == 0 || fw > 512u || fh > 512u)
            break;
        size_t px_cnt = (size_t)fw * (size_t)fh;
        if (pos + 2u + px_cnt > nwords)
            break;
        int side = (int)((fw > fh) ? fw : fh);
        if (side >= min_side) {
            int dist = side >= target_side ? (side - target_side) : (target_side - side) * 2;
            if (dist < best_dist) {
                best_dist = dist;
                best_off = pos + 2u;
                best_w = fw;
                best_h = fh;
            }
        }
        pos += 2u + px_cnt;
    }

    if (best_w == 0 || best_h == 0)
        return NULL;

    int side = (int)((best_w > best_h) ? best_w : best_h);
    int ox = (side - (int)best_w) / 2;
    int oy = (side - (int)best_h) / 2;
    uint32_t *argb = (uint32_t *)calloc((size_t)side * (size_t)side, 4u);
    if (!argb)
        return NULL;

    const uint8_t *src = raw + best_off * 4u;
    for (uint32_t r = 0; r < best_h; ++r) {
        for (uint32_t c = 0; c < best_w; ++c) {
            uint32_t px = 0;
            memcpy(&px, src + ((size_t)r * (size_t)best_w + (size_t)c) * 4u, 4);
            uint32_t a = (px >> 24) & 0xffu;
            uint32_t rv = (px >> 16) & 0xffu;
            uint32_t gv = (px >> 8) & 0xffu;
            uint32_t bv = px & 0xffu;
            if (a < 255u) {
                rv = (rv * a + 127u) / 255u;
                gv = (gv * a + 127u) / 255u;
                bv = (bv * a + 127u) / 255u;
            }
            argb[(size_t)(oy + (int)r) * (size_t)side + (size_t)(ox + (int)c)] =
                (a << 24) | (rv << 16) | (gv << 8) | bv;
        }
    }
    *out_side = side;
    return argb;
}

static uint32_t *
upscale_icon_2x(const uint32_t *src, int side, int *out_side2x)
{
    if (!src || side <= 0 || side > 128 || !out_side2x)
        return NULL;
    int s2 = side * 2;
    uint32_t *dst = (uint32_t *)malloc((size_t)s2 * (size_t)s2 * 4u);
    if (!dst)
        return NULL;
    for (int r = 0; r < side; ++r) {
        const uint32_t *srow = src + (size_t)r * (size_t)side;
        uint32_t *drow0 = dst + (size_t)(r * 2) * (size_t)s2;
        uint32_t *drow1 = dst + (size_t)(r * 2 + 1) * (size_t)s2;
        for (int c = 0; c < side; ++c) {
            uint32_t px = srow[c];
            drow0[c * 2] = px;
            drow0[c * 2 + 1] = px;
            drow1[c * 2] = px;
            drow1[c * 2 + 1] = px;
        }
    }
    *out_side2x = s2;
    return dst;
}

static bool
sync_toplevel_icon_locked(XmWlServer *srv, XmWlWindow *win, uint32_t changed_atom)
{
    if (!srv || !win || !win->xdg_top || !srv->icon_mgr || !srv->shm)
        return false;

    uint32_t a_wm_hints = 35u;     /* XA_WM_HINTS */
    uint32_t a_net_icon = _XmWlInternAtom(srv, "_NET_WM_ICON", 12, true);
    uint32_t a_xdg_iname = _XmWlInternAtom(srv, "_MOTIF_XDG_ICON_NAME", 20, true);

    if (changed_atom != 0 &&
        changed_atom != a_wm_hints &&
        (a_net_icon == 0 || changed_atom != a_net_icon) &&
        (a_xdg_iname == 0 || changed_atom != a_xdg_iname)) {
        return false;
    }

    int pref_1x = (srv->icon_pref_count > 0 && srv->icon_pref_sizes[0] >= 16)
                      ? srv->icon_pref_sizes[0]
                      : 48;

    uint32_t *argb_1x = NULL;
    int side_1x = 0;
    uint32_t *argb_2x = NULL;
    int side_2x = 0;

    XmWlProperty *net_ip = a_net_icon ? _XmWlGetProperty(win, a_net_icon) : NULL;
    if (net_ip && net_ip->format == 32 && net_ip->data && net_ip->byte_len >= 12) {
        argb_1x = extract_net_wm_icon_square(net_ip->data, net_ip->byte_len, pref_1x, 1, &side_1x);
        if (argb_1x && side_1x > 0) {
            argb_2x = extract_net_wm_icon_square(
                net_ip->data, net_ip->byte_len, side_1x * 2, side_1x + 1, &side_2x);
        }
    }

    if (!argb_1x) {
        XmWlProperty *hp = _XmWlGetProperty(win, a_wm_hints);
        if (hp && hp->data && hp->byte_len >= 16) {
            uint32_t hflags = 0, icon_pix = 0, icon_mask = 0;
            memcpy(&hflags, hp->data + 0, 4);
            memcpy(&icon_pix, hp->data + 12, 4);
            if ((hflags & 32u) && hp->byte_len >= 32)
                memcpy(&icon_mask, hp->data + 28, 4);
            if ((hflags & 4u) && icon_pix != 0) {
                int iw = 0, ih = 0;
                if (_XmWlBuildIconPixels(srv, icon_pix, (hflags & 32u) ? icon_mask : 0,
                                         &argb_1x, &iw, &ih)) {
                    side_1x = iw;
                }
            }
        }
    }

    if (argb_1x && !argb_2x && side_1x > 0 && side_1x <= 128)
        argb_2x = upscale_icon_2x(argb_1x, side_1x, &side_2x);

    char icon_name[128] = {0};
    XmWlProperty *xp = a_xdg_iname ? _XmWlGetProperty(win, a_xdg_iname) : NULL;
    if (xp && xp->data)
        (void)is_valid_xdg_icon_name((const char *)xp->data, xp->byte_len, icon_name);

    if (!argb_1x && !icon_name[0]) {
        free(argb_2x);
        if (win->has_sent_icon) {
            xdg_toplevel_icon_manager_v1_set_icon(srv->icon_mgr, win->xdg_top, NULL);
            win->has_sent_icon = false;
            win->sent_icon_hash = 0;
            return true;
        }
        return false;
    }

    uint64_t h = UINT64_C(14695981039346656037);
    if (icon_name[0])
        h = fnv1a_update64(h, icon_name, strlen(icon_name) + 1u);
    if (argb_1x && side_1x > 0) {
        h = fnv1a_update64(h, &side_1x, sizeof(side_1x));
        h = fnv1a_update64(h, argb_1x, (size_t)side_1x * (size_t)side_1x * 4u);
    }
    if (argb_2x && side_2x > 0) {
        h = fnv1a_update64(h, &side_2x, sizeof(side_2x));
        h = fnv1a_update64(h, argb_2x, (size_t)side_2x * (size_t)side_2x * 4u);
    }

    if (win->has_sent_icon && win->sent_icon_hash == h) {
        free(argb_1x);
        free(argb_2x);
        return false;
    }

    bool applied = false;
    size_t sz1 = (argb_1x && side_1x > 0) ? ((size_t)side_1x * (size_t)side_1x * 4u) : 0u;
    size_t sz2 = (argb_2x && side_2x > 0) ? ((size_t)side_2x * (size_t)side_2x * 4u) : 0u;
    size_t total_sz = sz1 + sz2;

    if (total_sz > 0) {
        int ifd = create_shm_fd(total_sz);
        if (ifd >= 0) {
            uint8_t *imap = (uint8_t *)mmap(NULL, total_sz, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0);
            if (imap != MAP_FAILED) {
                if (sz1 > 0)
                    memcpy(imap, argb_1x, sz1);
                if (sz2 > 0)
                    memcpy(imap + sz1, argb_2x, sz2);
                struct wl_shm_pool *ipool = wl_shm_create_pool(srv->shm, ifd, (int32_t)total_sz);
                struct wl_buffer *ibuf1 = sz1 > 0
                    ? wl_shm_pool_create_buffer(ipool, 0, side_1x, side_1x, side_1x * 4, WL_SHM_FORMAT_ARGB8888)
                    : NULL;
                struct wl_buffer *ibuf2 = sz2 > 0
                    ? wl_shm_pool_create_buffer(ipool, (int32_t)sz1, side_2x, side_2x, side_2x * 4, WL_SHM_FORMAT_ARGB8888)
                    : NULL;
                wl_shm_pool_destroy(ipool);

                struct xdg_toplevel_icon_v1 *ico =
                    xdg_toplevel_icon_manager_v1_create_icon(srv->icon_mgr);
                if (icon_name[0])
                    xdg_toplevel_icon_v1_set_name(ico, icon_name);
                if (ibuf1)
                    xdg_toplevel_icon_v1_add_buffer(ico, ibuf1, 1);
                if (ibuf2)
                    xdg_toplevel_icon_v1_add_buffer(ico, ibuf2, 2);
                xdg_toplevel_icon_manager_v1_set_icon(srv->icon_mgr, win->xdg_top, ico);
                xdg_toplevel_icon_v1_destroy(ico);
                if (ibuf2)
                    wl_buffer_destroy(ibuf2);
                if (ibuf1)
                    wl_buffer_destroy(ibuf1);
                munmap(imap, total_sz);
                applied = true;
            }
            close(ifd);
        }
    } else if (icon_name[0]) {
        struct xdg_toplevel_icon_v1 *ico =
            xdg_toplevel_icon_manager_v1_create_icon(srv->icon_mgr);
        xdg_toplevel_icon_v1_set_name(ico, icon_name);
        xdg_toplevel_icon_manager_v1_set_icon(srv->icon_mgr, win->xdg_top, ico);
        xdg_toplevel_icon_v1_destroy(ico);
        applied = true;
    }

    free(argb_1x);
    free(argb_2x);
    if (applied) {
        win->has_sent_icon = true;
        win->sent_icon_hash = h;
    }
    return applied;
}

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
            release_surface_ext(srv, pw->wl_surf);
            wl_surface_destroy(pw->wl_surf);
            pw->wl_surf = NULL;
        }
        pw->surf_listener_added = false;
        pw->current_wl_output = NULL;
        pw->xdg_configured = false;
        pw->is_popup = false;
        pw->popup_parent = NULL;
    }
    if (idx < srv->popup_depth)
        srv->popup_depth = idx;
}

static bool
sync_toplevel_metadata_all(XmWlServer *srv, XmWlWindow *win, uint32_t changed_atom)
{
    if (!win || !win->xdg_top) return false;
    bool changed = false;

    uint32_t a_net_name = _XmWlInternAtom(srv, "_NET_WM_NAME", 12, true);
    uint32_t a_wm_name = 39; /* XA_WM_NAME */
    if (changed_atom == 0 || changed_atom == a_wm_name ||
        (a_net_name != 0 && changed_atom == a_net_name)) {
        XmWlProperty *p = a_net_name ? _XmWlGetProperty(win, a_net_name) : NULL;
        if (!p || !p->data || p->byte_len == 0)
            p = _XmWlGetProperty(win, a_wm_name);
        if (p && p->data && p->byte_len > 0) {
            char title[256];
            size_t n = p->byte_len < sizeof(title) - 1 ? p->byte_len : sizeof(title) - 1;
            memcpy(title, p->data, n);
            title[n] = '\0';
            if (strcmp(title, win->sent_title) != 0) {
                memcpy(win->sent_title, title, n + 1);
                xdg_toplevel_set_title(win->xdg_top, title);
                changed = true;
            }
        }
    }

    uint32_t a_wm_class = 67; /* XA_WM_CLASS */
    if (changed_atom == 0 || changed_atom == a_wm_class) {
        XmWlProperty *p = _XmWlGetProperty(win, a_wm_class);
        if (p && p->data && p->byte_len > 0) {
            char app_id[128];
            size_t n = strnlen((const char *)p->data, p->byte_len);
            if (n >= sizeof(app_id)) n = sizeof(app_id) - 1;
            memcpy(app_id, p->data, n);
            app_id[n] = '\0';
            if (app_id[0] && strcmp(app_id, win->sent_app_id) != 0) {
                memcpy(win->sent_app_id, app_id, n + 1);
                xdg_toplevel_set_app_id(win->xdg_top, app_id);
                changed = true;
            }
        }
    }

    uint32_t a_transient = 68; /* XA_WM_TRANSIENT_FOR */
    if (changed_atom == 0 || changed_atom == a_transient) {
        XmWlProperty *p = _XmWlGetProperty(win, a_transient);
        uint32_t parent_xid = 0;
        if (p && p->data && p->byte_len >= 4)
            memcpy(&parent_xid, p->data, 4);
        if (parent_xid != win->sent_parent_xid) {
            if (parent_xid != 0) {
                XmWlWindow *pwin = _XmWlFindWindow(srv, parent_xid);
                XmWlWindow *ptop = pwin ? _XmWlEnclosingTopLevel(srv, pwin, NULL, NULL) : NULL;
                if (ptop && ptop != win && ptop->xdg_top) {
                    win->sent_parent_xid = parent_xid;
                    xdg_toplevel_set_parent(win->xdg_top, ptop->xdg_top);
                    changed = true;
                }
            } else if (win->sent_parent_xid != 0) {
                win->sent_parent_xid = 0;
                xdg_toplevel_set_parent(win->xdg_top, NULL);
                changed = true;
            }
        }
    }

    uint32_t a_normal = 40; /* XA_WM_NORMAL_HINTS */
    if (changed_atom == 0 || changed_atom == a_normal) {
        XmWlProperty *p = _XmWlGetProperty(win, a_normal);
        if (p && p->data && p->byte_len >= 9 * 4) {
            uint32_t flags = 0;
            int32_t min_w = 0, min_h = 0, max_w = 0, max_h = 0;
            memcpy(&flags, p->data + 0, 4);
            memcpy(&min_w, p->data + 5 * 4, 4);
            memcpy(&min_h, p->data + 6 * 4, 4);
            memcpy(&max_w, p->data + 7 * 4, 4);
            memcpy(&max_h, p->data + 8 * 4, 4);
            int32_t eff_min_w = ((flags & (1u << 4)) && min_w > 0 && min_h > 0) ? min_w : 0;
            int32_t eff_min_h = ((flags & (1u << 4)) && min_w > 0 && min_h > 0) ? min_h : 0;
            int32_t eff_max_w = ((flags & (1u << 5)) && max_w > 0 && max_h > 0) ? max_w : 0;
            int32_t eff_max_h = ((flags & (1u << 5)) && max_w > 0 && max_h > 0) ? max_h : 0;
            if (eff_min_w != win->sent_min_w || eff_min_h != win->sent_min_h) {
                win->sent_min_w = eff_min_w;
                win->sent_min_h = eff_min_h;
                xdg_toplevel_set_min_size(win->xdg_top, eff_min_w, eff_min_h);
                changed = true;
            }
            if (eff_max_w != win->sent_max_w || eff_max_h != win->sent_max_h) {
                win->sent_max_w = eff_max_w;
                win->sent_max_h = eff_max_h;
                xdg_toplevel_set_max_size(win->xdg_top, eff_max_w, eff_max_h);
                changed = true;
            }
        }
    }

    uint32_t a_mwm = _XmWlInternAtom(srv, "_MOTIF_WM_HINTS", 15, true);
    uint32_t a_net_state = _XmWlInternAtom(srv, "_NET_WM_STATE", 13, true);
    uint32_t a_net_modal = _XmWlInternAtom(srv, "_NET_WM_STATE_MODAL", 19, true);
    if (changed_atom == 0 ||
        (a_mwm != 0 && changed_atom == a_mwm) ||
        (a_net_state != 0 && changed_atom == a_net_state) ||
        changed_atom == a_transient) {
        XmWlProperty *p = a_mwm ? _XmWlGetProperty(win, a_mwm) : NULL;
        bool want_deco = true;
        bool want_modal = false;
        if (p && p->data && p->byte_len >= 12) {
            uint32_t mflags = 0, mdeco = 1;
            memcpy(&mflags, p->data + 0, 4);
            memcpy(&mdeco, p->data + 8, 4);
            if ((mflags & 2u) && mdeco == 0)
                want_deco = false;
            if ((mflags & 4u) && p->byte_len >= 16) {
                int32_t minput = 0;
                memcpy(&minput, p->data + 12, 4);
                if (minput == 1 || minput == 2 || minput == 3)
                    want_modal = true;
            }
        }
        if (!want_modal && a_net_state != 0 && a_net_modal != 0) {
            XmWlProperty *sp = _XmWlGetProperty(win, a_net_state);
            if (sp && sp->format == 32 && sp->data && sp->byte_len >= 4) {
                size_t natoms = sp->byte_len / 4;
                for (size_t ai = 0; ai < natoms; ++ai) {
                    uint32_t st_atom = 0;
                    memcpy(&st_atom, sp->data + ai * 4, 4);
                    if (st_atom == a_net_modal) {
                        want_modal = true;
                        break;
                    }
                }
            }
        }
        win->wants_decorations = want_deco;
        int8_t dmode = want_deco ? 1 : 0;
        if (win->xdg_deco && win->sent_deco_mode != dmode) {
            win->sent_deco_mode = dmode;
            zxdg_toplevel_decoration_v1_set_mode(
                win->xdg_deco,
                want_deco ? ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE
                          : ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
            changed = true;
        }
#ifdef HAVE_WAYLAND_XDG_DIALOG
        if (srv->xdg_dialog_mgr && (win->sent_parent_xid != 0 || want_modal || win->xdg_dialog != NULL)) {
            if (!win->xdg_dialog) {
                win->xdg_dialog = xdg_wm_dialog_v1_get_xdg_dialog(srv->xdg_dialog_mgr, win->xdg_top);
                changed = true;
            }
            if (win->xdg_dialog) {
                int8_t mmode = want_modal ? 1 : 0;
                if (win->sent_modal_mode != mmode) {
                    win->sent_modal_mode = mmode;
                    if (want_modal)
                        xdg_dialog_v1_set_modal(win->xdg_dialog);
                    else
                        xdg_dialog_v1_unset_modal(win->xdg_dialog);
                    changed = true;
                }
            }
        }
#endif
    }

    uint32_t a_tearing = _XmWlInternAtom(srv, "_MOTIF_WAYLAND_TEARING", 22, true);
    if (win->wl_surf && (changed_atom == 0 || (a_tearing != 0 && changed_atom == a_tearing))) {
        XmWlProperty *tp = a_tearing ? _XmWlGetProperty(win, a_tearing) : NULL;
        if (tp && tp->format == 32 && tp->data && tp->byte_len >= 4) {
            uint32_t tval = 0;
            memcpy(&tval, tp->data, 4);
            int8_t tmode = (tval != 0) ? 1 : 0;
            if (win->sent_tearing_mode != tmode &&
                apply_surface_tearing_locked(srv, win->wl_surf, tval != 0)) {
                win->sent_tearing_mode = tmode;
                changed = true;
            }
        }
    }

    uint32_t a_fifo = _XmWlInternAtom(srv, "_MOTIF_WAYLAND_FIFO", 19, true);
    if (changed_atom == 0 || (a_fifo != 0 && changed_atom == a_fifo)) {
        XmWlProperty *fp = a_fifo ? _XmWlGetProperty(win, a_fifo) : NULL;
        uint8_t fflags = 0;
        if (fp && fp->format == 32 && fp->data && fp->byte_len >= 4) {
            uint32_t fval = 0;
            memcpy(&fval, fp->data, 4);
            fflags = (uint8_t)(fval & 3u);
        }
        if (win->fifo_flags != fflags) {
            win->fifo_flags = fflags;
            if (fflags != 0 && win->wl_surf &&
                apply_surface_fifo_locked(srv, win->wl_surf,
                                          (fflags & 1u) != 0,
                                          (fflags & 2u) != 0)) {
                changed = true;
            }
        }
    }

    /* Dynamic window icon via xdg_toplevel_icon_v1 (_NET_WM_ICON, WM_HINTS, icon name, 1x+2x) */
    if (sync_toplevel_icon_locked(srv, win, changed_atom))
        changed = true;
    return changed;
}

static void
apply_xrdb_wheel_settings(XmWlServer *srv, const char *txt, size_t len)
{
    if (!srv || !txt || len == 0)
        return;
    const char *p = txt;
    const char *end = txt + len;
    while (p < end) {
        const char *nl = (const char *)memchr(p, '\n', (size_t)(end - p));
        size_t l = nl ? (size_t)(nl - p) : (size_t)(end - p);
        const char *colon = (const char *)memchr(p, ':', l);
        if (colon) {
            size_t klen = (size_t)(colon - p);
            while (klen > 0 && isspace((unsigned char)p[klen - 1]))
                klen--;
            const char *val = colon + 1;
            size_t vlen = (size_t)((p + l) - val);
            while (vlen > 0 && isspace((unsigned char)*val)) {
                val++;
                vlen--;
            }
            if ((klen >= 15 && !strncasecmp(p + klen - 15, "mouseWheelSpeed", 15)) && vlen > 0) {
                char nbuf[16];
                size_t cn = vlen < sizeof(nbuf) - 1 ? vlen : sizeof(nbuf) - 1;
                memcpy(nbuf, val, cn);
                nbuf[cn] = '\0';
                int sp = atoi(nbuf);
                if (sp >= 1 && sp <= 20) {
                    srv->wheel_speed_lines = sp;
                    _XmSetWheelScrollSpeed(sp);
                }
            } else if ((klen >= 16 && !strncasecmp(p + klen - 16, "mouseWheelSmooth", 16)) && vlen > 0) {
                bool sm = srv->wheel_smooth_enabled;
                if (*val == '1' || *val == 't' || *val == 'T' || *val == 'y' || *val == 'Y' ||
                    (vlen >= 2 && !strncasecmp(val, "on", 2)))
                    sm = true;
                else if (*val == '0' || *val == 'f' || *val == 'F' || *val == 'n' || *val == 'N' ||
                         (vlen >= 3 && !strncasecmp(val, "off", 3)))
                    sm = false;
                if (!sm && srv->wheel_smooth_enabled) {
                    srv->smooth_cancel_requested = true;
                    _XmWaylandNotifyMainThread(srv);
                }
                srv->wheel_smooth_enabled = sm;
            }
        }
        p = nl ? (nl + 1) : end;
    }
}

static void apply_wayland_outputs_ipc(XmWlServer *srv, const char *txt, size_t len);

void
_XmWlUpdateWindowMetadata(XmWlServer *srv, XmWlWindow *win, uint32_t changed_atom)
{
    if (!srv || !win) return;
    const char *aname = _XmWlAtomName(srv, changed_atom);
    XmWlProperty *rp = _XmWlGetProperty(win, changed_atom);

    if (win == srv->root && aname && rp && !strcmp(aname, "_DT_SM_WINDOW_INFO") &&
        rp->format == 32 && rp->data && rp->byte_len >= 8 && !srv->suppress_ipc_publish) {
        uint32_t sm_xid = 0;
        memcpy(&sm_xid, rp->data + 4, 4);
        if (sm_xid != 0 && sm_xid != XM_WL_SM_HELPER_XID)
            srv->local_sm_win_xid = sm_xid;
        return;
    }

    if (win == srv->root || win == srv->wm_helper || win == srv->sm_helper ||
        (srv->local_sm_win_xid != 0 && win->id == srv->local_sm_win_xid)) {
        if (aname && rp && (rp->format == 8 || rp->format == 32)) {
            if (win == srv->root && !strcmp(aname, "_MOTIF_WAYLAND_TEST_SCALE") &&
                rp->format == 8 && rp->data && rp->byte_len > 0) {
                char buf[32];
                size_t n = rp->byte_len < sizeof(buf) - 1 ? rp->byte_len : sizeof(buf) - 1;
                memcpy(buf, rp->data, n);
                buf[n] = '\0';
                double sc = strtod(buf, NULL);
                if (sc >= 0.5 && sc <= 4.0)
                    _XmWlApplyScaleUpdate(srv, sc);
            } else if (win == srv->root && !strcmp(aname, "_MOTIF_WAYLAND_TEST_OUTPUTS") &&
                       rp->format == 8 && rp->data && rp->byte_len > 0) {
                if (rp->byte_len >= 3 && !memcmp(rp->data, "off", 3)) {
                    srv->compositor_native_dpi = false;
                    srv->ipc_output_count = 0;
                    recompute_screen_metrics(srv);
                    _XmWlBroadcastRandRChange(srv);
                } else {
                    apply_wayland_outputs_ipc(srv, (const char *)rp->data, rp->byte_len);
                }
            } else if (!srv->suppress_ipc_publish && rp->data &&
                       strcmp(aname, "_DT_SM_WINDOW_INFO") != 0 &&
                       (!strncmp(aname, "RESOURCE_MANAGER", 16) ||
                        !strcmp(aname, "_SUN_TT_SESSION") ||
                        !strcmp(aname, "TT_SESSION") ||
                        !strcmp(aname, "SDT Pixel Set") ||
                        !strncmp(aname, "_DT_", 4) ||
                        !strncmp(aname, "_DTWM_", 6))) {
                _XmWlIpcPublishRootProp(srv, aname, rp->data, rp->byte_len);
                if (!strncmp(aname, "RESOURCE_MANAGER", 16))
                    apply_xrdb_wheel_settings(srv, (const char *)rp->data, rp->byte_len);
                /* Avoid unbounded local append growth on _DT_WM_REQUEST */
                if (!strcmp(aname, "_DT_WM_REQUEST"))
                    _XmWlDeleteProperty(srv, win, changed_atom, false);
            }
        }
        if (win == srv->root || win == srv->wm_helper || win == srv->sm_helper)
            return;
    }
    if (win->parent == srv->root && win->xdg_top) {
        if (sync_toplevel_metadata_all(srv, win, changed_atom)) {
            if (win->wl_surf && win->xdg_configured &&
                !win->resize_awaiting_client && !win->dirty) {
                wl_surface_commit(win->wl_surf);
            }
            wl_display_flush(srv->wl_dpy);
        }
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
    win->preferred_scale = srv->compositor_native_dpi
                               ? 1.0
                               : (srv->active_scale > 0.0 ? srv->active_scale : 1.0);
    win->wl_surf = wl_compositor_create_surface(srv->compositor);
    if (!win->wl_surf) return;
    wl_surface_add_listener(win->wl_surf, &g_wl_surface_listener, win);
    win->surf_listener_added = true;

    if (!srv->compositor_native_dpi) {
        if (srv->frac_scale_mgr) {
            win->frac_scale_obj = wp_fractional_scale_manager_v1_get_fractional_scale(
                srv->frac_scale_mgr, win->wl_surf);
            wp_fractional_scale_v1_add_listener(win->frac_scale_obj, &g_frac_scale_listener, win);
        }
        if (srv->viewporter) {
            win->viewport = wp_viewporter_get_viewport(srv->viewporter, win->wl_surf);
        }
    }

    /* If a Wayland drag is active and this is an override-redirect drag icon window */
    if (win->override_redirect && srv->wl_drag_source != NULL && srv->wl_drag_icon_surf != NULL) {
        win->is_drag_icon = true;
        win->xdg_configured = true;
        _XmWlDataAttachDragIcon(srv, win);
        _XmWlDamageTop(win, 0, 0, win->width, win->height);
        return;
    }

    if (!win->override_redirect) {
        win->is_popup = false;
        win->sent_title[0] = '\0';
        win->sent_app_id[0] = '\0';
        win->sent_parent_xid = 0;
        win->sent_min_w = win->sent_min_h = -1;
        win->sent_max_w = win->sent_max_h = -1;
        win->sent_deco_mode = -1;
        win->sent_modal_mode = -1;
        win->sent_tearing_mode = -1;
        win->fifo_flags = 0;
        win->xdg_surf = xdg_wm_base_get_xdg_surface(srv->xdg_wm_base, win->wl_surf);
        xdg_surface_add_listener(win->xdg_surf, &g_xdg_surface_listener, win);
        win->xdg_top = xdg_surface_get_toplevel(win->xdg_surf);
        xdg_toplevel_add_listener(win->xdg_top, &g_xdg_toplevel_listener, win);
        if (srv->deco_mgr) {
            win->xdg_deco = zxdg_decoration_manager_v1_get_toplevel_decoration(
                srv->deco_mgr, win->xdg_top);
            zxdg_toplevel_decoration_v1_add_listener(win->xdg_deco, &g_xdg_deco_listener, win);
        }
        (void)sync_toplevel_metadata_all(srv, win, 0);
        if (win->pending_activation_token[0] == '\0' && win->transient_for_xid == 0) {
            if (srv->startup_activation_token[0] != '\0') {
                snprintf(win->pending_activation_token, sizeof(win->pending_activation_token),
                         "%s", srv->startup_activation_token);
                srv->startup_activation_token[0] = '\0';
            } else {
                const char *late_tok = getenv("XDG_ACTIVATION_TOKEN");
                if (!late_tok || !late_tok[0])
                    late_tok = getenv("DESKTOP_STARTUP_ID");
                if (late_tok && late_tok[0]) {
                    snprintf(win->pending_activation_token, sizeof(win->pending_activation_token),
                             "%s", late_tok);
                    unsetenv("XDG_ACTIVATION_TOKEN");
                    unsetenv("DESKTOP_STARTUP_ID");
                }
            }
        }
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
        win->popup_parent = parent_win;
        win->xdg_surf = xdg_wm_base_get_xdg_surface(srv->xdg_wm_base, win->wl_surf);
        xdg_surface_add_listener(win->xdg_surf, &g_xdg_surface_listener, win);
        win->xdg_pop = xdg_surface_get_popup(win->xdg_surf, parent_win->xdg_surf, pos);
        xdg_positioner_destroy(pos);
        xdg_popup_add_listener(win->xdg_pop, &g_xdg_popup_listener, win);

        if (srv->seat && srv->last_button_serial != 0 &&
            (srv->ptr_grab_active ||
             (srv->last_button_time_ms != 0 && (_XmWlNowMs() - srv->last_button_time_ms) < 250)) &&
            srv->popup_depth == 0 && srv->wl_drag_source == NULL) {
            xdg_popup_grab(win->xdg_pop, srv->seat, srv->last_button_serial);
        }
        srv->popup_stack[srv->popup_depth++] = win;

        wl_surface_commit(win->wl_surf);
        wl_display_roundtrip(srv->wl_dpy);
        _XmWlDamageTop(win, 0, 0, win->width, win->height);
    }
}

void
_XmWlRepositionPopup(XmWlServer *srv, XmWlWindow *win)
{
    if (!srv || !win || !win->mapped || !win->is_popup || !win->xdg_pop || !srv->xdg_wm_base)
        return;

    XmWlWindow *parent_win = win->popup_parent;
    if (!parent_win || !parent_win->mapped || !parent_win->xdg_surf)
        return;

#ifdef XDG_POPUP_REPOSITION_SINCE_VERSION
    if (xdg_popup_get_version(win->xdg_pop) >= XDG_POPUP_REPOSITION_SINCE_VERSION) {
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
        xdg_popup_reposition(win->xdg_pop, pos, ++win->popup_reposition_token);
        xdg_positioner_destroy(pos);
        wl_display_flush(srv->wl_dpy);
        _XmWlDamageTop(win, 0, 0, win->width, win->height);
        return;
    }
#endif

    _XmWlUnmapWindowRole(srv, win);
    _XmWlSyncWindowRole(srv, win);
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
#ifdef HAVE_WAYLAND_XDG_DIALOG
    if (win->xdg_dialog) {
        xdg_dialog_v1_destroy(win->xdg_dialog);
        win->xdg_dialog = NULL;
    }
#endif
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
        if (win->is_drag_icon && win->wl_surf == srv->wl_drag_icon_surf) {
            /* Keep wl_drag_icon_surf alive across DragOverShell unmap/remap cycles */
            wl_surface_attach(win->wl_surf, NULL,
                              -srv->wl_drag_icon_dx, -srv->wl_drag_icon_dy);
            wl_surface_commit(win->wl_surf);
            srv->wl_drag_icon_dx = 0;
            srv->wl_drag_icon_dy = 0;
            if (srv->wl_drag_icon_owner == win)
                srv->wl_drag_icon_owner = NULL;
            win->wl_surf = NULL;
        } else {
            if (srv->wl_drag_icon_owner == win)
                srv->wl_drag_icon_owner = NULL;
            release_surface_ext(srv, win->wl_surf);
            wl_surface_destroy(win->wl_surf);
            win->wl_surf = NULL;
        }
    }
    win->surf_listener_added = false;
    win->current_wl_output = NULL;
    memset(win->wl_outputs, 0, sizeof(win->wl_outputs));
    win->wl_output_count = 0;
    win->has_authoritative_pos = false;
    win->xdg_configured = false;
    win->has_queued_wl_cfg = false;
    win->resize_awaiting_client = false;
    win->opaque_region_set = false;
    win->opaque_w = 0;
    win->opaque_h = 0;
    win->committed_w = 0;
    win->committed_h = 0;
    if (win->dmg_regions_inited)
        pixman_region32_clear(&win->dirty_region);
    win->sent_title[0] = '\0';
    win->sent_app_id[0] = '\0';
    win->pending_activation_token[0] = '\0';
    win->sent_parent_xid = 0;
    win->sent_min_w = win->sent_min_h = -1;
    win->sent_max_w = win->sent_max_h = -1;
    win->sent_deco_mode = -1;
    win->sent_modal_mode = -1;
    win->sent_tearing_mode = -1;
    win->fifo_flags = 0;
    win->has_sent_icon = false;
    win->sent_icon_hash = 0;
    win->is_popup = false;
    win->is_drag_icon = false;
    win->popup_parent = NULL;
    for (int i = 0; i < srv->popup_depth; ++i) {
        if (srv->popup_stack[i] && srv->popup_stack[i]->popup_parent == win)
            srv->popup_stack[i]->popup_parent = NULL;
    }
    if (srv->wl_dpy)
        wl_display_flush(srv->wl_dpy);
}

void
_XmWlDestroyWindowRole(XmWlServer *srv, XmWlWindow *win)
{
    if (!win) return;
    _XmWlUnmapWindowRole(srv, win);
    if (win->dmg_regions_inited) {
        pixman_region32_fini(&win->dirty_region);
        for (int si = 0; si < XM_WL_SHM_SLOTS; ++si)
            pixman_region32_fini(&win->dmg_hist_reg[si]);
        win->dmg_regions_inited = false;
    }
    for (int si = 0; si < XM_WL_SHM_SLOTS; ++si)
        free_shm_buf(&win->shm_bufs[si]);
    if (win->canvas) {
        pixman_image_unref(win->canvas);
        win->canvas = NULL;
    }
    free(win->canvas_bits);
    win->canvas_bits = NULL;
    win->canvas_cap_w = 0;
    win->canvas_cap_h = 0;
    win->canvas_stride = 0;
}

static void
process_queued_wl_configures(XmWlServer *srv)
{
    if (!srv || !srv->root) return;
    uint64_t now = now_mono_ms64();

    bool client_tx_backlogged = false;
    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        if (srv->clients[i].active && srv->clients[i].tx_len > 0) {
            client_tx_backlogged = true;
            break;
        }
    }

    for (XmWlWindow *w = srv->root->first_child; w; w = w->next_sibling) {
        if (!w->mapped || !w->wl_surf || !w->xdg_configured)
            continue;

        if (w->resize_awaiting_client && !client_tx_backlogged &&
            now >= w->resize_sent_ms + 45) {
            w->resize_awaiting_client = false;
            _XmWlDamageTop(w, 0, 0, w->width, w->height);
        }

        if (!w->has_queued_wl_cfg || w->resize_awaiting_client || client_tx_backlogged)
            continue;

        int cw = w->queued_wl_w > 0 ? w->queued_wl_w : (int)w->width;
        int ch = w->queued_wl_h > 0 ? w->queued_wl_h : (int)w->height;
        uint32_t serial = w->queued_wl_serial;
        w->has_queued_wl_cfg = false;
        w->pending_configure_serial = serial;

        if (cw != (int)w->width || ch != (int)w->height) {
            int old_w = w->width;
            int old_h = w->height;
            w->width = (uint16_t)cw;
            w->height = (uint16_t)ch;
            _XmWlEnsureTopCanvas(srv, w);
            _XmWlDeliverConfigureAndExpose(srv, w, cw, ch, old_w, old_h);
            /* Hold surface commit & ack_configure until the Xt main thread
             * finishes ConfigureNotify layout + child Expose redraws (or 45ms
             * fallback ceiling for raw Xlib clients). */
            w->resize_sync_gen = ++srv->resize_sync_seq;
            w->resize_awaiting_client = true;
            w->resize_sent_ms = now;
            w->dirty = false;
            if (w->dmg_regions_inited)
                pixman_region32_clear(&w->dirty_region);
            if (srv->attached_xm_display) {
                srv->pending_xt_resize_gen = w->resize_sync_gen;
                srv->has_pending_xt_resize = true;
                _XmWaylandNotifyMainThread(srv);
            }
        } else {
            _XmWlEnsureTopCanvas(srv, w);
            _XmWlDamageTop(w, 0, 0, w->width, w->height);
        }
    }
}

void
_XmWlFlushDirtySurfaces(XmWlServer *srv)
{
    if (!srv || !srv->root || !srv->wl_dpy) return;

    for (XmWlWindow *w = srv->root->first_child; w; w = w->next_sibling) {
        if (!w->mapped || !w->wl_surf || !w->xdg_configured ||
            w->resize_awaiting_client || !w->dirty || !w->canvas)
            continue;

        int log_w = w->canvas_w > 0 ? w->canvas_w : 1;
        int log_h = w->canvas_h > 0 ? w->canvas_h : 1;
        int buf_w = log_w;
        int buf_h = log_h;

        bool has_alpha = (w->depth == 32) || w->has_shape || w->is_drag_icon;
        XmWlShmBuf *sb = NULL;
        for (int si = 0; si < XM_WL_SHM_SLOTS; ++si) {
            if (!w->shm_bufs[si].busy) {
                if (!sb || w->shm_bufs[si].valid_gen > sb->valid_gen)
                    sb = &w->shm_bufs[si];
            }
        }
        uint64_t now_ms = now_mono_ms64();
        if (!sb) {
            int oldest = 0;
            for (int si = 1; si < XM_WL_SHM_SLOTS; ++si) {
                if (w->shm_bufs[si].attached_ms < w->shm_bufs[oldest].attached_ms)
                    oldest = si;
            }
            if (now_ms < w->shm_bufs[oldest].attached_ms + 50)
                continue;
            free_shm_buf(&w->shm_bufs[oldest]);
            sb = &w->shm_bufs[oldest];
        }
        if (!ensure_shm_buf(srv, sb, buf_w, buf_h, has_alpha))
            continue;

        ensure_win_dmg_regions(w);
        pixman_region32_t cur_dmg;
        pixman_region32_init(&cur_dmg);
        bool full_surface_dmg = (w->committed_w != log_w || w->committed_h != log_h || w->has_shape);
        if (!full_surface_dmg && pixman_region32_not_empty(&w->dirty_region)) {
            pixman_region32_intersect_rect(&cur_dmg, &w->dirty_region,
                                           0, 0, (unsigned)log_w, (unsigned)log_h);
        }
        if (!pixman_region32_not_empty(&cur_dmg)) {
            pixman_box32_t full_box = { 0, 0, log_w, log_h };
            pixman_region32_reset(&cur_dmg, &full_box);
        }

        uint64_t next_gen = ++w->frame_gen;
        pixman_region32_copy(&w->dmg_hist_reg[next_gen % XM_WL_SHM_SLOTS], &cur_dmg);

        pixman_region32_t copy_reg;
        pixman_region32_init(&copy_reg);
        if (!w->has_shape && sb->valid_w == log_w && sb->valid_h == log_h &&
            sb->valid_gen > 0 && (next_gen - sb->valid_gen) <= XM_WL_SHM_SLOTS) {
            pixman_region32_copy(&copy_reg, &cur_dmg);
            for (uint64_t g = sb->valid_gen + 1; g < next_gen; ++g) {
                pixman_region32_union(&copy_reg, &copy_reg,
                                      &w->dmg_hist_reg[g % XM_WL_SHM_SLOTS]);
            }
            pixman_region32_intersect_rect(&copy_reg, &copy_reg,
                                           0, 0, (unsigned)log_w, (unsigned)log_h);
        } else {
            pixman_box32_t full_box = { 0, 0, log_w, log_h };
            pixman_region32_reset(&copy_reg, &full_box);
        }

        int n_copy = 0;
        const pixman_box32_t *copy_boxes = pixman_region32_rectangles(&copy_reg, &n_copy);
        const uint8_t *src_base = (const uint8_t *)w->canvas_bits;
        uint8_t *dst_base = (uint8_t *)sb->pixels;
        for (int bi = 0; bi < n_copy; ++bi) {
            int cx1 = copy_boxes[bi].x1;
            int cy1 = copy_boxes[bi].y1;
            int cx2 = copy_boxes[bi].x2;
            int cy2 = copy_boxes[bi].y2;
            if (cx2 <= cx1 || cy2 <= cy1)
                continue;
            size_t copy_bytes = (size_t)(cx2 - cx1) * 4u;
            size_t x_byte_off = (size_t)cx1 * 4u;
            for (int ry = cy1; ry < cy2; ++ry) {
                memcpy(dst_base + (size_t)ry * (size_t)sb->stride + x_byte_off,
                       src_base + (size_t)ry * (size_t)w->canvas_stride + x_byte_off,
                       copy_bytes);
            }
        }
        pixman_region32_fini(&copy_reg);

        sb->valid_w = log_w;
        sb->valid_h = log_h;
        sb->valid_gen = next_gen;
        w->committed_w = log_w;
        w->committed_h = log_h;

        /* Apply non-rectangular SHAPE alpha + input region cutouts when present */
        if (w->has_shape && srv->compositor) {
            struct wl_region *ireg = wl_compositor_create_region(srv->compositor);
            uint8_t *inside = (uint8_t *)calloc((size_t)buf_w * (size_t)buf_h, 1);
            for (size_t ri = 0; ri < w->shape_rect_count; ++ri) {
                const XmWlRect *r = &w->shape_rects[ri];
                wl_region_add(ireg, r->x, r->y, r->width, r->height);
                if (inside) {
                    int sx1 = r->x < 0 ? 0 : r->x;
                    int sy1 = r->y < 0 ? 0 : r->y;
                    int sx2 = r->x + (int)r->width;
                    int sy2 = r->y + (int)r->height;
                    if (sx2 > buf_w) sx2 = buf_w;
                    if (sy2 > buf_h) sy2 = buf_h;
                    if (sx2 > sx1) {
                        for (int py = sy1; py < sy2; ++py)
                            memset(inside + (size_t)py * (size_t)buf_w + (size_t)sx1, 1, (size_t)(sx2 - sx1));
                    }
                }
            }
            if (inside) {
                for (int i = 0; i < buf_w * buf_h; ++i) {
                    if (!inside[i]) sb->pixels[i] = 0x00000000u;
                }
                free(inside);
            }
            sb->valid_gen = 0;
            if (!w->is_drag_icon)
                wl_surface_set_input_region(w->wl_surf, ireg);
            wl_region_destroy(ireg);
        }

        if (srv->compositor) {
            if (!has_alpha) {
                if (!w->opaque_region_set || w->opaque_w != log_w || w->opaque_h != log_h) {
                    struct wl_region *oreg = wl_compositor_create_region(srv->compositor);
                    if (oreg) {
                        wl_region_add(oreg, 0, 0, log_w, log_h);
                        wl_surface_set_opaque_region(w->wl_surf, oreg);
                        wl_region_destroy(oreg);
                        w->opaque_region_set = true;
                        w->opaque_w = log_w;
                        w->opaque_h = log_h;
                    }
                }
            } else if (w->opaque_region_set) {
                wl_surface_set_opaque_region(w->wl_surf, NULL);
                w->opaque_region_set = false;
                w->opaque_w = 0;
                w->opaque_h = 0;
            }
        }

        if (w->is_drag_icon && srv->compositor) {
            struct wl_region *empty_reg = wl_compositor_create_region(srv->compositor);
            if (empty_reg) {
                wl_surface_set_input_region(w->wl_surf, empty_reg);
                wl_region_destroy(empty_reg);
            }
        }

        if (w->pending_configure_serial != 0 && w->xdg_surf) {
            xdg_surface_ack_configure(w->xdg_surf, w->pending_configure_serial);
            w->pending_configure_serial = 0;
        }
        if (w->viewport) {
            wp_viewport_set_destination(w->viewport, log_w, log_h);
        }
        int32_t attach_dx = 0, attach_dy = 0;
        if (w->is_drag_icon) {
            int want_dx = srv->wl_drag_icon_dx;
            int want_dy = srv->wl_drag_icon_dy;
            if (srv->pointer_top != NULL) {
                int cand_dx = (int)w->x - (int)srv->ptr_root_x;
                int cand_dy = (int)w->y - (int)srv->ptr_root_y;
                if (cand_dx >= -(int)w->width - 32 && cand_dx <= 32 &&
                    cand_dy >= -(int)w->height - 32 && cand_dy <= 32) {
                    want_dx = cand_dx;
                    want_dy = cand_dy;
                }
            }
            attach_dx = (int32_t)(want_dx - srv->wl_drag_icon_dx);
            attach_dy = (int32_t)(want_dy - srv->wl_drag_icon_dy);
            srv->wl_drag_icon_dx = (int16_t)want_dx;
            srv->wl_drag_icon_dy = (int16_t)want_dy;
        }
        sb->busy = true;
        sb->attached_ms = now_ms;
        wl_surface_attach(w->wl_surf, sb->wl_buf, attach_dx, attach_dy);
        {
            int n_dmg = 0;
            const pixman_box32_t *dmg_boxes = pixman_region32_rectangles(&cur_dmg, &n_dmg);
            if (n_dmg > 0 && n_dmg <= 32) {
                for (int bi = 0; bi < n_dmg; ++bi) {
                    int bx1 = dmg_boxes[bi].x1;
                    int by1 = dmg_boxes[bi].y1;
                    int bw = dmg_boxes[bi].x2 - bx1;
                    int bh = dmg_boxes[bi].y2 - by1;
                    if (bw > 0 && bh > 0)
                        wl_surface_damage_buffer(w->wl_surf, bx1, by1, bw, bh);
                }
            } else {
                const pixman_box32_t *ext = pixman_region32_extents(&cur_dmg);
                int bw = ext->x2 - ext->x1;
                int bh = ext->y2 - ext->y1;
                if (bw > 0 && bh > 0)
                    wl_surface_damage_buffer(w->wl_surf, ext->x1, ext->y1, bw, bh);
                else
                    wl_surface_damage_buffer(w->wl_surf, 0, 0, log_w, log_h);
            }
        }
        pixman_region32_fini(&cur_dmg);
        pixman_region32_clear(&w->dirty_region);
        if (w->fifo_flags != 0) {
            (void)apply_surface_fifo_locked(
                srv, w->wl_surf,
                (w->fifo_flags & 1u) != 0,
                (w->fifo_flags & 2u) != 0);
        }
        wl_surface_commit(w->wl_surf);
        if (w->pending_activation_token[0] != '\0') {
            if (srv->activation_mgr)
                xdg_activation_v1_activate(srv->activation_mgr, w->pending_activation_token, w->wl_surf);
            w->pending_activation_token[0] = '\0';
        }
        w->dirty = false;
    }
    wl_display_flush(srv->wl_dpy);
}

/* ---------- Scale & DPI Live Updates ---------- */

static void
sync_rm_xft_dpi(XmWlServer *srv, int dpi)
{
    if (!srv || !srv->root || dpi < 48 || dpi > 576) return;
    uint32_t rm_atom = 23; /* XA_RESOURCE_MANAGER */
    XmWlProperty *rm = _XmWlGetProperty(srv->root, rm_atom);
    size_t prev_len = (rm && rm->data) ? rm->byte_len : 0;
    size_t cap = prev_len + 128;
    char *new_rm = (char *)malloc(cap);
    if (!new_rm) return;

    new_rm[0] = '\0';
    size_t out = 0;
    if (rm && rm->data && rm->byte_len > 0) {
        const char *p = (const char *)rm->data;
        const char *end = p + rm->byte_len;
        while (p < end && *p) {
            const char *nl = (const char *)memchr(p, '\n', (size_t)(end - p));
            size_t llen = nl ? (size_t)(nl - p + 1) : (size_t)(end - p);
            if (llen < 8 || strncasecmp(p, "Xft.dpi:", 8) != 0) {
                if (out + llen + 1 < cap) {
                    memcpy(new_rm + out, p, llen);
                    out += llen;
                    new_rm[out] = '\0';
                }
            }
            p += llen;
        }
        if (out > 0 && new_rm[out - 1] != '\n' && out + 2 < cap) {
            new_rm[out++] = '\n';
            new_rm[out] = '\0';
        }
    }
    snprintf(new_rm + out, cap - out, "Xft.dpi:\t%d\n", dpi);
    bool prev_sup = srv->suppress_ipc_publish;
    srv->suppress_ipc_publish = true;
    _XmWlSetProperty(srv, srv->root, rm_atom, 31 /* XA_STRING */, 8, 0 /* Replace */,
                     (const uint8_t *)new_rm, strlen(new_rm), true);
    srv->suppress_ipc_publish = prev_sup;
    free(new_rm);
}

static void
apply_wayland_outputs_ipc(XmWlServer *srv, const char *txt, size_t len)
{
    if (!srv || !txt || len == 0) return;

    XmWlIpcOutput parsed[XM_WL_MAX_OUTPUTS];
    int count = 0;
    int primary_dpi = 0;
    int max_dpi = 96;

    const char *p = txt;
    const char *end = txt + len;
    while (p < end && count < XM_WL_MAX_OUTPUTS) {
        const char *nl = (const char *)memchr(p, '\n', (size_t)(end - p));
        size_t llen = nl ? (size_t)(nl - p) : (size_t)(end - p);
        if (llen > 0 && llen < 512) {
            char line[512];
            memcpy(line, p, llen);
            line[llen] = '\0';
            unsigned idx = 0;
            char name[64] = {0};
            int en = 1, ox = 0, oy = 0, ow = 0, oh = 0, rot = 0, dpi = 96;
            if (sscanf(line, "%u|%63[^|]|%d|%d|%d|%d|%d|%d|%d",
                       &idx, name, &en, &ox, &oy, &ow, &oh, &rot, &dpi) >= 9 &&
                ow > 0 && oh > 0) {
                if (dpi < 48) dpi = 96;
                if (dpi > 576) dpi = 576;
                int rw = (rot == 90 || rot == 270) ? oh : ow;
                int rh = (rot == 90 || rot == 270) ? ow : oh;
                XmWlIpcOutput *io = &parsed[count++];
                memset(io, 0, sizeof(*io));
                snprintf(io->name, sizeof(io->name), "%s", name);
                io->enabled = (en != 0);
                io->x = ox;
                io->y = oy;
                io->width = rw;
                io->height = rh;
                io->dpi = dpi;
                if (io->enabled) {
                    if (dpi > max_dpi) max_dpi = dpi;
                    if (primary_dpi == 0 || (ox == 0 && oy == 0))
                        primary_dpi = dpi;
                }
            }
        }
        p = nl ? (nl + 1) : end;
    }

    if (count == 0) return;
    if (primary_dpi == 0) primary_dpi = 96;

    memcpy(srv->ipc_outputs, parsed, (size_t)count * sizeof(parsed[0]));
    srv->ipc_output_count = count;
    srv->compositor_native_dpi = true;
    srv->primary_dpi = primary_dpi;
    srv->logical_zoom = (double)primary_dpi / 96.0;
    srv->active_scale = 1.0;

    if (srv->root) {
        bool prev_sup = srv->suppress_ipc_publish;
        srv->suppress_ipc_publish = true;
        (void)_XmWlInternAtom(srv, "DPI", 3, false);
        uint32_t a_opt = _XmWlInternAtom(srv, "_MOTIF_RANDR_OUTPUT_DPI", 23, false);
        uint32_t one = 1u;
        _XmWlSetProperty(srv, srv->root, a_opt, 6u /* XA_CARDINAL */, 32, 0,
                         (const uint8_t *)&one, sizeof(one), true);
        srv->suppress_ipc_publish = prev_sup;
    }

    recompute_screen_metrics(srv);
    sync_rm_xft_dpi(srv, primary_dpi);

    int want_csz = (int)lround(24.0 * (double)max_dpi / 96.0);
    if (want_csz < 16) want_csz = 16;
    if (srv->shm && want_csz != srv->cursor_loaded_size) {
        struct wl_cursor_theme *nt = wl_cursor_theme_load(NULL, want_csz, srv->shm);
        if (nt) {
            if (srv->cursor_theme)
                wl_cursor_theme_destroy(srv->cursor_theme);
            srv->cursor_theme = nt;
            srv->cursor_loaded_size = want_csz;
        }
    }

    _XmWlBroadcastRandRChange(srv);
    if (srv->running) {
        srv->pending_xt_scale = srv->logical_zoom;
        srv->has_pending_xt_scale = true;
        _XmWaylandNotifyMainThread(srv);
    }
}

void
_XmWlApplyScaleUpdate(XmWlServer *srv, double new_scale)
{
    if (!srv || new_scale < 0.5 || new_scale > 4.0) return;
    srv->logical_zoom = new_scale;
    if (srv->active_scale < 1.0)
        srv->active_scale = 1.0;
    recompute_screen_metrics(srv);
    sync_rm_xft_dpi(srv, (int)lround(96.0 * new_scale));
    _XmWlBroadcastRandRChange(srv);

    srv->pending_xt_scale = new_scale;
    srv->has_pending_xt_scale = true;
    _XmWaylandNotifyMainThread(srv);
}

/* ---------- Compositor Root-Property UNIX IPC ---------- */

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

static void
send_all_nonblock(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, MSG_NOSIGNAL);
        if (n > 0) {
            sent += (size_t)n;
            continue;
        }
        if (n < 0 && errno == EINTR)
            continue;
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            struct pollfd pfd = { .fd = fd, .events = POLLOUT };
            if (poll(&pfd, 1, 50) > 0)
                continue;
        }
        break;
    }
}

void
_XmWlIpcPublishRootProp(XmWlServer *srv, const char *atom_name, const uint8_t *data, size_t len)
{
    if (!srv || srv->ipc_fd < 0 || !atom_name || len > 262144u) return;
    size_t aname_len = strlen(atom_name);
    size_t total_cap = 5 + aname_len + 1 + len * 2 + 2;
    char *msg = (char *)malloc(total_cap);
    if (!msg) return;
    int hn = snprintf(msg, total_cap, "SET %s ", atom_name);
    if (hn > 0 && (size_t)hn < total_cap) {
        size_t hex_len = hex_encode(data, len, msg + hn, total_cap - (size_t)hn);
        msg[(size_t)hn + hex_len] = '\n';
        send_all_nonblock(srv->ipc_fd, msg, (size_t)hn + hex_len + 1);
    }
    free(msg);
}

static bool
is_transient_ipc_rpc_atom(const char *aname, uint8_t *out_delim)
{
    if (!aname || !aname[0] || !out_delim)
        return false;
    size_t n = strlen(aname);
    if (!strcmp(aname, "_DT_WM_REQUEST") ||
        (n > 8 && (!strncmp(aname, "_DT_", 4) || !strncmp(aname, "_MOTIF_", 7)) &&
         !strcmp(aname + n - 8, "_REQUEST"))) {
        *out_delim = '\0';
        return true;
    }
    if ((n > 4 && (!strncmp(aname, "_DT", 3) || !strncmp(aname, "_MOTIF_", 7)) &&
         (!strcmp(aname + n - 4, "_IPC") || !strcmp(aname + n - 4, "_CMD")))) {
        *out_delim = '\n';
        return true;
    }
    return false;
}

static void
set_or_append_ipc_root_prop(XmWlServer *srv, uint32_t atom, uint32_t ptype,
                            const uint8_t *raw, size_t rlen,
                            bool queue_append, uint8_t delim)
{
    if (!srv || !srv->root || atom == 0)
        return;
    if (queue_append && rlen > 0) {
        XmWlProperty *existing = _XmWlGetProperty(srv->root, atom);
        if (existing && existing->format == 8 && existing->data && existing->byte_len > 0 &&
            existing->byte_len + 1u + rlen <= 65536u) {
            bool need_sep = (existing->data[existing->byte_len - 1u] != delim);
            size_t add_len = (need_sep ? 1u : 0u) + rlen;
            uint8_t *add_buf = (uint8_t *)malloc(add_len);
            if (add_buf) {
                if (need_sep) {
                    add_buf[0] = delim;
                    memcpy(add_buf + 1u, raw, rlen);
                } else {
                    memcpy(add_buf, raw, rlen);
                }
                _XmWlSetProperty(srv, srv->root, atom, ptype, 8, 2 /* PropModeAppend */,
                                 add_buf, add_len, true);
                free(add_buf);
                return;
            }
        }
    }
    _XmWlSetProperty(srv, srv->root, atom, ptype, 8, 0 /* PropModeReplace */, raw, rlen, true);
}

void
_XmWlIpcHandleReadable(XmWlServer *srv)
{
    if (!srv || srv->ipc_fd < 0) return;

    for (;;) {
        if (srv->ipc_rx_len + 8192 + 1 > srv->ipc_rx_cap) {
            size_t new_cap = srv->ipc_rx_cap ? srv->ipc_rx_cap * 2 : 65536;
            if (new_cap > 1048576u) {
                srv->ipc_rx_len = 0;
                break;
            }
            char *nb = (char *)realloc(srv->ipc_rx_buf, new_cap);
            if (!nb) return;
            srv->ipc_rx_buf = nb;
            srv->ipc_rx_cap = new_cap;
        }
        ssize_t n = recv(srv->ipc_fd, srv->ipc_rx_buf + srv->ipc_rx_len,
                         srv->ipc_rx_cap - 1 - srv->ipc_rx_len, 0);
        if (n > 0) {
            srv->ipc_rx_len += (size_t)n;
            srv->ipc_rx_buf[srv->ipc_rx_len] = '\0';
            continue;
        }
        if (n < 0 && errno == EINTR)
            continue;
        if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            close(srv->ipc_fd);
            srv->ipc_fd = -1;
            return;
        }
        break;
    }

    if (!srv->ipc_rx_buf || srv->ipc_rx_len == 0)
        return;

    char *start = srv->ipc_rx_buf;
    char *nl = NULL;
    while ((nl = strchr(start, '\n')) != NULL) {
        *nl = '\0';
        if (!strcmp(start, "READY")) {
            srv->ipc_initial_ready = true;
        } else if (!strncmp(start, "SURFACE_POS ", 12)) {
            unsigned int sid = 0;
            int px = 0, py = 0;
            if (sscanf(start + 12, "%u %d %d", &sid, &px, &py) == 3 &&
                sid != 0 && srv->root) {
                if (px < -32768) px = -32768;
                if (px > 32767) px = 32767;
                if (py < -32768) py = -32768;
                if (py > 32767) py = 32767;
                for (XmWlWindow *tw = srv->root->first_child; tw; tw = tw->next_sibling) {
                    if (tw->wl_surf &&
                        wl_proxy_get_id((struct wl_proxy *)tw->wl_surf) == sid) {
                        tw->has_authoritative_pos = true;
                        int16_t nx = (int16_t)px;
                        int16_t ny = (int16_t)py;
                        if (tw->x != nx || tw->y != ny) {
                            tw->x = nx;
                            tw->y = ny;
                            emit_top_configure_notify(srv, tw);
                        }
                        break;
                    }
                }
            }
        } else if (!strncmp(start, "PROP ", 5) || !strncmp(start, "PROP_QUEUE ", 11)) {
            bool force_queue = !strncmp(start, "PROP_QUEUE ", 11);
            char *p = start + (force_queue ? 11 : 5);
            char *sp = strrchr(p, ' ');
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
                    raw[rlen] = 0;
                    if ((!strcmp(aname, "_SUN_TT_SESSION") || !strcmp(aname, "TT_SESSION")) && rlen > 0) {
                        setenv("TT_SESSION", (const char *)raw, 1);
                        setenv("_SUN_TT_SESSION", (const char *)raw, 1);
                    }
                    srv->suppress_ipc_publish = true;
                    if (!strcmp(aname, "_DT_WORKSPACE_LIST_NAMES") && srv->wm_helper && rlen > 0) {
                        uint32_t ws_atoms[16];
                        size_t nws = 0, pos = 0;
                        while (pos < rlen && nws < 16) {
                            const char *wn = (const char *)(raw + pos);
                            size_t wlen = strnlen(wn, rlen - pos);
                            if (wlen > 0)
                                ws_atoms[nws++] = _XmWlInternAtom(srv, wn, wlen, false);
                            pos += wlen + 1;
                        }
                        uint32_t a_list = _XmWlInternAtom(srv, "_DT_WORKSPACE_LIST", 18, false);
                        _XmWlSetProperty(srv, srv->wm_helper, a_list, 4u /* XA_ATOM */, 32, 0,
                                         (const uint8_t *)ws_atoms, nws * 4u, true);
                    } else if (!strcmp(aname, "_DT_WORKSPACE_CURRENT_NAME") && rlen > 0) {
                        size_t wlen = strnlen((const char *)raw, rlen);
                        if (wlen > 0 && srv->wm_helper) {
                            uint32_t cur_a = _XmWlInternAtom(srv, (const char *)raw, wlen, false);
                            uint32_t a_cur = _XmWlInternAtom(srv, "_DT_WORKSPACE_CURRENT", 21, false);
                            _XmWlSetProperty(srv, srv->wm_helper, a_cur, 4u /* XA_ATOM */, 32, 0,
                                             (const uint8_t *)&cur_a, 4u, true);
                        }
                        uint32_t atom = _XmWlInternAtom(srv, aname, strlen(aname), false);
                        uint32_t utf8 = _XmWlInternAtom(srv, "UTF8_STRING", 11, false);
                        _XmWlSetProperty(srv, srv->root, atom, utf8, 8, 0, raw, rlen, true);
                    } else if (!strncmp(aname, "_DT_WORKSPACE_INFO_RAW_", 23) && srv->wm_helper && rlen > 0) {
                        const char *ws_id = aname + 23;
                        /* 7 NUL-separated strings: title, cset, bg, fg, backdropName, imageType, numWin */
                        const char *fields[7] = { "", "1", "0x0", "0xffffff", "NoBackdrop", "0", "0" };
                        size_t pos = 0;
                        for (int fi = 0; fi < 7 && pos < rlen; ++fi) {
                            fields[fi] = (const char *)(raw + pos);
                            pos += strnlen(fields[fi], rlen - pos) + 1;
                        }
                        uint32_t bd_atom = _XmWlInternAtom(srv, fields[4], strlen(fields[4]), false);
                        char info_buf[1024];
                        size_t ioff = 0;
                        ioff += (size_t)snprintf(info_buf + ioff, sizeof(info_buf) - ioff, "%s", fields[0]) + 1;
                        ioff += (size_t)snprintf(info_buf + ioff, sizeof(info_buf) - ioff, "%s", fields[1]) + 1;
                        ioff += (size_t)snprintf(info_buf + ioff, sizeof(info_buf) - ioff, "%s", fields[2]) + 1;
                        ioff += (size_t)snprintf(info_buf + ioff, sizeof(info_buf) - ioff, "%s", fields[3]) + 1;
                        ioff += (size_t)snprintf(info_buf + ioff, sizeof(info_buf) - ioff, "%lu", (unsigned long)bd_atom) + 1;
                        ioff += (size_t)snprintf(info_buf + ioff, sizeof(info_buf) - ioff, "%s", fields[5]) + 1;
                        ioff += (size_t)snprintf(info_buf + ioff, sizeof(info_buf) - ioff, "%s", fields[6]) + 1;
                        char prop_name[96];
                        snprintf(prop_name, sizeof(prop_name), "_DT_WORKSPACE_INFO_%s", ws_id);
                        uint32_t a_info = _XmWlInternAtom(srv, prop_name, strlen(prop_name), false);
                        _XmWlSetProperty(srv, srv->wm_helper, a_info, 31u /* XA_STRING */, 8, 0,
                                         (const uint8_t *)info_buf, ioff, true);
                    } else if (!strcmp(aname, "_DT_SM_STATE_INFO") && rlen >= 52 && srv->sm_helper) {
                        uint32_t a_sm_win = _XmWlInternAtom(srv, "_DT_SM_WINDOW_INFO", 18, false);
                        uint32_t a_state = _XmWlInternAtom(srv, "_DT_SM_STATE_INFO", 17, false);
                        if (srv->local_sm_win_xid == 0) {
                            uint32_t win_info[2] = { 0u, XM_WL_SM_HELPER_XID };
                            _XmWlSetProperty(srv, srv->root, a_sm_win, a_sm_win, 32, 0,
                                             (const uint8_t *)win_info, sizeof(win_info), true);
                        }
                        _XmWlSetProperty(srv, srv->sm_helper, a_state, a_state, 32, 0, raw, 52, true);
                        if (srv->local_sm_win_xid != 0) {
                            XmWlWindow *lsm = _XmWlFindWindow(srv, srv->local_sm_win_xid);
                            if (lsm) {
                                _XmWlSetProperty(srv, lsm, a_state, a_state, 32, 0, raw, 52, true);
                                uint8_t ev[32];
                                memset(ev, 0, sizeof(ev));
                                ev[0] = 33 | 0x80; /* ClientMessage */
                                ev[1] = 32;
                                memcpy(ev + 4, &srv->local_sm_win_xid, 4);
                                memcpy(ev + 8, &a_state, 4);
                                memcpy(ev + 12, raw, 4); /* flags */
                                for (int ci = 0; ci < XM_WL_MAX_CLIENTS; ++ci)
                                    if (srv->clients[ci].active && srv->clients[ci].handshake_done)
                                        _XmWlSendEventToClient(&srv->clients[ci], ev);
                            }
                        }
                    } else if (!strcmp(aname, "_DT_SM_SAVER_INFO") && srv->sm_helper) {
                        uint32_t a_saver = _XmWlInternAtom(srv, "_DT_SM_SAVER_INFO", 17, false);
                        _XmWlSetProperty(srv, srv->sm_helper, a_saver, 31u /* XA_STRING */, 8, 0, raw, rlen, true);
                        if (srv->local_sm_win_xid != 0) {
                            XmWlWindow *lsm = _XmWlFindWindow(srv, srv->local_sm_win_xid);
                            if (lsm) {
                                _XmWlSetProperty(srv, lsm, a_saver, 31u, 8, 0, raw, rlen, true);
                                uint8_t ev[32];
                                memset(ev, 0, sizeof(ev));
                                ev[0] = 33 | 0x80;
                                ev[1] = 32;
                                memcpy(ev + 4, &srv->local_sm_win_xid, 4);
                                memcpy(ev + 8, &a_saver, 4);
                                for (int ci = 0; ci < XM_WL_MAX_CLIENTS; ++ci)
                                    if (srv->clients[ci].active && srv->clients[ci].handshake_done)
                                        _XmWlSendEventToClient(&srv->clients[ci], ev);
                            }
                        }
                    } else if (!strcmp(aname, "_DT_SM_SAVE_TO_HOME") && rlen >= 8 && srv->local_sm_win_xid != 0) {
                        uint32_t a_proto = _XmWlInternAtom(srv, "_DT_SM_STM_PROTOCOL", 19, false);
                        uint32_t a_save = _XmWlInternAtom(srv, "_DT_SM_SAVE_TO_HOME", 19, false);
                        uint8_t ev[32];
                        memset(ev, 0, sizeof(ev));
                        ev[0] = 33 | 0x80;
                        ev[1] = 32;
                        memcpy(ev + 4, &srv->local_sm_win_xid, 4);
                        memcpy(ev + 8, &a_proto, 4);
                        memcpy(ev + 12, &a_save, 4);
                        memcpy(ev + 16, raw, 8);
                        for (int ci = 0; ci < XM_WL_MAX_CLIENTS; ++ci)
                            if (srv->clients[ci].active && srv->clients[ci].handshake_done)
                                _XmWlSendEventToClient(&srv->clients[ci], ev);
                    } else if ((!strcmp(aname, "_DT_PANEL_BUSY_START_SEQ") ||
                                !strcmp(aname, "_DT_PANEL_BUSY_STOP_SEQ")) &&
                               rlen > 0) {
                        uint32_t atom = _XmWlInternAtom(srv, aname, strlen(aname), false);
                        uint8_t fmt = (rlen >= 4u && (rlen % 4u) == 0u) ? 32u : 8u;
                        uint32_t ptype = (fmt == 32u) ? 6u /* XA_CARDINAL */ : 31u /* XA_STRING */;
                        if (srv->wm_helper)
                            _XmWlSetProperty(srv, srv->wm_helper, atom, ptype, fmt, 0, raw, rlen, true);
                        _XmWlSetProperty(srv, srv->root, atom, ptype, fmt, 0, raw, rlen, true);
                    } else if (!strcmp(aname, "_DT_WAYLAND_WHEEL") && rlen > 0) {
                        int nat = 0, sp = 3, sm = 1;
                        if (sscanf((const char *)raw, "%d %d %d", &nat, &sp, &sm) >= 3) {
                            (void)nat;
                            if (sp >= 1 && sp <= 20) {
                                srv->wheel_speed_lines = sp;
                                _XmSetWheelScrollSpeed(sp);
                            }
                            bool new_sm = (sm != 0);
                            if (!new_sm && srv->wheel_smooth_enabled) {
                                srv->smooth_cancel_requested = true;
                                _XmWaylandNotifyMainThread(srv);
                            }
                            srv->wheel_smooth_enabled = new_sm;
                        }
                        uint32_t atom = _XmWlInternAtom(srv, aname, strlen(aname), false);
                        _XmWlSetProperty(srv, srv->root, atom, 31u, 8, 0, raw, rlen, true);
                    } else {
                        if (!strcmp(aname, "RESOURCE_MANAGER") && rlen > 0)
                            apply_xrdb_wheel_settings(srv, (const char *)raw, rlen);
                        uint32_t atom = _XmWlInternAtom(srv, aname, strlen(aname), false);
                        uint32_t utf8 = _XmWlInternAtom(srv, "UTF8_STRING", 11, false);
                        uint32_t ptype = (!strcmp(aname, "RESOURCE_MANAGER") ||
                                          !strcmp(aname, "_SUN_TT_SESSION") ||
                                          !strcmp(aname, "TT_SESSION") ||
                                          !strcmp(aname, "SDT Pixel Set") ||
                                          !strcmp(aname, "_DT_COLOR_TYPE_MONITOR") ||
                                          !strcmp(aname, "_DT_WAYLAND_OUTPUTS") ||
                                          !strcmp(aname, "_DT_RESTORE_MODE"))
                                             ? 31u
                                             : utf8;
                        uint8_t rpc_delim = '\n';
                        bool queue_rpc = force_queue || is_transient_ipc_rpc_atom(aname, &rpc_delim);
                        set_or_append_ipc_root_prop(srv, atom, ptype, raw, rlen, queue_rpc, rpc_delim);
                        if (!strcmp(aname, "_DT_WAYLAND_OUTPUTS") && rlen > 0) {
                            apply_wayland_outputs_ipc(srv, (const char *)raw, rlen);
                        } else if (!strcmp(aname, "RESOURCE_MANAGER") &&
                                   srv->compositor_native_dpi && srv->primary_dpi >= 48) {
                            sync_rm_xft_dpi(srv, srv->primary_dpi);
                        }
                    }
                    srv->suppress_ipc_publish = false;
                    free(raw);
                }
            }
        }
        start = nl + 1;
    }
    size_t rem = strlen(start);
    if (rem > 0 && start != srv->ipc_rx_buf)
        memmove(srv->ipc_rx_buf, start, rem);
    srv->ipc_rx_len = rem;
    if (srv->ipc_rx_buf)
        srv->ipc_rx_buf[rem] = '\0';
}

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

    char cap_msg[64];
    int cap_len = snprintf(cap_msg, sizeof(cap_msg), "CLIENT_CAP native_dpi %d\n", (int)getpid());
    if (cap_len > 0 && (size_t)cap_len < sizeof(cap_msg))
        send_all_nonblock(srv->ipc_fd, cap_msg, (size_t)cap_len);

    /* Synchronously drain initial cached RESOURCE_MANAGER / _DT_* snapshot before XOpenDisplay */
    uint64_t deadline = now_mono_ms64() + 80;
    while (srv->ipc_fd >= 0 && !srv->ipc_initial_ready && now_mono_ms64() < deadline) {
        struct pollfd pfd = { .fd = srv->ipc_fd, .events = POLLIN };
        int prc = poll(&pfd, 1, 15);
        if (prc > 0 && (pfd.revents & (POLLIN | POLLHUP | POLLERR))) {
            _XmWlIpcHandleReadable(srv);
        } else if (prc == 0 && srv->ipc_rx_len == 0) {
            break;
        }
    }
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
            pthread_mutex_lock(&srv->lock);
            wl_display_dispatch_pending(srv->wl_dpy);
            process_queued_wl_configures(srv);
            pthread_mutex_unlock(&srv->lock);
            continue;
        }
        pthread_mutex_lock(&srv->lock);
        wl_display_flush(srv->wl_dpy);
        pthread_mutex_unlock(&srv->lock);

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
        uint64_t now_poll = now_mono_ms64();
        if (srv->held_keycode != 0 && srv->repeat_rate > 0) {
            timeout_ms = (srv->next_repeat_ms > now_poll) ? (int)(srv->next_repeat_ms - now_poll) : 0;
        }
        if (srv->root) {
            for (XmWlWindow *w = srv->root->first_child; w; w = w->next_sibling) {
                if (!w->mapped || !w->wl_surf || !w->xdg_configured)
                    continue;
                if (w->resize_awaiting_client) {
                    uint64_t due = w->resize_sent_ms + 35;
                    int rem = (due > now_poll) ? (int)(due - now_poll) : 0;
                    if (timeout_ms < 0 || rem < timeout_ms)
                        timeout_ms = rem;
                } else if (w->has_queued_wl_cfg) {
                    timeout_ms = 0;
                }
            }
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
                pthread_mutex_lock(&srv->lock);
                for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
                    if (srv->clients[i].active)
                        _XmWlDisconnectClient(srv, i);
                }
                pthread_mutex_lock(&srv->act_lock);
                if (srv->act_req_in_flight && !srv->act_req_done) {
                    srv->act_req_result[0] = '\0';
                    srv->act_req_done = true;
                    pthread_cond_broadcast(&srv->act_cond);
                }
                pthread_mutex_unlock(&srv->act_lock);
                pthread_mutex_unlock(&srv->lock);
                break;
            }
        } else {
            wl_display_cancel_read(srv->wl_dpy);
        }

        pthread_mutex_lock(&srv->lock);
        wl_display_dispatch_pending(srv->wl_dpy);
        process_queued_wl_configures(srv);

        if (prc > 0 && (pfds[wake_idx].revents & POLLIN)) {
            uint8_t drain[64];
            while (read(srv->wake_pipe[0], drain, sizeof(drain)) > 0) {}
        }
        process_pending_activation_requests(srv);

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

        bool any_client_read = false;
        for (nfds_t k = 0; k < nfds; ++k) {
            int ci = client_map[k];
            if (ci < 0 || !srv->clients[ci].active) continue;
            if (pfds[k].revents & (POLLERR | POLLHUP)) {
                if (pfds[k].revents & POLLIN) {
                    _XmWlHandleClientReadable(srv, ci);
                    any_client_read = true;
                }
                if (srv->clients[ci].active)
                    _XmWlDisconnectClient(srv, ci);
                continue;
            }
            if (pfds[k].revents & POLLIN) {
                _XmWlHandleClientReadable(srv, ci);
                any_client_read = true;
            }
        }

        if (any_client_read && srv->root && !srv->attached_xm_display) {
            bool pending_events = false;
            for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
                if (srv->clients[i].active && srv->clients[i].tx_len > 0) {
                    pending_events = true;
                    break;
                }
            }
            if (!pending_events) {
                for (XmWlWindow *w = srv->root->first_child; w; w = w->next_sibling) {
                    if (w->resize_awaiting_client && w->dirty)
                        w->resize_awaiting_client = false;
                }
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
        process_queued_wl_configures(srv);
        _XmWlFlushClientWrites(srv);
        pthread_mutex_unlock(&srv->lock);
    }
    return NULL;
}

/* ---------- Main-Thread XtAppAddInput Wakeup & Smooth Scrolling ---------- */

static void smooth_scroll_timer_cb(XtPointer closure, XtIntervalId *id);

static Widget
resolve_smooth_target_widget(Display *dpy, const uint32_t xids[8])
{
    if (!dpy)
        return NULL;
    for (size_t i = 0; i < 8 && xids[i] != 0; ++i) {
        Widget w = XtWindowToWidget(dpy, (Window)xids[i]);
        if (w && !w->core.being_destroyed)
            return w;
    }
    return NULL;
}

static void
fallback_discrete_wheel_from_main(XmWlServer *srv, int ax, double rem_v120)
{
    if (!srv || fabs(rem_v120) < 0.5)
        return;
    int32_t step = (int32_t)llround(rem_v120);
    if (step == 0)
        step = (rem_v120 < 0.0) ? -1 : 1;
    pthread_mutex_lock(&srv->lock);
    emit_discrete_wheel_v120(srv, ax, step, _XmWlNowMs());
    _XmWlFlushClientWrites(srv);
    pthread_mutex_unlock(&srv->lock);
}

static void
run_smooth_scroll_frame(XmWlServer *srv)
{
    if (!srv || !srv->attached_xm_display) {
        if (srv) {
            srv->smooth_rem_v120[0] = 0.0;
            srv->smooth_rem_v120[1] = 0.0;
            srv->smooth_unit_carry[0] = 0.0;
            srv->smooth_unit_carry[1] = 0.0;
        }
        return;
    }

    Display *dpy = XtDisplay(srv->attached_xm_display);
    Widget leaf = resolve_smooth_target_widget(dpy, srv->smooth_active_chain);
    srv->smooth_last_step_ms = _XmWlNowMs();

    for (int ax = 0; ax < 2; ++ax) {
        double rem = srv->smooth_rem_v120[ax];
        if (fabs(rem) < 0.05) {
            srv->smooth_rem_v120[ax] = 0.0;
            continue;
        }
        if (!leaf) {
            fallback_discrete_wheel_from_main(srv, ax, rem);
            srv->smooth_rem_v120[ax] = 0.0;
            srv->smooth_unit_carry[ax] = 0.0;
            continue;
        }

        double step;
        if (!srv->wheel_smooth_enabled) {
            if (fabs(rem) < 60.0)
                continue;
            step = rem;
        } else if (fabs(rem) <= 4.0) {
            step = rem;
        } else {
            step = rem * 0.34;
            if (fabs(step) < 4.0)
                step = copysign(4.0, rem);
        }

        Boolean at_boundary = False;
        Boolean handled = _XmSmoothScrollStep(
            leaf, ax == 1 ? True : False, step,
            &srv->smooth_unit_carry[ax], &at_boundary);

        if (!handled) {
            /* Non-scrollable widget (e.g. XmSpinBox / XmTabBox): forward exact
             * accumulated delta to discrete Button4..Button7 translations. */
            fallback_discrete_wheel_from_main(srv, ax, rem);
            srv->smooth_rem_v120[ax] = 0.0;
            srv->smooth_unit_carry[ax] = 0.0;
            continue;
        }

        if (at_boundary) {
            srv->smooth_rem_v120[ax] = 0.0;
            srv->smooth_unit_carry[ax] = 0.0;
        } else {
            srv->smooth_rem_v120[ax] = rem - step;
            if (fabs(srv->smooth_rem_v120[ax]) < 0.05)
                srv->smooth_rem_v120[ax] = 0.0;
        }
    }
    XFlush(dpy);
}

static void
arm_smooth_scroll_timer_if_needed(XmWlServer *srv)
{
    if (!srv || !srv->attached_xm_display)
        return;
    bool active = srv->wheel_smooth_enabled &&
                  (fabs(srv->smooth_rem_v120[0]) >= 0.05 ||
                   fabs(srv->smooth_rem_v120[1]) >= 0.05);
    if (active && !srv->smooth_timer_id) {
        XtAppContext app = XtWidgetToApplicationContext(srv->attached_xm_display);
        srv->smooth_timer_id = XtAppAddTimeOut(app, 12, smooth_scroll_timer_cb, srv);
    } else if (!active && srv->smooth_timer_id) {
        XtRemoveTimeOut(srv->smooth_timer_id);
        srv->smooth_timer_id = 0;
    }
}

static void
drain_smooth_scroll_main_thread(XmWlServer *srv, bool allow_immediate_step)
{
    if (!srv)
        return;
    uint32_t new_chain[8] = {0};
    double pending[2] = {0.0, 0.0};
    bool has_pending = false;
    bool cancel_req = false;

    pthread_mutex_lock(&srv->lock);
    if (srv->smooth_cancel_requested) {
        cancel_req = true;
        srv->smooth_cancel_requested = false;
    }
    if (srv->has_smooth_pending) {
        memcpy(new_chain, srv->smooth_xid_chain, sizeof(new_chain));
        pending[0] = srv->smooth_pending_v120[0];
        pending[1] = srv->smooth_pending_v120[1];
        srv->smooth_pending_v120[0] = 0.0;
        srv->smooth_pending_v120[1] = 0.0;
        srv->has_smooth_pending = false;
        has_pending = true;
    }
    pthread_mutex_unlock(&srv->lock);

    if (cancel_req) {
        srv->smooth_rem_v120[0] = 0.0;
        srv->smooth_rem_v120[1] = 0.0;
        srv->smooth_unit_carry[0] = 0.0;
        srv->smooth_unit_carry[1] = 0.0;
        if (srv->smooth_timer_id) {
            XtRemoveTimeOut(srv->smooth_timer_id);
            srv->smooth_timer_id = 0;
        }
    }

    if (!has_pending)
        return;

    /* Reset carry/momentum if the hovered leaf window changed */
    if (new_chain[0] != 0 && new_chain[0] != srv->smooth_active_chain[0]) {
        srv->smooth_rem_v120[0] = 0.0;
        srv->smooth_rem_v120[1] = 0.0;
        srv->smooth_unit_carry[0] = 0.0;
        srv->smooth_unit_carry[1] = 0.0;
    }
    memcpy(srv->smooth_active_chain, new_chain, sizeof(new_chain));

    for (int ax = 0; ax < 2; ++ax) {
        if (fabs(pending[ax]) < 0.05)
            continue;
        /* Instantaneous direction reversal with zero reverse-momentum lag */
        if ((srv->smooth_rem_v120[ax] > 0.0 && pending[ax] < 0.0) ||
            (srv->smooth_rem_v120[ax] < 0.0 && pending[ax] > 0.0)) {
            srv->smooth_rem_v120[ax] = 0.0;
            srv->smooth_unit_carry[ax] = 0.0;
        }
        srv->smooth_rem_v120[ax] += pending[ax];
    }

    if (allow_immediate_step) {
        uint64_t now = _XmWlNowMs();
        if (!srv->smooth_timer_id || (now - srv->smooth_last_step_ms) >= 10ULL)
            run_smooth_scroll_frame(srv);
        arm_smooth_scroll_timer_if_needed(srv);
    }
}

static void
smooth_scroll_timer_cb(XtPointer closure, XtIntervalId *id)
{
    (void)id;
    XmWlServer *srv = (XmWlServer *)closure;
    if (!srv)
        return;
    srv->smooth_timer_id = 0;
    drain_smooth_scroll_main_thread(srv, false);
    if (fabs(srv->smooth_rem_v120[0]) >= 0.05 ||
        fabs(srv->smooth_rem_v120[1]) >= 0.05) {
        run_smooth_scroll_frame(srv);
        arm_smooth_scroll_timer_if_needed(srv);
    }
}

static Boolean
xt_resize_sync_workproc(XtPointer closure)
{
    XmWlServer *srv = (XmWlServer *)closure;
    if (!srv || !srv->attached_xm_display) {
        if (srv) {
            pthread_mutex_lock(&srv->lock);
            srv->resize_sync_wp_id = 0;
            pthread_mutex_unlock(&srv->lock);
        }
        return True;
    }

    uint32_t target_gen = 0;
    pthread_mutex_lock(&srv->lock);
    target_gen = srv->pending_xt_resize_gen;
    pthread_mutex_unlock(&srv->lock);

    Display *dpy = XtDisplay(srv->attached_xm_display);
    XSync(dpy, False);
    if (QLength(dpy) > 0) {
        /* Child Expose / ConfigureNotify events arrived; keep this WorkProc
         * active so Xt dispatches all Expose redraws before committing. */
        return False;
    }

    pthread_mutex_lock(&srv->lock);
    if (srv->has_pending_xt_resize && srv->pending_xt_resize_gen != target_gen) {
        pthread_mutex_unlock(&srv->lock);
        return False;
    }
    bool released = false;
    if (srv->root) {
        for (XmWlWindow *w = srv->root->first_child; w; w = w->next_sibling) {
            if (w->resize_awaiting_client && w->resize_sync_gen <= target_gen) {
                w->resize_awaiting_client = false;
                _XmWlDamageTop(w, 0, 0, w->width, w->height);
                released = true;
            }
        }
    }
    srv->has_pending_xt_resize = false;
    srv->resize_sync_wp_id = 0;
    if (released)
        _XmWlWakeServer(srv);
    pthread_mutex_unlock(&srv->lock);
    return True;
}

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
    bool native_dpi = false;
    bool arm_resize_wp = false;
    pthread_mutex_lock(&srv->lock);
    native_dpi = srv->compositor_native_dpi;
    if (srv->has_pending_xt_scale) {
        new_scale = srv->pending_xt_scale;
        srv->has_pending_xt_scale = false;
        apply_scale = true;
    }
    if (srv->has_pending_xt_resize && !srv->resize_sync_wp_id && srv->attached_xm_display) {
        arm_resize_wp = true;
    }
    pthread_mutex_unlock(&srv->lock);

    if (arm_resize_wp && srv->attached_xm_display) {
        XtAppContext app = XtWidgetToApplicationContext(srv->attached_xm_display);
        XtWorkProcId wp = XtAppAddWorkProc(app, xt_resize_sync_workproc, srv);
        pthread_mutex_lock(&srv->lock);
        srv->resize_sync_wp_id = wp;
        pthread_mutex_unlock(&srv->lock);
    }

    if (apply_scale && srv->attached_xm_display) {
        Display *dpy = XtDisplay(srv->attached_xm_display);
#ifdef HAVE_XLIBRE_DPI
        if (native_dpi) {
            XrmDatabase db = XrmGetDatabase(dpy);
            if (db) {
                char dpi_buf[32];
                snprintf(dpi_buf, sizeof(dpi_buf), "%d", (int)lround(96.0 * new_scale));
                XrmPutStringResource(&db, "Xft.dpi", dpi_buf);
            }
        } else {
            XmSetDisplayScaleOverride(dpy, new_scale);
        }
#else
        (void)native_dpi;
        XmSetDisplayScaleOverride(dpy, new_scale);
#endif
    }
    _XmWlImeDrainMainThread(srv);
    drain_smooth_scroll_main_thread(srv, true);
}

void
_XmWaylandAttachDisplay(Widget xm_display)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !xm_display) return;
    bool want_core_xcursor = false;
    pthread_mutex_lock(&srv->lock);
    if (!srv->attached_xm_display && srv->xt_pipe[0] >= 0) {
        srv->attached_xm_display = xm_display;
        XtAppContext app = XtWidgetToApplicationContext(xm_display);
        srv->xt_input_id = XtAppAddInput(
            app, srv->xt_pipe[0], (XtPointer)XtInputReadMask, xt_wayland_wakeup_cb, srv);
        want_core_xcursor = (srv->cursor_shape_mgr != NULL && XtDisplay(xm_display) != NULL);
    }
    if (srv->startup_activation_token[0] == '\0') {
        const char *env_act = getenv("XDG_ACTIVATION_TOKEN");
        if (!env_act || !env_act[0])
            env_act = getenv("DESKTOP_STARTUP_ID");
        if (env_act && env_act[0]) {
            snprintf(srv->startup_activation_token, sizeof(srv->startup_activation_token),
                     "%s", env_act);
            unsetenv("XDG_ACTIVATION_TOKEN");
            unsetenv("DESKTOP_STARTUP_ID");
        }
    }
    pthread_mutex_unlock(&srv->lock);
    if (want_core_xcursor) {
        typedef int (*XcursorSetThemeFn)(Display *, const char *);
        typedef int (*XcursorSetThemeCoreFn)(Display *, int);
        void *xc_dl = dlopen("libXcursor.so.1", RTLD_LAZY);
        XcursorSetThemeFn set_theme = xc_dl
            ? (XcursorSetThemeFn)dlsym(xc_dl, "XcursorSetTheme")
            : (XcursorSetThemeFn)dlsym(RTLD_DEFAULT, "XcursorSetTheme");
        XcursorSetThemeCoreFn set_core = xc_dl
            ? (XcursorSetThemeCoreFn)dlsym(xc_dl, "XcursorSetThemeCore")
            : (XcursorSetThemeCoreFn)dlsym(RTLD_DEFAULT, "XcursorSetThemeCore");
        if (set_theme)
            set_theme(XtDisplay(xm_display), "core");
        if (set_core)
            set_core(XtDisplay(xm_display), 0);
    }
}

void
_XmWaylandDetachDisplay(Widget xm_display)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !xm_display) return;
    pthread_mutex_lock(&srv->lock);
    if (srv->attached_xm_display == xm_display) {
        if (srv->resize_sync_wp_id) {
            XtRemoveWorkProc(srv->resize_sync_wp_id);
            srv->resize_sync_wp_id = 0;
        }
        srv->has_pending_xt_resize = false;
        if (srv->smooth_timer_id) {
            XtRemoveTimeOut(srv->smooth_timer_id);
            srv->smooth_timer_id = 0;
        }
        srv->smooth_rem_v120[0] = 0.0;
        srv->smooth_rem_v120[1] = 0.0;
        srv->smooth_unit_carry[0] = 0.0;
        srv->smooth_unit_carry[1] = 0.0;
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
    XmWlServer *srv = g_wl_srv;
    if (!srv || g_wl_owner_pid != getpid())
        return;
    g_wl_owner_pid = 0;

    srv->running = false;
    _XmWlWakeServer(srv);
    pthread_join(srv->thread, NULL);
    g_wl_srv = NULL;

    if (srv->listen_fd >= 0) close(srv->listen_fd);
    if (srv->socket_path[0]) unlink(srv->socket_path);
    if (srv->runtime_dir[0]) rmdir(srv->runtime_dir);
    if (srv->ipc_fd >= 0) close(srv->ipc_fd);
    free(srv->ipc_rx_buf);
    if (srv->wake_pipe[0] >= 0) close(srv->wake_pipe[0]);
    if (srv->wake_pipe[1] >= 0) close(srv->wake_pipe[1]);
    if (srv->xt_pipe[0] >= 0) close(srv->xt_pipe[0]);
    if (srv->xt_pipe[1] >= 0) close(srv->xt_pipe[1]);
    if (srv->cursor_shape_dev) {
        wp_cursor_shape_device_v1_destroy(srv->cursor_shape_dev);
        srv->cursor_shape_dev = NULL;
    }
    if (srv->cursor_shape_mgr) {
        wp_cursor_shape_manager_v1_destroy(srv->cursor_shape_mgr);
        srv->cursor_shape_mgr = NULL;
    }
    if (srv->touch) {
        wl_touch_destroy(srv->touch);
        srv->touch = NULL;
    }
    for (int bi = 0; bi < 128; ++bi) {
        while (srv->cursor_buckets[bi]) {
            XmWlCursor *cur = srv->cursor_buckets[bi];
            srv->cursor_buckets[bi] = cur->hash_next;
            free_shm_buf(&cur->shm_buf);
            free(cur->argb_pixels);
            free(cur);
        }
    }
    if (srv->cursor_theme) {
        wl_cursor_theme_destroy(srv->cursor_theme);
        srv->cursor_theme = NULL;
    }
    _XmWlRenderDestroy(srv);
    while (srv->pending_act_tokens) {
        XmWlPendingTokenCtx *ctx = srv->pending_act_tokens;
        srv->pending_act_tokens = ctx->next;
        if (ctx->tok_obj)
            xdg_activation_token_v1_destroy(ctx->tok_obj);
        free(ctx);
    }
    if (srv->activation_mgr) {
        xdg_activation_v1_destroy(srv->activation_mgr);
        srv->activation_mgr = NULL;
    }
    while (srv->surf_exts) {
        release_surface_ext(srv, srv->surf_exts->surf);
    }
#ifdef HAVE_WAYLAND_FIFO
    if (srv->fifo_mgr) {
        wp_fifo_manager_v1_destroy(srv->fifo_mgr);
        srv->fifo_mgr = NULL;
    }
#endif
#ifdef HAVE_WAYLAND_COLOR_MANAGEMENT
    if (srv->color_mgr) {
        wp_color_manager_v1_destroy(srv->color_mgr);
        srv->color_mgr = NULL;
    }
#endif
    if (srv->icon_mgr) {
        xdg_toplevel_icon_manager_v1_destroy(srv->icon_mgr);
        srv->icon_mgr = NULL;
    }
#ifdef HAVE_WAYLAND_TEARING_CONTROL
    if (srv->tearing_mgr) {
        wp_tearing_control_manager_v1_destroy(srv->tearing_mgr);
        srv->tearing_mgr = NULL;
    }
#endif
#ifdef HAVE_WAYLAND_XDG_DIALOG
    if (srv->xdg_dialog_mgr) {
        xdg_wm_dialog_v1_destroy(srv->xdg_dialog_mgr);
        srv->xdg_dialog_mgr = NULL;
    }
#endif
#ifdef HAVE_WAYLAND_DMABUF
#ifdef ZWP_LINUX_DMABUF_V1_GET_DEFAULT_FEEDBACK_SINCE_VERSION
    if (srv->dmabuf_feedback) {
        zwp_linux_dmabuf_feedback_v1_destroy(srv->dmabuf_feedback);
        srv->dmabuf_feedback = NULL;
    }
#endif
    if (srv->dmabuf) {
        zwp_linux_dmabuf_v1_destroy(srv->dmabuf);
        srv->dmabuf = NULL;
    }
    free(srv->dmabuf_table);
    free(srv->dmabuf_formats);
    free(srv->dmabuf_staging);
#endif
    if (srv->wl_dpy) wl_display_disconnect(srv->wl_dpy);
    pthread_cond_destroy(&srv->act_cond);
    pthread_mutex_destroy(&srv->act_lock);
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
    pthread_mutex_init(&srv->act_lock, NULL);
    {
        pthread_condattr_t cattr;
        pthread_condattr_init(&cattr);
#ifdef CLOCK_MONOTONIC
        pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
#endif
        pthread_cond_init(&srv->act_cond, &cattr);
        pthread_condattr_destroy(&cattr);
    }
    srv->wl_dpy = wl_dpy;
    srv->listen_fd = -1;
    srv->ipc_fd = -1;
    srv->wake_pipe[0] = srv->wake_pipe[1] = -1;
    srv->xt_pipe[0] = srv->xt_pipe[1] = -1;
    srv->screen_w = 1280;
    srv->screen_h = 800;
    srv->screen_mm_w = 339;
    srv->screen_mm_h = 212;
    srv->active_scale = 1.0;
    srv->logical_zoom = 1.0;
    srv->repeat_rate = 25;
    srv->repeat_delay = 400;
    srv->wheel_speed_lines = 3;
    srv->wheel_smooth_enabled = true;
    srv->x11_focus_win = 1u; /* PointerRoot */
    srv->x11_focus_revert = 1u;

    const char *env_scale = getenv("CK_MOTIF_WAYLAND_SCALE");
    if (env_scale && env_scale[0]) {
        double sc = strtod(env_scale, NULL);
        if (sc >= 0.5 && sc <= 4.0) {
            srv->logical_zoom = sc;
            srv->active_scale = sc;
        }
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
    recompute_screen_metrics(srv);
    for (int i = 0; i < srv->output_count; ++i) {
        if (srv->outputs[i].scale > 0 && !srv->frac_scale_mgr &&
            (double)srv->outputs[i].scale > srv->active_scale) {
            srv->active_scale = (double)srv->outputs[i].scale;
        }
    }

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
        srv->cursor_loaded_size = 24;
        srv->cursor_surface = wl_compositor_create_surface(srv->compositor);
    }

    connect_compositor_ipc(srv);
    if (!srv->compositor_native_dpi && srv->logical_zoom != 1.0)
        _XmWlApplyScaleUpdate(srv, srv->logical_zoom);

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
    pthread_once(&g_wl_atfork_once, register_wayland_atfork);
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
        if (scale_return) {
            *scale_return = srv->compositor_native_dpi
                                ? 1.0
                                : (top->preferred_scale > 0.0 ? top->preferred_scale : srv->active_scale);
        }
        ok = True;
    }
    pthread_mutex_unlock(&srv->lock);
    return ok;
}

typedef struct {
    struct xdg_activation_v1 *mgr;
    char token[256];
    bool done;
} XmWlStandaloneActCtx;

static void
standalone_act_reg_global(void *data, struct wl_registry *reg, uint32_t name,
                          const char *iface, uint32_t ver)
{
    (void)ver;
    XmWlStandaloneActCtx *ctx = (XmWlStandaloneActCtx *)data;
    if (ctx && !ctx->mgr && iface && !strcmp(iface, xdg_activation_v1_interface.name)) {
        ctx->mgr = (struct xdg_activation_v1 *)wl_registry_bind(
            reg, name, &xdg_activation_v1_interface, 1);
    }
}

static void
standalone_act_reg_remove(void *data, struct wl_registry *reg, uint32_t name)
{
    (void)data; (void)reg; (void)name;
}

static const struct wl_registry_listener g_standalone_act_reg_listener = {
    .global = standalone_act_reg_global,
    .global_remove = standalone_act_reg_remove,
};

static void
standalone_act_tok_done(void *data, struct xdg_activation_token_v1 *tok, const char *token)
{
    (void)tok;
    XmWlStandaloneActCtx *ctx = (XmWlStandaloneActCtx *)data;
    if (!ctx) return;
    if (token && token[0])
        snprintf(ctx->token, sizeof(ctx->token), "%s", token);
    ctx->done = true;
}

static const struct xdg_activation_token_v1_listener g_standalone_act_tok_listener = {
    .done = standalone_act_tok_done,
};

static Boolean
request_standalone_wayland_token(const char *app_id, char **token_return)
{
    const char *wl_disp = getenv("WAYLAND_DISPLAY");
    if (!wl_disp || !wl_disp[0])
        return False;
    struct wl_display *dpy = wl_display_connect(NULL);
    if (!dpy)
        return False;

    XmWlStandaloneActCtx ctx;
    memset(&ctx, 0, sizeof(ctx));
    struct wl_registry *reg = wl_display_get_registry(dpy);
    if (reg) {
        wl_registry_add_listener(reg, &g_standalone_act_reg_listener, &ctx);
        wl_display_roundtrip(dpy);
    }
    if (ctx.mgr) {
        struct xdg_activation_token_v1 *tok =
            xdg_activation_v1_get_activation_token(ctx.mgr);
        if (tok) {
            xdg_activation_token_v1_add_listener(tok, &g_standalone_act_tok_listener, &ctx);
            if (app_id && app_id[0])
                xdg_activation_token_v1_set_app_id(tok, app_id);
            xdg_activation_token_v1_commit(tok);
            wl_display_roundtrip(dpy);
            xdg_activation_token_v1_destroy(tok);
        }
        xdg_activation_v1_destroy(ctx.mgr);
    }
    if (reg)
        wl_registry_destroy(reg);
    wl_display_disconnect(dpy);

    if (ctx.done && ctx.token[0]) {
        size_t len = strlen(ctx.token) + 1;
        char *copy = XtMalloc((Cardinal)len);
        if (copy) {
            memcpy(copy, ctx.token, len);
            *token_return = copy;
            return True;
        }
    }
    return False;
}

Boolean
XmWaylandRequestActivationToken(
    Display *display,
    Window source_window,
    const char *app_id,
    char **token_return)
{
    if (!token_return)
        return False;
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !srv->running)
        return request_standalone_wayland_token(app_id, token_return);
    if (display && !display_matches_wayland_server(display, srv))
        return request_standalone_wayland_token(app_id, token_return);
    if (pthread_equal(pthread_self(), srv->thread))
        return False;

    pthread_mutex_lock(&srv->lock);
    if (!srv->activation_mgr) {
        pthread_mutex_unlock(&srv->lock);
        return False;
    }
    pthread_mutex_unlock(&srv->lock);

    struct timespec deadline = {0, 0};
#ifdef CLOCK_MONOTONIC
    clock_gettime(CLOCK_MONOTONIC, &deadline);
#else
    clock_gettime(CLOCK_REALTIME, &deadline);
#endif
    deadline.tv_sec += 2;
    deadline.tv_nsec += 500000000L;
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec += 1;
        deadline.tv_nsec -= 1000000000L;
    }

    pthread_mutex_lock(&srv->act_lock);
    while (srv->act_req_in_flight) {
        if (pthread_cond_timedwait(&srv->act_cond, &srv->act_lock, &deadline) == ETIMEDOUT) {
            pthread_mutex_unlock(&srv->act_lock);
            return False;
        }
    }

    srv->act_req_seq++;
    srv->act_req_in_flight = true;
    srv->act_req_submitted = false;
    srv->act_req_done = false;
    srv->act_req_source_xid = (uint32_t)source_window;
    snprintf(srv->act_req_app_id, sizeof(srv->act_req_app_id), "%s", app_id ? app_id : "");
    srv->act_req_result[0] = '\0';

    _XmWlWakeServer(srv);

    while (!srv->act_req_done) {
        if (pthread_cond_timedwait(&srv->act_cond, &srv->act_lock, &deadline) == ETIMEDOUT)
            break;
    }

    Boolean ok = False;
    if (srv->act_req_done && srv->act_req_result[0] != '\0') {
        size_t len = strlen(srv->act_req_result) + 1;
        char *copy = XtMalloc((Cardinal)len);
        if (copy) {
            memcpy(copy, srv->act_req_result, len);
            *token_return = copy;
            ok = True;
        }
    }

    srv->act_req_in_flight = false;
    srv->act_req_done = false;
    pthread_cond_broadcast(&srv->act_cond);
    pthread_mutex_unlock(&srv->act_lock);
    return ok;
}

Boolean
XmWaylandGetDmabuf(
    Display *display,
    struct zwp_linux_dmabuf_v1 **dmabuf_return,
    unsigned int *version_return)
{
#ifdef HAVE_WAYLAND_DMABUF
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(display, srv))
        return False;
    Boolean ok = False;
    pthread_mutex_lock(&srv->lock);
    if (srv->dmabuf) {
        if (dmabuf_return) *dmabuf_return = srv->dmabuf;
        if (version_return) *version_return = (unsigned int)srv->dmabuf_version;
        ok = True;
    }
    pthread_mutex_unlock(&srv->lock);
    return ok;
#else
    (void)display;
    (void)dmabuf_return;
    (void)version_return;
    return False;
#endif
}

Boolean
XmWaylandHasDmabufFormat(
    Display *display,
    uint32_t drm_format,
    uint64_t modifier)
{
#ifdef HAVE_WAYLAND_DMABUF
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(display, srv))
        return False;
    Boolean found = False;
    pthread_mutex_lock(&srv->lock);
    if (srv->dmabuf) {
        for (size_t i = 0; i < srv->dmabuf_format_count; ++i) {
            if (srv->dmabuf_formats[i].format == drm_format &&
                (srv->dmabuf_formats[i].modifier == modifier ||
                 (srv->dmabuf_version < 3u &&
                  (modifier == 0u || modifier == XM_WL_DRM_FORMAT_MOD_INVALID)))) {
                found = True;
                break;
            }
        }
    }
    pthread_mutex_unlock(&srv->lock);
    return found;
#else
    (void)display;
    (void)drm_format;
    (void)modifier;
    return False;
#endif
}

Boolean
XmWaylandImportDmabuf(
    Display *display,
    const XmWaylandDmabufPlane *planes,
    int plane_count,
    int width,
    int height,
    uint32_t drm_format,
    uint32_t flags,
    struct wl_buffer **buffer_return)
{
#ifdef HAVE_WAYLAND_DMABUF
    if (!planes || plane_count < 1 || plane_count > 4 ||
        width <= 0 || height <= 0 || !buffer_return)
        return False;
    for (int i = 0; i < plane_count; ++i) {
        if (planes[i].fd < 0 || planes[i].stride == 0)
            return False;
    }
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(display, srv))
        return False;

    Boolean ok = False;
    pthread_mutex_lock(&srv->lock);
    if (srv->dmabuf && srv->wl_dpy) {
        struct zwp_linux_buffer_params_v1 *params =
            zwp_linux_dmabuf_v1_create_params(srv->dmabuf);
        if (params) {
            for (int i = 0; i < plane_count; ++i) {
                uint64_t eff_mod = (srv->dmabuf_version < 3u &&
                                    planes[i].modifier == XM_WL_DRM_FORMAT_MOD_INVALID)
                                       ? UINT64_C(0)
                                       : planes[i].modifier;
                uint32_t mod_hi = (uint32_t)(eff_mod >> 32);
                uint32_t mod_lo = (uint32_t)(eff_mod & 0xffffffffu);
                zwp_linux_buffer_params_v1_add(
                    params,
                    planes[i].fd,
                    (uint32_t)i,
                    planes[i].offset,
                    planes[i].stride,
                    mod_hi,
                    mod_lo);
            }
            struct wl_buffer *buf = zwp_linux_buffer_params_v1_create_immed(
                params, width, height, drm_format, flags);
            zwp_linux_buffer_params_v1_destroy(params);
            if (buf) {
                *buffer_return = buf;
                ok = True;
            }
        }
    }
    pthread_mutex_unlock(&srv->lock);
    return ok;
#else
    (void)display;
    (void)planes;
    (void)plane_count;
    (void)width;
    (void)height;
    (void)drm_format;
    (void)flags;
    (void)buffer_return;
    return False;
#endif
}

Boolean
XmWaylandSetSurfaceTearingHint(
    Display *display,
    struct wl_surface *surface,
    Boolean async_tearing)
{
    if (!surface)
        return False;
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(display, srv))
        return False;
    pthread_mutex_lock(&srv->lock);
    bool ok = apply_surface_tearing_locked(srv, surface, async_tearing != False);
    pthread_mutex_unlock(&srv->lock);
    return ok ? True : False;
}

Boolean
XmWaylandSetSurfaceFifo(
    Display *display,
    struct wl_surface *surface,
    Boolean set_barrier,
    Boolean wait_barrier)
{
    if (!surface)
        return False;
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(display, srv))
        return False;
    pthread_mutex_lock(&srv->lock);
    bool ok = apply_surface_fifo_locked(srv, surface, set_barrier != False, wait_barrier != False);
    pthread_mutex_unlock(&srv->lock);
    return ok ? True : False;
}

Boolean
XmWaylandSetSurfaceHdrMetadata(
    Display *display,
    struct wl_surface *surface,
    const XmWaylandHdrMetadata *metadata)
{
    if (!surface)
        return False;
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(display, srv))
        return False;
    pthread_mutex_lock(&srv->lock);
    bool ok = apply_surface_hdr_locked(srv, surface, metadata);
    pthread_mutex_unlock(&srv->lock);
    return ok ? True : False;
}

void
XmWaylandReleaseSurfaceExtensions(
    Display *display,
    struct wl_surface *surface)
{
    if (!surface)
        return;
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !display_matches_wayland_server(display, srv))
        return;
    pthread_mutex_lock(&srv->lock);
    release_surface_ext(srv, surface);
    pthread_mutex_unlock(&srv->lock);
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

Boolean
XmWaylandRequestActivationToken(
    Display *display,
    Window source_window,
    const char *app_id,
    char **token_return)
{
    (void)display;
    (void)source_window;
    (void)app_id;
    (void)token_return;
    return False;
}

Boolean
XmWaylandGetDmabuf(
    Display *display,
    struct zwp_linux_dmabuf_v1 **dmabuf_return,
    unsigned int *version_return)
{
    (void)display;
    (void)dmabuf_return;
    (void)version_return;
    return False;
}

Boolean
XmWaylandHasDmabufFormat(
    Display *display,
    uint32_t drm_format,
    uint64_t modifier)
{
    (void)display;
    (void)drm_format;
    (void)modifier;
    return False;
}

Boolean
XmWaylandImportDmabuf(
    Display *display,
    const XmWaylandDmabufPlane *planes,
    int plane_count,
    int width,
    int height,
    uint32_t drm_format,
    uint32_t flags,
    struct wl_buffer **buffer_return)
{
    (void)display;
    (void)planes;
    (void)plane_count;
    (void)width;
    (void)height;
    (void)drm_format;
    (void)flags;
    (void)buffer_return;
    return False;
}

Boolean
XmWaylandSetSurfaceTearingHint(
    Display *display,
    struct wl_surface *surface,
    Boolean async_tearing)
{
    (void)display;
    (void)surface;
    (void)async_tearing;
    return False;
}

Boolean
XmWaylandSetSurfaceFifo(
    Display *display,
    struct wl_surface *surface,
    Boolean set_barrier,
    Boolean wait_barrier)
{
    (void)display;
    (void)surface;
    (void)set_barrier;
    (void)wait_barrier;
    return False;
}

Boolean
XmWaylandSetSurfaceHdrMetadata(
    Display *display,
    struct wl_surface *surface,
    const XmWaylandHdrMetadata *metadata)
{
    (void)display;
    (void)surface;
    (void)metadata;
    return False;
}

void
XmWaylandReleaseSurfaceExtensions(
    Display *display,
    struct wl_surface *surface)
{
    (void)display;
    (void)surface;
}

#endif /* HAVE_WAYLAND_BACKEND */
