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

#ifdef __cplusplus
extern "C" {
#endif

struct wl_display;
struct wl_compositor;
struct wl_subcompositor;
struct wl_shm;
struct wl_surface;
struct wp_viewporter;

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

#ifdef __cplusplus
}
#endif

#endif /* _XmWayland_h */
