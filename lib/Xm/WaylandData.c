/*
 * Native Wayland selections (wl_data_device & zwp_primary_selection_v1) and
 * drag-and-drop protocol bridge for the libXm Wayland backend.
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

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

static inline uint32_t rd_u32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline void     wr_u32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }

static void
write_all_fd(int fd, const uint8_t *buf, size_t len)
{
    size_t off = 0;
    while (off < len) {
        ssize_t n = write(fd, buf + off, len - off);
        if (n > 0) {
            off += (size_t)n;
        } else if (n < 0 && errno == EINTR) {
            continue;
        } else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            struct pollfd pfd = { .fd = fd, .events = POLLOUT };
            if (poll(&pfd, 1, 500) <= 0)
                break;
        } else {
            break;
        }
    }
}

static uint8_t *
read_all_offer_fd(XmWlServer *srv, int rfd, size_t *out_len)
{
    *out_len = 0;
    int flags = fcntl(rfd, F_GETFL, 0);
    if (flags >= 0)
        fcntl(rfd, F_SETFL, flags | O_NONBLOCK);

    size_t cap = 4096;
    size_t len = 0;
    uint8_t *buf = (uint8_t *)malloc(cap);
    if (!buf)
        return NULL;

    int wl_fd = srv->wl_dpy ? wl_display_get_fd(srv->wl_dpy) : -1;
    int elapsed_ms = 0;

    while (elapsed_ms < 1500) {
        ssize_t n = read(rfd, buf + len, cap - len - 1);
        if (n > 0) {
            len += (size_t)n;
            if (cap - len < 1024) {
                size_t new_cap = cap * 2;
                if (new_cap > 16u * 1024u * 1024u)
                    break;
                uint8_t *nb = (uint8_t *)realloc(buf, new_cap);
                if (!nb) break;
                buf = nb;
                cap = new_cap;
            }
            continue;
        }
        if (n == 0)
            break;
        if (errno == EINTR)
            continue;
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            break;

        if (srv->wl_dpy)
            wl_display_flush(srv->wl_dpy);

        struct pollfd pfds[2];
        nfds_t nfds = 0;
        pfds[nfds++] = (struct pollfd){ .fd = rfd, .events = POLLIN };
        if (wl_fd >= 0)
            pfds[nfds++] = (struct pollfd){ .fd = wl_fd, .events = POLLIN };

        int pr = poll(pfds, nfds, 50);
        elapsed_ms += 50;
        if (pr > 0 && nfds == 2 && (pfds[1].revents & POLLIN)) {
            wl_display_dispatch_pending(srv->wl_dpy);
        }
    }
    buf[len] = '\0';
    *out_len = len;
    return buf;
}

static void
emit_xfixes_selection_notify(XmWlServer *srv, uint32_t selection, uint32_t owner, uint32_t time_ms)
{
    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        XmWlClient *cli = &srv->clients[i];
        if (!cli->active || !cli->handshake_done) continue;
        for (int s = 0; s < 8; ++s) {
            if (cli->xfixes_sel_atoms[s] == selection && (cli->xfixes_sel_masks[s] & 1u)) {
                uint8_t ev[32];
                memset(ev, 0, sizeof(ev));
                ev[0] = XM_WL_XFIXES_EVENT_BASE;
                ev[1] = 0; /* SetSelectionOwnerNotify */
                wr_u32(ev + 4, XM_WL_ROOT_XID);
                wr_u32(ev + 8, owner);
                wr_u32(ev + 12, selection);
                wr_u32(ev + 16, time_ms);
                wr_u32(ev + 20, time_ms);
                _XmWlSendEventToClient(cli, ev);
            }
        }
    }
}

static void
emit_selection_clear_to_owner(XmWlServer *srv, uint32_t old_owner, uint32_t selection, uint32_t time_ms)
{
    if (old_owner == 0 || old_owner == XM_WL_PROXY_SEL_XID)
        return;
    XmWlWindow *w = _XmWlFindWindow(srv, old_owner);
    if (!w) return;
    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = 29; /* SelectionClear */
    wr_u32(ev + 4, time_ms);
    wr_u32(ev + 8, old_owner);
    wr_u32(ev + 12, selection);
    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        if (srv->clients[i].active && srv->clients[i].handshake_done)
            _XmWlSendEventToClient(&srv->clients[i], ev);
    }
}

static bool
is_text_target_name(const char *tname)
{
    if (!tname) return false;
    return !strcmp(tname, "UTF8_STRING") ||
           !strcmp(tname, "STRING") ||
           !strcmp(tname, "TEXT") ||
           !strcmp(tname, "COMPOUND_TEXT") ||
           !strncasecmp(tname, "text/plain", 10);
}

static const char *
match_offer_mime_for_target(XmWlServer *srv, uint32_t target,
                            char mimes[16][96], int mime_count,
                            uint32_t *out_prop_type)
{
    const char *tname = _XmWlAtomName(srv, target);
    if (!tname) return NULL;
    *out_prop_type = target;

    /* Exact match first */
    for (int i = 0; i < mime_count; ++i) {
        if (!strcasecmp(mimes[i], tname))
            return mimes[i];
    }

    /* Text Equivalence */
    if (is_text_target_name(tname)) {
        const char *prefs[] = {
            "text/plain;charset=utf-8",
            "UTF8_STRING",
            "text/plain",
            "STRING",
            "TEXT",
        };
        for (size_t p = 0; p < sizeof(prefs) / sizeof(prefs[0]); ++p) {
            for (int i = 0; i < mime_count; ++i) {
                if (!strcasecmp(mimes[i], prefs[p]))
                    return mimes[i];
            }
        }
    }
    return NULL;
}

static uint32_t
target_atom_for_wayland_mime(XmWlServer *srv, const char *mime_type)
{
    if (!mime_type || !mime_type[0])
        return _XmWlInternAtom(srv, "UTF8_STRING", 11, false);
    if (!strcasecmp(mime_type, "text/plain;charset=utf-8") ||
        !strcasecmp(mime_type, "text/plain") ||
        !strcmp(mime_type, "UTF8_STRING")) {
        return _XmWlInternAtom(srv, "UTF8_STRING", 11, false);
    }
    if (!strcmp(mime_type, "STRING"))
        return 31u; /* XA_STRING */
    if (!strcmp(mime_type, "TEXT"))
        return _XmWlInternAtom(srv, "TEXT", 4, false);
    return _XmWlInternAtom(srv, mime_type, strlen(mime_type), false);
}

static void
queue_wayland_send_request(XmWlServer *srv, uint32_t selection_atom, uint32_t owner_xid,
                           const char *mime_type, int fd)
{
    if (!srv || owner_xid == 0 || owner_xid == XM_WL_PROXY_SEL_XID) {
        if (fd >= 0) close(fd);
        return;
    }

    int slot = -1;
    for (int i = 0; i < 8; ++i) {
        if (!srv->pending_transfers[i].active) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        if (fd >= 0) close(fd);
        return;
    }

    char prop_name[32];
    snprintf(prop_name, sizeof(prop_name), "_XM_WL_SEL_OUT_%d", slot);
    uint32_t prop_atom = _XmWlInternAtom(srv, prop_name, strlen(prop_name), false);
    uint32_t target_atom = target_atom_for_wayland_mime(srv, mime_type);
    uint32_t now_ms = _XmWlNowMs();

    _XmWlDeleteProperty(srv, srv->proxy_sel, prop_atom, false);

    XmWlPendingSelectionTransfer *tr = &srv->pending_transfers[slot];
    tr->active = true;
    tr->selection_atom = selection_atom;
    tr->requestor = XM_WL_PROXY_SEL_XID;
    tr->target_atom = target_atom;
    tr->property_atom = prop_atom;
    tr->time = now_ms;
    tr->write_fd = fd;
    tr->client_idx = -1;

    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = 30; /* SelectionRequest */
    wr_u32(ev + 4, now_ms);
    wr_u32(ev + 8, owner_xid);
    wr_u32(ev + 12, XM_WL_PROXY_SEL_XID);
    wr_u32(ev + 16, selection_atom);
    wr_u32(ev + 20, target_atom);
    wr_u32(ev + 24, prop_atom);

    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        if (srv->clients[i].active && srv->clients[i].handshake_done)
            _XmWlSendEventToClient(&srv->clients[i], ev);
    }
}

/* ---------- wl_data_source Listeners (Clipboard & Drag Source) ---------- */

static void
clip_source_target(void *data, struct wl_data_source *source, const char *mime_type)
{
    (void)data; (void)source; (void)mime_type;
}

static void
clip_source_send(void *data, struct wl_data_source *source, const char *mime_type, int32_t fd)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || source != srv->wl_clip_source) {
        if (fd >= 0) close(fd);
        return;
    }
    uint32_t a_clip = _XmWlInternAtom(srv, "CLIPBOARD", 9, false);
    queue_wayland_send_request(srv, a_clip, srv->sel_clipboard_owner, mime_type, fd);
}

static void
clip_source_cancelled(void *data, struct wl_data_source *source)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) {
        wl_data_source_destroy(source);
        return;
    }
    if (srv->wl_clip_source == source) {
        srv->wl_clip_source = NULL;
        uint32_t a_clip = _XmWlInternAtom(srv, "CLIPBOARD", 9, false);
        uint32_t old = srv->sel_clipboard_owner;
        if (old != 0 && old != XM_WL_PROXY_SEL_XID) {
            srv->sel_clipboard_owner = 0;
            emit_selection_clear_to_owner(srv, old, a_clip, _XmWlNowMs());
        }
    }
    wl_data_source_destroy(source);
}

static void clip_source_dnd_drop(void *d, struct wl_data_source *s) { (void)d; (void)s; }
static void clip_source_dnd_fin(void *d, struct wl_data_source *s)  { (void)d; (void)s; }
static void clip_source_action(void *d, struct wl_data_source *s, uint32_t a) { (void)d; (void)s; (void)a; }

static const struct wl_data_source_listener g_clip_source_listener = {
    .target = clip_source_target,
    .send = clip_source_send,
    .cancelled = clip_source_cancelled,
    .dnd_drop_performed = clip_source_dnd_drop,
    .dnd_finished = clip_source_dnd_fin,
    .action = clip_source_action,
};

static void
drag_source_target(void *data, struct wl_data_source *source, const char *mime_type)
{
    (void)data; (void)source; (void)mime_type;
}

static void
drag_source_send(void *data, struct wl_data_source *source, const char *mime_type, int32_t fd)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || source != srv->wl_drag_source) {
        if (fd >= 0) close(fd);
        return;
    }
    uint32_t a_xdnd = _XmWlInternAtom(srv, "XdndSelection", 13, false);
    queue_wayland_send_request(srv, a_xdnd, srv->sel_xdnd_owner, mime_type, fd);
}

static void
cleanup_drag_source(XmWlServer *srv, struct wl_data_source *source)
{
    if (srv->wl_drag_source == source) {
        srv->wl_drag_source = NULL;
        if (srv->wl_drag_icon_surf) {
            wl_surface_destroy(srv->wl_drag_icon_surf);
            srv->wl_drag_icon_surf = NULL;
        }
        /* Synthesize ButtonRelease if Motif's drag controller is still grabbing pointer */
        if (srv->button_mask != 0)
            _XmWlDispatchPointerButton(srv, 1, false, _XmWlNowMs());
    }
    wl_data_source_destroy(source);
}

static void
drag_source_cancelled(void *data, struct wl_data_source *source)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (srv) cleanup_drag_source(srv, source);
    else wl_data_source_destroy(source);
}

static void
drag_source_dnd_drop(void *data, struct wl_data_source *source)
{
    (void)source;
    XmWlServer *srv = (XmWlServer *)data;
    if (srv && srv->button_mask != 0)
        _XmWlDispatchPointerButton(srv, 1, false, _XmWlNowMs());
}

static void
drag_source_dnd_finished(void *data, struct wl_data_source *source)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (srv) cleanup_drag_source(srv, source);
    else wl_data_source_destroy(source);
}

static void
drag_source_action(void *data, struct wl_data_source *source, uint32_t dnd_action)
{
    (void)data; (void)source; (void)dnd_action;
}

static const struct wl_data_source_listener g_drag_source_listener = {
    .target = drag_source_target,
    .send = drag_source_send,
    .cancelled = drag_source_cancelled,
    .dnd_drop_performed = drag_source_dnd_drop,
    .dnd_finished = drag_source_dnd_finished,
    .action = drag_source_action,
};

/* ---------- wl_data_offer & wl_data_device Listeners ---------- */

static void
data_offer_offer(void *data, struct wl_data_offer *offer, const char *mime_type)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || !mime_type || offer != srv->wl_pending_offer)
        return;
    if (srv->wl_pending_mime_count < 16) {
        snprintf(srv->wl_pending_mimes[srv->wl_pending_mime_count++], 96, "%s", mime_type);
    }
}

static void data_offer_source_actions(void *d, struct wl_data_offer *o, uint32_t a) { (void)d; (void)o; (void)a; }
static void data_offer_action(void *d, struct wl_data_offer *o, uint32_t a) { (void)d; (void)o; (void)a; }

static const struct wl_data_offer_listener g_data_offer_listener = {
    .offer = data_offer_offer,
    .source_actions = data_offer_source_actions,
    .action = data_offer_action,
};

static XmWlWindow *
find_top_by_wl_surface(XmWlServer *srv, struct wl_surface *surface)
{
    if (!srv || !srv->root || !surface) return NULL;
    for (XmWlWindow *w = srv->root->first_child; w; w = w->next_sibling) {
        if (w->wl_surf == surface)
            return w;
    }
    return NULL;
}

static void
send_xdnd_client_message(XmWlServer *srv, XmWlWindow *top, uint32_t msg_atom,
                         uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4)
{
    if (!srv || !top) return;
    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = 33 | 0x80; /* ClientMessage (synthetic) */
    ev[1] = 32;
    wr_u32(ev + 4, top->id);
    wr_u32(ev + 8, msg_atom);
    wr_u32(ev + 12, d0);
    wr_u32(ev + 16, d1);
    wr_u32(ev + 20, d2);
    wr_u32(ev + 24, d3);
    wr_u32(ev + 28, d4);
    for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
        if (srv->clients[i].active && srv->clients[i].handshake_done)
            _XmWlSendEventToClient(&srv->clients[i], ev);
    }
}

static void
data_device_data_offer(void *data, struct wl_data_device *device, struct wl_data_offer *id)
{
    (void)device;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || !id) return;
    if (srv->wl_pending_offer && srv->wl_pending_offer != srv->wl_clip_offer &&
        srv->wl_pending_offer != srv->wl_dnd_offer) {
        wl_data_offer_destroy(srv->wl_pending_offer);
    }
    srv->wl_pending_offer = id;
    srv->wl_pending_mime_count = 0;
    wl_data_offer_add_listener(id, &g_data_offer_listener, srv);
}

static void
data_device_enter(void *data, struct wl_data_device *device, uint32_t serial,
                  struct wl_surface *surface, wl_fixed_t x, wl_fixed_t y,
                  struct wl_data_offer *id)
{
    (void)device;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;

    if (srv->wl_dnd_offer && srv->wl_dnd_offer != id) {
        wl_data_offer_destroy(srv->wl_dnd_offer);
        srv->wl_dnd_offer = NULL;
    }

    srv->wl_dnd_offer = id;
    srv->wl_dnd_serial = serial;
    srv->wl_dnd_drop_pending = false;
    srv->wl_dnd_x = (int16_t)wl_fixed_to_int(x);
    srv->wl_dnd_y = (int16_t)wl_fixed_to_int(y);
    srv->wl_dnd_target_top = find_top_by_wl_surface(srv, surface);

    if (id && id == srv->wl_pending_offer) {
        srv->wl_dnd_mime_count = srv->wl_pending_mime_count;
        memcpy(srv->wl_dnd_mimes, srv->wl_pending_mimes, sizeof(srv->wl_dnd_mimes));
        srv->wl_pending_offer = NULL;
        srv->wl_pending_mime_count = 0;
    } else {
        srv->wl_dnd_mime_count = 0;
    }

    if (!id || !srv->wl_dnd_target_top)
        return;

    uint32_t a_xdnd_sel = _XmWlInternAtom(srv, "XdndSelection", 13, false);
    uint32_t a_type_list = _XmWlInternAtom(srv, "XdndTypeList", 12, false);
    uint32_t a_enter = _XmWlInternAtom(srv, "XdndEnter", 9, false);
    uint32_t a_pos = _XmWlInternAtom(srv, "XdndPosition", 12, false);
    uint32_t a_copy = _XmWlInternAtom(srv, "XdndActionCopy", 14, false);

    srv->sel_xdnd_owner = XM_WL_PROXY_SEL_XID;
    (void)a_xdnd_sel;

    uint32_t type_atoms[16];
    int ntypes = 0;
    for (int i = 0; i < srv->wl_dnd_mime_count && ntypes < 16; ++i) {
        type_atoms[ntypes++] = _XmWlInternAtom(
            srv, srv->wl_dnd_mimes[i], strlen(srv->wl_dnd_mimes[i]), false);
    }
    _XmWlSetProperty(srv, srv->proxy_sel, a_type_list, 4u /* XA_ATOM */, 32, 0 /* Replace */,
                     (const uint8_t *)type_atoms, (size_t)ntypes * 4u, false);

    if (srv->wl_dnd_mime_count > 0) {
        wl_data_offer_accept(id, serial, srv->wl_dnd_mimes[0]);
        if (wl_data_offer_get_version(id) >= 3) {
            wl_data_offer_set_actions(
                id,
                WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY | WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE,
                WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY);
        }
    }

    uint32_t flags = (5u << 24) | (ntypes > 3 ? 1u : 0u);
    send_xdnd_client_message(
        srv, srv->wl_dnd_target_top, a_enter,
        XM_WL_PROXY_SEL_XID, flags,
        ntypes > 0 ? type_atoms[0] : 0u,
        ntypes > 1 ? type_atoms[1] : 0u,
        ntypes > 2 ? type_atoms[2] : 0u);

    int rx = (int)srv->wl_dnd_target_top->x + (int)srv->wl_dnd_x;
    int ry = (int)srv->wl_dnd_target_top->y + (int)srv->wl_dnd_y;
    uint32_t packed_xy = ((uint32_t)(uint16_t)rx << 16) | (uint16_t)ry;
    send_xdnd_client_message(
        srv, srv->wl_dnd_target_top, a_pos,
        XM_WL_PROXY_SEL_XID, 0u, packed_xy, _XmWlNowMs(), a_copy);
}

static void
data_device_leave(void *data, struct wl_data_device *device)
{
    (void)device;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;
    if (!srv->wl_dnd_drop_pending && srv->wl_dnd_target_top) {
        uint32_t a_leave = _XmWlInternAtom(srv, "XdndLeave", 9, false);
        send_xdnd_client_message(srv, srv->wl_dnd_target_top, a_leave,
                                 XM_WL_PROXY_SEL_XID, 0, 0, 0, 0);
        if (srv->wl_dnd_offer) {
            wl_data_offer_destroy(srv->wl_dnd_offer);
            srv->wl_dnd_offer = NULL;
        }
    }
    srv->wl_dnd_target_top = NULL;
}

static void
data_device_motion(void *data, struct wl_data_device *device, uint32_t time,
                   wl_fixed_t x, wl_fixed_t y)
{
    (void)device;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || !srv->wl_dnd_target_top) return;
    srv->wl_dnd_x = (int16_t)wl_fixed_to_int(x);
    srv->wl_dnd_y = (int16_t)wl_fixed_to_int(y);

    uint32_t a_pos = _XmWlInternAtom(srv, "XdndPosition", 12, false);
    uint32_t a_copy = _XmWlInternAtom(srv, "XdndActionCopy", 14, false);
    int rx = (int)srv->wl_dnd_target_top->x + (int)srv->wl_dnd_x;
    int ry = (int)srv->wl_dnd_target_top->y + (int)srv->wl_dnd_y;
    uint32_t packed_xy = ((uint32_t)(uint16_t)rx << 16) | (uint16_t)ry;
    send_xdnd_client_message(
        srv, srv->wl_dnd_target_top, a_pos,
        XM_WL_PROXY_SEL_XID, 0u, packed_xy, time ? time : _XmWlNowMs(), a_copy);
}

static void
data_device_drop(void *data, struct wl_data_device *device)
{
    (void)device;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || !srv->wl_dnd_target_top) return;
    srv->wl_dnd_drop_pending = true;
    uint32_t a_drop = _XmWlInternAtom(srv, "XdndDrop", 8, false);
    send_xdnd_client_message(
        srv, srv->wl_dnd_target_top, a_drop,
        XM_WL_PROXY_SEL_XID, 0u, _XmWlNowMs(), 0u, 0u);
}

static void
data_device_selection(void *data, struct wl_data_device *device, struct wl_data_offer *id)
{
    (void)device;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;

    if (srv->wl_clip_offer && srv->wl_clip_offer != id) {
        wl_data_offer_destroy(srv->wl_clip_offer);
        srv->wl_clip_offer = NULL;
    }
    srv->wl_clip_offer = id;
    if (id && id == srv->wl_pending_offer) {
        srv->wl_clip_mime_count = srv->wl_pending_mime_count;
        memcpy(srv->wl_clip_mimes, srv->wl_pending_mimes, sizeof(srv->wl_clip_mimes));
        srv->wl_pending_offer = NULL;
        srv->wl_pending_mime_count = 0;
    } else if (!id) {
        srv->wl_clip_mime_count = 0;
    }

    uint32_t a_clip = _XmWlInternAtom(srv, "CLIPBOARD", 9, false);
    uint32_t now_ms = _XmWlNowMs();

    /* If we ourselves hold a live wl_clip_source, keep our local X11 owner */
    if (srv->wl_clip_source != NULL && srv->sel_clipboard_owner != 0 &&
        srv->sel_clipboard_owner != XM_WL_PROXY_SEL_XID) {
        return;
    }

    uint32_t old = srv->sel_clipboard_owner;
    srv->sel_clipboard_owner = id ? XM_WL_PROXY_SEL_XID : 0u;
    if (old != srv->sel_clipboard_owner) {
        emit_selection_clear_to_owner(srv, old, a_clip, now_ms);
        emit_xfixes_selection_notify(srv, a_clip, srv->sel_clipboard_owner, now_ms);
    }
}

static const struct wl_data_device_listener g_data_device_listener = {
    .data_offer = data_device_data_offer,
    .enter = data_device_enter,
    .leave = data_device_leave,
    .motion = data_device_motion,
    .drop = data_device_drop,
    .selection = data_device_selection,
};

/* ---------- zwp_primary_selection_v1 Listeners ---------- */

static void
prim_source_send(void *data, struct zwp_primary_selection_source_v1 *source,
                 const char *mime_type, int32_t fd)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || source != srv->wl_prim_source) {
        if (fd >= 0) close(fd);
        return;
    }
    queue_wayland_send_request(srv, 1u /* XA_PRIMARY */, srv->sel_primary_owner, mime_type, fd);
}

static void
prim_source_cancelled(void *data, struct zwp_primary_selection_source_v1 *source)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (srv && srv->wl_prim_source == source) {
        srv->wl_prim_source = NULL;
        uint32_t old = srv->sel_primary_owner;
        if (old != 0 && old != XM_WL_PROXY_SEL_XID) {
            srv->sel_primary_owner = 0;
            emit_selection_clear_to_owner(srv, old, 1u /* XA_PRIMARY */, _XmWlNowMs());
        }
    }
    zwp_primary_selection_source_v1_destroy(source);
}

static const struct zwp_primary_selection_source_v1_listener g_prim_source_listener = {
    .send = prim_source_send,
    .cancelled = prim_source_cancelled,
};

static void
prim_offer_offer(void *data, struct zwp_primary_selection_offer_v1 *offer, const char *mime_type)
{
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || !mime_type || offer != srv->wl_pending_prim_offer)
        return;
    if (srv->wl_pending_prim_mime_count < 16) {
        snprintf(srv->wl_pending_prim_mimes[srv->wl_pending_prim_mime_count++], 96, "%s", mime_type);
    }
}

static const struct zwp_primary_selection_offer_v1_listener g_prim_offer_listener = {
    .offer = prim_offer_offer,
};

static void
prim_device_data_offer(void *data, struct zwp_primary_selection_device_v1 *device,
                       struct zwp_primary_selection_offer_v1 *offer)
{
    (void)device;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv || !offer) return;
    if (srv->wl_pending_prim_offer && srv->wl_pending_prim_offer != srv->wl_prim_offer) {
        zwp_primary_selection_offer_v1_destroy(srv->wl_pending_prim_offer);
    }
    srv->wl_pending_prim_offer = offer;
    srv->wl_pending_prim_mime_count = 0;
    zwp_primary_selection_offer_v1_add_listener(offer, &g_prim_offer_listener, srv);
}

static void
prim_device_selection(void *data, struct zwp_primary_selection_device_v1 *device,
                      struct zwp_primary_selection_offer_v1 *id)
{
    (void)device;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;

    if (srv->wl_prim_offer && srv->wl_prim_offer != id) {
        zwp_primary_selection_offer_v1_destroy(srv->wl_prim_offer);
        srv->wl_prim_offer = NULL;
    }
    srv->wl_prim_offer = id;
    if (id && id == srv->wl_pending_prim_offer) {
        srv->wl_prim_mime_count = srv->wl_pending_prim_mime_count;
        memcpy(srv->wl_prim_mimes, srv->wl_pending_prim_mimes, sizeof(srv->wl_prim_mimes));
        srv->wl_pending_prim_offer = NULL;
        srv->wl_pending_prim_mime_count = 0;
    } else if (!id) {
        srv->wl_prim_mime_count = 0;
    }

    if (srv->wl_prim_source != NULL && srv->sel_primary_owner != 0 &&
        srv->sel_primary_owner != XM_WL_PROXY_SEL_XID) {
        return;
    }

    uint32_t now_ms = _XmWlNowMs();
    uint32_t old = srv->sel_primary_owner;
    srv->sel_primary_owner = id ? XM_WL_PROXY_SEL_XID : 0u;
    if (old != srv->sel_primary_owner) {
        emit_selection_clear_to_owner(srv, old, 1u /* XA_PRIMARY */, now_ms);
        emit_xfixes_selection_notify(srv, 1u /* XA_PRIMARY */, srv->sel_primary_owner, now_ms);
    }
}

static const struct zwp_primary_selection_device_v1_listener g_prim_device_listener = {
    .data_offer = prim_device_data_offer,
    .selection = prim_device_selection,
};

/* ---------- Public Internal API ---------- */

void
_XmWlDataBindSeat(XmWlServer *srv)
{
    if (!srv || !srv->seat) return;
    if (srv->data_mgr && !srv->data_dev) {
        srv->data_dev = wl_data_device_manager_get_data_device(srv->data_mgr, srv->seat);
        if (srv->data_dev)
            wl_data_device_add_listener(srv->data_dev, &g_data_device_listener, srv);
    }
    if (srv->prim_mgr && !srv->prim_dev) {
        srv->prim_dev = zwp_primary_selection_device_manager_v1_get_device(srv->prim_mgr, srv->seat);
        if (srv->prim_dev)
            zwp_primary_selection_device_v1_add_listener(srv->prim_dev, &g_prim_device_listener, srv);
    }
}

static void
offer_standard_mimes_data_source(struct wl_data_source *src)
{
    static const char *const mimes[] = {
        "text/plain;charset=utf-8",
        "text/plain",
        "UTF8_STRING",
        "STRING",
        "TEXT",
        "text/uri-list",
        "x-special/gnome-copied-files",
        "image/png",
    };
    for (size_t i = 0; i < sizeof(mimes) / sizeof(mimes[0]); ++i)
        wl_data_source_offer(src, mimes[i]);
}

static struct {
    uint32_t atom;
    uint32_t owner;
} g_misc_selections[32];

void
_XmWlDataHandleSetSelectionOwner(XmWlServer *srv, uint32_t selection, uint32_t owner, uint32_t time)
{
    if (!srv) return;
    uint32_t now_ms = time ? time : _XmWlNowMs();
    uint32_t a_clip = _XmWlInternAtom(srv, "CLIPBOARD", 9, false);
    uint32_t a_xdnd = _XmWlInternAtom(srv, "XdndSelection", 13, false);

    if (owner != 0 && !_XmWlFindWindow(srv, owner))
        owner = 0;

    if (selection == a_clip) {
        uint32_t old = srv->sel_clipboard_owner;
        srv->sel_clipboard_owner = owner;
        if (old != owner)
            emit_selection_clear_to_owner(srv, old, selection, now_ms);

        if (srv->wl_clip_source) {
            wl_data_source_destroy(srv->wl_clip_source);
            srv->wl_clip_source = NULL;
        }
        if (srv->data_mgr && srv->data_dev) {
            if (owner != 0 && owner != XM_WL_PROXY_SEL_XID) {
                srv->wl_clip_source = wl_data_device_manager_create_data_source(srv->data_mgr);
                wl_data_source_add_listener(srv->wl_clip_source, &g_clip_source_listener, srv);
                offer_standard_mimes_data_source(srv->wl_clip_source);
                wl_data_device_set_selection(srv->data_dev, srv->wl_clip_source, srv->last_input_serial);
            } else if (owner == 0) {
                wl_data_device_set_selection(srv->data_dev, NULL, srv->last_input_serial);
            }
            wl_display_flush(srv->wl_dpy);
        }
        emit_xfixes_selection_notify(srv, selection, owner, now_ms);
    } else if (selection == 1u /* XA_PRIMARY */) {
        uint32_t old = srv->sel_primary_owner;
        srv->sel_primary_owner = owner;
        if (old != owner)
            emit_selection_clear_to_owner(srv, old, selection, now_ms);

        if (srv->wl_prim_source) {
            zwp_primary_selection_source_v1_destroy(srv->wl_prim_source);
            srv->wl_prim_source = NULL;
        }
        if (srv->prim_mgr && srv->prim_dev) {
            if (owner != 0 && owner != XM_WL_PROXY_SEL_XID) {
                srv->wl_prim_source = zwp_primary_selection_device_manager_v1_create_source(srv->prim_mgr);
                zwp_primary_selection_source_v1_add_listener(srv->wl_prim_source, &g_prim_source_listener, srv);
                static const char *const pmimes[] = {
                    "text/plain;charset=utf-8", "text/plain", "UTF8_STRING", "STRING", "TEXT",
                    "text/uri-list", "image/png"
                };
                for (size_t i = 0; i < sizeof(pmimes) / sizeof(pmimes[0]); ++i)
                    zwp_primary_selection_source_v1_offer(srv->wl_prim_source, pmimes[i]);
                zwp_primary_selection_device_v1_set_selection(
                    srv->prim_dev, srv->wl_prim_source, srv->last_input_serial);
            } else if (owner == 0) {
                zwp_primary_selection_device_v1_set_selection(
                    srv->prim_dev, NULL, srv->last_input_serial);
            }
            wl_display_flush(srv->wl_dpy);
        }
        emit_xfixes_selection_notify(srv, selection, owner, now_ms);
    } else if (selection == a_xdnd) {
        uint32_t old = srv->sel_xdnd_owner;
        srv->sel_xdnd_owner = owner;
        if (old != owner)
            emit_selection_clear_to_owner(srv, old, selection, now_ms);

        if (owner != 0 && owner != XM_WL_PROXY_SEL_XID &&
            srv->data_mgr && srv->data_dev && srv->compositor &&
            srv->pointer_top && srv->pointer_top->wl_surf && srv->last_button_serial != 0) {
            if (srv->wl_drag_source) {
                wl_data_source_destroy(srv->wl_drag_source);
                srv->wl_drag_source = NULL;
            }
            if (srv->wl_drag_icon_surf) {
                wl_surface_destroy(srv->wl_drag_icon_surf);
                srv->wl_drag_icon_surf = NULL;
            }
            srv->wl_drag_source = wl_data_device_manager_create_data_source(srv->data_mgr);
            wl_data_source_add_listener(srv->wl_drag_source, &g_drag_source_listener, srv);

            XmWlWindow *owin = _XmWlFindWindow(srv, owner);
            uint32_t a_tlist = _XmWlInternAtom(srv, "XdndTypeList", 12, true);
            XmWlProperty *tp = (owin && a_tlist) ? _XmWlGetProperty(owin, a_tlist) : NULL;
            if (tp && tp->data && tp->byte_len >= 4) {
                size_t cnt = tp->byte_len / 4u;
                for (size_t k = 0; k < cnt; ++k) {
                    uint32_t a = rd_u32(tp->data + k * 4u);
                    const char *nm = _XmWlAtomName(srv, a);
                    if (nm && nm[0])
                        wl_data_source_offer(srv->wl_drag_source, nm);
                }
            }
            offer_standard_mimes_data_source(srv->wl_drag_source);
            if (wl_data_source_get_version(srv->wl_drag_source) >= 3) {
                wl_data_source_set_actions(
                    srv->wl_drag_source,
                    WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY |
                    WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE |
                    WL_DATA_DEVICE_MANAGER_DND_ACTION_ASK);
            }
            srv->wl_drag_icon_surf = wl_compositor_create_surface(srv->compositor);
            wl_data_device_start_drag(
                srv->data_dev, srv->wl_drag_source,
                srv->pointer_top->wl_surf, srv->wl_drag_icon_surf,
                srv->last_button_serial);
            wl_display_flush(srv->wl_dpy);
        }
    } else {
        for (size_t i = 0; i < 32; ++i) {
            if (g_misc_selections[i].atom == selection || g_misc_selections[i].atom == 0) {
                uint32_t old = g_misc_selections[i].owner;
                g_misc_selections[i].atom = selection;
                g_misc_selections[i].owner = owner;
                if (old != owner)
                    emit_selection_clear_to_owner(srv, old, selection, now_ms);
                emit_xfixes_selection_notify(srv, selection, owner, now_ms);
                break;
            }
        }
    }
}

uint32_t
_XmWlDataGetSelectionOwner(XmWlServer *srv, uint32_t selection)
{
    if (!srv) return 0;
    uint32_t a_clip = _XmWlInternAtom(srv, "CLIPBOARD", 9, false);
    uint32_t a_xdnd = _XmWlInternAtom(srv, "XdndSelection", 13, false);
    uint32_t a_cm = _XmWlInternAtom(srv, "_NET_WM_CM_S0", 13, false);
    uint32_t a_wm = _XmWlInternAtom(srv, "WM_S0", 5, false);
    if (selection == a_clip)
        return srv->sel_clipboard_owner;
    if (selection == 1u /* XA_PRIMARY */)
        return srv->sel_primary_owner;
    if (selection == a_xdnd)
        return srv->sel_xdnd_owner;
    if (selection == a_cm || selection == a_wm)
        return XM_WL_WM_HELPER_XID;
    for (size_t i = 0; i < 32; ++i) {
        if (g_misc_selections[i].atom == selection)
            return g_misc_selections[i].owner;
    }
    return 0;
}

static void
send_selection_notify(XmWlClient *cli, uint32_t requestor, uint32_t selection,
                      uint32_t target, uint32_t property, uint32_t time)
{
    if (!cli || !cli->active) return;
    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = 31; /* SelectionNotify */
    wr_u32(ev + 4, time);
    wr_u32(ev + 8, requestor);
    wr_u32(ev + 12, selection);
    wr_u32(ev + 16, target);
    wr_u32(ev + 20, property);
    _XmWlSendEventToClient(cli, ev);
}

void
_XmWlDataHandleConvertSelection(XmWlServer *srv, int client_idx, uint32_t requestor,
                                uint32_t selection, uint32_t target, uint32_t property, uint32_t time)
{
    if (!srv || client_idx < 0 || client_idx >= XM_WL_MAX_CLIENTS)
        return;
    XmWlClient *cli = &srv->clients[client_idx];
    uint32_t now_ms = time ? time : _XmWlNowMs();
    uint32_t prop_out = property ? property : target;

    uint32_t owner = _XmWlDataGetSelectionOwner(srv, selection);
    XmWlWindow *req_win = _XmWlFindWindow(srv, requestor);
    if (!req_win || owner == 0) {
        send_selection_notify(cli, requestor, selection, target, 0, now_ms);
        return;
    }

    /* Local X11 owner: standard in-process X11 SelectionRequest */
    if (owner != XM_WL_PROXY_SEL_XID) {
        uint8_t ev[32];
        memset(ev, 0, sizeof(ev));
        ev[0] = 30; /* SelectionRequest */
        wr_u32(ev + 4, now_ms);
        wr_u32(ev + 8, owner);
        wr_u32(ev + 12, requestor);
        wr_u32(ev + 16, selection);
        wr_u32(ev + 20, target);
        wr_u32(ev + 24, prop_out);
        for (int i = 0; i < XM_WL_MAX_CLIENTS; ++i) {
            if (srv->clients[i].active && srv->clients[i].handshake_done)
                _XmWlSendEventToClient(&srv->clients[i], ev);
        }
        return;
    }

    /* External Wayland offer */
    uint32_t a_clip = _XmWlInternAtom(srv, "CLIPBOARD", 9, false);
    uint32_t a_xdnd = _XmWlInternAtom(srv, "XdndSelection", 13, false);
    uint32_t a_targets = _XmWlInternAtom(srv, "TARGETS", 7, false);
    uint32_t a_timestamp = _XmWlInternAtom(srv, "TIMESTAMP", 9, false);

    char (*mimes)[96] = NULL;
    int mime_count = 0;
    struct wl_data_offer *data_offer = NULL;
    struct zwp_primary_selection_offer_v1 *prim_offer = NULL;

    if (selection == a_clip && srv->wl_clip_offer) {
        data_offer = srv->wl_clip_offer;
        mimes = srv->wl_clip_mimes;
        mime_count = srv->wl_clip_mime_count;
    } else if (selection == a_xdnd && srv->wl_dnd_offer) {
        data_offer = srv->wl_dnd_offer;
        mimes = srv->wl_dnd_mimes;
        mime_count = srv->wl_dnd_mime_count;
    } else if (selection == 1u /* XA_PRIMARY */ && srv->wl_prim_offer) {
        prim_offer = srv->wl_prim_offer;
        mimes = srv->wl_prim_mimes;
        mime_count = srv->wl_prim_mime_count;
    } else {
        send_selection_notify(cli, requestor, selection, target, 0, now_ms);
        return;
    }

    if (target == a_targets) {
        uint32_t atoms[24];
        size_t n = 0;
        atoms[n++] = a_targets;
        atoms[n++] = a_timestamp;
        bool has_text = false;
        for (int i = 0; i < mime_count && n < 20; ++i) {
            atoms[n++] = _XmWlInternAtom(srv, mimes[i], strlen(mimes[i]), false);
            if (is_text_target_name(mimes[i]))
                has_text = true;
        }
        if (has_text) {
            atoms[n++] = _XmWlInternAtom(srv, "UTF8_STRING", 11, false);
            atoms[n++] = 31u; /* XA_STRING */
            atoms[n++] = _XmWlInternAtom(srv, "TEXT", 4, false);
        }
        _XmWlSetProperty(srv, req_win, prop_out, 4u /* XA_ATOM */, 32, 0,
                         (const uint8_t *)atoms, n * 4u, true);
        send_selection_notify(cli, requestor, selection, target, prop_out, now_ms);
        return;
    }

    if (target == a_timestamp) {
        _XmWlSetProperty(srv, req_win, prop_out, 19u /* XA_INTEGER */, 32, 0,
                         (const uint8_t *)&now_ms, 4u, true);
        send_selection_notify(cli, requestor, selection, target, prop_out, now_ms);
        return;
    }

    uint32_t prop_type = target;
    const char *chosen_mime = match_offer_mime_for_target(srv, target, mimes, mime_count, &prop_type);
    if (!chosen_mime) {
        send_selection_notify(cli, requestor, selection, target, 0, now_ms);
        return;
    }

    int pfd[2] = {-1, -1};
    if (pipe2(pfd, O_CLOEXEC) != 0) {
        send_selection_notify(cli, requestor, selection, target, 0, now_ms);
        return;
    }

    if (data_offer)
        wl_data_offer_receive(data_offer, chosen_mime, pfd[1]);
    else
        zwp_primary_selection_offer_v1_receive(prim_offer, chosen_mime, pfd[1]);
    wl_display_flush(srv->wl_dpy);
    close(pfd[1]);

    size_t rx_len = 0;
    uint8_t *rx_bytes = read_all_offer_fd(srv, pfd[0], &rx_len);
    close(pfd[0]);

    if (!rx_bytes) {
        send_selection_notify(cli, requestor, selection, target, 0, now_ms);
        return;
    }

    _XmWlSetProperty(srv, req_win, prop_out, prop_type, 8, 0, rx_bytes, rx_len, true);
    free(rx_bytes);
    send_selection_notify(cli, requestor, selection, target, prop_out, now_ms);
}

bool
_XmWlDataInterceptSendEvent(XmWlServer *srv, uint32_t dest_win, const uint8_t ev[32])
{
    if (!srv || dest_win != XM_WL_PROXY_SEL_XID)
        return false;

    uint8_t code = ev[0] & 0x7fu;

    /* SelectionNotify completing a pending Wayland send(mime, fd) transfer */
    if (code == 31) {
        uint32_t sel_atom = rd_u32(ev + 12);
        uint32_t prop_atom = rd_u32(ev + 20);

        for (int i = 0; i < 8; ++i) {
            XmWlPendingSelectionTransfer *tr = &srv->pending_transfers[i];
            if (!tr->active || tr->selection_atom != sel_atom)
                continue;

            /* Retry with XA_STRING if UTF8_STRING was refused by a legacy widget */
            uint32_t a_utf8 = _XmWlInternAtom(srv, "UTF8_STRING", 11, true);
            if (prop_atom == 0 && tr->target_atom == a_utf8) {
                uint32_t owner = _XmWlDataGetSelectionOwner(srv, sel_atom);
                if (owner != 0 && owner != XM_WL_PROXY_SEL_XID) {
                    tr->target_atom = 31u; /* XA_STRING */
                    uint8_t req_ev[32];
                    memset(req_ev, 0, sizeof(req_ev));
                    req_ev[0] = 30; /* SelectionRequest */
                    wr_u32(req_ev + 4, tr->time);
                    wr_u32(req_ev + 8, owner);
                    wr_u32(req_ev + 12, XM_WL_PROXY_SEL_XID);
                    wr_u32(req_ev + 16, sel_atom);
                    wr_u32(req_ev + 20, 31u);
                    wr_u32(req_ev + 24, tr->property_atom);
                    for (int c = 0; c < XM_WL_MAX_CLIENTS; ++c) {
                        if (srv->clients[c].active && srv->clients[c].handshake_done)
                            _XmWlSendEventToClient(&srv->clients[c], req_ev);
                    }
                    return true;
                }
            }

            if (prop_atom != 0 && srv->proxy_sel) {
                XmWlProperty *p = _XmWlGetProperty(srv->proxy_sel, prop_atom);
                if (p && p->data && p->byte_len > 0 && tr->write_fd >= 0) {
                    write_all_fd(tr->write_fd, p->data, p->byte_len);
                }
                _XmWlDeleteProperty(srv, srv->proxy_sel, prop_atom, false);
            }
            if (tr->write_fd >= 0) {
                close(tr->write_fd);
                tr->write_fd = -1;
            }
            tr->active = false;
            return true;
        }
        return true;
    }

    /* XDND XdndStatus / XdndFinished replies from a drop site */
    if (code == 33) {
        uint32_t msg_type = rd_u32(ev + 8);
        uint32_t a_status = _XmWlInternAtom(srv, "XdndStatus", 10, true);
        uint32_t a_finished = _XmWlInternAtom(srv, "XdndFinished", 12, true);
        uint32_t a_move = _XmWlInternAtom(srv, "XdndActionMove", 14, true);

        if (a_status && msg_type == a_status && srv->wl_dnd_offer) {
            uint32_t flags = rd_u32(ev + 16);
            uint32_t act_atom = rd_u32(ev + 28);
            bool accepted = (flags & 1u) != 0;
            wl_data_offer_accept(
                srv->wl_dnd_offer, srv->wl_dnd_serial,
                (accepted && srv->wl_dnd_mime_count > 0) ? srv->wl_dnd_mimes[0] : NULL);
            if (wl_data_offer_get_version(srv->wl_dnd_offer) >= 3) {
                uint32_t wl_act = accepted
                    ? ((a_move && act_atom == a_move)
                           ? WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE
                           : WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY)
                    : WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE;
                wl_data_offer_set_actions(
                    srv->wl_dnd_offer,
                    WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY | WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE,
                    wl_act);
            }
            wl_display_flush(srv->wl_dpy);
            return true;
        }

        if (a_finished && msg_type == a_finished) {
            uint32_t flags = rd_u32(ev + 16);
            bool accepted = (flags & 1u) != 0;
            if (srv->wl_dnd_offer) {
                if (accepted && wl_data_offer_get_version(srv->wl_dnd_offer) >= 3)
                    wl_data_offer_finish(srv->wl_dnd_offer);
                wl_data_offer_destroy(srv->wl_dnd_offer);
                srv->wl_dnd_offer = NULL;
                wl_display_flush(srv->wl_dpy);
            }
            srv->wl_dnd_drop_pending = false;
            return true;
        }
    }

    return true;
}

void
_XmWlDataAttachDragIcon(XmWlServer *srv, XmWlWindow *drag_win)
{
    if (!srv || !drag_win) return;
    if (srv->wl_drag_icon_surf) {
        if (drag_win->frac_scale_obj) {
            wp_fractional_scale_v1_destroy(drag_win->frac_scale_obj);
            drag_win->frac_scale_obj = NULL;
        }
        if (drag_win->viewport) {
            wp_viewport_destroy(drag_win->viewport);
            drag_win->viewport = NULL;
        }
        if (drag_win->wl_surf) {
            wl_surface_destroy(drag_win->wl_surf);
        }
        drag_win->wl_surf = srv->wl_drag_icon_surf;
        srv->wl_drag_icon_surf = NULL;
        if (srv->viewporter)
            drag_win->viewport = wp_viewporter_get_viewport(srv->viewporter, drag_win->wl_surf);
    }
}

#endif /* HAVE_WAYLAND_BACKEND */
