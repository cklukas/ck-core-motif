/*
 * Native Wayland zwp_text_input_v3 input method integration for XmText and
 * XmTextField on the libXm Wayland backend.
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

#include <X11/IntrinsicP.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <stdlib.h>
#include <string.h>

static void
text_input_enter(void *data, struct zwp_text_input_v3 *zwp_text_input_v3,
                 struct wl_surface *surface)
{
    (void)zwp_text_input_v3;
    (void)surface;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;
    _XmWlImeSyncFocus(srv);
}

static void
text_input_leave(void *data, struct zwp_text_input_v3 *zwp_text_input_v3,
                 struct wl_surface *surface)
{
    (void)zwp_text_input_v3;
    (void)surface;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;
    srv->ime_enabled = false;
}

static void
text_input_preedit_string(void *data, struct zwp_text_input_v3 *zwp_text_input_v3,
                          const char *text, int32_t cursor_begin, int32_t cursor_end)
{
    (void)zwp_text_input_v3;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;
    if (text) {
        size_t n = strlen(text);
        if (n >= sizeof(srv->ime_pending_preedit))
            n = sizeof(srv->ime_pending_preedit) - 1;
        memcpy(srv->ime_pending_preedit, text, n);
        srv->ime_pending_preedit[n] = '\0';
    } else {
        srv->ime_pending_preedit[0] = '\0';
    }
    srv->ime_pending_cursor_begin = cursor_begin;
    srv->ime_pending_cursor_end = cursor_end;
    srv->ime_has_pending_preedit = true;
}

static void
text_input_commit_string(void *data, struct zwp_text_input_v3 *zwp_text_input_v3,
                         const char *text)
{
    (void)zwp_text_input_v3;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;
    if (text) {
        size_t n = strlen(text);
        if (n >= sizeof(srv->ime_pending_commit))
            n = sizeof(srv->ime_pending_commit) - 1;
        memcpy(srv->ime_pending_commit, text, n);
        srv->ime_pending_commit[n] = '\0';
    } else {
        srv->ime_pending_commit[0] = '\0';
    }
    srv->ime_has_pending_commit = true;
}

static void
text_input_delete_surrounding_text(void *data, struct zwp_text_input_v3 *zwp_text_input_v3,
                                   uint32_t before_length, uint32_t after_length)
{
    (void)zwp_text_input_v3;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;
    srv->ime_pending_del_before = before_length;
    srv->ime_pending_del_after = after_length;
    srv->ime_has_pending_delete = true;
}

static void
text_input_done(void *data, struct zwp_text_input_v3 *zwp_text_input_v3,
                uint32_t serial)
{
    (void)zwp_text_input_v3;
    (void)serial;
    XmWlServer *srv = (XmWlServer *)data;
    if (!srv) return;

    bool notify = false;
    if (srv->ime_has_pending_delete) {
        srv->xt_ime_del_before = srv->ime_pending_del_before;
        srv->xt_ime_del_after = srv->ime_pending_del_after;
        srv->xt_has_ime_delete = true;
        srv->ime_has_pending_delete = false;
        notify = true;
    }
    if (srv->ime_has_pending_commit) {
        memcpy(srv->xt_ime_commit, srv->ime_pending_commit, sizeof(srv->xt_ime_commit));
        srv->xt_has_ime_commit = true;
        srv->ime_has_pending_commit = false;
        /* Per zwp_text_input_v3 spec, an omitted preedit_string in done clears preedit */
        if (!srv->ime_has_pending_preedit) {
            srv->xt_ime_preedit[0] = '\0';
            srv->xt_has_ime_preedit = true;
        }
        notify = true;
    }
    if (srv->ime_has_pending_preedit) {
        memcpy(srv->xt_ime_preedit, srv->ime_pending_preedit, sizeof(srv->xt_ime_preedit));
        srv->xt_has_ime_preedit = true;
        srv->ime_has_pending_preedit = false;
        notify = true;
    } else if (!notify && srv->ime_active_preedit_chars > 0) {
        srv->xt_ime_preedit[0] = '\0';
        srv->xt_has_ime_preedit = true;
        notify = true;
    }

    if (notify)
        _XmWaylandNotifyMainThread(srv);
}

static const struct zwp_text_input_v3_listener g_text_input_listener = {
    .enter = text_input_enter,
    .leave = text_input_leave,
    .preedit_string = text_input_preedit_string,
    .commit_string = text_input_commit_string,
    .delete_surrounding_text = text_input_delete_surrounding_text,
    .done = text_input_done,
};

void
_XmWlImeBindSeat(XmWlServer *srv)
{
    if (!srv || !srv->seat || !srv->text_input_mgr || srv->text_input)
        return;
    srv->text_input = zwp_text_input_manager_v3_get_text_input(srv->text_input_mgr, srv->seat);
    if (srv->text_input)
        zwp_text_input_v3_add_listener(srv->text_input, &g_text_input_listener, srv);
}

void
_XmWlImeSyncFocus(XmWlServer *srv)
{
    if (!srv || !srv->text_input)
        return;

    if (srv->ime_focused_widget && srv->wl_focus_top) {
        if (!srv->ime_enabled) {
            zwp_text_input_v3_enable(srv->text_input);
            srv->ime_enabled = true;
        }
        zwp_text_input_v3_set_content_type(
            srv->text_input,
            ZWP_TEXT_INPUT_V3_CONTENT_HINT_NONE,
            ZWP_TEXT_INPUT_V3_CONTENT_PURPOSE_NORMAL);
        int32_t sx = srv->ime_spot_x >= 0 ? srv->ime_spot_x : 0;
        int32_t sy = srv->ime_spot_y >= 0 ? srv->ime_spot_y : 0;
        int32_t sw = srv->ime_spot_w > 0 ? srv->ime_spot_w : 2;
        int32_t sh = srv->ime_spot_h > 0 ? srv->ime_spot_h : 18;
        zwp_text_input_v3_set_cursor_rectangle(srv->text_input, sx, sy, sw, sh);
        zwp_text_input_v3_commit(srv->text_input);
        if (srv->wl_dpy)
            wl_display_flush(srv->wl_dpy);
    } else if (srv->ime_enabled) {
        zwp_text_input_v3_disable(srv->text_input);
        zwp_text_input_v3_commit(srv->text_input);
        srv->ime_enabled = false;
        if (srv->wl_dpy)
            wl_display_flush(srv->wl_dpy);
    }
}

/* ---------- Widget Text Helpers (Main Xt Thread Only) ---------- */

static XmTextPosition
widget_get_cursor(Widget w)
{
    if (XmIsText(w))
        return XmTextGetInsertionPosition(w);
    if (XmIsTextField(w))
        return XmTextFieldGetInsertionPosition(w);
    return 0;
}

static void
widget_set_cursor(Widget w, XmTextPosition pos)
{
    if (XmIsText(w))
        XmTextSetInsertionPosition(w, pos);
    else if (XmIsTextField(w))
        XmTextFieldSetInsertionPosition(w, pos);
}

static XmTextPosition
widget_get_last_pos(Widget w)
{
    if (XmIsText(w))
        return XmTextGetLastPosition(w);
    if (XmIsTextField(w))
        return XmTextFieldGetLastPosition(w);
    return 0;
}

static void
widget_replace(Widget w, XmTextPosition from_pos, XmTextPosition to_pos, char *str)
{
    if (XmIsText(w))
        XmTextReplace(w, from_pos, to_pos, str);
    else if (XmIsTextField(w))
        XmTextFieldReplace(w, from_pos, to_pos, str);
}

static void
widget_set_highlight(Widget w, XmTextPosition left, XmTextPosition right, XmHighlightMode mode)
{
    if (XmIsText(w))
        XmTextSetHighlight(w, left, right, mode);
    else if (XmIsTextField(w))
        XmTextFieldSetHighlight(w, left, right, mode);
}

static char *
widget_get_string(Widget w)
{
    if (XmIsText(w))
        return XmTextGetString(w);
    if (XmIsTextField(w))
        return XmTextFieldGetString(w);
    return NULL;
}

static XmTextPosition
count_widget_positions_for_bytes(const char *s, size_t max_bytes, bool char_positions)
{
    if (!s || max_bytes == 0)
        return 0;
    if (!char_positions)
        return (XmTextPosition)max_bytes;

    XmTextPosition count = 0;
    size_t i = 0;
    while (i < max_bytes && s[i] != '\0') {
        unsigned char c = (unsigned char)s[i];
        size_t step = 1;
        if ((c & 0x80u) == 0) step = 1;
        else if ((c & 0xe0u) == 0xc0u) step = 2;
        else if ((c & 0xf0u) == 0xe0u) step = 3;
        else if ((c & 0xf8u) == 0xf0u) step = 4;
        if (i + step > max_bytes)
            break;
        i += step;
        count++;
    }
    return count;
}

static size_t
byte_offset_for_widget_pos(const char *s, XmTextPosition pos, bool char_positions)
{
    if (!s || pos <= 0)
        return 0;
    size_t slen = strlen(s);
    if (!char_positions)
        return (size_t)pos <= slen ? (size_t)pos : slen;

    size_t off = 0;
    XmTextPosition cur = 0;
    while (off < slen && cur < pos) {
        unsigned char c = (unsigned char)s[off];
        size_t step = 1;
        if ((c & 0x80u) == 0) step = 1;
        else if ((c & 0xe0u) == 0xc0u) step = 2;
        else if ((c & 0xf0u) == 0xe0u) step = 3;
        else if ((c & 0xf8u) == 0xf0u) step = 4;
        if (off + step > slen)
            step = 1;
        off += step;
        cur++;
    }
    return off;
}

void
_XmWlImeDrainMainThread(XmWlServer *srv)
{
    if (!srv) return;

    Widget w = NULL;
    bool has_del = false, has_commit = false, has_preedit = false;
    uint32_t del_before = 0, del_after = 0;
    char commit_buf[512];
    char preedit_buf[512];

    pthread_mutex_lock(&srv->lock);
    w = srv->ime_focused_widget;
    if (w && !w->core.being_destroyed && (XmIsText(w) || XmIsTextField(w))) {
        if (srv->xt_has_ime_delete) {
            has_del = true;
            del_before = srv->xt_ime_del_before;
            del_after = srv->xt_ime_del_after;
            srv->xt_has_ime_delete = false;
        }
        if (srv->xt_has_ime_commit) {
            has_commit = true;
            memcpy(commit_buf, srv->xt_ime_commit, sizeof(commit_buf));
            srv->xt_has_ime_commit = false;
        }
        if (srv->xt_has_ime_preedit) {
            has_preedit = true;
            memcpy(preedit_buf, srv->xt_ime_preedit, sizeof(preedit_buf));
            srv->xt_has_ime_preedit = false;
        }
    } else {
        srv->xt_has_ime_delete = false;
        srv->xt_has_ime_commit = false;
        srv->xt_has_ime_preedit = false;
        w = NULL;
    }
    pthread_mutex_unlock(&srv->lock);

    if (!w || (!has_del && !has_commit && !has_preedit))
        return;

    /* Clear any existing active preedit span before applying delete/commit/new preedit */
    if (srv->ime_active_preedit_chars > 0) {
        XmTextPosition p0 = (XmTextPosition)srv->ime_preedit_pos;
        XmTextPosition p1 = p0 + (XmTextPosition)srv->ime_active_preedit_chars;
        XmTextPosition last = widget_get_last_pos(w);
        if (p0 < 0) p0 = 0;
        if (p1 > last) p1 = last;
        if (p1 > p0) {
            widget_set_highlight(w, p0, p1, XmHIGHLIGHT_NORMAL);
            widget_replace(w, p0, p1, (char *)"");
            widget_set_cursor(w, p0);
        }
        srv->ime_active_preedit_chars = 0;
    }

    if (has_del && (del_before > 0 || del_after > 0)) {
        char *cur_str = widget_get_string(w);
        if (cur_str) {
            size_t total_bytes = strlen(cur_str);
            XmTextPosition total_pos = widget_get_last_pos(w);
            bool char_pos = (total_bytes != (size_t)total_pos) || (MB_CUR_MAX > 1);
            XmTextPosition cpos = widget_get_cursor(w);
            size_t cbyte = byte_offset_for_widget_pos(cur_str, cpos, char_pos);
            size_t b_start = del_before <= cbyte ? (cbyte - del_before) : 0;
            size_t b_end = (cbyte + del_after <= total_bytes) ? (cbyte + del_after) : total_bytes;
            XmTextPosition d0 = count_widget_positions_for_bytes(cur_str, b_start, char_pos);
            XmTextPosition d1 = count_widget_positions_for_bytes(cur_str, b_end, char_pos);
            if (d1 > d0) {
                widget_replace(w, d0, d1, (char *)"");
                widget_set_cursor(w, d0);
            }
            XtFree(cur_str);
        }
    }

    if (has_commit && commit_buf[0] != '\0') {
        XmTextPosition cpos = widget_get_cursor(w);
        XmTextPosition before_last = widget_get_last_pos(w);
        widget_replace(w, cpos, cpos, commit_buf);
        XmTextPosition after_last = widget_get_last_pos(w);
        XmTextPosition delta = after_last - before_last;
        if (delta > 0)
            widget_set_cursor(w, cpos + delta);
    }

    if (has_preedit && preedit_buf[0] != '\0') {
        XmTextPosition cpos = widget_get_cursor(w);
        XmTextPosition before_last = widget_get_last_pos(w);
        widget_replace(w, cpos, cpos, preedit_buf);
        XmTextPosition after_last = widget_get_last_pos(w);
        XmTextPosition delta = after_last - before_last;
        if (delta > 0) {
            srv->ime_preedit_pos = (long)cpos;
            srv->ime_active_preedit_chars = (int)delta;
            widget_set_highlight(w, cpos, cpos + delta, XmHIGHLIGHT_SECONDARY_SELECTED);
            widget_set_cursor(w, cpos + delta);
        }
    }
}

static void
compute_widget_spot_in_toplevel(XmWlServer *srv, Widget w, ArgList args, Cardinal num_args)
{
    int spot_x = 4;
    int spot_y = (int)w->core.height > 4 ? (int)w->core.height - 4 : 16;
    for (Cardinal i = 0; i < num_args; ++i) {
        if (args[i].name && !strcmp(args[i].name, XmNspotLocation) && args[i].value) {
            const XPoint *pt = (const XPoint *)args[i].value;
            spot_x = pt->x;
            spot_y = pt->y;
            break;
        }
    }

    /* Walk widget parents up to shell to convert widget-local spot to top-level coordinates */
    int wx = spot_x;
    int wy = spot_y;
    for (Widget cur = w; cur && !XtIsShell(cur); cur = XtParent(cur)) {
        wx += cur->core.x;
        wy += cur->core.y;
    }
    srv->ime_spot_x = wx;
    srv->ime_spot_y = wy > 16 ? wy - 16 : 0;
    srv->ime_spot_w = 2;
    srv->ime_spot_h = 18;
}

void
_XmWaylandImSetFocus(Widget w, ArgList args, Cardinal num_args)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !w || !(XmIsText(w) || XmIsTextField(w)))
        return;

    pthread_mutex_lock(&srv->lock);
    if (srv->ime_focused_widget != w) {
        srv->ime_focused_widget = w;
        srv->ime_active_preedit_chars = 0;
    }
    compute_widget_spot_in_toplevel(srv, w, args, num_args);
    _XmWlImeSyncFocus(srv);
    pthread_mutex_unlock(&srv->lock);
}

void
_XmWaylandImUnsetFocus(Widget w)
{
    XmWlServer *srv = _XmWaylandGetServer();
    if (!srv || !w)
        return;

    pthread_mutex_lock(&srv->lock);
    if (srv->ime_focused_widget == w) {
        srv->ime_focused_widget = NULL;
        srv->ime_active_preedit_chars = 0;
        _XmWlImeSyncFocus(srv);
    }
    pthread_mutex_unlock(&srv->lock);
}

void
_XmWaylandImUnregister(Widget w)
{
    _XmWaylandImUnsetFocus(w);
}

#endif /* HAVE_WAYLAND_BACKEND */
