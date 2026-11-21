# Native Wayland runtime backend

When built with `--enable-wayland`, `libXm` includes an optional XWayland-free
Wayland runtime backend that allows unmodified `Xlib`/`Xt`/Motif applications to
connect directly to a Wayland compositor (`WAYLAND_DISPLAY`) without running an
`Xwayland` or `Xorg` server.

## Build configuration

Configure discovers the Wayland client stack automatically (`--enable-wayland=auto`,
the default on platforms providing the Wayland development packages; pass
`--enable-wayland=yes` to require it or `--enable-wayland=no` to disable it):

- `wayland-scanner` (build-time protocol code generator)
- `wayland-client >= 1.20` and `wayland-cursor`
- `wayland-protocols >= 1.31` (`xdg-shell`, `xdg-decoration-unstable-v1`,
  `xdg-toplevel-icon-v1`, `primary-selection-unstable-v1`,
  `text-input-unstable-v3`, `fractional-scale-v1`, `viewporter`)
- `pixman-1`, `xkbcommon`, `freetype2`, and `fontconfig`

On X11-only hosts (such as macOS with XQuartz), `--enable-wayland=auto` disables
the Wayland backend cleanly without changing any public headers or the
`libXm.so.9` / `libXm.9.dylib` symbol ABI.

## Runtime display selection

At library load time, `libXm` selects its transport as follows:

1. `CK_MOTIF_BACKEND=x11` (or `0` / `off`): always use standard X11 / XWayland
   (`DISPLAY`), disabling the Wayland backend.
2. `CK_MOTIF_BACKEND=wayland` (or `1`): connect directly to `WAYLAND_DISPLAY`
   even if `DISPLAY` is also set in the environment.
3. Default (`CK_MOTIF_BACKEND` unset or `auto`): if `WAYLAND_DISPLAY` is set
   and `DISPLAY` is unset or empty, `libXm` activates the native Wayland
   backend automatically. If `DISPLAY` points to a real X11 or XWayland server,
   standard X11 transport remains active.

When active, each Motif process creates a private `0700` directory beneath
`$XDG_RUNTIME_DIR` (`ck-wlx11-<pid>-XXXXXX/S:0`) and serves an in-process X11r11
virtualizer backed directly by `wayland-client`. If a child Motif process
inherits a parent's `ck-wlx11-*` `DISPLAY` string, the child automatically
initializes its own independent Wayland connection and private runtime socket,
and exit cleanup only unlinks the socket owned by the creating PID.

`libXm` sets `CK_MOTIF_WAYLAND_ACTIVE=1` and `CK_MOTIF_WAYLAND_DISPLAY` in the
process environment when the native Wayland backend is active.

## Window management, menus, and rendering

- **Top-level shells**: `ApplicationShell`, `TopLevelShell`, and
  `TransientShell` windows map to `xdg_surface` + `xdg_toplevel`, negotiating
  server-side decorations through `zxdg_decoration_manager_v1` (honoring
  `_MOTIF_WM_HINTS`), transient parent linkage, size bounds (`WM_NORMAL_HINTS`),
  UTF-8 titles (`_NET_WM_NAME` / `WM_NAME`), `app_id` (`WM_CLASS`), and dynamic
  ARGB window icons (`xdg_toplevel_icon_v1` built from `WM_HINTS` icon pixmaps
  and masks). Post-map `xdg_surface.configure` resizes coalesce to the latest
  pending size and hold `xdg_surface.ack_configure` until the client finishes
  processing `ConfigureNotify` and painting its resized widget tree, paired with
  `VendorShell` `ConfigureNotify` queue compression so interactive drags settle
  immediately without accumulating intermediate layouts or rubber-banding.
- **Menus and popups**: Override-redirect menu shells (`XmMenuShell` pulldowns,
  context menus, submenus, option menus, and tooltips—including callback-posted
  `XmMenuPosition` popups and `owner_events = True` passive button grabs) map to
  constrained `xdg_popup` surfaces with `xdg_positioner` flip/slide/resize rules,
  live `xdg_popup_reposition` updates when a shared `XmMenuShell` moves between
  menu-bar cascades or context-menu targets, monotonic press/release event
  timestamps for sub-millisecond clicks, and LIFO `xdg_popup_grab` off-surface
  dismissal.
- **Rasterizer and typography**: Subwindow clipping, X11-compliant
  `ChangeWindowAttributes` background updates (preserving armed menu/button
  glyphs), classic 3D Motif bevels,
  pixmap tiles/stipples, and `RENDER` 0.11 operations (`Composite`,
  `Trapezoids`, `Triangles`, `LinearGradient`, `RadialGradient`, and glyph sets
  used by `libXft`/HarfBuzz/FreeType and modern themes) composite into
  double-buffered `wl_shm` pools via `pixman-1` with frame-callback damage
  tracking. Legacy core X11 font requests (`OpenFont`, `QueryFont`,
  `ListFonts`, `ListFontsWithInfo`, `PolyText8/16`, `ImageText8/16`) expose
  scalable and multi-size XLFDs across standard Latin-1 and Unicode families and
  rasterize directly through Fontconfig and FreeType without server-side PCF/BDF
  font paths. `RANDR` 1.5 output/CRTC/property queries and compositor IPC helper
  windows (`wm_helper` and `sm_helper`) allow desktop control panels and session
  managers to operate without `Xwayland`.

## Clipboard, drag-and-drop, input methods, and scaling

- **Selections**: `CLIPBOARD` (`wl_data_device`) and `PRIMARY`
  (`zwp_primary_selection_v1`) synchronize bidirectionally between Motif
  widgets and external Wayland clients across `UTF8_STRING`, `STRING`, `TEXT`,
  `text/plain;charset=utf-8`, `text/plain`, `text/uri-list`, and `image/png`.
- **Drag-and-drop**: Outgoing `XmDragStart` operations initiate native
  `wl_data_device_start_drag` sessions with persistent cursor-attached
  `XmDragOverShell` drag-icon surfaces (supporting both 24/32-bit pixmap icons
  and 1-bit `XmDragIcon` cursor bitmaps with `PictStandardA1`/`PictStandardA8`
  masks, hotspot offsets, and empty pointer input regions so underlying drop
  targets remain directly hit-testable); incoming `wl_data_device` drags bridge
  through XDND v5 to registered `XmDropSite` widgets.
- **Input methods**: `XmIm` binds `zwp_text_input_v3` for `XmText` and
  `XmTextField` without requiring an XIM server, forwarding cursor spot
  rectangles and applying preedit spans, surrounding-text deletions, and UTF-8
  commits on the `Xt` main loop.
- **Pointer hover-wheel & smooth scrolling**: `wl_seat` binds up to v9
  (`axis_value120` high-resolution wheel and touchpad scroll axes), routing
  wheel events directly to the deepest widget under the pointer without moving
  `XmEXPLICIT` keyboard focus or firing `AnyButton` popup-menu passive grabs.
  When smooth scrolling is enabled (`*mouseWheelSmooth: True`, the default), an
  ~83 Hz main-thread interpolator drives sub-line / pixel steps across
  `XmScrolledWindow`, `XmScrollBar`, `XmList`, `XmText`, `XmTreeView`, and
  `XmDataGrid` with zero speed cap, instant direction reversal, and clean
  start/end clamping, scaling dynamically with `*mouseWheelSpeed` (`1..10`,
  default `3`) and live `_DT_WAYLAND_WHEEL` compositor IPC updates.
- **Fractional and HiDPI scaling**: `wp_fractional_scale_v1`, `wl_output.scale`,
  and `wp_viewporter` size physical `wl_shm` buffers and embedded
  `XmWaylandGetWidgetSurface` subsurface scale factors while preserving 96-DPI
  Wayland logical coordinates so `libXft` glyphs are never double-scaled inside
  logical widget boxes. Explicit toolkit logical zoom (`CK_MOTIF_WAYLAND_SCALE`)
  drives `XmSetDisplayScaleOverride` and `RANDR` screen-change notifications so
  realized widgets reflow live across zoom changes.

## Subsurface embedding API (`<Xm/Wayland.h>`)

Applications that embed external Wayland buffers or off-screen renderers (such
as media players or off-screen web views) inside a Motif widget can include
`<Xm/Wayland.h>`:

- `XmWaylandIsActive(Display *)` reports whether `display` is backed by `libXm`'s
  native Wayland runtime.
- `XmWaylandLock(Display *)` and `XmWaylandUnlock(Display *)` serialize direct
  `libwayland-client` requests on `libXm`'s `wl_display` connection.
- `XmWaylandGetDisplayHandles(...)` returns the active `wl_display`,
  `wl_compositor`, `wl_subcompositor`, `wl_shm`, and `wp_viewporter` globals.
- `XmWaylandGetWidgetSurface(widget, &top_surface, &x, &y, &w, &h, &scale)`
  resolves a realized widget's enclosing top-level `wl_surface` together with
  its logical bounds and scale factor so the caller can attach a `wl_subsurface`
  with an empty input region (`wl_surface_set_input_region`) while preserving
  Motif pointer, wheel, focus, popup-menu, and `zwp_text_input_v3` routing.
- Applications that still prefer X11/XWayland when a real X11 `DISPLAY` is
  present may define `const int ck_motif_prefer_x11 = 1;` so library-load
  auto-selection keeps X11 even when `CK_MOTIF_BACKEND=wayland` is set in the
  environment, while still activating the native Wayland backend in pure
  XWayland-free (`DISPLAY` unset) sessions.

On X11, XWayland, macOS, and `--disable-wayland` builds, these entry points
remain present in `libXm` and safely return `False`.

## Qualification

Run the automated Wayland backend suite against a built tree or staged prefix:

```sh
python3 tests/automated/run-wayland-backend.py \
  --prefix /path/to/motif-prefix \
  --out /path/to/scratch/wayland-backend-results
```

The runner starts an isolated headless Wayland compositor with `DISPLAY` unset,
verifies that no `Xwayland` instance is used for the client, and validates
shell mapping, classic and modern theme rendering, Unicode/CJK text rasterization,
`xdg_popup` menus, Wayland clipboard and drag-and-drop transfers,
`zwp_text_input_v3` composition, rapid `xdg_toplevel.configure` resize-burst
coalescing, live scale reflow, and child-process socket isolation.
