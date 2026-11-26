/*
 * Public Wayland integration and subsurface embedding API for libXm.
 *
 * All functions are safe to call on X11, XWayland, macOS, and non-Wayland
 * builds: when the native Wayland backend is inactive or disabled at build
 * time, query functions return False without modifying output pointers and
 * lock/unlock calls are no-ops.
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _XmWayland_h
#define _XmWayland_h

#include <X11/Intrinsic.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct wl_display;
struct wl_compositor;
struct wl_subcompositor;
struct wl_shm;
struct wl_surface;
struct wl_buffer;
struct wp_viewporter;
struct zwp_linux_dmabuf_v1;

/* Plane descriptor for zero-copy XmWaylandImportDmabuf() subsurface import. */
typedef struct {
    int fd;
    uint32_t offset;
    uint32_t stride;
    uint64_t modifier;
} XmWaylandDmabufPlane;

/* Optional flags bitfield for XmWaylandImportDmabuf() (matches zwp_linux_buffer_params_v1.flags). */
#define XM_WAYLAND_DMABUF_FLAG_Y_INVERT     1u
#define XM_WAYLAND_DMABUF_FLAG_INTERLACED   2u
#define XM_WAYLAND_DMABUF_FLAG_BOTTOM_FIRST 4u

/* Transfer function & color primaries constants for XmWaylandSetSurfaceHdrMetadata(). */
#define XM_WAYLAND_COLOR_TF_SRGB       1u
#define XM_WAYLAND_COLOR_TF_BT1886     2u
#define XM_WAYLAND_COLOR_TF_ST2084_PQ  3u
#define XM_WAYLAND_COLOR_TF_HLG        4u

#define XM_WAYLAND_COLOR_PRIMARIES_SRGB       1u
#define XM_WAYLAND_COLOR_PRIMARIES_DISPLAY_P3 2u
#define XM_WAYLAND_COLOR_PRIMARIES_BT2020     3u

/* Parametric HDR10 / WCG surface colorimetry descriptor (wp_color_management_v1). */
typedef struct {
    uint32_t transfer_function; /* XM_WAYLAND_COLOR_TF_* */
    uint32_t primaries;         /* XM_WAYLAND_COLOR_PRIMARIES_* */
    Boolean has_mastering_display_primaries;
    /* CIE 1931 xy chromaticity coordinates * 1,000,000 (e.g. D65 = 312700, 329000) */
    int32_t display_primaries_rx, display_primaries_ry;
    int32_t display_primaries_gx, display_primaries_gy;
    int32_t display_primaries_bx, display_primaries_by;
    int32_t white_point_x, white_point_y;
    Boolean has_mastering_luminance;
    uint32_t min_luminance_10000; /* min luminance (cd/m^2) * 10,000 */
    uint32_t max_luminance;       /* max luminance (cd/m^2) */
    uint32_t max_cll;             /* 0 = unset; MaxCLL (cd/m^2) */
    uint32_t max_fall;            /* 0 = unset; MaxFALL (cd/m^2) */
} XmWaylandHdrMetadata;

/*
 * Optional weak symbol an application executable may define as non-zero
 * (e.g. `extern const int ck_motif_prefer_x11 = 1;`) to keep standard X11 /
 * XWayland transport when a real X11 DISPLAY is present, even if the session
 * sets CK_MOTIF_BACKEND=wayland. When DISPLAY is unset (--no-xwayland), the
 * native Wayland backend still activates if WAYLAND_DISPLAY is available.
 */
#if defined(__GNUC__) || defined(__clang__)
extern const int ck_motif_prefer_x11 __attribute__((weak));
#endif

/* Returns True when `display` is backed by libXm's native Wayland runtime. */
extern Boolean XmWaylandIsActive(Display *display);

/*
 * Acquire/release libXm's internal Wayland client mutex around direct
 * libwayland-client requests (wl_compositor, wl_subcompositor, wl_surface,
 * wp_viewporter, wl_display_flush) sharing libXm's wl_display connection.
 */
extern void XmWaylandLock(Display *display);
extern void XmWaylandUnlock(Display *display);

/*
 * Retrieve libXm's active Wayland client globals. Any output pointer may be
 * NULL if not needed. Caller should hold XmWaylandLock(display) while issuing
 * Wayland requests on the returned objects.
 */
extern Boolean XmWaylandGetDisplayHandles(
    Display *display,
    struct wl_display **wl_display_return,
    struct wl_compositor **wl_compositor_return,
    struct wl_subcompositor **wl_subcompositor_return,
    struct wl_shm **wl_shm_return,
    struct wp_viewporter **wp_viewporter_return);

/*
 * Resolve a realized widget's enclosing top-level wl_surface and compute the
 * widget's logical bounds (x, y, width, height) relative to that top-level
 * surface along with the surface's current fractional/output scale factor.
 *
 * Recommended wl_subsurface embedding contract (e.g. CEF OSR or video):
 *   1. Hold XmWaylandLock(dpy) while creating/updating the wl_surface and
 *      wl_subsurface via wl_subcompositor_get_subsurface(sub, child, top_surf).
 *   2. Call wl_subsurface_set_desync(sub), wl_subsurface_place_above(sub, top_surf),
 *      and wl_subsurface_set_position(sub, x, y).
 *   3. Set an empty input region on `child` (wl_compositor_create_region +
 *      wl_surface_set_input_region) so pointer, wheel, keyboard focus, popup
 *      menus, and zwp_text_input_v3 IME remain routed through the Motif widget
 *      and forwarded by the application.
 *   4. On widget ConfigureNotify / XmNresizeCallback or scale change, re-query
 *      XmWaylandGetWidgetSurface and update subsurface position and viewport.
 */
extern Boolean XmWaylandGetWidgetSurface(
    Widget widget,
    struct wl_surface **toplevel_surface_return,
    int *x_return,
    int *y_return,
    int *width_return,
    int *height_return,
    double *scale_return);

/*
 * Mint a one-shot Wayland focus-activation token (xdg_activation_v1) seeded
 * with libXm's latest seat input serial, optional `source_window` surface
 * (pass None to fall back to the currently focused or hovered Motif shell),
 * and optional `app_id` hint for the target application.
 *
 * Must be called without holding XmWaylandLock(display). On success, returns
 * True and stores a newly XtMalloc()-allocated token string in `*token_return`
 * (caller frees with XtFree()). Returns False without modifying `*token_return`
 * when the native Wayland backend is inactive, xdg_activation_v1 is not
 * advertised by the compositor, or token creation times out.
 */
extern Boolean XmWaylandRequestActivationToken(
    Display *display,
    Window source_window,
    const char *app_id,
    char **token_return);

/*
 * Retrieve libXm's bound zwp_linux_dmabuf_v1 global and negotiated protocol
 * version (2..4). Either output pointer may be NULL. Returns False when the
 * native Wayland backend is inactive or zwp_linux_dmabuf_v1 is unavailable.
 */
extern Boolean XmWaylandGetDmabuf(
    Display *display,
    struct zwp_linux_dmabuf_v1 **dmabuf_return,
    unsigned int *version_return);

/*
 * Check whether the compositor advertised support for `(drm_format, modifier)`
 * via zwp_linux_dmabuf_v1 (either v3 modifier events or v4 default_feedback
 * tranche tables), or implicit modifier fallback (DRM_FORMAT_MOD_INVALID).
 */
extern Boolean XmWaylandHasDmabufFormat(
    Display *display,
    uint32_t drm_format,
    uint64_t modifier);

/*
 * Import 1..4 DMA-BUF planes into a zero-copy Wayland `wl_buffer *` via
 * zwp_linux_buffer_params_v1_create_immed without CPU readback.
 *
 * Caller MUST hold XmWaylandLock(display) across this call and subsequent
 * wl_surface_attach / wl_surface_commit / wl_display_flush calls. Plane file
 * descriptors remain owned by the caller (libwayland duplicates them onto the
 * wire before returning).
 */
extern Boolean XmWaylandImportDmabuf(
    Display *display,
    const XmWaylandDmabufPlane *planes,
    int plane_count,
    int width,
    int height,
    uint32_t drm_format,
    uint32_t flags,
    struct wl_buffer **buffer_return);

/*
 * Configure wp_tearing_control_v1 presentation hint (async_tearing=True ->
 * ASYNC, False -> VSYNC) on `surface` (either a Motif top-level wl_surface or
 * an embedded wl_subsurface). Caller MUST hold XmWaylandLock(display).
 */
extern Boolean XmWaylandSetSurfaceTearingHint(
    Display *display,
    struct wl_surface *surface,
    Boolean async_tearing);

/*
 * Configure wp_fifo_v1 presentation barrier constraints (`set_barrier` and/or
 * `wait_barrier`) on `surface` prior to wl_surface_commit. Caller MUST hold
 * XmWaylandLock(display).
 */
extern Boolean XmWaylandSetSurfaceFifo(
    Display *display,
    struct wl_surface *surface,
    Boolean set_barrier,
    Boolean wait_barrier);

/*
 * Configure wp_color_management_surface_v1 parametric HDR10 / wide-gamut
 * colorimetry (`BT.2020 PQ`, `BT.2020 HLG`, `Display-P3`, or `sRGB`) on
 * `surface` (pass `metadata = NULL` to unset back to compositor default SDR).
 * Identical metadata across consecutive frames is deduplicated in-place.
 * Caller MUST hold XmWaylandLock(display).
 */
extern Boolean XmWaylandSetSurfaceHdrMetadata(
    Display *display,
    struct wl_surface *surface,
    const XmWaylandHdrMetadata *metadata);

/*
 * Destroy any cached wp_tearing_control_v1 / wp_fifo_v1 / wp_color_management_surface_v1
 * auxiliary protocol objects associated with `surface` before the caller destroys
 * `surface`. Caller MUST hold XmWaylandLock(display).
 */
extern void XmWaylandReleaseSurfaceExtensions(
    Display *display,
    struct wl_surface *surface);

#ifdef __cplusplus
}
#endif

#endif /* _XmWayland_h */
