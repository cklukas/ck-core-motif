# Buffered toolkit painting

Toolkit-owned full-frame controls paint through their normal Xt expose methods
into a back buffer and present only the completed frame. Existing applications
need no source changes. Real widget window IDs, event routing, public APIs,
application drawing callbacks and widget record layouts retain their contracts.
This is not a compositor for an entire window hierarchy.

The buffered controls are Avatar, Badge, BreadcrumbBar, Calendar, Chip, DataGrid,
DatePicker, Divider, IconLabel, InfoBar, Link, ProgressBar, ProgressRing, Prompt,
RatingControl, Spinner, TimePicker, ToggleSwitch and TreeView. Drawing helpers
use the transaction's Drawable; Window-only operations continue using the real
Xt window. Early returns still complete the background frame. Full-window
invalidation from these controls is coalesced through a zero-delay Xt timeout,
without first clearing the visible window. Destruction cancels pending work.

When the X11 DBE headers are available at configure time, libXm queries DBE and
the widget's actual visual. Supported windows use an Xdbe back buffer. Missing
headers, an absent extension, an unsupported visual, or a failed DBE allocation
select an ordinary server pixmap, presented with XCopyArea. If pixmap allocation
also fails, painting falls back to the actual window. No extension is required
for existing applications to run. libXm already links libXext.

Buffers are retained between frames, refreshed for changed size/depth/windows,
and released on widget destruction. The background is filled in the target
Drawable, including tiled and ParentRelative backgrounds; no XClearWindow call
is redirected to a back buffer. Core clearing requests affect both DBE buffers
and must not be used as offscreen background drawing.

For diagnosis, set the startup X resource `paintBuffer` (class `PaintBuffer`):

```
ckmotif-gallery -xrm '*paintBuffer: auto'
ckmotif-gallery -xrm '*paintBuffer: pixmap'
ckmotif-gallery -xrm '*paintBuffer: direct'
```

`auto` is the default; `pixmap` exercises the portable buffered path and `direct`
is an explicit diagnostic override. These resources apply to the listed
controls, not arbitrary application expose callbacks. DBE does not provide
vertical synchronization and cannot suppress X server exposure during mapping
or resizing. Application code drawing directly into XtWindow still draws into
the visible window.

## Validation

`tests/automated/check-buffered-paint.c` exercises real X drawables and verifies
that intermediate frame content does not change the front buffer, that the
completed frame is presented, and that resizing, unrealize/realize and pending
repaint destruction remain valid. Run with default resources, forced pixmap and
forced direct modes. Building Paint.c without the DBE feature define tests the
compile-time fallback; an X server without DOUBLE-BUFFER tests runtime absence.
Gallery native integration additionally verifies navigation, responsive layout,
and selection of appearance and scale while preserving the current example.

For a configured build and an explicit test display, run:

```
python3 tests/automated/run-buffered-paint.py --build /absolute/build --out /absolute/fresh-test-output
```

On macOS, place both paths on the designated project temporary volume. The
runner records source/library hashes, commands and results. Besides the linked
library's DBE, pixmap and direct paths, it compiles the same internal painter
without DBE headers, with extension discovery reporting absence, and with an
asynchronous DBE allocation-request error. The latter verifies fallback and
restoration of the application's X error handler.

Focus drawing shares the target Drawable with content drawing. TreeView focus
transitions repaint its complete frame, preserving selected-row outlines rather
than clearing them through the inherited Primitive border method. Native pixel
fixtures wait for window-manager viewability before capturing an X window.

The buffered classes schedule resource-driven redraws themselves instead of
returning an Xt redisplay request that would clear their front windows first.
Primitive's own redraw requests follow that path for already buffered controls.
The native regression also checks that a ProgressBar frame remains unchanged
between XtSetValues and the queued repaint. Native core background changes,
window mapping and geometry exposures still follow Xt/X11 semantics.
