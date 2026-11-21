/*
 * Pixman / wl_shm rasterizer, FreeType/Fontconfig core X11 font engine, and
 * XRender 0.11 extension implementation for the libXm Wayland backend.
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline uint16_t rd_u16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static inline int16_t  rd_i16(const uint8_t *p) { int16_t v;  memcpy(&v, p, 2); return v; }
static inline uint32_t rd_u32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline int32_t  rd_i32(const uint8_t *p) { int32_t v;  memcpy(&v, p, 4); return v; }
static inline void     wr_u16(uint8_t *p, uint16_t v) { memcpy(p, &v, 2); }
static inline void     wr_u32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }

void
_XmWlRenderInit(XmWlServer *srv)
{
    if (!srv) return;
    FcInit();
    if (FT_Init_FreeType(&srv->ft_lib) == 0)
        srv->ft_inited = true;
}

void
_XmWlRenderDestroy(XmWlServer *srv)
{
    if (!srv) return;
    if (srv->ft_inited) {
        FT_Done_FreeType(srv->ft_lib);
        srv->ft_inited = false;
    }
}

void
_XmWlFreePixmapData(XmWlPixmap *pix)
{
    if (!pix) return;
    if (pix->img) {
        pixman_image_unref(pix->img);
        pix->img = NULL;
    }
    free(pix->bits);
    pix->bits = NULL;
}

void
_XmWlFreeFontData(XmWlFont *font)
{
    if (!font) return;
    if (font->face) {
        FT_Done_Face(font->face);
        font->face = NULL;
    }
}

void
_XmWlFreePictureData(XmWlPicture *pic)
{
    if (!pic) return;
    if (pic->source_img) {
        pixman_image_unref(pic->source_img);
        pic->source_img = NULL;
    }
    free(pic->clip_rects);
    pic->clip_rects = NULL;
}

void
_XmWlFreeGlyphSetData(XmWlGlyphSet *gs)
{
    if (!gs) return;
    for (int i = 0; i < 64; ++i) {
        XmWlGlyph *g = gs->buckets[i];
        while (g) {
            XmWlGlyph *next = g->next;
            if (g->img) pixman_image_unref(g->img);
            free(g->bits);
            free(g);
            g = next;
        }
        gs->buckets[i] = NULL;
    }
}

/* ---------- Target Surface + Visible Clip Region Resolution ---------- */

typedef struct {
    uint32_t *pixels;
    pixman_image_t *img;
    int width;
    int height;
    int stride_px;
    uint8_t depth;
    int dx;
    int dy;
    XmWlWindow *top_win;
    pixman_region32_t clip;
} DrawTarget;

static bool
resolve_draw_target(XmWlServer *srv, uint32_t drawable_id, const XmWlGC *gc, DrawTarget *out)
{
    memset(out, 0, sizeof(*out));
    pixman_region32_init(&out->clip);

    XmWlPixmap *pix = _XmWlFindPixmap(srv, drawable_id);
    if (pix) {
        if (!pix->bits || !pix->img) {
            pixman_region32_fini(&out->clip);
            return false;
        }
        out->pixels = pix->bits;
        out->img = pix->img;
        out->width = pix->width;
        out->height = pix->height;
        out->stride_px = pix->stride / 4;
        out->depth = pix->depth;
        pixman_region32_init_rect(&out->clip, 0, 0, pix->width, pix->height);
        if (gc && gc->has_clip_rects) {
            pixman_region32_t gc_reg;
            pixman_region32_init(&gc_reg);
            for (size_t i = 0; i < gc->clip_rect_count; ++i) {
                pixman_region32_union_rect(
                    &gc_reg, &gc_reg,
                    gc->clip_x_origin + gc->clip_rects[i].x,
                    gc->clip_y_origin + gc->clip_rects[i].y,
                    gc->clip_rects[i].width, gc->clip_rects[i].height);
            }
            pixman_region32_intersect(&out->clip, &out->clip, &gc_reg);
            pixman_region32_fini(&gc_reg);
        }
        return true;
    }

    XmWlWindow *win = _XmWlFindWindow(srv, drawable_id);
    if (!win || win == srv->root || win->win_class != 1) {
        pixman_region32_fini(&out->clip);
        return false;
    }

    int dx = 0, dy = 0;
    XmWlWindow *top = _XmWlEnclosingTopLevel(srv, win, &dx, &dy);
    if (!top) {
        pixman_region32_fini(&out->clip);
        return false;
    }
    _XmWlEnsureTopCanvas(srv, top);
    if (!top->canvas || !top->canvas_bits) {
        pixman_region32_fini(&out->clip);
        return false;
    }

    /* Check ancestor mapped status and intersect ancestor bounds */
    int cx1 = dx, cy1 = dy;
    int cx2 = dx + (int)win->width;
    int cy2 = dy + (int)win->height;
    XmWlWindow *cur = win;
    int cur_x = dx, cur_y = dy;
    while (cur && cur != top) {
        if (!cur->mapped) {
            pixman_region32_fini(&out->clip);
            return false;
        }
        int px = cur_x - cur->x - (int)cur->border_width;
        int py = cur_y - cur->y - (int)cur->border_width;
        XmWlWindow *par = cur->parent;
        if (par) {
            if (px > cx1) cx1 = px;
            if (py > cy1) cy1 = py;
            if (px + (int)par->width < cx2) cx2 = px + (int)par->width;
            if (py + (int)par->height < cy2) cy2 = py + (int)par->height;
        }
        cur_x = px;
        cur_y = py;
        cur = par;
    }
    if (cx1 < 0) cx1 = 0;
    if (cy1 < 0) cy1 = 0;
    if (cx2 > top->canvas_w) cx2 = top->canvas_w;
    if (cy2 > top->canvas_h) cy2 = top->canvas_h;
    if (cx2 <= cx1 || cy2 <= cy1) {
        pixman_region32_fini(&out->clip);
        return false;
    }

    pixman_region32_fini(&out->clip);
    pixman_region32_init_rect(&out->clip, cx1, cy1, (unsigned)(cx2 - cx1), (unsigned)(cy2 - cy1));

    /* ClipByChildren: subtract mapped InputOutput children of win AND higher-z siblings */
    if (!gc || gc->subwindow_mode == 0) {
        for (XmWlWindow *ch = win->first_child; ch; ch = ch->next_sibling) {
            if (ch->mapped && ch->win_class == 1) {
                pixman_region32_t sub;
                int bw2 = (int)ch->border_width * 2;
                pixman_region32_init_rect(&sub, dx + ch->x, dy + ch->y,
                                          (unsigned)((int)ch->width + bw2),
                                          (unsigned)((int)ch->height + bw2));
                pixman_region32_subtract(&out->clip, &out->clip, &sub);
                pixman_region32_fini(&sub);
            }
        }
        XmWlWindow *anc = win;
        int ax = dx, ay = dy;
        while (anc && anc != top && anc->parent) {
            int pax = ax - anc->x - (int)anc->border_width;
            int pay = ay - anc->y - (int)anc->border_width;
            for (XmWlWindow *sib = anc->next_sibling; sib; sib = sib->next_sibling) {
                if (sib->mapped && sib->win_class == 1) {
                    pixman_region32_t sub;
                    int bw2 = (int)sib->border_width * 2;
                    pixman_region32_init_rect(&sub, pax + sib->x, pay + sib->y,
                                              (unsigned)((int)sib->width + bw2),
                                              (unsigned)((int)sib->height + bw2));
                    pixman_region32_subtract(&out->clip, &out->clip, &sub);
                    pixman_region32_fini(&sub);
                }
            }
            ax = pax;
            ay = pay;
            anc = anc->parent;
        }
    }

    if (gc && gc->has_clip_rects) {
        pixman_region32_t gc_reg;
        pixman_region32_init(&gc_reg);
        for (size_t i = 0; i < gc->clip_rect_count; ++i) {
            pixman_region32_union_rect(
                &gc_reg, &gc_reg,
                dx + gc->clip_x_origin + gc->clip_rects[i].x,
                dy + gc->clip_y_origin + gc->clip_rects[i].y,
                gc->clip_rects[i].width, gc->clip_rects[i].height);
        }
        pixman_region32_intersect(&out->clip, &out->clip, &gc_reg);
        pixman_region32_fini(&gc_reg);
    }

    out->pixels = top->canvas_bits;
    out->img = top->canvas;
    out->width = top->canvas_w;
    out->height = top->canvas_h;
    out->stride_px = top->canvas_stride / 4;
    out->depth = top->depth;
    out->dx = dx;
    out->dy = dy;
    out->top_win = top;
    return pixman_region32_not_empty(&out->clip);
}

static inline uint32_t
format_gc_pixel(uint8_t depth, uint32_t raw)
{
    if (depth == 32) return raw;
    if (depth == 1) return (raw & 1u) ? 1u : 0u;
    if (depth == 8) return raw & 0xffu;
    return 0xff000000u | (raw & 0x00ffffffu);
}

static inline uint32_t
apply_rop(uint8_t func, uint32_t src, uint32_t dst, uint8_t depth)
{
    uint32_t res = src;
    switch (func) {
    case 0:  res = 0; break;                  /* GXclear */
    case 1:  res = src & dst; break;          /* GXand */
    case 2:  res = src & ~dst; break;         /* GXandReverse */
    case 3:  res = src; break;                /* GXcopy */
    case 4:  res = (~src) & dst; break;       /* GXandInverted */
    case 5:  res = dst; break;                /* GXnoop */
    case 6:  res = src ^ dst; break;          /* GXxor */
    case 7:  res = src | dst; break;          /* GXor */
    case 8:  res = ~(src | dst); break;       /* GXnor */
    case 9:  res = ~(src ^ dst); break;       /* GXequiv */
    case 10: res = ~dst; break;               /* GXinvert */
    case 11: res = src | ~dst; break;         /* GXorReverse */
    case 12: res = ~src; break;               /* GXcopyInverted */
    case 13: res = (~src) | dst; break;       /* GXorInverted */
    case 14: res = ~(src & dst); break;       /* GXnand */
    case 15: res = 0xffffffffu; break;        /* GXset */
    default: res = src; break;
    }
    if (depth == 24) res = 0xff000000u | (res & 0x00ffffffu);
    else if (depth == 1) res &= 1u;
    else if (depth == 8) res &= 0xffu;
    return res;
}

static inline bool
clip_mask_allows(XmWlServer *srv, const XmWlGC *gc, int local_x, int local_y)
{
    if (!gc || gc->clip_mask_pixmap == 0) return true;
    XmWlPixmap *cm = _XmWlFindPixmap(srv, gc->clip_mask_pixmap);
    if (!cm || !cm->bits) return true;
    int mx = local_x - gc->clip_x_origin;
    int my = local_y - gc->clip_y_origin;
    if (mx < 0 || my < 0 || mx >= (int)cm->width || my >= (int)cm->height)
        return false;
    uint32_t v = cm->bits[(size_t)my * (size_t)(cm->stride / 4) + (size_t)mx];
    return (v & 0x01ffffffu) != 0 || (v >> 24) != 0;
}

static inline void
put_clipped_pixel(XmWlServer *srv, DrawTarget *dt, const XmWlGC *gc, int tx, int ty, uint32_t src_px)
{
    if (tx < 0 || ty < 0 || tx >= dt->width || ty >= dt->height) return;
    if (!pixman_region32_contains_point(&dt->clip, tx, ty, NULL)) return;
    if (!clip_mask_allows(srv, gc, tx - dt->dx, ty - dt->dy)) return;

    uint32_t *dst_p = &dt->pixels[(size_t)ty * (size_t)dt->stride_px + (size_t)tx];
    if (!gc || gc->function == 3) {
        *dst_p = src_px;
    } else {
        *dst_p = apply_rop(gc->function, src_px, *dst_p, dt->depth);
    }
}

static void
fill_box_gc(XmWlServer *srv, DrawTarget *dt, const XmWlGC *gc, int x1, int y1, int x2, int y2)
{
    if (x2 <= x1 || y2 <= y1) return;
    pixman_region32_t box_reg;
    pixman_region32_init_rect(&box_reg, x1, y1, (unsigned)(x2 - x1), (unsigned)(y2 - y1));
    pixman_region32_intersect(&box_reg, &box_reg, &dt->clip);

    int nboxes = 0;
    pixman_box32_t *boxes = pixman_region32_rectangles(&box_reg, &nboxes);
    if (nboxes <= 0) {
        pixman_region32_fini(&box_reg);
        return;
    }

    uint32_t fg_px = format_gc_pixel(dt->depth, gc ? gc->fg : 0);
    uint32_t bg_px = format_gc_pixel(dt->depth, gc ? gc->bg : 0);
    uint8_t fill_style = gc ? gc->fill_style : 0;
    uint8_t func = gc ? gc->function : 3;

    XmWlPixmap *tile = (fill_style == 1 && gc) ? _XmWlFindPixmap(srv, gc->tile_pixmap) : NULL;
    XmWlPixmap *stip = ((fill_style == 2 || fill_style == 3) && gc) ? _XmWlFindPixmap(srv, gc->stipple_pixmap) : NULL;

    for (int bi = 0; bi < nboxes; ++bi) {
        int bx1 = boxes[bi].x1, by1 = boxes[bi].y1;
        int bx2 = boxes[bi].x2, by2 = boxes[bi].y2;
        if (dt->top_win)
            _XmWlDamageTop(dt->top_win, bx1, by1, bx2 - bx1, by2 - by1);

        if (fill_style == 0 && func == 3 && (!gc || gc->clip_mask_pixmap == 0)) {
            for (int py = by1; py < by2; ++py) {
                uint32_t *row = dt->pixels + (size_t)py * (size_t)dt->stride_px + (size_t)bx1;
                for (int px = bx1; px < bx2; ++px)
                    *row++ = fg_px;
            }
            continue;
        }

        for (int py = by1; py < by2; ++py) {
            for (int px = bx1; px < bx2; ++px) {
                if (!clip_mask_allows(srv, gc, px - dt->dx, py - dt->dy))
                    continue;
                uint32_t spx = fg_px;
                if (tile && tile->bits && tile->width > 0 && tile->height > 0) {
                    int tx = (px - dt->dx - gc->ts_x_origin) % (int)tile->width;
                    int ty = (py - dt->dy - gc->ts_y_origin) % (int)tile->height;
                    if (tx < 0) tx += tile->width;
                    if (ty < 0) ty += tile->height;
                    spx = format_gc_pixel(dt->depth, tile->bits[(size_t)ty * (size_t)(tile->stride / 4) + (size_t)tx]);
                } else if (stip && stip->bits && stip->width > 0 && stip->height > 0) {
                    int sx = (px - dt->dx - gc->ts_x_origin) % (int)stip->width;
                    int sy = (py - dt->dy - gc->ts_y_origin) % (int)stip->height;
                    if (sx < 0) sx += stip->width;
                    if (sy < 0) sy += stip->height;
                    uint32_t sv = stip->bits[(size_t)sy * (size_t)(stip->stride / 4) + (size_t)sx];
                    bool on = (sv & 1u) != 0 || (sv & 0x00ffffffu) != 0;
                    if (!on) {
                        if (fill_style == 2) continue; /* FillStippled: transparent 0-bits */
                        spx = bg_px;                   /* FillOpaqueStippled */
                    }
                }
                uint32_t *dst_p = &dt->pixels[(size_t)py * (size_t)dt->stride_px + (size_t)px];
                *dst_p = (func == 3) ? spx : apply_rop(func, spx, *dst_p, dt->depth);
            }
        }
    }
    pixman_region32_fini(&box_reg);
}

void
_XmWlPaintWindowBackground(XmWlServer *srv, XmWlWindow *win, int x, int y, int w, int h)
{
    if (!srv || !win || win->win_class != 1) return;
    if (w <= 0) w = win->width;
    if (h <= 0) h = win->height;
    if (w <= 0 || h <= 0) return;

    XmWlGC tmp_gc;
    memset(&tmp_gc, 0, sizeof(tmp_gc));
    tmp_gc.function = 3;
    tmp_gc.subwindow_mode = 0;
    if (win->bg_pixmap > 1) {
        tmp_gc.fill_style = 1;
        tmp_gc.tile_pixmap = win->bg_pixmap;
    } else if (win->has_bg_pixel) {
        tmp_gc.fill_style = 0;
        tmp_gc.fg = win->bg_pixel;
    } else {
        return; /* ParentRelative / None */
    }

    DrawTarget dt;
    if (!resolve_draw_target(srv, win->id, &tmp_gc, &dt))
        return;
    fill_box_gc(srv, &dt, &tmp_gc, dt.dx + x, dt.dy + y, dt.dx + x + w, dt.dy + y + h);
    pixman_region32_fini(&dt.clip);
}

/* ---------- Core X11 Drawing Operations ---------- */

void
_XmWlRenderPolyFillRect(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, const uint8_t *rects, size_t count)
{
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;
    for (size_t i = 0; i < count; ++i) {
        int16_t rx = rd_i16(rects + i * 8 + 0);
        int16_t ry = rd_i16(rects + i * 8 + 2);
        uint16_t rw = rd_u16(rects + i * 8 + 4);
        uint16_t rh = rd_u16(rects + i * 8 + 6);
        fill_box_gc(srv, &dt, gc, dt.dx + rx, dt.dy + ry, dt.dx + rx + (int)rw, dt.dy + ry + (int)rh);
    }
    pixman_region32_fini(&dt.clip);
}

static void
draw_line_gc(XmWlServer *srv, DrawTarget *dt, const XmWlGC *gc, int x0, int y0, int x1, int y1)
{
    int lw = (gc && gc->line_width > 1) ? gc->line_width : 1;
    uint32_t fg_px = format_gc_pixel(dt->depth, gc ? gc->fg : 0);
    uint32_t bg_px = format_gc_pixel(dt->depth, gc ? gc->bg : 0);
    bool dashed = gc && (gc->line_style == 1 || gc->line_style == 2);
    int dash_len = (gc && gc->dash_count > 0 && gc->dashes[0] > 0) ? gc->dashes[0] : 4;

    if (y0 == y1 && !dashed) {
        int xa = x0 < x1 ? x0 : x1;
        int xb = x0 < x1 ? x1 : x0;
        fill_box_gc(srv, dt, gc, xa, y0 - lw / 2, xb + 1, y0 - lw / 2 + lw);
        return;
    }
    if (x0 == x1 && !dashed) {
        int ya = y0 < y1 ? y0 : y1;
        int yb = y0 < y1 ? y1 : y0;
        fill_box_gc(srv, dt, gc, x0 - lw / 2, ya, x0 - lw / 2 + lw, yb + 1);
        return;
    }

    int adx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int ady = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = adx + ady;
    int step = gc ? gc->dash_offset : 0;
    int min_x = (x0 < x1 ? x0 : x1) - lw, max_x = (x0 > x1 ? x0 : x1) + lw + 1;
    int min_y = (y0 < y1 ? y0 : y1) - lw, max_y = (y0 > y1 ? y0 : y1) + lw + 1;

    for (;;) {
        bool draw_on = !dashed || (((step / dash_len) & 1) == 0);
        if (draw_on || (gc && gc->line_style == 2)) {
            uint32_t px = draw_on ? fg_px : bg_px;
            int half = lw / 2;
            for (int oy = -half; oy < -half + lw; ++oy) {
                for (int ox = -half; ox < -half + lw; ++ox) {
                    put_clipped_pixel(srv, dt, gc, x0 + ox, y0 + oy, px);
                }
            }
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= ady) { err += ady; x0 += sx; }
        if (e2 <= adx) { err += adx; y0 += sy; }
        ++step;
    }
    if (dt->top_win)
        _XmWlDamageTop(dt->top_win, min_x, min_y, max_x - min_x, max_y - min_y);
}

void
_XmWlRenderPolyRect(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, const uint8_t *rects, size_t count)
{
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;
    for (size_t i = 0; i < count; ++i) {
        int x = dt.dx + rd_i16(rects + i * 8 + 0);
        int y = dt.dy + rd_i16(rects + i * 8 + 2);
        int w = rd_u16(rects + i * 8 + 4);
        int h = rd_u16(rects + i * 8 + 6);
        draw_line_gc(srv, &dt, gc, x, y, x + w, y);
        draw_line_gc(srv, &dt, gc, x + w, y, x + w, y + h);
        draw_line_gc(srv, &dt, gc, x + w, y + h, x, y + h);
        draw_line_gc(srv, &dt, gc, x, y + h, x, y);
    }
    pixman_region32_fini(&dt.clip);
}

void
_XmWlRenderPolySegment(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, const uint8_t *segs, size_t count)
{
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;
    for (size_t i = 0; i < count; ++i) {
        int x1 = dt.dx + rd_i16(segs + i * 8 + 0);
        int y1 = dt.dy + rd_i16(segs + i * 8 + 2);
        int x2 = dt.dx + rd_i16(segs + i * 8 + 4);
        int y2 = dt.dy + rd_i16(segs + i * 8 + 6);
        draw_line_gc(srv, &dt, gc, x1, y1, x2, y2);
    }
    pixman_region32_fini(&dt.clip);
}

void
_XmWlRenderPolyLine(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, uint8_t coord_mode, const uint8_t *pts, size_t count)
{
    if (count < 2) return;
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;
    int px = dt.dx + rd_i16(pts + 0);
    int py = dt.dy + rd_i16(pts + 2);
    for (size_t i = 1; i < count; ++i) {
        int nx = rd_i16(pts + i * 4 + 0);
        int ny = rd_i16(pts + i * 4 + 2);
        if (coord_mode == 1) { nx += px; ny += py; }
        else { nx += dt.dx; ny += dt.dy; }
        draw_line_gc(srv, &dt, gc, px, py, nx, ny);
        px = nx;
        py = ny;
    }
    pixman_region32_fini(&dt.clip);
}

void
_XmWlRenderPolyPoint(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, uint8_t coord_mode, const uint8_t *pts, size_t count)
{
    if (count == 0) return;
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;
    uint32_t fg_px = format_gc_pixel(dt.depth, gc ? gc->fg : 0);
    int px = 0, py = 0;
    for (size_t i = 0; i < count; ++i) {
        int nx = rd_i16(pts + i * 4 + 0);
        int ny = rd_i16(pts + i * 4 + 2);
        if (i == 0 || coord_mode == 0) { px = dt.dx + nx; py = dt.dy + ny; }
        else { px += nx; py += ny; }
        put_clipped_pixel(srv, &dt, gc, px, py, fg_px);
        if (dt.top_win) _XmWlDamageTop(dt.top_win, px, py, 1, 1);
    }
    pixman_region32_fini(&dt.clip);
}

static int cmp_int(const void *a, const void *b) { return (*(const int *)a) - (*(const int *)b); }

void
_XmWlRenderFillPoly(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, uint8_t coord_mode, const uint8_t *pts, size_t count)
{
    if (count < 3 || count > 4096) return;
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;

    int *vx = (int *)malloc(count * sizeof(int));
    int *vy = (int *)malloc(count * sizeof(int));
    int *xints = (int *)malloc(count * sizeof(int));
    if (!vx || !vy || !xints) {
        free(vx); free(vy); free(xints);
        pixman_region32_fini(&dt.clip);
        return;
    }

    int min_y = dt.height, max_y = 0;
    for (size_t i = 0; i < count; ++i) {
        int nx = rd_i16(pts + i * 4 + 0);
        int ny = rd_i16(pts + i * 4 + 2);
        if (i == 0 || coord_mode == 0) { vx[i] = dt.dx + nx; vy[i] = dt.dy + ny; }
        else { vx[i] = vx[i - 1] + nx; vy[i] = vy[i - 1] + ny; }
        if (vy[i] < min_y) min_y = vy[i];
        if (vy[i] > max_y) max_y = vy[i];
    }
    if (min_y < 0) min_y = 0;
    if (max_y >= dt.height) max_y = dt.height - 1;

    for (int y = min_y; y <= max_y; ++y) {
        size_t nint = 0;
        size_t j = count - 1;
        for (size_t i = 0; i < count; j = i++) {
            int yi = vy[i], yj = vy[j];
            if ((yi <= y && yj > y) || (yj <= y && yi > y)) {
                double x = (double)vx[i] + ((double)(y - yi) + 0.5) * (double)(vx[j] - vx[i]) / (double)(yj - yi);
                xints[nint++] = (int)floor(x);
            }
        }
        if (nint >= 2) {
            qsort(xints, nint, sizeof(int), cmp_int);
            for (size_t k = 0; k + 1 < nint; k += 2) {
                fill_box_gc(srv, &dt, gc, xints[k], y, xints[k + 1] + 1, y + 1);
            }
        }
    }

    free(vx);
    free(vy);
    free(xints);
    pixman_region32_fini(&dt.clip);
}

void
_XmWlRenderPolyArc(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, const uint8_t *arcs, size_t count, bool fill)
{
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;
    uint32_t fg_px = format_gc_pixel(dt.depth, gc ? gc->fg : 0);

    for (size_t i = 0; i < count; ++i) {
        int x = dt.dx + rd_i16(arcs + i * 12 + 0);
        int y = dt.dy + rd_i16(arcs + i * 12 + 2);
        int w = rd_u16(arcs + i * 12 + 4);
        int h = rd_u16(arcs + i * 12 + 6);
        int16_t a1 = rd_i16(arcs + i * 12 + 8);
        int16_t a2 = rd_i16(arcs + i * 12 + 10);
        if (w <= 0 || h <= 0 || a2 == 0) continue;

        double cx = (double)x + (double)w * 0.5;
        double cy = (double)y + (double)h * 0.5;
        double rx = (double)w * 0.5;
        double ry = (double)h * 0.5;
        bool full_circle = (abs(a2) >= 360 * 64);

        if (fill) {
            double start_rad = (double)a1 * M_PI / (180.0 * 64.0);
            double sweep_rad = (double)a2 * M_PI / (180.0 * 64.0);
            for (int py = y; py <= y + h; ++py) {
                double ny = ((double)py + 0.5 - cy) / (ry > 0.5 ? ry : 0.5);
                if (fabs(ny) > 1.0) continue;
                for (int px = x; px <= x + w; ++px) {
                    double nx = ((double)px + 0.5 - cx) / (rx > 0.5 ? rx : 0.5);
                    if (nx * nx + ny * ny > 1.0) continue;
                    if (!full_circle) {
                        double ang = atan2(-ny, nx);
                        double rel = ang - start_rad;
                        while (sweep_rad > 0 && rel < 0) rel += 2.0 * M_PI;
                        while (sweep_rad < 0 && rel > 0) rel -= 2.0 * M_PI;
                        if ((sweep_rad > 0 && rel > sweep_rad) || (sweep_rad < 0 && rel < sweep_rad))
                            continue;
                    }
                    put_clipped_pixel(srv, &dt, gc, px, py, fg_px);
                }
            }
            if (dt.top_win) _XmWlDamageTop(dt.top_win, x, y, w + 1, h + 1);
        } else {
            int steps = (w + h) * 2;
            if (steps < 32) steps = 32;
            double start_rad = (double)a1 * M_PI / (180.0 * 64.0);
            double sweep_rad = (double)a2 * M_PI / (180.0 * 64.0);
            int prev_x = 0, prev_y = 0;
            for (int s = 0; s <= steps; ++s) {
                double ang = start_rad + sweep_rad * ((double)s / (double)steps);
                int px = (int)lround(cx + rx * cos(ang));
                int py = (int)lround(cy - ry * sin(ang));
                if (s > 0) draw_line_gc(srv, &dt, gc, prev_x, prev_y, px, py);
                prev_x = px;
                prev_y = py;
            }
        }
    }
    pixman_region32_fini(&dt.clip);
}

void
_XmWlRenderPutImage(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, uint8_t format,
                    uint16_t width, uint16_t height, int16_t dst_x, int16_t dst_y,
                    uint8_t left_pad, uint8_t depth, const uint8_t *data, size_t data_len)
{
    if (width == 0 || height == 0 || !data) return;
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;

    uint32_t fg_px = format_gc_pixel(dt.depth, gc ? gc->fg : 0xffffffffu);
    uint32_t bg_px = format_gc_pixel(dt.depth, gc ? gc->bg : 0);

    if (format == 0 || (format == 2 && depth == 1)) {
        /* XYBitmap or 1-bpp ZPixmap (LSBFirst bit order, 32-bit padded scanlines) */
        size_t total_bits = (size_t)width + (size_t)left_pad;
        size_t stride_bytes = ((total_bits + 31u) / 32u) * 4u;
        for (uint16_t r = 0; r < height; ++r) {
            if ((size_t)(r + 1) * stride_bytes > data_len) break;
            const uint8_t *row = data + (size_t)r * stride_bytes;
            for (uint16_t c = 0; c < width; ++c) {
                size_t bit_idx = (size_t)c + (size_t)left_pad;
                bool on = ((row[bit_idx >> 3] >> (bit_idx & 7u)) & 1u) != 0;
                uint32_t spx = (dt.depth == 1) ? (on ? 1u : 0u) : (on ? fg_px : bg_px);
                put_clipped_pixel(srv, &dt, gc, dt.dx + dst_x + c, dt.dy + dst_y + r, spx);
            }
        }
    } else if (format == 1) {
        /* XYPixmap: `depth` bit-planes from MSB to LSB */
        size_t total_bits = (size_t)width + (size_t)left_pad;
        size_t plane_row_bytes = ((total_bits + 31u) / 32u) * 4u;
        size_t plane_bytes = plane_row_bytes * (size_t)height;
        for (uint16_t r = 0; r < height; ++r) {
            for (uint16_t c = 0; c < width; ++c) {
                uint32_t val = 0;
                size_t bit_idx = (size_t)c + (size_t)left_pad;
                for (uint8_t p = 0; p < depth; ++p) {
                    size_t off = (size_t)p * plane_bytes + (size_t)r * plane_row_bytes + (bit_idx >> 3);
                    if (off < data_len && ((data[off] >> (bit_idx & 7u)) & 1u)) {
                        val |= (UINT32_C(1) << (depth - 1u - p));
                    }
                }
                put_clipped_pixel(srv, &dt, gc, dt.dx + dst_x + c, dt.dy + dst_y + r,
                                  format_gc_pixel(dt.depth, val));
            }
        }
    } else if (format == 2 && depth == 8) {
        size_t stride_bytes = (((size_t)width + 3u) / 4u) * 4u;
        for (uint16_t r = 0; r < height; ++r) {
            if ((size_t)r * stride_bytes + width > data_len) break;
            const uint8_t *row = data + (size_t)r * stride_bytes;
            for (uint16_t c = 0; c < width; ++c) {
                uint8_t a = row[c];
                uint32_t spx = (dt.depth == 8) ? a : ((uint32_t)a << 24) | ((uint32_t)a << 16) | ((uint32_t)a << 8) | a;
                put_clipped_pixel(srv, &dt, gc, dt.dx + dst_x + c, dt.dy + dst_y + r, spx);
            }
        }
    } else if (format == 2 && (depth == 24 || depth == 32)) {
        size_t stride_bytes = (size_t)width * 4u;
        for (uint16_t r = 0; r < height; ++r) {
            if ((size_t)(r + 1) * stride_bytes > data_len) break;
            const uint32_t *row = (const uint32_t *)(data + (size_t)r * stride_bytes);
            for (uint16_t c = 0; c < width; ++c) {
                uint32_t raw = row[c];
                uint32_t spx = (depth == 24 && dt.depth != 32) ? (0xff000000u | (raw & 0x00ffffffu)) : raw;
                put_clipped_pixel(srv, &dt, gc, dt.dx + dst_x + c, dt.dy + dst_y + r, spx);
            }
        }
    }

    if (dt.top_win)
        _XmWlDamageTop(dt.top_win, dt.dx + dst_x, dt.dy + dst_y, width, height);
    pixman_region32_fini(&dt.clip);
}

bool
_XmWlRenderGetImage(XmWlServer *srv, uint32_t drawable, int16_t x, int16_t y,
                    uint16_t width, uint16_t height, uint32_t plane_mask, uint8_t format,
                    uint8_t **out_bytes, size_t *out_len, uint8_t *out_depth, uint32_t *out_visual)
{
    *out_bytes = NULL;
    *out_len = 0;
    *out_depth = 24;
    *out_visual = 0;

    uint32_t *src_pixels = NULL;
    int src_w = 0, src_h = 0, src_stride_px = 0, dx = 0, dy = 0;
    uint8_t depth = 24;
    uint32_t visual = 0;

    XmWlPixmap *pix = _XmWlFindPixmap(srv, drawable);
    if (pix) {
        src_pixels = pix->bits;
        src_w = pix->width;
        src_h = pix->height;
        src_stride_px = pix->stride / 4;
        depth = pix->depth;
        visual = 0;
    } else {
        XmWlWindow *win = _XmWlFindWindow(srv, drawable);
        if (!win) return false;
        if (win == srv->root) {
            /* Synthesize root screenshot from mapped top-levels */
            depth = 24;
            visual = XM_WL_VISUAL_RGB24_ID;
            size_t sz = (size_t)width * (size_t)height * 4u;
            uint32_t *buf = (uint32_t *)calloc((size_t)width * (size_t)height, 4u);
            if (!buf) return false;
            for (size_t i = 0; i < (size_t)width * (size_t)height; ++i)
                buf[i] = 0xff5a6d7cu;
            for (XmWlWindow *tw = srv->root->first_child; tw; tw = tw->next_sibling) {
                if (!tw->mapped || !tw->canvas_bits) continue;
                for (int ry = 0; ry < (int)height; ++ry) {
                    int wy = (int)y + ry - tw->y;
                    if (wy < 0 || wy >= tw->canvas_h) continue;
                    for (int rx = 0; rx < (int)width; ++rx) {
                        int wx = (int)x + rx - tw->x;
                        if (wx < 0 || wx >= tw->canvas_w) continue;
                        buf[(size_t)ry * (size_t)width + (size_t)rx] =
                            tw->canvas_bits[(size_t)wy * (size_t)tw->canvas_w + (size_t)wx] & plane_mask;
                    }
                }
            }
            *out_bytes = (uint8_t *)buf;
            *out_len = sz;
            *out_depth = depth;
            *out_visual = visual;
            return true;
        }
        XmWlWindow *top = _XmWlEnclosingTopLevel(srv, win, &dx, &dy);
        if (!top) return false;
        _XmWlEnsureTopCanvas(srv, top);
        src_pixels = top->canvas_bits;
        src_w = top->canvas_w;
        src_h = top->canvas_h;
        src_stride_px = top->canvas_stride / 4;
        depth = win->depth ? win->depth : 24;
        visual = win->visual_id;
    }

    if (!src_pixels) return false;
    *out_depth = depth;
    *out_visual = visual;

    if (format == 2 && depth == 1) {
        size_t row_bytes = (((size_t)width + 31u) / 32u) * 4u;
        size_t total = row_bytes * (size_t)height;
        uint8_t *buf = (uint8_t *)calloc(1, total ? total : 4);
        if (!buf) return false;
        for (uint16_t r = 0; r < height; ++r) {
            int sy = dy + y + r;
            if (sy < 0 || sy >= src_h) continue;
            for (uint16_t c = 0; c < width; ++c) {
                int sx = dx + x + c;
                if (sx < 0 || sx >= src_w) continue;
                uint32_t v = src_pixels[(size_t)sy * (size_t)src_stride_px + (size_t)sx];
                if (v & 1u) buf[(size_t)r * row_bytes + (c >> 3)] |= (uint8_t)(1u << (c & 7u));
            }
        }
        *out_bytes = buf;
        *out_len = total;
        return true;
    }

    if (format == 1) {
        /* XYPixmap */
        size_t plane_row = (((size_t)width + 31u) / 32u) * 4u;
        size_t total = plane_row * (size_t)height * (size_t)depth;
        uint8_t *buf = (uint8_t *)calloc(1, total ? total : 4);
        if (!buf) return false;
        for (uint8_t p = 0; p < depth; ++p) {
            uint32_t bit = UINT32_C(1) << (depth - 1u - p);
            if (!(plane_mask & bit)) continue;
            for (uint16_t r = 0; r < height; ++r) {
                int sy = dy + y + r;
                if (sy < 0 || sy >= src_h) continue;
                for (uint16_t c = 0; c < width; ++c) {
                    int sx = dx + x + c;
                    if (sx < 0 || sx >= src_w) continue;
                    uint32_t v = src_pixels[(size_t)sy * (size_t)src_stride_px + (size_t)sx];
                    if (v & bit)
                        buf[((size_t)p * (size_t)height + (size_t)r) * plane_row + (c >> 3)] |=
                            (uint8_t)(1u << (c & 7u));
                }
            }
        }
        *out_bytes = buf;
        *out_len = total;
        return true;
    }

    if (depth == 8) {
        size_t row_bytes = (((size_t)width + 3u) / 4u) * 4u;
        size_t total = row_bytes * (size_t)height;
        uint8_t *buf = (uint8_t *)calloc(1, total ? total : 4);
        if (!buf) return false;
        for (uint16_t r = 0; r < height; ++r) {
            int sy = dy + y + r;
            if (sy < 0 || sy >= src_h) continue;
            for (uint16_t c = 0; c < width; ++c) {
                int sx = dx + x + c;
                if (sx < 0 || sx >= src_w) continue;
                buf[(size_t)r * row_bytes + c] =
                    (uint8_t)(src_pixels[(size_t)sy * (size_t)src_stride_px + (size_t)sx] & 0xffu);
            }
        }
        *out_bytes = buf;
        *out_len = total;
        return true;
    }

    size_t total = (size_t)width * (size_t)height * 4u;
    uint32_t *buf = (uint32_t *)calloc((size_t)width * (size_t)height, 4u);
    if (!buf) return false;
    for (uint16_t r = 0; r < height; ++r) {
        int sy = dy + y + r;
        if (sy < 0 || sy >= src_h) continue;
        for (uint16_t c = 0; c < width; ++c) {
            int sx = dx + x + c;
            if (sx < 0 || sx >= src_w) continue;
            uint32_t v = src_pixels[(size_t)sy * (size_t)src_stride_px + (size_t)sx];
            if (depth == 24) v &= 0x00ffffffu;
            buf[(size_t)r * (size_t)width + (size_t)c] = v & plane_mask;
        }
    }
    *out_bytes = (uint8_t *)buf;
    *out_len = total;
    return true;
}

static void
emit_no_expose(XmWlClient *cli, uint32_t dst_drawable, uint8_t major_op)
{
    if (!cli) return;
    uint8_t ev[32];
    memset(ev, 0, sizeof(ev));
    ev[0] = 14; /* NoExpose */
    wr_u16(ev + 2, cli->seq);
    wr_u32(ev + 4, dst_drawable);
    wr_u16(ev + 8, 0);
    ev[10] = major_op;
    _XmWlSendEventToClient(cli, ev);
}

void
_XmWlRenderCopyArea(XmWlServer *srv, XmWlClient *cli, uint32_t src_d, uint32_t dst_d, XmWlGC *gc,
                    int16_t sx, int16_t sy, uint16_t w, uint16_t h, int16_t dx, int16_t dy)
{
    DrawTarget src_dt, dst_dt;
    bool ok_src = resolve_draw_target(srv, src_d, NULL, &src_dt);
    bool ok_dst = resolve_draw_target(srv, dst_d, gc, &dst_dt);
    if (ok_src && ok_dst && w > 0 && h > 0) {
        uint32_t *tmp = (uint32_t *)calloc((size_t)w * (size_t)h, 4u);
        if (tmp) {
            for (uint16_t r = 0; r < h; ++r) {
                int rsy = src_dt.dy + sy + r;
                if (rsy < 0 || rsy >= src_dt.height) continue;
                for (uint16_t c = 0; c < w; ++c) {
                    int rsx = src_dt.dx + sx + c;
                    if (rsx < 0 || rsx >= src_dt.width) continue;
                    tmp[(size_t)r * (size_t)w + (size_t)c] =
                        src_dt.pixels[(size_t)rsy * (size_t)src_dt.stride_px + (size_t)rsx];
                }
            }
            for (uint16_t r = 0; r < h; ++r) {
                for (uint16_t c = 0; c < w; ++c) {
                    uint32_t spx = format_gc_pixel(dst_dt.depth, tmp[(size_t)r * (size_t)w + (size_t)c]);
                    put_clipped_pixel(srv, &dst_dt, gc, dst_dt.dx + dx + c, dst_dt.dy + dy + r, spx);
                }
            }
            free(tmp);
            if (dst_dt.top_win)
                _XmWlDamageTop(dst_dt.top_win, dst_dt.dx + dx, dst_dt.dy + dy, w, h);
        }
    }
    if (ok_src) pixman_region32_fini(&src_dt.clip);
    if (ok_dst) pixman_region32_fini(&dst_dt.clip);
    if (gc && gc->graphics_exposures)
        emit_no_expose(cli, dst_d, 62 /* CopyArea */);
}

void
_XmWlRenderCopyPlane(XmWlServer *srv, XmWlClient *cli, uint32_t src_d, uint32_t dst_d, XmWlGC *gc,
                     int16_t sx, int16_t sy, uint16_t w, uint16_t h, int16_t dx, int16_t dy, uint32_t bit_plane)
{
    DrawTarget src_dt, dst_dt;
    bool ok_src = resolve_draw_target(srv, src_d, NULL, &src_dt);
    bool ok_dst = resolve_draw_target(srv, dst_d, gc, &dst_dt);
    if (ok_src && ok_dst && w > 0 && h > 0) {
        uint32_t fg_px = format_gc_pixel(dst_dt.depth, gc ? gc->fg : 0xffffffffu);
        uint32_t bg_px = format_gc_pixel(dst_dt.depth, gc ? gc->bg : 0);
        uint32_t mask = bit_plane ? bit_plane : 1u;
        uint8_t *tmp = (uint8_t *)calloc((size_t)w * (size_t)h, 1u);
        if (tmp) {
            for (uint16_t r = 0; r < h; ++r) {
                int rsy = src_dt.dy + sy + r;
                if (rsy < 0 || rsy >= src_dt.height) continue;
                for (uint16_t c = 0; c < w; ++c) {
                    int rsx = src_dt.dx + sx + c;
                    if (rsx < 0 || rsx >= src_dt.width) continue;
                    uint32_t v = src_dt.pixels[(size_t)rsy * (size_t)src_dt.stride_px + (size_t)rsx];
                    tmp[(size_t)r * (size_t)w + (size_t)c] = ((v & mask) != 0) ? 1u : 0u;
                }
            }
            for (uint16_t r = 0; r < h; ++r) {
                for (uint16_t c = 0; c < w; ++c) {
                    bool on = tmp[(size_t)r * (size_t)w + (size_t)c] != 0;
                    uint32_t spx = (dst_dt.depth == 1) ? (on ? 1u : 0u) : (on ? fg_px : bg_px);
                    put_clipped_pixel(srv, &dst_dt, gc, dst_dt.dx + dx + c, dst_dt.dy + dy + r, spx);
                }
            }
            free(tmp);
            if (dst_dt.top_win)
                _XmWlDamageTop(dst_dt.top_win, dst_dt.dx + dx, dst_dt.dy + dy, w, h);
        }
    }
    if (ok_src) pixman_region32_fini(&src_dt.clip);
    if (ok_dst) pixman_region32_fini(&dst_dt.clip);
    if (gc && gc->graphics_exposures)
        emit_no_expose(cli, dst_d, 63 /* CopyPlane */);
}

/* ---------- FreeType + Fontconfig Core XLFD Font Engine ---------- */

static void
parse_xlfd_spec(const char *name, int *out_px, bool *out_mono, bool *out_bold, bool *out_italic)
{
    *out_px = 13;
    *out_mono = false;
    *out_bold = false;
    *out_italic = false;
    if (!name || !name[0]) return;

    if (!strcasecmp(name, "fixed") || !strcasecmp(name, "6x13") || !strcasecmp(name, "8x13")) {
        *out_px = 12; *out_mono = true; return;
    }
    if (!strcasecmp(name, "9x15")) {
        *out_px = 14; *out_mono = true; return;
    }
    if (!strcasecmp(name, "6x10")) {
        *out_px = 10; *out_mono = true; return;
    }
    if (strcasestr(name, "mono") || strcasestr(name, "courier") || strcasestr(name, "interface user") || strcasestr(name, "fixed"))
        *out_mono = true;
    if (strcasestr(name, "-bold-") || strcasestr(name, "demibold"))
        *out_bold = true;

    if (name[0] == '-') {
        /* XLFD fields: -1:foundry-2:family-3:weight-4:slant-5:setwidth-6:addstyle-7:px-8:pt-...-11:spacing-... */
        const char *fields[15] = {0};
        char copy[256];
        snprintf(copy, sizeof(copy), "%s", name);
        int nf = 0;
        char *p = copy + 1;
        while (p && nf < 14) {
            fields[nf++] = p;
            char *dash = strchr(p, '-');
            if (!dash) break;
            *dash = '\0';
            p = dash + 1;
        }
        if (nf >= 3 && fields[2] && !strcasecmp(fields[2], "bold")) *out_bold = true;
        if (nf >= 4 && fields[3] && (!strcasecmp(fields[3], "i") || !strcasecmp(fields[3], "o"))) *out_italic = true;
        if (nf >= 7 && fields[6] && isdigit((unsigned char)fields[6][0])) {
            int px = atoi(fields[6]);
            if (px >= 6 && px <= 72) *out_px = px;
        } else if (nf >= 8 && fields[7] && isdigit((unsigned char)fields[7][0])) {
            int pt10 = atoi(fields[7]);
            if (pt10 >= 50 && pt10 <= 720) *out_px = (pt10 * 96 + 360) / 720;
        } else if (nf >= 6 && fields[5]) {
            /* CDE symbolic size aliases: xs, s, m, l, xl, xxl (including wildcard suffixes like m*) */
            if (!strncasecmp(fields[5], "xxl", 3)) *out_px = 22;
            else if (!strncasecmp(fields[5], "xl", 2)) *out_px = 18;
            else if (!strncasecmp(fields[5], "xs", 2)) *out_px = 10;
            else if (tolower((unsigned char)fields[5][0]) == 's') *out_px = 12;
            else if (tolower((unsigned char)fields[5][0]) == 'm') *out_px = 14;
            else if (tolower((unsigned char)fields[5][0]) == 'l') *out_px = 16;
        }
        if (nf >= 11 && fields[10] && (!strcasecmp(fields[10], "m") || !strcasecmp(fields[10], "c")))
            *out_mono = true;
    }
}

XmWlFont *
_XmWlOpenFont(XmWlServer *srv, uint32_t id, const char *name, size_t len)
{
    XmWlFont *f = (XmWlFont *)calloc(1, sizeof(*f));
    if (!f) return NULL;
    f->id = id;
    size_t n = len < sizeof(f->name) - 1 ? len : sizeof(f->name) - 1;
    if (name && n > 0) memcpy(f->name, name, n);
    f->name[n] = '\0';

    int px = 13;
    bool mono = false, bold = false, italic = false;
    parse_xlfd_spec(f->name, &px, &mono, &bold, &italic);
    if (srv->logical_zoom > 1.01 && f->name[0] != '-')
        px = (int)lround((double)px * srv->logical_zoom);
    f->pixel_size = px;
    f->monospace = mono;
    f->bold = bold;

    if (srv->ft_inited) {
        FcPattern *pat = FcPatternCreate();
        FcPatternAddString(pat, FC_FAMILY, (const FcChar8 *)(mono ? "monospace" : "sans-serif"));
        FcPatternAddDouble(pat, FC_PIXEL_SIZE, (double)px);
        FcPatternAddInteger(pat, FC_WEIGHT, bold ? FC_WEIGHT_BOLD : FC_WEIGHT_REGULAR);
        FcPatternAddInteger(pat, FC_SLANT, italic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);
        FcConfigSubstitute(NULL, pat, FcMatchPattern);
        FcDefaultSubstitute(pat);
        FcResult res;
        FcPattern *match = FcFontMatch(NULL, pat, &res);
        if (match) {
            FcChar8 *file = NULL;
            int idx = 0;
            if (FcPatternGetString(match, FC_FILE, 0, &file) == FcResultMatch && file) {
                FcPatternGetInteger(match, FC_INDEX, 0, &idx);
                if (FT_New_Face(srv->ft_lib, (const char *)file, idx, &f->face) == 0) {
                    FT_Set_Pixel_Sizes(f->face, 0, (FT_UInt)px);
                }
            }
            FcPatternDestroy(match);
        }
        FcPatternDestroy(pat);
    }

    if (f->face) {
        f->ascent = (int16_t)((f->face->size->metrics.ascender + 63) >> 6);
        f->descent = (int16_t)((-f->face->size->metrics.descender + 63) >> 6);
        if (f->ascent < 1) f->ascent = (int16_t)px;
        if (f->descent < 1) f->descent = (int16_t)(px / 4 + 1);
    } else {
        f->ascent = (int16_t)((px * 4) / 5);
        f->descent = (int16_t)(px - f->ascent + 1);
    }

    int16_t min_lb = 32767, min_rb = 32767, min_w = 32767, min_a = 32767, min_d = 32767;
    int16_t max_lb = -32768, max_rb = -32768, max_w = -32768, max_a = -32768, max_d = -32768;

    for (int ch = 0; ch < 256; ++ch) {
        XmWlCharInfo *ci = &f->chars[ch];
        int adv = mono ? ((px * 3 + 2) / 5) : ((px + 1) / 2);
        if (adv < 4) adv = 4;
        ci->lbearing = 0;
        ci->rbearing = (int16_t)adv;
        ci->width = (int16_t)adv;
        ci->ascent = f->ascent;
        ci->descent = f->descent;

        if (f->face && ch >= 32) {
            FT_UInt gi = FT_Get_Char_Index(f->face, (FT_ULong)ch);
            if (gi != 0 && FT_Load_Glyph(f->face, gi, FT_LOAD_DEFAULT) == 0) {
                FT_GlyphSlot gs = f->face->glyph;
                ci->width = (int16_t)((gs->advance.x + 32) >> 6);
                ci->lbearing = (int16_t)(gs->metrics.horiBearingX >> 6);
                ci->rbearing = (int16_t)((gs->metrics.horiBearingX + gs->metrics.width + 63) >> 6);
                ci->ascent = (int16_t)((gs->metrics.horiBearingY + 63) >> 6);
                ci->descent = (int16_t)((gs->metrics.height - gs->metrics.horiBearingY + 63) >> 6);
                if (ci->width == 0 && ch == 32)
                    ci->width = (int16_t)adv;
            }
        }

        if (ch >= 32 && ch <= 126) {
            if (ci->lbearing < min_lb) min_lb = ci->lbearing;
            if (ci->rbearing < min_rb) min_rb = ci->rbearing;
            if (ci->width < min_w) min_w = ci->width;
            if (ci->ascent < min_a) min_a = ci->ascent;
            if (ci->descent < min_d) min_d = ci->descent;
            if (ci->lbearing > max_lb) max_lb = ci->lbearing;
            if (ci->rbearing > max_rb) max_rb = ci->rbearing;
            if (ci->width > max_w) max_w = ci->width;
            if (ci->ascent > max_a) max_a = ci->ascent;
            if (ci->descent > max_d) max_d = ci->descent;
        }
    }
    f->min_bounds = (XmWlCharInfo){ min_lb, min_rb, min_w, min_a, min_d, 0 };
    f->max_bounds = (XmWlCharInfo){ max_lb, max_rb, max_w, max_a, max_d, 0 };
    return f;
}

static int
draw_codepoint_ft(XmWlServer *srv, DrawTarget *dt, const XmWlGC *gc, XmWlFont *font,
                  int pen_x, int baseline_y, uint32_t codepoint)
{
    if (!font) return 6;
    if (!font->face) {
        return (codepoint < 256) ? font->chars[codepoint].width : 7;
    }

    FT_UInt gi = FT_Get_Char_Index(font->face, (FT_ULong)codepoint);
    if (FT_Load_Glyph(font->face, gi, FT_LOAD_RENDER) != 0) {
        return (codepoint < 256) ? font->chars[codepoint].width : 7;
    }

    FT_GlyphSlot gs = font->face->glyph;
    FT_Bitmap *bm = &gs->bitmap;
    int gx0 = pen_x + gs->bitmap_left;
    int gy0 = baseline_y - gs->bitmap_top;

    uint32_t fg_raw = gc ? gc->fg : 0;
    uint8_t sr = (uint8_t)((fg_raw >> 16) & 0xffu);
    uint8_t sg = (uint8_t)((fg_raw >> 8) & 0xffu);
    uint8_t sb = (uint8_t)(fg_raw & 0xffu);

    for (unsigned int row = 0; row < bm->rows; ++row) {
        int ty = gy0 + (int)row;
        if (ty < 0 || ty >= dt->height) continue;
        const uint8_t *src_row = bm->buffer + (int)row * bm->pitch;
        for (unsigned int col = 0; col < bm->width; ++col) {
            uint8_t cov = 0;
            if (bm->pixel_mode == FT_PIXEL_MODE_GRAY) {
                cov = src_row[col];
            } else if (bm->pixel_mode == FT_PIXEL_MODE_MONO) {
                cov = ((src_row[col >> 3] >> (7u - (col & 7u))) & 1u) ? 255u : 0u;
            }
            if (cov == 0) continue;
            int tx = gx0 + (int)col;
            if (tx < 0 || tx >= dt->width) continue;
            if (!pixman_region32_contains_point(&dt->clip, tx, ty, NULL)) continue;
            if (!clip_mask_allows(srv, gc, tx - dt->dx, ty - dt->dy)) continue;

            uint32_t *dst_p = &dt->pixels[(size_t)ty * (size_t)dt->stride_px + (size_t)tx];
            if (dt->depth == 1) {
                if (cov >= 128) *dst_p = (fg_raw & 1u) ? 1u : 0u;
            } else if (cov == 255 && (!gc || gc->function == 3)) {
                *dst_p = format_gc_pixel(dt->depth, fg_raw);
            } else {
                uint32_t d = *dst_p;
                uint8_t dr = (uint8_t)((d >> 16) & 0xffu);
                uint8_t dg = (uint8_t)((d >> 8) & 0xffu);
                uint8_t db = (uint8_t)(d & 0xffu);
                uint8_t da = (uint8_t)((d >> 24) & 0xffu);
                uint32_t inv = 255u - cov;
                uint8_t nr = (uint8_t)((sr * cov + dr * inv + 127u) / 255u);
                uint8_t ng = (uint8_t)((sg * cov + dg * inv + 127u) / 255u);
                uint8_t nb = (uint8_t)((sb * cov + db * inv + 127u) / 255u);
                uint8_t na = (dt->depth == 32) ? (uint8_t)(cov + (da * inv + 127u) / 255u) : 0xffu;
                *dst_p = ((uint32_t)na << 24) | ((uint32_t)nr << 16) | ((uint32_t)ng << 8) | nb;
            }
        }
    }
    if (dt->top_win && bm->width > 0 && bm->rows > 0)
        _XmWlDamageTop(dt->top_win, gx0, gy0, (int)bm->width, (int)bm->rows);

    return (int)((gs->advance.x + 32) >> 6);
}

void
_XmWlRenderPolyText(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, int16_t x, int16_t y,
                    const uint8_t *items, size_t len, bool is_16bit)
{
    if (!gc || !items || len == 0) return;
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;

    XmWlFont *font = _XmWlFindFont(srv, gc->font_id);
    if (!font) {
        font = _XmWlOpenFont(srv, 0, "fixed", 5);
    }
    bool temp_font = (font && font->id == 0);

    int pen_x = dt.dx + x;
    int base_y = dt.dy + y;
    size_t pos = 0;

    while (pos < len) {
        uint8_t nchars = items[pos++];
        if (nchars == 0) continue;
        if (nchars == 255) {
            if (pos + 4 > len) break;
            uint32_t fid = ((uint32_t)items[pos] << 24) | ((uint32_t)items[pos + 1] << 16) |
                           ((uint32_t)items[pos + 2] << 8) | items[pos + 3];
            pos += 4;
            gc->font_id = fid;
            XmWlFont *nf = _XmWlFindFont(srv, fid);
            if (nf) {
                if (temp_font) { _XmWlFreeFontData(font); free(font); temp_font = false; }
                font = nf;
            }
            continue;
        }
        if (pos >= len) break;
        int8_t delta = (int8_t)items[pos++];
        pen_x += delta;
        size_t byte_need = is_16bit ? ((size_t)nchars * 2u) : (size_t)nchars;
        if (pos + byte_need > len) break;
        for (uint8_t i = 0; i < nchars; ++i) {
            uint32_t cp = is_16bit ? (((uint32_t)items[pos + i * 2] << 8) | items[pos + i * 2 + 1])
                                   : items[pos + i];
            pen_x += draw_codepoint_ft(srv, &dt, gc, font, pen_x, base_y, cp);
        }
        pos += byte_need;
    }

    if (temp_font) { _XmWlFreeFontData(font); free(font); }
    pixman_region32_fini(&dt.clip);
}

void
_XmWlRenderImageText(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, int16_t x, int16_t y,
                     const uint8_t *str, size_t count, bool is_16bit)
{
    if (!gc || !str || count == 0) return;
    DrawTarget dt;
    if (!resolve_draw_target(srv, drawable, gc, &dt)) return;

    XmWlFont *font = _XmWlFindFont(srv, gc->font_id);
    if (!font) font = _XmWlOpenFont(srv, 0, "fixed", 5);
    bool temp_font = (font && font->id == 0);

    int total_w = 0;
    for (size_t i = 0; i < count; ++i) {
        uint32_t cp = is_16bit ? (((uint32_t)str[i * 2] << 8) | str[i * 2 + 1]) : str[i];
        total_w += (cp < 256 && font) ? font->chars[cp].width : (font ? font->max_bounds.width : 8);
    }

    /* Fill background cell rectangle first per X11 ImageText semantics */
    XmWlGC bg_gc = *gc;
    bg_gc.function = 3;
    bg_gc.fill_style = 0;
    bg_gc.fg = gc->bg;
    int asc = font ? font->ascent : 11;
    int desc = font ? font->descent : 3;
    fill_box_gc(srv, &dt, &bg_gc, dt.dx + x, dt.dy + y - asc, dt.dx + x + total_w, dt.dy + y + desc);

    int pen_x = dt.dx + x;
    int base_y = dt.dy + y;
    for (size_t i = 0; i < count; ++i) {
        uint32_t cp = is_16bit ? (((uint32_t)str[i * 2] << 8) | str[i * 2 + 1]) : str[i];
        pen_x += draw_codepoint_ft(srv, &dt, gc, font, pen_x, base_y, cp);
    }

    if (temp_font) { _XmWlFreeFontData(font); free(font); }
    pixman_region32_fini(&dt.clip);
}

/* ---------- Icon Pixmap Builder (for xdg_toplevel_icon_v1) ---------- */

bool
_XmWlBuildIconPixels(XmWlServer *srv, uint32_t icon_pixmap, uint32_t icon_mask,
                     uint32_t **out_argb, int *out_w, int *out_h)
{
    XmWlPixmap *ip = _XmWlFindPixmap(srv, icon_pixmap);
    if (!ip || !ip->bits || ip->width == 0 || ip->height == 0)
        return false;
    XmWlPixmap *mp = icon_mask ? _XmWlFindPixmap(srv, icon_mask) : NULL;

    int w = ip->width, h = ip->height;
    uint32_t *argb = (uint32_t *)calloc((size_t)w * (size_t)h, 4u);
    if (!argb) return false;

    for (int r = 0; r < h; ++r) {
        for (int c = 0; c < w; ++c) {
            uint32_t v = ip->bits[(size_t)r * (size_t)(ip->stride / 4) + (size_t)c];
            uint32_t px = (ip->depth == 1) ? ((v & 1u) ? 0xff222222u : 0xffe8e8e8u)
                        : (ip->depth == 32 ? v : (0xff000000u | (v & 0x00ffffffu)));
            if (mp && mp->bits && c < (int)mp->width && r < (int)mp->height) {
                uint32_t mv = mp->bits[(size_t)r * (size_t)(mp->stride / 4) + (size_t)c];
                if ((mv & 1u) == 0 && (mv & 0x00ffffffu) == 0)
                    px = 0x00000000u;
            }
            argb[(size_t)r * (size_t)w + (size_t)c] = px;
        }
    }
    *out_argb = argb;
    *out_w = w;
    *out_h = h;
    return true;
}

/* ---------- XRender 0.11 Extension Implementation ---------- */

static pixman_op_t
pict_op_to_pixman(uint8_t op)
{
    switch (op) {
    case 0:  return PIXMAN_OP_CLEAR;
    case 1:  return PIXMAN_OP_SRC;
    case 2:  return PIXMAN_OP_DST;
    case 3:  return PIXMAN_OP_OVER;
    case 4:  return PIXMAN_OP_OVER_REVERSE;
    case 5:  return PIXMAN_OP_IN;
    case 6:  return PIXMAN_OP_IN_REVERSE;
    case 7:  return PIXMAN_OP_OUT;
    case 8:  return PIXMAN_OP_OUT_REVERSE;
    case 9:  return PIXMAN_OP_ATOP;
    case 10: return PIXMAN_OP_ATOP_REVERSE;
    case 11: return PIXMAN_OP_XOR;
    case 12: return PIXMAN_OP_ADD;
    case 13: return PIXMAN_OP_SATURATE;
    default: return PIXMAN_OP_OVER;
    }
}

static pixman_repeat_t
repeat_to_pixman(uint8_t r)
{
    switch (r) {
    case 1: return PIXMAN_REPEAT_NORMAL;
    case 2: return PIXMAN_REPEAT_PAD;
    case 3: return PIXMAN_REPEAT_REFLECT;
    default: return PIXMAN_REPEAT_NONE;
    }
}

static void
apply_picture_values(XmWlPicture *pic, uint32_t mask, const uint8_t *vals, size_t nvals)
{
    size_t idx = 0;
    for (uint32_t bit = 0; bit < 16 && idx < nvals; ++bit) {
        if (!(mask & (UINT32_C(1) << bit))) continue;
        uint32_t v = rd_u32(vals + idx * 4);
        idx++;
        switch (bit) {
        case 0: pic->repeat = (uint8_t)(v & 3u); break;
        case 4: pic->clip_x_origin = (int16_t)v; break;
        case 5: pic->clip_y_origin = (int16_t)v; break;
        case 6:
            pic->clip_mask_pixmap = v;
            if (v == 0) { free(pic->clip_rects); pic->clip_rects = NULL; pic->clip_rect_count = 0; pic->has_clip_rects = false; }
            break;
        case 11: pic->component_alpha = (v != 0); break;
        default: break;
        }
    }
}

static pixman_image_t *
resolve_picture_image(XmWlServer *srv, XmWlPicture *pic, bool is_dst,
                      int *out_dx, int *out_dy, XmWlWindow **out_top,
                      pixman_region32_t *out_clip)
{
    *out_dx = 0;
    *out_dy = 0;
    *out_top = NULL;
    if (!pic) return NULL;

    if (pic->source_img) {
        pixman_image_set_repeat(pic->source_img, repeat_to_pixman(pic->repeat));
        pixman_image_set_component_alpha(pic->source_img, pic->component_alpha);
        if (pic->has_transform)
            pixman_image_set_transform(pic->source_img, &pic->transform);
        return pic->source_img;
    }

    XmWlPixmap *px = _XmWlFindPixmap(srv, pic->drawable_id);
    if (px && px->img) {
        pixman_image_set_repeat(px->img, repeat_to_pixman(pic->repeat));
        pixman_image_set_component_alpha(px->img, pic->component_alpha);
        if (pic->has_transform)
            pixman_image_set_transform(px->img, &pic->transform);
        else {
            pixman_transform_t id;
            pixman_transform_init_identity(&id);
            pixman_image_set_transform(px->img, &id);
        }
        if (is_dst && out_clip) {
            pixman_region32_init_rect(out_clip, 0, 0, px->width, px->height);
            if (pic->has_clip_rects) {
                pixman_region32_t cr;
                pixman_region32_init(&cr);
                for (size_t i = 0; i < pic->clip_rect_count; ++i) {
                    pixman_region32_union_rect(
                        &cr, &cr,
                        pic->clip_x_origin + pic->clip_rects[i].x,
                        pic->clip_y_origin + pic->clip_rects[i].y,
                        pic->clip_rects[i].width, pic->clip_rects[i].height);
                }
                pixman_region32_intersect(out_clip, out_clip, &cr);
                pixman_region32_fini(&cr);
            }
        }
        return px->img;
    }

    XmWlWindow *win = _XmWlFindWindow(srv, pic->drawable_id);
    if (!win || win == srv->root) return NULL;
    int dx = 0, dy = 0;
    XmWlWindow *top = _XmWlEnclosingTopLevel(srv, win, &dx, &dy);
    if (!top) return NULL;
    _XmWlEnsureTopCanvas(srv, top);
    if (!top->canvas) return NULL;

    *out_dx = dx;
    *out_dy = dy;
    *out_top = top;
    pixman_image_set_component_alpha(top->canvas, pic->component_alpha);

    if (is_dst && out_clip) {
        DrawTarget dt;
        if (resolve_draw_target(srv, win->id, NULL, &dt)) {
            pixman_region32_init(out_clip);
            pixman_region32_copy(out_clip, &dt.clip);
            pixman_region32_fini(&dt.clip);
        } else {
            pixman_region32_init(out_clip);
        }
        if (pic->has_clip_rects) {
            pixman_region32_t cr;
            pixman_region32_init(&cr);
            for (size_t i = 0; i < pic->clip_rect_count; ++i) {
                pixman_region32_union_rect(
                    &cr, &cr,
                    dx + pic->clip_x_origin + pic->clip_rects[i].x,
                    dy + pic->clip_y_origin + pic->clip_rects[i].y,
                    pic->clip_rects[i].width, pic->clip_rects[i].height);
            }
            pixman_region32_intersect(out_clip, out_clip, &cr);
            pixman_region32_fini(&cr);
        }
    }
    return top->canvas;
}

static XmWlGlyph *
find_glyph(XmWlGlyphSet *gs, uint32_t gid)
{
    if (!gs) return NULL;
    for (XmWlGlyph *g = gs->buckets[gid & 63u]; g; g = g->next) {
        if (g->glyph_id == gid) return g;
    }
    return NULL;
}

void
_XmWlHandleRenderRequest(XmWlServer *srv, XmWlClient *cli, uint8_t minor_op, const uint8_t *req, size_t req_len)
{
    switch (minor_op) {
    case 0: { /* RenderQueryVersion */
        uint8_t rep[32];
        memset(rep, 0, sizeof(rep));
        rep[0] = 1;
        wr_u16(rep + 2, cli->seq);
        wr_u32(rep + 8, 0);
        wr_u32(rep + 12, 11); /* 0.11 */
        _XmWlSendReply(cli, rep, NULL, 0);
        break;
    }
    case 1: { /* RenderQueryPictFormats */
        /*
         * 5 PictFormInfos (28 bytes each = 140 bytes)
         * 1 PictScreen: num_depths=4, fallback=RGB24 (8 bytes)
         *   Depth 1: num_visuals=0 (8 bytes)
         *   Depth 8: num_visuals=0 (8 bytes)
         *   Depth 24: num_visuals=1 -> (Visual 0x21, RGB24) (8 + 8 = 16 bytes)
         *   Depth 32: num_visuals=1 -> (Visual 0x22, ARGB32) (8 + 8 = 16 bytes)
         * 1 Subpixel uint32 (SubPixelUnknown = 0) (4 bytes)
         * Total extra = 140 + 8 + 8 + 8 + 16 + 16 + 4 = 200 bytes (50 words)
         */
        uint8_t extra[200];
        memset(extra, 0, sizeof(extra));
        struct {
            uint32_t id; uint8_t type; uint8_t depth;
            uint16_t rs, rm, gs, gm, bs, bm, as, am;
        } fmts[5] = {
            { XM_WL_PICTFMT_ARGB32, 1, 32, 16, 0xff, 8, 0xff, 0, 0xff, 24, 0xff },
            { XM_WL_PICTFMT_RGB24,  1, 24, 16, 0xff, 8, 0xff, 0, 0xff,  0, 0x00 },
            { XM_WL_PICTFMT_A8,     1,  8,  0, 0x00, 0, 0x00, 0, 0x00,  0, 0xff },
            { XM_WL_PICTFMT_A4,     1,  4,  0, 0x00, 0, 0x00, 0, 0x00,  0, 0x0f },
            { XM_WL_PICTFMT_A1,     1,  1,  0, 0x00, 0, 0x00, 0, 0x00,  0, 0x01 },
        };
        for (int i = 0; i < 5; ++i) {
            uint8_t *p = extra + i * 28;
            wr_u32(p + 0, fmts[i].id);
            p[4] = fmts[i].type;
            p[5] = fmts[i].depth;
            wr_u16(p + 8, fmts[i].rs);  wr_u16(p + 10, fmts[i].rm);
            wr_u16(p + 12, fmts[i].gs); wr_u16(p + 14, fmts[i].gm);
            wr_u16(p + 16, fmts[i].bs); wr_u16(p + 18, fmts[i].bm);
            wr_u16(p + 20, fmts[i].as); wr_u16(p + 22, fmts[i].am);
            wr_u32(p + 24, 0);
        }
        uint8_t *s = extra + 140;
        wr_u32(s + 0, 4); /* 4 depths */
        wr_u32(s + 4, XM_WL_PICTFMT_RGB24);
        s += 8;
        /* depth 1 */
        s[0] = 1; wr_u16(s + 2, 0); s += 8;
        /* depth 8 */
        s[0] = 8; wr_u16(s + 2, 0); s += 8;
        /* depth 24 */
        s[0] = 24; wr_u16(s + 2, 1);
        wr_u32(s + 8, XM_WL_VISUAL_RGB24_ID);
        wr_u32(s + 12, XM_WL_PICTFMT_RGB24);
        s += 16;
        /* depth 32 */
        s[0] = 32; wr_u16(s + 2, 1);
        wr_u32(s + 8, XM_WL_VISUAL_ARGB32_ID);
        wr_u32(s + 12, XM_WL_PICTFMT_ARGB32);
        s += 16;
        /* subpixel */
        wr_u32(s, 0);

        uint8_t rep[32];
        memset(rep, 0, sizeof(rep));
        rep[0] = 1;
        wr_u16(rep + 2, cli->seq);
        wr_u32(rep + 4, sizeof(extra) / 4);
        wr_u32(rep + 8, 5);  /* num_formats */
        wr_u32(rep + 12, 1); /* num_screens */
        wr_u32(rep + 16, 4); /* num_depths */
        wr_u32(rep + 20, 2); /* num_visuals */
        wr_u32(rep + 24, 1); /* num_subpixel */
        _XmWlSendReply(cli, rep, extra, sizeof(extra));
        break;
    }
    case 2: { /* RenderQueryPictIndexValues */
        uint8_t rep[32];
        memset(rep, 0, sizeof(rep));
        rep[0] = 1;
        wr_u16(rep + 2, cli->seq);
        _XmWlSendReply(cli, rep, NULL, 0);
        break;
    }
    case 4: { /* RenderCreatePicture */
        if (req_len < 20) break;
        uint32_t pid = rd_u32(req + 4);
        uint32_t did = rd_u32(req + 8);
        uint32_t fmt = rd_u32(req + 12);
        uint32_t vmask = rd_u32(req + 16);
        XmWlPicture *pic = (XmWlPicture *)calloc(1, sizeof(*pic));
        if (!pic) break;
        pic->id = pid;
        pic->drawable_id = did;
        pic->format_id = fmt;
        pic->filter = PIXMAN_FILTER_GOOD;
        pixman_transform_init_identity(&pic->transform);
        apply_picture_values(pic, vmask, req + 20, (req_len - 20) / 4);
        uint32_t b = pid & 255u;
        pic->hash_next = srv->pic_buckets[b];
        srv->pic_buckets[b] = pic;
        break;
    }
    case 5: { /* RenderChangePicture */
        if (req_len < 12) break;
        XmWlPicture *pic = _XmWlFindPicture(srv, rd_u32(req + 4));
        if (pic) apply_picture_values(pic, rd_u32(req + 8), req + 12, (req_len - 12) / 4);
        break;
    }
    case 6: { /* RenderSetPictureClipRectangles */
        if (req_len < 12) break;
        XmWlPicture *pic = _XmWlFindPicture(srv, rd_u32(req + 4));
        if (!pic) break;
        pic->clip_x_origin = rd_i16(req + 8);
        pic->clip_y_origin = rd_i16(req + 10);
        size_t n = (req_len - 12) / 8;
        free(pic->clip_rects);
        pic->clip_rects = n ? (XmWlRect *)calloc(n, sizeof(XmWlRect)) : NULL;
        pic->clip_rect_count = n;
        pic->has_clip_rects = true;
        for (size_t i = 0; i < n && pic->clip_rects; ++i) {
            pic->clip_rects[i].x = rd_i16(req + 12 + i * 8 + 0);
            pic->clip_rects[i].y = rd_i16(req + 12 + i * 8 + 2);
            pic->clip_rects[i].width = rd_u16(req + 12 + i * 8 + 4);
            pic->clip_rects[i].height = rd_u16(req + 12 + i * 8 + 6);
        }
        break;
    }
    case 7: { /* RenderFreePicture */
        if (req_len < 8) break;
        uint32_t pid = rd_u32(req + 4);
        uint32_t b = pid & 255u;
        XmWlPicture **pp = &srv->pic_buckets[b];
        while (*pp) {
            if ((*pp)->id == pid) {
                XmWlPicture *dead = *pp;
                *pp = dead->hash_next;
                _XmWlFreePictureData(dead);
                free(dead);
                break;
            }
            pp = &(*pp)->hash_next;
        }
        break;
    }
    case 8: { /* RenderComposite */
        if (req_len < 36) break;
        uint8_t op = req[4];
        XmWlPicture *src = _XmWlFindPicture(srv, rd_u32(req + 8));
        XmWlPicture *msk = _XmWlFindPicture(srv, rd_u32(req + 12));
        XmWlPicture *dst = _XmWlFindPicture(srv, rd_u32(req + 16));
        int16_t sx = rd_i16(req + 20), sy = rd_i16(req + 22);
        int16_t mx = rd_i16(req + 24), my = rd_i16(req + 26);
        int16_t dx = rd_i16(req + 28), dy = rd_i16(req + 30);
        uint16_t w = rd_u16(req + 32), h = rd_u16(req + 34);
        if (!src || !dst || w == 0 || h == 0) break;

        int sdx = 0, sdy = 0, mdx = 0, mdy = 0, ddx = 0, ddy = 0;
        XmWlWindow *stop = NULL, *mtop = NULL, *dtop = NULL;
        pixman_region32_t dclip;
        pixman_image_t *simg = resolve_picture_image(srv, src, false, &sdx, &sdy, &stop, NULL);
        pixman_image_t *mimg = msk ? resolve_picture_image(srv, msk, false, &mdx, &mdy, &mtop, NULL) : NULL;
        pixman_image_t *dimg = resolve_picture_image(srv, dst, true, &ddx, &ddy, &dtop, &dclip);
        if (simg && dimg) {
            pixman_image_set_clip_region32(dimg, &dclip);
            pixman_image_composite32(pict_op_to_pixman(op), simg, mimg, dimg,
                                     sdx + sx, sdy + sy, mdx + mx, mdy + my,
                                     ddx + dx, ddy + dy, w, h);
            pixman_image_set_clip_region32(dimg, NULL);
            if (dtop) _XmWlDamageTop(dtop, ddx + dx, ddy + dy, w, h);
        }
        if (dimg) pixman_region32_fini(&dclip);
        break;
    }
    case 10: { /* RenderTrapezoids */
        if (req_len < 24) break;
        uint8_t op = req[4];
        XmWlPicture *src = _XmWlFindPicture(srv, rd_u32(req + 8));
        XmWlPicture *dst = _XmWlFindPicture(srv, rd_u32(req + 12));
        uint32_t mask_fmt = rd_u32(req + 16);
        int16_t sx = rd_i16(req + 20), sy = rd_i16(req + 22);
        size_t ntraps = (req_len - 24) / 40;
        if (!src || !dst || ntraps == 0) break;

        int sdx = 0, sdy = 0, ddx = 0, ddy = 0;
        XmWlWindow *stop = NULL, *dtop = NULL;
        pixman_region32_t dclip;
        pixman_image_t *simg = resolve_picture_image(srv, src, false, &sdx, &sdy, &stop, NULL);
        pixman_image_t *dimg = resolve_picture_image(srv, dst, true, &ddx, &ddy, &dtop, &dclip);
        if (simg && dimg) {
            pixman_trapezoid_t *traps = (pixman_trapezoid_t *)calloc(ntraps, sizeof(*traps));
            if (traps) {
                pixman_fixed_t ox = pixman_int_to_fixed(ddx);
                pixman_fixed_t oy = pixman_int_to_fixed(ddy);
                for (size_t i = 0; i < ntraps; ++i) {
                    const uint8_t *tp = req + 24 + i * 40;
                    traps[i].top = rd_i32(tp + 0) + oy;
                    traps[i].bottom = rd_i32(tp + 4) + oy;
                    traps[i].left.p1.x = rd_i32(tp + 8) + ox;
                    traps[i].left.p1.y = rd_i32(tp + 12) + oy;
                    traps[i].left.p2.x = rd_i32(tp + 16) + ox;
                    traps[i].left.p2.y = rd_i32(tp + 20) + oy;
                    traps[i].right.p1.x = rd_i32(tp + 24) + ox;
                    traps[i].right.p1.y = rd_i32(tp + 28) + oy;
                    traps[i].right.p2.x = rd_i32(tp + 32) + ox;
                    traps[i].right.p2.y = rd_i32(tp + 36) + oy;
                }
                pixman_format_code_t mfmt = (mask_fmt == XM_WL_PICTFMT_A1) ? PIXMAN_a1 : PIXMAN_a8;
                pixman_image_set_clip_region32(dimg, &dclip);
                pixman_composite_trapezoids(pict_op_to_pixman(op), simg, dimg, mfmt,
                                            sdx + sx, sdy + sy, ddx, ddy, (int)ntraps, traps);
                pixman_image_set_clip_region32(dimg, NULL);
                if (dtop) _XmWlDamageTop(dtop, 0, 0, dtop->width, dtop->height);
                free(traps);
            }
        }
        if (dimg) pixman_region32_fini(&dclip);
        break;
    }
    case 11: { /* RenderTriangles */
        if (req_len < 24) break;
        uint8_t op = req[4];
        XmWlPicture *src = _XmWlFindPicture(srv, rd_u32(req + 8));
        XmWlPicture *dst = _XmWlFindPicture(srv, rd_u32(req + 12));
        uint32_t mask_fmt = rd_u32(req + 16);
        int16_t sx = rd_i16(req + 20), sy = rd_i16(req + 22);
        size_t ntris = (req_len - 24) / 24;
        if (!src || !dst || ntris == 0) break;

        int sdx = 0, sdy = 0, ddx = 0, ddy = 0;
        XmWlWindow *stop = NULL, *dtop = NULL;
        pixman_region32_t dclip;
        pixman_image_t *simg = resolve_picture_image(srv, src, false, &sdx, &sdy, &stop, NULL);
        pixman_image_t *dimg = resolve_picture_image(srv, dst, true, &ddx, &ddy, &dtop, &dclip);
        if (simg && dimg) {
            pixman_triangle_t *tris = (pixman_triangle_t *)calloc(ntris, sizeof(*tris));
            if (tris) {
                pixman_fixed_t ox = pixman_int_to_fixed(ddx);
                pixman_fixed_t oy = pixman_int_to_fixed(ddy);
                for (size_t i = 0; i < ntris; ++i) {
                    const uint8_t *tp = req + 24 + i * 24;
                    tris[i].p1.x = rd_i32(tp + 0) + ox;  tris[i].p1.y = rd_i32(tp + 4) + oy;
                    tris[i].p2.x = rd_i32(tp + 8) + ox;  tris[i].p2.y = rd_i32(tp + 12) + oy;
                    tris[i].p3.x = rd_i32(tp + 16) + ox; tris[i].p3.y = rd_i32(tp + 20) + oy;
                }
                pixman_format_code_t mfmt = (mask_fmt == XM_WL_PICTFMT_A1) ? PIXMAN_a1 : PIXMAN_a8;
                pixman_image_set_clip_region32(dimg, &dclip);
                pixman_composite_triangles(pict_op_to_pixman(op), simg, dimg, mfmt,
                                           sdx + sx, sdy + sy, ddx, ddy, (int)ntris, tris);
                pixman_image_set_clip_region32(dimg, NULL);
                if (dtop) _XmWlDamageTop(dtop, 0, 0, dtop->width, dtop->height);
                free(tris);
            }
        }
        if (dimg) pixman_region32_fini(&dclip);
        break;
    }
    case 17: { /* RenderCreateGlyphSet */
        if (req_len < 12) break;
        uint32_t gsid = rd_u32(req + 4);
        uint32_t fmt = rd_u32(req + 8);
        XmWlGlyphSet *gs = (XmWlGlyphSet *)calloc(1, sizeof(*gs));
        if (!gs) break;
        gs->id = gsid;
        gs->format_id = fmt;
        gs->ref_count = 1;
        uint32_t b = gsid & 63u;
        gs->hash_next = srv->gs_buckets[b];
        srv->gs_buckets[b] = gs;
        break;
    }
    case 18: { /* RenderReferenceGlyphSet */
        if (req_len < 12) break;
        uint32_t new_id = rd_u32(req + 4);
        XmWlGlyphSet *orig = _XmWlFindGlyphSet(srv, rd_u32(req + 8));
        if (orig) {
            XmWlGlyphSet *gs = (XmWlGlyphSet *)calloc(1, sizeof(*gs));
            if (gs) {
                gs->id = new_id;
                gs->format_id = orig->format_id;
                gs->ref_count = 1;
                uint32_t b = new_id & 63u;
                gs->hash_next = srv->gs_buckets[b];
                srv->gs_buckets[b] = gs;
            }
        }
        break;
    }
    case 19: { /* RenderFreeGlyphSet */
        if (req_len < 8) break;
        uint32_t gsid = rd_u32(req + 4);
        uint32_t b = gsid & 63u;
        XmWlGlyphSet **pp = &srv->gs_buckets[b];
        while (*pp) {
            if ((*pp)->id == gsid) {
                XmWlGlyphSet *dead = *pp;
                *pp = dead->hash_next;
                _XmWlFreeGlyphSetData(dead);
                free(dead);
                break;
            }
            pp = &(*pp)->hash_next;
        }
        break;
    }
    case 20: { /* RenderAddGlyphs */
        if (req_len < 12) break;
        XmWlGlyphSet *gs = _XmWlFindGlyphSet(srv, rd_u32(req + 4));
        uint32_t nglyphs = rd_u32(req + 8);
        size_t headers_end = 12u + (size_t)nglyphs * 4u + (size_t)nglyphs * 12u;
        if (!gs || nglyphs > 65536 || headers_end > req_len) break;

        const uint8_t *gids = req + 12;
        const uint8_t *ginfos = gids + (size_t)nglyphs * 4u;
        const uint8_t *img_bytes = ginfos + (size_t)nglyphs * 12u;
        size_t img_rem = req_len - headers_end;

        for (uint32_t i = 0; i < nglyphs; ++i) {
            uint32_t gid = rd_u32(gids + (size_t)i * 4u);
            const uint8_t *gi = ginfos + (size_t)i * 12u;
            uint16_t gw = rd_u16(gi + 0), gh = rd_u16(gi + 2);
            int16_t gx = rd_i16(gi + 4), gy = rd_i16(gi + 6);
            int16_t gx_off = rd_i16(gi + 8), gy_off = rd_i16(gi + 10);

            size_t src_stride = 0;
            if (gs->format_id == XM_WL_PICTFMT_ARGB32)
                src_stride = (size_t)gw * 4u;
            else if (gs->format_id == XM_WL_PICTFMT_A1)
                src_stride = (((size_t)gw + 31u) / 32u) * 4u;
            else
                src_stride = (((size_t)gw + 3u) / 4u) * 4u; /* A8 */

            size_t glyph_bytes = src_stride * (size_t)gh;
            glyph_bytes = (glyph_bytes + 3u) & ~3u; /* XRender pads each glyph to 4 bytes */
            if (glyph_bytes > img_rem) glyph_bytes = img_rem;

            XmWlGlyph *g = find_glyph(gs, gid);
            if (!g) {
                g = (XmWlGlyph *)calloc(1, sizeof(*g));
                if (!g) break;
                g->glyph_id = gid;
                g->next = gs->buckets[gid & 63u];
                gs->buckets[gid & 63u] = g;
            } else {
                if (g->img) { pixman_image_unref(g->img); g->img = NULL; }
                free(g->bits); g->bits = NULL;
            }
            g->width = (int16_t)gw;
            g->height = (int16_t)gh;
            g->x = gx;
            g->y = gy;
            g->x_off = gx_off;
            g->y_off = gy_off;

            if (gw > 0 && gh > 0) {
                g->bits = (uint32_t *)calloc((size_t)gw * (size_t)gh, 4u);
                if (g->bits) {
                    for (uint16_t r = 0; r < gh; ++r) {
                        if ((size_t)r * src_stride >= img_rem) break;
                        const uint8_t *srow = img_bytes + (size_t)r * src_stride;
                        for (uint16_t c = 0; c < gw; ++c) {
                            uint32_t argb = 0;
                            if (gs->format_id == XM_WL_PICTFMT_ARGB32) {
                                argb = rd_u32(srow + (size_t)c * 4u);
                            } else if (gs->format_id == XM_WL_PICTFMT_A1) {
                                bool on = ((srow[c >> 3] >> (c & 7u)) & 1u) != 0;
                                argb = on ? 0xffffffffu : 0x00000000u;
                            } else {
                                uint8_t a = srow[c];
                                argb = ((uint32_t)a << 24) | ((uint32_t)a << 16) | ((uint32_t)a << 8) | a;
                            }
                            g->bits[(size_t)r * (size_t)gw + (size_t)c] = argb;
                        }
                    }
                    g->img = pixman_image_create_bits(PIXMAN_a8r8g8b8, gw, gh, g->bits, (int)gw * 4);
                }
            }
            img_bytes += glyph_bytes;
            img_rem -= glyph_bytes;
        }
        break;
    }
    case 22: /* RenderFreeGlyphs */
        break;
    case 23: /* RenderCompositeGlyphs8 */
    case 24: /* RenderCompositeGlyphs16 */
    case 25: { /* RenderCompositeGlyphs32 */
        if (req_len < 28) break;
        uint8_t op = req[4];
        XmWlPicture *src = _XmWlFindPicture(srv, rd_u32(req + 8));
        XmWlPicture *dst = _XmWlFindPicture(srv, rd_u32(req + 12));
        XmWlGlyphSet *gs = _XmWlFindGlyphSet(srv, rd_u32(req + 20));
        int16_t x_src = rd_i16(req + 24), y_src = rd_i16(req + 26);
        if (!src || !dst || !gs) break;

        int sdx = 0, sdy = 0, ddx = 0, ddy = 0;
        XmWlWindow *stop = NULL, *dtop = NULL;
        pixman_region32_t dclip;
        pixman_image_t *simg = resolve_picture_image(srv, src, false, &sdx, &sdy, &stop, NULL);
        pixman_image_t *dimg = resolve_picture_image(srv, dst, true, &ddx, &ddy, &dtop, &dclip);
        if (!simg || !dimg) {
            if (dimg) pixman_region32_fini(&dclip);
            break;
        }
        pixman_image_set_clip_region32(dimg, &dclip);

        size_t elem_size = (minor_op == 23) ? 1u : (minor_op == 24 ? 2u : 4u);
        size_t pos = 28;
        int pen_x = 0, pen_y = 0;
        bool first_elt = true;
        int origin_x = 0, origin_y = 0;

        while (pos + 8 <= req_len) {
            uint8_t len_elt = req[pos];
            int16_t dx_elt = rd_i16(req + pos + 4);
            int16_t dy_elt = rd_i16(req + pos + 6);
            pos += 8;

            if (len_elt == 255) {
                if (pos + 4 > req_len) break;
                XmWlGlyphSet *ngs = _XmWlFindGlyphSet(srv, rd_u32(req + pos));
                if (ngs) gs = ngs;
                pos += 4;
                continue;
            }
            pen_x += dx_elt;
            pen_y += dy_elt;
            if (first_elt) {
                origin_x = pen_x;
                origin_y = pen_y;
                first_elt = false;
            }
            size_t raw_bytes = (size_t)len_elt * elem_size;
            size_t padded_bytes = (raw_bytes + 3u) & ~3u;
            if (pos + raw_bytes > req_len) break;

            for (uint8_t gi = 0; gi < len_elt; ++gi) {
                uint32_t gid = (elem_size == 1) ? req[pos + gi]
                             : (elem_size == 2 ? rd_u16(req + pos + (size_t)gi * 2u)
                                               : rd_u32(req + pos + (size_t)gi * 4u));
                XmWlGlyph *g = find_glyph(gs, gid);
                if (g) {
                    if (g->img && g->width > 0 && g->height > 0) {
                        int gx = pen_x - g->x;
                        int gy = pen_y - g->y;
                        if (gs->format_id == XM_WL_PICTFMT_ARGB32 && !src->component_alpha) {
                            /* Color emoji or subpixel ARGB glyph */
                            pixman_image_set_component_alpha(g->img, true);
                        }
                        pixman_image_composite32(
                            pict_op_to_pixman(op), simg, g->img, dimg,
                            sdx + x_src + (gx - origin_x), sdy + y_src + (gy - origin_y),
                            0, 0, ddx + gx, ddy + gy, g->width, g->height);
                        if (dtop) _XmWlDamageTop(dtop, ddx + gx, ddy + gy, g->width, g->height);
                    }
                    pen_x += g->x_off;
                    pen_y += g->y_off;
                }
            }
            pos += padded_bytes;
        }
        pixman_image_set_clip_region32(dimg, NULL);
        pixman_region32_fini(&dclip);
        break;
    }
    case 26: { /* RenderFillRectangles */
        if (req_len < 20) break;
        uint8_t op = req[4];
        XmWlPicture *dst = _XmWlFindPicture(srv, rd_u32(req + 8));
        pixman_color_t col = {
            .red   = rd_u16(req + 12),
            .green = rd_u16(req + 14),
            .blue  = rd_u16(req + 16),
            .alpha = rd_u16(req + 18),
        };
        size_t nrects = (req_len - 20) / 8;
        if (!dst || nrects == 0) break;

        int ddx = 0, ddy = 0;
        XmWlWindow *dtop = NULL;
        pixman_region32_t dclip;
        pixman_image_t *dimg = resolve_picture_image(srv, dst, true, &ddx, &ddy, &dtop, &dclip);
        if (dimg) {
            pixman_rectangle16_t *prects = (pixman_rectangle16_t *)calloc(nrects, sizeof(*prects));
            if (prects) {
                for (size_t i = 0; i < nrects; ++i) {
                    prects[i].x = (int16_t)(ddx + rd_i16(req + 20 + i * 8 + 0));
                    prects[i].y = (int16_t)(ddy + rd_i16(req + 20 + i * 8 + 2));
                    prects[i].width = rd_u16(req + 20 + i * 8 + 4);
                    prects[i].height = rd_u16(req + 20 + i * 8 + 6);
                    if (dtop) _XmWlDamageTop(dtop, prects[i].x, prects[i].y, prects[i].width, prects[i].height);
                }
                pixman_image_set_clip_region32(dimg, &dclip);
                pixman_image_fill_rectangles(pict_op_to_pixman(op), dimg, &col, (int)nrects, prects);
                pixman_image_set_clip_region32(dimg, NULL);
                free(prects);
            }
            pixman_region32_fini(&dclip);
        }
        break;
    }
    case 28: { /* RenderSetPictureTransform */
        if (req_len < 44) break;
        XmWlPicture *pic = _XmWlFindPicture(srv, rd_u32(req + 4));
        if (pic) {
            for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c)
                    pic->transform.matrix[r][c] = rd_i32(req + 8 + (r * 3 + c) * 4);
            pic->has_transform = true;
        }
        break;
    }
    case 29: { /* RenderQueryFilters */
        /* 2 filters: "nearest" (7), "bilinear" (8) -> 1+7 + 1+8 = 17 -> padded to 20 bytes */
        uint8_t extra[20];
        memset(extra, 0, sizeof(extra));
        extra[0] = 7; memcpy(extra + 1, "nearest", 7);
        extra[8] = 8; memcpy(extra + 9, "bilinear", 8);
        uint8_t rep[32];
        memset(rep, 0, sizeof(rep));
        rep[0] = 1;
        wr_u16(rep + 2, cli->seq);
        wr_u32(rep + 4, sizeof(extra) / 4);
        wr_u32(rep + 8, 0); /* num_aliases */
        wr_u32(rep + 12, 2); /* num_filters */
        _XmWlSendReply(cli, rep, extra, sizeof(extra));
        break;
    }
    case 30: /* RenderSetPictureFilter */
    case 27: /* RenderCreateCursor */
    case 31: /* RenderCreateAnimCursor */
        break;
    case 33: { /* RenderCreateSolidFill */
        if (req_len < 16) break;
        uint32_t pid = rd_u32(req + 4);
        pixman_color_t col = {
            .red   = rd_u16(req + 8),
            .green = rd_u16(req + 10),
            .blue  = rd_u16(req + 12),
            .alpha = rd_u16(req + 14),
        };
        XmWlPicture *pic = (XmWlPicture *)calloc(1, sizeof(*pic));
        if (pic) {
            pic->id = pid;
            pic->repeat = 1;
            pic->source_img = pixman_image_create_solid_fill(&col);
            uint32_t b = pid & 255u;
            pic->hash_next = srv->pic_buckets[b];
            srv->pic_buckets[b] = pic;
        }
        break;
    }
    case 34: { /* RenderCreateLinearGradient */
        if (req_len < 28) break;
        uint32_t pid = rd_u32(req + 4);
        pixman_point_fixed_t p1 = { rd_i32(req + 8), rd_i32(req + 12) };
        pixman_point_fixed_t p2 = { rd_i32(req + 16), rd_i32(req + 20) };
        uint32_t nstops = rd_u32(req + 24);
        if (nstops == 0 || nstops > 1024 || 28u + (size_t)nstops * 12u > req_len) break;
        pixman_gradient_stop_t *stops = (pixman_gradient_stop_t *)calloc(nstops, sizeof(*stops));
        if (!stops) break;
        const uint8_t *sfixed = req + 28;
        const uint8_t *scols = sfixed + (size_t)nstops * 4u;
        for (uint32_t i = 0; i < nstops; ++i) {
            stops[i].x = rd_i32(sfixed + (size_t)i * 4u);
            stops[i].color.red   = rd_u16(scols + (size_t)i * 8u + 0);
            stops[i].color.green = rd_u16(scols + (size_t)i * 8u + 2);
            stops[i].color.blue  = rd_u16(scols + (size_t)i * 8u + 4);
            stops[i].color.alpha = rd_u16(scols + (size_t)i * 8u + 6);
        }
        XmWlPicture *pic = (XmWlPicture *)calloc(1, sizeof(*pic));
        if (pic) {
            pic->id = pid;
            pic->repeat = 2; /* Pad */
            pic->source_img = pixman_image_create_linear_gradient(&p1, &p2, stops, (int)nstops);
            uint32_t b = pid & 255u;
            pic->hash_next = srv->pic_buckets[b];
            srv->pic_buckets[b] = pic;
        }
        free(stops);
        break;
    }
    case 35: { /* RenderCreateRadialGradient */
        if (req_len < 36) break;
        uint32_t pid = rd_u32(req + 4);
        pixman_point_fixed_t inner = { rd_i32(req + 8), rd_i32(req + 12) };
        pixman_point_fixed_t outer = { rd_i32(req + 16), rd_i32(req + 20) };
        pixman_fixed_t ir = rd_i32(req + 24);
        pixman_fixed_t orad = rd_i32(req + 28);
        uint32_t nstops = rd_u32(req + 32);
        if (nstops == 0 || nstops > 1024 || 36u + (size_t)nstops * 12u > req_len) break;
        pixman_gradient_stop_t *stops = (pixman_gradient_stop_t *)calloc(nstops, sizeof(*stops));
        if (!stops) break;
        const uint8_t *sfixed = req + 36;
        const uint8_t *scols = sfixed + (size_t)nstops * 4u;
        for (uint32_t i = 0; i < nstops; ++i) {
            stops[i].x = rd_i32(sfixed + (size_t)i * 4u);
            stops[i].color.red   = rd_u16(scols + (size_t)i * 8u + 0);
            stops[i].color.green = rd_u16(scols + (size_t)i * 8u + 2);
            stops[i].color.blue  = rd_u16(scols + (size_t)i * 8u + 4);
            stops[i].color.alpha = rd_u16(scols + (size_t)i * 8u + 6);
        }
        XmWlPicture *pic = (XmWlPicture *)calloc(1, sizeof(*pic));
        if (pic) {
            pic->id = pid;
            pic->repeat = 2;
            pic->source_img = pixman_image_create_radial_gradient(&inner, &outer, ir, orad, stops, (int)nstops);
            uint32_t b = pid & 255u;
            pic->hash_next = srv->pic_buckets[b];
            srv->pic_buckets[b] = pic;
        }
        free(stops);
        break;
    }
    default:
        break;
    }
}

#endif /* HAVE_WAYLAND_BACKEND */
