/*
 * Bit-exact SIMD vs. scalar parity and Wayland XRender A8 glyph-atlas /
 * rounded-bevel / buffered-painting regression qualification.
 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/ProgressBar.h>
#include <Xm/Wayland.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrender.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); exit(1); } } while (0)

extern void _XmWaylandRenderSetSimdEnabled(bool enabled);
extern bool _XmWaylandRenderGetSimdEnabled(void);
extern const char *_XmWaylandRenderGetSimdBackend(void);
extern void _XmWaylandRenderResetStats(void);
extern void _XmWaylandRenderGetStats(uint64_t *out_atlas_glyphs,
                                     uint64_t *out_atlas_pages,
                                     uint64_t *out_simd_glyph_blits,
                                     uint64_t *out_simd_span_ops);
extern void _XmWaylandSimdFillRow32(uint32_t *dst, uint32_t pixel, size_t count);
extern void _XmWaylandSimdOverSpanSolid(uint32_t *dst, uint32_t pm_src, size_t count, uint8_t depth);
extern void _XmWaylandSimdBlitGlyphA8Over(uint32_t *dst, int dst_stride_px,
                                          const uint8_t *mask, int mask_stride,
                                          int w, int h, uint32_t pm_src, uint8_t depth);

static uint32_t xorshift32(uint32_t *s) {
    uint32_t x = *s;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *s = x;
    return x;
}

static uint32_t make_premul(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    uint32_t pr = ((uint32_t)r * a + 127u) / 255u;
    uint32_t pg = ((uint32_t)g * a + 127u) / 255u;
    uint32_t pb = ((uint32_t)b * a + 127u) / 255u;
    return ((uint32_t)a << 24) | (pr << 16) | (pg << 8) | pb;
}

static void verify_direct_simd_parity(void) {
    _XmWaylandRenderSetSimdEnabled(true);
    const char *backend = _XmWaylandRenderGetSimdBackend();
    CHECK(backend != NULL);
#if defined(__aarch64__) || defined(__ARM_NEON)
    CHECK(strcmp(backend, "neon") == 0);
#elif defined(__x86_64__) || defined(__SSE2__)
    CHECK(strcmp(backend, "avx2") == 0 || strcmp(backend, "sse2") == 0);
#endif
    _XmWaylandRenderSetSimdEnabled(false);
    CHECK(strcmp(_XmWaylandRenderGetSimdBackend(), "scalar") == 0);

    const size_t lengths[] = {0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 32, 33, 73, 137};
    const uint8_t alphas[] = {0, 1, 17, 64, 127, 128, 199, 254, 255};
    const uint8_t depths[] = {24, 32};
    uint32_t rng = 0x9e3779b9u;

    uint32_t buf_simd[256];
    uint32_t buf_scal[256];

    /* 1. FillRow32 & OverSpanSolid across alignments, lengths, alphas, and depths */
    for (size_t off = 0; off < 4; ++off) {
        for (size_t li = 0; li < sizeof(lengths) / sizeof(lengths[0]); ++li) {
            size_t len = lengths[li];
            for (size_t i = 0; i < 256; ++i)
                buf_simd[i] = buf_scal[i] = xorshift32(&rng);
            uint32_t fill_px = 0xff336699u ^ xorshift32(&rng);
            _XmWaylandRenderSetSimdEnabled(true);
            _XmWaylandSimdFillRow32(buf_simd + off, fill_px, len);
            _XmWaylandRenderSetSimdEnabled(false);
            _XmWaylandSimdFillRow32(buf_scal + off, fill_px, len);
            CHECK(memcmp(buf_simd, buf_scal, sizeof(buf_simd)) == 0);

            for (size_t ai = 0; ai < sizeof(alphas) / sizeof(alphas[0]); ++ai) {
                for (size_t di = 0; di < 2; ++di) {
                    for (size_t i = 0; i < 256; ++i)
                        buf_simd[i] = buf_scal[i] = xorshift32(&rng);
                    uint32_t pm = make_premul(alphas[ai], 231, 108, 49);
                    _XmWaylandRenderSetSimdEnabled(true);
                    _XmWaylandSimdOverSpanSolid(buf_simd + off, pm, len, depths[di]);
                    _XmWaylandRenderSetSimdEnabled(false);
                    _XmWaylandSimdOverSpanSolid(buf_scal + off, pm, len, depths[di]);
                    CHECK(memcmp(buf_simd, buf_scal, sizeof(buf_simd)) == 0);
                }
            }
        }
    }

    /* 2. BlitGlyphA8Over across all-zero 4x runs, all-255 4x runs, 0..255 ramps, and random masks */
    const int widths[] = {1, 3, 4, 5, 8, 13, 29, 67};
    const int heights[] = {1, 5, 17};
    uint32_t img_simd[96 * 24];
    uint32_t img_scal[96 * 24];
    uint8_t mask[80 * 24];

    for (size_t wi = 0; wi < sizeof(widths) / sizeof(widths[0]); ++wi) {
        for (size_t hi = 0; hi < sizeof(heights) / sizeof(heights[0]); ++hi) {
            int gw = widths[wi], gh = heights[hi];
            for (int pat = 0; pat < 4; ++pat) {
                for (size_t i = 0; i < sizeof(mask); ++i) {
                    if (pat == 0) mask[i] = (uint8_t)((i / 4) % 2 == 0 ? 0 : 255);
                    else if (pat == 1) mask[i] = (uint8_t)(i & 0xffu);
                    else if (pat == 2) mask[i] = (uint8_t)((xorshift32(&rng) & 3u) == 0 ? 0 : (xorshift32(&rng) & 0xffu));
                    else mask[i] = 255u;
                }
                for (size_t ai = 0; ai < sizeof(alphas) / sizeof(alphas[0]); ++ai) {
                    for (size_t di = 0; di < 2; ++di) {
                        for (size_t i = 0; i < 96 * 24; ++i)
                            img_simd[i] = img_scal[i] = xorshift32(&rng);
                        uint32_t pm = make_premul(alphas[ai], 19, 185, 244);
                        _XmWaylandRenderSetSimdEnabled(true);
                        _XmWaylandSimdBlitGlyphA8Over(img_simd + 3, 96, mask + 1, 80, gw, gh, pm, depths[di]);
                        _XmWaylandRenderSetSimdEnabled(false);
                        _XmWaylandSimdBlitGlyphA8Over(img_scal + 3, 96, mask + 1, 80, gw, gh, pm, depths[di]);
                        CHECK(memcmp(img_simd, img_scal, sizeof(img_simd)) == 0);
                    }
                }
            }
        }
    }
    _XmWaylandRenderSetSimdEnabled(true);
}

static void pump_ms(XtAppContext app, Display *dpy, int ms) {
    struct timespec t0, now;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (;;) {
        XFlush(dpy);
        while (XtAppPending(app))
            XtAppProcessEvent(app, XtIMAll);
        clock_gettime(CLOCK_MONOTONIC, &now);
        long el = (now.tv_sec - t0.tv_sec) * 1000L + (now.tv_nsec - t0.tv_nsec) / 1000000L;
        if (el >= ms)
            break;
        struct timespec sl = {0, 8 * 1000 * 1000L};
        nanosleep(&sl, NULL);
    }
}

static uint64_t hash_drawable(Display *dpy, Drawable d, unsigned int w, unsigned int h) {
    XImage *im = XGetImage(dpy, d, 0, 0, w, h, AllPlanes, ZPixmap);
    CHECK(im != NULL);
    uint64_t fnv = UINT64_C(14695981039346656037);
    for (unsigned int y = 0; y < h; ++y) {
        for (unsigned int x = 0; x < w; ++x) {
            uint32_t p = (uint32_t)XGetPixel(im, (int)x, (int)y) & 0x00ffffffu;
            fnv ^= p;
            fnv *= UINT64_C(1099511628211);
        }
    }
    XDestroyImage(im);
    return fnv;
}

static uint64_t render_scene_once(XtAppContext app, Display *dpy, bool simd_on) {
    _XmWaylandRenderSetSimdEnabled(simd_on);
    CHECK(XmSetAppearanceProfile(dpy, XmAPPEARANCE_MODERN_DARK));

    Widget shell = XtVaAppCreateShell(
        "simdScene", "SimdScene", topLevelShellWidgetClass, dpy,
        XmNwidth, 360, XmNheight, 220, NULL);
    Widget form = XtVaCreateManagedWidget("form", xmFormWidgetClass, shell,
                                          XmNwidth, 360, XmNheight, 220, NULL);

    XmString s1 = XmStringCreateLocalized("Modern Bevel & Atlas Text 0123456789");
    Widget btn = XtVaCreateManagedWidget(
        "btn", xmPushButtonWidgetClass, form,
        XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 12,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 12,
        XmNwidth, 320, XmNheight, 40,
        XmNlabelString, s1, NULL);
    XmStringFree(s1);
    (void)btn;

    Widget prog = XtVaCreateManagedWidget(
        "prog", xmProgressBarWidgetClass, form,
        XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 64,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 12,
        XmNwidth, 320, XmNheight, 28,
        XmNprogressValue, 65, NULL);
    (void)prog;

    XtRealizeWidget(shell);
    pump_ms(app, dpy, 180);

    /* Also exercise XRender GlyphSet A8 atlas + ReferenceGlyphSet + FreeGlyphs + FillRectangles */
    Pixmap pm = XCreatePixmap(dpy, XtWindow(shell), 160, 48, 24);
    XRenderPictFormat *fmt_rgb = XRenderFindStandardFormat(dpy, PictStandardRGB24);
    XRenderPictFormat *fmt_a8  = XRenderFindStandardFormat(dpy, PictStandardA8);
    CHECK(fmt_rgb && fmt_a8);

    Picture dst_pic = XRenderCreatePicture(dpy, pm, fmt_rgb, 0, NULL);
    XRenderColor bg_col = {0x1800, 0x2200, 0x3000, 0xffff};
    XRenderFillRectangle(dpy, PictOpSrc, dst_pic, &bg_col, 0, 0, 160, 48);

    /* Translucent rounded/overlay span fills */
    XRenderColor ov_col = {0x4000, 0x6000, 0x8000, 0x8000};
    XRectangle ov_rects[3] = {{4, 4, 152, 12}, {6, 16, 148, 12}, {8, 28, 144, 12}};
    XRenderFillRectangles(dpy, PictOpOver, dst_pic, &ov_col, ov_rects, 3);

    GlyphSet gs = XRenderCreateGlyphSet(dpy, fmt_a8);
    Glyph gids[2] = {65, 66};
    XGlyphInfo ginfos[2] = {
        {8, 10, 0, 8, 10, 0},
        {9, 10, 0, 8, 11, 0},
    };
    uint8_t gbytes[8 * 10 + 12 * 10];
    for (size_t i = 0; i < sizeof(gbytes); ++i)
        gbytes[i] = (uint8_t)((i * 29u) & 0xffu);
    XRenderAddGlyphs(dpy, gs, gids, ginfos, 2, (const char *)gbytes, (int)sizeof(gbytes));

    /* ReferenceGlyphSet must share the underlying A8 atlas! */
    GlyphSet gs_ref = XRenderReferenceGlyphSet(dpy, gs);
    XRenderColor fg_col = {0xf000, 0xf400, 0xff00, 0xffff};
    Picture fg_pic = XRenderCreateSolidFill(dpy, &fg_col);
    const char str_ab[4] = {65, 66, 65, 66};
    XRenderCompositeString8(dpy, PictOpOver, fg_pic, dst_pic, fmt_a8, gs_ref, 0, 0, 14, 24, str_ab, 4);

    /* Free glyph 66 and verify glyph 65 still renders while 66 is a clean no-op */
    Glyph free_id = 66;
    XRenderFreeGlyphs(dpy, gs, &free_id, 1);
    XRenderCompositeString8(dpy, PictOpOver, fg_pic, dst_pic, fmt_a8, gs_ref, 0, 0, 70, 24, str_ab, 4);
    XSync(dpy, False);

    uint64_t h_pm = hash_drawable(dpy, pm, 160, 48);
    uint64_t h_win = hash_drawable(dpy, XtWindow(form), 340, 120);

    XRenderFreePicture(dpy, fg_pic);
    XRenderFreePicture(dpy, dst_pic);
    XRenderFreeGlyphSet(dpy, gs_ref);
    XRenderFreeGlyphSet(dpy, gs);
    XFreePixmap(dpy, pm);
    XtDestroyWidget(shell);
    pump_ms(app, dpy, 40);

    return h_pm ^ (h_win * UINT64_C(0x9e3779b97f4a7c15));
}

int main(int argc, char **argv) {
    verify_direct_simd_parity();

    XtToolkitInitialize();
    XtAppContext app = XtCreateApplicationContext();
    Display *dpy = XtOpenDisplay(app, NULL, "simdCheck", "SimdCheck", NULL, 0, &argc, argv);
    CHECK(dpy != NULL);
    CHECK(XmWaylandIsActive(dpy));

    _XmWaylandRenderResetStats();
    uint64_t hash_simd = render_scene_once(app, dpy, true);
    uint64_t atlas_glyphs = 0, atlas_pages = 0, simd_blits = 0, simd_spans = 0;
    _XmWaylandRenderGetStats(&atlas_glyphs, &atlas_pages, &simd_blits, &simd_spans);
    CHECK(atlas_glyphs >= 2);
    CHECK(atlas_pages >= 1);
    CHECK(simd_blits >= 6);
    CHECK(simd_spans >= 4);

    uint64_t hash_scalar = render_scene_once(app, dpy, false);
    CHECK(hash_simd == hash_scalar);
    _XmWaylandRenderSetSimdEnabled(true);

    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    printf("PASS WaylandRender A8 glyph-atlas + SIMD (%s) bit-exact parity (atlas_glyphs=%llu pages=%llu simd_blits=%llu simd_spans=%llu)\n",
           _XmWaylandRenderGetSimdBackend(),
           (unsigned long long)atlas_glyphs,
           (unsigned long long)atlas_pages,
           (unsigned long long)simd_blits,
           (unsigned long long)simd_spans);
    return 0;
}
