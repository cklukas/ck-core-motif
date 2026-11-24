/*
 * Native Wayland runtime backend internal declarations for libXm.
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _XmWaylandBackendI_h
#define _XmWaylandBackendI_h

#include <X11/Intrinsic.h>

#ifdef HAVE_WAYLAND_BACKEND

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <fontconfig/fontconfig.h>
#include <pixman.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>

#include "wayland-xdg-shell-client-protocol.h"
#include "wayland-xdg-decoration-client-protocol.h"
#include "wayland-xdg-toplevel-icon-client-protocol.h"
#include "wayland-xdg-activation-client-protocol.h"
#include "wayland-tablet-v2-client-protocol.h"
#include "wayland-cursor-shape-client-protocol.h"
#include "wayland-primary-selection-client-protocol.h"
#include "wayland-text-input-v3-client-protocol.h"
#include "wayland-fractional-scale-client-protocol.h"
#include "wayland-viewporter-client-protocol.h"

#if defined(__GNUC__) || defined(__clang__)
#define XM_WL_INTERNAL __attribute__((visibility("hidden")))
#else
#define XM_WL_INTERNAL
#endif

#define XM_WL_MAX_CLIENTS        16
#define XM_WL_MAX_OUTPUTS        8
#define XM_WL_MAX_ATOMS          1024
#define XM_WL_MAX_POPUPS         16
#define XM_WL_SHM_SLOTS          4
#define XM_WL_MAX_TOUCH_POINTS   8

#define XM_WL_ROOT_XID           UINT32_C(0x00000040)
#define XM_WL_WM_HELPER_XID      UINT32_C(0x00000041)
#define XM_WL_PROXY_SEL_XID      UINT32_C(0x00000042)
#define XM_WL_SM_HELPER_XID      UINT32_C(0x00000043)
#define XM_WL_DEFAULT_CMAP_XID   UINT32_C(0x00000020)
#define XM_WL_VISUAL_RGB24_ID    UINT32_C(0x00000021)
#define XM_WL_VISUAL_ARGB32_ID   UINT32_C(0x00000022)
#define XM_WL_RANDR_OUTPUT_XID   UINT32_C(0x00000050)
#define XM_WL_RANDR_CRTC_XID     UINT32_C(0x00000051)
#define XM_WL_RANDR_MODE_XID     UINT32_C(0x00000052)

/* Extension major opcodes & bases */
#define XM_WL_EXT_BIGREQ_OP      130
#define XM_WL_EXT_XCMISC_OP      131
#define XM_WL_EXT_RENDER_OP      132
#define XM_WL_EXT_SHAPE_OP       133
#define XM_WL_EXT_XFIXES_OP      134
#define XM_WL_EXT_RANDR_OP       135
#define XM_WL_EXT_XINERAMA_OP    136

#define XM_WL_SHAPE_EVENT_BASE   64
#define XM_WL_XFIXES_EVENT_BASE  66
#define XM_WL_RANDR_EVENT_BASE   68

#define XM_WL_RENDER_ERROR_BASE  140
#define XM_WL_XFIXES_ERROR_BASE  150
#define XM_WL_RANDR_ERROR_BASE   155

/* Render PictFormat IDs */
#define XM_WL_PICTFMT_ARGB32     UINT32_C(0x00000060)
#define XM_WL_PICTFMT_RGB24      UINT32_C(0x00000061)
#define XM_WL_PICTFMT_A8         UINT32_C(0x00000062)
#define XM_WL_PICTFMT_A4         UINT32_C(0x00000063)
#define XM_WL_PICTFMT_A1         UINT32_C(0x00000064)

typedef struct XmWlServer XmWlServer;
typedef struct XmWlClient XmWlClient;
typedef struct XmWlWindow XmWlWindow;
typedef struct XmWlPixmap XmWlPixmap;
typedef struct XmWlGC XmWlGC;
typedef struct XmWlFont XmWlFont;
typedef struct XmWlPicture XmWlPicture;
typedef struct XmWlGlyphSet XmWlGlyphSet;
typedef struct XmWlRegion XmWlRegion;
typedef struct XmWlCursor XmWlCursor;
typedef struct XmWlProperty XmWlProperty;
typedef struct XmWlShmBuf XmWlShmBuf;
typedef struct XmWlPendingTokenCtx XmWlPendingTokenCtx;

struct XmWlProperty {
    uint32_t atom;
    uint32_t type;
    uint8_t format; /* 8, 16, or 32 */
    uint8_t *data;
    size_t byte_len;
    XmWlProperty *next;
};

struct XmWlShmBuf {
    struct wl_shm_pool *pool;
    struct wl_buffer *wl_buf;
    uint32_t *pixels;
    pixman_image_t *pix_img;
    size_t pool_cap;
    size_t byte_size;
    int width;
    int height;
    int stride;
    int fd;
    bool has_alpha;
    bool busy;
    int valid_w;
    int valid_h;
    uint64_t valid_gen;
    uint64_t attached_ms;
};

typedef struct {
    int16_t x, y;
    uint16_t width, height;
} XmWlRect;

struct XmWlWindow {
    uint32_t id;
    XmWlWindow *parent;
    XmWlWindow *first_child;
    XmWlWindow *last_child;
    XmWlWindow *prev_sibling;
    XmWlWindow *next_sibling;
    XmWlWindow *hash_next;

    int16_t x, y;
    uint16_t width, height;
    uint16_t border_width;
    uint8_t depth;
    uint8_t win_class; /* 1=InputOutput, 2=InputOnly */
    uint32_t visual_id;
    bool override_redirect;
    bool mapped;
    uint8_t bit_gravity;
    uint8_t win_gravity;
    uint8_t backing_store;

    bool has_bg_pixel;
    uint32_t bg_pixel;
    uint32_t bg_pixmap;
    uint32_t border_pixel;
    uint32_t colormap_id;
    uint32_t cursor_id;
    uint32_t do_not_propagate;
    uint32_t event_masks[XM_WL_MAX_CLIENTS];
    uint32_t shape_select_mask;

    XmWlProperty *props;
    XmWlRect *shape_rects;
    size_t shape_rect_count;
    bool has_shape;

    /* Wayland surface & shell roles (only for direct children of root) */
    struct wl_surface *wl_surf;
    struct xdg_surface *xdg_surf;
    struct xdg_toplevel *xdg_top;
    struct zxdg_toplevel_decoration_v1 *xdg_deco;
    struct xdg_toplevel_icon_v1 *xdg_icon;
    struct xdg_popup *xdg_pop;
    struct wp_fractional_scale_v1 *frac_scale_obj;
    struct wp_viewport *viewport;
    struct wl_callback *frame_cb;

    bool is_popup;
    bool is_drag_icon;
    struct XmWlWindow *popup_parent;
    uint32_t popup_reposition_token;
    bool xdg_configured;
    uint32_t pending_configure_serial;
    int32_t pending_cfg_w;
    int32_t pending_cfg_h;
    bool has_queued_wl_cfg;
    uint32_t queued_wl_serial;
    int32_t queued_wl_w;
    int32_t queued_wl_h;
    bool resize_awaiting_client;
    uint64_t resize_sent_ms;
    uint32_t transient_for_xid;
    int32_t min_w, min_h, max_w, max_h;
    bool wants_decorations;
    char sent_title[256];
    char sent_app_id[128];
    char pending_activation_token[256];
    uint32_t sent_parent_xid;
    int32_t sent_min_w, sent_min_h, sent_max_w, sent_max_h;
    int8_t sent_deco_mode;
    double preferred_scale; /* e.g. 1.0, 1.25, 1.5, 2.0 */
    struct wl_output *current_wl_output;
    bool surf_listener_added;

    /* Backing canvas & wl_shm triple-buffered pool for top-level / popup windows */
    pixman_image_t *canvas;
    uint32_t *canvas_bits;
    int canvas_w;
    int canvas_h;
    int canvas_cap_w;
    int canvas_cap_h;
    int canvas_stride;
    XmWlShmBuf shm_bufs[XM_WL_SHM_SLOTS];
    bool opaque_region_set;
    int opaque_w;
    int opaque_h;
    uint32_t resize_sync_gen;
    uint64_t frame_gen;
    int committed_w;
    int committed_h;
    bool dmg_regions_inited;
    pixman_region32_t dirty_region;
    pixman_region32_t dmg_hist_reg[XM_WL_SHM_SLOTS];
    bool dirty;
    int32_t dmg_x1, dmg_y1, dmg_x2, dmg_y2;
};

struct XmWlPixmap {
    uint32_t id;
    uint16_t width;
    uint16_t height;
    uint8_t depth;
    pixman_image_t *img;
    uint32_t *bits;
    int stride;
    XmWlPixmap *hash_next;
};

struct XmWlGC {
    uint32_t id;
    uint8_t depth;
    uint8_t function;       /* GXcopy=3, GXxor=6, GXinvert=10, etc. */
    uint32_t plane_mask;
    uint32_t fg;
    uint32_t bg;
    uint16_t line_width;
    uint8_t line_style;     /* LineSolid=0, LineOnOffDash=1, LineDoubleDash=2 */
    uint8_t cap_style;
    uint8_t join_style;
    uint8_t fill_style;     /* FillSolid=0, FillTiled=1, FillStippled=2, FillOpaqueStippled=3 */
    uint8_t fill_rule;
    uint8_t arc_mode;
    uint32_t tile_pixmap;
    uint32_t stipple_pixmap;
    int16_t ts_x_origin;
    int16_t ts_y_origin;
    uint32_t font_id;
    uint8_t subwindow_mode; /* ClipByChildren=0, IncludeInferiors=1 */
    bool graphics_exposures;
    int16_t clip_x_origin;
    int16_t clip_y_origin;
    uint32_t clip_mask_pixmap;
    XmWlRect *clip_rects;
    size_t clip_rect_count;
    bool has_clip_rects;
    uint8_t dash_offset;
    uint8_t dashes[16];
    uint8_t dash_count;
    XmWlGC *hash_next;
};

typedef struct {
    int16_t lbearing;
    int16_t rbearing;
    int16_t width;
    int16_t ascent;
    int16_t descent;
    uint16_t attributes;
} XmWlCharInfo;

struct XmWlFont {
    uint32_t id;
    char name[256];
    FT_Face face;
    int pixel_size;
    bool monospace;
    bool bold;
    int16_t ascent;
    int16_t descent;
    XmWlCharInfo min_bounds;
    XmWlCharInfo max_bounds;
    XmWlCharInfo chars[256];
    XmWlFont *hash_next;
};

struct XmWlPicture {
    uint32_t id;
    uint32_t drawable_id; /* 0 for solid/gradient source pictures */
    uint32_t format_id;
    uint8_t repeat;
    bool component_alpha;
    int16_t clip_x_origin;
    int16_t clip_y_origin;
    uint32_t clip_mask_pixmap;
    XmWlRect *clip_rects;
    size_t clip_rect_count;
    bool has_clip_rects;
    pixman_transform_t transform;
    bool has_transform;
    pixman_filter_t filter;
    pixman_image_t *source_img; /* for solid fill / gradients */
    XmWlPicture *hash_next;
};

typedef struct XmWlGlyph {
    uint32_t glyph_id;
    int16_t x, y;         /* hotspot origin inside glyph image */
    int16_t width, height;
    int16_t x_off, y_off; /* advance */
    pixman_image_t *img;
    uint32_t *bits;
    struct XmWlGlyph *next;
} XmWlGlyph;

struct XmWlGlyphSet {
    uint32_t id;
    uint32_t format_id;
    int ref_count;
    XmWlGlyph *buckets[64];
    XmWlGlyphSet *hash_next;
};

struct XmWlRegion {
    uint32_t id;
    pixman_region32_t reg;
    XmWlRegion *hash_next;
};

struct XmWlCursor {
    uint32_t id;
    bool is_glyph;
    uint16_t glyph_char;
    uint16_t width, height;
    int16_t hot_x, hot_y;
    uint32_t *argb_pixels;
    XmWlShmBuf shm_buf;
    XmWlCursor *hash_next;
};

typedef struct {
    bool active;
    int32_t id;
    double x, y;
    struct wl_surface *surface;
} XmWlTouchPoint;

typedef struct {
    uint32_t window;
    uint16_t button; /* 0 = AnyButton */
    uint16_t modifiers; /* 0x8000 = AnyModifier */
    uint32_t event_mask;
    bool owner_events;
    int client_idx;
} XmWlPassiveButtonGrab;

struct XmWlClient {
    bool active;
    bool handshake_done;
    int fd;
    int index;
    uint32_t resource_base;
    uint16_t seq;
    uint8_t *rx_buf;
    size_t rx_len;
    size_t rx_cap;
    uint8_t *tx_buf;
    size_t tx_len;
    size_t tx_cap;
    bool tx_pkt_aligned;
    uint32_t xfixes_sel_atoms[8];
    uint32_t xfixes_sel_masks[8];
    uint32_t randr_mask;
};

typedef struct {
    struct wl_output *wl_out;
    uint32_t global_name;
    char name[64];
    int32_t x, y;
    int32_t width, height;
    int32_t phys_w_mm, phys_h_mm;
    int32_t scale;
    bool done;
} XmWlOutputInfo;

typedef struct {
    char name[64];
    bool enabled;
    int32_t x, y;
    int32_t width, height; /* Rotated physical layout dimensions */
    int32_t dpi;
} XmWlIpcOutput;

typedef struct {
    uint32_t selection_atom;
    uint32_t requestor;
    uint32_t target_atom;
    uint32_t property_atom;
    uint32_t time;
    int client_idx;
    int write_fd; /* Wayland send(mime, fd) waiting on X11 SelectionNotify */
    bool active;
} XmWlPendingSelectionTransfer;

struct XmWlServer {
    pthread_mutex_t lock;
    pthread_t thread;
    bool running;
    int listen_fd;
    int wake_pipe[2];
    int ipc_fd; /* optional $XDG_RUNTIME_DIR/dtwm-wayland-<WAYLAND_DISPLAY>.ipc */
    bool suppress_ipc_publish;
    bool ipc_initial_ready;
    char *ipc_rx_buf;
    size_t ipc_rx_len;
    size_t ipc_rx_cap;
    char runtime_dir[512];
    char socket_path[544];
    char display_spec[560];

    /* Main-thread XtAppAddInput wakeup for IME, HiDPI scale reflow & resize sync */
    int xt_pipe[2];
    XtInputId xt_input_id;
    Widget attached_xm_display;
    double pending_xt_scale;
    bool has_pending_xt_scale;
    uint32_t resize_sync_seq;
    uint32_t pending_xt_resize_gen;
    bool has_pending_xt_resize;
    XtWorkProcId resize_sync_wp_id;

    /* Wayland objects */
    struct wl_display *wl_dpy;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_subcompositor *subcompositor;
    struct wl_shm *shm;
    struct xdg_wm_base *xdg_wm_base;
    struct zxdg_decoration_manager_v1 *deco_mgr;
    struct xdg_toplevel_icon_manager_v1 *icon_mgr;
    struct xdg_activation_v1 *activation_mgr;
    struct wl_seat *seat;
    struct wl_pointer *pointer;
    struct wl_keyboard *keyboard;
    struct wl_touch *touch;
    struct wp_cursor_shape_manager_v1 *cursor_shape_mgr;
    struct wp_cursor_shape_device_v1 *cursor_shape_dev;
    struct wl_cursor_theme *cursor_theme;
    struct wl_surface *cursor_surface;
    struct wl_data_device_manager *data_mgr;
    struct wl_data_device *data_dev;
    struct zwp_primary_selection_device_manager_v1 *prim_mgr;
    struct zwp_primary_selection_device_v1 *prim_dev;
    struct zwp_text_input_manager_v3 *text_input_mgr;
    struct zwp_text_input_v3 *text_input;
    struct wp_fractional_scale_manager_v1 *frac_scale_mgr;
    struct wp_viewporter *viewporter;

    XmWlOutputInfo outputs[XM_WL_MAX_OUTPUTS];
    int output_count;
    XmWlIpcOutput ipc_outputs[XM_WL_MAX_OUTPUTS];
    int ipc_output_count;
    bool compositor_native_dpi;
    int primary_dpi;
    int cursor_loaded_size;
    int screen_w;
    int screen_h;
    int screen_mm_w;
    int screen_mm_h;
    double active_scale;
    double logical_zoom;

    /* X11 clients & resources */
    XmWlClient clients[XM_WL_MAX_CLIENTS];
    char *atoms[XM_WL_MAX_ATOMS];
    uint32_t atom_count;

    XmWlWindow *win_buckets[256];
    XmWlPixmap *pix_buckets[256];
    XmWlGC *gc_buckets[256];
    XmWlFont *font_buckets[64];
    XmWlPicture *pic_buckets[256];
    XmWlGlyphSet *gs_buckets[64];
    XmWlRegion *reg_buckets[64];
    XmWlCursor *cursor_buckets[128];
    XmWlWindow *root;
    XmWlWindow *wm_helper;
    XmWlWindow *sm_helper;
    XmWlWindow *proxy_sel;
    uint32_t local_sm_win_xid;

    /* Active popup LIFO stack */
    XmWlWindow *popup_stack[XM_WL_MAX_POPUPS];
    int popup_depth;

    /* Input & Focus state */
    struct xkb_context *xkb_ctx;
    struct xkb_keymap *xkb_keymap;
    struct xkb_state *xkb_state;
    uint32_t keysym_table[256][4];
    uint8_t mod_map[8][4];
    int32_t repeat_rate;
    int32_t repeat_delay;
    uint32_t held_keycode;
    uint64_t next_repeat_ms;

    uint32_t last_input_serial;
    uint32_t last_ptr_enter_serial;
    uint32_t last_applied_cursor_id;
    uint32_t last_applied_cursor_shape;
    uint32_t last_activation_serial;
    uint32_t last_button_serial;
    uint32_t last_button_time_ms;
    uint32_t server_time_ms;
    XmWlWindow *pointer_top;
    XmWlWindow *hovered_win;
    int16_t ptr_surf_x, ptr_surf_y;
    int16_t ptr_root_x, ptr_root_y;
    uint16_t button_mask;
    uint16_t mod_mask;

    /* Touchscreen (wl_touch) state */
    XmWlTouchPoint touch_pts[XM_WL_MAX_TOUCH_POINTS];
    int active_touch_count;
    bool touch_btn1_down;
    bool touch_two_finger_pan;
    double touch_pan_last_cx, touch_pan_last_cy;

    /* High-resolution & discrete Wayland pointer axis state (0=vert, 1=horiz) */
    double wheel_frame_px[2];
    int32_t wheel_frame_v120[2];
    bool wheel_frame_has_px[2];
    bool wheel_frame_has_v120[2];
    uint32_t wheel_frame_time_ms;
    int32_t wheel_accum_v120[2];
    bool wheel_gesture_active[2];
    int wheel_last_dir[2];
    uint64_t wheel_last_ms[2];

    /* Main-thread ~83 Hz Wayland smooth-scrolling queue & interpolator */
    int wheel_speed_lines;
    bool wheel_smooth_enabled;
    uint32_t smooth_xid_chain[8];
    double smooth_pending_v120[2];
    bool has_smooth_pending;
    bool smooth_cancel_requested;
    XtIntervalId smooth_timer_id;
    uint32_t smooth_active_chain[8];
    double smooth_rem_v120[2];
    double smooth_unit_carry[2];
    uint64_t smooth_last_step_ms;

    XmWlWindow *wl_focus_top;
    uint32_t x11_focus_win;
    uint8_t x11_focus_revert;

    /* Pointer & keyboard grabs */
    bool ptr_grab_active;
    bool ptr_grab_implicit;
    uint32_t ptr_grab_win;
    uint32_t ptr_grab_cursor_id;
    uint32_t ptr_grab_mask;
    bool ptr_grab_owner_events;
    int ptr_grab_client;
    XmWlPassiveButtonGrab passive_grabs[64];
    size_t passive_grab_count;

    /* FreeType library */
    FT_Library ft_lib;
    bool ft_inited;

    /* Selections & DnD */
    uint32_t sel_clipboard_owner;
    uint32_t sel_primary_owner;
    uint32_t sel_xdnd_owner;
    struct wl_data_source *wl_clip_source;
    struct wl_data_offer *wl_clip_offer;
    char wl_clip_mimes[32][96];
    int wl_clip_mime_count;
    struct wl_data_offer *wl_pending_offer;
    char wl_pending_mimes[32][96];
    int wl_pending_mime_count;

    struct zwp_primary_selection_source_v1 *wl_prim_source;
    struct zwp_primary_selection_offer_v1 *wl_prim_offer;
    char wl_prim_mimes[32][96];
    int wl_prim_mime_count;
    struct zwp_primary_selection_offer_v1 *wl_pending_prim_offer;
    char wl_pending_prim_mimes[32][96];
    int wl_pending_prim_mime_count;

    /* Wayland DnD state */
    struct wl_data_source *wl_drag_source;
    struct wl_surface *wl_drag_icon_surf;
    XmWlWindow *wl_drag_icon_owner;
    int16_t wl_drag_icon_dx, wl_drag_icon_dy;
    struct wl_data_offer *wl_dnd_offer;
    char wl_dnd_mimes[32][96];
    int wl_dnd_mime_count;
    XmWlWindow *wl_dnd_target_top;
    int16_t wl_dnd_x, wl_dnd_y;
    uint32_t wl_dnd_serial;
    bool wl_dnd_drop_pending;

    XmWlPendingSelectionTransfer pending_transfers[8];

    /* zwp_text_input_v3 state */
    bool ime_enabled;
    char ime_pending_preedit[512];
    int32_t ime_pending_cursor_begin;
    int32_t ime_pending_cursor_end;
    bool ime_has_pending_preedit;
    char ime_pending_commit[512];
    bool ime_has_pending_commit;
    uint32_t ime_pending_del_before;
    uint32_t ime_pending_del_after;
    bool ime_has_pending_delete;
    int32_t ime_spot_x, ime_spot_y, ime_spot_w, ime_spot_h;

    /* Queued IME mutations drained on the main Xt thread */
    Widget ime_focused_widget;
    int ime_active_preedit_chars;
    long ime_preedit_pos;
    char xt_ime_commit[512];
    bool xt_has_ime_commit;
    char xt_ime_preedit[512];
    bool xt_has_ime_preedit;
    uint32_t xt_ime_del_before;
    uint32_t xt_ime_del_after;
    bool xt_has_ime_delete;

    /* xdg_activation_v1 startup & token minting state */
    char startup_activation_token[256];
    XmWlPendingTokenCtx *pending_act_tokens;
    pthread_mutex_t act_lock;
    pthread_cond_t act_cond;
    uint64_t act_req_seq;
    bool act_req_in_flight;
    bool act_req_submitted;
    bool act_req_done;
    uint32_t act_req_source_xid;
    char act_req_app_id[128];
    char act_req_result[256];
};

/* Global server singleton accessor */
XM_WL_INTERNAL XmWlServer *_XmWaylandGetServer(void);
XM_WL_INTERNAL Boolean _XmWaylandEnsureDisplay(void);
XM_WL_INTERNAL void _XmWaylandAttachDisplay(Widget xm_display);
XM_WL_INTERNAL void _XmWaylandDetachDisplay(Widget xm_display);
XM_WL_INTERNAL void _XmWaylandNotifyMainThread(XmWlServer *srv);

/* WaylandBackend.c shell & surface lifecycle */
XM_WL_INTERNAL uint32_t _XmWlNowMs(void);
XM_WL_INTERNAL void _XmWlWakeServer(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlSyncWindowRole(XmWlServer *srv, XmWlWindow *win);
XM_WL_INTERNAL void _XmWlRepositionPopup(XmWlServer *srv, XmWlWindow *win);
XM_WL_INTERNAL void _XmWlUnmapWindowRole(XmWlServer *srv, XmWlWindow *win);
XM_WL_INTERNAL void _XmWlDestroyWindowRole(XmWlServer *srv, XmWlWindow *win);
XM_WL_INTERNAL void _XmWlUpdateWindowMetadata(XmWlServer *srv, XmWlWindow *win, uint32_t changed_atom);
XM_WL_INTERNAL void _XmWlFlushDirtySurfaces(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlUpdatePointerCursor(XmWlServer *srv, bool force);
XM_WL_INTERNAL XmWlCursor *_XmWlFindCursor(XmWlServer *srv, uint32_t id);
XM_WL_INTERNAL void _XmWlRegisterGlyphCursor(XmWlServer *srv, uint32_t id, uint16_t glyph_char);
XM_WL_INTERNAL void _XmWlRegisterBitmapCursor(XmWlServer *srv, uint32_t id, uint16_t w, uint16_t h,
                                              int16_t hot_x, int16_t hot_y, const uint32_t *argb);
XM_WL_INTERNAL void _XmWlFreeCursor(XmWlServer *srv, uint32_t id);
XM_WL_INTERNAL void _XmWlEnsureTopCanvas(XmWlServer *srv, XmWlWindow *top);
XM_WL_INTERNAL void _XmWlDamageTop(XmWlWindow *top, int x, int y, int w, int h);
XM_WL_INTERNAL void _XmWlRebuildKeymapFromXkb(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlIpcPublishRootProp(XmWlServer *srv, const char *atom_name, const uint8_t *data, size_t len);
XM_WL_INTERNAL void _XmWlIpcHandleReadable(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlApplyScaleUpdate(XmWlServer *srv, double new_scale);
XM_WL_INTERNAL void _XmWlRequestSelfActivate(XmWlServer *srv, XmWlWindow *top);

/* WaylandX11Server.c resource & wire helpers */
XM_WL_INTERNAL void _XmWlInitAtomsAndRoot(XmWlServer *srv);
XM_WL_INTERNAL uint32_t _XmWlInternAtom(XmWlServer *srv, const char *name, size_t len, bool only_if_exists);
XM_WL_INTERNAL const char *_XmWlAtomName(XmWlServer *srv, uint32_t atom);
XM_WL_INTERNAL XmWlWindow *_XmWlFindWindow(XmWlServer *srv, uint32_t xid);
XM_WL_INTERNAL XmWlPixmap *_XmWlFindPixmap(XmWlServer *srv, uint32_t xid);
XM_WL_INTERNAL XmWlGC *_XmWlFindGC(XmWlServer *srv, uint32_t xid);
XM_WL_INTERNAL XmWlFont *_XmWlFindFont(XmWlServer *srv, uint32_t xid);
XM_WL_INTERNAL XmWlPicture *_XmWlFindPicture(XmWlServer *srv, uint32_t xid);
XM_WL_INTERNAL XmWlGlyphSet *_XmWlFindGlyphSet(XmWlServer *srv, uint32_t xid);
XM_WL_INTERNAL XmWlRegion *_XmWlFindRegion(XmWlServer *srv, uint32_t xid);
XM_WL_INTERNAL XmWlWindow *_XmWlEnclosingTopLevel(XmWlServer *srv, XmWlWindow *win, int *out_dx, int *out_dy);
XM_WL_INTERNAL XmWlProperty *_XmWlGetProperty(XmWlWindow *win, uint32_t atom);
XM_WL_INTERNAL void _XmWlSetProperty(XmWlServer *srv, XmWlWindow *win, uint32_t atom, uint32_t type,
                                     uint8_t format, uint8_t mode, const uint8_t *data, size_t byte_len,
                                     bool emit_notify);
XM_WL_INTERNAL void _XmWlDeleteProperty(XmWlServer *srv, XmWlWindow *win, uint32_t atom, bool emit_notify);
XM_WL_INTERNAL void _XmWlSendEventToWindow(XmWlServer *srv, XmWlWindow *win, uint32_t mask, const uint8_t ev[32]);
XM_WL_INTERNAL void _XmWlSendEventToClient(XmWlClient *cli, const uint8_t ev[32]);
XM_WL_INTERNAL void _XmWlSendReply(XmWlClient *cli, const uint8_t hdr[32], const void *extra, size_t extra_len);
XM_WL_INTERNAL void _XmWlSendError(XmWlClient *cli, uint8_t err_code, uint32_t bad_val, uint16_t minor_op, uint8_t major_op);
XM_WL_INTERNAL void _XmWlHandleClientReadable(XmWlServer *srv, int client_idx);
XM_WL_INTERNAL void _XmWlFlushClientWrites(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlDisconnectClient(XmWlServer *srv, int client_idx);
XM_WL_INTERNAL void _XmWlDispatchPointerMotion(XmWlServer *srv, XmWlWindow *top, int16_t sx, int16_t sy, uint32_t time_ms);
XM_WL_INTERNAL void _XmWlDispatchPointerButton(XmWlServer *srv, uint8_t button, bool pressed, uint32_t time_ms);
XM_WL_INTERNAL size_t _XmWlHitTestWindowChain(XmWlServer *srv, uint32_t out_xids[8]);
XM_WL_INTERNAL void _XmWlCancelActiveDragGrab(XmWlServer *srv, uint32_t time_ms);
XM_WL_INTERNAL void _XmWlDispatchPointerLeaveTop(XmWlServer *srv, XmWlWindow *top, uint32_t time_ms);
XM_WL_INTERNAL void _XmWlDispatchKey(XmWlServer *srv, uint8_t x11_keycode, bool pressed, uint32_t time_ms);
XM_WL_INTERNAL void _XmWlDispatchFocusTop(XmWlServer *srv, XmWlWindow *top, bool focused);
XM_WL_INTERNAL void _XmWlDeliverConfigureAndExpose(XmWlServer *srv, XmWlWindow *top, int width, int height, int old_w, int old_h);
XM_WL_INTERNAL void _XmWlDeliverCloseRequest(XmWlServer *srv, XmWlWindow *top);
XM_WL_INTERNAL void _XmWlBroadcastRandRChange(XmWlServer *srv);

/* WaylandRender.c rasterizer, fonts & XRender */
XM_WL_INTERNAL void _XmWlRenderInit(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlRenderDestroy(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlPaintWindowBackground(XmWlServer *srv, XmWlWindow *win, int x, int y, int w, int h);
XM_WL_INTERNAL void _XmWlFreePixmapData(XmWlPixmap *pix);
XM_WL_INTERNAL void _XmWlFreeFontData(XmWlFont *font);
XM_WL_INTERNAL void _XmWlFreePictureData(XmWlPicture *pic);
XM_WL_INTERNAL void _XmWlFreeGlyphSetData(XmWlGlyphSet *gs);
XM_WL_INTERNAL XmWlFont *_XmWlOpenFont(XmWlServer *srv, uint32_t id, const char *name, size_t len);
XM_WL_INTERNAL void _XmWlRenderPolyFillRect(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, const uint8_t *rects, size_t count);
XM_WL_INTERNAL void _XmWlRenderPolyRect(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, const uint8_t *rects, size_t count);
XM_WL_INTERNAL void _XmWlRenderPolySegment(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, const uint8_t *segs, size_t count);
XM_WL_INTERNAL void _XmWlRenderPolyLine(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, uint8_t coord_mode, const uint8_t *pts, size_t count);
XM_WL_INTERNAL void _XmWlRenderPolyPoint(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, uint8_t coord_mode, const uint8_t *pts, size_t count);
XM_WL_INTERNAL void _XmWlRenderFillPoly(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, uint8_t coord_mode, const uint8_t *pts, size_t count);
XM_WL_INTERNAL void _XmWlRenderPolyArc(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, const uint8_t *arcs, size_t count, bool fill);
XM_WL_INTERNAL void _XmWlRenderPutImage(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, uint8_t format,
                                        uint16_t width, uint16_t height, int16_t dst_x, int16_t dst_y,
                                        uint8_t left_pad, uint8_t depth, const uint8_t *data, size_t data_len);
XM_WL_INTERNAL bool _XmWlRenderGetImage(XmWlServer *srv, uint32_t drawable, int16_t x, int16_t y,
                                        uint16_t width, uint16_t height, uint32_t plane_mask, uint8_t format,
                                        uint8_t **out_bytes, size_t *out_len, uint8_t *out_depth, uint32_t *out_visual);
XM_WL_INTERNAL void _XmWlRenderCopyArea(XmWlServer *srv, XmWlClient *cli, uint32_t src_d, uint32_t dst_d, XmWlGC *gc,
                                        int16_t sx, int16_t sy, uint16_t w, uint16_t h, int16_t dx, int16_t dy);
XM_WL_INTERNAL void _XmWlRenderCopyPlane(XmWlServer *srv, XmWlClient *cli, uint32_t src_d, uint32_t dst_d, XmWlGC *gc,
                                         int16_t sx, int16_t sy, uint16_t w, uint16_t h, int16_t dx, int16_t dy, uint32_t bit_plane);
XM_WL_INTERNAL void _XmWlRenderPolyText(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, int16_t x, int16_t y,
                                        const uint8_t *items, size_t len, bool is_16bit);
XM_WL_INTERNAL void _XmWlRenderImageText(XmWlServer *srv, uint32_t drawable, XmWlGC *gc, int16_t x, int16_t y,
                                         const uint8_t *str, size_t count, bool is_16bit);
XM_WL_INTERNAL void _XmWlHandleRenderRequest(XmWlServer *srv, XmWlClient *cli, uint8_t minor_op, const uint8_t *req, size_t req_len);
XM_WL_INTERNAL bool _XmWlBuildIconPixels(XmWlServer *srv, uint32_t icon_pixmap, uint32_t icon_mask,
                                         uint32_t **out_argb, int *out_w, int *out_h);

/* WaylandData.c selections & DnD */
XM_WL_INTERNAL void _XmWlDataBindSeat(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlDataHandleSetSelectionOwner(XmWlServer *srv, uint32_t selection, uint32_t owner, uint32_t time);
XM_WL_INTERNAL uint32_t _XmWlDataGetSelectionOwner(XmWlServer *srv, uint32_t selection);
XM_WL_INTERNAL void _XmWlDataHandleConvertSelection(XmWlServer *srv, int client_idx, uint32_t requestor,
                                                    uint32_t selection, uint32_t target, uint32_t property, uint32_t time);
XM_WL_INTERNAL bool _XmWlDataInterceptSendEvent(XmWlServer *srv, uint32_t dest_win, const uint8_t ev[32]);
XM_WL_INTERNAL void _XmWlDataAttachDragIcon(XmWlServer *srv, XmWlWindow *drag_win);

/* WaylandIme.c zwp_text_input_v3 */
XM_WL_INTERNAL void _XmWlImeBindSeat(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlImeSyncFocus(XmWlServer *srv);
XM_WL_INTERNAL void _XmWlImeDrainMainThread(XmWlServer *srv);
XM_WL_INTERNAL void _XmWaylandImSetFocus(Widget w, ArgList args, Cardinal num_args);
XM_WL_INTERNAL void _XmWaylandImUnsetFocus(Widget w);
XM_WL_INTERNAL void _XmWaylandImUnregister(Widget w);

#endif /* HAVE_WAYLAND_BACKEND */
#endif /* _XmWaylandBackendI_h */
