#!/usr/bin/env python3
"""Run automated qualification for the XWayland-free libXm Wayland runtime backend."""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import textwrap
import time


MINI_COMPOSITOR_C = textwrap.dedent(
    r"""
    #define _GNU_SOURCE
    #include <errno.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <stdbool.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/mman.h>
    #include <time.h>
    #include <unistd.h>
    #include <wayland-server.h>
    #include "xdg-shell-server.h"
    #include "xdg-activation-server.h"
    #include "text-input-v3-server.h"
    #include "primary-selection-server.h"
    #include "cursor-shape-server.h"
    #include "linux-dmabuf-v1-server.h"
    #include "xdg-dialog-v1-server.h"
    #include "tearing-control-v1-server.h"
    #include "fifo-v1-server.h"

    typedef struct {
        struct wl_resource *res;
        char mimes[16][96];
        int mime_count;
    } DataSrc;

    typedef struct {
        struct wl_display *dpy;
        const char *sync_dir;
        struct wl_resource *primary_surf;
        struct wl_resource *primary_xdg_surf;
        struct wl_resource *primary_xdg_top;
        struct wl_resource *primary_ptr;
        struct wl_resource *primary_kbd;
        struct wl_resource *primary_touch;
        struct wl_resource *primary_data_dev;
        struct wl_resource *text_in_res;

        struct wl_resource *data_devs[8];
        int data_dev_count;
        struct wl_resource *prim_devs[8];
        int prim_dev_count;

        DataSrc *cur_clip_src;
        DataSrc *cur_prim_src;

        bool ime_enabled_seen;
        bool ime_cursor_rect_seen;
        bool dnd_finished_seen;
        uint32_t act_token_seq;
        uint32_t last_tok_serial;
        bool last_tok_had_surface;
        char last_tok_app_id[128];
        int activate_calls;
        char first_activated_token[128];
        char last_activated_token[128];
        int shape_calls;
        bool seen_shape_default;
        bool seen_shape_text;
        bool seen_shape_pointer;
        uint32_t last_cursor_shape;
        int custom_cursor_calls;
        struct wl_resource *out1_res[8];
        int out1_count;
        struct wl_resource *out2_res[8];
        int out2_count;
        int dmabuf_imports;
        int32_t dmabuf_w;
        int32_t dmabuf_h;
        uint32_t dmabuf_stride;
        uint32_t dmabuf_fmt;
        uint32_t dmabuf_px0;
        int subsurf_created;
        int top_parent_calls;
        int dialog_created;
        int modal_set_calls;
        int modal_unset_calls;
        int dialog_destroyed_before_top;
        int tearing_objs;
        int tearing_async_calls;
        int tearing_vsync_calls;
        int tearing_destroyed;
        int fifo_objs;
        int fifo_set_calls;
        int fifo_wait_calls;
        int fifo_destroyed;
        int phase;
    } MiniSrv;

    static MiniSrv *g_ms = NULL;

    static void noop_destroy(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        wl_resource_destroy(r);
    }

    static bool file_exists(const char *dir, const char *name) {
        char p[512];
        snprintf(p, sizeof(p), "%s/%s", dir, name);
        return access(p, F_OK) == 0;
    }

    static void write_flag(const char *dir, const char *name) {
        char p[512];
        snprintf(p, sizeof(p), "%s/%s", dir, name);
        FILE *f = fopen(p, "w");
        if (f) { fprintf(f, "1\n"); fclose(f); }
    }

    /* Forward declaration for selection broadcast */
    static void broadcast_clipboard_to_devices(MiniSrv *ms);
    static void broadcast_primary_to_devices(MiniSrv *ms);

    /* wl_surface & wl_compositor */
    typedef struct {
        struct wl_resource *buf;
        struct wl_listener destroy;
    } HeldBuf;

    typedef struct {
        MiniSrv *ms;
        struct wl_resource *xdg_surf;
        struct wl_resource *xdg_top;
        struct wl_resource *xdg_pop;
        int32_t pop_w;
        int32_t pop_h;
        bool configured;
        HeldBuf held[3];
        int held_idx;
        int last_held_slot;
        int pending_dmg_rects;
        int64_t pending_dmg_area;
        int dmg_ring_commits;
        bool has_dialog;
        bool has_tearing;
        bool has_fifo;
    } SurfState;

    typedef struct {
        int32_t w;
        int32_t h;
        int32_t ax;
        int32_t ay;
    } PosState;

    static void on_held_buf_destroy(struct wl_listener *l, void *data) {
        (void)data;
        HeldBuf *hb = wl_container_of(l, hb, destroy);
        wl_list_remove(&hb->destroy.link);
        wl_list_init(&hb->destroy.link);
        hb->buf = NULL;
    }

    static void release_held_slot(HeldBuf *hb, bool send_rel) {
        if (hb->buf) {
            if (send_rel)
                wl_buffer_send_release(hb->buf);
            wl_list_remove(&hb->destroy.link);
            wl_list_init(&hb->destroy.link);
            hb->buf = NULL;
        }
    }

    static void free_pos_state(struct wl_resource *r) {
        free(wl_resource_get_user_data(r));
    }

    static void free_surf_state(struct wl_resource *r) {
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss) {
            for (int i = 0; i < 3; ++i)
                release_held_slot(&ss->held[i], false);
            free(ss);
        }
    }

    static void surf_attach(struct wl_client *c, struct wl_resource *r, struct wl_resource *b, int32_t x, int32_t y) {
        (void)c; (void)x; (void)y;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && ss->ms && r == ss->ms->primary_surf &&
            file_exists(ss->ms->sync_dir, "dmg_ring.active")) {
            int slot = ss->held_idx % 3;
            ss->held_idx++;
            release_held_slot(&ss->held[slot], true);
            if (b) {
                ss->held[slot].buf = b;
                wl_resource_add_destroy_listener(b, &ss->held[slot].destroy);
            }
            ss->last_held_slot = slot;
            return;
        }
        if (ss) {
            for (int i = 0; i < 3; ++i)
                release_held_slot(&ss->held[i], true);
            ss->last_held_slot = -1;
        }
        if (b) wl_buffer_send_release(b);
    }
    static void surf_damage(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h) {
        (void)c; (void)x; (void)y;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && w > 0 && h > 0) {
            ss->pending_dmg_rects++;
            ss->pending_dmg_area += (int64_t)w * (int64_t)h;
        }
    }
    static void surf_frame(struct wl_client *c, struct wl_resource *r, uint32_t cb_id) {
        (void)r;
        struct wl_resource *cb = wl_resource_create(c, &wl_callback_interface, 1, cb_id);
        wl_callback_send_done(cb, 100);
        wl_resource_destroy(cb);
    }
    static void surf_region(struct wl_client *c, struct wl_resource *r, struct wl_resource *reg) {
        (void)c; (void)r; (void)reg;
    }
    static uint32_t sample_shm_px(struct wl_shm_buffer *shmb, int x, int y) {
        if (!shmb) return 0;
        int w = wl_shm_buffer_get_width(shmb);
        int h = wl_shm_buffer_get_height(shmb);
        int stride = wl_shm_buffer_get_stride(shmb);
        if (x < 0 || y < 0 || x >= w || y >= h || stride <= 0) return 0;
        const uint8_t *base = (const uint8_t *)wl_shm_buffer_get_data(shmb);
        if (!base) return 0;
        const uint32_t *row = (const uint32_t *)(base + (size_t)y * (size_t)stride);
        return row[x] & 0x00ffffffu;
    }
    static void surf_commit(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (!ss || !ss->ms) return;
        MiniSrv *ms = ss->ms;
        if (r == ms->primary_surf && file_exists(ms->sync_dir, "dmg_ring.active") &&
            ss->last_held_slot >= 0 && ss->held[ss->last_held_slot].buf) {
            struct wl_shm_buffer *shmb = wl_shm_buffer_get(ss->held[ss->last_held_slot].buf);
            uint32_t p00 = 0, p10 = 0, p01 = 0, p11 = 0;
            if (shmb) {
                wl_shm_buffer_begin_access(shmb);
                p00 = sample_shm_px(shmb, 6, 6);
                p10 = sample_shm_px(shmb, 302, 6);
                p01 = sample_shm_px(shmb, 6, 182);
                p11 = sample_shm_px(shmb, 302, 182);
                wl_shm_buffer_end_access(shmb);
            }
            ss->dmg_ring_commits++;
            char tmp_p[512], dst_p[512];
            snprintf(tmp_p, sizeof(tmp_p), "%s/dmg_state.txt.tmp", ms->sync_dir);
            snprintf(dst_p, sizeof(dst_p), "%s/dmg_state.txt", ms->sync_dir);
            FILE *df = fopen(tmp_p, "w");
            if (df) {
                fprintf(df, "commits=%d\nrects=%d\narea=%lld\np00=0x%06x\np10=0x%06x\np01=0x%06x\np11=0x%06x\n",
                        ss->dmg_ring_commits, ss->pending_dmg_rects,
                        (long long)ss->pending_dmg_area, p00, p10, p01, p11);
                fclose(df);
                rename(tmp_p, dst_p);
            }
        }
        ss->pending_dmg_rects = 0;
        ss->pending_dmg_area = 0;
        if (ss->xdg_surf && !ss->configured) {
            ss->configured = true;
            if (ss->xdg_top) {
                struct wl_array states;
                wl_array_init(&states);
                int32_t *st = wl_array_add(&states, sizeof(int32_t));
                *st = XDG_TOPLEVEL_STATE_ACTIVATED;
                xdg_toplevel_send_configure(ss->xdg_top, 360, 220, &states);
                wl_array_release(&states);
            } else if (ss->xdg_pop) {
                xdg_popup_send_configure(
                    ss->xdg_pop, 0, 0,
                    ss->pop_w > 0 ? ss->pop_w : 120,
                    ss->pop_h > 0 ? ss->pop_h : 60);
            }
            xdg_surface_send_configure(ss->xdg_surf, 1);

            if (r == ms->primary_surf) {
                if (ms->primary_kbd) {
                    struct wl_array keys;
                    wl_array_init(&keys);
                    wl_keyboard_send_enter(ms->primary_kbd, 2, r, &keys);
                    wl_array_release(&keys);
                }
                if (ms->text_in_res)
                    zwp_text_input_v3_send_enter(ms->text_in_res, r);
            }
        }
    }
    static void surf_int(struct wl_client *c, struct wl_resource *r, int32_t v) { (void)c; (void)r; (void)v; }
    static void surf_offset(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y) { (void)c; (void)r; (void)x; (void)y; }

    static const struct wl_surface_interface g_surf_impl = {
        .destroy = noop_destroy, .attach = surf_attach, .damage = surf_damage, .frame = surf_frame,
        .set_opaque_region = surf_region, .set_input_region = surf_region, .commit = surf_commit,
        .set_buffer_transform = surf_int, .set_buffer_scale = surf_int, .damage_buffer = surf_damage, .offset = surf_offset,
    };

    static void reg_op(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h) {
        (void)c; (void)r; (void)x; (void)y; (void)w; (void)h;
    }
    static const struct wl_region_interface g_region_impl = { .destroy = noop_destroy, .add = reg_op, .subtract = reg_op };

    static void comp_create_surface(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        MiniSrv *ms = wl_resource_get_user_data(r);
        SurfState *ss = calloc(1, sizeof(*ss));
        ss->ms = ms;
        ss->last_held_slot = -1;
        for (int i = 0; i < 3; ++i) {
            wl_list_init(&ss->held[i].destroy.link);
            ss->held[i].destroy.notify = on_held_buf_destroy;
        }
        struct wl_resource *sr = wl_resource_create(c, &wl_surface_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(sr, &g_surf_impl, ss, free_surf_state);
        if (!ms->primary_surf) ms->primary_surf = sr;
    }
    static void comp_create_region(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        struct wl_resource *rr = wl_resource_create(c, &wl_region_interface, 1, id);
        wl_resource_set_implementation(rr, &g_region_impl, wl_resource_get_user_data(r), NULL);
    }
    static const struct wl_compositor_interface g_comp_impl = { .create_surface = comp_create_surface, .create_region = comp_create_region };
    static void bind_comp(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        struct wl_resource *r = wl_resource_create(c, &wl_compositor_interface, ver < 4 ? ver : 4, id);
        wl_resource_set_implementation(r, &g_comp_impl, d, NULL);
    }

    /* xdg_wm_base */
    static void write_proto_ext_state(MiniSrv *ms) {
        if (!ms || !ms->sync_dir) return;
        char tmp_p[512], dst_p[512];
        snprintf(tmp_p, sizeof(tmp_p), "%s/proto_ext_state.txt.tmp", ms->sync_dir);
        snprintf(dst_p, sizeof(dst_p), "%s/proto_ext_state.txt", ms->sync_dir);
        FILE *f = fopen(tmp_p, "w");
        if (f) {
            fprintf(f,
                    "dmabuf_imports=%d\ndmabuf_w=%d\ndmabuf_h=%d\ndmabuf_stride=%u\n"
                    "dmabuf_fmt=0x%08x\ndmabuf_px0=0x%08x\nsubsurf_created=%d\n"
                    "top_parent_calls=%d\ndialog_created=%d\nmodal_set=%d\nmodal_unset=%d\n"
                    "dialog_destroyed_before_top=%d\n"
                    "tearing_objs=%d\ntearing_async=%d\ntearing_vsync=%d\ntearing_destroyed=%d\n"
                    "fifo_objs=%d\nfifo_set=%d\nfifo_wait=%d\nfifo_destroyed=%d\n",
                    ms->dmabuf_imports, (int)ms->dmabuf_w, (int)ms->dmabuf_h, ms->dmabuf_stride,
                    ms->dmabuf_fmt, ms->dmabuf_px0, ms->subsurf_created,
                    ms->top_parent_calls, ms->dialog_created, ms->modal_set_calls, ms->modal_unset_calls,
                    ms->dialog_destroyed_before_top,
                    ms->tearing_objs, ms->tearing_async_calls, ms->tearing_vsync_calls, ms->tearing_destroyed,
                    ms->fifo_objs, ms->fifo_set_calls, ms->fifo_wait_calls, ms->fifo_destroyed);
            fclose(f);
            rename(tmp_p, dst_p);
        }
    }

    static void top_destroy_cb(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && ss->xdg_top == r)
            ss->xdg_top = NULL;
        wl_resource_destroy(r);
    }
    static void top_str(struct wl_client *c, struct wl_resource *r, const char *s) { (void)c; (void)r; (void)s; }
    static void top_parent(struct wl_client *c, struct wl_resource *r, struct wl_resource *p) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && ss->ms && p) {
            ss->ms->top_parent_calls++;
            write_proto_ext_state(ss->ms);
        }
    }
    static void top_sz(struct wl_client *c, struct wl_resource *r, int32_t w, int32_t h) { (void)c; (void)r; (void)w; (void)h; }
    static void top_void(struct wl_client *c, struct wl_resource *r) { (void)c; (void)r; }
    static void top_fs(struct wl_client *c, struct wl_resource *r, struct wl_resource *o) { (void)c; (void)r; (void)o; }
    static void top_seat_u32(struct wl_client *c, struct wl_resource *r, struct wl_resource *s, uint32_t u) { (void)c; (void)r; (void)s; (void)u; }
    static void top_resize(struct wl_client *c, struct wl_resource *r, struct wl_resource *s, uint32_t u, uint32_t e) { (void)c; (void)r; (void)s; (void)u; (void)e; }
    static void top_menu(struct wl_client *c, struct wl_resource *r, struct wl_resource *s, uint32_t u, int32_t x, int32_t y) { (void)c; (void)r; (void)s; (void)u; (void)x; (void)y; }

    static const struct xdg_toplevel_interface g_xdg_top_impl = {
        .destroy = top_destroy_cb, .set_parent = top_parent, .set_title = top_str, .set_app_id = top_str,
        .show_window_menu = top_menu, .move = top_seat_u32, .resize = top_resize,
        .set_max_size = top_sz, .set_min_size = top_sz, .set_maximized = top_void,
        .unset_maximized = top_void, .set_fullscreen = top_fs, .unset_fullscreen = top_void, .set_minimized = top_void,
    };

    static void pos_size(struct wl_client *c, struct wl_resource *r, int32_t w, int32_t h) {
        (void)c;
        PosState *ps = wl_resource_get_user_data(r);
        if (ps) { ps->w = w; ps->h = h; }
    }
    static void pos_anchor_rect(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h) {
        (void)c; (void)w; (void)h;
        PosState *ps = wl_resource_get_user_data(r);
        if (ps) { ps->ax = x; ps->ay = y; }
    }
    static void pos_u32(struct wl_client *c, struct wl_resource *r, uint32_t v) { (void)c; (void)r; (void)v; }
    static void pos_off(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y) { (void)c; (void)r; (void)x; (void)y; }
    static void pos_void(struct wl_client *c, struct wl_resource *r) { (void)c; (void)r; }
    static void pos_parent_sz(struct wl_client *c, struct wl_resource *r, int32_t w, int32_t h) { (void)c; (void)r; (void)w; (void)h; }
    static const struct xdg_positioner_interface g_xdg_pos_impl = {
        .destroy = noop_destroy, .set_size = pos_size, .set_anchor_rect = pos_anchor_rect,
        .set_anchor = pos_u32, .set_gravity = pos_u32, .set_constraint_adjustment = pos_u32,
        .set_offset = pos_off, .set_reactive = pos_void, .set_parent_size = pos_parent_sz, .set_parent_configure = pos_u32,
    };

    static void pop_grab(struct wl_client *c, struct wl_resource *r, struct wl_resource *seat, uint32_t s) {
        (void)c; (void)r; (void)seat; (void)s;
    }
    static void pop_reposition(struct wl_client *c, struct wl_resource *r, struct wl_resource *pos, uint32_t tok) {
        (void)c; (void)pos;
        xdg_popup_send_repositioned(r, tok);
    }
    static const struct xdg_popup_interface g_xdg_pop_impl = {
        .destroy = noop_destroy, .grab = pop_grab, .reposition = pop_reposition,
    };

    static void xs_get_toplevel(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        SurfState *ss = wl_resource_get_user_data(r);
        struct wl_resource *tr = wl_resource_create(c, &xdg_toplevel_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(tr, &g_xdg_top_impl, ss, NULL);
        if (ss) ss->xdg_top = tr;
    }
    static void xs_get_popup(struct wl_client *c, struct wl_resource *r, uint32_t id,
                             struct wl_resource *parent, struct wl_resource *pos) {
        (void)parent;
        SurfState *ss = wl_resource_get_user_data(r);
        PosState *ps = pos ? wl_resource_get_user_data(pos) : NULL;
        struct wl_resource *pr = wl_resource_create(c, &xdg_popup_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(pr, &g_xdg_pop_impl, ss, NULL);
        if (ss) {
            ss->xdg_pop = pr;
            if (ps) { ss->pop_w = ps->w; ss->pop_h = ps->h; }
        }
    }
    static void xs_ack(struct wl_client *c, struct wl_resource *r, uint32_t s) { (void)c; (void)r; (void)s; }
    static void xs_geom(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h) { (void)c; (void)r; (void)x; (void)y; (void)w; (void)h; }
    static const struct xdg_surface_interface g_xdg_surf_impl = {
        .destroy = noop_destroy, .get_toplevel = xs_get_toplevel, .get_popup = xs_get_popup,
        .set_window_geometry = xs_geom, .ack_configure = xs_ack,
    };

    static void wm_create_positioner(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        PosState *ps = calloc(1, sizeof(*ps));
        struct wl_resource *pr = wl_resource_create(c, &xdg_positioner_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(pr, &g_xdg_pos_impl, ps, free_pos_state);
    }
    static void wm_get_xdg_surface(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *sr) {
        SurfState *ss = wl_resource_get_user_data(sr);
        struct wl_resource *xr = wl_resource_create(c, &xdg_surface_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(xr, &g_xdg_surf_impl, ss, NULL);
        if (ss) {
            ss->xdg_surf = xr;
            if (ss->ms && !ss->ms->primary_xdg_surf) {
                ss->ms->primary_surf = sr;
                ss->ms->primary_xdg_surf = xr;
            }
        }
    }
    static void wm_pong(struct wl_client *c, struct wl_resource *r, uint32_t s) { (void)c; (void)r; (void)s; }
    static const struct xdg_wm_base_interface g_wm_impl = {
        .destroy = noop_destroy, .create_positioner = wm_create_positioner, .get_xdg_surface = wm_get_xdg_surface, .pong = wm_pong
    };
    static void bind_wm(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        struct wl_resource *r = wl_resource_create(c, &xdg_wm_base_interface, ver < 5 ? ver : 5, id);
        wl_resource_set_implementation(r, &g_wm_impl, d, NULL);
    }

    /* wl_seat */
    static void write_cursor_state(MiniSrv *ms) {
        if (!ms) return;
        char p[512];
        snprintf(p, sizeof(p), "%s/cursor_state.txt", ms->sync_dir);
        FILE *f = fopen(p, "w");
        if (f) {
            fprintf(f, "shape_calls=%d\nseen_default=%d\nseen_text=%d\nseen_pointer=%d\nlast_shape=%u\ncustom_calls=%d\n",
                    ms->shape_calls,
                    ms->seen_shape_default ? 1 : 0,
                    ms->seen_shape_text ? 1 : 0,
                    ms->seen_shape_pointer ? 1 : 0,
                    ms->last_cursor_shape,
                    ms->custom_cursor_calls);
            fclose(f);
        }
    }
    static void ptr_cursor(struct wl_client *c, struct wl_resource *r, uint32_t s, struct wl_resource *sr, int32_t x, int32_t y) {
        (void)c; (void)s; (void)x; (void)y;
        MiniSrv *ms = wl_resource_get_user_data(r);
        if (ms && sr) {
            ms->custom_cursor_calls++;
            write_cursor_state(ms);
        }
    }
    static const struct wl_pointer_interface g_ptr_impl = { .set_cursor = ptr_cursor, .release = noop_destroy };
    static const struct wl_keyboard_interface g_kbd_impl = { .release = noop_destroy };
    static const struct wl_touch_interface g_touch_impl = { .release = noop_destroy };

    static void seat_get_pointer(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *pr = wl_resource_create(c, &wl_pointer_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(pr, &g_ptr_impl, ms, NULL);
        if (!ms->primary_ptr) ms->primary_ptr = pr;
    }
    static void seat_get_keyboard(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *kr = wl_resource_create(c, &wl_keyboard_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(kr, &g_kbd_impl, ms, NULL);
        if (!ms->primary_kbd) ms->primary_kbd = kr;
    }
    static void seat_get_touch(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *tr = wl_resource_create(c, &wl_touch_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(tr, &g_touch_impl, ms, NULL);
        if (!ms->primary_touch) ms->primary_touch = tr;
    }
    static const struct wl_seat_interface g_seat_impl = {
        .get_pointer = seat_get_pointer, .get_keyboard = seat_get_keyboard, .get_touch = seat_get_touch, .release = noop_destroy
    };
    static void bind_seat(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        struct wl_resource *r = wl_resource_create(c, &wl_seat_interface, ver < 5 ? ver : 5, id);
        wl_resource_set_implementation(r, &g_seat_impl, d, NULL);
        wl_seat_send_capabilities(r, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD | WL_SEAT_CAPABILITY_TOUCH);
    }

    /* wp_cursor_shape_manager_v1 */
    static void cshape_dev_set_shape(struct wl_client *c, struct wl_resource *r, uint32_t ser, uint32_t shape) {
        (void)c; (void)ser;
        MiniSrv *ms = wl_resource_get_user_data(r);
        if (!ms) return;
        ms->shape_calls++;
        ms->last_cursor_shape = shape;
        if (shape == WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT) ms->seen_shape_default = true;
        if (shape == WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_TEXT) ms->seen_shape_text = true;
        if (shape == WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_POINTER) ms->seen_shape_pointer = true;
        write_cursor_state(ms);
    }
    static const struct wp_cursor_shape_device_v1_interface g_cshape_dev_impl = {
        .destroy = noop_destroy,
        .set_shape = cshape_dev_set_shape,
    };
    static void cshape_get_ptr(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *ptr) {
        (void)ptr;
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *dr = wl_resource_create(c, &wp_cursor_shape_device_v1_interface, 1, id);
        wl_resource_set_implementation(dr, &g_cshape_dev_impl, ms, NULL);
    }
    static void cshape_get_tab(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *tool) {
        (void)tool;
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *dr = wl_resource_create(c, &wp_cursor_shape_device_v1_interface, 1, id);
        wl_resource_set_implementation(dr, &g_cshape_dev_impl, ms, NULL);
    }
    static const struct wp_cursor_shape_manager_v1_interface g_cshape_mgr_impl = {
        .destroy = noop_destroy,
        .get_pointer = cshape_get_ptr,
        .get_tablet_tool_v2 = cshape_get_tab,
    };
    static void bind_cshape(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        (void)ver;
        struct wl_resource *r = wl_resource_create(c, &wp_cursor_shape_manager_v1_interface, 1, id);
        wl_resource_set_implementation(r, &g_cshape_mgr_impl, d, NULL);
    }

    /* wl_data_device_manager (DnD + multi-client clipboard broker) */
    static void dnd_offer_accept(struct wl_client *c, struct wl_resource *r, uint32_t s, const char *m) { (void)c; (void)r; (void)s; (void)m; }
    static void dnd_offer_receive(struct wl_client *c, struct wl_resource *r, const char *mime, int32_t fd) {
        (void)c; (void)r; (void)mime;
        const char *payload = "file:///wayland/dropped-item.txt\r\n";
        ssize_t nw = write(fd, payload, strlen(payload));
        (void)nw;
        close(fd);
    }
    static void dnd_offer_finish(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        MiniSrv *ms = wl_resource_get_user_data(r);
        if (ms) ms->dnd_finished_seen = true;
    }
    static void dnd_offer_actions(struct wl_client *c, struct wl_resource *r, uint32_t a, uint32_t p) { (void)c; (void)r; (void)a; (void)p; }
    static const struct wl_data_offer_interface g_dnd_offer_impl = {
        .accept = dnd_offer_accept, .receive = dnd_offer_receive, .destroy = noop_destroy,
        .finish = dnd_offer_finish, .set_actions = dnd_offer_actions
    };

    static void clip_offer_receive(struct wl_client *c, struct wl_resource *r, const char *mime, int32_t fd) {
        (void)c;
        DataSrc *src = wl_resource_get_user_data(r);
        if (src && src->res) {
            wl_data_source_send_send(src->res, mime, fd);
            wl_client_flush(wl_resource_get_client(src->res));
        }
        close(fd);
    }
    static const struct wl_data_offer_interface g_clip_offer_impl = {
        .accept = dnd_offer_accept, .receive = clip_offer_receive, .destroy = noop_destroy,
        .finish = dnd_offer_actions ? NULL : NULL, .set_actions = dnd_offer_actions
    };

    static void free_data_src(struct wl_resource *r) {
        DataSrc *ds = wl_resource_get_user_data(r);
        if (!ds) return;
        if (g_ms && g_ms->cur_clip_src == ds) g_ms->cur_clip_src = NULL;
        if (g_ms && g_ms->cur_prim_src == ds) g_ms->cur_prim_src = NULL;
        free(ds);
    }
    static void dsrc_offer(struct wl_client *c, struct wl_resource *r, const char *m) {
        (void)c;
        DataSrc *ds = wl_resource_get_user_data(r);
        if (ds && m && ds->mime_count < 16)
            snprintf(ds->mimes[ds->mime_count++], 96, "%s", m);
    }
    static void dsrc_act(struct wl_client *c, struct wl_resource *r, uint32_t a) { (void)c; (void)r; (void)a; }
    static const struct wl_data_source_interface g_dsrc_impl = {
        .offer = dsrc_offer, .destroy = noop_destroy, .set_actions = dsrc_act
    };

    static void send_clip_offer_to_dev(DataSrc *src, struct wl_resource *dev) {
        if (!dev) return;
        if (!src || !src->res) {
            wl_data_device_send_selection(dev, NULL);
            return;
        }
        /* Do not overwrite the owning client's own local selection */
        if (wl_resource_get_client(dev) == wl_resource_get_client(src->res))
            return;
        struct wl_resource *off = wl_resource_create(
            wl_resource_get_client(dev), &wl_data_offer_interface, wl_resource_get_version(dev), 0);
        wl_resource_set_implementation(off, &g_clip_offer_impl, src, NULL);
        wl_data_device_send_data_offer(dev, off);
        for (int i = 0; i < src->mime_count; ++i)
            wl_data_offer_send_offer(off, src->mimes[i]);
        wl_data_device_send_selection(dev, off);
    }

    static void broadcast_clipboard_to_devices(MiniSrv *ms) {
        for (int i = 0; i < ms->data_dev_count; ++i) {
            if (ms->data_devs[i])
                send_clip_offer_to_dev(ms->cur_clip_src, ms->data_devs[i]);
        }
    }

    static void unlink_data_dev(struct wl_resource *r) {
        if (!g_ms) return;
        for (int i = 0; i < g_ms->data_dev_count; ++i)
            if (g_ms->data_devs[i] == r) g_ms->data_devs[i] = NULL;
        if (g_ms->primary_data_dev == r) g_ms->primary_data_dev = NULL;
    }

    static void ddev_start_drag(struct wl_client *c, struct wl_resource *r, struct wl_resource *src,
                                struct wl_resource *orig, struct wl_resource *ico, uint32_t ser) {
        (void)c; (void)r; (void)src; (void)orig; (void)ico; (void)ser;
    }
    static void ddev_set_sel(struct wl_client *c, struct wl_resource *r, struct wl_resource *src, uint32_t ser) {
        (void)c; (void)ser;
        MiniSrv *ms = wl_resource_get_user_data(r);
        DataSrc *new_ds = src ? wl_resource_get_user_data(src) : NULL;
        if (ms->cur_clip_src && ms->cur_clip_src != new_ds && ms->cur_clip_src->res)
            wl_data_source_send_cancelled(ms->cur_clip_src->res);
        ms->cur_clip_src = new_ds;
        broadcast_clipboard_to_devices(ms);
    }
    static const struct wl_data_device_interface g_ddev_impl = {
        .start_drag = ddev_start_drag, .set_selection = ddev_set_sel, .release = noop_destroy
    };
    static void dmgr_create_src(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        (void)r;
        DataSrc *ds = calloc(1, sizeof(*ds));
        ds->res = wl_resource_create(c, &wl_data_source_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(ds->res, &g_dsrc_impl, ds, free_data_src);
    }
    static void dmgr_get_dev(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *seat) {
        (void)seat;
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *dr = wl_resource_create(c, &wl_data_device_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(dr, &g_ddev_impl, ms, unlink_data_dev);
        if (!ms->primary_data_dev) ms->primary_data_dev = dr;
        if (ms->data_dev_count < 8) ms->data_devs[ms->data_dev_count++] = dr;
        if (ms->cur_clip_src) send_clip_offer_to_dev(ms->cur_clip_src, dr);
    }
    static const struct wl_data_device_manager_interface g_dmgr_impl = {
        .create_data_source = dmgr_create_src, .get_data_device = dmgr_get_dev
    };
    static void bind_dmgr(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        struct wl_resource *r = wl_resource_create(c, &wl_data_device_manager_interface, ver < 3 ? ver : 3, id);
        wl_resource_set_implementation(r, &g_dmgr_impl, d, NULL);
    }

    /* zwp_primary_selection_device_manager_v1 */
    static void prim_offer_receive(struct wl_client *c, struct wl_resource *r, const char *mime, int32_t fd) {
        (void)c;
        DataSrc *src = wl_resource_get_user_data(r);
        if (src && src->res) {
            zwp_primary_selection_source_v1_send_send(src->res, mime, fd);
            wl_client_flush(wl_resource_get_client(src->res));
        }
        close(fd);
    }
    static const struct zwp_primary_selection_offer_v1_interface g_prim_offer_impl = {
        .receive = prim_offer_receive, .destroy = noop_destroy
    };
    static const struct zwp_primary_selection_source_v1_interface g_psrc_impl = {
        .offer = dsrc_offer, .destroy = noop_destroy
    };
    static void send_prim_offer_to_dev(DataSrc *src, struct wl_resource *dev) {
        if (!dev) return;
        if (!src || !src->res) {
            zwp_primary_selection_device_v1_send_selection(dev, NULL);
            return;
        }
        if (wl_resource_get_client(dev) == wl_resource_get_client(src->res))
            return;
        struct wl_resource *off = wl_resource_create(
            wl_resource_get_client(dev), &zwp_primary_selection_offer_v1_interface, 1, 0);
        wl_resource_set_implementation(off, &g_prim_offer_impl, src, NULL);
        zwp_primary_selection_device_v1_send_data_offer(dev, off);
        for (int i = 0; i < src->mime_count; ++i)
            zwp_primary_selection_offer_v1_send_offer(off, src->mimes[i]);
        zwp_primary_selection_device_v1_send_selection(dev, off);
    }
    static void broadcast_primary_to_devices(MiniSrv *ms) {
        for (int i = 0; i < ms->prim_dev_count; ++i) {
            if (ms->prim_devs[i])
                send_prim_offer_to_dev(ms->cur_prim_src, ms->prim_devs[i]);
        }
    }
    static void unlink_prim_dev(struct wl_resource *r) {
        if (!g_ms) return;
        for (int i = 0; i < g_ms->prim_dev_count; ++i)
            if (g_ms->prim_devs[i] == r) g_ms->prim_devs[i] = NULL;
    }
    static void pdev_set_sel(struct wl_client *c, struct wl_resource *r, struct wl_resource *src, uint32_t ser) {
        (void)c; (void)ser;
        MiniSrv *ms = wl_resource_get_user_data(r);
        DataSrc *new_ds = src ? wl_resource_get_user_data(src) : NULL;
        if (ms->cur_prim_src && ms->cur_prim_src != new_ds && ms->cur_prim_src->res)
            zwp_primary_selection_source_v1_send_cancelled(ms->cur_prim_src->res);
        ms->cur_prim_src = new_ds;
        broadcast_primary_to_devices(ms);
    }
    static const struct zwp_primary_selection_device_v1_interface g_pdev_impl = {
        .set_selection = pdev_set_sel, .destroy = noop_destroy
    };
    static void pmgr_create_src(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        (void)r;
        DataSrc *ds = calloc(1, sizeof(*ds));
        ds->res = wl_resource_create(c, &zwp_primary_selection_source_v1_interface, 1, id);
        wl_resource_set_implementation(ds->res, &g_psrc_impl, ds, free_data_src);
    }
    static void pmgr_get_dev(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *seat) {
        (void)seat;
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *pr = wl_resource_create(c, &zwp_primary_selection_device_v1_interface, 1, id);
        wl_resource_set_implementation(pr, &g_pdev_impl, ms, unlink_prim_dev);
        if (ms->prim_dev_count < 8) ms->prim_devs[ms->prim_dev_count++] = pr;
        if (ms->cur_prim_src) send_prim_offer_to_dev(ms->cur_prim_src, pr);
    }
    static const struct zwp_primary_selection_device_manager_v1_interface g_pmgr_impl = {
        .create_source = pmgr_create_src, .get_device = pmgr_get_dev, .destroy = noop_destroy
    };
    static void bind_pmgr(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        (void)ver;
        struct wl_resource *r = wl_resource_create(c, &zwp_primary_selection_device_manager_v1_interface, 1, id);
        wl_resource_set_implementation(r, &g_pmgr_impl, d, NULL);
    }

    /* zwp_text_input_manager_v3 */
    static void ti_enable(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        MiniSrv *ms = wl_resource_get_user_data(r);
        if (ms) ms->ime_enabled_seen = true;
    }
    static void ti_disable(struct wl_client *c, struct wl_resource *r) { (void)c; (void)r; }
    static void ti_surr(struct wl_client *c, struct wl_resource *r, const char *t, int32_t cur, int32_t anc) {
        (void)c; (void)r; (void)t; (void)cur; (void)anc;
    }
    static void ti_cause(struct wl_client *c, struct wl_resource *r, uint32_t cs) { (void)c; (void)r; (void)cs; }
    static void ti_ctype(struct wl_client *c, struct wl_resource *r, uint32_t h, uint32_t p) { (void)c; (void)r; (void)h; (void)p; }
    static void ti_rect(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h) {
        (void)c; (void)x; (void)y; (void)w; (void)h;
        MiniSrv *ms = wl_resource_get_user_data(r);
        if (ms) ms->ime_cursor_rect_seen = true;
    }
    static void ti_commit(struct wl_client *c, struct wl_resource *r) { (void)c; (void)r; }

    static const struct zwp_text_input_v3_interface g_ti_impl = {
        .destroy = noop_destroy, .enable = ti_enable, .disable = ti_disable,
        .set_surrounding_text = ti_surr, .set_text_change_cause = ti_cause,
        .set_content_type = ti_ctype, .set_cursor_rectangle = ti_rect, .commit = ti_commit,
    };
    static void tim_get_ti(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *seat) {
        (void)seat;
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *tr = wl_resource_create(c, &zwp_text_input_v3_interface, 1, id);
        wl_resource_set_implementation(tr, &g_ti_impl, ms, NULL);
        if (!ms->text_in_res) {
            ms->text_in_res = tr;
            if (ms->primary_surf)
                zwp_text_input_v3_send_enter(ms->text_in_res, ms->primary_surf);
        }
    }
    static const struct zwp_text_input_manager_v3_interface g_tim_impl = {
        .destroy = noop_destroy, .get_text_input = tim_get_ti
    };
    static void bind_tim(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        (void)ver;
        struct wl_resource *r = wl_resource_create(c, &zwp_text_input_manager_v3_interface, 1, id);
        wl_resource_set_implementation(r, &g_tim_impl, d, NULL);
    }

    /* xdg_activation_v1 */
    typedef struct {
        MiniSrv *ms;
        uint32_t serial;
        bool has_surf;
        char app_id[128];
    } ActTokState;

    static void free_act_tok(struct wl_resource *r) {
        free(wl_resource_get_user_data(r));
    }
    static void write_act_state(MiniSrv *ms) {
        char p[512];
        snprintf(p, sizeof(p), "%s/act_state.txt", ms->sync_dir);
        FILE *f = fopen(p, "w");
        if (f) {
            fprintf(f, "calls=%d\nfirst=%s\nlast=%s\nserial=%u\nhas_surf=%d\napp_id=%s\n",
                    ms->activate_calls,
                    ms->first_activated_token,
                    ms->last_activated_token,
                    ms->last_tok_serial,
                    ms->last_tok_had_surface ? 1 : 0,
                    ms->last_tok_app_id);
            fclose(f);
        }
    }
    static void atok_set_serial(struct wl_client *c, struct wl_resource *r, uint32_t ser, struct wl_resource *seat) {
        (void)c; (void)seat;
        ActTokState *st = wl_resource_get_user_data(r);
        if (st) st->serial = ser;
    }
    static void atok_set_app_id(struct wl_client *c, struct wl_resource *r, const char *app_id) {
        (void)c;
        ActTokState *st = wl_resource_get_user_data(r);
        if (st && app_id) snprintf(st->app_id, sizeof(st->app_id), "%s", app_id);
    }
    static void atok_set_surface(struct wl_client *c, struct wl_resource *r, struct wl_resource *surf) {
        (void)c;
        ActTokState *st = wl_resource_get_user_data(r);
        if (st && surf) st->has_surf = true;
    }
    static void atok_commit(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        ActTokState *st = wl_resource_get_user_data(r);
        if (!st || !st->ms) return;
        MiniSrv *ms = st->ms;
        ms->last_tok_serial = st->serial;
        ms->last_tok_had_surface = st->has_surf;
        snprintf(ms->last_tok_app_id, sizeof(ms->last_tok_app_id), "%s", st->app_id);
        char tok_buf[96];
        snprintf(tok_buf, sizeof(tok_buf), "mini-act-tok-%u", ++ms->act_token_seq);
        write_act_state(ms);
        xdg_activation_token_v1_send_done(r, tok_buf);
    }
    static const struct xdg_activation_token_v1_interface g_atok_impl = {
        .set_serial = atok_set_serial,
        .set_app_id = atok_set_app_id,
        .set_surface = atok_set_surface,
        .commit = atok_commit,
        .destroy = noop_destroy,
    };
    static void act_get_token(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        MiniSrv *ms = wl_resource_get_user_data(r);
        ActTokState *st = calloc(1, sizeof(*st));
        st->ms = ms;
        struct wl_resource *tr = wl_resource_create(c, &xdg_activation_token_v1_interface, 1, id);
        wl_resource_set_implementation(tr, &g_atok_impl, st, free_act_tok);
    }
    static void act_activate(struct wl_client *c, struct wl_resource *r, const char *token, struct wl_resource *surf) {
        (void)c; (void)surf;
        MiniSrv *ms = wl_resource_get_user_data(r);
        if (!ms || !token) return;
        ms->activate_calls++;
        if (ms->activate_calls == 1)
            snprintf(ms->first_activated_token, sizeof(ms->first_activated_token), "%s", token);
        snprintf(ms->last_activated_token, sizeof(ms->last_activated_token), "%s", token);
        write_act_state(ms);
    }
    static const struct xdg_activation_v1_interface g_act_impl = {
        .destroy = noop_destroy,
        .get_activation_token = act_get_token,
        .activate = act_activate,
    };
    static void bind_act(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        (void)ver;
        struct wl_resource *r = wl_resource_create(c, &xdg_activation_v1_interface, 1, id);
        wl_resource_set_implementation(r, &g_act_impl, d, NULL);
    }

    typedef struct {
        MiniSrv *ms;
        int idx;
    } OutSlot;
    static const struct wl_output_interface g_out_impl = {
        .release = noop_destroy,
    };
    static void bind_out(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        OutSlot *os = (OutSlot *)d;
        uint32_t v = ver < 4 ? ver : 4;
        struct wl_resource *r = wl_resource_create(c, &wl_output_interface, (int)v, id);
        wl_resource_set_implementation(r, &g_out_impl, os, NULL);
        int32_t ox = (os->idx == 0) ? 0 : 1920;
        int32_t sc = (os->idx == 0) ? 1 : 2;
        const char *nm = (os->idx == 0) ? "OUT-1" : "OUT-2";
        wl_output_send_geometry(r, ox, 0, 508, 285, WL_OUTPUT_SUBPIXEL_UNKNOWN,
                                "CK", nm, WL_OUTPUT_TRANSFORM_NORMAL);
        wl_output_send_mode(r, WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED,
                            1920, 1080, 60000);
        if (v >= 2)
            wl_output_send_scale(r, sc);
        if (v >= 4)
            wl_output_send_name(r, nm);
        if (v >= 2)
            wl_output_send_done(r);
        if (os->idx == 0 && os->ms->out1_count < 8)
            os->ms->out1_res[os->ms->out1_count++] = r;
        else if (os->idx == 1 && os->ms->out2_count < 8)
            os->ms->out2_res[os->ms->out2_count++] = r;
    }
    static struct wl_resource *find_out_for_surf(struct wl_resource **arr, int n, struct wl_resource *surf) {
        if (!surf) return NULL;
        struct wl_client *sc = wl_resource_get_client(surf);
        for (int i = 0; i < n; ++i) {
            if (arr[i] && wl_resource_get_client(arr[i]) == sc)
                return arr[i];
        }
        return NULL;
    }

    /* wl_subcompositor */
    static void subsurf_pos(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y) {
        (void)c; (void)r; (void)x; (void)y;
    }
    static void subsurf_place(struct wl_client *c, struct wl_resource *r, struct wl_resource *sib) {
        (void)c; (void)r; (void)sib;
    }
    static void subsurf_void(struct wl_client *c, struct wl_resource *r) { (void)c; (void)r; }
    static const struct wl_subsurface_interface g_subsurf_impl = {
        .destroy = noop_destroy,
        .set_position = subsurf_pos,
        .place_above = subsurf_place,
        .place_below = subsurf_place,
        .set_sync = subsurf_void,
        .set_desync = subsurf_void,
    };
    static void subcomp_get_subsurface(struct wl_client *c, struct wl_resource *r, uint32_t id,
                                       struct wl_resource *surf, struct wl_resource *parent) {
        (void)surf; (void)parent;
        MiniSrv *ms = wl_resource_get_user_data(r);
        struct wl_resource *sr = wl_resource_create(c, &wl_subsurface_interface, 1, id);
        wl_resource_set_implementation(sr, &g_subsurf_impl, ms, NULL);
        if (ms) {
            ms->subsurf_created++;
            write_proto_ext_state(ms);
        }
    }
    static const struct wl_subcompositor_interface g_subcomp_impl = {
        .destroy = noop_destroy,
        .get_subsurface = subcomp_get_subsurface,
    };
    static void bind_subcomp(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        (void)ver;
        struct wl_resource *r = wl_resource_create(c, &wl_subcompositor_interface, 1, id);
        wl_resource_set_implementation(r, &g_subcomp_impl, d, NULL);
    }

    /* zwp_linux_dmabuf_v1 (v4 default_feedback + create_immed) */
    typedef struct {
        MiniSrv *ms;
        int fd;
        uint32_t offset;
        uint32_t stride;
        uint64_t modifier;
        bool has_plane0;
    } DmabufParamsState;

    static const struct wl_buffer_interface g_dmabuf_wlbuf_impl = {
        .destroy = noop_destroy,
    };
    static void free_dmabuf_params(struct wl_resource *r) {
        DmabufParamsState *ps = wl_resource_get_user_data(r);
        if (ps) {
            if (ps->fd >= 0)
                close(ps->fd);
            free(ps);
        }
    }
    static void dbp_add(struct wl_client *c, struct wl_resource *r, int32_t fd,
                        uint32_t plane_idx, uint32_t offset, uint32_t stride,
                        uint32_t mod_hi, uint32_t mod_lo) {
        (void)c;
        DmabufParamsState *ps = wl_resource_get_user_data(r);
        if (!ps || plane_idx != 0) {
            if (fd >= 0) close(fd);
            return;
        }
        if (ps->fd >= 0) close(ps->fd);
        ps->fd = fd;
        ps->offset = offset;
        ps->stride = stride;
        ps->modifier = ((uint64_t)mod_hi << 32) | (uint64_t)mod_lo;
        ps->has_plane0 = true;
    }
    static void dbp_create(struct wl_client *c, struct wl_resource *r,
                           int32_t w, int32_t h, uint32_t fmt, uint32_t flags) {
        (void)c; (void)r; (void)w; (void)h; (void)fmt; (void)flags;
    }
    static void dbp_create_immed(struct wl_client *c, struct wl_resource *r, uint32_t buf_id,
                                 int32_t w, int32_t h, uint32_t fmt, uint32_t flags) {
        (void)flags;
        DmabufParamsState *ps = wl_resource_get_user_data(r);
        struct wl_resource *br = wl_resource_create(c, &wl_buffer_interface, 1, buf_id);
        wl_resource_set_implementation(br, &g_dmabuf_wlbuf_impl, NULL, NULL);
        if (ps && ps->ms && ps->has_plane0 && ps->fd >= 0 && w > 0 && h > 0) {
            size_t map_len = (size_t)ps->offset + (size_t)ps->stride * (size_t)h;
            uint32_t px0 = 0;
            void *mp = mmap(NULL, map_len, PROT_READ, MAP_PRIVATE, ps->fd, 0);
            if (mp != MAP_FAILED) {
                memcpy(&px0, (const uint8_t *)mp + ps->offset, sizeof(px0));
                munmap(mp, map_len);
            }
            close(ps->fd);
            ps->fd = -1;
            ps->ms->dmabuf_imports++;
            ps->ms->dmabuf_w = w;
            ps->ms->dmabuf_h = h;
            ps->ms->dmabuf_stride = ps->stride;
            ps->ms->dmabuf_fmt = fmt;
            ps->ms->dmabuf_px0 = px0;
            write_proto_ext_state(ps->ms);
        }
    }
    static const struct zwp_linux_buffer_params_v1_interface g_dbp_impl = {
        .destroy = noop_destroy,
        .add = dbp_add,
        .create = dbp_create,
        .create_immed = dbp_create_immed,
    };
    static const struct zwp_linux_dmabuf_feedback_v1_interface g_dbfb_impl = {
        .destroy = noop_destroy,
    };
    static void send_dmabuf_feedback(struct wl_resource *fbr) {
        struct {
            uint32_t format;
            uint32_t pad;
            uint64_t modifier;
        } table[2] = {
            { 0x34325241u /* ARGB8888 */, 0u, 0ULL /* LINEAR */ },
            { 0x34325258u /* XRGB8888 */, 0u, 0x00ffffffffffffffULL /* INVALID */ },
        };
        int fd = memfd_create("mini-dmabuf-table", MFD_CLOEXEC);
        if (fd < 0) return;
        if (ftruncate(fd, (off_t)sizeof(table)) == 0) {
            void *mp = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (mp != MAP_FAILED) {
                memcpy(mp, table, sizeof(table));
                munmap(mp, sizeof(table));
            }
            zwp_linux_dmabuf_feedback_v1_send_format_table(fbr, fd, (uint32_t)sizeof(table));
        }
        close(fd);
        struct wl_array dev_arr;
        wl_array_init(&dev_arr);
        uint64_t *dev_p = wl_array_add(&dev_arr, sizeof(uint64_t));
        *dev_p = 0x0800e200ULL;
        zwp_linux_dmabuf_feedback_v1_send_main_device(fbr, &dev_arr);
        zwp_linux_dmabuf_feedback_v1_send_tranche_target_device(fbr, &dev_arr);
        wl_array_release(&dev_arr);

        struct wl_array idx_arr;
        wl_array_init(&idx_arr);
        uint16_t *idxs = wl_array_add(&idx_arr, 2 * sizeof(uint16_t));
        idxs[0] = 0;
        idxs[1] = 1;
        zwp_linux_dmabuf_feedback_v1_send_tranche_formats(fbr, &idx_arr);
        wl_array_release(&idx_arr);

        zwp_linux_dmabuf_feedback_v1_send_tranche_flags(fbr, 0);
        zwp_linux_dmabuf_feedback_v1_send_tranche_done(fbr);
        zwp_linux_dmabuf_feedback_v1_send_done(fbr);
    }
    static void dmabuf_create_params(struct wl_client *c, struct wl_resource *r, uint32_t params_id) {
        MiniSrv *ms = wl_resource_get_user_data(r);
        DmabufParamsState *ps = calloc(1, sizeof(*ps));
        ps->ms = ms;
        ps->fd = -1;
        struct wl_resource *pr = wl_resource_create(
            c, &zwp_linux_buffer_params_v1_interface, wl_resource_get_version(r), params_id);
        wl_resource_set_implementation(pr, &g_dbp_impl, ps, free_dmabuf_params);
    }
    static void dmabuf_get_default_fb(struct wl_client *c, struct wl_resource *r, uint32_t id) {
        struct wl_resource *fbr = wl_resource_create(
            c, &zwp_linux_dmabuf_feedback_v1_interface, wl_resource_get_version(r), id);
        wl_resource_set_implementation(fbr, &g_dbfb_impl, wl_resource_get_user_data(r), NULL);
        send_dmabuf_feedback(fbr);
    }
    static void dmabuf_get_surf_fb(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *surf) {
        (void)surf;
        dmabuf_get_default_fb(c, r, id);
    }
    static const struct zwp_linux_dmabuf_v1_interface g_dmabuf_impl = {
        .destroy = noop_destroy,
        .create_params = dmabuf_create_params,
        .get_default_feedback = dmabuf_get_default_fb,
        .get_surface_feedback = dmabuf_get_surf_fb,
    };
    static void bind_dmabuf(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        uint32_t v = ver < 4 ? ver : 4;
        struct wl_resource *r = wl_resource_create(c, &zwp_linux_dmabuf_v1_interface, (int)v, id);
        wl_resource_set_implementation(r, &g_dmabuf_impl, d, NULL);
    }

    /* xdg_wm_dialog_v1 */
    static void xdlg_destroy(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss) {
            ss->has_dialog = false;
            if (ss->ms && ss->xdg_top != NULL) {
                ss->ms->dialog_destroyed_before_top++;
                write_proto_ext_state(ss->ms);
            }
        }
        wl_resource_destroy(r);
    }
    static void xdlg_set_modal(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && ss->ms) {
            ss->ms->modal_set_calls++;
            write_proto_ext_state(ss->ms);
        }
    }
    static void xdlg_unset_modal(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && ss->ms) {
            ss->ms->modal_unset_calls++;
            write_proto_ext_state(ss->ms);
        }
    }
    static const struct xdg_dialog_v1_interface g_xdlg_impl = {
        .destroy = xdlg_destroy,
        .set_modal = xdlg_set_modal,
        .unset_modal = xdlg_unset_modal,
    };
    static void xdlg_mgr_get_dialog(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *top) {
        SurfState *ss = top ? wl_resource_get_user_data(top) : NULL;
        if (ss && ss->has_dialog) {
            wl_resource_post_error(r, XDG_WM_DIALOG_V1_ERROR_ALREADY_USED, "duplicate xdg_dialog_v1");
            return;
        }
        struct wl_resource *dr = wl_resource_create(c, &xdg_dialog_v1_interface, 1, id);
        wl_resource_set_implementation(dr, &g_xdlg_impl, ss, NULL);
        if (ss) {
            ss->has_dialog = true;
            if (ss->ms) {
                ss->ms->dialog_created++;
                write_proto_ext_state(ss->ms);
            }
        }
    }
    static const struct xdg_wm_dialog_v1_interface g_xdlg_mgr_impl = {
        .destroy = noop_destroy,
        .get_xdg_dialog = xdlg_mgr_get_dialog,
    };
    static void bind_xdlg_mgr(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        (void)ver;
        struct wl_resource *r = wl_resource_create(c, &xdg_wm_dialog_v1_interface, 1, id);
        wl_resource_set_implementation(r, &g_xdlg_mgr_impl, d, NULL);
    }

    /* wp_tearing_control_manager_v1 */
    static void tear_destroy(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss) {
            ss->has_tearing = false;
            if (ss->ms) {
                ss->ms->tearing_destroyed++;
                write_proto_ext_state(ss->ms);
            }
        }
        wl_resource_destroy(r);
    }
    static void tear_set_hint(struct wl_client *c, struct wl_resource *r, uint32_t hint) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && ss->ms) {
            if (hint == WP_TEARING_CONTROL_V1_PRESENTATION_HINT_ASYNC)
                ss->ms->tearing_async_calls++;
            else
                ss->ms->tearing_vsync_calls++;
            write_proto_ext_state(ss->ms);
        }
    }
    static const struct wp_tearing_control_v1_interface g_tear_impl = {
        .set_presentation_hint = tear_set_hint,
        .destroy = tear_destroy,
    };
    static void tear_mgr_get(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *surf) {
        SurfState *ss = surf ? wl_resource_get_user_data(surf) : NULL;
        if (ss && ss->has_tearing) {
            wl_resource_post_error(r, WP_TEARING_CONTROL_MANAGER_V1_ERROR_TEARING_CONTROL_EXISTS, "duplicate tearing control");
            return;
        }
        struct wl_resource *tr = wl_resource_create(c, &wp_tearing_control_v1_interface, 1, id);
        wl_resource_set_implementation(tr, &g_tear_impl, ss, NULL);
        if (ss) {
            ss->has_tearing = true;
            if (ss->ms) {
                ss->ms->tearing_objs++;
                write_proto_ext_state(ss->ms);
            }
        }
    }
    static const struct wp_tearing_control_manager_v1_interface g_tear_mgr_impl = {
        .destroy = noop_destroy,
        .get_tearing_control = tear_mgr_get,
    };
    static void bind_tear_mgr(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        (void)ver;
        struct wl_resource *r = wl_resource_create(c, &wp_tearing_control_manager_v1_interface, 1, id);
        wl_resource_set_implementation(r, &g_tear_mgr_impl, d, NULL);
    }

    /* wp_fifo_manager_v1 */
    static void fifo_destroy(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss) {
            ss->has_fifo = false;
            if (ss->ms) {
                ss->ms->fifo_destroyed++;
                write_proto_ext_state(ss->ms);
            }
        }
        wl_resource_destroy(r);
    }
    static void fifo_set_barrier(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && ss->ms) {
            ss->ms->fifo_set_calls++;
            write_proto_ext_state(ss->ms);
        }
    }
    static void fifo_wait_barrier(struct wl_client *c, struct wl_resource *r) {
        (void)c;
        SurfState *ss = wl_resource_get_user_data(r);
        if (ss && ss->ms) {
            ss->ms->fifo_wait_calls++;
            write_proto_ext_state(ss->ms);
        }
    }
    static const struct wp_fifo_v1_interface g_fifo_impl = {
        .set_barrier = fifo_set_barrier,
        .wait_barrier = fifo_wait_barrier,
        .destroy = fifo_destroy,
    };
    static void fifo_mgr_get(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *surf) {
        SurfState *ss = surf ? wl_resource_get_user_data(surf) : NULL;
        if (ss && ss->has_fifo) {
            wl_resource_post_error(r, WP_FIFO_MANAGER_V1_ERROR_ALREADY_EXISTS, "duplicate fifo");
            return;
        }
        struct wl_resource *fr = wl_resource_create(c, &wp_fifo_v1_interface, 1, id);
        wl_resource_set_implementation(fr, &g_fifo_impl, ss, NULL);
        if (ss) {
            ss->has_fifo = true;
            if (ss->ms) {
                ss->ms->fifo_objs++;
                write_proto_ext_state(ss->ms);
            }
        }
    }
    static const struct wp_fifo_manager_v1_interface g_fifo_mgr_impl = {
        .destroy = noop_destroy,
        .get_fifo = fifo_mgr_get,
    };
    static void bind_fifo_mgr(struct wl_client *c, void *d, uint32_t ver, uint32_t id) {
        (void)ver;
        struct wl_resource *r = wl_resource_create(c, &wp_fifo_manager_v1_interface, 1, id);
        wl_resource_set_implementation(r, &g_fifo_mgr_impl, d, NULL);
    }

    int main(int argc, char **argv) {
        if (argc < 3) return 2;
        MiniSrv ms;
        memset(&ms, 0, sizeof(ms));
        g_ms = &ms;
        ms.sync_dir = argv[2];
        ms.dpy = wl_display_create();
        if (!ms.dpy) return 1;
        if (wl_display_add_socket(ms.dpy, argv[1]) != 0) return 1;

        OutSlot out1 = { .ms = &ms, .idx = 0 };
        OutSlot out2 = { .ms = &ms, .idx = 1 };
        wl_global_create(ms.dpy, &wl_compositor_interface, 4, &ms, bind_comp);
        wl_global_create(ms.dpy, &wl_subcompositor_interface, 1, &ms, bind_subcomp);
        wl_display_init_shm(ms.dpy);
        wl_global_create(ms.dpy, &wl_output_interface, 4, &out1, bind_out);
        wl_global_create(ms.dpy, &wl_output_interface, 4, &out2, bind_out);
        wl_global_create(ms.dpy, &xdg_wm_base_interface, 5, &ms, bind_wm);
        wl_global_create(ms.dpy, &wl_seat_interface, 5, &ms, bind_seat);
        wl_global_create(ms.dpy, &wl_data_device_manager_interface, 3, &ms, bind_dmgr);
        wl_global_create(ms.dpy, &zwp_primary_selection_device_manager_v1_interface, 1, &ms, bind_pmgr);
        wl_global_create(ms.dpy, &zwp_text_input_manager_v3_interface, 1, &ms, bind_tim);
        wl_global_create(ms.dpy, &xdg_activation_v1_interface, 1, &ms, bind_act);
        wl_global_create(ms.dpy, &wp_cursor_shape_manager_v1_interface, 1, &ms, bind_cshape);
        wl_global_create(ms.dpy, &zwp_linux_dmabuf_v1_interface, 4, &ms, bind_dmabuf);
        wl_global_create(ms.dpy, &xdg_wm_dialog_v1_interface, 1, &ms, bind_xdlg_mgr);
        wl_global_create(ms.dpy, &wp_tearing_control_manager_v1_interface, 1, &ms, bind_tear_mgr);
        wl_global_create(ms.dpy, &wp_fifo_manager_v1_interface, 1, &ms, bind_fifo_mgr);

        write_flag(ms.sync_dir, "srv.ready");

        struct wl_event_loop *loop = wl_display_get_event_loop(ms.dpy);
        int stage = 0;
        struct timespec loop_t0 = {0, 0};
        clock_gettime(CLOCK_MONOTONIC, &loop_t0);
        for (;;) {
            struct timespec loop_now = {0, 0};
            clock_gettime(CLOCK_MONOTONIC, &loop_now);
            if ((loop_now.tv_sec - loop_t0.tv_sec) > 45)
                break;
            wl_event_loop_dispatch(loop, 15);
            wl_display_flush_clients(ms.dpy);

            if (stage == 0 && file_exists(ms.sync_dir, "ime_client.ready") && ms.text_in_res) {
                if (!ms.ime_enabled_seen || !ms.ime_cursor_rect_seen) {
                    fprintf(stderr, "FAIL: zwp_text_input_v3 enable/cursor_rectangle not received\n");
                    return 1;
                }
                zwp_text_input_v3_send_preedit_string(ms.text_in_res, "にほん", 9, 9);
                zwp_text_input_v3_send_done(ms.text_in_res, 1);
                wl_display_flush_clients(ms.dpy);
                write_flag(ms.sync_dir, "ime_preedit1.sent");
                stage = 1;
            } else if (stage == 1 && file_exists(ms.sync_dir, "ime_preedit1.verified")) {
                zwp_text_input_v3_send_preedit_string(ms.text_in_res, "日本語", 9, 9);
                zwp_text_input_v3_send_done(ms.text_in_res, 2);
                wl_display_flush_clients(ms.dpy);
                struct timespec ts = {0, 40 * 1000 * 1000L};
                nanosleep(&ts, NULL);
                zwp_text_input_v3_send_commit_string(ms.text_in_res, "日本語OK");
                zwp_text_input_v3_send_done(ms.text_in_res, 3);
                wl_display_flush_clients(ms.dpy);
                nanosleep(&ts, NULL);
                zwp_text_input_v3_send_delete_surrounding_text(ms.text_in_res, 2, 0);
                zwp_text_input_v3_send_commit_string(ms.text_in_res, "!");
                zwp_text_input_v3_send_done(ms.text_in_res, 4);
                wl_display_flush_clients(ms.dpy);
                write_flag(ms.sync_dir, "ime_commit.sent");
                stage = 2;
            } else if (stage == 2 && file_exists(ms.sync_dir, "ime_commit.verified") &&
                       ms.primary_data_dev && ms.primary_surf) {
                struct wl_client *cli = wl_resource_get_client(ms.primary_data_dev);
                struct wl_resource *off = wl_resource_create(
                    cli, &wl_data_offer_interface, wl_resource_get_version(ms.primary_data_dev), 0);
                wl_resource_set_implementation(off, &g_dnd_offer_impl, &ms, NULL);
                wl_data_device_send_data_offer(ms.primary_data_dev, off);
                wl_data_offer_send_offer(off, "text/uri-list");
                if (wl_resource_get_version(off) >= 3)
                    wl_data_offer_send_source_actions(off, WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY);
                wl_data_device_send_enter(
                    ms.primary_data_dev, 10, ms.primary_surf,
                    wl_fixed_from_int(120), wl_fixed_from_int(140), off);
                wl_data_device_send_motion(
                    ms.primary_data_dev, 200,
                    wl_fixed_from_int(130), wl_fixed_from_int(145));
                wl_data_device_send_drop(ms.primary_data_dev);
                wl_display_flush_clients(ms.dpy);
                stage = 3;
            } else if (stage == 3 && file_exists(ms.sync_dir, "clipboard_exchange.verified") &&
                       ms.primary_xdg_surf) {
                SurfState *ss = wl_resource_get_user_data(ms.primary_xdg_surf);
                if (ss && ss->xdg_top) {
                    struct wl_array states;
                    wl_array_init(&states);
                    for (int wave = 0; wave < 4; ++wave) {
                        for (int step = 1; step <= 10; ++step) {
                            int k = wave * 10 + step;
                            xdg_toplevel_send_configure(ss->xdg_top, 360 + k * 6, 220 + k * 4, &states);
                            xdg_surface_send_configure(ms.primary_xdg_surf, (uint32_t)(500 + k));
                        }
                        wl_display_flush_clients(ms.dpy);
                        struct timespec rts = {0, 8 * 1000 * 1000L};
                        nanosleep(&rts, NULL);
                    }
                    wl_array_release(&states);
                }
                write_flag(ms.sync_dir, "resize_burst.sent");
                stage = 4;
            } else if (stage == 4 && file_exists(ms.sync_dir, "wheel_test.ready") &&
                       ms.primary_ptr && ms.primary_surf) {
                struct timespec wts = {0, 30 * 1000 * 1000L};
                /* Hover left XmAUTOMATIC+DrawingArea pane at (90, 250) without clicking */
                wl_pointer_send_enter(ms.primary_ptr, 700, ms.primary_surf,
                                      wl_fixed_from_int(90), wl_fixed_from_int(250));
                wl_pointer_send_motion(ms.primary_ptr, 1200,
                                       wl_fixed_from_int(90), wl_fixed_from_int(250));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                nanosleep(&wts, NULL);

                /* Sub-notch 3.75px (30-v120 = 1/4 notch) smooth pixel scroll */
                wl_pointer_send_axis(ms.primary_ptr, 1220, WL_POINTER_AXIS_VERTICAL_SCROLL,
                                     wl_fixed_from_double(3.75));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                write_flag(ms.sync_dir, "wheel_subnotch.sent");
                stage = 5;
            } else if (stage == 5 && file_exists(ms.sync_dir, "wheel_subnotch.verified")) {
                /* Tick 1: discrete wheel event with axis(0) (tests axis_discrete fallback) */
                wl_pointer_send_axis_discrete(ms.primary_ptr, WL_POINTER_AXIS_VERTICAL_SCROLL, 1);
                wl_pointer_send_axis(ms.primary_ptr, 1240, WL_POINTER_AXIS_VERTICAL_SCROLL, wl_fixed_from_int(0));
                wl_pointer_send_frame(ms.primary_ptr);
                /* Tick 2: continuous 15px scroll step */
                wl_pointer_send_axis(ms.primary_ptr, 1280, WL_POINTER_AXIS_VERTICAL_SCROLL, wl_fixed_from_int(15));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                write_flag(ms.sync_dir, "wheel_auto.sent");
                stage = 6;
            } else if (stage == 6 && file_exists(ms.sync_dir, "wheel_auto.verified")) {
                struct timespec wts = {0, 25 * 1000 * 1000L};
                /* Hover right XmAPPLICATION_DEFINED pane at (400, 250) without clicking */
                wl_pointer_send_motion(ms.primary_ptr, 1500,
                                       wl_fixed_from_int(400), wl_fixed_from_int(250));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                nanosleep(&wts, NULL);

                wl_pointer_send_axis_discrete(ms.primary_ptr, WL_POINTER_AXIS_VERTICAL_SCROLL, 1);
                wl_pointer_send_axis(ms.primary_ptr, 1540, WL_POINTER_AXIS_VERTICAL_SCROLL, wl_fixed_from_int(15));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_pointer_send_axis_discrete(ms.primary_ptr, WL_POINTER_AXIS_VERTICAL_SCROLL, 1);
                wl_pointer_send_axis(ms.primary_ptr, 1580, WL_POINTER_AXIS_VERTICAL_SCROLL, wl_fixed_from_int(15));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);

                write_flag(ms.sync_dir, "wheel_test.sent");
                stage = 7;
            } else if (stage == 7 && file_exists(ms.sync_dir, "wheel_mid.verified")) {
                /* Rapid 12ms continuous hi-res stream scrolling all the way to the end (160) */
                struct timespec fts = {0, 12 * 1000 * 1000L};
                for (int s = 0; s < 16; ++s) {
                    wl_pointer_send_axis(ms.primary_ptr, (uint32_t)(1700 + s * 12),
                                         WL_POINTER_AXIS_VERTICAL_SCROLL, wl_fixed_from_double(4.5));
                    wl_pointer_send_frame(ms.primary_ptr);
                    wl_display_flush_clients(ms.dpy);
                    nanosleep(&fts, NULL);
                }
                write_flag(ms.sync_dir, "wheel_to_end.sent");
                stage = 8;
            } else if (stage == 8 && file_exists(ms.sync_dir, "wheel_to_end.verified")) {
                /* Rapid 12ms continuous hi-res stream scrolling all the way back to start (0) */
                struct timespec fts = {0, 12 * 1000 * 1000L};
                for (int s = 0; s < 24; ++s) {
                    wl_pointer_send_axis(ms.primary_ptr, (uint32_t)(2000 + s * 12),
                                         WL_POINTER_AXIS_VERTICAL_SCROLL, wl_fixed_from_double(-4.5));
                    wl_pointer_send_frame(ms.primary_ptr);
                    wl_display_flush_clients(ms.dpy);
                    nanosleep(&fts, NULL);
                }
                write_flag(ms.sync_dir, "wheel_to_start.sent");
                stage = 9;
            } else if (stage == 9 && file_exists(ms.sync_dir, "menu_drag.ready") &&
                       ms.primary_ptr && ms.primary_surf) {
                char cpath[512];
                snprintf(cpath, sizeof(cpath), "%s/menu_drag.coords", ms.sync_dir);
                int mx = 24, my = 12, drag_y = 48;
                FILE *cf = fopen(cpath, "r");
                if (cf) {
                    (void)fscanf(cf, "%d %d %d", &mx, &my, &drag_y);
                    fclose(cf);
                }
                wl_pointer_send_enter(ms.primary_ptr, 800, ms.primary_surf,
                                      wl_fixed_from_int(mx), wl_fixed_from_int(my));
                wl_pointer_send_motion(ms.primary_ptr, 2400,
                                       wl_fixed_from_int(mx), wl_fixed_from_int(my));
                wl_pointer_send_button(ms.primary_ptr, 801, 2410, 0x110,
                                       WL_POINTER_BUTTON_STATE_PRESSED);
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                for (int k = 0; k < 12; ++k) {
                    wl_event_loop_dispatch(loop, 10);
                    wl_display_flush_clients(ms.dpy);
                }
                /* Implicit grab: pointer stays on ms.primary_surf while dragging into popup */
                wl_pointer_send_motion(ms.primary_ptr, 2460,
                                       wl_fixed_from_int(mx), wl_fixed_from_int(drag_y));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                for (int k = 0; k < 6; ++k) {
                    wl_event_loop_dispatch(loop, 10);
                    wl_display_flush_clients(ms.dpy);
                }
                wl_pointer_send_button(ms.primary_ptr, 802, 2500, 0x110,
                                       WL_POINTER_BUTTON_STATE_RELEASED);
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                write_flag(ms.sync_dir, "menu_drag.sent");
                stage = 10;
            } else if (stage == 10 && file_exists(ms.sync_dir, "cursor_touch.ready") &&
                       ms.primary_ptr && ms.primary_touch && ms.primary_surf) {
                char ctpath[512];
                snprintf(ctpath, sizeof(ctpath), "%s/cursor_touch.coords", ms.sync_dir);
                int tf_x = 60, tf_y = 24, btn_x = 420, btn_y = 140, pan_x = 90, pan_y = 260;
                FILE *ctf = fopen(ctpath, "r");
                if (ctf) {
                    (void)fscanf(ctf, "%d %d %d %d %d %d", &tf_x, &tf_y, &btn_x, &btn_y, &pan_x, &pan_y);
                    fclose(ctf);
                }
                /* 1) Hover text field -> TEXT cursor shape */
                wl_pointer_send_enter(ms.primary_ptr, 850, ms.primary_surf,
                                      wl_fixed_from_int(tf_x), wl_fixed_from_int(tf_y));
                wl_pointer_send_motion(ms.primary_ptr, 2600,
                                       wl_fixed_from_int(tf_x), wl_fixed_from_int(tf_y));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                for (int k = 0; k < 6; ++k) {
                    wl_event_loop_dispatch(loop, 8);
                    wl_display_flush_clients(ms.dpy);
                }
                /* 2) Hover hand button -> POINTER cursor shape */
                wl_pointer_send_motion(ms.primary_ptr, 2630,
                                       wl_fixed_from_int(btn_x), wl_fixed_from_int(btn_y));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                for (int k = 0; k < 6; ++k) {
                    wl_event_loop_dispatch(loop, 8);
                    wl_display_flush_clients(ms.dpy);
                }
                /* 3) Hover custom bitmap cursor widget -> custom wl_pointer.set_cursor */
                wl_pointer_send_motion(ms.primary_ptr, 2660,
                                       wl_fixed_from_int(pan_x), wl_fixed_from_int(pan_y));
                wl_pointer_send_frame(ms.primary_ptr);
                wl_display_flush_clients(ms.dpy);
                for (int k = 0; k < 6; ++k) {
                    wl_event_loop_dispatch(loop, 8);
                    wl_display_flush_clients(ms.dpy);
                }
                /* 4) 1-finger wl_touch tap on touch_btn */
                wl_touch_send_down(ms.primary_touch, 901, 2700, ms.primary_surf, 0,
                                   wl_fixed_from_int(btn_x), wl_fixed_from_int(btn_y));
                wl_touch_send_frame(ms.primary_touch);
                wl_display_flush_clients(ms.dpy);
                for (int k = 0; k < 4; ++k) {
                    wl_event_loop_dispatch(loop, 8);
                    wl_display_flush_clients(ms.dpy);
                }
                wl_touch_send_up(ms.primary_touch, 902, 2730, 0);
                wl_touch_send_frame(ms.primary_touch);
                wl_display_flush_clients(ms.dpy);
                for (int k = 0; k < 6; ++k) {
                    wl_event_loop_dispatch(loop, 8);
                    wl_display_flush_clients(ms.dpy);
                }
                /* 5) 2-finger wl_touch upward pan over sw_auto -> scrolls down smoothly */
                wl_touch_send_down(ms.primary_touch, 903, 2800, ms.primary_surf, 0,
                                   wl_fixed_from_int(pan_x - 16), wl_fixed_from_int(pan_y));
                wl_touch_send_down(ms.primary_touch, 904, 2802, ms.primary_surf, 1,
                                   wl_fixed_from_int(pan_x + 16), wl_fixed_from_int(pan_y));
                wl_touch_send_frame(ms.primary_touch);
                wl_display_flush_clients(ms.dpy);
                for (int s = 1; s <= 4; ++s) {
                    struct timespec tts = {0, 14 * 1000 * 1000L};
                    nanosleep(&tts, NULL);
                    wl_touch_send_motion(ms.primary_touch, (uint32_t)(2810 + s * 15), 0,
                                         wl_fixed_from_int(pan_x - 16), wl_fixed_from_int(pan_y - s * 15));
                    wl_touch_send_motion(ms.primary_touch, (uint32_t)(2810 + s * 15), 1,
                                         wl_fixed_from_int(pan_x + 16), wl_fixed_from_int(pan_y - s * 15));
                    wl_touch_send_frame(ms.primary_touch);
                    wl_display_flush_clients(ms.dpy);
                    wl_event_loop_dispatch(loop, 5);
                }
                wl_touch_send_up(ms.primary_touch, 905, 2900, 0);
                wl_touch_send_up(ms.primary_touch, 906, 2902, 1);
                wl_touch_send_frame(ms.primary_touch);
                wl_display_flush_clients(ms.dpy);
                write_flag(ms.sync_dir, "cursor_touch.sent");
                stage = 11;
            } else if (stage == 11 && file_exists(ms.sync_dir, "multi_out.step1_req")) {
                struct wl_resource *o1 = find_out_for_surf(ms.out1_res, ms.out1_count, ms.primary_surf);
                struct wl_resource *o2 = find_out_for_surf(ms.out2_res, ms.out2_count, ms.primary_surf);
                if (ms.primary_surf && o1 && o2) {
                    /* Enter OUT-1 (1x), then straddle OUT-2 (2x): must stay 1x while straddling */
                    wl_surface_send_enter(ms.primary_surf, o1);
                    wl_surface_send_enter(ms.primary_surf, o2);
                    wl_display_flush_clients(ms.dpy);
                }
                write_flag(ms.sync_dir, "multi_out.step1_sent");
                stage = 12;
            } else if (stage == 12 && file_exists(ms.sync_dir, "multi_out.step2_req")) {
                struct wl_resource *o1 = find_out_for_surf(ms.out1_res, ms.out1_count, ms.primary_surf);
                if (ms.primary_surf && o1) {
                    /* Leave OUT-1 -> exclusively on OUT-2 (2x) */
                    wl_surface_send_leave(ms.primary_surf, o1);
                    wl_display_flush_clients(ms.dpy);
                }
                write_flag(ms.sync_dir, "multi_out.step2_sent");
                stage = 13;
            } else if (stage == 13 && file_exists(ms.sync_dir, "multi_out.step3_req")) {
                struct wl_resource *o1 = find_out_for_surf(ms.out1_res, ms.out1_count, ms.primary_surf);
                if (ms.primary_surf && o1) {
                    /* Touch OUT-1 and retreat back onto OUT-2 -> must stay 2x */
                    wl_surface_send_enter(ms.primary_surf, o1);
                    wl_surface_send_leave(ms.primary_surf, o1);
                    wl_display_flush_clients(ms.dpy);
                }
                write_flag(ms.sync_dir, "multi_out.step3_sent");
                stage = 14;
            } else if (stage == 14 && file_exists(ms.sync_dir, "multi_out.step4_req")) {
                struct wl_resource *o1 = find_out_for_surf(ms.out1_res, ms.out1_count, ms.primary_surf);
                struct wl_resource *o2 = find_out_for_surf(ms.out2_res, ms.out2_count, ms.primary_surf);
                if (ms.primary_surf && o1 && o2) {
                    /* Cross back onto OUT-1 (1x) */
                    wl_surface_send_enter(ms.primary_surf, o1);
                    wl_surface_send_leave(ms.primary_surf, o2);
                    wl_display_flush_clients(ms.dpy);
                }
                write_flag(ms.sync_dir, "multi_out.step4_sent");
                stage = 15;
            } else if (stage == 15 && file_exists(ms.sync_dir, "menu_drag.verified")) {
                stage = 16;
                break;
            }
        }
        wl_display_destroy(ms.dpy);
        return (stage == 16 && ms.dnd_finished_seen) ? 0 : 1;
    }
    """
)


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def wait_file(path: Path, timeout_s: float = 10.0) -> None:
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        if path.exists():
            return
        time.sleep(0.03)
    raise RuntimeError(f"timed out waiting for {path}")


def assert_no_xwayland_in_runtime(runtime_dir: Path) -> None:
    rt_bytes = f"XDG_RUNTIME_DIR={runtime_dir}".encode()
    proc_root = Path("/proc")
    if not proc_root.is_dir():
        return
    for entry in proc_root.iterdir():
        if not entry.name.isdigit():
            continue
        try:
            comm = (entry / "comm").read_text(errors="ignore").strip()
            if "Xwayland" not in comm:
                continue
            environ = (entry / "environ").read_bytes()
            if rt_bytes in environ:
                raise RuntimeError(f"unexpected Xwayland process {entry.name} inside isolated Wayland runtime")
        except (FileNotFoundError, PermissionError, ProcessLookupError):
            continue


def pkg_config_flags(pkg: str) -> tuple[list[str], list[str]]:
    cf = subprocess.check_output(["pkg-config", "--cflags", pkg], text=True).split()
    lf = subprocess.check_output(["pkg-config", "--libs", pkg], text=True).split()
    return cf, lf


def pkg_config_var(pkg: str, var: str) -> str:
    return subprocess.check_output(["pkg-config", f"--variable={var}", pkg], text=True).strip()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--sanitizers", default="", help="Optional -fsanitize=LIST for check-wayland-backend client")
    args = parser.parse_args()

    if platform.system() == "Darwin":
        print("SKIP: native Wayland runtime backend targets Wayland hosts", file=sys.stderr)
        return 0

    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
        parser.error("--out must be a fresh directory separate from --prefix")

    output.mkdir(parents=True)
    (output / "tmp").mkdir()
    (output / "proto").mkdir()
    (output / "screenshots").mkdir()

    root = Path(__file__).resolve().parents[2]
    source = root / "tests/automated/check-wayland-backend.c"
    if (prefix / "lib/libXm.so").is_file():
        lib_dir = (prefix / "lib").resolve(strict=True)
        library = (lib_dir / "libXm.so").resolve(strict=True)
        inc_flags = ["-I" + str(prefix / "include")]
    else:
        lib_dir = (prefix / "lib/Xm/.libs").resolve(strict=True)
        library = (lib_dir / "libXm.so").resolve(strict=True)
        inc_flags = ["-I" + str(prefix / "lib"), "-I" + str(root / "lib")]

    base_env = dict(
        os.environ,
        TMPDIR=str(output / "tmp"),
        XDG_CACHE_HOME=str(output / "tmp"),
        PYTHONDONTWRITEBYTECODE="1",
        LC_ALL="C.UTF-8",
        LD_LIBRARY_PATH=str(lib_dir),
    )
    for var in ("DISPLAY", "WAYLAND_DISPLAY", "CK_MOTIF_BACKEND", "CK_MOTIF_WAYLAND_ACTIVE",
                "XENVIRONMENT", "XAPPLRESDIR", "LD_PRELOAD", "LD_AUDIT"):
        base_env.pop(var, None)

    receipt: dict = {
        "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "library": str(library),
        "library_sha256": sha256(library),
        "source": str(source),
        "source_sha256": sha256(source),
        "screenshots": [],
        "steps": [],
        "success": False,
    }
    started = time.monotonic()
    cc = os.environ.get("CC", "cc")

    try:
        # 1. Compile Motif client binary
        client_bin = output / "check-wayland-backend"
        xrandr_cflags: list[str] = []
        xrandr_libs: list[str] = []
        if subprocess.run(["pkg-config", "--exists", "xrandr"], env=base_env).returncode == 0:
            xr_cf, xr_lf = pkg_config_flags("xrandr")
            xrandr_cflags = ["-DHAVE_XRANDR_TEST=1", *xr_cf]
            xrandr_libs = xr_lf
        wlc_cflags: list[str] = []
        wlc_libs: list[str] = []
        if subprocess.run(["pkg-config", "--exists", "wayland-client"], env=base_env).returncode == 0:
            wc_cf, wc_lf = pkg_config_flags("wayland-client")
            wlc_cflags = ["-DHAVE_WAYLAND_CLIENT_TEST=1", *wc_cf]
            wlc_libs = wc_lf
        san_flags: list[str] = []
        if args.sanitizers:
            san_flags = [f"-fsanitize={args.sanitizers}", "-fno-omit-frame-pointer", "-g"]
            if "undefined" in args.sanitizers:
                san_flags.append("-fno-sanitize-recover=undefined")
        compile_cmd = [
            cc, "-std=c11", "-Wall", "-Wextra", "-Werror", "-O1" if args.sanitizers else "-O2",
            *san_flags, *inc_flags, *xrandr_cflags, *wlc_cflags,
            "-L" + str(lib_dir), "-Wl,-rpath," + str(lib_dir),
            str(source), "-lXm", "-lXt", "-lX11", "-lXrender", *xrandr_libs, *wlc_libs, "-lm", "-o", str(client_bin),
        ]
        cres = subprocess.run(compile_cmd, env=base_env, capture_output=True, text=True, timeout=60)
        (output / "compile-client.log").write_text(cres.stdout + cres.stderr)
        receipt["steps"].append({"name": "compile_client", "exit": cres.returncode, "sanitizers": args.sanitizers})
        if cres.returncode != 0:
            print(cres.stdout + cres.stderr, file=sys.stderr)
            return 1

        client_san_env: dict[str, str] = {}
        if args.sanitizers:
            client_san_env["ASAN_OPTIONS"] = "detect_leaks=0:halt_on_error=1:print_stacktrace=1"
            client_san_env["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
            client_san_env["TSAN_OPTIONS"] = "halt_on_error=1:second_deadlock_stack=1"

        # 2. Generate & compile deterministic Wayland IME / DnD / Clipboard protocol server
        scanner = shutil.which("wayland-scanner")
        if not scanner:
            raise RuntimeError("wayland-scanner not found")
        wp_dir = Path(pkg_config_var("wayland-protocols", "pkgdatadir"))
        xdg_xml = wp_dir / "stable/xdg-shell/xdg-shell.xml"
        act_xml = wp_dir / "staging/xdg-activation/xdg-activation-v1.xml"
        ti_xml = wp_dir / "unstable/text-input/text-input-unstable-v3.xml"
        prim_xml = wp_dir / "unstable/primary-selection/primary-selection-unstable-v1.xml"
        cshape_xml = wp_dir / "staging/cursor-shape/cursor-shape-v1.xml"
        tab_xml = wp_dir / "unstable/tablet/tablet-unstable-v2.xml"
        dmabuf_xml = wp_dir / "stable/linux-dmabuf/linux-dmabuf-v1.xml"
        if not dmabuf_xml.is_file():
            dmabuf_xml = wp_dir / "unstable/linux-dmabuf/linux-dmabuf-unstable-v1.xml"
        xdlg_xml = wp_dir / "staging/xdg-dialog/xdg-dialog-v1.xml"
        tear_xml = wp_dir / "staging/tearing-control/tearing-control-v1.xml"
        fifo_xml = wp_dir / "staging/fifo/fifo-v1.xml"

        proto_dir = output / "proto"
        subprocess.run([scanner, "server-header", str(xdg_xml), str(proto_dir / "xdg-shell-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(xdg_xml), str(proto_dir / "xdg-shell-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(act_xml), str(proto_dir / "xdg-activation-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(act_xml), str(proto_dir / "xdg-activation-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(ti_xml), str(proto_dir / "text-input-v3-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(ti_xml), str(proto_dir / "text-input-v3-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(prim_xml), str(proto_dir / "primary-selection-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(prim_xml), str(proto_dir / "primary-selection-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(tab_xml), str(proto_dir / "tablet-v2-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(tab_xml), str(proto_dir / "tablet-v2-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(cshape_xml), str(proto_dir / "cursor-shape-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(cshape_xml), str(proto_dir / "cursor-shape-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(dmabuf_xml), str(proto_dir / "linux-dmabuf-v1-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(dmabuf_xml), str(proto_dir / "linux-dmabuf-v1-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(xdlg_xml), str(proto_dir / "xdg-dialog-v1-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(xdlg_xml), str(proto_dir / "xdg-dialog-v1-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(tear_xml), str(proto_dir / "tearing-control-v1-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(tear_xml), str(proto_dir / "tearing-control-v1-code.c")], check=True)
        subprocess.run([scanner, "server-header", str(fifo_xml), str(proto_dir / "fifo-v1-server.h")], check=True)
        subprocess.run([scanner, "private-code", str(fifo_xml), str(proto_dir / "fifo-v1-code.c")], check=True)

        srv_c = proto_dir / "mini-compositor.c"
        srv_c.write_text(MINI_COMPOSITOR_C)
        wls_cf, wls_lf = pkg_config_flags("wayland-server")
        srv_bin = output / "mini-compositor"
        sres = subprocess.run(
            [
                cc, "-std=c11", "-Wall", "-Wextra", "-O2",
                "-I" + str(proto_dir), *wls_cf,
                str(srv_c),
                str(proto_dir / "xdg-shell-code.c"),
                str(proto_dir / "xdg-activation-code.c"),
                str(proto_dir / "text-input-v3-code.c"),
                str(proto_dir / "primary-selection-code.c"),
                str(proto_dir / "tablet-v2-code.c"),
                str(proto_dir / "cursor-shape-code.c"),
                str(proto_dir / "linux-dmabuf-v1-code.c"),
                str(proto_dir / "xdg-dialog-v1-code.c"),
                str(proto_dir / "tearing-control-v1-code.c"),
                str(proto_dir / "fifo-v1-code.c"),
                *wls_lf, "-o", str(srv_bin),
            ],
            env=base_env,
            capture_output=True,
            text=True,
            timeout=60,
        )
        (output / "compile-mini-srv.log").write_text(sres.stdout + sres.stderr)
        receipt["steps"].append({"name": "compile_mini_compositor", "exit": sres.returncode})
        if sres.returncode != 0:
            print(sres.stdout + sres.stderr, file=sys.stderr)
            return 1

        # 3. Run Phase B: deterministic zwp_text_input_v3 IME, wl_data_device DnD & cross-process clipboard
        ime_rt = output / "rt-ime"
        ime_sync = output / "sync-ime"
        ime_rt.mkdir(mode=0o700)
        ime_sync.mkdir()
        ime_srv_env = dict(
            base_env,
            XDG_RUNTIME_DIR=str(ime_rt),
            WAYLAND_DISPLAY="wayland-ime0",
        )
        ime_cli_env = dict(
            ime_srv_env,
            XDG_ACTIVATION_TOKEN="startup-tok-initial-01",
            **client_san_env,
        )

        srv_proc = subprocess.Popen(
            [str(srv_bin), "wayland-ime0", str(ime_sync)],
            env=ime_srv_env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        try:
            wait_file(ime_sync / "srv.ready", 5.0)
            cli_res = subprocess.run(
                [str(client_bin), "--ime-dnd-check", str(ime_sync)],
                env=ime_cli_env,
                capture_output=True,
                text=True,
                timeout=60 if args.sanitizers else 30,
            )
            if cli_res.returncode != 0 and srv_proc.poll() is None:
                srv_proc.terminate()
            srv_out, _ = srv_proc.communicate(timeout=10)
            (output / "ime-dnd.log").write_text(cli_res.stdout + cli_res.stderr + "\n" + srv_out)
            receipt["steps"].append(
                {"name": "ime_dnd_clipboard_check", "client_exit": cli_res.returncode, "server_exit": srv_proc.returncode}
            )
            if cli_res.returncode != 0 or srv_proc.returncode != 0:
                print(cli_res.stdout + cli_res.stderr + "\n" + srv_out, file=sys.stderr)
                return 1
        finally:
            if srv_proc.poll() is None:
                srv_proc.kill()
                srv_proc.wait()

        # 4. Run Phase A: full headless Wayland compositor qualification (labwc, xwayland=no)
        labwc = shutil.which("labwc")
        grim = shutil.which("grim")
        wlr_randr = shutil.which("wlr-randr")
        if not labwc:
            raise RuntimeError("labwc headless compositor is required for full Wayland visual qualification")

        wl_rt = output / "rt-wl"
        wl_cfg = output / "labwc-cfg"
        wl_sync = output / "sync-wl"
        wl_rt.mkdir(mode=0o700)
        wl_cfg.mkdir()
        wl_sync.mkdir()
        (wl_cfg / "rc.xml").write_text(
            '<?xml version="1.0"?>\n'
            "<labwc_config><core><xwayland>no</xwayland></core></labwc_config>\n"
        )

        comp_env = dict(
            base_env,
            XDG_RUNTIME_DIR=str(wl_rt),
            WLR_BACKENDS="headless",
            WLR_RENDERER="pixman",
            WLR_HEADLESS_OUTPUTS="1",
        )
        comp_log_f = (output / "labwc.log").open("w")
        comp_proc = subprocess.Popen(
            [labwc, "-C", str(wl_cfg)],
            env=comp_env,
            stdout=comp_log_f,
            stderr=subprocess.STDOUT,
        )
        try:
            wl_sock_name = None
            deadline = time.monotonic() + 10.0
            while time.monotonic() < deadline:
                socks = sorted(p.name for p in wl_rt.glob("wayland-*") if not p.name.endswith(".lock"))
                if socks:
                    wl_sock_name = socks[0]
                    break
                time.sleep(0.05)
            if not wl_sock_name:
                raise RuntimeError("labwc did not publish a wayland-* socket")

            assert_no_xwayland_in_runtime(wl_rt)
            app_env = dict(base_env, XDG_RUNTIME_DIR=str(wl_rt), WAYLAND_DISPLAY=wl_sock_name)
            app_cli_env = dict(app_env, **client_san_env)

            app_proc = subprocess.Popen(
                [str(client_bin), "--compositor-check", str(wl_sync)],
                env=app_cli_env,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
            )
            try:
                stages = [
                    ("stage_classic", "classic.png"),
                    ("stage_popup", "popup_menu.png"),
                    ("stage_modern_light", "modern_light.png"),
                    ("stage_modern_dark", "modern_dark.png"),
                ]
                for prefix_name, shot_name in stages:
                    wait_file(wl_sync / f"{prefix_name}.ready", 12.0)
                    assert_no_xwayland_in_runtime(wl_rt)
                    if grim:
                        shot_path = output / "screenshots" / shot_name
                        subprocess.run([grim, str(shot_path)], env=app_env, check=True, timeout=10)
                        receipt["screenshots"].append(str(shot_path))
                    (wl_sync / f"{prefix_name}.done").write_text("1\n")

                wait_file(wl_sync / "stage_scale2x.request", 12.0)
                if wlr_randr:
                    subprocess.run(
                        [wlr_randr, "--output", "HEADLESS-1", "--scale", "2"],
                        env=app_env,
                        capture_output=True,
                        timeout=10,
                    )
                (wl_sync / "stage_scale2x.applied").write_text("1\n")

                wait_file(wl_sync / "stage_scale2x.ready", 12.0)
                if grim:
                    shot_2x = output / "screenshots" / "scale_2x.png"
                    subprocess.run([grim, str(shot_2x)], env=app_env, check=True, timeout=10)
                    receipt["screenshots"].append(str(shot_2x))
                (wl_sync / "stage_scale2x.done").write_text("1\n")

                app_out, _ = app_proc.communicate(timeout=15)
                (output / "compositor-check.log").write_text(app_out)
                receipt["steps"].append({"name": "compositor_check", "exit": app_proc.returncode})
                if app_proc.returncode != 0:
                    print(app_out, file=sys.stderr)
                    return 1
            finally:
                if app_proc.poll() is None:
                    app_proc.kill()
                    app_proc.wait()
        finally:
            if comp_proc.poll() is None:
                comp_proc.terminate()
                try:
                    comp_proc.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    comp_proc.kill()
                    comp_proc.wait()
            comp_log_f.close()

        if sha256(library) != receipt["library_sha256"]:
            raise RuntimeError("libXm changed during qualification")

        receipt["success"] = True
        print(
            "PASS Wayland native runtime backend "
            "(XWayland-free shells, popups, XRender/FreeType, clipboard, DnD, zwp_text_input_v3, scale reflow)"
        )
        return 0
    finally:
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["seconds"] = round(time.monotonic() - started, 3)
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    raise SystemExit(main())
