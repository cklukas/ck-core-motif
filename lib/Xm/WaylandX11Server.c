/*
 * In-process X11r11 wire protocol virtualizer, subwindow hierarchy manager,
 * input/event dispatcher, and core extensions (BIG-REQUESTS, XC-MISC, SHAPE,
 * XFIXES, RANDR, XINERAMA) for the libXm Wayland backend.
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_WAYLAND_BACKEND

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "WaylandBackendI.h"

#include <ctype.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

/* X11 Event Masks */
#define X11_KEY_PRESS_MASK             (1u << 0)
#define X11_KEY_RELEASE_MASK           (1u << 1)
#define X11_BUTTON_PRESS_MASK          (1u << 2)
#define X11_BUTTON_RELEASE_MASK        (1u << 3)
#define X11_ENTER_WINDOW_MASK          (1u << 4)
#define X11_LEAVE_WINDOW_MASK          (1u << 5)
#define X11_POINTER_MOTION_MASK        (1u << 6)
#define X11_POINTER_MOTION_HINT_MASK   (1u << 7)
#define X11_BUTTON1_MOTION_MASK        (1u << 8)
#define X11_BUTTON2_MOTION_MASK        (1u << 9)
#define X11_BUTTON3_MOTION_MASK        (1u << 10)
#define X11_BUTTON4_MOTION_MASK        (1u << 11)
#define X11_BUTTON5_MOTION_MASK        (1u << 12)
#define X11_BUTTON_MOTION_MASK         (1u << 13)
#define X11_EXPOSURE_MASK              (1u << 15)
#define X11_VISIBILITY_CHANGE_MASK     (1u << 16)
#define X11_STRUCTURE_NOTIFY_MASK      (1u << 17)
#define X11_RESIZE_REDIRECT_MASK       (1u << 18)
#define X11_SUBSTRUCTURE_NOTIFY_MASK   (1u << 19)
#define X11_SUBSTRUCTURE_REDIRECT_MASK (1u << 20)
#define X11_FOCUS_CHANGE_MASK          (1u << 21)
#define X11_PROPERTY_CHANGE_MASK       (1u << 22)

static inline uint16_t rd_u16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static inline int16_t  rd_i16(const uint8_t *p) { int16_t v;  memcpy(&v, p, 2); return v; }
static inline uint32_t rd_u32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline int32_t  rd_i32(const uint8_t *p) { int32_t v;  memcpy(&v, p, 4); return v; }
static inline void     wr_u16(uint8_t *p, uint16_t v) { memcpy(p, &v, 2); }
static inline void     wr_i16(uint8_t *p, int16_t v)  { memcpy(p, &v, 2); }
static inline void     wr_u32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }
static inline void     wr_i32(uint8_t *p, int32_t v)  { memcpy(p, &v, 4); }

/* ---------- Predefined X11 Atoms (1..68) ---------- */

static const char *const g_predefined_atoms[69] = {
    NULL,
    "PRIMARY", "SECONDARY", "ARC", "ATOM", "BITMAP", "CARDINAL", "COLORMAP", "CURSOR",
    "CUT_BUFFER0", "CUT_BUFFER1", "CUT_BUFFER2", "CUT_BUFFER3",
    "CUT_BUFFER4", "CUT_BUFFER5", "CUT_BUFFER6", "CUT_BUFFER7",
    "DRAWABLE", "FONT", "INTEGER", "PIXMAP", "POINT", "RECTANGLE", "RESOURCE_MANAGER",
    "RGB_COLOR_MAP", "RGB_BEST_MAP", "RGB_BLUE_MAP", "RGB_DEFAULT_MAP",
    "RGB_GRAY_MAP", "RGB_GREEN_MAP", "RGB_RED_MAP",
    "STRING", "VISUALID", "WINDOW", "WM_COMMAND", "WM_HINTS", "WM_CLIENT_MACHINE",
    "WM_ICON_NAME", "WM_ICON_SIZE", "WM_NAME", "WM_NORMAL_HINTS", "WM_SIZE_HINTS",
    "WM_ZOOM_HINTS", "MIN_SPACE", "NORM_SPACE", "MAX_SPACE", "END_SPACE",
    "SUPERSCRIPT_X", "SUPERSCRIPT_Y", "SUBSCRIPT_X", "SUBSCRIPT_Y",
    "UNDERLINE_POSITION", "UNDERLINE_THICKNESS", "STRIKEOUT_ASCENT", "STRIKEOUT_DESCENT",
    "ITALIC_ANGLE", "X_HEIGHT", "QUAD_WIDTH", "WEIGHT", "POINT_SIZE",
    "RESOLUTION", "COPYRIGHT", "NOTICE", "FONT_NAME", "FAMILY_NAME",
    "FULL_NAME", "CAP_HEIGHT", "WM_CLASS", "WM_TRANSIENT_FOR"
};

uint32_t
_XmWlInternAtom(XmWlServer *srv, const char *name, size_t len, bool only_if_exists)
{
    if (!srv || !name || len == 0)
        return 0;
    for (uint32_t i = 1; i <= srv->atom_count; ++i) {
        if (srv->atoms[i] && strlen(srv->atoms[i]) == len &&
            memcmp(srv->atoms[i], name, len) == 0) {
            return i;
        }
    }
    if (only_if_exists || srv->atom_count + 1 >= XM_WL_MAX_ATOMS)
        return 0;

    char *copy = (char *)malloc(len + 1);
    if (!copy) return 0;
    memcpy(copy, name, len);
    copy[len] = '\0';
    uint32_t id = ++srv->atom_count;
    srv->atoms[id] = copy;
    return id;
}

const char *
_XmWlAtomName(XmWlServer *srv, uint32_t atom)
{
    if (!srv || atom == 0 || atom > srv->atom_count)
        return NULL;
    return srv->atoms[atom];
}

/* ---------- Resource Hash Lookups & Creation ---------- */

XmWlWindow *
_XmWlFindWindow(XmWlServer *srv, uint32_t xid)
{
    if (!srv || xid == 0) return NULL;
    for (XmWlWindow *w = srv->win_buckets[xid & 0xffu]; w; w = w->hash_next) {
        if (w->id == xid) return w;
    }
    return NULL;
}

XmWlPixmap *
_XmWlFindPixmap(XmWlServer *srv, uint32_t xid)
{
    if (!srv || xid == 0) return NULL;
    for (XmWlPixmap *p = srv->pix_buckets[xid & 0xffu]; p; p = p->hash_next) {
        if (p->id == xid) return p;
    }
    return NULL;
}

XmWlGC *
_XmWlFindGC(XmWlServer *srv, uint32_t xid)
{
    if (!srv || xid == 0) return NULL;
    for (XmWlGC *g = srv->gc_buckets[xid & 0xffu]; g; g = g->hash_next) {
        if (g->id == xid) return g;
    }
    return NULL;
}

XmWlFont *
_XmWlFindFont(XmWlServer *srv, uint32_t xid)
{
    if (!srv || xid == 0) return NULL;
    for (XmWlFont *f = srv->font_buckets[xid & 0x3fu]; f; f = f->hash_next) {
        if (f->id == xid) return f;
    }
    return NULL;
}

XmWlPicture *
_XmWlFindPicture(XmWlServer *srv, uint32_t xid)
{
    if (!srv || xid == 0) return NULL;
    for (XmWlPicture *p = srv->pic_buckets[xid & 0xffu]; p; p = p->hash_next) {
        if (p->id == xid) return p;
    }
    return NULL;
}

XmWlGlyphSet *
_XmWlFindGlyphSet(XmWlServer *srv, uint32_t xid)
{
    if (!srv || xid == 0) return NULL;
    for (XmWlGlyphSet *gs = srv->gs_buckets[xid & 0x3fu]; gs; gs = gs->hash_next) {
        if (gs->id == xid) return gs;
    }
    return NULL;
}

XmWlRegion *
_XmWlFindRegion(XmWlServer *srv, uint32_t xid)
{
    if (!srv || xid == 0) return NULL;
    for (XmWlRegion *r = srv->reg_buckets[xid & 0x3fu]; r; r = r->hash_next) {
        if (r->id == xid) return r;
    }
    return NULL;
}

static XmWlWindow *
alloc_window(XmWlServer *srv, uint32_t id, XmWlWindow *parent)
{
    XmWlWindow *w = (XmWlWindow *)calloc(1, sizeof(*w));
    if (!w) return NULL;
    w->id = id;
    w->parent = parent;
    w->width = 1;
    w->height = 1;
    w->depth = parent ? parent->depth : 24;
    w->win_class = 1;
    w->visual_id = (w->depth == 32) ? XM_WL_VISUAL_ARGB32_ID : XM_WL_VISUAL_RGB24_ID;
    w->colormap_id = XM_WL_DEFAULT_CMAP_XID;
    w->sent_min_w = w->sent_min_h = -1;
    w->sent_max_w = w->sent_max_h = -1;
    w->sent_deco_mode = -1;
    w->preferred_scale = srv->active_scale > 0.0 ? srv->active_scale : 1.0;
    for (int si = 0; si < XM_WL_SHM_SLOTS; ++si)
        w->shm_bufs[si].fd = -1;

    if (parent) {
        /* X11 CreateWindow places new child at top of sibling stack (last_child) */
        w->prev_sibling = parent->last_child;
        if (parent->last_child)
            parent->last_child->next_sibling = w;
        else
            parent->first_child = w;
        parent->last_child = w;
    }

    uint32_t b = id & 0xffu;
    w->hash_next = srv->win_buckets[b];
    srv->win_buckets[b] = w;
    return w;
}

XmWlWindow *
_XmWlEnclosingTopLevel(XmWlServer *srv, XmWlWindow *win, int *out_dx, int *out_dy)
{
    int dx = 0, dy = 0;
    if (!srv || !win || win == srv->root) {
        if (out_dx) *out_dx = 0;
        if (out_dy) *out_dy = 0;
        return NULL;
    }
    XmWlWindow *cur = win;
    while (cur->parent && cur->parent != srv->root) {
        dx += (int)cur->x + (int)cur->border_width;
        dy += (int)cur->y + (int)cur->border_width;
        cur = cur->parent;
    }
    if (cur->parent != srv->root) {
        if (out_dx) *out_dx = 0;
        if (out_dy) *out_dy = 0;
        return NULL;
    }
    if (out_dx) *out_dx = dx;
    if (out_dy) *out_dy = dy;
    return cur;
}

static bool
is_window_viewable(XmWlServer *srv, const XmWlWindow *w)
{
    if (!w) return false;
    for (const XmWlWindow *c = w; c && c != srv->root; c = c->parent) {
        if (!c->mapped)
            return false;
    }
    return true;
}

void
_XmWlInitAtomsAndRoot(XmWlServer *srv)
{
    if (!srv) return;
    for (uint32_t i = 1; i <= 68; ++i) {
        srv->atoms[i] = strdup(g_predefined_atoms[i]);
        srv->atom_count = i;
    }

    srv->root = alloc_window(srv, XM_WL_ROOT_XID, NULL);
    srv->root->x = 0;
    srv->root->y = 0;
    srv->root->width = (uint16_t)srv->screen_w;
    srv->root->height = (uint16_t)srv->screen_h;
    srv->root->depth = 24;
    srv->root->visual_id = XM_WL_VISUAL_RGB24_ID;
    srv->root->mapped = true;
    srv->root->has_bg_pixel = true;
    srv->root->bg_pixel = 0x004a6b82u;

    srv->wm_helper = alloc_window(srv, XM_WL_WM_HELPER_XID, srv->root);
    srv->sm_helper = alloc_window(srv, XM_WL_SM_HELPER_XID, srv->root);
    srv->proxy_sel = alloc_window(srv, XM_WL_PROXY_SEL_XID, srv->root);

    /* Advertise Motif WM presence (_MOTIF_WM_INFO) so shell negotiation succeeds */
    uint32_t a_mwm_info = _XmWlInternAtom(srv, "_MOTIF_WM_INFO", 14, false);
    uint32_t mwm_info[2] = { 2u /* MWM_INFO_STARTUP_CUSTOM */, XM_WL_WM_HELPER_XID };
    _XmWlSetProperty(srv, srv->root, a_mwm_info, a_mwm_info, 32, 0,
                     (const uint8_t *)mwm_info, sizeof(mwm_info), false);

    /* Advertise EWMH compositing manager (_NET_WM_CM_S0) so DragC uses ARGB XmDRAG_WINDOW */
    uint32_t a_cm = _XmWlInternAtom(srv, "_NET_WM_CM_S0", 13, false);
    (void)a_cm;

    /* Seed ToolTalk session atoms (_SUN_TT_SESSION / TT_SESSION) as XA_STRING (31)
     * from process environment so tt_X_session(XDisplayString(dpy)) succeeds immediately. */
    const char *tt_sess = getenv("TT_SESSION");
    if (!tt_sess || !tt_sess[0])
        tt_sess = getenv("_SUN_TT_SESSION");
    if (tt_sess && tt_sess[0]) {
        size_t tt_len = strlen(tt_sess);
        uint32_t a_sun_tt = _XmWlInternAtom(srv, "_SUN_TT_SESSION", 15, false);
        uint32_t a_tt = _XmWlInternAtom(srv, "TT_SESSION", 10, false);
        _XmWlSetProperty(srv, srv->root, a_sun_tt, 31u /* XA_STRING */, 8, 0,
                         (const uint8_t *)tt_sess, tt_len, false);
        _XmWlSetProperty(srv, srv->root, a_tt, 31u /* XA_STRING */, 8, 0,
                         (const uint8_t *)tt_sess, tt_len, false);
    }

    /* Default font fallback at XID 0x00000070 */
    (void)_XmWlOpenFont(srv, 0x00000070u, "fixed", 5);
}

/* ---------- Properties ---------- */

XmWlProperty *
_XmWlGetProperty(XmWlWindow *win, uint32_t atom)
{
    if (!win || atom == 0) return NULL;
    for (XmWlProperty *p = win->props; p; p = p->next) {
        if (p->atom == atom) return p;
    }
    return NULL;
}

static void
emit_property_notify(XmWlServer *srv, XmWlWindow *win, uint32_t atom, uint8_t state)
{
    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = 28; /* PropertyNotify */
    wr_u32(ev + 4, win->id);
    wr_u32(ev + 8, atom);
    wr_u32(ev + 12, _XmWlNowMs());
    ev[16] = state; /* 0=NewValue, 1=Delete */
    _XmWlSendEventToWindow(srv, win, X11_PROPERTY_CHANGE_MASK, ev);
}

void
_XmWlSetProperty(XmWlServer *srv, XmWlWindow *win, uint32_t atom, uint32_t type,
                 uint8_t format, uint8_t mode, const uint8_t *data, size_t byte_len,
                 bool emit_notify)
{
    if (!srv || !win || atom == 0) return;
    if (format != 8 && format != 16 && format != 32)
        return;

    XmWlProperty *p = _XmWlGetProperty(win, atom);
    if (!p) {
        p = (XmWlProperty *)calloc(1, sizeof(*p));
        if (!p) return;
        p->atom = atom;
        p->next = win->props;
        win->props = p;
        mode = 0; /* PropModeReplace */
    }

    if (mode == 0 /* Replace */) {
        uint8_t *nb = byte_len > 0 ? (uint8_t *)malloc(byte_len) : NULL;
        if (byte_len > 0 && !nb) return;
        if (nb && data) memcpy(nb, data, byte_len);
        free(p->data);
        p->data = nb;
        p->byte_len = byte_len;
        p->type = type;
        p->format = format;
    } else if (mode == 1 /* Prepend */ && p->format == format) {
        size_t nl = p->byte_len + byte_len;
        uint8_t *nb = nl > 0 ? (uint8_t *)malloc(nl) : NULL;
        if (nl > 0 && !nb) return;
        if (byte_len > 0 && data) memcpy(nb, data, byte_len);
        if (p->byte_len > 0 && p->data) memcpy(nb + byte_len, p->data, p->byte_len);
        free(p->data);
        p->data = nb;
        p->byte_len = nl;
    } else if (mode == 2 /* Append */ && p->format == format) {
        size_t nl = p->byte_len + byte_len;
        uint8_t *nb = nl > 0 ? (uint8_t *)realloc(p->data, nl) : NULL;
        if (nl > 0 && !nb) return;
        if (byte_len > 0 && data) memcpy(nb + p->byte_len, data, byte_len);
        p->data = nb;
        p->byte_len = nl;
    }

    if (emit_notify) {
        emit_property_notify(srv, win, atom, 0);
        _XmWlUpdateWindowMetadata(srv, win, atom);
    }
}

void
_XmWlDeleteProperty(XmWlServer *srv, XmWlWindow *win, uint32_t atom, bool emit_notify)
{
    if (!srv || !win || atom == 0) return;
    XmWlProperty **pp = &win->props;
    while (*pp) {
        if ((*pp)->atom == atom) {
            XmWlProperty *victim = *pp;
            *pp = victim->next;
            free(victim->data);
            free(victim);
            if (emit_notify)
                emit_property_notify(srv, win, atom, 1);
            return;
        }
        pp = &(*pp)->next;
    }
}

/* ---------- Wire Transmit Buffering ---------- */

static void
queue_tx_bytes(XmWlClient *cli, const void *data, size_t len)
{
    if (!cli || !cli->active || len == 0) return;
    if (cli->tx_len + len > cli->tx_cap) {
        size_t ncap = cli->tx_cap ? cli->tx_cap * 2 : 16384;
        while (ncap < cli->tx_len + len)
            ncap *= 2;
        uint8_t *nb = (uint8_t *)realloc(cli->tx_buf, ncap);
        if (!nb) return;
        cli->tx_buf = nb;
        cli->tx_cap = ncap;
    }
    memcpy(cli->tx_buf + cli->tx_len, data, len);
    cli->tx_len += len;
}

void
_XmWlFlushClientWrites(XmWlServer *srv)
{
    if (!srv) return;
    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        XmWlClient *cli = &srv->clients[i];
        if (!cli->active || cli->tx_len == 0 || cli->fd < 0)
            continue;
        size_t off = 0;
        while (off < cli->tx_len) {
            ssize_t n = send(cli->fd, cli->tx_buf + off, cli->tx_len - off, MSG_NOSIGNAL);
            if (n > 0) {
                off += (size_t)n;
            } else if (n < 0 && errno == EINTR) {
                continue;
            } else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                break;
            } else {
                _XmWlDisconnectClient(srv, i);
                off = 0;
                break;
            }
        }
        if (cli->active && off > 0) {
            if (off < cli->tx_len) {
                if (cli->tx_pkt_aligned) {
                    size_t pos = 0;
                    while (pos < off && pos + 32u <= cli->tx_len) {
                        size_t plen = (cli->tx_buf[pos] == 1u)
                            ? (32u + 4u * (size_t)rd_u32(cli->tx_buf + pos + 4))
                            : 32u;
                        if (plen < 32u) { pos = cli->tx_len + 1u; break; }
                        pos += plen;
                    }
                    cli->tx_pkt_aligned = (pos == off);
                }
                memmove(cli->tx_buf, cli->tx_buf + off, cli->tx_len - off);
                cli->tx_len -= off;
            } else {
                cli->tx_len = 0;
                if (cli->handshake_done)
                    cli->tx_pkt_aligned = true;
            }
        }
    }
}

void
_XmWlSendReply(XmWlClient *cli, const uint8_t hdr[32], const void *extra, size_t extra_len)
{
    if (!cli || !cli->active) return;
    size_t padded_extra = (extra_len + 3u) & ~3u;
    uint8_t out[32];
    memcpy(out, hdr, 32);
    out[0] = 1; /* X_Reply */
    wr_u16(out + 2, cli->seq);
    wr_u32(out + 4, (uint32_t)(padded_extra / 4u));
    queue_tx_bytes(cli, out, 32);
    if (extra_len > 0 && extra)
        queue_tx_bytes(cli, extra, extra_len);
    if (padded_extra > extra_len) {
        static const uint8_t zero[4] = {0, 0, 0, 0};
        queue_tx_bytes(cli, zero, padded_extra - extra_len);
    }
}

void
_XmWlSendError(XmWlClient *cli, uint8_t err_code, uint32_t bad_val, uint16_t minor_op, uint8_t major_op)
{
    if (!cli || !cli->active) return;
    uint8_t err[32];
    memset(err, 0, sizeof(err));
    err[0] = 0; /* X_Error */
    err[1] = err_code;
    wr_u16(err + 2, cli->seq);
    wr_u32(err + 4, bad_val);
    wr_u16(err + 8, minor_op);
    err[10] = major_op;
    queue_tx_bytes(cli, err, 32);
}

void
_XmWlSendEventToClient(XmWlClient *cli, const uint8_t ev[32])
{
    if (!cli || !cli->active || !cli->handshake_done) return;
    if (cli->tx_len == 0)
        cli->tx_pkt_aligned = true;

    /* Coalesce back-to-back unsent full-window Expose (0,0,w,h,count=0) events
     * for the same window XID so rapid resize/layout batches never queue
     * redundant full-widget redraws. */
    if ((ev[0] & 0x7fu) == 12u && cli->tx_pkt_aligned && cli->tx_len >= 32u &&
        rd_u16(ev + 8) == 0u && rd_u16(ev + 10) == 0u && rd_u16(ev + 16) == 0u) {
        uint32_t target_wid = rd_u32(ev + 4);
        size_t pos = 0;
        while (pos + 32u <= cli->tx_len) {
            uint8_t *p = cli->tx_buf + pos;
            size_t plen = (p[0] == 1u) ? (32u + 4u * (size_t)rd_u32(p + 4)) : 32u;
            if (plen < 32u || pos + plen > cli->tx_len)
                break;
            if ((p[0] & 0x7fu) == 12u && rd_u32(p + 4) == target_wid &&
                rd_u16(p + 8) == 0u && rd_u16(p + 10) == 0u && rd_u16(p + 16) == 0u) {
                memmove(p, p + 32u, cli->tx_len - (pos + 32u));
                cli->tx_len -= 32u;
                continue;
            }
            pos += plen;
        }
    }

    uint8_t out[32];
    memcpy(out, ev, 32);
    if ((out[0] & 0x7fu) != 11) /* KeymapNotify has no sequence field */
        wr_u16(out + 2, cli->seq);
    queue_tx_bytes(cli, out, 32);
}

void
_XmWlSendEventToWindow(XmWlServer *srv, XmWlWindow *win, uint32_t mask, const uint8_t ev[32])
{
    if (!srv || !win) return;
    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        XmWlClient *cli = &srv->clients[i];
        if (!cli->active || !cli->handshake_done) continue;
        if (mask == 0 || (win->event_masks[i] & mask) != 0)
            _XmWlSendEventToClient(cli, ev);
    }
}

void
_XmWlDisconnectClient(XmWlServer *srv, int client_idx)
{
    if (!srv || client_idx < 0 || client_idx >= XM_WL_MAX_CLIENTS)
        return;
    XmWlClient *cli = &srv->clients[client_idx];
    if (!cli->active) return;

    if (cli->fd >= 0) {
        close(cli->fd);
        cli->fd = -1;
    }
    free(cli->rx_buf);
    free(cli->tx_buf);
    memset(cli, 0, sizeof(*cli));
    cli->fd = -1;

    if (srv->ptr_grab_active && srv->ptr_grab_client == client_idx) {
        srv->ptr_grab_active = false;
        srv->ptr_grab_win = 0;
    }
}

/* ---------- Named X11 Color Lookup ---------- */

typedef struct {
    char name[48];
    uint8_t r, g, b;
} NamedRgb;

static NamedRgb *g_rgb_db = NULL;
static size_t g_rgb_db_count = 0;
static bool g_rgb_db_loaded = false;

static void
normalize_color_key(const char *src, size_t len, char *dst, size_t dst_sz)
{
    size_t j = 0;
    for (size_t i = 0; i < len && j + 1 < dst_sz; ++i) {
        unsigned char c = (unsigned char)src[i];
        if (c == ' ' || c == '\t' || c == '_' || c == '-')
            continue;
        dst[j++] = (char)tolower(c);
    }
    dst[j] = '\0';
}

static void
ensure_rgb_db_loaded(void)
{
    if (g_rgb_db_loaded) return;
    g_rgb_db_loaded = true;

    const char *paths[] = {
        "/usr/share/X11/rgb.txt",
        "/etc/X11/rgb.txt",
        "/usr/lib/X11/rgb.txt",
    };
    FILE *fp = NULL;
    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        fp = fopen(paths[i], "r");
        if (fp) break;
    }
    if (!fp) return;

    size_t cap = 1024;
    g_rgb_db = (NamedRgb *)malloc(cap * sizeof(NamedRgb));
    if (!g_rgb_db) {
        fclose(fp);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '!' || line[0] == '#' || line[0] == '\n')
            continue;
        int r = 0, g = 0, b = 0, consumed = 0;
        if (sscanf(line, "%d %d %d %n", &r, &g, &b, &consumed) == 3 && consumed > 0) {
            char *nm = line + consumed;
            size_t nl = strcspn(nm, "\r\n");
            if (nl == 0) continue;
            if (g_rgb_db_count >= cap) {
                size_t ncap = cap * 2;
                NamedRgb *nb = (NamedRgb *)realloc(g_rgb_db, ncap * sizeof(NamedRgb));
                if (!nb) break;
                g_rgb_db = nb;
                cap = ncap;
            }
            NamedRgb *e = &g_rgb_db[g_rgb_db_count++];
            normalize_color_key(nm, nl, e->name, sizeof(e->name));
            e->r = (uint8_t)(r < 0 ? 0 : (r > 255 ? 255 : r));
            e->g = (uint8_t)(g < 0 ? 0 : (g > 255 ? 255 : g));
            e->b = (uint8_t)(b < 0 ? 0 : (b > 255 ? 255 : b));
        }
    }
    fclose(fp);
}

static int
hex_val(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static bool
parse_hex_component(const char *s, size_t len, uint16_t *out16)
{
    if (len == 0 || len > 4) return false;
    uint32_t v = 0;
    for (size_t i = 0; i < len; ++i) {
        int h = hex_val(s[i]);
        if (h < 0) return false;
        v = (v << 4) | (uint32_t)h;
    }
    if (len == 1) *out16 = (uint16_t)(v * 0x1111u);
    else if (len == 2) *out16 = (uint16_t)(v * 0x0101u);
    else if (len == 3) *out16 = (uint16_t)((v << 4) | (v >> 8));
    else *out16 = (uint16_t)v;
    return true;
}

static bool
lookup_x11_color(const char *name, size_t len, uint16_t *r16, uint16_t *g16, uint16_t *b16)
{
    if (!name || len == 0) return false;

    if (name[0] == '#') {
        size_t hex_len = len - 1;
        if (hex_len == 3 || hex_len == 6 || hex_len == 9 || hex_len == 12) {
            size_t c = hex_len / 3;
            return parse_hex_component(name + 1, c, r16) &&
                   parse_hex_component(name + 1 + c, c, g16) &&
                   parse_hex_component(name + 1 + 2 * c, c, b16);
        }
        return false;
    }

    if (len > 4 && !strncasecmp(name, "rgb:", 4)) {
        const char *p1 = name + 4;
        const char *end = name + len;
        const char *s1 = (const char *)memchr(p1, '/', (size_t)(end - p1));
        if (!s1) return false;
        const char *p2 = s1 + 1;
        const char *s2 = (const char *)memchr(p2, '/', (size_t)(end - p2));
        if (!s2) return false;
        const char *p3 = s2 + 1;
        return parse_hex_component(p1, (size_t)(s1 - p1), r16) &&
               parse_hex_component(p2, (size_t)(s2 - p2), g16) &&
               parse_hex_component(p3, (size_t)(end - p3), b16);
    }

    char key[64];
    normalize_color_key(name, len, key, sizeof(key));

    /* grayN / greyN (0..100) */
    if ((!strncmp(key, "gray", 4) || !strncmp(key, "grey", 4)) && isdigit((unsigned char)key[4])) {
        int pct = atoi(key + 4);
        if (pct >= 0 && pct <= 100) {
            uint8_t v = (uint8_t)((pct * 255 + 50) / 100);
            *r16 = *g16 = *b16 = (uint16_t)(v * 0x0101u);
            return true;
        }
    }

    ensure_rgb_db_loaded();
    for (size_t i = 0; i < g_rgb_db_count; ++i) {
        if (!strcmp(g_rgb_db[i].name, key)) {
            *r16 = (uint16_t)(g_rgb_db[i].r * 0x0101u);
            *g16 = (uint16_t)(g_rgb_db[i].g * 0x0101u);
            *b16 = (uint16_t)(g_rgb_db[i].b * 0x0101u);
            return true;
        }
    }

    /* Built-in fallback essentials if /usr/share/X11/rgb.txt is unavailable */
    static const struct { const char *k; uint8_t r, g, b; } builtins[] = {
        {"black", 0, 0, 0}, {"white", 255, 255, 255}, {"red", 255, 0, 0},
        {"green", 0, 255, 0}, {"blue", 0, 0, 255}, {"yellow", 255, 255, 0},
        {"cyan", 0, 255, 255}, {"magenta", 255, 0, 255}, {"gray", 190, 190, 190},
        {"grey", 190, 190, 190}, {"lightgray", 211, 211, 211}, {"lightgrey", 211, 211, 211},
        {"darkgray", 169, 169, 169}, {"darkgrey", 169, 169, 169}, {"dimgray", 105, 105, 105},
        {"orange", 255, 165, 0}, {"brown", 165, 42, 42}, {"wheat", 245, 222, 179},
        {"bisque", 255, 228, 196}, {"beige", 245, 245, 220}, {"navy", 0, 0, 128},
        {"steelblue", 70, 130, 180}, {"cadetblue", 95, 158, 160}, {"gold", 255, 215, 0},
    };
    for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); ++i) {
        if (!strcmp(builtins[i].k, key)) {
            *r16 = (uint16_t)(builtins[i].r * 0x0101u);
            *g16 = (uint16_t)(builtins[i].g * 0x0101u);
            *b16 = (uint16_t)(builtins[i].b * 0x0101u);
            return true;
        }
    }
    return false;
}

/* ---------- Window Destruction & Exposure Helpers ---------- */

static void
emit_expose_single(XmWlServer *srv, XmWlWindow *win)
{
    if (!win || !win->mapped || win->win_class != 1)
        return;
    _XmWlPaintWindowBackground(srv, win, 0, 0, win->width, win->height);
    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = 12; /* Expose */
    wr_u32(ev + 4, win->id);
    wr_u16(ev + 8, 0);
    wr_u16(ev + 10, 0);
    wr_u16(ev + 12, win->width);
    wr_u16(ev + 14, win->height);
    wr_u16(ev + 16, 0);
    _XmWlSendEventToWindow(srv, win, X11_EXPOSURE_MASK, ev);
}

static void
emit_expose_recursive(XmWlServer *srv, XmWlWindow *win)
{
    if (!win || !win->mapped) return;
    emit_expose_single(srv, win);
    for (XmWlWindow *c = win->first_child; c; c = c->next_sibling)
        emit_expose_recursive(srv, c);
}

static void
emit_expose_uncovered_descendants(XmWlServer *srv, XmWlWindow *anc,
                                  int rel_ox, int rel_oy, int old_w, int old_h)
{
    if (!anc || !anc->mapped)
        return;
    for (XmWlWindow *c = anc->first_child; c; c = c->next_sibling) {
        if (!c->mapped)
            continue;
        int cx = rel_ox + (int)c->x + (int)c->border_width;
        int cy = rel_oy + (int)c->y + (int)c->border_width;
        if (cx + (int)c->width > old_w || cy + (int)c->height > old_h)
            emit_expose_single(srv, c);
        emit_expose_uncovered_descendants(srv, c, cx, cy, old_w, old_h);
    }
}

static void
expose_uncovered_siblings(XmWlServer *srv, XmWlWindow *w,
                          int ox, int oy, int ow, int oh, bool all_siblings)
{
    if (!srv || !w || !w->parent || w->parent == srv->root || ow <= 0 || oh <= 0)
        return;
    int ox2 = ox + ow;
    int oy2 = oy + oh;
    for (XmWlWindow *sib = w->parent->first_child; sib; sib = sib->next_sibling) {
        if (sib == w) {
            if (!all_siblings)
                break;
            continue;
        }
        if (!sib->mapped)
            continue;
        int sx1 = (int)sib->x;
        int sy1 = (int)sib->y;
        int sx2 = sx1 + (int)sib->width + 2 * (int)sib->border_width;
        int sy2 = sy1 + (int)sib->height + 2 * (int)sib->border_width;
        if (sx1 < ox2 && sx2 > ox && sy1 < oy2 && sy2 > oy)
            emit_expose_recursive(srv, sib);
    }
}

static void
destroy_window_tree(XmWlServer *srv, XmWlWindow *win, bool emit_events)
{
    if (!srv || !win || win == srv->root || win == srv->wm_helper ||
        win == srv->sm_helper || win == srv->proxy_sel)
        return;

    while (win->first_child)
        destroy_window_tree(srv, win->first_child, emit_events);

    if (srv->hovered_win == win) srv->hovered_win = NULL;
    if (srv->x11_focus_win == win->id) srv->x11_focus_win = 1u;
    if (srv->ptr_grab_active && srv->ptr_grab_win == win->id) {
        srv->ptr_grab_active = false;
        srv->ptr_grab_win = 0;
    }

    _XmWlDestroyWindowRole(srv, win);

    if (emit_events) {
        uint8_t ev[32];
        memset(ev, 0, sizeof(ev));
        ev[0] = 17; /* DestroyNotify */
        wr_u32(ev + 4, win->id);
        wr_u32(ev + 8, win->id);
        _XmWlSendEventToWindow(srv, win, X11_STRUCTURE_NOTIFY_MASK, ev);
        if (win->parent) {
            wr_u32(ev + 4, win->parent->id);
            _XmWlSendEventToWindow(srv, win->parent, X11_SUBSTRUCTURE_NOTIFY_MASK, ev);
        }
    }

    /* Unlink from parent */
    if (win->parent) {
        if (win->prev_sibling) win->prev_sibling->next_sibling = win->next_sibling;
        else win->parent->first_child = win->next_sibling;
        if (win->next_sibling) win->next_sibling->prev_sibling = win->prev_sibling;
        else win->parent->last_child = win->prev_sibling;
    }

    /* Unlink from hash bucket */
    XmWlWindow **bp = &srv->win_buckets[win->id & 0xffu];
    while (*bp) {
        if (*bp == win) {
            *bp = win->hash_next;
            break;
        }
        bp = &(*bp)->hash_next;
    }

    /* Free properties & shape */
    XmWlProperty *p = win->props;
    while (p) {
        XmWlProperty *nx = p->next;
        free(p->data);
        free(p);
        p = nx;
    }
    free(win->shape_rects);
    free(win);
}

/* ---------- Hit Testing & Input Event Synthesis ---------- */

static XmWlWindow *
hit_test_child_at(XmWlServer *srv, XmWlWindow *win, int lx, int ly, int *out_wx, int *out_wy)
{
    if (!win || !win->mapped) return NULL;
    for (XmWlWindow *c = win->last_child; c; c = c->prev_sibling) {
        if (!c->mapped) continue;
        int cx1 = (int)c->x;
        int cy1 = (int)c->y;
        int cw = (int)c->width + 2 * (int)c->border_width;
        int ch = (int)c->height + 2 * (int)c->border_width;
        if (lx >= cx1 && ly >= cy1 && lx < cx1 + cw && ly < cy1 + ch) {
            int sub_x = lx - cx1 - (int)c->border_width;
            int sub_y = ly - cy1 - (int)c->border_width;
            if (c->has_shape) {
                bool inside_shape = false;
                for (size_t s = 0; s < c->shape_rect_count; ++s) {
                    const XmWlRect *sr = &c->shape_rects[s];
                    if (sub_x >= sr->x && sub_y >= sr->y &&
                        sub_x < sr->x + (int)sr->width && sub_y < sr->y + (int)sr->height) {
                        inside_shape = true;
                        break;
                    }
                }
                if (!inside_shape) continue;
            }
            return hit_test_child_at(srv, c, sub_x, sub_y, out_wx, out_wy);
        }
    }
    *out_wx = lx;
    *out_wy = ly;
    return win;
}

static void
window_to_root_coords(XmWlServer *srv, const XmWlWindow *win, int wx, int wy, int *rx, int *ry)
{
    int x = wx, y = wy;
    for (const XmWlWindow *c = win; c && c != srv->root; c = c->parent) {
        x += (int)c->x + (int)c->border_width;
        y += (int)c->y + (int)c->border_width;
    }
    *rx = x;
    *ry = y;
}

static void
root_to_window_coords(XmWlServer *srv, const XmWlWindow *win, int rx, int ry, int *wx, int *wy)
{
    int ox = 0, oy = 0;
    window_to_root_coords(srv, win, 0, 0, &ox, &oy);
    *wx = rx - ox;
    *wy = ry - oy;
}

static uint16_t
current_state_mask(const XmWlServer *srv)
{
    return (uint16_t)(srv->mod_mask | srv->button_mask);
}

static void
emit_crossing_event(XmWlServer *srv, XmWlWindow *win, bool enter, uint8_t mode, uint32_t time_ms)
{
    if (!srv || !win) return;
    int wx = 0, wy = 0;
    root_to_window_coords(srv, win, srv->ptr_root_x, srv->ptr_root_y, &wx, &wy);

    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = enter ? 7 : 8; /* EnterNotify / LeaveNotify */
    ev[1] = 0; /* NotifyAncestor */
    wr_u32(ev + 4, time_ms);
    wr_u32(ev + 8, XM_WL_ROOT_XID);
    wr_u32(ev + 12, win->id);
    wr_u32(ev + 16, 0);
    wr_i16(ev + 20, srv->ptr_root_x);
    wr_i16(ev + 22, srv->ptr_root_y);
    wr_i16(ev + 24, (int16_t)wx);
    wr_i16(ev + 26, (int16_t)wy);
    wr_u16(ev + 28, current_state_mask(srv));
    ev[30] = mode; /* 0=NotifyNormal, 1=NotifyGrab, 2=NotifyUngrab */
    ev[31] = 0x03; /* same-screen | focus */
    _XmWlSendEventToWindow(srv, win, enter ? X11_ENTER_WINDOW_MASK : X11_LEAVE_WINDOW_MASK, ev);
}

static XmWlWindow *
resolve_popup_or_top_at_root(XmWlServer *srv, XmWlWindow *fallback_top,
                             int16_t *out_sx, int16_t *out_sy)
{
    if (!srv)
        return fallback_top;
    int rx = (int)srv->ptr_root_x;
    int ry = (int)srv->ptr_root_y;
    for (int d = srv->popup_depth - 1; d >= 0; --d) {
        XmWlWindow *pop = srv->popup_stack[d];
        if (!pop || !pop->mapped || pop->is_drag_icon)
            continue;
        int px = (int)pop->x;
        int py = (int)pop->y;
        int pw = (int)pop->width + 2 * (int)pop->border_width;
        int ph = (int)pop->height + 2 * (int)pop->border_width;
        if (rx >= px && ry >= py && rx < px + pw && ry < py + ph) {
            if (out_sx)
                *out_sx = (int16_t)(rx - px - (int)pop->border_width);
            if (out_sy)
                *out_sy = (int16_t)(ry - py - (int)pop->border_width);
            return pop;
        }
    }
    if (fallback_top) {
        if (out_sx)
            *out_sx = (int16_t)(rx - (int)fallback_top->x - (int)fallback_top->border_width);
        if (out_sy)
            *out_sy = (int16_t)(ry - (int)fallback_top->y - (int)fallback_top->border_width);
    }
    return fallback_top;
}

static void
deliver_propagated_pointer_event(XmWlServer *srv, XmWlWindow *leaf, uint32_t req_mask,
                                 uint8_t ev_code, uint8_t detail, uint32_t time_ms)
{
    if (!srv || !leaf) return;

    /* Active grab routing */
    if (srv->ptr_grab_active) {
        XmWlWindow *gwin = _XmWlFindWindow(srv, srv->ptr_grab_win);
        XmWlClient *gcli = (srv->ptr_grab_client >= 0 && srv->ptr_grab_client < XM_WL_MAX_CLIENTS)
            ? &srv->clients[srv->ptr_grab_client] : NULL;
        if (gwin && gcli && gcli->active) {
            XmWlWindow *dest = NULL;
            if (srv->ptr_grab_owner_events) {
                for (XmWlWindow *cur = leaf; cur && cur != srv->root; cur = cur->parent) {
                    if (cur->event_masks[srv->ptr_grab_client] & req_mask) {
                        dest = cur;
                        break;
                    }
                    if (cur->do_not_propagate & req_mask)
                        break;
                }
            }
            if (!dest && (srv->ptr_grab_mask & req_mask)) {
                dest = gwin;
            }
            if (dest) {
                int wx = 0, wy = 0;
                root_to_window_coords(srv, dest, srv->ptr_root_x, srv->ptr_root_y, &wx, &wy);
                uint32_t child_id = 0;
                XmWlWindow *root_top = resolve_popup_or_top_at_root(srv, srv->pointer_top, NULL, NULL);
                if (dest == srv->root && root_top &&
                    root_top->parent == srv->root &&
                    root_top->mapped && !root_top->is_drag_icon) {
                    child_id = root_top->id;
                } else {
                    for (XmWlWindow *c = dest->last_child; c; c = c->prev_sibling) {
                        if (!c->mapped || c->is_drag_icon) continue;
                        if (wx >= c->x && wy >= c->y &&
                            wx < c->x + (int)c->width + 2 * (int)c->border_width &&
                            wy < c->y + (int)c->height + 2 * (int)c->border_width) {
                            child_id = c->id;
                            break;
                        }
                    }
                }
                uint8_t ev[32];
                memset(ev, 0, sizeof(ev));
                ev[0] = ev_code;
                ev[1] = detail;
                wr_u32(ev + 4, time_ms);
                wr_u32(ev + 8, XM_WL_ROOT_XID);
                wr_u32(ev + 12, dest->id);
                wr_u32(ev + 16, child_id);
                wr_i16(ev + 20, srv->ptr_root_x);
                wr_i16(ev + 22, srv->ptr_root_y);
                wr_i16(ev + 24, (int16_t)wx);
                wr_i16(ev + 26, (int16_t)wy);
                wr_u16(ev + 28, current_state_mask(srv));
                ev[30] = 1; /* same_screen */
                _XmWlSendEventToClient(gcli, ev);
            }
            return;
        }
    }

    /* Normal ancestor propagation */
    uint32_t child_xid = 0;
    for (XmWlWindow *cur = leaf; cur; cur = cur->parent) {
        bool any_selected = false;
        for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
            if (srv->clients[i].active && (cur->event_masks[i] & req_mask)) {
                any_selected = true;
                break;
            }
        }
        if (any_selected) {
            int wx = 0, wy = 0;
            root_to_window_coords(srv, cur, srv->ptr_root_x, srv->ptr_root_y, &wx, &wy);
            uint8_t ev[32];
            memset(ev, 0, sizeof(ev));
            ev[0] = ev_code;
            ev[1] = detail;
            wr_u32(ev + 4, time_ms);
            wr_u32(ev + 8, XM_WL_ROOT_XID);
            wr_u32(ev + 12, cur->id);
            wr_u32(ev + 16, child_xid);
            wr_i16(ev + 20, srv->ptr_root_x);
            wr_i16(ev + 22, srv->ptr_root_y);
            wr_i16(ev + 24, (int16_t)wx);
            wr_i16(ev + 26, (int16_t)wy);
            wr_u16(ev + 28, current_state_mask(srv));
            ev[30] = 1;
            _XmWlSendEventToWindow(srv, cur, req_mask, ev);

            /* Implicit button grab on ButtonPress */
            if (ev_code == 4 && detail >= 1 && detail <= 3 && !srv->ptr_grab_active) {
                for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
                    if (srv->clients[i].active && (cur->event_masks[i] & X11_BUTTON_PRESS_MASK)) {
                        srv->ptr_grab_active = true;
                        srv->ptr_grab_implicit = true;
                        srv->ptr_grab_win = cur->id;
                        srv->ptr_grab_mask = cur->event_masks[i];
                        srv->ptr_grab_owner_events = true;
                        srv->ptr_grab_client = i;
                        break;
                    }
                }
            }
            return;
        }
        if (cur->do_not_propagate & req_mask)
            break;
        child_xid = cur->id;
    }
}

void
_XmWlDispatchPointerMotion(XmWlServer *srv, XmWlWindow *top, int16_t sx, int16_t sy, uint32_t time_ms)
{
    if (!srv || !top) return;
    srv->server_time_ms = time_ms;
    srv->ptr_surf_x = sx;
    srv->ptr_surf_y = sy;
    srv->ptr_root_x = (int16_t)((int)top->x + (int)sx);
    srv->ptr_root_y = (int16_t)((int)top->y + (int)sy);

    int16_t hit_sx = sx, hit_sy = sy;
    XmWlWindow *hit_top = resolve_popup_or_top_at_root(srv, top, &hit_sx, &hit_sy);
    int wx = 0, wy = 0;
    XmWlWindow *leaf = hit_test_child_at(srv, hit_top, hit_sx, hit_sy, &wx, &wy);
    if (!leaf) leaf = hit_top;

    if (leaf != srv->hovered_win) {
        if (srv->hovered_win)
            emit_crossing_event(srv, srv->hovered_win, false, 0, time_ms);
        srv->hovered_win = leaf;
        emit_crossing_event(srv, leaf, true, 0, time_ms);
    }

    uint32_t mmask = X11_POINTER_MOTION_MASK | X11_POINTER_MOTION_HINT_MASK;
    if (srv->button_mask != 0) {
        mmask |= X11_BUTTON_MOTION_MASK;
        if (srv->button_mask & (1u << 8))  mmask |= X11_BUTTON1_MOTION_MASK;
        if (srv->button_mask & (1u << 9))  mmask |= X11_BUTTON2_MOTION_MASK;
        if (srv->button_mask & (1u << 10)) mmask |= X11_BUTTON3_MOTION_MASK;
    }
    deliver_propagated_pointer_event(srv, leaf, mmask, 6 /* MotionNotify */, 0, time_ms);
}

XM_WL_INTERNAL size_t
_XmWlHitTestWindowChain(XmWlServer *srv, uint32_t out_xids[8])
{
    for (size_t i = 0; i < 8; ++i)
        out_xids[i] = 0;
    if (!srv)
        return 0;
    XmWlWindow *top = srv->pointer_top;
    if (!top && srv->popup_depth > 0)
        top = srv->popup_stack[srv->popup_depth - 1];
    if (!top)
        top = srv->wl_focus_top;
    if (!top)
        return 0;

    int16_t hit_sx = srv->ptr_surf_x, hit_sy = srv->ptr_surf_y;
    XmWlWindow *hit_top = resolve_popup_or_top_at_root(srv, top, &hit_sx, &hit_sy);
    int wx = 0, wy = 0;
    XmWlWindow *leaf = hit_test_child_at(srv, hit_top, hit_sx, hit_sy, &wx, &wy);
    if (!leaf)
        leaf = hit_top;

    size_t count = 0;
    for (XmWlWindow *cur = leaf; cur && cur != srv->root && count < 8; cur = cur->parent) {
        out_xids[count++] = cur->id;
    }
    return count;
}

void
_XmWlDispatchPointerButton(XmWlServer *srv, uint8_t button, bool pressed, uint32_t time_ms)
{
    if (!srv || button < 1) return;
    srv->server_time_ms = time_ms;
    bool is_wheel = (button >= 4 && button <= 7);

    XmWlWindow *top = srv->pointer_top;
    if (!top && srv->popup_depth > 0)
        top = srv->popup_stack[srv->popup_depth - 1];
    if (!top)
        top = srv->wl_focus_top;
    if (!top) return;

    int16_t hit_sx = srv->ptr_surf_x, hit_sy = srv->ptr_surf_y;
    XmWlWindow *hit_top = resolve_popup_or_top_at_root(srv, top, &hit_sx, &hit_sy);
    int wx = 0, wy = 0;
    XmWlWindow *leaf = hit_test_child_at(srv, hit_top, hit_sx, hit_sy, &wx, &wy);
    if (!leaf) leaf = hit_top;

    if (pressed) {
        /* Check passive button grabs from root down to leaf.
         * Never match AnyButton (button == 0) passive grabs on scroll-wheel
         * buttons 4..7, as press-only AnyButton grabs swallow <Btn4Up>/<Btn5Up>. */
        for (size_t g = 0; g < srv->passive_grab_count && !srv->ptr_grab_active; ++g) {
            const XmWlPassiveButtonGrab *pg = &srv->passive_grabs[g];
            if (is_wheel ? (pg->button != button) : (pg->button != 0 && pg->button != button))
                continue;
            if (pg->modifiers != 0x8000u && pg->modifiers != srv->mod_mask)
                continue;
            for (XmWlWindow *anc = leaf; anc; anc = anc->parent) {
                if (anc->id == pg->window) {
                    srv->ptr_grab_active = true;
                    srv->ptr_grab_implicit = true;
                    srv->ptr_grab_win = pg->window;
                    srv->ptr_grab_mask = pg->event_mask;
                    srv->ptr_grab_owner_events = pg->owner_events;
                    srv->ptr_grab_client = pg->client_idx;
                    break;
                }
            }
        }
        deliver_propagated_pointer_event(srv, leaf, X11_BUTTON_PRESS_MASK, 4 /* ButtonPress */, button, time_ms);
        if (button >= 1 && button <= 5)
            srv->button_mask |= (uint16_t)(1u << (7 + button));
    } else {
        deliver_propagated_pointer_event(srv, leaf, X11_BUTTON_RELEASE_MASK, 5 /* ButtonRelease */, button, time_ms);
        if (button >= 1 && button <= 5)
            srv->button_mask &= (uint16_t)~(1u << (7 + button));
        if (srv->button_mask == 0 && srv->ptr_grab_active && srv->ptr_grab_implicit) {
            srv->ptr_grab_active = false;
            srv->ptr_grab_implicit = false;
            srv->ptr_grab_win = 0;
        }
    }
}

void
_XmWlCancelActiveDragGrab(XmWlServer *srv, uint32_t time_ms)
{
    if (!srv || srv->button_mask == 0) return;
    srv->server_time_ms = time_ms;
    srv->pointer_top = NULL;
    srv->ptr_surf_x = -500;
    srv->ptr_surf_y = -500;
    srv->ptr_root_x = -500;
    srv->ptr_root_y = -500;
    if (srv->ptr_grab_active && srv->ptr_grab_client >= 0 &&
        srv->ptr_grab_client < XM_WL_MAX_CLIENTS &&
        srv->clients[srv->ptr_grab_client].active) {
        XmWlClient *gcli = &srv->clients[srv->ptr_grab_client];
        uint32_t dest_id = srv->ptr_grab_win ? srv->ptr_grab_win : XM_WL_ROOT_XID;
        uint8_t ev[32];
        memset(ev, 0, sizeof(ev));
        ev[0] = 2; /* KeyPress (Escape -> CancelDrag) */
        ev[1] = 9; /* KEY_ESC + 8 */
        wr_u32(ev + 4, time_ms);
        wr_u32(ev + 8, XM_WL_ROOT_XID);
        wr_u32(ev + 12, dest_id);
        wr_u32(ev + 16, 0); /* subwindow = None */
        wr_i16(ev + 20, -500);
        wr_i16(ev + 22, -500);
        wr_i16(ev + 24, -500);
        wr_i16(ev + 26, -500);
        wr_u16(ev + 28, current_state_mask(srv));
        ev[30] = 1;
        _XmWlSendEventToClient(gcli, ev);

        ev[0] = 3; /* KeyRelease (Escape) */
        _XmWlSendEventToClient(gcli, ev);

        ev[0] = 5; /* ButtonRelease */
        ev[1] = 1; /* Button1 */
        _XmWlSendEventToClient(gcli, ev);
    }
    srv->button_mask = 0;
    if (srv->ptr_grab_active && srv->ptr_grab_implicit) {
        srv->ptr_grab_active = false;
        srv->ptr_grab_implicit = false;
        srv->ptr_grab_win = 0;
    }
}

void
_XmWlDispatchPointerLeaveTop(XmWlServer *srv, XmWlWindow *top, uint32_t time_ms)
{
    (void)top;
    if (!srv) return;
    if (srv->hovered_win) {
        emit_crossing_event(srv, srv->hovered_win, false, 0, time_ms);
        srv->hovered_win = NULL;
    }
}

void
_XmWlDispatchKey(XmWlServer *srv, uint8_t x11_keycode, bool pressed, uint32_t time_ms)
{
    if (!srv) return;
    srv->server_time_ms = time_ms;

    XmWlWindow *target = NULL;
    if (srv->x11_focus_win > 1u)
        target = _XmWlFindWindow(srv, srv->x11_focus_win);
    if (!target || !is_window_viewable(srv, target))
        target = srv->wl_focus_top;
    if (!target && srv->hovered_win)
        target = srv->hovered_win;
    if (!target) return;

    uint32_t mask = pressed ? X11_KEY_PRESS_MASK : X11_KEY_RELEASE_MASK;
    uint32_t child_id = 0;
    for (XmWlWindow *cur = target; cur; cur = cur->parent) {
        bool any = false;
        for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
            if (srv->clients[i].active && (cur->event_masks[i] & mask)) {
                any = true;
                break;
            }
        }
        if (any) {
            int wx = 0, wy = 0;
            root_to_window_coords(srv, cur, srv->ptr_root_x, srv->ptr_root_y, &wx, &wy);
            uint8_t ev[32];
            memset(ev, 0, sizeof(ev));
            ev[0] = pressed ? 2 : 3; /* KeyPress / KeyRelease */
            ev[1] = x11_keycode;
            wr_u32(ev + 4, time_ms);
            wr_u32(ev + 8, XM_WL_ROOT_XID);
            wr_u32(ev + 12, cur->id);
            wr_u32(ev + 16, child_id);
            wr_i16(ev + 20, srv->ptr_root_x);
            wr_i16(ev + 22, srv->ptr_root_y);
            wr_i16(ev + 24, (int16_t)wx);
            wr_i16(ev + 26, (int16_t)wy);
            wr_u16(ev + 28, current_state_mask(srv));
            ev[30] = 1;
            _XmWlSendEventToWindow(srv, cur, mask, ev);
            return;
        }
        if (cur->do_not_propagate & mask)
            break;
        child_id = cur->id;
    }
}

void
_XmWlDispatchFocusTop(XmWlServer *srv, XmWlWindow *top, bool focused)
{
    if (!srv || !top) return;
    if (focused && (srv->x11_focus_win <= 1u || !_XmWlFindWindow(srv, srv->x11_focus_win)))
        srv->x11_focus_win = top->id;

    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = focused ? 9 : 10; /* FocusIn / FocusOut */
    ev[1] = 0; /* NotifyAncestor */
    wr_u32(ev + 4, top->id);
    ev[8] = 0; /* NotifyNormal */
    _XmWlSendEventToWindow(srv, top, X11_FOCUS_CHANGE_MASK, ev);
    _XmWlImeSyncFocus(srv);
}

void
_XmWlDeliverConfigureAndExpose(XmWlServer *srv, XmWlWindow *top, int width, int height, int old_w, int old_h)
{
    if (!srv || !top) return;
    uint8_t cfg[32];
    memset(cfg, 0, sizeof(cfg));
    cfg[0] = 22; /* ConfigureNotify */
    wr_u32(cfg + 4, top->id);
    wr_u32(cfg + 8, top->id);
    wr_u32(cfg + 12, top->prev_sibling ? top->prev_sibling->id : 0);
    wr_i16(cfg + 16, top->x);
    wr_i16(cfg + 18, top->y);
    wr_u16(cfg + 20, (uint16_t)width);
    wr_u16(cfg + 22, (uint16_t)height);
    wr_u16(cfg + 24, top->border_width);
    cfg[26] = top->override_redirect ? 1 : 0;
    _XmWlSendEventToWindow(srv, top, X11_STRUCTURE_NOTIFY_MASK, cfg);
    emit_expose_single(srv, top);
    if (width > old_w || height > old_h)
        emit_expose_uncovered_descendants(srv, top, 0, 0, old_w, old_h);
}

void
_XmWlDeliverCloseRequest(XmWlServer *srv, XmWlWindow *top)
{
    if (!srv || !top) return;
    uint32_t a_protos = _XmWlInternAtom(srv, "WM_PROTOCOLS", 12, false);
    uint32_t a_del = _XmWlInternAtom(srv, "WM_DELETE_WINDOW", 16, false);
    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = 33 | 0x80; /* ClientMessage */
    ev[1] = 32;
    wr_u32(ev + 4, top->id);
    wr_u32(ev + 8, a_protos);
    wr_u32(ev + 12, a_del);
    wr_u32(ev + 16, _XmWlNowMs());
    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        if (srv->clients[i].active && srv->clients[i].handshake_done)
            _XmWlSendEventToClient(&srv->clients[i], ev);
    }
}

void
_XmWlBroadcastRandRChange(XmWlServer *srv)
{
    if (!srv) return;
    uint32_t now_ms = _XmWlNowMs();
    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        XmWlClient *cli = &srv->clients[i];
        if (!cli->active || !cli->handshake_done || (cli->randr_mask & 0x0fu) == 0)
            continue;
        uint8_t ev[32];
        memset(ev, 0, sizeof(ev));
        ev[0] = XM_WL_RANDR_EVENT_BASE; /* RRScreenChangeNotify */
        ev[1] = 1; /* RR_Rotate_0 */
        wr_u32(ev + 4, now_ms);
        wr_u32(ev + 8, now_ms);
        wr_u32(ev + 12, XM_WL_ROOT_XID);
        wr_u32(ev + 16, XM_WL_ROOT_XID);
        wr_u16(ev + 20, 0); /* sizeID */
        wr_u16(ev + 22, 0); /* subpixel */
        wr_u16(ev + 24, (uint16_t)srv->screen_w);
        wr_u16(ev + 26, (uint16_t)srv->screen_h);
        wr_u16(ev + 28, (uint16_t)srv->screen_mm_w);
        wr_u16(ev + 30, (uint16_t)srv->screen_mm_h);
        _XmWlSendEventToClient(cli, ev);
    }
}

/* ---------- GC Value Mask Application ---------- */

static void
apply_gc_values(XmWlGC *gc, uint32_t vmask, const uint8_t *vals, size_t max_bytes)
{
    size_t off = 0;
    for (int bit = 0; bit <= 22; ++bit) {
        if (!(vmask & (1u << bit)))
            continue;
        if (off + 4 > max_bytes)
            break;
        uint32_t v = rd_u32(vals + off);
        off += 4;
        switch (bit) {
        case 0:  gc->function = (uint8_t)v; break;
        case 1:  gc->plane_mask = v; break;
        case 2:  gc->fg = v; break;
        case 3:  gc->bg = v; break;
        case 4:  gc->line_width = (uint16_t)v; break;
        case 5:  gc->line_style = (uint8_t)v; break;
        case 6:  gc->cap_style = (uint8_t)v; break;
        case 7:  gc->join_style = (uint8_t)v; break;
        case 8:  gc->fill_style = (uint8_t)v; break;
        case 9:  gc->fill_rule = (uint8_t)v; break;
        case 10: gc->tile_pixmap = v; break;
        case 11: gc->stipple_pixmap = v; break;
        case 12: gc->ts_x_origin = (int16_t)v; break;
        case 13: gc->ts_y_origin = (int16_t)v; break;
        case 14: gc->font_id = v; break;
        case 15: gc->subwindow_mode = (uint8_t)v; break;
        case 16: gc->graphics_exposures = (v != 0); break;
        case 17: gc->clip_x_origin = (int16_t)v; break;
        case 18: gc->clip_y_origin = (int16_t)v; break;
        case 19:
            gc->clip_mask_pixmap = v;
            if (v == 0) {
                free(gc->clip_rects);
                gc->clip_rects = NULL;
                gc->clip_rect_count = 0;
                gc->has_clip_rects = false;
            }
            break;
        case 20: gc->dash_offset = (uint8_t)v; break;
        case 21: gc->dashes[0] = (uint8_t)v; gc->dash_count = 1; break;
        case 22: gc->arc_mode = (uint8_t)v; break;
        default: break;
        }
    }
}

static void
apply_window_attributes(XmWlWindow *win, int client_idx, uint32_t vmask,
                        const uint8_t *vals, size_t max_bytes)
{
    size_t off = 0;
    for (int bit = 0; bit <= 14; ++bit) {
        if (!(vmask & (1u << bit)))
            continue;
        if (off + 4 > max_bytes)
            break;
        uint32_t v = rd_u32(vals + off);
        off += 4;
        switch (bit) {
        case 0: /* CWBackPixmap */
            win->bg_pixmap = v;
            if (v != 0) win->has_bg_pixel = false;
            break;
        case 1: /* CWBackPixel */
            win->bg_pixel = v;
            win->has_bg_pixel = true;
            win->bg_pixmap = 0;
            break;
        case 3: /* CWBorderPixel */
            win->border_pixel = v;
            break;
        case 4:  win->bit_gravity = (uint8_t)v; break;
        case 5:  win->win_gravity = (uint8_t)v; break;
        case 6:  win->backing_store = (uint8_t)v; break;
        case 9:  win->override_redirect = (v != 0); break;
        case 11: win->event_masks[client_idx] = v; break;
        case 12: win->do_not_propagate = v; break;
        case 13: win->colormap_id = v; break;
        case 14: win->cursor_id = v; break;
        default: break;
        }
    }
}

/* ---------- Extension Request Handlers ---------- */

static void
handle_shape_request(XmWlServer *srv, XmWlClient *cli, uint8_t minor_op, const uint8_t *req, size_t req_len)
{
    if (minor_op == 0) { /* QueryVersion */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, 1);
        wr_u16(hdr + 10, 1);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 1 && req_len >= 16) { /* Rectangles */
        uint8_t op = req[4];
        uint32_t dest = rd_u32(req + 8);
        int16_t xoff = rd_i16(req + 12);
        int16_t yoff = rd_i16(req + 14);
        XmWlWindow *w = _XmWlFindWindow(srv, dest);
        if (!w) return;
        size_t n = (req_len - 16) / 8u;
        if (op == 0 /* ShapeSet */) {
            free(w->shape_rects);
            w->shape_rects = n > 0 ? (XmWlRect *)malloc(n * sizeof(XmWlRect)) : NULL;
            w->shape_rect_count = 0;
            for (size_t i = 0; i < n && w->shape_rects; ++i) {
                w->shape_rects[i].x = (int16_t)(rd_i16(req + 16 + i * 8) + xoff);
                w->shape_rects[i].y = (int16_t)(rd_i16(req + 18 + i * 8) + yoff);
                w->shape_rects[i].width = rd_u16(req + 20 + i * 8);
                w->shape_rects[i].height = rd_u16(req + 22 + i * 8);
                w->shape_rect_count++;
            }
            w->has_shape = true;
        }
    } else if (minor_op == 2 && req_len >= 20) { /* Mask */
        uint32_t dest = rd_u32(req + 8);
        uint32_t src_pix = rd_u32(req + 16);
        XmWlWindow *w = _XmWlFindWindow(srv, dest);
        if (w && src_pix == 0) {
            free(w->shape_rects);
            w->shape_rects = NULL;
            w->shape_rect_count = 0;
            w->has_shape = false;
        }
    } else if (minor_op == 5 && req_len >= 8) { /* QueryExtents */
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[8] = (w && w->has_shape) ? 1 : 0;
        wr_u16(hdr + 16, w ? w->width : 1);
        wr_u16(hdr + 18, w ? w->height : 1);
        wr_u16(hdr + 24, w ? w->width : 1);
        wr_u16(hdr + 26, w ? w->height : 1);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 6 && req_len >= 12) { /* SelectInput */
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (w) w->shape_select_mask = req[8];
    } else if (minor_op == 7 && req_len >= 8) { /* InputSelected */
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = w ? (uint8_t)w->shape_select_mask : 0;
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 8 && req_len >= 12) { /* GetRectangles */
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        if (w && w->has_shape && w->shape_rect_count > 0) {
            wr_u32(hdr + 8, (uint32_t)w->shape_rect_count);
            size_t elen = w->shape_rect_count * 8u;
            uint8_t *eb = (uint8_t *)malloc(elen);
            for (size_t i = 0; i < w->shape_rect_count; ++i) {
                wr_i16(eb + i * 8 + 0, w->shape_rects[i].x);
                wr_i16(eb + i * 8 + 2, w->shape_rects[i].y);
                wr_u16(eb + i * 8 + 4, w->shape_rects[i].width);
                wr_u16(eb + i * 8 + 6, w->shape_rects[i].height);
            }
            _XmWlSendReply(cli, hdr, eb, elen);
            free(eb);
        } else {
            wr_u32(hdr + 8, 1);
            uint8_t r8[8] = {0};
            wr_u16(r8 + 4, w ? w->width : 1);
            wr_u16(r8 + 6, w ? w->height : 1);
            _XmWlSendReply(cli, hdr, r8, 8);
        }
    }
}

static XmWlRegion *
ensure_region(XmWlServer *srv, uint32_t id)
{
    XmWlRegion *r = _XmWlFindRegion(srv, id);
    if (r) {
        pixman_region32_fini(&r->reg);
        pixman_region32_init(&r->reg);
        return r;
    }
    r = (XmWlRegion *)calloc(1, sizeof(*r));
    if (!r) return NULL;
    r->id = id;
    pixman_region32_init(&r->reg);
    uint32_t b = id & 0x3fu;
    r->hash_next = srv->reg_buckets[b];
    srv->reg_buckets[b] = r;
    return r;
}

static void
handle_xfixes_request(XmWlServer *srv, XmWlClient *cli, uint8_t minor_op, const uint8_t *req, size_t req_len)
{
    if (minor_op == 0) { /* QueryVersion */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, 5);
        wr_u32(hdr + 12, 0);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 2 && req_len >= 16) { /* SelectSelectionInput */
        uint32_t sel = rd_u32(req + 8);
        uint32_t mask = rd_u32(req + 12);
        for (int i = 0; i < 8; ++i) {
            if (cli->xfixes_sel_atoms[i] == sel || cli->xfixes_sel_atoms[i] == 0) {
                cli->xfixes_sel_atoms[i] = sel;
                cli->xfixes_sel_masks[i] = mask;
                break;
            }
        }
    } else if (minor_op == 5 && req_len >= 8) { /* CreateRegion */
        XmWlRegion *r = ensure_region(srv, rd_u32(req + 4));
        if (!r) return;
        size_t n = (req_len - 8) / 8u;
        for (size_t i = 0; i < n; ++i) {
            int16_t rx = rd_i16(req + 8 + i * 8);
            int16_t ry = rd_i16(req + 10 + i * 8);
            uint16_t rw = rd_u16(req + 12 + i * 8);
            uint16_t rh = rd_u16(req + 14 + i * 8);
            pixman_region32_union_rect(&r->reg, &r->reg, rx, ry, rw, rh);
        }
    } else if ((minor_op >= 6 && minor_op <= 9) && req_len >= 12) { /* CreateRegionFrom* */
        (void)ensure_region(srv, rd_u32(req + 4));
    } else if (minor_op == 10 && req_len >= 8) { /* DestroyRegion */
        uint32_t id = rd_u32(req + 4);
        XmWlRegion **pp = &srv->reg_buckets[id & 0x3fu];
        while (*pp) {
            if ((*pp)->id == id) {
                XmWlRegion *v = *pp;
                *pp = v->hash_next;
                pixman_region32_fini(&v->reg);
                free(v);
                break;
            }
            pp = &(*pp)->hash_next;
        }
    } else if (minor_op == 11 && req_len >= 8) { /* SetRegion */
        XmWlRegion *r = ensure_region(srv, rd_u32(req + 4));
        if (!r) return;
        size_t n = (req_len - 8) / 8u;
        for (size_t i = 0; i < n; ++i) {
            pixman_region32_union_rect(
                &r->reg, &r->reg,
                rd_i16(req + 8 + i * 8), rd_i16(req + 10 + i * 8),
                rd_u16(req + 12 + i * 8), rd_u16(req + 14 + i * 8));
        }
    } else if (minor_op == 12 && req_len >= 12) { /* CopyRegion */
        XmWlRegion *src = _XmWlFindRegion(srv, rd_u32(req + 4));
        XmWlRegion *dst = ensure_region(srv, rd_u32(req + 8));
        if (src && dst) pixman_region32_copy(&dst->reg, &src->reg);
    } else if ((minor_op >= 13 && minor_op <= 15) && req_len >= 16) { /* Union / Intersect / Subtract */
        XmWlRegion *s1 = _XmWlFindRegion(srv, rd_u32(req + 4));
        XmWlRegion *s2 = _XmWlFindRegion(srv, rd_u32(req + 8));
        XmWlRegion *dst = ensure_region(srv, rd_u32(req + 12));
        if (s1 && s2 && dst) {
            if (minor_op == 13) pixman_region32_union(&dst->reg, &s1->reg, &s2->reg);
            else if (minor_op == 14) pixman_region32_intersect(&dst->reg, &s1->reg, &s2->reg);
            else pixman_region32_subtract(&dst->reg, &s1->reg, &s2->reg);
        }
    } else if (minor_op == 17 && req_len >= 12) { /* TranslateRegion */
        XmWlRegion *r = _XmWlFindRegion(srv, rd_u32(req + 4));
        if (r) pixman_region32_translate(&r->reg, rd_i16(req + 8), rd_i16(req + 10));
    } else if (minor_op == 19 && req_len >= 8) { /* FetchRegion */
        XmWlRegion *r = _XmWlFindRegion(srv, rd_u32(req + 4));
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        int nboxes = 0;
        pixman_box32_t *boxes = r ? pixman_region32_rectangles(&r->reg, &nboxes) : NULL;
        if (r) {
            pixman_box32_t *ext = pixman_region32_extents(&r->reg);
            wr_i16(hdr + 8, (int16_t)ext->x1);
            wr_i16(hdr + 10, (int16_t)ext->y1);
            wr_u16(hdr + 12, (uint16_t)(ext->x2 - ext->x1));
            wr_u16(hdr + 14, (uint16_t)(ext->y2 - ext->y1));
        }
        uint8_t *eb = nboxes > 0 ? (uint8_t *)malloc((size_t)nboxes * 8u) : NULL;
        for (int i = 0; i < nboxes && eb; ++i) {
            wr_i16(eb + i * 8 + 0, (int16_t)boxes[i].x1);
            wr_i16(eb + i * 8 + 2, (int16_t)boxes[i].y1);
            wr_u16(eb + i * 8 + 4, (uint16_t)(boxes[i].x2 - boxes[i].x1));
            wr_u16(eb + i * 8 + 6, (uint16_t)(boxes[i].y2 - boxes[i].y1));
        }
        _XmWlSendReply(cli, hdr, eb, (size_t)nboxes * 8u);
        free(eb);
    } else if (minor_op == 20 && req_len >= 16) { /* SetGCClipRegion */
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 4));
        uint32_t reg_id = rd_u32(req + 8);
        int16_t xo = rd_i16(req + 12);
        int16_t yo = rd_i16(req + 14);
        if (gc) {
            gc->clip_x_origin = xo;
            gc->clip_y_origin = yo;
            free(gc->clip_rects);
            gc->clip_rects = NULL;
            gc->clip_rect_count = 0;
            XmWlRegion *r = _XmWlFindRegion(srv, reg_id);
            if (!r) {
                gc->has_clip_rects = false;
            } else {
                int nb = 0;
                pixman_box32_t *bx = pixman_region32_rectangles(&r->reg, &nb);
                gc->clip_rects = nb > 0 ? (XmWlRect *)malloc((size_t)nb * sizeof(XmWlRect)) : NULL;
                for (int i = 0; i < nb && gc->clip_rects; ++i) {
                    gc->clip_rects[i].x = (int16_t)bx[i].x1;
                    gc->clip_rects[i].y = (int16_t)bx[i].y1;
                    gc->clip_rects[i].width = (uint16_t)(bx[i].x2 - bx[i].x1);
                    gc->clip_rects[i].height = (uint16_t)(bx[i].y2 - bx[i].y1);
                    gc->clip_rect_count++;
                }
                gc->has_clip_rects = true;
            }
        }
    } else if (minor_op == 22 && req_len >= 16) { /* SetPictureClipRegion */
        XmWlPicture *pic = _XmWlFindPicture(srv, rd_u32(req + 4));
        uint32_t reg_id = rd_u32(req + 8);
        int16_t xo = rd_i16(req + 12);
        int16_t yo = rd_i16(req + 14);
        if (pic) {
            pic->clip_x_origin = xo;
            pic->clip_y_origin = yo;
            free(pic->clip_rects);
            pic->clip_rects = NULL;
            pic->clip_rect_count = 0;
            XmWlRegion *r = _XmWlFindRegion(srv, reg_id);
            if (!r) {
                pic->has_clip_rects = false;
            } else {
                int nb = 0;
                pixman_box32_t *bx = pixman_region32_rectangles(&r->reg, &nb);
                pic->clip_rects = nb > 0 ? (XmWlRect *)malloc((size_t)nb * sizeof(XmWlRect)) : NULL;
                for (int i = 0; i < nb && pic->clip_rects; ++i) {
                    pic->clip_rects[i].x = (int16_t)bx[i].x1;
                    pic->clip_rects[i].y = (int16_t)bx[i].y1;
                    pic->clip_rects[i].width = (uint16_t)(bx[i].x2 - bx[i].x1);
                    pic->clip_rects[i].height = (uint16_t)(bx[i].y2 - bx[i].y1);
                    pic->clip_rect_count++;
                }
                pic->has_clip_rects = true;
            }
        }
    }
}

static void
handle_randr_request(XmWlServer *srv, XmWlClient *cli, uint8_t minor_op, const uint8_t *req, size_t req_len)
{
    uint32_t now_ms = _XmWlNowMs();
    if (minor_op == 0) { /* QueryVersion */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, 1);
        wr_u32(hdr + 12, 5);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 2) { /* SetScreenConfig */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 0; /* RRSetConfigSuccess */
        wr_u32(hdr + 8, now_ms);
        wr_u32(hdr + 12, now_ms);
        wr_u32(hdr + 16, XM_WL_ROOT_XID);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 4 && req_len >= 12) { /* SelectInput */
        cli->randr_mask = rd_u16(req + 8);
    } else if (minor_op == 7 || minor_op == 10 || minor_op == 11 ||
               minor_op == 14 || minor_op == 24 || minor_op == 30) {
        /* Void RANDR requests (SetScreenSize, ConfigureOutputProperty,
         * ChangeOutputProperty, DeleteOutputProperty, SetCrtcGamma, SetOutputPrimary) */
    } else if (minor_op == 5) { /* GetScreenInfo */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 1; /* rotations: Rotate_0 */
        wr_u32(hdr + 8, XM_WL_ROOT_XID);
        wr_u32(hdr + 12, now_ms);
        wr_u32(hdr + 16, now_ms);
        wr_u16(hdr + 20, 1); /* nSizes */
        wr_u16(hdr + 22, 0); /* sizeID */
        wr_u16(hdr + 24, 1); /* rotation */
        wr_u16(hdr + 26, 60); /* rate */
        wr_u16(hdr + 28, 1); /* nInfo */
        uint8_t extra[12];
        memset(extra, 0, sizeof(extra));
        wr_u16(extra + 0, (uint16_t)srv->screen_w);
        wr_u16(extra + 2, (uint16_t)srv->screen_h);
        wr_u16(extra + 4, (uint16_t)srv->screen_mm_w);
        wr_u16(extra + 6, (uint16_t)srv->screen_mm_h);
        wr_u16(extra + 8, 1);
        wr_u16(extra + 10, 60);
        _XmWlSendReply(cli, hdr, extra, sizeof(extra));
    } else if (minor_op == 6) { /* GetScreenSizeRange */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, 320);
        wr_u16(hdr + 10, 200);
        wr_u16(hdr + 12, 8192);
        wr_u16(hdr + 14, 8192);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 8 || minor_op == 25) { /* GetScreenResources / Current */
        int nout = (srv->compositor_native_dpi && srv->ipc_output_count > 0)
                       ? srv->ipc_output_count
                       : 1;
        const char *mname = "wayland";
        size_t mlen = strlen(mname);
        size_t names_bytes = (size_t)nout * mlen;
        size_t extra_len = (size_t)nout * 4u + (size_t)nout * 4u + (size_t)nout * 32u + names_bytes;
        uint8_t *extra = (uint8_t *)calloc(1, extra_len);
        if (!extra) return;
        for (int i = 0; i < nout; ++i) {
            wr_u32(extra + (size_t)i * 4u, XM_WL_RANDR_CRTC_XID + (uint32_t)i * 4u);
            wr_u32(extra + (size_t)(nout + i) * 4u, XM_WL_RANDR_OUTPUT_XID + (uint32_t)i * 4u);
            int mw = (srv->compositor_native_dpi && i < srv->ipc_output_count && srv->ipc_outputs[i].width > 0)
                         ? srv->ipc_outputs[i].width
                         : srv->screen_w;
            int mh = (srv->compositor_native_dpi && i < srv->ipc_output_count && srv->ipc_outputs[i].height > 0)
                         ? srv->ipc_outputs[i].height
                         : srv->screen_h;
            size_t moff = (size_t)nout * 8u + (size_t)i * 32u;
            wr_u32(extra + moff + 0, XM_WL_RANDR_MODE_XID + (uint32_t)i * 4u);
            wr_u16(extra + moff + 4, (uint16_t)mw);
            wr_u16(extra + moff + 6, (uint16_t)mh);
            wr_u32(extra + moff + 8, 60000000u);
            wr_u16(extra + moff + 28, (uint16_t)mlen);
            memcpy(extra + (size_t)nout * 40u + (size_t)i * mlen, mname, mlen);
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, now_ms);
        wr_u32(hdr + 12, now_ms);
        wr_u16(hdr + 16, (uint16_t)nout); /* ncrtcs */
        wr_u16(hdr + 18, (uint16_t)nout); /* noutputs */
        wr_u16(hdr + 20, (uint16_t)nout); /* nmodes */
        wr_u16(hdr + 22, (uint16_t)names_bytes);
        _XmWlSendReply(cli, hdr, extra, extra_len);
        free(extra);
    } else if (minor_op == 9) { /* GetOutputInfo */
        uint32_t req_out = (req_len >= 8) ? rd_u32(req + 4) : XM_WL_RANDR_OUTPUT_XID;
        int idx = (req_out >= XM_WL_RANDR_OUTPUT_XID)
                      ? (int)((req_out - XM_WL_RANDR_OUTPUT_XID) / 4u)
                      : 0;
        bool use_ipc = (srv->compositor_native_dpi && idx >= 0 && idx < srv->ipc_output_count);
        const char *oname = (use_ipc && srv->ipc_outputs[idx].name[0])
                                ? srv->ipc_outputs[idx].name
                                : "WL-1";
        bool connected = use_ipc ? srv->ipc_outputs[idx].enabled : true;
        int ow = use_ipc ? srv->ipc_outputs[idx].width : srv->screen_w;
        int oh = use_ipc ? srv->ipc_outputs[idx].height : srv->screen_h;
        int odpi = (use_ipc && srv->ipc_outputs[idx].dpi > 0) ? srv->ipc_outputs[idx].dpi : 96;
        uint32_t mm_w = use_ipc ? (uint32_t)lround((double)ow * 25.4 / (double)odpi) : (uint32_t)srv->screen_mm_w;
        uint32_t mm_h = use_ipc ? (uint32_t)lround((double)oh * 25.4 / (double)odpi) : (uint32_t)srv->screen_mm_h;
        if (mm_w < 1) mm_w = 1;
        if (mm_h < 1) mm_h = 1;
        uint32_t crtc_xid = XM_WL_RANDR_CRTC_XID + (uint32_t)(use_ipc ? idx : 0) * 4u;
        uint32_t mode_xid = XM_WL_RANDR_MODE_XID + (uint32_t)(use_ipc ? idx : 0) * 4u;
        size_t nlen = strlen(oname);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 0; /* RR_StatusSuccess */
        wr_u32(hdr + 8, now_ms);
        wr_u32(hdr + 12, connected ? crtc_xid : 0u);
        wr_u32(hdr + 16, mm_w);
        wr_u32(hdr + 20, mm_h);
        hdr[24] = connected ? 0 : 1; /* RR_Connected / RR_Disconnected */
        hdr[25] = 0; /* SubPixelUnknown */
        wr_u16(hdr + 26, 1); /* nCrtcs */
        wr_u16(hdr + 28, 1); /* nModes */
        wr_u16(hdr + 30, 1); /* nPreferred */
        uint8_t extra[12 + 64];
        memset(extra, 0, sizeof(extra));
        if (nlen > 63) nlen = 63;
        wr_u16(extra + 0, 0); /* nClones */
        wr_u16(extra + 2, (uint16_t)nlen);
        wr_u32(extra + 4, crtc_xid);
        wr_u32(extra + 8, mode_xid);
        memcpy(extra + 12, oname, nlen);
        _XmWlSendReply(cli, hdr, extra, 12 + nlen);
    } else if (minor_op == 12) { /* ListOutputProperties */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        if (srv->compositor_native_dpi) {
            uint32_t a_dpi = _XmWlInternAtom(srv, "DPI", 3, false);
            wr_u16(hdr + 8, 1);
            uint8_t extra[4];
            wr_u32(extra, a_dpi);
            _XmWlSendReply(cli, hdr, extra, 4);
        } else {
            _XmWlSendReply(cli, hdr, NULL, 0);
        }
    } else if (minor_op == 13) { /* QueryOutputProperty */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 15) { /* GetOutputProperty */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        if (srv->compositor_native_dpi && req_len >= 12) {
            uint32_t req_out = rd_u32(req + 4);
            uint32_t prop_atom = rd_u32(req + 8);
            uint32_t a_dpi = _XmWlInternAtom(srv, "DPI", 3, true);
            int idx = (req_out >= XM_WL_RANDR_OUTPUT_XID)
                          ? (int)((req_out - XM_WL_RANDR_OUTPUT_XID) / 4u)
                          : 0;
            if (a_dpi != 0 && prop_atom == a_dpi) {
                int32_t dpi_val = (idx >= 0 && idx < srv->ipc_output_count && srv->ipc_outputs[idx].dpi > 0)
                                      ? srv->ipc_outputs[idx].dpi
                                      : (srv->primary_dpi > 0 ? srv->primary_dpi : 96);
                hdr[1] = 32; /* format */
                wr_u32(hdr + 8, 19u); /* XA_INTEGER */
                wr_u32(hdr + 12, 0u); /* bytesAfter */
                wr_u32(hdr + 16, 1u); /* numItems */
                uint8_t extra[4];
                wr_u32(extra, (uint32_t)dpi_val);
                _XmWlSendReply(cli, hdr, extra, 4);
                return;
            }
        }
        /* format = 0, type = None (0), bytesAfter = 0, numItems = 0 */
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 20) { /* GetCrtcInfo */
        uint32_t req_crtc = (req_len >= 8) ? rd_u32(req + 4) : XM_WL_RANDR_CRTC_XID;
        int idx = (req_crtc >= XM_WL_RANDR_CRTC_XID)
                      ? (int)((req_crtc - XM_WL_RANDR_CRTC_XID) / 4u)
                      : 0;
        bool use_ipc = (srv->compositor_native_dpi && idx >= 0 && idx < srv->ipc_output_count);
        bool enabled = use_ipc ? srv->ipc_outputs[idx].enabled : true;
        int cx = (use_ipc && enabled) ? srv->ipc_outputs[idx].x : 0;
        int cy = (use_ipc && enabled) ? srv->ipc_outputs[idx].y : 0;
        int cw = (use_ipc && enabled) ? srv->ipc_outputs[idx].width : (use_ipc ? 0 : srv->screen_w);
        int ch = (use_ipc && enabled) ? srv->ipc_outputs[idx].height : (use_ipc ? 0 : srv->screen_h);
        uint32_t out_xid = XM_WL_RANDR_OUTPUT_XID + (uint32_t)(use_ipc ? idx : 0) * 4u;
        uint32_t mode_xid = enabled ? (XM_WL_RANDR_MODE_XID + (uint32_t)(use_ipc ? idx : 0) * 4u) : 0u;
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 0; /* RR_StatusSuccess */
        wr_u32(hdr + 8, now_ms);
        wr_i16(hdr + 12, (int16_t)cx);
        wr_i16(hdr + 14, (int16_t)cy);
        wr_u16(hdr + 16, (uint16_t)cw);
        wr_u16(hdr + 18, (uint16_t)ch);
        wr_u32(hdr + 20, mode_xid);
        wr_u16(hdr + 24, 1); /* Rotate_0 */
        wr_u16(hdr + 26, 1); /* rotations */
        wr_u16(hdr + 28, enabled ? 1 : 0); /* nOutput */
        wr_u16(hdr + 30, 1); /* nPossibleOutput */
        uint8_t extra[8];
        if (enabled) {
            wr_u32(extra + 0, out_xid);
            wr_u32(extra + 4, out_xid);
            _XmWlSendReply(cli, hdr, extra, 8);
        } else {
            wr_u32(extra + 0, out_xid);
            _XmWlSendReply(cli, hdr, extra, 4);
        }
    } else if (minor_op == 21) { /* SetCrtcConfig */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 0; /* RRSetConfigSuccess */
        wr_u32(hdr + 8, now_ms);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 22) { /* GetCrtcGammaSize */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, 0);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 23) { /* GetCrtcGamma */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, 0);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 31) { /* GetOutputPrimary */
        uint32_t prim_xid = XM_WL_RANDR_OUTPUT_XID;
        if (srv->compositor_native_dpi) {
            for (int i = 0; i < srv->ipc_output_count; ++i) {
                if (srv->ipc_outputs[i].enabled) {
                    prim_xid = XM_WL_RANDR_OUTPUT_XID + (uint32_t)i * 4u;
                    break;
                }
            }
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, prim_xid);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else {
        /* Never leave _XReply blocked on unsupported RANDR minor opcodes */
        _XmWlSendError(cli, 1 /* BadRequest */, 0, minor_op, XM_WL_EXT_RANDR_OP);
    }
}

static void
handle_xinerama_request(XmWlServer *srv, XmWlClient *cli, uint8_t minor_op)
{
    uint8_t hdr[32];
    memset(hdr, 0, sizeof(hdr));
    if (minor_op == 0) { /* QueryVersion */
        wr_u16(hdr + 8, 1);
        wr_u16(hdr + 10, 1);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 1) { /* GetState */
        hdr[1] = 1;
        wr_u32(hdr + 8, XM_WL_ROOT_XID);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 2) { /* GetScreenCount */
        hdr[1] = 1;
        wr_u32(hdr + 8, XM_WL_ROOT_XID);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 3) { /* GetScreenSize */
        wr_u32(hdr + 8, (uint32_t)srv->screen_w);
        wr_u32(hdr + 12, (uint32_t)srv->screen_h);
        wr_u32(hdr + 16, XM_WL_ROOT_XID);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 4) { /* IsActive */
        wr_u32(hdr + 8, 1);
        _XmWlSendReply(cli, hdr, NULL, 0);
    } else if (minor_op == 5) { /* QueryScreens */
        uint8_t scr[XM_WL_MAX_OUTPUTS * 8];
        int nscr = 0;
        if (srv->compositor_native_dpi) {
            for (int i = 0; i < srv->ipc_output_count && nscr < XM_WL_MAX_OUTPUTS; ++i) {
                if (!srv->ipc_outputs[i].enabled) continue;
                wr_i16(scr + nscr * 8 + 0, (int16_t)srv->ipc_outputs[i].x);
                wr_i16(scr + nscr * 8 + 2, (int16_t)srv->ipc_outputs[i].y);
                wr_u16(scr + nscr * 8 + 4, (uint16_t)srv->ipc_outputs[i].width);
                wr_u16(scr + nscr * 8 + 6, (uint16_t)srv->ipc_outputs[i].height);
                nscr++;
            }
        }
        if (nscr == 0) {
            wr_i16(scr + 0, 0);
            wr_i16(scr + 2, 0);
            wr_u16(scr + 4, (uint16_t)srv->screen_w);
            wr_u16(scr + 6, (uint16_t)srv->screen_h);
            nscr = 1;
        }
        wr_u32(hdr + 8, (uint32_t)nscr);
        _XmWlSendReply(cli, hdr, scr, (size_t)nscr * 8u);
    }
}

/* ---------- CoreFont Reply Serialization ---------- */

static void
pack_charinfo(uint8_t *dst, const XmWlCharInfo *ci)
{
    wr_i16(dst + 0, ci->lbearing);
    wr_i16(dst + 2, ci->rbearing);
    wr_i16(dst + 4, ci->width);
    wr_i16(dst + 6, ci->ascent);
    wr_i16(dst + 8, ci->descent);
    wr_u16(dst + 10, ci->attributes);
}

static const char *const g_core_xlfd_catalog[] = {
    /* CDE interface system (proportional sans) */
    "-dt-interface system-medium-r-normal-xs-10-100-96-96-p-60-iso8859-1",
    "-dt-interface system-medium-r-normal-xs-10-100-96-96-p-60-iso10646-1",
    "-dt-interface system-bold-r-normal-xs-10-100-96-96-p-64-iso8859-1",
    "-dt-interface system-bold-r-normal-xs-10-100-96-96-p-64-iso10646-1",
    "-dt-interface system-medium-r-normal-s-12-120-96-96-p-70-iso8859-1",
    "-dt-interface system-medium-r-normal-s-12-120-96-96-p-70-iso10646-1",
    "-dt-interface system-bold-r-normal-s-12-120-96-96-p-75-iso8859-1",
    "-dt-interface system-bold-r-normal-s-12-120-96-96-p-75-iso10646-1",
    "-dt-interface system-medium-r-normal-m-14-140-96-96-p-80-iso8859-1",
    "-dt-interface system-medium-r-normal-m-14-140-96-96-p-80-iso10646-1",
    "-dt-interface system-bold-r-normal-m-14-140-96-96-p-86-iso8859-1",
    "-dt-interface system-bold-r-normal-m-14-140-96-96-p-86-iso10646-1",
    "-dt-interface system-medium-r-normal-l-16-160-96-96-p-90-iso8859-1",
    "-dt-interface system-medium-r-normal-l-16-160-96-96-p-90-iso10646-1",
    "-dt-interface system-bold-r-normal-l-16-160-96-96-p-96-iso8859-1",
    "-dt-interface system-bold-r-normal-l-16-160-96-96-p-96-iso10646-1",
    "-dt-interface system-medium-r-normal-xl-18-180-96-96-p-100-iso8859-1",
    "-dt-interface system-medium-r-normal-xl-18-180-96-96-p-100-iso10646-1",
    "-dt-interface system-bold-r-normal-xl-18-180-96-96-p-108-iso8859-1",
    "-dt-interface system-bold-r-normal-xl-18-180-96-96-p-108-iso10646-1",
    "-dt-interface system-medium-r-normal-xxl-22-220-96-96-p-120-iso8859-1",
    "-dt-interface system-medium-r-normal-xxl-22-220-96-96-p-120-iso10646-1",
    /* CDE interface user (monospace terminal/text) */
    "-dt-interface user-medium-r-normal-xs-10-100-96-96-m-60-iso8859-1",
    "-dt-interface user-medium-r-normal-xs-10-100-96-96-m-60-iso10646-1",
    "-dt-interface user-bold-r-normal-xs-10-100-96-96-m-60-iso8859-1",
    "-dt-interface user-bold-r-normal-xs-10-100-96-96-m-60-iso10646-1",
    "-dt-interface user-medium-r-normal-s-12-120-96-96-m-70-iso8859-1",
    "-dt-interface user-medium-r-normal-s-12-120-96-96-m-70-iso10646-1",
    "-dt-interface user-bold-r-normal-s-12-120-96-96-m-70-iso8859-1",
    "-dt-interface user-bold-r-normal-s-12-120-96-96-m-70-iso10646-1",
    "-dt-interface user-medium-r-normal-m-14-140-96-96-m-80-iso8859-1",
    "-dt-interface user-medium-r-normal-m-14-140-96-96-m-80-iso10646-1",
    "-dt-interface user-bold-r-normal-m-14-140-96-96-m-80-iso8859-1",
    "-dt-interface user-bold-r-normal-m-14-140-96-96-m-80-iso10646-1",
    "-dt-interface user-medium-r-normal-l-16-160-96-96-m-90-iso8859-1",
    "-dt-interface user-medium-r-normal-l-16-160-96-96-m-90-iso10646-1",
    "-dt-interface user-bold-r-normal-l-16-160-96-96-m-90-iso8859-1",
    "-dt-interface user-bold-r-normal-l-16-160-96-96-m-90-iso10646-1",
    "-dt-interface user-medium-r-normal-xl-18-180-96-96-m-100-iso8859-1",
    "-dt-interface user-medium-r-normal-xl-18-180-96-96-m-100-iso10646-1",
    "-dt-interface user-medium-r-normal-xxl-22-220-96-96-m-120-iso8859-1",
    "-dt-interface user-medium-r-normal-xxl-22-220-96-96-m-120-iso10646-1",
    /* CDE application & standard X11 core families (bitmap sizes + scalable entries) */
    "-dt-application-medium-r-normal-sans-0-0-0-0-p-0-iso8859-1",
    "-dt-application-medium-r-normal-sans-0-0-0-0-p-0-iso10646-1",
    "-dt-application-bold-r-normal-sans-0-0-0-0-p-0-iso8859-1",
    "-dt-application-bold-r-normal-sans-0-0-0-0-p-0-iso10646-1",
    "-dt-application-medium-r-normal-sans-10-100-96-96-p-60-iso8859-1",
    "-dt-application-medium-r-normal-sans-10-100-96-96-p-60-iso10646-1",
    "-dt-application-medium-r-normal-sans-12-120-96-96-p-70-iso8859-1",
    "-dt-application-medium-r-normal-sans-12-120-96-96-p-70-iso10646-1",
    "-dt-application-bold-r-normal-sans-12-120-96-96-p-75-iso8859-1",
    "-dt-application-bold-r-normal-sans-12-120-96-96-p-75-iso10646-1",
    "-dt-application-medium-r-normal-sans-14-140-96-96-p-80-iso8859-1",
    "-dt-application-medium-r-normal-sans-14-140-96-96-p-80-iso10646-1",
    "-dt-application-bold-r-normal-sans-14-140-96-96-p-85-iso8859-1",
    "-dt-application-bold-r-normal-sans-14-140-96-96-p-85-iso10646-1",
    "-dt-application-medium-r-normal-sans-16-160-96-96-p-90-iso8859-1",
    "-dt-application-medium-r-normal-sans-16-160-96-96-p-90-iso10646-1",
    "-dt-application-medium-r-normal-sans-18-180-96-96-p-100-iso8859-1",
    "-dt-application-medium-r-normal-sans-18-180-96-96-p-100-iso10646-1",
    "-dt-application-medium-r-normal-sans-20-200-96-96-p-110-iso8859-1",
    "-dt-application-medium-r-normal-sans-20-200-96-96-p-110-iso10646-1",
    "-b&h-lucida-medium-r-normal-sans-0-0-0-0-p-0-iso8859-1",
    "-b&h-lucida-medium-r-normal-sans-0-0-0-0-p-0-iso10646-1",
    "-b&h-lucida-bold-r-normal-sans-0-0-0-0-p-0-iso8859-1",
    "-b&h-lucida-bold-r-normal-sans-0-0-0-0-p-0-iso10646-1",
    "-b&h-lucida-medium-r-normal-sans-10-100-75-75-p-60-iso8859-1",
    "-b&h-lucida-medium-r-normal-sans-10-100-75-75-p-60-iso10646-1",
    "-b&h-lucida-medium-r-normal-sans-12-120-75-75-p-71-iso8859-1",
    "-b&h-lucida-medium-r-normal-sans-12-120-75-75-p-71-iso10646-1",
    "-b&h-lucida-bold-r-normal-sans-12-120-75-75-p-79-iso8859-1",
    "-b&h-lucida-bold-r-normal-sans-12-120-75-75-p-79-iso10646-1",
    "-b&h-lucida-medium-r-normal-sans-14-140-75-75-p-82-iso8859-1",
    "-b&h-lucida-medium-r-normal-sans-14-140-75-75-p-82-iso10646-1",
    "-b&h-lucida-medium-r-normal-sans-18-180-75-75-p-106-iso8859-1",
    "-b&h-lucida-medium-r-normal-sans-18-180-75-75-p-106-iso10646-1",
    "-b&h-lucidatypewriter-medium-r-normal-sans-0-0-0-0-m-0-iso8859-1",
    "-b&h-lucidatypewriter-medium-r-normal-sans-0-0-0-0-m-0-iso10646-1",
    "-b&h-lucidatypewriter-medium-r-normal-sans-10-100-75-75-m-60-iso8859-1",
    "-b&h-lucidatypewriter-medium-r-normal-sans-10-100-75-75-m-60-iso10646-1",
    "-b&h-lucidatypewriter-medium-r-normal-sans-12-120-75-75-m-70-iso8859-1",
    "-b&h-lucidatypewriter-medium-r-normal-sans-12-120-75-75-m-70-iso10646-1",
    "-b&h-lucidatypewriter-bold-r-normal-sans-12-120-75-75-m-70-iso8859-1",
    "-b&h-lucidatypewriter-bold-r-normal-sans-12-120-75-75-m-70-iso10646-1",
    "-b&h-lucidatypewriter-medium-r-normal-sans-14-140-75-75-m-80-iso8859-1",
    "-b&h-lucidatypewriter-medium-r-normal-sans-14-140-75-75-m-80-iso10646-1",
    "-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1",
    "-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso10646-1",
    "-misc-fixed-medium-r-normal--12-120-75-75-c-60-iso8859-1",
    "-misc-fixed-medium-r-normal--12-120-75-75-c-60-iso10646-1",
    "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1",
    "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso10646-1",
    "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-15",
    "-misc-fixed-bold-r-normal--13-120-75-75-c-70-iso8859-1",
    "-misc-fixed-bold-r-normal--13-120-75-75-c-70-iso10646-1",
    "-misc-fixed-medium-r-normal--14-130-75-75-c-70-iso8859-1",
    "-misc-fixed-medium-r-normal--14-130-75-75-c-70-iso10646-1",
    "-misc-fixed-medium-r-normal--15-140-75-75-c-90-iso8859-1",
    "-misc-fixed-medium-r-normal--15-140-75-75-c-90-iso10646-1",
    "-misc-fixed-medium-r-normal--18-170-75-75-c-90-iso8859-1",
    "-misc-fixed-medium-r-normal--18-170-75-75-c-90-iso10646-1",
    "-adobe-helvetica-medium-r-normal--0-0-0-0-p-0-iso8859-1",
    "-adobe-helvetica-medium-r-normal--0-0-0-0-p-0-iso10646-1",
    "-adobe-helvetica-bold-r-normal--0-0-0-0-p-0-iso8859-1",
    "-adobe-helvetica-bold-r-normal--0-0-0-0-p-0-iso10646-1",
    "-adobe-helvetica-medium-r-normal--10-100-75-75-p-56-iso8859-1",
    "-adobe-helvetica-medium-r-normal--10-100-75-75-p-56-iso10646-1",
    "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1",
    "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso10646-1",
    "-adobe-helvetica-bold-r-normal--12-120-75-75-p-70-iso8859-1",
    "-adobe-helvetica-bold-r-normal--12-120-75-75-p-70-iso10646-1",
    "-adobe-helvetica-medium-r-normal--14-140-75-75-p-77-iso8859-1",
    "-adobe-helvetica-medium-r-normal--14-140-75-75-p-77-iso10646-1",
    "-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1",
    "-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso10646-1",
    "-adobe-courier-medium-r-normal--0-0-0-0-m-0-iso8859-1",
    "-adobe-courier-medium-r-normal--0-0-0-0-m-0-iso10646-1",
    "-adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1",
    "-adobe-courier-medium-r-normal--10-100-75-75-m-60-iso10646-1",
    "-adobe-courier-medium-r-normal--12-120-75-75-m-70-iso8859-1",
    "-adobe-courier-medium-r-normal--12-120-75-75-m-70-iso10646-1",
    "-adobe-courier-medium-r-normal--14-140-75-75-m-90-iso8859-1",
    "-adobe-courier-medium-r-normal--14-140-75-75-m-90-iso10646-1",
    "fixed",
    "variable",
    "6x13",
    "8x13",
    "9x15",
};

static const char *
resolve_concrete_xlfd_for_font(const XmWlFont *f)
{
    if (f && f->name[0] == '-' && !strchr(f->name, '*') && !strchr(f->name, '?')) {
        return f->name;
    }
    if (f && f->name[0]) {
        for (size_t i = 0; i < sizeof(g_core_xlfd_catalog) / sizeof(g_core_xlfd_catalog[0]); ++i) {
            if (g_core_xlfd_catalog[i][0] == '-' &&
                fnmatch(f->name, g_core_xlfd_catalog[i], FNM_CASEFOLD) == 0) {
                return g_core_xlfd_catalog[i];
            }
        }
    }
    return (f && f->monospace)
               ? "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1"
               : "-b&h-lucida-medium-r-normal-sans-12-120-75-75-p-71-iso8859-1";
}

static size_t
match_font_catalog(const char *pat, uint16_t max_names, const char **out_names, size_t out_cap)
{
    if (max_names == 0 || out_cap == 0) return 0;
    const char *eff_pat = (pat && pat[0]) ? pat : "*";

    /* X11 XCreateFontSet often passes comma-separated or whitespace-trimmed XLFD patterns */
    size_t count = 0;
    size_t lim = max_names < out_cap ? max_names : out_cap;
    for (size_t i = 0; i < sizeof(g_core_xlfd_catalog) / sizeof(g_core_xlfd_catalog[0]) && count < lim; ++i) {
        if (fnmatch(eff_pat, g_core_xlfd_catalog[i], FNM_CASEFOLD) == 0) {
            out_names[count++] = g_core_xlfd_catalog[i];
        }
    }
    if (count == 0 && !strchr(eff_pat, '*') && !strchr(eff_pat, '?') && eff_pat[0] == '-') {
        /* Exact XLFD requested that is not in static table: echo back if 14 dashes */
        int dashes = 0;
        for (const char *p = eff_pat; *p; ++p) if (*p == '-') dashes++;
        if (dashes == 14) {
            out_names[count++] = eff_pat;
        }
    }
    if (count == 0) {
        /* Fallback when caller specifies an uncommon foundry/family wildcard with iso8859-1 or iso10646-1 */
        if (strcasestr(eff_pat, "iso10646-1")) {
            out_names[count++] = strcasestr(eff_pat, "user") || strcasestr(eff_pat, "-m-") || strcasestr(eff_pat, "-c-")
                                     ? "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso10646-1"
                                     : "-dt-interface system-medium-r-normal-m-14-140-96-96-p-80-iso10646-1";
        } else if (strcasestr(eff_pat, "iso8859-1")) {
            out_names[count++] = strcasestr(eff_pat, "user") || strcasestr(eff_pat, "-m-") || strcasestr(eff_pat, "-c-")
                                     ? "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1"
                                     : "-dt-interface system-medium-r-normal-m-14-140-96-96-p-80-iso8859-1";
        } else if (!strcasecmp(eff_pat, "*")) {
            out_names[count++] = "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1";
        }
    }
    return count;
}

static void
send_query_font_reply(XmWlServer *srv, XmWlClient *cli, uint32_t font_or_gc_id)
{
    XmWlFont *f = _XmWlFindFont(srv, font_or_gc_id);
    if (!f) {
        XmWlGC *gc = _XmWlFindGC(srv, font_or_gc_id);
        if (gc) f = _XmWlFindFont(srv, gc->font_id);
    }
    if (!f) f = _XmWlFindFont(srv, 0x00000070u);
    if (!f) f = _XmWlOpenFont(srv, 0x00000070u, "fixed", 5);
    if (!f) {
        _XmWlSendError(cli, 7 /* BadFont */, font_or_gc_id, 0, 47);
        return;
    }

    const char *xlfd = resolve_concrete_xlfd_for_font(f);
    uint32_t xlfd_atom = _XmWlInternAtom(srv, xlfd, strlen(xlfd), false);
    bool is_ucs2 = strcasestr(xlfd, "iso10646-1") != NULL;

    XmWlCharInfo uni_bounds = f->max_bounds;
    if (f->chars[(unsigned char)'n'].width > 0)
        uni_bounds.width = f->chars[(unsigned char)'n'].width;

    uint8_t hdr[32];
    memset(hdr, 0, sizeof(hdr));
    pack_charinfo(hdr + 8, is_ucs2 ? &uni_bounds : &f->min_bounds);
    /* max_bounds spans hdr[24..31] + extra[0..3] */
    uint8_t maxb[12];
    pack_charinfo(maxb, is_ucs2 ? &uni_bounds : &f->max_bounds);
    memcpy(hdr + 24, maxb, 8);

    uint32_t n_chars = is_ucs2 ? 0u : 256u;
    uint16_t n_props = 1u;
    size_t extra_len = 28u + (size_t)n_props * 8u + (size_t)n_chars * 12u;
    uint8_t *extra = (uint8_t *)calloc(1, extra_len);
    if (!extra) return;
    memcpy(extra + 0, maxb + 8, 4);
    /* extra[4..7] unused */
    wr_u16(extra + 8, 0);                        /* min_char_or_byte2 */
    wr_u16(extra + 10, 255);                     /* max_char_or_byte2 */
    wr_u16(extra + 12, 32);                      /* default_char */
    wr_u16(extra + 14, n_props);                 /* nFontProps */
    extra[16] = 0;                               /* LeftToRight */
    extra[17] = 0;                               /* min_byte1 */
    extra[18] = is_ucs2 ? 255u : 0u;             /* max_byte1 */
    extra[19] = 1;                               /* all_chars_exist */
    wr_i16(extra + 20, f->ascent);
    wr_i16(extra + 22, f->descent);
    wr_u32(extra + 24, n_chars);                 /* nCharInfos */

    /* FONTPROP[0]: XA_FONT (18) -> concrete XLFD atom */
    wr_u32(extra + 28, 18u);
    wr_u32(extra + 32, xlfd_atom);

    for (uint32_t c = 0; c < n_chars; ++c)
        pack_charinfo(extra + 36 + (size_t)c * 12u, &f->chars[c]);

    _XmWlSendReply(cli, hdr, extra, extra_len);
    free(extra);
}

/* ---------- Core Request Dispatch ---------- */

static void
dispatch_x11_request(XmWlServer *srv, XmWlClient *cli, const uint8_t *req, size_t req_len)
{
    uint8_t major = req[0];
    uint8_t data1 = req[1];
    cli->seq++;

    switch (major) {
    case 1: { /* CreateWindow */
        if (req_len < 32) break;
        uint8_t depth = data1;
        uint32_t wid = rd_u32(req + 4);
        uint32_t pid = rd_u32(req + 8);
        int16_t x = rd_i16(req + 12);
        int16_t y = rd_i16(req + 14);
        uint16_t w = rd_u16(req + 16);
        uint16_t h = rd_u16(req + 18);
        uint16_t bw = rd_u16(req + 20);
        uint16_t wclass = rd_u16(req + 22);
        uint32_t visual = rd_u32(req + 24);
        uint32_t vmask = rd_u32(req + 28);

        XmWlWindow *parent = _XmWlFindWindow(srv, pid);
        if (!parent) {
            _XmWlSendError(cli, 3 /* BadWindow */, pid, 0, major);
            break;
        }
        XmWlWindow *nw = alloc_window(srv, wid, parent);
        if (!nw) break;
        nw->x = x;
        nw->y = y;
        nw->width = w > 0 ? w : 1;
        nw->height = h > 0 ? h : 1;
        nw->border_width = bw;
        nw->win_class = (wclass == 2) ? 2 : 1;
        nw->depth = (nw->win_class == 2) ? 0 : (depth ? depth : parent->depth);
        nw->visual_id = visual ? visual : ((nw->depth == 32) ? XM_WL_VISUAL_ARGB32_ID : parent->visual_id);
        apply_window_attributes(nw, cli->index, vmask, req + 32, req_len - 32);

        uint8_t ev[32];
        memset(ev, 0, sizeof(ev));
        ev[0] = 16; /* CreateNotify */
        wr_u32(ev + 4, parent->id);
        wr_u32(ev + 8, nw->id);
        wr_i16(ev + 12, nw->x);
        wr_i16(ev + 14, nw->y);
        wr_u16(ev + 16, nw->width);
        wr_u16(ev + 18, nw->height);
        wr_u16(ev + 20, nw->border_width);
        ev[22] = nw->override_redirect ? 1 : 0;
        _XmWlSendEventToWindow(srv, parent, X11_SUBSTRUCTURE_NOTIFY_MASK, ev);
        break;
    }

    case 2: { /* ChangeWindowAttributes */
        if (req_len < 12) break;
        uint32_t wid = rd_u32(req + 4);
        uint32_t vmask = rd_u32(req + 8);
        XmWlWindow *w = _XmWlFindWindow(srv, wid);
        if (!w) { _XmWlSendError(cli, 3, wid, 0, major); break; }
        /* Per X11 protocol spec, changing CWBackPixel/CWBackPixmap updates
         * window attributes only and must not clear existing window pixels
         * (Motif PushB/CascadeB restore background_pixel after painting armed text). */
        apply_window_attributes(w, cli->index, vmask, req + 12, req_len - 12);
        break;
    }

    case 3: { /* GetWindowAttributes (44-byte reply: 32 hdr + 12 extra) */
        if (req_len < 8) break;
        uint32_t wid = rd_u32(req + 4);
        XmWlWindow *w = _XmWlFindWindow(srv, wid);
        if (!w) { _XmWlSendError(cli, 3, wid, 0, major); break; }
        uint8_t hdr[32];
        uint8_t extra[12];
        memset(hdr, 0, sizeof(hdr));
        memset(extra, 0, sizeof(extra));
        hdr[1] = w->backing_store;
        wr_u32(hdr + 8, w->visual_id);
        wr_u16(hdr + 12, w->win_class);
        hdr[14] = w->bit_gravity;
        hdr[15] = w->win_gravity;
        wr_u32(hdr + 16, 0xffffffffu); /* backing_planes */
        wr_u32(hdr + 20, 0);
        hdr[24] = 0; /* save_under */
        hdr[25] = 1; /* map_is_installed */
        hdr[26] = !w->mapped ? 0 : (is_window_viewable(srv, w) ? 2 : 1);
        hdr[27] = w->override_redirect ? 1 : 0;
        wr_u32(hdr + 28, w->colormap_id);

        uint32_t all_masks = 0;
        for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i)
            all_masks |= w->event_masks[i];
        wr_u32(extra + 0, all_masks);
        wr_u32(extra + 4, w->event_masks[cli->index]);
        wr_u16(extra + 8, (uint16_t)w->do_not_propagate);
        _XmWlSendReply(cli, hdr, extra, 12);
        break;
    }

    case 4: { /* DestroyWindow */
        if (req_len < 8) break;
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (w) destroy_window_tree(srv, w, true);
        break;
    }

    case 5: { /* DestroySubwindows */
        if (req_len < 8) break;
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (w && w != srv->root) {
            while (w->first_child)
                destroy_window_tree(srv, w->first_child, true);
        }
        break;
    }

    case 7: { /* ReparentWindow */
        if (req_len < 16) break;
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        XmWlWindow *np = _XmWlFindWindow(srv, rd_u32(req + 8));
        if (!w || !np || w == srv->root) break;
        if (w->parent == srv->root && np != srv->root)
            _XmWlUnmapWindowRole(srv, w);
        if (w->parent) {
            if (w->prev_sibling) w->prev_sibling->next_sibling = w->next_sibling;
            else w->parent->first_child = w->next_sibling;
            if (w->next_sibling) w->next_sibling->prev_sibling = w->prev_sibling;
            else w->parent->last_child = w->prev_sibling;
        }
        w->parent = np;
        w->prev_sibling = np->last_child;
        w->next_sibling = NULL;
        if (np->last_child) np->last_child->next_sibling = w;
        else np->first_child = w;
        np->last_child = w;
        w->x = rd_i16(req + 12);
        w->y = rd_i16(req + 14);

        uint8_t ev[32];
        memset(ev, 0, sizeof(ev));
        ev[0] = 21; /* ReparentNotify */
        wr_u32(ev + 4, w->id);
        wr_u32(ev + 8, w->id);
        wr_u32(ev + 12, np->id);
        wr_i16(ev + 16, w->x);
        wr_i16(ev + 18, w->y);
        ev[20] = w->override_redirect ? 1 : 0;
        _XmWlSendEventToWindow(srv, w, X11_STRUCTURE_NOTIFY_MASK, ev);
        if (np == srv->root && w->mapped)
            _XmWlSyncWindowRole(srv, w);
        break;
    }

    case 8: { /* MapWindow */
        if (req_len < 8) break;
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (!w || w == srv->root || w->mapped) break;
        w->mapped = true;
        if (w->parent == srv->root)
            _XmWlSyncWindowRole(srv, w);

        uint8_t ev[32];
        memset(ev, 0, sizeof(ev));
        ev[0] = 19; /* MapNotify */
        wr_u32(ev + 4, w->id);
        wr_u32(ev + 8, w->id);
        ev[12] = w->override_redirect ? 1 : 0;
        _XmWlSendEventToWindow(srv, w, X11_STRUCTURE_NOTIFY_MASK, ev);
        if (w->parent) {
            wr_u32(ev + 4, w->parent->id);
            _XmWlSendEventToWindow(srv, w->parent, X11_SUBSTRUCTURE_NOTIFY_MASK, ev);
        }

        if (is_window_viewable(srv, w))
            emit_expose_recursive(srv, w);
        break;
    }

    case 9: { /* MapSubwindows */
        if (req_len < 8) break;
        XmWlWindow *p = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (!p) break;
        for (XmWlWindow *c = p->first_child; c; c = c->next_sibling) {
            if (c == srv->wm_helper || c == srv->sm_helper ||
                c == srv->proxy_sel || c->mapped)
                continue;
            c->mapped = true;
            if (p == srv->root)
                _XmWlSyncWindowRole(srv, c);
            uint8_t ev[32];
            memset(ev, 0, sizeof(ev));
            ev[0] = 19;
            wr_u32(ev + 4, c->id);
            wr_u32(ev + 8, c->id);
            ev[12] = c->override_redirect ? 1 : 0;
            _XmWlSendEventToWindow(srv, c, X11_STRUCTURE_NOTIFY_MASK, ev);
            if (is_window_viewable(srv, c))
                emit_expose_recursive(srv, c);
        }
        break;
    }

    case 10: { /* UnmapWindow */
        if (req_len < 8) break;
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (!w || w == srv->root || !w->mapped) break;
        w->mapped = false;
        if (w->parent == srv->root)
            _XmWlUnmapWindowRole(srv, w);
        else if (is_window_viewable(srv, w->parent)) {
            uint16_t uw = (uint16_t)(w->width + 2u * w->border_width);
            uint16_t uh = (uint16_t)(w->height + 2u * w->border_width);
            _XmWlPaintWindowBackground(srv, w->parent, w->x, w->y, uw, uh);
            expose_uncovered_siblings(srv, w, w->x, w->y, uw, uh, false);
            if (uw > 0 && uh > 0) {
                uint8_t exp[32];
                memset(exp, 0, sizeof(exp));
                exp[0] = 12; /* Expose */
                wr_u32(exp + 4, w->parent->id);
                wr_u16(exp + 8, (uint16_t)(w->x > 0 ? w->x : 0));
                wr_u16(exp + 10, (uint16_t)(w->y > 0 ? w->y : 0));
                wr_u16(exp + 12, uw);
                wr_u16(exp + 14, uh);
                _XmWlSendEventToWindow(srv, w->parent, X11_EXPOSURE_MASK, exp);
            }
        }

        uint8_t ev[32];
        memset(ev, 0, sizeof(ev));
        ev[0] = 18; /* UnmapNotify */
        wr_u32(ev + 4, w->id);
        wr_u32(ev + 8, w->id);
        _XmWlSendEventToWindow(srv, w, X11_STRUCTURE_NOTIFY_MASK, ev);
        if (w->parent) {
            wr_u32(ev + 4, w->parent->id);
            _XmWlSendEventToWindow(srv, w->parent, X11_SUBSTRUCTURE_NOTIFY_MASK, ev);
        }
        break;
    }

    case 11: { /* UnmapSubwindows */
        if (req_len < 8) break;
        XmWlWindow *p = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (!p) break;
        for (XmWlWindow *c = p->first_child; c; c = c->next_sibling) {
            if (!c->mapped) continue;
            c->mapped = false;
            if (p == srv->root) _XmWlUnmapWindowRole(srv, c);
        }
        if (p != srv->root && is_window_viewable(srv, p))
            _XmWlPaintWindowBackground(srv, p, 0, 0, p->width, p->height);
        break;
    }

    case 12: { /* ConfigureWindow */
        if (req_len < 12) break;
        uint32_t wid = rd_u32(req + 4);
        uint16_t vmask = rd_u16(req + 8);
        XmWlWindow *w = _XmWlFindWindow(srv, wid);
        if (!w || w == srv->root) break;

        int16_t old_x = w->x, old_y = w->y;
        uint16_t old_w = w->width, old_h = w->height;
        uint32_t sibling_id = 0;
        uint8_t stack_mode = 0;
        bool has_stack = false;

        size_t off = 12;
        bool top_managed = (w->parent == srv->root && w->xdg_top != NULL && !w->override_redirect);
        bool comp_resizing = top_managed && (w->has_queued_wl_cfg || w->resize_awaiting_client);
        for (int bit = 0; bit <= 6; ++bit) {
            if (!(vmask & (1u << bit))) continue;
            if (off + 4 > req_len) break;
            uint32_t v = rd_u32(req + off);
            off += 4;
            switch (bit) {
            case 0: if (!comp_resizing) w->x = (int16_t)v; break;
            case 1: if (!comp_resizing) w->y = (int16_t)v; break;
            case 2: if (!comp_resizing) w->width = (uint16_t)(v > 0 ? v : 1); break;
            case 3: if (!comp_resizing) w->height = (uint16_t)(v > 0 ? v : 1); break;
            case 4: w->border_width = (uint16_t)v; break;
            case 5: sibling_id = v; break;
            case 6: stack_mode = (uint8_t)v; has_stack = true; break;
            default: break;
            }
        }

        if (has_stack && w->parent) {
            XmWlWindow *p = w->parent;
            if (stack_mode == 0 /* Above */ && sibling_id == 0 && p->last_child != w) {
                if (w->prev_sibling) w->prev_sibling->next_sibling = w->next_sibling;
                else p->first_child = w->next_sibling;
                if (w->next_sibling) w->next_sibling->prev_sibling = w->prev_sibling;
                else p->last_child = w->prev_sibling;
                w->prev_sibling = p->last_child;
                w->next_sibling = NULL;
                if (p->last_child) p->last_child->next_sibling = w;
                else p->first_child = w;
                p->last_child = w;
            } else if (stack_mode == 1 /* Below */ && sibling_id == 0 && p->first_child != w) {
                if (w->prev_sibling) w->prev_sibling->next_sibling = w->next_sibling;
                else p->first_child = w->next_sibling;
                if (w->next_sibling) w->next_sibling->prev_sibling = w->prev_sibling;
                else p->last_child = w->prev_sibling;
                w->prev_sibling = NULL;
                w->next_sibling = p->first_child;
                if (p->first_child) p->first_child->prev_sibling = w;
                else p->last_child = w;
                p->first_child = w;
            }
        }

        if (w->parent == srv->root)
            _XmWlEnsureTopCanvas(srv, w);

        uint8_t cfg[32];
        memset(cfg, 0, sizeof(cfg));
        cfg[0] = 22; /* ConfigureNotify */
        wr_u32(cfg + 4, w->id);
        wr_u32(cfg + 8, w->id);
        wr_u32(cfg + 12, w->prev_sibling ? w->prev_sibling->id : 0);
        wr_i16(cfg + 16, w->x);
        wr_i16(cfg + 18, w->y);
        wr_u16(cfg + 20, w->width);
        wr_u16(cfg + 22, w->height);
        wr_u16(cfg + 24, w->border_width);
        cfg[26] = w->override_redirect ? 1 : 0;
        _XmWlSendEventToWindow(srv, w, X11_STRUCTURE_NOTIFY_MASK, cfg);
        if (w->parent) {
            wr_u32(cfg + 4, w->parent->id);
            _XmWlSendEventToWindow(srv, w->parent, X11_SUBSTRUCTURE_NOTIFY_MASK, cfg);
        }

        bool pos_changed = (old_x != w->x || old_y != w->y);
        bool size_changed = (old_w != w->width || old_h != w->height);
        if ((pos_changed || size_changed) && w->parent == srv->root && w->mapped && w->is_popup) {
            _XmWlRepositionPopup(srv, w);
        }
        if ((pos_changed || size_changed || has_stack) && is_window_viewable(srv, w)) {
            if (w->parent && w->parent != srv->root &&
                (pos_changed || has_stack || w->width < old_w || w->height < old_h)) {
                uint16_t uw = (uint16_t)(old_w + 2u * w->border_width);
                uint16_t uh = (uint16_t)(old_h + 2u * w->border_width);
                _XmWlPaintWindowBackground(srv, w->parent, old_x, old_y, uw, uh);
                expose_uncovered_siblings(srv, w, old_x, old_y, uw, uh, has_stack);
                if (uw > 0 && uh > 0) {
                    uint8_t exp[32];
                    memset(exp, 0, sizeof(exp));
                    exp[0] = 12; /* Expose */
                    wr_u32(exp + 4, w->parent->id);
                    wr_u16(exp + 8, (uint16_t)(old_x > 0 ? old_x : 0));
                    wr_u16(exp + 10, (uint16_t)(old_y > 0 ? old_y : 0));
                    wr_u16(exp + 12, uw);
                    wr_u16(exp + 14, uh);
                    _XmWlSendEventToWindow(srv, w->parent, X11_EXPOSURE_MASK, exp);
                }
            }
            if ((pos_changed || (has_stack && stack_mode == 0)) && w->parent != srv->root) {
                emit_expose_recursive(srv, w);
            } else if (size_changed) {
                emit_expose_single(srv, w);
                if ((int)w->width > (int)old_w || (int)w->height > (int)old_h)
                    emit_expose_uncovered_descendants(srv, w, 0, 0, (int)old_w, (int)old_h);
            } else {
                _XmWlDamageTop(w, 0, 0, w->width, w->height);
            }
        }
        break;
    }

    case 14: { /* GetGeometry */
        if (req_len < 8) break;
        uint32_t did = rd_u32(req + 4);
        XmWlWindow *w = _XmWlFindWindow(srv, did);
        XmWlPixmap *pix = w ? NULL : _XmWlFindPixmap(srv, did);
        if (!w && !pix) { _XmWlSendError(cli, 9 /* BadDrawable */, did, 0, major); break; }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = w ? w->depth : pix->depth;
        wr_u32(hdr + 8, XM_WL_ROOT_XID);
        wr_i16(hdr + 12, w ? w->x : 0);
        wr_i16(hdr + 14, w ? w->y : 0);
        wr_u16(hdr + 16, w ? w->width : pix->width);
        wr_u16(hdr + 18, w ? w->height : pix->height);
        wr_u16(hdr + 20, w ? w->border_width : 0);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 15: { /* QueryTree */
        if (req_len < 8) break;
        uint32_t wid = rd_u32(req + 4);
        XmWlWindow *w = _XmWlFindWindow(srv, wid);
        if (!w) { _XmWlSendError(cli, 3, wid, 0, major); break; }
        size_t n = 0;
        for (XmWlWindow *c = w->first_child; c; c = c->next_sibling)
            n++;
        uint32_t *kids = n > 0 ? (uint32_t *)malloc(n * 4u) : NULL;
        size_t k = 0;
        for (XmWlWindow *c = w->first_child; c && kids; c = c->next_sibling)
            kids[k++] = c->id;
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, XM_WL_ROOT_XID);
        wr_u32(hdr + 12, w->parent ? w->parent->id : 0);
        wr_u16(hdr + 16, (uint16_t)k);
        _XmWlSendReply(cli, hdr, kids, k * 4u);
        free(kids);
        break;
    }

    case 16: { /* InternAtom */
        if (req_len < 8) break;
        uint16_t nlen = rd_u16(req + 4);
        if (8u + nlen > req_len) break;
        uint32_t atom = _XmWlInternAtom(srv, (const char *)(req + 8), nlen, data1 != 0);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, atom);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 17: { /* GetAtomName */
        if (req_len < 8) break;
        uint32_t atom = rd_u32(req + 4);
        const char *nm = _XmWlAtomName(srv, atom);
        if (!nm) { _XmWlSendError(cli, 5 /* BadAtom */, atom, 0, major); break; }
        size_t nl = strlen(nm);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, (uint16_t)nl);
        _XmWlSendReply(cli, hdr, nm, nl);
        break;
    }

    case 18: { /* ChangeProperty */
        if (req_len < 24) break;
        uint8_t mode = data1;
        uint32_t wid = rd_u32(req + 4);
        uint32_t prop = rd_u32(req + 8);
        uint32_t type = rd_u32(req + 12);
        uint8_t fmt = req[16];
        uint32_t nitems = rd_u32(req + 20);
        XmWlWindow *w = _XmWlFindWindow(srv, wid);
        if (!w) { _XmWlSendError(cli, 3, wid, 0, major); break; }
        if (fmt != 8 && fmt != 16 && fmt != 32) break;
        size_t blen = (size_t)nitems * (size_t)(fmt / 8u);
        if (24u + blen > req_len) blen = req_len - 24u;
        _XmWlSetProperty(srv, w, prop, type, fmt, mode, req + 24, blen, true);
        break;
    }

    case 19: { /* DeleteProperty */
        if (req_len < 12) break;
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (w) _XmWlDeleteProperty(srv, w, rd_u32(req + 8), true);
        break;
    }

    case 20: { /* GetProperty */
        if (req_len < 24) break;
        bool del = (data1 != 0);
        uint32_t wid = rd_u32(req + 4);
        uint32_t prop = rd_u32(req + 8);
        uint32_t req_type = rd_u32(req + 12);
        uint32_t long_off = rd_u32(req + 16);
        uint32_t long_len = rd_u32(req + 20);
        XmWlWindow *w = _XmWlFindWindow(srv, wid);
        if (!w) { _XmWlSendError(cli, 3, wid, 0, major); break; }

        XmWlProperty *p = _XmWlGetProperty(w, prop);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        if (!p) {
            _XmWlSendReply(cli, hdr, NULL, 0);
            break;
        }
        if (req_type != 0 && req_type != p->type) {
            hdr[1] = p->format;
            wr_u32(hdr + 8, p->type);
            wr_u32(hdr + 12, (uint32_t)p->byte_len);
            _XmWlSendReply(cli, hdr, NULL, 0);
            break;
        }
        size_t off_b = (size_t)long_off * 4u;
        if (off_b > p->byte_len) off_b = p->byte_len;
        size_t rem_b = p->byte_len - off_b;
        size_t max_b = (long_len > 0x0fffffffu) ? rem_b : (size_t)long_len * 4u;
        size_t copy_b = rem_b < max_b ? rem_b : max_b;
        size_t after_b = rem_b - copy_b;

        hdr[1] = p->format;
        wr_u32(hdr + 8, p->type);
        wr_u32(hdr + 12, (uint32_t)after_b);
        wr_u32(hdr + 16, (uint32_t)(copy_b / (p->format / 8u)));
        _XmWlSendReply(cli, hdr, p->data ? (p->data + off_b) : NULL, copy_b);

        if (del && after_b == 0)
            _XmWlDeleteProperty(srv, w, prop, true);
        break;
    }

    case 21: { /* ListProperties */
        if (req_len < 8) break;
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (!w) { _XmWlSendError(cli, 3, rd_u32(req + 4), 0, major); break; }
        size_t n = 0;
        for (XmWlProperty *p = w->props; p; p = p->next) n++;
        uint32_t *atoms = n > 0 ? (uint32_t *)malloc(n * 4u) : NULL;
        size_t k = 0;
        for (XmWlProperty *p = w->props; p && atoms; p = p->next) atoms[k++] = p->atom;
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, (uint16_t)k);
        _XmWlSendReply(cli, hdr, atoms, k * 4u);
        free(atoms);
        break;
    }

    case 22: { /* SetSelectionOwner */
        if (req_len < 16) break;
        uint32_t owner = rd_u32(req + 4);
        uint32_t sel = rd_u32(req + 8);
        uint32_t tm = rd_u32(req + 12);
        _XmWlDataHandleSetSelectionOwner(srv, sel, owner, tm);
        break;
    }

    case 23: { /* GetSelectionOwner */
        if (req_len < 8) break;
        uint32_t sel = rd_u32(req + 4);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, _XmWlDataGetSelectionOwner(srv, sel));
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 24: { /* ConvertSelection */
        if (req_len < 24) break;
        _XmWlDataHandleConvertSelection(
            srv, cli->index,
            rd_u32(req + 4), rd_u32(req + 8),
            rd_u32(req + 12), rd_u32(req + 16), rd_u32(req + 20));
        break;
    }

    case 25: { /* SendEvent */
        if (req_len < 44) break;
        uint32_t dest = rd_u32(req + 4);
        uint32_t emask = rd_u32(req + 8);
        uint8_t ev[32];
        memcpy(ev, req + 12, 32);
        ev[0] |= 0x80u;

        if (_XmWlDataInterceptSendEvent(srv, dest, ev))
            break;

        XmWlWindow *dw = NULL;
        if (dest == 0 /* PointerWindow */) dw = srv->hovered_win ? srv->hovered_win : srv->pointer_top;
        else if (dest == 1 /* InputFocus */) dw = _XmWlFindWindow(srv, srv->x11_focus_win);
        else dw = _XmWlFindWindow(srv, dest);

        if (dw == srv->sm_helper && (ev[0] & 0x7fu) == 33 && ev[1] == 32) {
            uint32_t mtype = rd_u32(ev + 8);
            uint32_t d0 = rd_u32(ev + 12);
            const char *mname = _XmWlAtomName(srv, mtype);
            const char *d0name = _XmWlAtomName(srv, d0);
            if (mname && d0name &&
                !strcmp(mname, "_DT_SM_STM_PROTOCOL") &&
                !strcmp(d0name, "_DT_SM_SAVE_TO_HOME")) {
                _XmWlIpcPublishRootProp(srv, "_DT_SM_SAVE_TO_HOME", ev + 16, 8);
            }
        } else if (dw == srv->root && (ev[0] & 0x7fu) == 33 && ev[1] == 32) {
            uint32_t mtype = rd_u32(ev + 8);
            const char *mname = _XmWlAtomName(srv, mtype);
            if (mname && !strcmp(mname, "_NET_ACTIVE_WINDOW")) {
                uint32_t act_wid = rd_u32(ev + 4);
                XmWlWindow *aw = _XmWlFindWindow(srv, act_wid);
                int dx = 0, dy = 0;
                XmWlWindow *atop = aw ? _XmWlEnclosingTopLevel(srv, aw, &dx, &dy) : NULL;
                if (atop)
                    _XmWlRequestSelfActivate(srv, atop);
            }
        }

        if (dw) {
            if (emask == 0) {
                for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i)
                    if (srv->clients[i].active) _XmWlSendEventToClient(&srv->clients[i], ev);
            } else {
                _XmWlSendEventToWindow(srv, dw, emask, ev);
            }
        }
        break;
    }

    case 26: { /* GrabPointer */
        if (req_len < 24) break;
        uint32_t gwin = rd_u32(req + 4);
        uint16_t emask = rd_u16(req + 8);
        XmWlWindow *w = _XmWlFindWindow(srv, gwin);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        if (!w || !is_window_viewable(srv, w)) {
            hdr[1] = 3; /* GrabNotViewable */
        } else {
            hdr[1] = 0; /* GrabSuccess */
            srv->ptr_grab_active = true;
            srv->ptr_grab_implicit = false;
            srv->ptr_grab_win = gwin;
            srv->ptr_grab_mask = emask;
            srv->ptr_grab_owner_events = (data1 != 0);
            srv->ptr_grab_client = cli->index;
        }
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 27: { /* UngrabPointer */
        srv->ptr_grab_active = false;
        srv->ptr_grab_implicit = false;
        srv->ptr_grab_win = 0;
        break;
    }

    case 28: { /* GrabButton */
        if (req_len < 24) break;
        if (srv->passive_grab_count < 64) {
            XmWlPassiveButtonGrab *pg = &srv->passive_grabs[srv->passive_grab_count++];
            pg->owner_events = (data1 != 0);
            pg->window = rd_u32(req + 4);
            pg->event_mask = rd_u16(req + 8);
            pg->button = req[20];
            pg->modifiers = rd_u16(req + 22);
            pg->client_idx = cli->index;
        }
        break;
    }

    case 29: { /* UngrabButton */
        if (req_len < 12) break;
        uint32_t gwin = rd_u32(req + 4);
        uint8_t btn = data1;
        size_t w_idx = 0;
        for (size_t r_idx = 0; r_idx < srv->passive_grab_count; ++r_idx) {
            const XmWlPassiveButtonGrab *pg = &srv->passive_grabs[r_idx];
            if (pg->window == gwin && (btn == 0 || pg->button == btn))
                continue;
            srv->passive_grabs[w_idx++] = *pg;
        }
        srv->passive_grab_count = w_idx;
        break;
    }

    case 31: { /* GrabKeyboard */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 0; /* GrabSuccess */
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 38: { /* QueryPointer */
        if (req_len < 8) break;
        uint32_t wid = rd_u32(req + 4);
        XmWlWindow *w = _XmWlFindWindow(srv, wid);
        if (!w) { _XmWlSendError(cli, 3, wid, 0, major); break; }
        int wx = 0, wy = 0;
        root_to_window_coords(srv, w, srv->ptr_root_x, srv->ptr_root_y, &wx, &wy);
        uint32_t child_id = 0;
        XmWlWindow *root_top = resolve_popup_or_top_at_root(srv, srv->pointer_top, NULL, NULL);
        if (w == srv->root && root_top &&
            root_top->parent == srv->root &&
            root_top->mapped && !root_top->is_drag_icon) {
            child_id = root_top->id;
        } else {
            for (XmWlWindow *c = w->last_child; c; c = c->prev_sibling) {
                if (!c->mapped || c->is_drag_icon) continue;
                if (wx >= c->x && wy >= c->y &&
                    wx < c->x + (int)c->width + 2 * (int)c->border_width &&
                    wy < c->y + (int)c->height + 2 * (int)c->border_width) {
                    child_id = c->id;
                    break;
                }
            }
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 1; /* same_screen */
        wr_u32(hdr + 8, XM_WL_ROOT_XID);
        wr_u32(hdr + 12, child_id);
        wr_i16(hdr + 16, srv->ptr_root_x);
        wr_i16(hdr + 18, srv->ptr_root_y);
        wr_i16(hdr + 20, (int16_t)wx);
        wr_i16(hdr + 22, (int16_t)wy);
        wr_u16(hdr + 24, current_state_mask(srv));
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 39: { /* GetMotionEvents */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 40: { /* TranslateCoordinates */
        if (req_len < 16) break;
        XmWlWindow *src_w = _XmWlFindWindow(srv, rd_u32(req + 4));
        XmWlWindow *dst_w = _XmWlFindWindow(srv, rd_u32(req + 8));
        int16_t sx = rd_i16(req + 12);
        int16_t sy = rd_i16(req + 14);
        if (!src_w || !dst_w) { _XmWlSendError(cli, 3, rd_u32(req + 4), 0, major); break; }
        int rx = 0, ry = 0, dx = 0, dy = 0;
        window_to_root_coords(srv, src_w, sx, sy, &rx, &ry);
        root_to_window_coords(srv, dst_w, rx, ry, &dx, &dy);
        uint32_t child_id = 0;
        XmWlWindow *root_top = resolve_popup_or_top_at_root(srv, srv->pointer_top, NULL, NULL);
        if (dst_w == srv->root && root_top &&
            root_top->parent == srv->root &&
            root_top->mapped && !root_top->is_drag_icon &&
            dx == srv->ptr_root_x && dy == srv->ptr_root_y) {
            child_id = root_top->id;
        } else {
            for (XmWlWindow *c = dst_w->last_child; c; c = c->prev_sibling) {
                if (!c->mapped || c->is_drag_icon) continue;
                if (dx >= c->x && dy >= c->y &&
                    dx < c->x + (int)c->width + 2 * (int)c->border_width &&
                    dy < c->y + (int)c->height + 2 * (int)c->border_width) {
                    child_id = c->id;
                    break;
                }
            }
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 1; /* same_screen */
        wr_u32(hdr + 8, child_id);
        wr_i16(hdr + 12, (int16_t)dx);
        wr_i16(hdr + 14, (int16_t)dy);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 42: { /* SetInputFocus */
        if (req_len < 12) break;
        srv->x11_focus_revert = data1;
        srv->x11_focus_win = rd_u32(req + 4);
        XmWlWindow *fw = _XmWlFindWindow(srv, srv->x11_focus_win);
        if (fw) {
            uint8_t ev[32];
            memset(ev, 0, sizeof(ev));
            ev[0] = 9; /* FocusIn */
            wr_u32(ev + 4, fw->id);
            _XmWlSendEventToWindow(srv, fw, X11_FOCUS_CHANGE_MASK, ev);
        }
        break;
    }

    case 43: { /* GetInputFocus */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = srv->x11_focus_revert;
        wr_u32(hdr + 8, srv->x11_focus_win);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 44: { /* QueryKeymap (40 bytes: 32 hdr + 8 extra) */
        uint8_t hdr[32];
        uint8_t extra[8];
        memset(hdr, 0, sizeof(hdr));
        memset(extra, 0, sizeof(extra));
        _XmWlSendReply(cli, hdr, extra, 8);
        break;
    }

    case 45: { /* OpenFont */
        if (req_len < 12) break;
        uint32_t fid = rd_u32(req + 4);
        uint16_t nlen = rd_u16(req + 8);
        if (12u + nlen > req_len) break;
        (void)_XmWlOpenFont(srv, fid, (const char *)(req + 12), nlen);
        break;
    }

    case 46: { /* CloseFont */
        if (req_len < 8) break;
        uint32_t fid = rd_u32(req + 4);
        if (fid == 0x00000070u) break;
        XmWlFont **pp = &srv->font_buckets[fid & 0x3fu];
        while (*pp) {
            if ((*pp)->id == fid) {
                XmWlFont *v = *pp;
                *pp = v->hash_next;
                _XmWlFreeFontData(v);
                free(v);
                break;
            }
            pp = &(*pp)->hash_next;
        }
        break;
    }

    case 47: { /* QueryFont */
        if (req_len < 8) break;
        send_query_font_reply(srv, cli, rd_u32(req + 4));
        break;
    }

    case 48: { /* QueryTextExtents */
        if (req_len < 8) break;
        uint32_t fid = rd_u32(req + 4);
        XmWlFont *f = _XmWlFindFont(srv, fid);
        if (!f) {
            XmWlGC *gc = _XmWlFindGC(srv, fid);
            if (gc) f = _XmWlFindFont(srv, gc->font_id);
        }
        if (!f) f = _XmWlFindFont(srv, 0x00000070u);

        size_t slen = req_len - 8;
        if (data1 != 0 && slen >= 2) slen -= 2; /* odd_length in UTF-16 byte pairs */
        int32_t overall_w = 0;
        size_t nchars = slen / 2u;
        for (size_t i = 0; i < nchars; ++i) {
            uint8_t ch = req[8 + i * 2 + 1];
            overall_w += f ? f->chars[ch].width : 7;
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 0; /* LeftToRight */
        wr_i16(hdr + 8, f ? f->ascent : 11);
        wr_i16(hdr + 10, f ? f->descent : 3);
        wr_i16(hdr + 12, f ? f->ascent : 11);
        wr_i16(hdr + 14, f ? f->descent : 3);
        wr_i32(hdr + 16, overall_w);
        wr_i32(hdr + 20, 0);
        wr_i32(hdr + 24, overall_w);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 49: { /* ListFonts */
        if (req_len < 8) break;
        uint16_t max_names = rd_u16(req + 4);
        uint16_t plen = rd_u16(req + 6);
        if (8u + plen > req_len) break;
        char pat[256];
        size_t pn = plen < sizeof(pat) - 1 ? plen : sizeof(pat) - 1;
        memcpy(pat, req + 8, pn);
        pat[pn] = '\0';

        const char *matched[512];
        size_t nmatch = match_font_catalog(pat, max_names, matched, 512);

        size_t buf_cap = nmatch * 128u + 16u;
        uint8_t *buf = (uint8_t *)calloc(1, buf_cap);
        if (!buf) break;
        size_t off = 0;
        uint16_t count = 0;
        for (size_t i = 0; i < nmatch; ++i) {
            size_t nl = strlen(matched[i]);
            if (nl > 255) nl = 255;
            if (off + 1 + nl > buf_cap) break;
            buf[off++] = (uint8_t)nl;
            memcpy(buf + off, matched[i], nl);
            off += nl;
            count++;
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, count);
        _XmWlSendReply(cli, hdr, buf, off);
        free(buf);
        break;
    }

    case 50: { /* ListFontsWithInfo */
        if (req_len < 8) break;
        uint16_t max_names = rd_u16(req + 4);
        uint16_t plen = rd_u16(req + 6);
        if (8u + plen > req_len) break;
        char pat[256];
        size_t pn = plen < sizeof(pat) - 1 ? plen : sizeof(pat) - 1;
        memcpy(pat, req + 8, pn);
        pat[pn] = '\0';

        const char *matched[512];
        size_t nmatch = match_font_catalog(pat, max_names, matched, 512);

        for (size_t i = 0; i < nmatch; ++i) {
            const char *nm = matched[i];
            size_t nl = strlen(nm);
            if (nl == 0 || nl > 255) continue;
            bool is_ucs2 = strcasestr(nm, "iso10646-1") != NULL;
            bool is_mono = strcasestr(nm, "interface user") != NULL ||
                           strcasestr(nm, "fixed") != NULL ||
                           strcasestr(nm, "typewriter") != NULL ||
                           strcasestr(nm, "courier") != NULL ||
                           strcasestr(nm, "-m-") != NULL ||
                           strcasestr(nm, "-c-") != NULL;
            int px = 13;
            bool prescaled = false;
            const char *p = nm;
            if (nm[0] == '-') {
                int dash_cnt = 0;
                while (*p && dash_cnt < 7) {
                    if (*p == '-') dash_cnt++;
                    p++;
                }
                if (dash_cnt == 7 && isdigit((unsigned char)*p)) {
                    int parsed = atoi(p);
                    if (parsed >= 8 && parsed <= 144) px = parsed;
                }
                while (*p && dash_cnt < 10) {
                    if (*p == '-') dash_cnt++;
                    p++;
                }
                if (dash_cnt == 10 && isdigit((unsigned char)*p) && atoi(p) > 105)
                    prescaled = true;
            }
            if (!prescaled && srv->logical_zoom > 1.01)
                px = (int)lround((double)px * srv->logical_zoom);
            int16_t asc = (int16_t)(px - (px / 5));
            int16_t desc = (int16_t)(px - asc);
            if (desc < 2) desc = 2;
            int16_t cw = (int16_t)(is_mono ? ((px * 6 + 5) / 10) : ((px * 7 + 5) / 10));
            if (cw < 5) cw = 5;
            XmWlCharInfo min_ci = { .lbearing = 0, .rbearing = (int16_t)(is_mono ? cw : 2), .width = (int16_t)(is_mono ? cw : 3), .ascent = 0, .descent = 0, .attributes = 0 };
            XmWlCharInfo max_ci = { .lbearing = 0, .rbearing = cw, .width = cw, .ascent = asc, .descent = desc, .attributes = 0 };
            if (is_ucs2) min_ci = max_ci;

            uint32_t xlfd_atom = _XmWlInternAtom(srv, nm, nl, false);

            uint8_t hdr[32];
            memset(hdr, 0, sizeof(hdr));
            hdr[1] = (uint8_t)nl;
            uint8_t maxb[12];
            pack_charinfo(hdr + 8, &min_ci);
            pack_charinfo(maxb, &max_ci);
            memcpy(hdr + 24, maxb, 8);

            size_t extra_len = 28u + 8u + nl;
            uint8_t *extra = (uint8_t *)calloc(1, extra_len);
            if (extra) {
                memcpy(extra + 0, maxb + 8, 4);
                wr_u16(extra + 8, 0);
                wr_u16(extra + 10, 255);
                wr_u16(extra + 12, 32);
                wr_u16(extra + 14, 1); /* nFontProps = 1 */
                extra[16] = 0;
                extra[17] = 0;
                extra[18] = is_ucs2 ? 255u : 0u;
                extra[19] = 1;
                wr_i16(extra + 20, asc);
                wr_i16(extra + 22, desc);
                wr_u32(extra + 24, (uint32_t)(nmatch - 1 - i)); /* replies_hint */
                wr_u32(extra + 28, 18u); /* XA_FONT */
                wr_u32(extra + 32, xlfd_atom);
                memcpy(extra + 36, nm, nl);
                _XmWlSendReply(cli, hdr, extra, extra_len);
                free(extra);
            }
        }

        /* End-of-series terminator reply: name_len=0, length=7 -> 28 extra bytes */
        uint8_t thdr[32];
        uint8_t textra[28];
        memset(thdr, 0, sizeof(thdr));
        memset(textra, 0, sizeof(textra));
        thdr[1] = 0;
        _XmWlSendReply(cli, thdr, textra, 28);
        break;
    }

    case 52: { /* GetFontPath */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 53: { /* CreatePixmap */
        if (req_len < 16) break;
        uint8_t depth = data1 ? data1 : 24;
        uint32_t pid = rd_u32(req + 4);
        uint16_t w = rd_u16(req + 12);
        uint16_t h = rd_u16(req + 14);
        if (w == 0) w = 1;
        if (h == 0) h = 1;
        XmWlPixmap *pix = (XmWlPixmap *)calloc(1, sizeof(*pix));
        if (!pix) break;
        pix->id = pid;
        pix->width = w;
        pix->height = h;
        pix->depth = depth;
        pix->stride = (int)w * 4;
        pix->bits = (uint32_t *)calloc((size_t)w * (size_t)h, sizeof(uint32_t));
        if (pix->bits) {
            pix->img = pixman_image_create_bits(PIXMAN_a8r8g8b8, w, h, pix->bits, pix->stride);
        }
        uint32_t b = pid & 0xffu;
        pix->hash_next = srv->pix_buckets[b];
        srv->pix_buckets[b] = pix;
        break;
    }

    case 54: { /* FreePixmap */
        if (req_len < 8) break;
        uint32_t pid = rd_u32(req + 4);
        XmWlPixmap **pp = &srv->pix_buckets[pid & 0xffu];
        while (*pp) {
            if ((*pp)->id == pid) {
                XmWlPixmap *v = *pp;
                *pp = v->hash_next;
                _XmWlFreePixmapData(v);
                free(v);
                break;
            }
            pp = &(*pp)->hash_next;
        }
        break;
    }

    case 55: { /* CreateGC */
        if (req_len < 16) break;
        uint32_t gid = rd_u32(req + 4);
        uint32_t did = rd_u32(req + 8);
        uint32_t vmask = rd_u32(req + 12);
        XmWlWindow *dw = _XmWlFindWindow(srv, did);
        XmWlPixmap *dp = dw ? NULL : _XmWlFindPixmap(srv, did);
        XmWlGC *gc = (XmWlGC *)calloc(1, sizeof(*gc));
        if (!gc) break;
        gc->id = gid;
        gc->depth = dw ? dw->depth : (dp ? dp->depth : 24);
        gc->function = 3; /* GXcopy */
        gc->plane_mask = 0xffffffffu;
        gc->fg = 0x00000000u;
        gc->bg = 0x00ffffffu;
        gc->font_id = 0x00000070u;
        gc->graphics_exposures = true;
        gc->dashes[0] = 4;
        gc->dash_count = 1;
        apply_gc_values(gc, vmask, req + 16, req_len - 16);
        uint32_t b = gid & 0xffu;
        gc->hash_next = srv->gc_buckets[b];
        srv->gc_buckets[b] = gc;
        break;
    }

    case 56: { /* ChangeGC */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 4));
        if (gc) apply_gc_values(gc, rd_u32(req + 8), req + 12, req_len - 12);
        break;
    }

    case 57: { /* CopyGC */
        if (req_len < 16) break;
        XmWlGC *src = _XmWlFindGC(srv, rd_u32(req + 4));
        XmWlGC *dst = _XmWlFindGC(srv, rd_u32(req + 8));
        uint32_t vmask = rd_u32(req + 12);
        if (!src || !dst) break;
        if (vmask & (1u << 0))  dst->function = src->function;
        if (vmask & (1u << 1))  dst->plane_mask = src->plane_mask;
        if (vmask & (1u << 2))  dst->fg = src->fg;
        if (vmask & (1u << 3))  dst->bg = src->bg;
        if (vmask & (1u << 4))  dst->line_width = src->line_width;
        if (vmask & (1u << 5))  dst->line_style = src->line_style;
        if (vmask & (1u << 8))  dst->fill_style = src->fill_style;
        if (vmask & (1u << 10)) dst->tile_pixmap = src->tile_pixmap;
        if (vmask & (1u << 11)) dst->stipple_pixmap = src->stipple_pixmap;
        if (vmask & (1u << 12)) dst->ts_x_origin = src->ts_x_origin;
        if (vmask & (1u << 13)) dst->ts_y_origin = src->ts_y_origin;
        if (vmask & (1u << 14)) dst->font_id = src->font_id;
        if (vmask & (1u << 15)) dst->subwindow_mode = src->subwindow_mode;
        if (vmask & (1u << 16)) dst->graphics_exposures = src->graphics_exposures;
        if (vmask & (1u << 17)) dst->clip_x_origin = src->clip_x_origin;
        if (vmask & (1u << 18)) dst->clip_y_origin = src->clip_y_origin;
        if (vmask & (1u << 19)) dst->clip_mask_pixmap = src->clip_mask_pixmap;
        break;
    }

    case 58: { /* SetDashes */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 4));
        if (gc) {
            gc->dash_offset = (uint8_t)rd_u16(req + 8);
            uint16_t n = rd_u16(req + 10);
            if (n > 16) n = 16;
            if (12u + n <= req_len && n > 0) {
                memcpy(gc->dashes, req + 12, n);
                gc->dash_count = (uint8_t)n;
            }
        }
        break;
    }

    case 59: { /* SetClipRectangles */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 4));
        if (gc) {
            gc->clip_x_origin = rd_i16(req + 8);
            gc->clip_y_origin = rd_i16(req + 10);
            gc->clip_mask_pixmap = 0;
            free(gc->clip_rects);
            size_t n = (req_len - 12) / 8u;
            gc->clip_rects = n > 0 ? (XmWlRect *)malloc(n * sizeof(XmWlRect)) : NULL;
            gc->clip_rect_count = 0;
            for (size_t i = 0; i < n && gc->clip_rects; ++i) {
                gc->clip_rects[i].x = rd_i16(req + 12 + i * 8);
                gc->clip_rects[i].y = rd_i16(req + 14 + i * 8);
                gc->clip_rects[i].width = rd_u16(req + 16 + i * 8);
                gc->clip_rects[i].height = rd_u16(req + 18 + i * 8);
                gc->clip_rect_count++;
            }
            gc->has_clip_rects = true;
        }
        break;
    }

    case 60: { /* FreeGC */
        if (req_len < 8) break;
        uint32_t gid = rd_u32(req + 4);
        XmWlGC **pp = &srv->gc_buckets[gid & 0xffu];
        while (*pp) {
            if ((*pp)->id == gid) {
                XmWlGC *v = *pp;
                *pp = v->hash_next;
                free(v->clip_rects);
                free(v);
                break;
            }
            pp = &(*pp)->hash_next;
        }
        break;
    }

    case 61: { /* ClearArea */
        if (req_len < 16) break;
        XmWlWindow *w = _XmWlFindWindow(srv, rd_u32(req + 4));
        if (!w || !is_window_viewable(srv, w)) break;
        int16_t cx = rd_i16(req + 8);
        int16_t cy = rd_i16(req + 10);
        uint16_t cw = rd_u16(req + 12);
        uint16_t ch = rd_u16(req + 14);
        if (cw == 0) cw = (cx < (int)w->width) ? (uint16_t)(w->width - cx) : 0;
        if (ch == 0) ch = (cy < (int)w->height) ? (uint16_t)(w->height - cy) : 0;
        if (cw > 0 && ch > 0) {
            _XmWlPaintWindowBackground(srv, w, cx, cy, cw, ch);
            if (data1 != 0) {
                uint8_t ev[32];
                memset(ev, 0, sizeof(ev));
                ev[0] = 12; /* Expose */
                wr_u32(ev + 4, w->id);
                wr_u16(ev + 8, (uint16_t)cx);
                wr_u16(ev + 10, (uint16_t)cy);
                wr_u16(ev + 12, cw);
                wr_u16(ev + 14, ch);
                _XmWlSendEventToWindow(srv, w, X11_EXPOSURE_MASK, ev);
            }
        }
        break;
    }

    case 62: { /* CopyArea */
        if (req_len < 28) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 12));
        if (gc)
            _XmWlRenderCopyArea(srv, cli, rd_u32(req + 4), rd_u32(req + 8), gc,
                                rd_i16(req + 16), rd_i16(req + 18),
                                rd_u16(req + 24), rd_u16(req + 26),
                                rd_i16(req + 20), rd_i16(req + 22));
        break;
    }

    case 63: { /* CopyPlane */
        if (req_len < 32) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 12));
        if (gc)
            _XmWlRenderCopyPlane(srv, cli, rd_u32(req + 4), rd_u32(req + 8), gc,
                                 rd_i16(req + 16), rd_i16(req + 18),
                                 rd_u16(req + 24), rd_u16(req + 26),
                                 rd_i16(req + 20), rd_i16(req + 22), rd_u32(req + 28));
        break;
    }

    case 64: { /* PolyPoint */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) _XmWlRenderPolyPoint(srv, rd_u32(req + 4), gc, data1, req + 12, (req_len - 12) / 4u);
        break;
    }

    case 65: { /* PolyLine */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) _XmWlRenderPolyLine(srv, rd_u32(req + 4), gc, data1, req + 12, (req_len - 12) / 4u);
        break;
    }

    case 66: { /* PolySegment */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) _XmWlRenderPolySegment(srv, rd_u32(req + 4), gc, req + 12, (req_len - 12) / 8u);
        break;
    }

    case 67: { /* PolyRectangle */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) _XmWlRenderPolyRect(srv, rd_u32(req + 4), gc, req + 12, (req_len - 12) / 8u);
        break;
    }

    case 68: /* PolyArc */
    case 71: { /* PolyFillArc */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) _XmWlRenderPolyArc(srv, rd_u32(req + 4), gc, req + 12, (req_len - 12) / 12u, major == 71);
        break;
    }

    case 69: { /* FillPoly */
        if (req_len < 16) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) _XmWlRenderFillPoly(srv, rd_u32(req + 4), gc, req[13], req + 16, (req_len - 16) / 4u);
        break;
    }

    case 70: { /* PolyFillRectangle */
        if (req_len < 12) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) _XmWlRenderPolyFillRect(srv, rd_u32(req + 4), gc, req + 12, (req_len - 12) / 8u);
        break;
    }

    case 72: { /* PutImage */
        if (req_len < 24) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc)
            _XmWlRenderPutImage(srv, rd_u32(req + 4), gc, data1,
                                rd_u16(req + 12), rd_u16(req + 14),
                                rd_i16(req + 16), rd_i16(req + 18),
                                req[20], req[21], req + 24, req_len - 24);
        break;
    }

    case 73: { /* GetImage */
        if (req_len < 20) break;
        uint8_t *bytes = NULL;
        size_t blen = 0;
        uint8_t out_depth = 24;
        uint32_t out_vis = XM_WL_VISUAL_RGB24_ID;
        if (!_XmWlRenderGetImage(srv, rd_u32(req + 4), rd_i16(req + 8), rd_i16(req + 10),
                                 rd_u16(req + 12), rd_u16(req + 14), rd_u32(req + 16), data1,
                                 &bytes, &blen, &out_depth, &out_vis)) {
            _XmWlSendError(cli, 8 /* BadMatch */, rd_u32(req + 4), 0, major);
            break;
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = out_depth;
        wr_u32(hdr + 8, out_vis);
        _XmWlSendReply(cli, hdr, bytes, blen);
        free(bytes);
        break;
    }

    case 74: /* PolyText8 */
    case 75: { /* PolyText16 */
        if (req_len < 16) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) _XmWlRenderPolyText(srv, rd_u32(req + 4), gc, rd_i16(req + 12), rd_i16(req + 14),
                                    req + 16, req_len - 16, major == 75);
        break;
    }

    case 76: /* ImageText8 */
    case 77: { /* ImageText16 */
        if (req_len < 16) break;
        XmWlGC *gc = _XmWlFindGC(srv, rd_u32(req + 8));
        if (gc) {
            size_t avail = req_len - 16;
            size_t cnt = data1;
            size_t need = (major == 77) ? cnt * 2u : cnt;
            if (need <= avail)
                _XmWlRenderImageText(srv, rd_u32(req + 4), gc, rd_i16(req + 12), rd_i16(req + 14),
                                     req + 16, cnt, major == 77);
        }
        break;
    }

    case 83: { /* ListInstalledColormaps */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, 1);
        uint32_t cm = XM_WL_DEFAULT_CMAP_XID;
        _XmWlSendReply(cli, hdr, &cm, 4);
        break;
    }

    case 84: { /* AllocColor */
        if (req_len < 16) break;
        uint16_t r16 = rd_u16(req + 8);
        uint16_t g16 = rd_u16(req + 10);
        uint16_t b16 = rd_u16(req + 12);
        uint32_t pix = ((uint32_t)(r16 >> 8) << 16) |
                       ((uint32_t)(g16 >> 8) << 8)  |
                       (uint32_t)(b16 >> 8);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, (uint16_t)((r16 & 0xff00u) | (r16 >> 8)));
        wr_u16(hdr + 10, (uint16_t)((g16 & 0xff00u) | (g16 >> 8)));
        wr_u16(hdr + 12, (uint16_t)((b16 & 0xff00u) | (b16 >> 8)));
        wr_u32(hdr + 16, pix);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 85: { /* AllocNamedColor */
        if (req_len < 12) break;
        uint16_t nlen = rd_u16(req + 8);
        if (12u + nlen > req_len) break;
        uint16_t r16 = 0, g16 = 0, b16 = 0;
        if (!lookup_x11_color((const char *)(req + 12), nlen, &r16, &g16, &b16)) {
            _XmWlSendError(cli, 15 /* BadName */, 0, 0, major);
            break;
        }
        uint32_t pix = ((uint32_t)(r16 >> 8) << 16) |
                       ((uint32_t)(g16 >> 8) << 8)  |
                       (uint32_t)(b16 >> 8);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, pix);
        wr_u16(hdr + 12, r16);
        wr_u16(hdr + 14, g16);
        wr_u16(hdr + 16, b16);
        wr_u16(hdr + 18, r16);
        wr_u16(hdr + 20, g16);
        wr_u16(hdr + 22, b16);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 86: /* AllocColorCells (read-only TrueColor -> BadAlloc) */
    case 87: { /* AllocColorPlanes */
        _XmWlSendError(cli, 11 /* BadAlloc */, 0, 0, major);
        break;
    }

    case 91: { /* QueryColors */
        if (req_len < 8) break;
        size_t n = (req_len - 8) / 4u;
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, (uint16_t)n);
        uint8_t *rgb = n > 0 ? (uint8_t *)calloc(n, 8) : NULL;
        for (size_t i = 0; i < n && rgb; ++i) {
            uint32_t px = rd_u32(req + 8 + i * 4u);
            uint8_t r8 = (uint8_t)((px >> 16) & 0xffu);
            uint8_t g8 = (uint8_t)((px >> 8) & 0xffu);
            uint8_t b8 = (uint8_t)(px & 0xffu);
            wr_u16(rgb + i * 8 + 0, (uint16_t)(r8 * 0x0101u));
            wr_u16(rgb + i * 8 + 2, (uint16_t)(g8 * 0x0101u));
            wr_u16(rgb + i * 8 + 4, (uint16_t)(b8 * 0x0101u));
        }
        _XmWlSendReply(cli, hdr, rgb, n * 8u);
        free(rgb);
        break;
    }

    case 92: { /* LookupColor */
        if (req_len < 12) break;
        uint16_t nlen = rd_u16(req + 8);
        if (12u + nlen > req_len) break;
        uint16_t r16 = 0, g16 = 0, b16 = 0;
        if (!lookup_x11_color((const char *)(req + 12), nlen, &r16, &g16, &b16)) {
            _XmWlSendError(cli, 15 /* BadName */, 0, 0, major);
            break;
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, r16);
        wr_u16(hdr + 10, g16);
        wr_u16(hdr + 12, b16);
        wr_u16(hdr + 14, r16);
        wr_u16(hdr + 16, g16);
        wr_u16(hdr + 18, b16);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 97: { /* QueryBestSize */
        if (req_len < 12) break;
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, rd_u16(req + 8) ? rd_u16(req + 8) : 16);
        wr_u16(hdr + 10, rd_u16(req + 10) ? rd_u16(req + 10) : 16);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 98: { /* QueryExtension */
        if (req_len < 8) break;
        uint16_t nlen = rd_u16(req + 4);
        if (8u + nlen > req_len) break;
        const char *enm = (const char *)(req + 8);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        if (nlen == 12 && !memcmp(enm, "BIG-REQUESTS", 12)) {
            hdr[8] = 1; hdr[9] = XM_WL_EXT_BIGREQ_OP;
        } else if (nlen == 7 && !memcmp(enm, "XC-MISC", 7)) {
            hdr[8] = 1; hdr[9] = XM_WL_EXT_XCMISC_OP;
        } else if (nlen == 6 && !memcmp(enm, "RENDER", 6)) {
            hdr[8] = 1; hdr[9] = XM_WL_EXT_RENDER_OP; hdr[11] = XM_WL_RENDER_ERROR_BASE;
        } else if (nlen == 5 && !memcmp(enm, "SHAPE", 5)) {
            hdr[8] = 1; hdr[9] = XM_WL_EXT_SHAPE_OP; hdr[10] = XM_WL_SHAPE_EVENT_BASE;
        } else if (nlen == 6 && !memcmp(enm, "XFIXES", 6)) {
            hdr[8] = 1; hdr[9] = XM_WL_EXT_XFIXES_OP; hdr[10] = XM_WL_XFIXES_EVENT_BASE; hdr[11] = XM_WL_XFIXES_ERROR_BASE;
        } else if (nlen == 5 && !memcmp(enm, "RANDR", 5)) {
            hdr[8] = 1; hdr[9] = XM_WL_EXT_RANDR_OP; hdr[10] = XM_WL_RANDR_EVENT_BASE; hdr[11] = XM_WL_RANDR_ERROR_BASE;
        } else if (nlen == 8 && !memcmp(enm, "XINERAMA", 8)) {
            hdr[8] = 1; hdr[9] = XM_WL_EXT_XINERAMA_OP;
        }
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 99: { /* ListExtensions */
        static const char *const exts[] = {
            "BIG-REQUESTS", "XC-MISC", "RENDER", "SHAPE", "XFIXES", "RANDR", "XINERAMA"
        };
        uint8_t buf[128];
        size_t off = 0;
        for (size_t i = 0; i < sizeof(exts) / sizeof(exts[0]); ++i) {
            size_t nl = strlen(exts[i]);
            buf[off++] = (uint8_t)nl;
            memcpy(buf + off, exts[i], nl);
            off += nl;
        }
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = (uint8_t)(sizeof(exts) / sizeof(exts[0]));
        _XmWlSendReply(cli, hdr, buf, off);
        break;
    }

    case 101: { /* GetKeyboardMapping */
        if (req_len < 8) break;
        uint8_t first = req[4];
        uint8_t count = req[5];
        if (first < 8) first = 8;
        if ((int)first + (int)count > 256)
            count = (uint8_t)(256 - first);
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 4; /* keysyms_per_keycode */
        size_t elen = (size_t)count * 4u * 4u;
        uint32_t *syms = elen > 0 ? (uint32_t *)malloc(elen) : NULL;
        for (uint8_t i = 0; i < count && syms; ++i) {
            memcpy(&syms[(size_t)i * 4u], srv->keysym_table[first + i], 16);
        }
        _XmWlSendReply(cli, hdr, syms, elen);
        free(syms);
        break;
    }

    case 103: { /* GetKeyboardControl (52 bytes: 32 hdr + 20 extra) */
        uint8_t hdr[32];
        uint8_t extra[20];
        memset(hdr, 0, sizeof(hdr));
        memset(extra, 0, sizeof(extra));
        hdr[1] = 1; /* global_auto_repeat */
        hdr[12] = 50; /* bell_percent */
        wr_u16(hdr + 14, 400);
        wr_u16(hdr + 16, 100);
        _XmWlSendReply(cli, hdr, extra, 20);
        break;
    }

    case 106: { /* GetPointerControl */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u16(hdr + 8, 2);
        wr_u16(hdr + 10, 1);
        wr_u16(hdr + 12, 4);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 108: { /* GetScreenSaver */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 110: { /* ListHosts */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case 117: { /* GetPointerMapping */
        uint8_t map[5] = {1, 2, 3, 4, 5};
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 5;
        _XmWlSendReply(cli, hdr, map, 5);
        break;
    }

    case 119: { /* GetModifierMapping */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        hdr[1] = 4; /* keycodes_per_modifier */
        _XmWlSendReply(cli, hdr, srv->mod_map, 32);
        break;
    }

    case XM_WL_EXT_BIGREQ_OP: { /* BIG-REQUESTS */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        wr_u32(hdr + 8, 0x00ffffffu);
        _XmWlSendReply(cli, hdr, NULL, 0);
        break;
    }

    case XM_WL_EXT_XCMISC_OP: { /* XC-MISC */
        uint8_t hdr[32];
        memset(hdr, 0, sizeof(hdr));
        if (data1 == 0) { /* GetVersion */
            wr_u16(hdr + 8, 1);
            wr_u16(hdr + 10, 1);
            _XmWlSendReply(cli, hdr, NULL, 0);
        } else if (data1 == 1) { /* GetXIDRange */
            wr_u32(hdr + 8, cli->resource_base | 0x00100000u);
            wr_u32(hdr + 12, 0x000fffffu);
            _XmWlSendReply(cli, hdr, NULL, 0);
        } else if (data1 == 2 && req_len >= 8) { /* GetXIDList */
            uint32_t cnt = rd_u32(req + 4);
            if (cnt > 256) cnt = 256;
            wr_u32(hdr + 8, cnt);
            uint32_t *ids = cnt > 0 ? (uint32_t *)malloc(cnt * 4u) : NULL;
            static uint32_t next_misc = 0x00180000u;
            for (uint32_t i = 0; i < cnt && ids; ++i)
                ids[i] = cli->resource_base | (next_misc++ & 0x001fffffu);
            _XmWlSendReply(cli, hdr, ids, cnt * 4u);
            free(ids);
        }
        break;
    }

    case XM_WL_EXT_RENDER_OP:
        _XmWlHandleRenderRequest(srv, cli, data1, req, req_len);
        break;

    case XM_WL_EXT_SHAPE_OP:
        handle_shape_request(srv, cli, data1, req, req_len);
        break;

    case XM_WL_EXT_XFIXES_OP:
        handle_xfixes_request(srv, cli, data1, req, req_len);
        break;

    case XM_WL_EXT_RANDR_OP:
        handle_randr_request(srv, cli, data1, req, req_len);
        break;

    case XM_WL_EXT_XINERAMA_OP:
        handle_xinerama_request(srv, cli, data1);
        break;

    default:
        /* Void no-op opcodes: UngrabKeyboard(32), GrabKey(33), UngrabKey(34),
         * AllowEvents(35), GrabServer(36), UngrabServer(37), WarpPointer(41),
         * SetFontPath(51), CreateColormap(78), FreeColormap(79), FreeColors(88),
         * CreateCursor(93), CreateGlyphCursor(94), FreeCursor(95), RecolorCursor(96),
         * ChangeKeyboardControl(102), Bell(104), SetScreenSaver(107),
         * SetCloseDownMode(112), ForceScreenSaver(115), NoOperation(127) */
        break;
    }
}

/* ---------- Client Setup Handshake & Stream Framing ---------- */

static void
send_setup_acceptance(XmWlServer *srv, XmWlClient *cli)
{
    if (srv && srv->root) {
        uint32_t a_sun_tt = _XmWlInternAtom(srv, "_SUN_TT_SESSION", 15, false);
        if (!_XmWlGetProperty(srv->root, a_sun_tt)) {
            const char *tt_sess = getenv("TT_SESSION");
            if (!tt_sess || !tt_sess[0])
                tt_sess = getenv("_SUN_TT_SESSION");
            if (tt_sess && tt_sess[0]) {
                size_t tt_len = strlen(tt_sess);
                uint32_t a_tt = _XmWlInternAtom(srv, "TT_SESSION", 10, false);
                _XmWlSetProperty(srv, srv->root, a_sun_tt, 31u /* XA_STRING */, 8, 0,
                                 (const uint8_t *)tt_sess, tt_len, false);
                _XmWlSetProperty(srv, srv->root, a_tt, 31u /* XA_STRING */, 8, 0,
                                 (const uint8_t *)tt_sess, tt_len, false);
            }
        }
    }

    uint8_t setup[256];
    memset(setup, 0, sizeof(setup));
    size_t p = 0;

    setup[0] = 1; /* Success */
    wr_u16(setup + 2, 11);
    wr_u16(setup + 4, 0);
    p = 8;

    const char *vendor = "CK-Motif-WlX11"; /* 14 bytes -> padded to 16 */
    wr_u32(setup + p + 0, 12000000u);      /* release_number */
    wr_u32(setup + p + 4, cli->resource_base);
    wr_u32(setup + p + 8, 0x001fffffu);    /* resource_id_mask */
    wr_u32(setup + p + 12, 256u);          /* motion_buffer_size */
    wr_u16(setup + p + 16, 14);            /* vendor_len */
    wr_u16(setup + p + 18, 65535);         /* maximum_request_length */
    setup[p + 20] = 1;                     /* roots_len */
    setup[p + 21] = 4;                     /* pixmap_formats_len */
    setup[p + 22] = 0;                     /* image_byte_order = LSBFirst */
    setup[p + 23] = 0;                     /* bitmap_format_bit_order = LeastSignificant */
    setup[p + 24] = 32;                    /* bitmap_format_scanline_unit */
    setup[p + 25] = 32;                    /* bitmap_format_scanline_pad */
    setup[p + 26] = 8;                     /* min_keycode */
    setup[p + 27] = 255;                   /* max_keycode */
    p += 32;

    memcpy(setup + p, vendor, 14);
    p += 16;

    /* 4 Pixmap Formats: depths 1, 8, 24, 32 */
    const uint8_t fmts[4][2] = { {1, 1}, {8, 8}, {24, 32}, {32, 32} };
    for (int i = 0; i < 4; ++i) {
        setup[p + 0] = fmts[i][0];
        setup[p + 1] = fmts[i][1];
        setup[p + 2] = 32;
        p += 8;
    }

    /* Screen 0 (40 bytes) */
    wr_u32(setup + p + 0, XM_WL_ROOT_XID);
    wr_u32(setup + p + 4, XM_WL_DEFAULT_CMAP_XID);
    wr_u32(setup + p + 8, 0x00ffffffu);
    wr_u32(setup + p + 12, 0x00000000u);
    wr_u32(setup + p + 16, 0);
    wr_u16(setup + p + 20, (uint16_t)srv->screen_w);
    wr_u16(setup + p + 22, (uint16_t)srv->screen_h);
    wr_u16(setup + p + 24, (uint16_t)srv->screen_mm_w);
    wr_u16(setup + p + 26, (uint16_t)srv->screen_mm_h);
    wr_u16(setup + p + 28, 1);
    wr_u16(setup + p + 30, 1);
    wr_u32(setup + p + 32, XM_WL_VISUAL_RGB24_ID);
    setup[p + 36] = 0;  /* backing_stores */
    setup[p + 37] = 0;  /* save_unders */
    setup[p + 38] = 24; /* root_depth */
    setup[p + 39] = 4;  /* allowed_depths_len */
    p += 40;

    /* Depth 24 -> Visual 0x21 (TrueColor) */
    setup[p + 0] = 24;
    wr_u16(setup + p + 2, 1);
    p += 8;
    wr_u32(setup + p + 0, XM_WL_VISUAL_RGB24_ID);
    setup[p + 4] = 4; /* TrueColor */
    setup[p + 5] = 8; /* bits_per_rgb_value */
    wr_u16(setup + p + 6, 256);
    wr_u32(setup + p + 8, 0x00ff0000u);
    wr_u32(setup + p + 12, 0x0000ff00u);
    wr_u32(setup + p + 16, 0x000000ffu);
    p += 24;

    /* Depth 32 -> Visual 0x22 (TrueColor ARGB) */
    setup[p + 0] = 32;
    wr_u16(setup + p + 2, 1);
    p += 8;
    wr_u32(setup + p + 0, XM_WL_VISUAL_ARGB32_ID);
    setup[p + 4] = 4; /* TrueColor */
    setup[p + 5] = 8;
    wr_u16(setup + p + 6, 256);
    wr_u32(setup + p + 8, 0x00ff0000u);
    wr_u32(setup + p + 12, 0x0000ff00u);
    wr_u32(setup + p + 16, 0x000000ffu);
    p += 24;

    /* Depth 1 (0 visuals) */
    setup[p + 0] = 1;
    p += 8;

    /* Depth 8 (0 visuals) */
    setup[p + 0] = 8;
    p += 8;

    wr_u16(setup + 6, (uint16_t)((p - 8u) / 4u));
    queue_tx_bytes(cli, setup, p);
}

void
_XmWlHandleClientReadable(XmWlServer *srv, int client_idx)
{
    if (!srv || client_idx < 0 || client_idx >= XM_WL_MAX_CLIENTS)
        return;
    XmWlClient *cli = &srv->clients[client_idx];
    if (!cli->active || cli->fd < 0)
        return;

    uint8_t tmp[65536];
    for (;;) {
        ssize_t n = recv(cli->fd, tmp, sizeof(tmp), 0);
        if (n > 0) {
            if (cli->rx_len + (size_t)n > cli->rx_cap) {
                size_t ncap = cli->rx_cap ? cli->rx_cap * 2 : 65536;
                while (ncap < cli->rx_len + (size_t)n)
                    ncap *= 2;
                uint8_t *nb = (uint8_t *)realloc(cli->rx_buf, ncap);
                if (!nb) {
                    _XmWlDisconnectClient(srv, client_idx);
                    return;
                }
                cli->rx_buf = nb;
                cli->rx_cap = ncap;
            }
            memcpy(cli->rx_buf + cli->rx_len, tmp, (size_t)n);
            cli->rx_len += (size_t)n;
        } else if (n == 0) {
            /* Drain any complete requests already buffered before closing */
            break;
        } else if (errno == EINTR) {
            continue;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            break;
        } else {
            _XmWlDisconnectClient(srv, client_idx);
            return;
        }
    }

    size_t consumed = 0;
    while (cli->active && consumed < cli->rx_len) {
        const uint8_t *cur = cli->rx_buf + consumed;
        size_t avail = cli->rx_len - consumed;

        if (!cli->handshake_done) {
            if (avail < 12) break;
            uint16_t nlen = rd_u16(cur + 6);
            uint16_t dlen = rd_u16(cur + 8);
            size_t need = 12u + ((nlen + 3u) & ~3u) + ((dlen + 3u) & ~3u);
            if (avail < need) break;
            cli->handshake_done = true;
            send_setup_acceptance(srv, cli);
            consumed += need;
            continue;
        }

        if (avail < 4) break;
        uint16_t len16 = rd_u16(cur + 2);
        if (len16 != 0) {
            size_t req_bytes = (size_t)len16 * 4u;
            if (avail < req_bytes) break;
            dispatch_x11_request(srv, cli, cur, req_bytes);
            consumed += req_bytes;
        } else {
            /* BIG-REQUESTS 8-byte extended length header */
            if (avail < 8) break;
            uint32_t len32 = rd_u32(cur + 4);
            if (len32 < 2 || len32 > 0x00ffffffu) {
                _XmWlDisconnectClient(srv, client_idx);
                return;
            }
            size_t wire_bytes = (size_t)len32 * 4u;
            if (avail < wire_bytes) break;
            size_t norm_bytes = wire_bytes - 4u;
            uint8_t *norm = (uint8_t *)malloc(norm_bytes);
            if (!norm) {
                _XmWlDisconnectClient(srv, client_idx);
                return;
            }
            memcpy(norm, cur, 4);
            memcpy(norm + 4, cur + 8, wire_bytes - 8u);
            dispatch_x11_request(srv, cli, norm, norm_bytes);
            free(norm);
            consumed += wire_bytes;
        }
    }

    if (cli->active && consumed > 0) {
        if (consumed < cli->rx_len)
            memmove(cli->rx_buf, cli->rx_buf + consumed, cli->rx_len - consumed);
        cli->rx_len -= consumed;
    }
}

#endif /* HAVE_WAYLAND_BACKEND */
