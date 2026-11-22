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
surf_enter(void *data, struct wl_surface *wl_surface, struct wl_output *output)
{
    (void)wl_surface;
    XmWlWindow *win = (XmWlWindow *)data;
    XmWlServer *srv = g_wl_srv;
    if (!win || !srv || !output) return;
    win->current_wl_output = output;
    if (!srv->compositor_native_dpi || win->is_popup || win->is_drag_icon)
        return;

    const char *oname = NULL;
    for (int i = 0; i < srv->output_count; ++i) {
        if (srv->outputs[i].wl_out == output && srv->outputs[i].name[0]) {
            oname = srv->outputs[i].name;
            break;
        }
    }
    if (!oname) return;

    for (int i = 0; i < srv->ipc_output_count; ++i) {
        const XmWlIpcOutput *io = &srv->ipc_outputs[i];
        if (!io->enabled || strcmp(io->name, oname) != 0)
            continue;
        int cx = (int)win->x + (int)win->width / 2;
        int cy = (int)win->y + (int)win->height / 2;
        if (cx < io->x || cx >= io->x + io->width ||
            cy < io->y || cy >= io->y + io->height) {
            win->x = (int16_t)(io->x + 32);
            win->y = (int16_t)(io->y + 32);
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
        break;
    }
}

static void
surf_leave(void *data, struct wl_surface *wl_surface, struct wl_output *output)
{
    (void)wl_surface;
    XmWlWindow *win = (XmWlWindow *)data;
    if (win && win->current_wl_output == output)
        win->current_wl_output = NULL;
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
            g_wl_srv->active_scale = (double)o->scale;
            if (g_wl_srv->root) {
                for (XmWlWindow *tw = g_wl_srv->root->first_child; tw; tw = tw->next_sibling)
                    _XmWlDamageTop(tw, 0, 0, tw->width, tw->height);
            }
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
        if (p && p->data && p->byte_len >= 4) {
            uint32_t parent_xid = 0;
            memcpy(&parent_xid, p->data, 4);
            if (parent_xid != win->sent_parent_xid) {
                XmWlWindow *ptop = _XmWlFindWindow(srv, parent_xid);
                if (ptop && ptop->xdg_top) {
                    win->sent_parent_xid = parent_xid;
                    xdg_toplevel_set_parent(win->xdg_top, ptop->xdg_top);
                    changed = true;
                }
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
    if (changed_atom == 0 || (a_mwm != 0 && changed_atom == a_mwm)) {
        XmWlProperty *p = a_mwm ? _XmWlGetProperty(win, a_mwm) : NULL;
        bool want_deco = true;
        if (p && p->data && p->byte_len >= 12) {
            uint32_t mflags = 0, mdeco = 1;
            memcpy(&mflags, p->data + 0, 4);
            memcpy(&mdeco, p->data + 8, 4);
            if ((mflags & 2u) && mdeco == 0)
                want_deco = false;
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
    }

    /* Dynamic window icon via xdg_toplevel_icon_v1 (xPropWMHints: flags@0, iconPixmap@12, iconMask@28) */
    if (srv->icon_mgr && srv->shm && (changed_atom == 0 || changed_atom == 35 /* XA_WM_HINTS */)) {
        uint32_t a_wm_hints = 35; /* XA_WM_HINTS */
        XmWlProperty *hp = _XmWlGetProperty(win, a_wm_hints);
        if (hp && hp->data && hp->byte_len >= 16) {
            uint32_t hflags = 0, icon_pix = 0, icon_mask = 0;
            memcpy(&hflags, hp->data + 0, 4);
            memcpy(&icon_pix, hp->data + 12, 4);
            if ((hflags & 32u) && hp->byte_len >= 32)
                memcpy(&icon_mask, hp->data + 28, 4);
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
                            changed = true;
                        }
                        close(ifd);
                    }
                    free(argb);
                }
            }
        }
    }
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
        if (sync_toplevel_metadata_all(srv, win, changed_atom) &&
            win->wl_surf && win->xdg_configured &&
            !win->resize_awaiting_client && !win->dirty) {
            wl_surface_commit(win->wl_surf);
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
            wl_surface_destroy(win->wl_surf);
            win->wl_surf = NULL;
        }
    }
    win->surf_listener_added = false;
    win->current_wl_output = NULL;
    win->xdg_configured = false;
    win->has_queued_wl_cfg = false;
    win->resize_awaiting_client = false;
    win->opaque_region_set = false;
    win->opaque_w = 0;
    win->opaque_h = 0;
    win->sent_title[0] = '\0';
    win->sent_app_id[0] = '\0';
    win->sent_parent_xid = 0;
    win->sent_min_w = win->sent_min_h = -1;
    win->sent_max_w = win->sent_max_h = -1;
    win->sent_deco_mode = -1;
    win->is_popup = false;
    win->is_drag_icon = false;
    win->popup_parent = NULL;
    for (int i = 0; i < srv->popup_depth; ++i) {
        if (srv->popup_stack[i] && srv->popup_stack[i]->popup_parent == win)
            srv->popup_stack[i]->popup_parent = NULL;
    }
}

void
_XmWlDestroyWindowRole(XmWlServer *srv, XmWlWindow *win)
{
    if (!win) return;
    _XmWlUnmapWindowRole(srv, win);
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

        int dx1 = w->dmg_x1 < 0 ? 0 : w->dmg_x1;
        int dy1 = w->dmg_y1 < 0 ? 0 : w->dmg_y1;
        int dx2 = w->dmg_x2 > log_w ? log_w : w->dmg_x2;
        int dy2 = w->dmg_y2 > log_h ? log_h : w->dmg_y2;
        if (dx2 <= dx1 || dy2 <= dy1) {
            dx1 = 0; dy1 = 0; dx2 = log_w; dy2 = log_h;
        }

        uint64_t next_gen = ++w->frame_gen;
        XmWlRect *cur_hist = &w->dmg_hist[next_gen % XM_WL_SHM_SLOTS];
        cur_hist->x = (int16_t)dx1;
        cur_hist->y = (int16_t)dy1;
        cur_hist->width = (uint16_t)(dx2 - dx1);
        cur_hist->height = (uint16_t)(dy2 - dy1);

        int cx1 = 0, cy1 = 0, cx2 = log_w, cy2 = log_h;
        if (!w->has_shape && sb->valid_w == log_w && sb->valid_h == log_h &&
            sb->valid_gen > 0 && (next_gen - sb->valid_gen) <= XM_WL_SHM_SLOTS) {
            cx1 = dx1; cy1 = dy1; cx2 = dx2; cy2 = dy2;
            for (uint64_t g = sb->valid_gen + 1; g < next_gen; ++g) {
                const XmWlRect *hr = &w->dmg_hist[g % XM_WL_SHM_SLOTS];
                int hx1 = hr->x;
                int hy1 = hr->y;
                int hx2 = hx1 + (int)hr->width;
                int hy2 = hy1 + (int)hr->height;
                if (hx1 < cx1) cx1 = hx1;
                if (hy1 < cy1) cy1 = hy1;
                if (hx2 > cx2) cx2 = hx2;
                if (hy2 > cy2) cy2 = hy2;
            }
            if (cx1 < 0) cx1 = 0;
            if (cy1 < 0) cy1 = 0;
            if (cx2 > log_w) cx2 = log_w;
            if (cy2 > log_h) cy2 = log_h;
        }

        const uint8_t *src_base = (const uint8_t *)w->canvas_bits;
        uint8_t *dst_base = (uint8_t *)sb->pixels;
        size_t copy_bytes = (size_t)(cx2 - cx1) * 4u;
        size_t x_byte_off = (size_t)cx1 * 4u;
        for (int ry = cy1; ry < cy2; ++ry) {
            memcpy(dst_base + (size_t)ry * (size_t)sb->stride + x_byte_off,
                   src_base + (size_t)ry * (size_t)w->canvas_stride + x_byte_off,
                   copy_bytes);
        }
        sb->valid_w = log_w;
        sb->valid_h = log_h;
        sb->valid_gen = next_gen;

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
        wl_surface_damage_buffer(w->wl_surf, dx1, dy1, dx2 - dx1, dy2 - dy1);
        wl_surface_commit(w->wl_surf);
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
        } else if (!strncmp(start, "PROP ", 5)) {
            char *p = start + 5;
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
                        _XmWlSetProperty(srv, srv->root, atom, ptype, 8, 0, raw, rlen, true);
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
    free(srv->ipc_rx_buf);
    if (srv->wake_pipe[0] >= 0) close(srv->wake_pipe[0]);
    if (srv->wake_pipe[1] >= 0) close(srv->wake_pipe[1]);
    if (srv->xt_pipe[0] >= 0) close(srv->xt_pipe[0]);
    if (srv->xt_pipe[1] >= 0) close(srv->xt_pipe[1]);
    if (srv->cursor_theme) {
        wl_cursor_theme_destroy(srv->cursor_theme);
        srv->cursor_theme = NULL;
    }
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
