/*
 * Fuzz target: native Wayland backend X11r11 & XRender wire protocol server
 * (lib/Xm/WaylandX11Server.c and lib/Xm/WaylandRender.c, compiled unchanged).
 *
 * Feeds arbitrary client byte streams through _XmWlHandleClientReadable(),
 * exercising connection setup handshakes, BIG-REQUESTS framing, core X11
 * window/property/atom/pixmap/GC/drawing/image/cursor/selection requests, and
 * extension opcodes (RENDER 0.11, SHAPE, XFIXES, RANDR 1.5, XINERAMA, XC-MISC).
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HAVE_WAYLAND_BACKEND 1
#include "WaylandBackendI.h"

/* ---------- Stand-ins for WaylandBackend / WaylandData / WaylandIme ---------- */

uint32_t
_XmWlNowMs(void)
{
    return 1000u;
}

void
_XmWlSyncWindowRole(XmWlServer *srv, XmWlWindow *win)
{
    (void)srv;
    (void)win;
}

void
_XmWlRepositionPopup(XmWlServer *srv, XmWlWindow *win)
{
    (void)srv;
    (void)win;
}

void
_XmWlUnmapWindowRole(XmWlServer *srv, XmWlWindow *win)
{
    (void)srv;
    (void)win;
}

void
_XmWlDestroyWindowRole(XmWlServer *srv, XmWlWindow *win)
{
    (void)srv;
    if (!win)
        return;
    if (win->canvas) {
        pixman_image_unref(win->canvas);
        win->canvas = NULL;
    }
    free(win->canvas_bits);
    win->canvas_bits = NULL;
    win->canvas_w = 0;
    win->canvas_h = 0;
    win->canvas_cap_w = 0;
    win->canvas_cap_h = 0;
    win->canvas_stride = 0;
}

void
_XmWlUpdateWindowMetadata(XmWlServer *srv, XmWlWindow *win, uint32_t changed_atom)
{
    if (!srv || !win)
        return;
    (void)_XmWlAtomName(srv, changed_atom);
    (void)_XmWlGetProperty(win, changed_atom);
}

void
_XmWlUpdatePointerCursor(XmWlServer *srv, bool force)
{
    (void)srv;
    (void)force;
}

XmWlCursor *
_XmWlFindCursor(XmWlServer *srv, uint32_t id)
{
    if (!srv || id == 0)
        return NULL;
    for (XmWlCursor *c = srv->cursor_buckets[id & 0x7fu]; c; c = c->hash_next) {
        if (c->id == id)
            return c;
    }
    return NULL;
}

void
_XmWlFreeCursor(XmWlServer *srv, uint32_t id)
{
    if (!srv || id == 0)
        return;
    XmWlCursor **pp = &srv->cursor_buckets[id & 0x7fu];
    while (*pp) {
        if ((*pp)->id == id) {
            XmWlCursor *cur = *pp;
            *pp = cur->hash_next;
            free(cur->argb_pixels);
            free(cur);
            return;
        }
        pp = &(*pp)->hash_next;
    }
}

void
_XmWlRegisterGlyphCursor(XmWlServer *srv, uint32_t id, uint16_t glyph_char)
{
    if (!srv || id == 0)
        return;
    _XmWlFreeCursor(srv, id);
    XmWlCursor *c = (XmWlCursor *)calloc(1, sizeof(*c));
    if (!c)
        return;
    c->id = id;
    c->is_glyph = true;
    c->glyph_char = glyph_char;
    c->hash_next = srv->cursor_buckets[id & 0x7fu];
    srv->cursor_buckets[id & 0x7fu] = c;
}

void
_XmWlRegisterBitmapCursor(XmWlServer *srv, uint32_t id, uint16_t w, uint16_t h,
                          int16_t hot_x, int16_t hot_y, const uint32_t *argb)
{
    if (!srv || id == 0 || w == 0 || h == 0 || w > 256 || h > 256 || !argb)
        return;
    _XmWlFreeCursor(srv, id);
    XmWlCursor *c = (XmWlCursor *)calloc(1, sizeof(*c));
    if (!c)
        return;
    size_t n = (size_t)w * (size_t)h * sizeof(uint32_t);
    c->argb_pixels = (uint32_t *)malloc(n);
    if (!c->argb_pixels) {
        free(c);
        return;
    }
    memcpy(c->argb_pixels, argb, n);
    c->id = id;
    c->width = w;
    c->height = h;
    c->hot_x = hot_x;
    c->hot_y = hot_y;
    c->hash_next = srv->cursor_buckets[id & 0x7fu];
    srv->cursor_buckets[id & 0x7fu] = c;
}

void
_XmWlEnsureTopCanvas(XmWlServer *srv, XmWlWindow *top)
{
    (void)srv;
    if (!top)
        return;
    int w = top->width > 0 ? top->width : 1;
    int h = top->height > 0 ? top->height : 1;
    if (w > 256)
        w = 256;
    if (h > 256)
        h = 256;
    if (top->canvas && top->canvas_w == w && top->canvas_h == h)
        return;
    if (top->canvas) {
        pixman_image_unref(top->canvas);
        top->canvas = NULL;
    }
    free(top->canvas_bits);
    top->canvas_bits = (uint32_t *)calloc((size_t)w * (size_t)h, sizeof(uint32_t));
    if (!top->canvas_bits)
        return;
    top->canvas_w = w;
    top->canvas_h = h;
    top->canvas_cap_w = w;
    top->canvas_cap_h = h;
    top->canvas_stride = w * 4;
    top->canvas = pixman_image_create_bits(PIXMAN_a8r8g8b8, w, h, top->canvas_bits, top->canvas_stride);
}

void
_XmWlDamageTop(XmWlWindow *top, int x, int y, int w, int h)
{
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    if (top)
        top->dirty = true;
}

void
_XmWlIpcPublishRootProp(XmWlServer *srv, const char *atom_name, const uint8_t *data, size_t len)
{
    (void)srv;
    (void)atom_name;
    (void)data;
    (void)len;
}

void
_XmWlRequestSelfActivate(XmWlServer *srv, XmWlWindow *top)
{
    (void)srv;
    (void)top;
}

void
_XmWlDataHandleSetSelectionOwner(XmWlServer *srv, uint32_t selection, uint32_t owner, uint32_t time)
{
    (void)time;
    if (!srv)
        return;
    if (selection == 1u)
        srv->sel_primary_owner = owner;
    else
        srv->sel_clipboard_owner = owner;
}

uint32_t
_XmWlDataGetSelectionOwner(XmWlServer *srv, uint32_t selection)
{
    if (!srv)
        return 0;
    return (selection == 1u) ? srv->sel_primary_owner : srv->sel_clipboard_owner;
}

void
_XmWlDataHandleConvertSelection(XmWlServer *srv, int client_idx, uint32_t requestor,
                                uint32_t selection, uint32_t target, uint32_t property, uint32_t time)
{
    (void)srv;
    (void)client_idx;
    (void)requestor;
    (void)selection;
    (void)target;
    (void)property;
    (void)time;
}

bool
_XmWlDataInterceptSendEvent(XmWlServer *srv, uint32_t dest_win, const uint8_t ev[32])
{
    (void)srv;
    (void)dest_win;
    (void)ev;
    return false;
}

void
_XmWlImeSyncFocus(XmWlServer *srv)
{
    (void)srv;
}

/* ---------- Non-blocking empty socket & deterministic server teardown ---------- */

static int g_empty_sv[2] = { -1, -1 };

static void
ensure_empty_socketpair(void)
{
    if (g_empty_sv[0] >= 0)
        return;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, g_empty_sv) != 0)
        abort();
    int fl = fcntl(g_empty_sv[0], F_GETFL, 0);
    if (fl < 0 || fcntl(g_empty_sv[0], F_SETFL, fl | O_NONBLOCK) != 0)
        abort();
}

static void
free_all_server_resources(XmWlServer *srv)
{
    _XmWlDisconnectClient(srv, 0);

    for (size_t b = 0; b < 256; ++b) {
        while (srv->win_buckets[b]) {
            XmWlWindow *w = srv->win_buckets[b];
            srv->win_buckets[b] = w->hash_next;
            while (w->props) {
                XmWlProperty *p = w->props;
                w->props = p->next;
                free(p->data);
                free(p);
            }
            free(w->shape_rects);
            _XmWlDestroyWindowRole(srv, w);
            free(w);
        }
        while (srv->pix_buckets[b]) {
            XmWlPixmap *px = srv->pix_buckets[b];
            srv->pix_buckets[b] = px->hash_next;
            _XmWlFreePixmapData(px);
            free(px);
        }
        while (srv->gc_buckets[b]) {
            XmWlGC *gc = srv->gc_buckets[b];
            srv->gc_buckets[b] = gc->hash_next;
            free(gc->clip_rects);
            free(gc);
        }
        while (srv->pic_buckets[b]) {
            XmWlPicture *pic = srv->pic_buckets[b];
            srv->pic_buckets[b] = pic->hash_next;
            _XmWlFreePictureData(pic);
            free(pic);
        }
    }
    for (size_t b = 0; b < 64; ++b) {
        while (srv->font_buckets[b]) {
            XmWlFont *f = srv->font_buckets[b];
            srv->font_buckets[b] = f->hash_next;
            _XmWlFreeFontData(f);
            free(f);
        }
        while (srv->gs_buckets[b]) {
            XmWlGlyphSet *gs = srv->gs_buckets[b];
            srv->gs_buckets[b] = gs->hash_next;
            _XmWlFreeGlyphSetData(gs);
            free(gs);
        }
        while (srv->reg_buckets[b]) {
            XmWlRegion *r = srv->reg_buckets[b];
            srv->reg_buckets[b] = r->hash_next;
            pixman_region32_fini(&r->reg);
            free(r);
        }
    }
    for (size_t b = 0; b < 128; ++b) {
        while (srv->cursor_buckets[b]) {
            XmWlCursor *c = srv->cursor_buckets[b];
            srv->cursor_buckets[b] = c->hash_next;
            free(c->argb_pixels);
            free(c);
        }
    }
    for (uint32_t i = 1; i <= srv->atom_count && i < XM_WL_MAX_ATOMS; ++i) {
        free(srv->atoms[i]);
        srv->atoms[i] = NULL;
    }
}

int
LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    ensure_empty_socketpair();

    XmWlServer *srv = (XmWlServer *)calloc(1, sizeof(*srv));
    if (!srv)
        abort();

    srv->screen_w = 640;
    srv->screen_h = 480;
    srv->screen_mm_w = 169;
    srv->screen_mm_h = 127;
    srv->active_scale = 1.0;
    srv->logical_zoom = 1.0;
    srv->x11_focus_win = 1u;
    srv->x11_focus_revert = 1u;
    _XmWlInitAtomsAndRoot(srv);

    int cfd = dup(g_empty_sv[0]);
    if (cfd < 0) {
        free_all_server_resources(srv);
        free(srv);
        return 0;
    }

    XmWlClient *cli = &srv->clients[0];
    cli->active = true;
    cli->fd = cfd;
    cli->index = 0;
    cli->resource_base = 0x00200000u;
    /* Inputs starting with 'l' or 'B' exercise X11 connection setup framing;
     * all other inputs jump directly into request dispatch. */
    cli->handshake_done = !(size > 0 && (data[0] == 'l' || data[0] == 'B'));
    cli->rx_buf = (uint8_t *)malloc(size ? size : 1u);
    if (!cli->rx_buf) {
        free_all_server_resources(srv);
        free(srv);
        abort();
    }
    if (size > 0)
        memcpy(cli->rx_buf, data, size);
    cli->rx_len = size;
    cli->rx_cap = size ? size : 1u;

    _XmWlHandleClientReadable(srv, 0);

    /* Touch any queued reply/event/error bytes so uninitialized reply reads trip sanitizers */
    if (cli->active && cli->tx_buf && cli->tx_len > 0) {
        volatile uint8_t acc = 0;
        for (size_t i = 0; i < cli->tx_len; ++i)
            acc ^= cli->tx_buf[i];
        (void)acc;
    }

    free_all_server_resources(srv);
    free(srv);
    return 0;
}
