---
nav_exclude: true
---
# Bugfix Notes Archive 10

## `fontselector-font-count-bounds.md`

# FontSelector font-list count bounds

`XmFontSelector` stores family and non-XLFD font counts in signed-short
fields, even though an X server can report many more names. The loader now
caps each retained list at the representable range, stops adding unrepresentable
families, and performs its allocation byte conversions only after checking Xt's
`Cardinal` limit. Sorting and assignments use size-correct, explicitly bounded
counts.

The focused source contract is covered by
`tests/automated/check-fontselector-font-count-bounds.py`.

## `fontselector-portability.md`

# FontSelector portability hygiene

`XmFontSelector` keeps its supported resolution table in the same signed-short
representation used by `FontInfo`, so resolution selection cannot narrow an
arbitrary `int` into widget state. Iteration over Xt counts now uses `Cardinal`,
and the encoding-menu local index has an independent name rather than shadowing
the outer Xt argument index. `GetValuesHook` compares `XtArgVal` through its
documented integer representation before treating it as an output address.

These changes preserve the legacy public API while removing conversion,
signedness, shadowing, and pointer-versus-integer diagnostics under the strict
compiler profile. The focused source contract is covered by
`tests/automated/check-fontselector-portability.py`.

## `fontselector-toggle-state.md`

# FontSelector toggle callback state

`XmFontSelector` now normalizes the integer `set` member delivered by toggle
callbacks to Motif's `Boolean` representation before storing it in widget
resources or passing it to flag helpers. This preserves the public callback
contract while avoiding implementation-defined narrowing when callback state is
carried through Boolean state fields. The encoding matcher also makes its
non-positive capacity rejection an unambiguous standalone branch.

The focused source contract is covered by
`tests/automated/check-fontselector-toggle-state.py`.

## `form-layout-decision.md`

# WP-11 Form/hierarchy correction decision

WP-11 requires a source-backed layout defect or redundant-work hotspot before
changing `lib/Xm/Form.c`.  The WP-10 installed consumer was run twice against
the same fresh `uconsole` prefix under Xvfb.  Both result files had SHA-256
`6df8761d3757bcfa4d7751c2d38467c8764ee06aafdd2a4a67fa88df4ec3eacb` and the
same metrics:

| Observation | Count |
| --- | ---: |
| `XtMakeGeometryRequest` | 11 |
| `XtQueryGeometry` | 26 |
| `XClearArea` | 62 |
| `XCopyArea` | 125 |
| `XFillRectangle` | 209 |
| Configure events | 68 |
| Expose events | 89 |
| Map / Unmap events | 5 / 1 |
| Callback mutations | 4 |
| No-op geometry requests | 0 |
| No-op Configure events | 0 |

The corpus also completed a twenty-step resize storm, nested
`XmScrolledWindow`/`XmForm` layout, post-realization child insertion and
destruction, dynamic manage/unmanage, callback-triggered mutation and a
map/unmap cycle while preserving positive final child geometry.

## Decision

No generic `Form.c` correction is justified by the current evidence.  The
counter values show activity, but they do not identify any activity as
redundant or incorrect; the explicit no-op control is clean and the repeated
run is deterministic.  Changing attachment evaluation or pass scheduling
would therefore be speculative and could alter legacy Xt negotiation.

WP-11 is complete with **no source change** under the roadmap's explicit
completion rule.  A future correction remains possible only after a
reproducible defect or hotspot is supplied by a real client, a broader
platform/window-manager run, or a more precise toolkit instrumentation
contract.  Any such correction must rerun the WP-10 corpus and preserve final
geometry, focus order, child destruction and callback reentrancy.

This decision is intentionally not a performance claim for every Form layout,
X server or architecture.  x86_64, cyclic/conflicting attachment diagnostics,
and multi-window-manager qualification remain open evidence gaps.

## `form-shadow-redisplay-guards.md`

# Form shadow redisplay guards

`XmForm` draws its manager shadow from both the resize path (when a shrink may
not generate an expose) and the ordinary expose path.  Those paths previously
assumed that the window, manager shadow GCs, thickness, and dimensions were
already valid.

Both paths now use one prerequisite check.  Invalid intermediate state skips
only the shadow request; child placement and gadget redisplay continue, and a
valid realized form retains its existing shadow geometry and type.

## `frame-baseline-count.md`

# Frame title baseline count

Frame title layout uses the baselines returned by `XmWidgetGetBaselines` for
baseline-aligned titles.  A successful query must still be treated as empty
when it returns no baseline array or a zero count; indexing `nlines - 1` in
that case would underflow before the fallback geometry can be used.  The
layout now requires both a non-null array and a positive count before reading
the first or last baseline.

The regression contract is checked by
`tests/automated/check-frame-baseline-count.py`.

## `frame-shadow-redisplay-guards.md`

# Frame shadow redisplay guards

`XmFrame` caches the previous shadow rectangle so it can repaint a frame
around title and work-area changes.  During realization, resource replacement,
or a zero-sized geometry transition, that cached state can be incomplete.
`DrawShadow` now requires both manager shadow GCs, non-zero cached dimensions,
and a non-zero shadow thickness before calling `XmeDrawShadows`.

This only suppresses an invalid intermediate draw.  Realized frames with a
valid rectangle continue to use the existing shadow type and geometry, and
frame title/gadget expose processing is unchanged.

## `gadget-default-dimensions.md`

# Gadget default dimensions

When a Gadget receives no explicit width or height, Motif expands its base
size for highlight and shadow thickness. The calculation now uses a wide
intermediate and saturates at the representable `Dimension` maximum instead of
wrapping a large valid thickness into a small control. Ordinary dimensions are
unchanged.

The focused source contract is covered by
`tests/automated/check-gadget-default-dimensions.py`.

## `gallery-build-safety.md`

# Gallery build-safety guards

The component gallery keeps strict warnings enabled, because its screenshots
and example controls are release-facing evidence.  Geometry used for Xt
`Dimension` resources is now explicitly clamped at the legacy resource width,
and screenshot dimensions are checked before conversion to Xlib or file I/O
sizes.  Text extraction rejects strings that exceed Xt's `Cardinal` allocation
contract.

The flow-page layout also keeps all intermediate inset and cumulative-height
calculations in an unsigned extent type. It narrows only at the Xt resource
boundary: canvas sizes clamp through `dimension_from_extent`, while child
positions clamp through `position_from_extent`. This prevents signed promotion
and narrowing from wrapping a gallery page as labels or navigation content
become large.

The toolbar sample requests normal and overflow action buttons separately.  A
non-overflowable action has no overflow button, so the gallery now skips that
absent widget before assigning its Open, Save, Print, or Undo icons.

The focused source-contract check is:

```sh
python3 tests/automated/check-gallery-narrowing-guards.py
```

## `gallery-shell-examples.md`

# Gallery shell examples

The Dialog Shell page opens a populated, modeless dialog owned by the gallery's
top-level window. Its Form child supplies a heading, explanatory text, a Close
action and the Dialog Shell positioning contract. The child starts unmanaged;
opening manages it, and Close or Escape unmanages it. Reopening reuses the same
window and centers it over the gallery, subject to screen-edge clamping.

Construct a Dialog Shell with a dialog-savvy manager child, such as Form or
BulletinBoard, and manage that child to open the dialog. A plain label plus a
separate `XtPopup` does not demonstrate that lifecycle or its default placement.
The example's transient owner is the gallery shell, rather than its launcher
button. These are gallery construction changes; the library's application
positioning and shell APIs are unchanged.

The launcher is constructed with its complete label before initial geometry
negotiation. Its text, margins and focus frame must fit after palette, density
and scale changes.

To open this example immediately for manual inspection:

```sh
ckmotif-gallery --component xm.dialogshellwidgetclass --open-example
```

Native acceptance exercises the actual gallery page and its launcher/Close
callbacks on an isolated display:

```sh
python3 tests/automated/run-gallery-dialog-shell.py \
  --build /absolute/build --results /absolute/fresh-results
```

`--integration-shells` checks Classic, modern light and modern dark, multiple
scales/densities, launcher bounds, visible rendered content, child bounds,
transient ownership, centered placement and repeated close/reopen cycles.
The gallery builds this verifier with strict warning checks; keep the horizontal
and vertical screen-edge clamps as distinct statements so both the compiler and
reviewers see the independent bounds clearly.

## `geoutils-box-width-comparator.md`

# GeoUtils box-width ordering

The geometry allocator now uses a proper three-way comparator when sorting
boxes by width. The previous boolean result could report only equality or
“greater,” violating `qsort`'s comparator contract and producing unstable
layout ordering.

## `geoutils-debug-bounds.md`

# GeoUtils debug-label bounds

The optional geometry diagnostics prepend caller-supplied headers to list and
matrix coordinates.  The old debug-only code used `sprintf` into 256-byte
stack buffers, so a long header could corrupt the diagnostic process.

Both debug label formats now use `snprintf` with the destination size.  The
diagnostic text is unchanged when it fits.

`tests/automated/check-geoutils-debug-bounds.py` protects these paths.

## `getsubres-c23-reporting.md`

# GetSubRes C23 callbacks and report storage

The GetSubRes demonstration now uses typed C23 declarations for its class
lookup, application entry point, and callbacks.  Resource report rows are
formatted into exact storage using `vsnprintf`, then appended only when their
length fits Motif's legacy allocation and text-column boundaries.  This avoids
the former fixed-format buffer overflow while preserving the report content.

The focused source-contract check is:

```sh
python3 tests/automated/check-getsubres-c23-reporting.py
```

## `hash-table-limits.md`

# Hash-table accounting limits

The internal Motif hash table exposes its number of entries as a `Cardinal`.
Insertion now fails once that counter reaches its representable maximum, so a
very large table cannot wrap its reported count to zero. Existing insertions,
lookups, removal order, and table sizing are unchanged.

When compiled with `DEBUG`, an empty table's diagnostic summary now reports a
zero average chain length instead of dividing by zero.

The focused regression is `tests/automated/check-xm-hash-lifecycle.py`.

## `header-bar-decision.md`

# Header-bar decision

WP-40 evaluates a header-bar presentation separately from the bounded
client-area composites.  This package does not change window-manager title-bar
controls, decorations, drag regions, system menus or shell integration.

The decision is **defer**.  Xt and ordinary X11 do not provide a portable
contract for replacing or extending the window-manager frame.  A client-area
header can be composed from existing Motif managers, but calling it a title-bar
replacement would make decoration ownership, hit testing, accessibility and
window-management behavior depend on the selected desktop and compositor.

Reopen this decision only with a versioned server/desktop contract, a real
window evidence matrix and an explicit shell-boundary ownership model.  The
existing `XmToolbar` and `XmSidebar` remain the supported client-area
composition points and do not imply title-bar integration.

## `heap-alloc-alignment.md`

# Legacy heap allocation alignment

`_XmHeapAlloc` now validates and rounds requested sizes before deciding whether
the current segment has enough capacity. This prevents alignment arithmetic
from wrapping and keeps `bytes_remaining` from underflowing. Successful heap
allocation and alignment remain unchanged.
`_XmHeapFree(NULL)` is also a no-op, matching the allocator’s null-safe
failure behavior.

The focused regression check is
`tests/automated/check-heap-alloc-alignment.py`.

## `hierarchy-allocation.md`

# Hierarchy list allocation

Hierarchy node-table and child-list growth now allocates into temporary
pointers and publishes the replacement only after `XtRealloc` succeeds.  Size
products and `Cardinal` growth are checked before allocation.  A failed growth
leaves the existing list available and skips the operation instead of losing
the old pointer or dereferencing null storage.

`tests/automated/check-hierarchy-growth-allocation.py` protects these paths.

## `hierarchy-child-insertion-braces.md`

# Hierarchy child insertion control flow

The hierarchy widget's `AddChildToList` routine nests an insertion search,
its sibling loop, and the matching-child branch. One closing brace was
missing after the search, so later helper definitions were parsed as nested
functions and clean builds failed with invalid-storage-class errors.

The missing brace is restored. Child ordering, duplicate handling, and the
public hierarchy API are unchanged; this is a source-structure correction.

## `hierarchy-child-list-bounds.md`

# Hierarchy child-list allocation bounds

Hierarchy's dynamic child-constraint list now checks the computed byte count
against Xt's `Cardinal` limit after the existing host-size multiplication
check.  Oversized growth requests return without passing a truncated size to
`XtRealloc`.

The focused source-contract check is:

    python3 tests/automated/check-hierarchy-child-list-bounds.py

## `hierarchy-init-failure.md`

# Hierarchy initialization failure

Hierarchy now initializes its ownership fields before allocating the root
constraint node. If that allocation fails, the widget remains in a valid
empty state, and destruction skips the absent child array instead of
dereferencing a null root node. Normal hierarchy construction is unchanged.

## `hierarchy-node-table-allocation.md`

# Hierarchy node-table allocation

`BuildNodeTable` rebuilds the hierarchy's constraint table from the number of
managed children.  The byte-size check now covers both `size_t` and Xt's
`Cardinal` allocation range before multiplication.  The existing table remains
published when `XtRealloc` fails, so the hierarchy does not dereference a
truncated or null allocation.

## `i18-list-adjust-position-bounds.md`

# I18List visibility movement bounds

`AdjustVisiblePosition` now negates movement values in wide arithmetic and
clamps them before updating the legacy `short` horizontal location.  This
avoids the signed-minimum negation and compound-assignment wrap that could
send a visibility adjustment in the wrong direction.

The source-contract regression is
`tests/automated/check-i18-list-adjust-position-bounds.py`.

## `i18-list-available-height.md`

# I18List available-height arithmetic

`AdjustFirstRowAndCol` now computes the content height available for rows in
signed wide arithmetic and clamps negative results to zero.  Small widgets
can therefore not underflow their unsigned `Dimension` height and produce an
invalid rows-per-screen count.

The source-contract regression is
`tests/automated/check-i18-list-available-height.py`.

## `i18-list-background-height.md`

# I18List background fill height

The full-list background fill now subtracts visual-style spacing only when
the widget height can contain it; otherwise it uses zero.  Small widgets no
longer underflow the unsigned `Dimension` and request an enormous fill
rectangle.

The source-contract regression is
`tests/automated/check-i18-list-background-height.py`.

## `i18-list-cell-edge-bounds.md`

# I18List cell right-edge bounds

`MakeCellVisible` now computes a cell's right edge in a wide type and clamps
it before passing the value to the legacy visibility helper.  This prevents a
wide column plus its starting offset from wrapping through `short` and
making the wrong portion of the cell visible.

The source-contract regression is
`tests/automated/check-i18-list-cell-edge-bounds.py`.

## `i18-list-column-pixel-bounds.md`

# I18List column pixel-offset bounds

The helper that converts a column number to its pixel offset now accumulates
column widths in `long long` and clamps the historical `short` result.  Wide
lists therefore cannot wrap their starting pixel offset into a negative or
otherwise incorrect position during layout and hit testing.

The source-contract regression is
`tests/automated/check-i18-list-column-pixel-bounds.py`.

## `i18-list-display-column-origin.md`

# I18List display column origin

`DisplayList` now bounds its initial RTL and LTR column origins before walking
the columns.  A wide widget or horizontal offset can no longer wrap `cur_x`
before the per-column movement safeguards run.

The source-contract regression is
`tests/automated/check-i18-list-display-column-origin.py`.

## `i18-list-display-column-steps.md`

# I18List display column steps

`DisplayList` now clamps each RTL/LTR column advancement, including the
inter-column spacing step.  Wide column metrics can no longer wrap `cur_x`
before header, text, or pixmap drawing.

The source-contract regression is
`tests/automated/check-i18-list-display-column-steps.py`.

## `i18-list-display-fill-origin.md`

# I18List display fill origins

The selected-row and entry-background fills in `DisplayList` now use bounded
coordinate arithmetic for both layout directions.  Wide content and widget
widths can no longer wrap the fill origin before it reaches Xlib.

The source-contract regression is
`tests/automated/check-i18-list-display-fill-origin.py`.

## `i18-list-display-row-y.md`

# I18List display row Y bounds

`DisplayList` now computes its initial row baseline in wide arithmetic and
clamps it.  Each subsequent row advance is clamped as well, preventing large
row offsets or metrics from wrapping the `Position` used by fills and pixmap
copies.

The source-contract regression is
`tests/automated/check-i18-list-display-row-y.py`.

## `i18-list-display-text-baseline.md`

# I18List display text baseline

The text/icon rendering branch of `DisplayList` now computes its starting row
baseline in wide arithmetic and clamps both visual-style adjustments.  Large
row offsets can no longer wrap the baseline before text and pixmap rendering.

The source-contract regression is
`tests/automated/check-i18-list-display-text-baseline.py`.

## `i18-list-first-column-bounds.md`

# I18List first-column restoration bounds

`AdjustFirstRowAndCol` now accumulates the restored horizontal origin in
`long long` and clamps both width-derived assignments before storing the
legacy `short` location.  This prevents wide column sets or widget widths
from wrapping the restored first-column position.

The source-contract regression is
`tests/automated/check-i18-list-first-column-bounds.py`.

## `i18-list-header-baselines.md`

# I18List header and icon baselines

`DisplayList` now bounds the title and first-column-icon baseline setup,
including visual-style spacing adjustments.  Separator metrics that exceed the
legacy `Position` range can no longer wrap before header text or row pixmaps
are drawn.

The source-contract regression is
`tests/automated/check-i18-list-header-baselines.py`.

## `i18-list-hit-test-height.md`

# I18List hit-test height arithmetic

I18List motion tracking compares the event Y coordinate with the widget
height in a signed wide type.  The previous cast to `short` could wrap a
large `core.height` before the off-bottom check, causing motion state to be
reported incorrectly for tall list widgets.  The change preserves the
public widget API and only removes that intermediate narrowing.

The source-contract regression is
`tests/automated/check-i18-list-hit-test-height.py`.

## `i18-list-invert-origin-bounds.md`

# I18List selection-inversion origin bounds

`InvertArea` now computes the full-row inversion origin in wide arithmetic and
clamps both right-to-left and left-to-right origins before drawing.  Wide
content or widget widths can no longer wrap the selection highlight origin
through the legacy short coordinate.

The source-contract regression is
`tests/automated/check-i18-list-invert-origin-bounds.py`.

## `i18-list-left-arrow-bounds.md`

# I18List left-arrow scrolling bounds

The horizontal left-arrow callback now combines its partial-column
adjustments in wide arithmetic and clamps the resulting horizontal origin.
This avoids compound `short` updates wrapping when a list is positioned near
the representable coordinate limits.

The source-contract regression is
`tests/automated/check-i18-list-left-arrow-bounds.py`.

## `i18-list-left-visibility-move-bounds.md`

# I18List left-visibility movement bounds

When a cell begins left of the visible window, `MakePositionVisible` derives
a negative movement from the visible origin and cell start.  That movement is
now calculated in wide arithmetic and clamped before entering the historical
short-position helper, preventing large offsets from wrapping into a positive
movement.

The source-contract regression is
`tests/automated/check-i18-list-left-visibility-move-bounds.py`.

## `i18-list-pixel-hit-bounds.md`

# I18List pixel hit-test bounds

Pixel-to-column hit testing now accumulates column positions in `long long`.
Previously, the `short` accumulator could wrap while traversing wide columns,
causing selection and pointer hit testing to identify the wrong column.  The
existing `short` event-coordinate and column-index interfaces remain intact.

The source-contract regression is
`tests/automated/check-i18-list-pixel-hit-bounds.py`.

## `i18-list-pixmap-centering.md`

# I18List pixmap centering

Selection highlighting now centers a row pixmap using signed wide arithmetic
before clamping the legacy short Y offset.  A pixmap taller than its row no
longer causes unsigned subtraction to underflow and place the image far from
the row.

The source-contract regression is
`tests/automated/check-i18-list-pixmap-centering.py`.

## `i18-list-position-row-bounds.md`

# I18List position-to-row bounds

`CvtPositionToRowColumn` now keeps the Y coordinate in a wide local while
subtracting the header and spacing offsets.  The previous in-place mutation
of the `short` parameter could wrap for negative or edge coordinates before
row calculation; normal event and widget APIs remain unchanged.

The source-contract regression is
`tests/automated/check-i18-list-position-row-bounds.py`.

## `i18-list-right-arrow-bounds.md`

# I18List right-arrow scrolling bounds

The horizontal right-arrow callback now computes content/window differences
in wide arithmetic and clamps every resulting horizontal-origin update before
storing the legacy `short` value.  This prevents large content widths or
column edges from wrapping the scroll position.

The source-contract regression is
`tests/automated/check-i18-list-right-arrow-bounds.py`.

## `i18-list-row-position-bounds.md`

# I18List row-to-position bounds

`CvtRowColumnToPosition` now computes row Y coordinates in wide arithmetic
and clamps the legacy `short` output.  Large row offsets or row metrics can
no longer wrap the returned position and place the rendered cell incorrectly.

The source-contract regression is
`tests/automated/check-i18-list-row-position-bounds.py`.

## `i18-list-rtl-pixmap-width.md`

# I18List RTL pixmap placement

Right-to-left pixmap placement now subtracts the pixmap width from the right
edge when computing its X destination.  The previous use of pixmap height
misplaced non-square images horizontally; Y centering continues to use the
height as intended.

The source-contract regression is
`tests/automated/check-i18-list-rtl-pixmap-width.py`.

## `i18-list-scroll-callback-bounds.md`

# I18List scrollbar callback bounds

Scrollbar callback values are supplied as integers but the historical I18List
scroll helpers accept `short` amounts.  The callbacks now clamp values to the
representable `short` range before calling those helpers.  This prevents a
large externally supplied scrollbar value from wrapping into an opposite
scroll direction while preserving the existing internal and public API
contracts.

The source-contract regression is
`tests/automated/check-i18-list-scroll-callback-bounds.py`.

## `i18-list-selected-fill-y.md`

# I18List selected-row fill Y advancement

The selected-row background loop in `DisplayList` now clamps each row-height
advance before drawing the next fill.  This closes the remaining direct
`Position` increment that could wrap on large row metrics.

The source-contract regression is
`tests/automated/check-i18-list-selected-fill-y.py`.

## `i18-list-slider-height.md`

# I18List slider-height arithmetic

`ResizeSliders` now computes the vertical content height in signed wide
arithmetic before deriving rows per screen.  Small widgets no longer
underflow their unsigned `Dimension` height while subtracting title and
visual-style spacing, so scrollbar sizing remains stable.

The source-contract regression is
`tests/automated/check-i18-list-slider-height.py`.

## `i18-list-text-centering.md`

# I18List text centering

Text centering in `DisplayList` now derives a signed wide offset before
updating the row baseline.  A text run taller than its row no longer causes
unsigned subtraction to underflow, and each baseline update is clamped before
the next draw.

The source-contract regression is
`tests/automated/check-i18-list-text-centering.py`.

## `i18-list-total-width-bounds.md`

# I18List total-width bounds

I18List retains its historical `short` width helper contract, but now sums
column widths in `long long` and saturates at `SHRT_MAX` before returning.
Without this guard, many wide columns could overflow the old cast and produce
a negative or otherwise incorrect content width, corrupting resize and
horizontal-scroll calculations.

The source-contract regression is
`tests/automated/check-i18-list-total-width-bounds.py`.

## `i18-list-visibility-padding-bounds.md`

# I18List visibility-padding bounds

`MakePositionVisible` adds horizontal padding around a cell's right edge.
That padding is now applied in wide arithmetic and clamped before storing the
legacy `short` coordinate.  A cell near the representable edge can no longer
wrap when the visibility calculation adds its normal spacing.

The source-contract regression is
`tests/automated/check-i18-list-visibility-padding-bounds.py`.

## `i18-list-visible-height-bounds.md`

# I18List visible-height bounds

`IsRowVisible` probes the bottom of the widget by passing its height plus one
to the row/column conversion helper.  That value is now clamped through the
shared bounded-short conversion instead of being narrowed directly.  Tall
widgets therefore cannot wrap the probe to a negative coordinate and report
the wrong visible row.

The source-contract regression is
`tests/automated/check-i18-list-visible-height-bounds.py`.

## `i18-list-visible-origin-bounds.md`

# I18List visible-origin bounds

`MakePositionVisible` derives the visible origin from the current horizontal
offset.  The adjustment now uses wide arithmetic and clamps to the legacy
`short` range, preventing an offset at the negative limit from wrapping the
origin and misclassifying a cell as already visible.

The source-contract regression is
`tests/automated/check-i18-list-visible-origin-bounds.py`.

## `i18-list-visible-window-bounds.md`

# I18List visible-window bounds

`MakePositionVisible` now computes the visible-window end once in
`long long` arithmetic.  Derived beginning, ending, movement, and remaining
width values are clamped before entering legacy `short` helpers.  This keeps
wide widget dimensions from wrapping while preserving normal-size scrolling
behavior and the existing API.

The source-contract regression is
`tests/automated/check-i18-list-visible-window-bounds.py`.

## `i18list-row-helper-inputs.md`

# I18List row-helper input guards

Shared I18List row-selection helpers now validate their widget before reading
row storage or looking up an Xt application context. This covers selected-row
array retrieval and all public select/deselect wrappers while preserving valid
selection and notification behavior.
The row-visibility convenience wrapper also returns safely for a NULL list.

The focused source contract is checked by
`tests/automated/check-i18list-row-helper-inputs.py`.

## `i18list-search-inputs.md`

# I18List selection and search input guards

`XmI18ListGetSelectedRows` now returns `NULL` for a NULL widget before row
storage is accessed. `XmI18ListDoSearch` rejects a NULL widget or search string
before creating a compound string or traversing rows. Valid selection and
multilingual search behavior is unchanged. `XmI18ListFindRow` also validates
its search string and output-column pointer.

The focused source contract is checked by
`tests/automated/check-i18list-search-inputs.py`.

## `icon-cache-directory-bounds.md`

# Icon-directory cache accumulation bounds

The icon-directory cache stores directory entries in a fixed 64 KiB staging
buffer. Its entry-length and accumulated-offset arithmetic now stays in
`size_t` and checks the remaining capacity before copying a name. This avoids
narrowing a `strlen` result to `int` before the bound check.

The focused source regression is
`tests/automated/check-icon-cache-directory-bounds.py`.

Directory cache records store their key length as an `int`; path components
outside that range now use the existing uncached fallback rather than
truncating the cache key.

## `icon-cache-flush-length-bounds.md`

# Icon-cache flush path length bounds

`XmeFlushIconFileCache` now retains the path length returned by `strlen` as a
`size_t` while comparing and flushing cached directory entries. This avoids a
`Cardinal` narrowing wrap for unusually long invalidation paths.

The focused source regression is
`tests/automated/check-icon-cache-flush-length.py`.

## `icon-cache-path-length-bounds.md`

# Icon-cache path length bounds

`CheckDirCache` now retains directory, file, and cached-entry name lengths as
`size_t` while deriving them from paths and offset arrays. This prevents a
long path from narrowing through `int` before cache matching, allocation, or
`strncmp` calls; the stored legacy record layout remains unchanged.

The focused source regression is
`tests/automated/check-icon-cache-path-lengths.py`.

## `icon-cache-pointer-types.md`

# Keep icon cache pointer conversions explicit

The icon-directory cache stores opaque pointers to a union so invalid and
valid cache entries share one table.  `CheckDirCache` consumes valid entries
through the more specific `DtValidCachedDir` pointer.  The code now makes the
two representation-preserving conversions explicit when inserting and
retrieving entries.

This documents the intended layout and avoids incompatible-pointer-type
errors from strict compilers without changing the cache ABI or allocation
ownership.

## `icon-directory-bounds.md`

# Icon-directory allocation bounds

The icon-name cache copies directory prefixes into Xt-owned storage. The
directory lookup path now rejects lengths that cannot represent the trailing
NUL byte or Xt `Cardinal` allocation width, and passes the checked byte count
explicitly. Existing uncached-directory fallback behavior is preserved.

## `icon-directory-cache.md`

# Icon-directory cache allocation

Icon-directory cache growth now publishes a replacement directory array only
after `XtRealloc` succeeds, with checked count arithmetic against both host
`size_t` and Xt's `Cardinal` byte range.  Valid directory
entries accumulate the name heap in `size_t` and check the complete
offset-array plus name-heap size before allocation.  Directory-name and
cached-entry allocations are checked and return an uncached result cleanly
when memory is unavailable, leaving the process-wide cache consistent.

`tests/automated/check-icon-cache-growth-allocation.py` protects these paths.

## `icon-file-bounds.md`

# Sized icon-name construction bounds

Icon lookup appends the requested size suffix to an image name before probing
its cache and search paths. The suffix path now keeps lengths in `size_t`,
checks both the combined allocation size and Xt's `Cardinal` width, and treats
an unrepresentable or failed allocation as a normal lookup miss. Existing valid
icon names and cache behavior are unchanged.

The source-level regression is
`tests/automated/check-icon-suffix-bounds.py`.

Cache-entry path joining applies the same checked `size_t` arithmetic and
checks the resulting size against Xt's `Cardinal` width. It skips an
unrepresentable or unavailable cache entry and falls through to normal path
lookup without changing valid cache behavior.

When registering a resolved icon in the cache, the directory component length
is also kept in `size_t` and checked before the Xt allocation. An
unrepresentable directory component skips only the optional cache insertion;
the resolved icon remains usable through the normal lookup result.

## `icon-hash-bounds.md`

# Icon-name hash portability

Icon-cache hashing now bounds its unsigned name mix before converting to
Motif's signed `XmHashValue` type. High-bit lengths or name bytes therefore do
not rely on implementation-defined signed narrowing.

## `icon-mask-name-allocation.md`

# IconG mask-name allocation

Icon gadget mask lookup used the same fixed 255-byte mask-name buffer as the
legacy image-cache path.  Arbitrarily long icon names could overflow that
stack buffer before the mask lookup.

IconG now allocates the generated name from the caller's image-name length,
releases it after lookup, and leaves the mask unspecified if allocation fails.
Existing mask lookup and destruction semantics are unchanged.

`tests/automated/check-icon-mask-name-allocation.py` protects this contract.

## `icon-name-hash.md`

# Icon-name hash tail selection

The icon-name cache hash now incorporates the final character of a non-empty
name. The previous implementation indexed the terminating NUL byte, causing
many names with the same first character and length to collide. Empty names
continue to use index zero safely.

The focused source regression is
`tests/automated/check-icon-name-hash-tail.py`.

## `iconbutton-string-conversion.md`

# IconButton string conversion bounds

`XmIconButton` converts its legacy character label to an `XmString` through an
Xt `XrmValue`. It now rejects a missing label or a label whose NUL-terminated
byte length cannot fit Xt's `Cardinal` source-size field before conversion.
Ordinary label conversion and fallback-to-widget-name behavior are unchanged.

The focused regression check is
`tests/automated/check-iconbutton-string-conversion.py`.

## `icong-baseline-allocation.md`

# IconG baseline allocation

IconG baseline queries allocate a one-entry `Dimension` array for the
calculated baseline.  The allocation is now checked before the value is
written; failure returns zero lines and a null baseline result.

The focused source contract is checked by
`tests/automated/check-icong-baseline-allocation.py`.

## `icong-mask-name-bounds.md`

# IconG mask-name allocation

When an icon needs an implicit mask, IconG derives a cache name from the image
name.  The name length is now checked for `size_t` overflow and Xt's
allocation-width limit before suffix space is added.  Missing or
unrepresentable names skip mask creation safely.

The focused source contract is checked by
`tests/automated/check-icong-mask-name-bounds.py`.

## `im-close-xim-inputs.md`

# XIM close input contract

`XmImCloseXIM` tears down the input-method state associated with a widget.
During focus or widget teardown the widget may already be absent; the
function now returns before application-context lookup when passed `NULL`.
Valid XIM cleanup is unchanged.

## `im-focus-inputs.md`

# Input-method focus/value input contract

`XmImSetFocusValues`, `XmImSetValues`, and `XmImUnsetFocus` now return when
their widget argument is `NULL`, before application-context lookup or widget
parent traversal.  This makes focus cleanup and optional IM updates safe
during teardown while preserving valid-widget behavior.

## `im-get-xic-inputs.md`

# XIC creation input contract

`XmImGetXIC` creates or retrieves an input context for a widget.  A null
widget cannot identify an input-method registration, so the function now
returns `NULL` before application-context lookup.  Valid XIC creation and
reuse are unchanged.

## `im-get-xim-inputs.md`

# XIM query input contract

`XmImGetXIM` returns the input-method connection associated with a widget.
When no widget is available, there is no display or registration to query;
the function now returns `NULL` before application-context lookup.  Valid XIM
queries are unchanged.

## `im-lookup-inputs.md`

# Input-method lookup input contract

`XmImMbLookupString` now rejects a null widget or key event, negative output
capacity, and a missing output buffer when the requested capacity is nonzero.
It returns zero before application-context lookup or XIM access.  Empty
capacity with a null buffer remains valid, and normal lookup behavior is
unchanged for valid arguments.

## `im-register-inputs.md`

# Input-method registration input contract

`XmImRegister` now returns immediately when its widget is `NULL`, before
application-context lookup or parent-shell traversal.  This makes optional
registration during failed construction safe; valid registration behavior is
unchanged.

## `im-reset-inputs.md`

# Input-method reset input contract

`XmImMbResetIC` writes the reset result through its output pointer and then
resets the widget's input context.  It now returns when either the widget or
output pointer is missing, before app-context lookup or output dereference.
Valid reset operations are unchanged.

## `im-reset-state-inputs.md`

# XIC reset-state query input contract

`XmImGetXICResetState` now returns the default `XIMInitialState` when passed
a null widget, before looking up input-method registration state.  Queries for
valid widgets retain their existing XIC reset-state behavior.

## `im-set-xic-inputs.md`

# XIC setter input contract

`XmImSetXIC` associates or queries an input context for a widget.  A null
widget has no registration to update; the function now returns `NULL` before
application-context lookup.  Valid association and query behavior is
unchanged.

## `im-varargs-inputs.md`

# Input-method varargs setter input contract

`XmImVaSetFocusValues` and `XmImVaSetValues` now return immediately when
their widget argument is `NULL`, before application-context lookup or varargs
processing.  Valid varargs updates are unchanged.

## `image-cache-buffer-bounds.md`

# Image-cache scaled buffer bounds

Scaled image rows are allocated through Xt after their byte count is computed
in `size_t`.  The byte count is now also required to fit Xt's `Cardinal`
argument before calling `XtMalloc`, preventing truncation on 64-bit hosts.
Oversized scaled buffers are discarded cleanly; normal scaling is unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-image-cache-buffer-bounds.py
```

## `image-cache-depth-sentinel.md`

# Image depth sentinel bounds

Motif accepts negative image depths as a compatibility convention for bitmap
and pixmap selection. `INT_MIN` cannot be negated in a signed `int`, so
`_XmGetScaledPixmap` now rejects it before normalization. Other valid depth
values retain their existing behavior.

The focused source contract is checked by
`tests/automated/check-image-depth-sentinel.py`.

## `image-cache-depth1-bounds.md`

# Depth-1 image-copy bounds

The depth-1 XYPixmap conversion path now rejects a checked image byte count
larger than Xt's `Cardinal` allocation range before copying image data. The
bounded count is converted explicitly for the allocator, avoiding truncation
on platforms where `size_t` is wider.

The focused source contract is checked by
`tests/automated/check-image-scaling-depth1.py`.

## `image-cache-geometry-bounds.md`

# Cached pixmap geometry bounds

`_XmCachePixmap` now rejects X server geometry that cannot fit the cache
record's public `Dimension` width and height fields or its integer depth field.
This prevents large queried pixmaps from being silently narrowed before
metadata consumers use them.

The focused source contract is covered by
`tests/automated/check-cache-geometry-status.py`.

## `image-cache-growth-rollback.md`

# Image-cache growth rollback

`CacheColorPixel` now computes the proposed cache capacity separately and
publishes it only after `XtRealloc` succeeds.  If growth fails, the existing
capacity remains authoritative, so a later insertion cannot mistake an
unallocated range for usable cache storage.

The source-contract regression is
`tests/automated/check-image-cache-growth-rollback.py`.

## `image-cache-hash-bounds.md`

# ImageCache hash portability

Image and GC cache hash values now bound their 32-bit mixes before converting
to Motif's signed `XmHashValue` type. This avoids implementation-defined
high-bit narrowing while preserving deterministic cache lookup behavior.

## `image-cache-hash-tail.md`

# ImageCache string hash distribution

The internal image-cache string hash now incorporates the final character of
the name. The previous code read the terminating NUL byte for every non-empty
name, needlessly collapsing hash entropy and increasing collisions.

## `image-cache-print-resolution-bounds.md`

# Scaled-image print resolution bounds

The optional Xprint fast path derives an integer image resolution from the
source-to-destination scale ratio.  A tiny ratio can produce a value larger
than `INT_MAX`; the renderer now verifies finiteness and range before narrowing
to the Xprint API's `int` parameter.  Out-of-range values use the normal
scaled-image path instead of invoking the extension with an invalid value.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-image-print-resolution-bounds.py
```

## `image-cache-scaling-ratio.md`

# Image-cache scale-ratio inputs

`_XmGetScaledPixmap` now rejects negative and non-finite scale ratios before
cache lookup or image loading. A ratio of zero remains valid and continues to
select the print-resolution scaling mode.

The focused source contract is checked by
`tests/automated/check-image-scaling-ratio-input.py`.

## `image-cache-scaling-strip-bound.md`

# Scaled-image strip endpoint safety

`_XmPutScaledImage` renders large images in vertical strips.  The final strip
may start close to the largest representable unsigned coordinate, so adding
the nominal strip height before checking the remaining range can wrap and
produce an invalid endpoint.  The renderer now clamps the strip height from
the remaining distance first, then performs the addition.

The source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-image-scaling-strip-bound.py
```

## `image-cache-table-bounds.md`

# Image-cache scaling table bounds

Image scaling allocates four coordinate and span tables through Xt.  The
allocation guards now require each table size to fit both native `size_t`
arithmetic and Xt's `Cardinal` allocation argument.  This avoids truncating a
large 64-bit dimension when it is passed to `XtMalloc`, while leaving normal
scaling unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-image-cache-table-bounds.py
```

## `image-cache-warning-state.md`

# Image-cache warning state

Several image-cache callbacks receive hash keys or closure pointers required by
the generic callback ABI but not by their particular operation. The lock
wrappers also intentionally compile to no-ops in non-threaded builds. These
paths now mark that state explicitly, keeping strict warning builds useful
without changing callback signatures or locking behavior.

The source contract is covered by
`tests/automated/check-image-cache-warning-state.py`.

## `image-color-release.md`

# Image color-release callback

The XPM color-release callback decrements Motif's shared color-cache
references and is intentionally best-effort: the XPM reader does not consume a
failure result from this callback.  The implementation now makes that
contract explicit by discarding each internal release result and returning
zero after validating its inputs.  It no longer carries a dead status value
that suggested a result was propagated.

The callback contract is covered by
`tests/automated/check-image-free-colors-callback.py`.

## `image-conversion-cleanup.md`

# Depth-1 conversion cleanup

Depth-1 XYPixmap images may temporarily replace their data with a converted
copy before being uploaded.  The non-cached cleanup path now destroys the
`XImage` first (freeing the converted copy), then frees the saved original
buffer directly.  It no longer dereferences the destroyed image or frees the
wrong allocation.

`tests/automated/check-image-conversion-cleanup.py` protects this ownership
sequence.

## `image-converted-cleanup.md`

# Converted image cleanup ownership

Depth-one `XYPixmap` images may be temporarily converted by replacing their
source data with a buffer allocated by `XtMalloc`. The original source data
still belongs to the image loader, while the replacement belongs to Xt.

The conversion cleanup path now records whether the current image data is the
Xt-owned replacement. It releases that buffer with `XtFree` before invoking
`XDestroyImage`, while leaving untouched loader-owned data for the XImage
destructor. The original buffer is released separately after the wrapper is
destroyed.

The ownership contract is covered by
`tests/automated/check-image-converted-cleanup.py`.

## `image-data-input.md`

# Scaled-pixmap image data input

`_XmGetScaledPixmap` now rejects an `XImage` whose dimensions are positive but
whose pixel-data pointer is `NULL`.  Such an image is malformed for rendering;
previously the depth-one XYPixmap conversion could invert the missing data
before X11 had a chance to report an error.

The rejection happens before scaling, cache insertion, or pixel conversion and
returns `XmUNSPECIFIED_PIXMAP` through the existing failure path.  Valid image
loading and the public pixmap ABI are unchanged.

The source regression is
`tests/automated/check-image-data-input.py`.

## `image-rejection-cleanup.md`

# Rejected image cleanup

When a decoded image has invalid dimensions, an unusable scaling ratio, or a
depth mismatch, `_XmGetScaledPixmap` can reject it before creating a pixmap.
Those paths now destroy non-cached `XImage` objects and release temporary XPM
color allocations.  The same release helper is used when a cached pixmap is
finally disposed, keeping color ownership symmetric.

`tests/automated/check-image-rejection-cleanup.py` protects these paths.

## `image-scale-cleanup.md`

# Scaled image cleanup ownership

The scaler creates an `XImage` with a null data pointer and then installs a
buffer allocated with `XtMalloc`.  Calling the default `XDestroyImage`
destructor directly would release that buffer through Xlib's allocator, which
does not preserve the allocator ownership contract.

Scaled-image teardown now frees the installed data with `XtFree`, clears the
pointer, and only then destroys the XImage structure.  The same helper is used
for allocation failure and normal cleanup paths.

The ownership contract is covered by
`tests/automated/check-image-scaling-destroy.py`.

## `image-scale-row-stride.md`

# Scaled image row-stride validation

`_XmPutScaledImage` accepts an `XImage` supplied by a caller.  Checking the
total `height * bytes_per_line` allocation is not enough: a malformed image
can advertise a row stride smaller than the bytes required by its width and
pixel depth.  Scaling such an image could read past each row while copying
pixels.

The scaler now rejects non-positive `bits_per_pixel` values and verifies that
every row is at least `ceil(width * bits_per_pixel / 8)` bytes before it reads
the image.  For `XYPixmap`, it also accounts for every depth plane when
validating the total source storage.  Valid XImages retain the existing
scaling and XPutImage paths.

The source contract is covered by
`tests/automated/check-image-scale-row-stride.py`.

## `image-scaling-conversions.md`

# Image scaling conversions

Image-cache scaling now keeps table indices unsigned until converting to the
signed coordinates required by X11. The 8-bit fast path explicitly narrows a
pixel to the byte value consumed by `memset`, and cache allocation byte counts
are checked before conversion to Xt's `Cardinal` size.

The focused source contract is checked by
`tests/automated/check-image-scaling-conversions.py`.

## `image-scaling-depth-input.md`

# Image-scaling depth input

`_XmPutScaledImage` now rejects an `XImage` with a non-positive `depth` before
converting it to an unsigned or `size_t` value.  Invalid depth values could
otherwise evade the plane-size checks for small rows and reach destination
image construction with an invalid X11 depth.

The focused source-contract check is:

```sh
python3 tests/automated/check-image-scaling-depth-input.py
```

## `image-scaling-depth1.md`

# Depth-1 image-copy bounds

The depth-1 XYPixmap conversion copied `height * bytes_per_line` through an
`int`, then allocated that result.  Large images could wrap the product, and a
failed allocation could replace a cached image's data pointer with `NULL`.

The conversion now checks the product in `size_t`, allocates into a temporary
pointer, and updates the image only after success.  Lock and non-cached cleanup
remain balanced on rejection.  `tests/automated/check-image-scaling-depth1.py`
protects these invariants.

## `image-scaling-destination-bounds.md`

# Scaled-image destination bounds

`_XmPutScaledImage` now rejects a destination whose X origin plus width would
exceed the signed X11 coordinate range. The Y path already applied the same
protection before constructing strips; the X check prevents a large or
malformed request from wrapping when handed to Xlib. Normal coordinates and
negative origins that remain within the signed range are unchanged.

Regression coverage: `tests/automated/check-image-scaling-destination-bounds.py`.

## `image-scaling-format-input.md`

# Image-scaling format input

`_XmPutScaledImage` now accepts only the three X11-defined `XImage` formats:
`XYBitmap`, `XYPixmap`, and `ZPixmap`.  Unknown format values are rejected
before the scaler selects its byte-copy or `XGetPixel` path, keeping malformed
images from reaching format-dependent Xlib operations.

The focused source-contract check is:

```sh
python3 tests/automated/check-image-scaling-format-input.py
```

## `image-scaling-row-stride.md`

# Image scaling row-stride rounding

The scaled-image path derives a temporary strip height by dividing a fixed
budget by the scaled source row stride.  For a very small positive scale,
floating-point rounding can produce a zero `scaled_row_bytes` even though the
rounding helper itself succeeded.  The scaler now rejects that result before
the division, preventing a divide-by-zero fault while preserving all normal
scales.

The focused source contract is checked by
`tests/automated/check-image-scaling-row-stride.py`.

## `image-scaling-source-origin-y.md`

# Cropped image scaling Y origin

The scaled-image path now normalizes Y coordinates relative to `src_y`, in
parallel with its X handling. A nonzero vertical crop previously used the
full-image scaled Y coordinate when deciding whether to draw, which could
skip the crop entirely. The normalized bottom edge is bounded before drawing;
zero-origin calls retain their existing behavior.

Regression coverage: `tests/automated/check-image-scaling-source-origin-y.py`.

## `image-scaling-source-origin.md`

# Cropped image scaling source origin

`_XmPutScaledImage` now normalizes scaled X coordinates relative to `src_x`,
just as its Y path normalizes coordinates relative to `src_y`. Previously a
nonzero source X origin left the output positioned using full-image coordinates,
which could place pixels outside the destination image. The normalized right
edge is checked against `dest_width` before any pixels are written. Calls with
the usual zero origin retain identical coordinates.

Regression coverage: `tests/automated/check-image-scaling-source-origin.py`.

## `image-scaling-x11-inputs.md`

# Scaled-image X11 input validation

`_XmPutScaledImage` now returns before entering Xlib when its display, target
drawable, or graphics context is missing. Existing image, dimension, and
allocation checks remain unchanged; valid scaling calls keep their behavior.

`tests/automated/check-image-scaling-x11-inputs.py` protects this contract.

## `image-screen-fallback.md`

# Scaled-pixmap screen fallback

`_XmGetScaledPixmap` accepts either a `Screen` or a widget from which to
resolve one.  It previously called `DisplayOfScreen(screen)` before applying
that fallback, so a null screen could crash even when a widget was supplied;
a null screen and null widget also reached the dereference.

The entry point now resolves only when a widget is present, rejects an
unresolvable request, and obtains the display afterward.  The compatibility
entry points and normal screen path are unchanged.

`tests/automated/check-image-screen-fallback.py` protects this contract.

## `info-bar-redisplay-guard.md`

# InfoBar redisplay guards

`XmInfoBar` now treats a missing drawing GC or an unavailable X11 font as a
non-rendering state and returns safely from `Redisplay`. X11 can report either
allocation failure, and neither result is safe to dereference. The normal
rendering path and callback behavior are unchanged.

The font metrics are obtained from the GC with `XGetGCValues(..., GCFont,
...)` before calling `XQueryFont`. A GC context ID is not a font ID; using
`XGContextFromGC` here caused `BadFont` on servers with independent X11
resource namespaces.

The focused source contract is checked by
`tests/automated/check-info-bar-redisplay-guard.py`.

The public dismiss and action callbacks also reject a null widget before the
Xt subclass check, so optional callback handles fail safely.

## `infobar-close-hit-rectangle.md`

# InfoBar close hit rectangle

The InfoBar close action now uses the same centered 18×18 rectangle as the
drawn close control. Clicks above or below the icon no longer dismiss the bar,
while keyboard Escape and the existing action callback remain unchanged.

## `infobar-query-geometry.md`

# InfoBar geometry negotiation

`XmInfoBar` now reports its current width and height through the normal Xt
geometry-query contract.  A parent layout manager therefore sees application
sizes such as `XmNwidth` and `XmNheight`, rather than the widget's original
360x34 defaults after those resources have been changed.

`tests/automated/check-info-bar.c` exercises an explicit 360x40 InfoBar through
`XtQueryGeometry`; `tests/automated/check-infobar-query-geometry.py` preserves
the source-level query contract.

## `inverse-compound-text-converter-inputs.md`

# Inverse compound-text converter inputs

The inverse compound-text conversion path now rejects missing `XrmValue`
records before initializing output storage. This covers the public
`XmCvtXmStringToText` converter and the shared text/UTF-8 helpers used by
internal wrappers. Valid XmString conversions retain their existing output and
allocation behavior.

Regression coverage: `tests/automated/check-inverse-compound-text-inputs.py`.

## `jpeg-error-cleanup.md`

# JPEG decoder error cleanup

The libjpeg error callback uses `longjmp`, including for failures after output
storage has been allocated.  The error branch now frees and clears
`image_data` after destroying the decompressor, so malformed or resource
exhaustion failures do not leak decoded pixels.

`tests/automated/check-jpeg-error-cleanup.py` protects this ownership rule.

## `jpeg-pixel-layout.md`

# JPEG XImage pixel layout

JPEG conversion used fixed two-byte and four-byte writes based only on root
visual depth.  That overran depth-8 images, mishandled valid 24-bit XImages,
and ignored row padding.

The writer now derives the pixel width and byte order from the created
`XImage`, addresses each row through `bytes_per_line`, and rejects unsupported
pixel widths cleanly.  The existing visual-mask conversion remains intact.
`tests/automated/check-jpeg-pixel-layout.py` protects this contract.

## `label-metric-null-guards.md`

# Safe label metrics for ProgressBar and ToggleSwitch

`XmProgressBar`, `XmSpinner`, and `XmToggleSwitch` accept a `NULL` label when an application
clears the label resource.  Their redisplay and geometry paths now measure the
label only when both a render font and an `XmString` are available.  A cleared
label therefore renders as an unlabeled control and reports geometry based on
the control itself, without passing a null string to `XmStringExtent`.

This preserves the existing resource behavior while making label clearing safe
for applications that update resources dynamically.

## `label-mnemonic-charset-width.md`

# Label mnemonic charset width

The Label `XmNmnemonicCharSet` get-values hook only needs to distinguish an
empty string from a non-empty one.  It now checks the first character directly
instead of narrowing `strlen()` from `size_t` to `int`, so long valid charset
names remain exportable.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-label-mnemonic-charset-width.py
```

## `label-wcs-bounds.md`

# Label wide-string conversion bounds

The wide-character `XmLabel` value path converts a `wchar_t` value with
`wcstombs` before creating the localized compound string. The conversion
needs room for the terminating NUL even when the source is empty. The helper
now allocates `MB_CUR_MAX * length + 1` bytes and rejects multiplication
overflow and values that exceed Xt's `Cardinal` allocation interface before
allocating, preserving the existing label behavior for valid values while
preventing an out-of-bounds terminator write or wrapped allocation.

`tests/automated/check-label-wcs-bounds.py` guards the allocation contract.

## `label-wide-length-width.md`

# Label wide-string length width

The Label wide-character resource conversion now scans the input with
`size_t`.  This keeps length tracking and the subsequent multibyte allocation
in the same wide range, avoiding an `int` counter wrap for unusually large
wide strings.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-label-wide-length-width.py
```

## `label-widget-destroy.md`

# Badge, Link, and Prompt label destruction

Badge, Link, and Prompt copy their `XmString` label/text resources so that
resource replacement is transactional. Their destructors now release those
owned copies, preventing leaks across repeated widget creation and destruction.

## `latin-lower-bounds.md`

# Bounded Latin-1 resource conversion

`XmCopyISOLatin1Lowered` remains available for callers that own an adequately
sized destination. Resource converters that lower arbitrary input into fixed
`BUFSIZ` or action buffers now use the bounded internal variant
`XmCopyISOLatin1LoweredN`, which always terminates the destination and safely
truncates unrepresentable input.

The focused source check is
`tests/automated/check-latin-lower-bounds.py`.

## `layout-manager-decision.md`

# WP-12 layout-manager decision

WP-12 is an opt-in escape hatch, not a reason to replace `XmForm`.  The
existing managers were reviewed against the only measured layout corpus
available in this repository:

| Existing manager | Strength | Boundary relevant to this roadmap |
| --- | --- | --- |
| `XmForm` | Expresses anchored and proportional relationships; preserves established resource contracts | Attachment graphs are verbose, but WP-10/WP-11 found no reproducible redundant pass or correctness defect to fix |
| `XmRowColumn` | Simple one-dimensional stacking and menu/list composition | Does not express two-dimensional tracks or flexible cross-axis allocation |
| `XmPanedWindow` | User-adjustable split panes | Its sash model is not a general-purpose grid or flex contract |
| `XmScrolledWindow` | Viewport and scroll-bar ownership | It is a container/viewport, not a layout policy for arbitrary children |
| `XmBulletinBoard` / `XmManager` | Base geometry and child lifecycle behavior | Applications must provide positions or a specialized manager |

The WP-10 corpus demonstrates that nested Form layouts, dynamic child
lifecycle, callback mutation and resize storms complete deterministically on
the qualified AArch64 host.  WP-11 found no source-backed Form defect.  No
real client corpus in this checkout demonstrates a recurring layout need that
cannot be expressed by composing `XmForm`, `XmRowColumn`, and existing
containers.  A new grid/flex manager would therefore add a public class,
resource vocabulary, accessibility semantics, focus ordering, and another
geometry-negotiation implementation without a demonstrated consumer problem.

## Decision

**No-go for implementation in the current modernization tranche.**  Do not
add a speculative layout manager or a second declarative layout API.  Existing
Form clients remain unchanged, and the current managers continue to be the
supported composition tools.

Re-open WP-12 only when a real installed consumer supplies a reproducible
layout corpus showing a recurring requirement that cannot be represented by
those managers.  That corpus must include internationalized direction,
keyboard focus order, accessibility semantics, child destruction/reentrancy,
and Xt geometry negotiation before a new class is designed.  At that point the
design must begin with a small public contract and an independent installed
fixture; it must not grow out of private Form internals.

This is a product-scope decision, not a claim that Motif layout APIs are
pleasant or that every application layout is easy.  It records why adding
another manager now would be unjustified by the available evidence.

## `layout-requirements-corpus.md`

# WP-29 modern-layout requirements corpus

WP-29 asks whether an additive layout family is justified by installed-client
evidence.  The corpus is deliberately independent of any private application
and reuses the qualified WP-10 consumer fixture:

```sh
MOTIF_TEST_PREFIX=/path/to/prefix \
  python3 tests/automated/run-form-layout-corpus.py /path/to/prefix results
```

The fixture covers nested `XmForm` and `XmScrolledWindow` composition,
`XmRowColumn`, resize storms, map/unmap, post-realization insertion and
removal, manage/unmanage, callback-triggered mutation, destruction, and the
same-value negative control.  The WP-11 decision recorded two identical runs:
the result hashes and geometry/redraw/event metrics matched, and the no-op
control produced zero additional geometry requests or Configure events.

## Requirements matrix

| Requirement | Existing evidence | Candidate contract implication |
| --- | --- | --- |
| Main-axis stacking and gaps | `XmRowColumn` composition is deterministic | Stack would need direction, gap, padding, alignment, visibility, RTL, focus order, and Xt negotiation resources |
| Cross-axis grow/shrink and wrapping | No installed consumer demonstrates a missing requirement | Flex/Flow would need bounded basis/grow/shrink, wrapping, rounding, and a documented non-CSS subset |
| Explicit tracks and dense forms | `XmForm` and nested managers cover the qualified corpus | Grid would need track definitions, constraints, mutation, focus, accessibility, and a second geometry contract |
| User-adjustable split panes | `XmPanedWindow` already owns this policy | No candidate should duplicate sash semantics |
| Accessibility and reentrancy | WP-10 covers callback mutation and lifecycle; no new manager need is shown | Any future manager must expose stable child semantics and survive destruction during callbacks |
| RTL and keyboard traversal | Existing corpus and control fixtures provide baseline coverage | A new manager must specify logical direction and traversal before implementation |

## Independent decisions

* **Stack: no-go now.** The available corpus does not show a recurring stack
  requirement that `XmRowColumn` composition cannot express safely.
* **Flex/Flow: no-go now.** There is no measured client need for grow/shrink or
  wrapping, and adding a CSS-like vocabulary without a consumer would create
  an unsupported compatibility promise.
* **Grid: no-go now.** The corpus does not demonstrate a dense-track layout
  that cannot be composed from `XmForm`, `XmRowColumn`, and existing
  containers; a second negotiation contract would be speculative.

These are scope decisions, not claims that Motif layout APIs are pleasant or
that every application layout is easy.  They preserve existing geometry
negotiation and avoid adding three public managers without a reproducible
consumer requirement.

## Reopen criteria

Reopen an individual candidate only when an installed consumer supplies a
reproducible layout corpus showing a requirement that existing managers cannot
represent.  The corpus must include final geometry, resize storms, child
mutation, destruction/reentrancy, RTL direction, keyboard focus order,
accessibility semantics, Xt geometry negotiation, and comparative metrics
against the current managers.  A new package must begin with the smallest
public contract and an independent installed fixture; it must not grow from
private `XmForm` records or browser/CSS compatibility assumptions.

## `legacy-charset-canonicalization.md`

# Legacy charset canonicalization bounds

The compatibility charset canonicalizer copies arbitrary charset names into an
owned string.  Its length now remains `size_t` and is checked against Xt's
allocation width before adding the terminator.  Oversized names fail cleanly
instead of truncating an `int` length.

The focused source contract is checked by
`tests/automated/check-legacy-charset-canonicalization.py`.

## `legacy-converter-abi.md`

# Legacy resource converter ABI and output sizing

The IconButton, ButtonBox, and Column resource converters now use the full
`XtTypeConverter` callback signature and are registered without incompatible
function-pointer casts. Existing resource spellings, values, and cache modes
are unchanged.

The ButtonBox fill-option converter also now checks the caller's output buffer
before replacing `to->size` with the required size. An undersized destination
is reported through the standard Xt size-negotiation result instead of being
written through.

The focused regression check is
`tests/automated/check-legacy-converter-abi.py`.

## `legacy-filesel-restore-bounds.md`

# Legacy file-selection restore bounds

The compatibility file-selection restore action concatenates the directory and
pattern strings into the filter field.  Lengths now remain `size_t` and the
combined size is checked before the allocation is narrowed to Xt's type.
Unrepresentable values are rejected with the existing cleanup path.

The focused source contract is checked by
`tests/automated/check-legacy-filesel-restore-bounds.py`.

## `legacy-menu-accelerator-bounds.md`

# Legacy menu-accelerator copy bounds

The compatibility row-column getter returns an owned copy of the menu
accelerator.  It now checks the string length before allocating terminator
space and narrowing to Xt's allocation type; an unrepresentable value returns
null without wrapping the allocation.

The focused source contract is checked by
`tests/automated/check-legacy-menu-accelerator-bounds.py`.

## `list-add-inputs.md`

# List add input validation

The List add APIs now reject null and non-List widgets, as well as null single-item strings, before
entering the application context. Shared insertion also rejects non-positive
counts and null elements in an item array, preventing invalid calls from
reaching list mutation or string-copy code.

`tests/automated/check-list-add-inputs.py` protects this contract.

## `list-add-items-height-state.md`

# Preserve list height state while adding items

`APIAddItems` passes the previous maximum item height to `SetNewSize` so a
list can refresh its visible-item geometry when an insertion changes the
height extent.  The local variable was declared without an initializer,
which left the comparison undefined on the normal successful insertion path.

The value is now captured from `MaxItemHeight` before the insertion.  This
preserves the existing resize behavior while making the state comparison
deterministic on all supported architectures.

## `list-api-inputs.md`

# List selection and geometry API inputs

The public List helpers `XmListGetSelectedPos`, `XmListSetHorizPos`,
`XmListYToPos`, `XmListUpdateSelectedList`, and `XmListPosSelected` now reject
non-List widget handles before context lookup or private-state access.  Their
existing null, range, output, and callback behavior for valid List widgets is
unchanged.

The same class validation now covers add-mode, item-existence/position,
keyboard-position, and match-position helpers.

The focused source regression is
`tests/automated/check-list-api-inputs.py`.

## `list-callback-length-bounds.md`

# List and ComboBox callback length bounds

`XmListCallbackStruct.item_length` is part of the public Motif ABI and is an
`int`, while `XmStringLength()` returns an unsigned length.  List and ComboBox
now clamp oversized compound-string lengths to `INT_MAX` before populating the
callback record or the List's internal cache, preventing signed wraparound
without changing the ABI.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-list-callback-length-bounds.py
```

## `list-delete-all-input.md`

# List delete-all input validation

`XmListDeleteAllItems` now rejects a null widget before application-context
access or list-state reads. Valid list clearing behavior is unchanged.

`tests/automated/check-list-delete-all-input.py` protects this contract.

## `list-delete-copy-allocation.md`

# List deletion copy allocation

`XmListDeleteItems` now checks the temporary `XmString` copy-array size for
overflow and handles allocation failure before copying or deleting items. The
application lock is released on both failure paths, preserving normal list
deletion behavior while avoiding a null dereference.

`tests/automated/check-list-delete-copy-allocation.py` protects this contract.

## `list-delete-inputs.md`

# List deletion input validation

`XmListDeleteItems` now rejects a null widget, null item array, or non-positive
item count before reading widget state or entering the application context.
Valid deletion calls retain their existing behavior.

`tests/automated/check-list-delete-inputs.py` protects this contract.

## `list-delete-item-inputs.md`

# List item deletion input validation

`XmListDeleteItem` now rejects a null widget or null `XmString` before entering
the application context or searching the list. Valid item deletion remains
unchanged.

`tests/automated/check-list-delete-item-inputs.py` protects this contract.

## `list-drag-selection-allocation.md`

# XmList drag selection allocation

The drag-and-drop conversion path now validates both the host `size_t` limit
and Xt's `Cardinal` allocation limit before copying selected strings.  This
keeps large-list handling from truncating the requested byte count on 64-bit
hosts while preserving the existing failure behavior.

The focused source-contract check is:

```sh
python3 tests/automated/check-list-drag-selection-allocation.py
```

## `list-growth-allocation.md`

# List allocation failure handling

List insertion now treats the internal item-array resize as a transaction. It
checks integer, `size_t`, and Xt `Cardinal` bounds, keeps the old array when
`XtRealloc` fails, and stops the insertion before internal rows or selection
state are updated. Deletion shrinking applies the same Xt-width check.
This keeps the widget coherent under memory pressure and preserves the public
`XmListAdd*` API behavior.

`XmListGetMatchPos` also keeps its original result buffer when shrinking the
temporary match array cannot be satisfied. The caller still receives every
match, with no leaked buffer or invalid null result.

## `list-item-navigation-inputs.md`

# Item-based list navigation input validation

`XmListSetItem` and `XmListSetBottomItem` now reject null widgets or item
strings before toolkit access or list lookup. Valid navigation behavior is
unchanged.

`tests/automated/check-list-item-navigation-inputs.py` protects this contract.

## `list-navigation-inputs.md`

# List selection and navigation input validation

`XmListDeselectAllItems`, `XmListSetPos`, and `XmListSetBottomPos` now reject
null widgets before application-context access or list-state reads. Valid
selection and navigation behavior remains unchanged.

`tests/automated/check-list-navigation-inputs.py` protects this contract.

## `list-position-delete-inputs.md`

# Position-based list deletion input validation

Position-based deletion now rejects null widgets before application-context
access. The shared position helper also rejects null arrays and non-positive
counts, avoiding invalid iteration while preserving valid deletion semantics.

`tests/automated/check-list-position-delete-inputs.py` protects this contract.

## `list-query-inputs.md`

# List query and horizontal-position input validation

`XmListSetHorizPos` now rejects a null widget before application-context
access. `XmListPosSelected` also rejects empty-list and invalid last-item
states before resolving the special position zero to an internal index.

`tests/automated/check-list-query-inputs.py` protects this contract.

## `list-replace-inputs.md`

# List replacement input validation

List replacement APIs now reject null widgets before toolkit access. Shared
replacement paths also reject non-positive counts and null old or new
`XmString` elements before comparing or replacing entries.

`tests/automated/check-list-replace-inputs.py` protects this contract.

## `list-replace-positions-inputs.md`

# Position-based list replacement input validation

`XmListReplacePositions` now rejects null widgets, position or item arrays,
non-positive counts, and null replacement strings before entering the
application context or indexing list state.

`tests/automated/check-list-replace-positions-inputs.py` protects this
contract.

## `list-scroll-arithmetic.md`

# List scrollbar arithmetic

The List widget now computes scroll candidates in `int64_t` before applying
the scrollbar minimum and maximum bounds.  This prevents signed `int`
overflow when an application supplies extreme scrollbar values or increments,
while preserving the normal one-increment scrolling behavior.

The focused source-contract check is:

```sh
python3 tests/automated/check-list-scroll-arithmetic.py
```

## `list-selection-inputs.md`

# List selection input validation

Item selection and deselection now reject null widgets before toolkit access,
and item-based operations reject null `XmString` values before list lookup.
Position selection behavior remains unchanged.

`tests/automated/check-list-selection-inputs.py` protects this contract.

## `list-sensitivity-cancellation.md`

# List sensitivity cancellation

A native List cancels a pending selection gesture when it or an ancestor loses
effective sensitivity. A later release after re-enabling requires a new press;
it cannot complete the cancelled gesture. Pending drag initiation and automatic
scroll timers are removed, and append-selection state is cleared. Existing
selection data remains intact.

`run-list-sensitivity.py` exercises single, multiple, browse and extended
selection with direct and ancestor sensitivity changes in classic, modern light
and modern dark appearances. It verifies that cancellation suppresses the old
release while a subsequent fresh gesture still notifies normally.

## `list-translation-size.md`

# List translation allocation size

List class initialization concatenates two translation tables before parsing
them.  The combined size is now computed in `size_t` with an overflow check
before stack or heap allocation, keeping the initialization path safe if the
translation sources grow.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-list-translation-size.py
```

## `list-visibility-sum-bounds.md`

# List visibility-sum bounds

List visibility checks compare the top row plus the visible-row count in
`long long` arithmetic.  This prevents signed `int` wraparound from making a
row appear visible after extreme geometry or resource values.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-list-visibility-sum-bounds.py
```

## `mainwindow-live-layout.md`

# MainWindow live layout

Changing a realized `XmMainWindow` margin or separator resource can require a
new child layout without changing the outer widget size. Xt does not call a
widget resize method for that unchanged-size case. MainWindow now explicitly
relayouts its children there, while retaining the existing geometry path when
the requested outer size changes.

The focused regression is
`tests/automated/check-mainwindow-width-query-bounds.py`.

## `mask-name-allocation.md`

# Mask-name allocation

`XmeGetMask` previously generated the companion mask name in a fixed 256-byte
buffer even though image names are caller-provided.  Long paths could overflow
the stack before the mask lookup; null screens and names also reached Xlib or
string handling.

The call site now rejects missing inputs, allocates enough space for the
original name plus the `_m` suffix, verifies that the size fits Xt's
`Cardinal` allocation interface, and releases the temporary name after the
lookup. Existing mask naming remains unchanged.

`tests/automated/check-mask-name-allocation.py` protects this contract.

## `menu-stream-bounds.md`

# Mwm menu command stream bounds

`ParseClientCommand` joins a command fragment and its continuation with one
space before parsing. Its allocation now reserves both that separator and the
terminating NUL, and rejects arithmetic that cannot fit in the Xt allocation
type. Existing menu syntax and parsing behavior are unchanged for
representable input.

`tests/automated/check-menu-stream-bounds.py` guards the allocation contract.

## `menustate-allocation.md`

# MenuState allocation

Menu-state creation now returns `NULL` before publishing or initializing a
failed state record. Drag-mode updates also tolerate that failure, preventing
menu interaction from dereferencing a null state object.

`tests/automated/check-menustate-allocation.py` protects this contract.

## `modern-gallery-screenshot-qualification.md`

# Gallery screenshot qualification

`tests/automated/run-gallery-screenshots.py` is the display-backed capture
gate for the component gallery. It is intentionally separate from the native
interaction test: a component must both remain interactive and produce a
valid image through the X11 capture path.

For every manifest entry, the runner starts the verified gallery binary with
`--component ID --screenshot-mode --appearance PROFILE --scale SCALE`, locates the mapped `ckmotif-gallery`
top-level window with `xwininfo`, captures that window with `xwd`, and converts
the native capture to PNG with `xwdtopnm` and `pnmtopng`. It selects the
largest mapped window with that exact title and ignores the 1x1 withdrawn
placeholder that some window managers expose during shell startup.

`PROFILE` is one of `classic`, `modern-light`, `high-contrast` or
`modern-dark`. `SCALE` is a finite positive display-scale override no greater
than 64. The gallery selects the appearance and scale before creating its child
widgets. The readiness property is published only after the initial event
queue settles; the runner checks that it belongs to the launched process and
that the gallery reports the requested manifest component ID. A capture cannot
therefore pass merely because the welcome page mapped successfully. The runner records both values in
`receipt.json`, so captures at different settings cannot be confused with
the default profile.

The runner terminates each child and drains its diagnostic pipe before moving
to the next entry. If a component exits before its window appears, the
receipt includes its exit status and stderr; this keeps a toolkit failure
visible instead of misreporting it as a missing display.

Each output directory contains a `receipt.json` with the manifest and binary
and actual loaded-library hashes, capture-tool paths, per-entry XWD/PNG hashes, elapsed time, and an
explicit result:

- `pass`: every manifest scenario produced both captures;
- `skip`: the binary, display, or capture tools were unavailable;
- `fail`: a scenario or conversion failed, with `reason` and
  `current_entry` identifying the boundary.

On Linux, a qualification run can use a task-owned display and output path:

```sh
DISPLAY=:121 python3 tests/automated/run-gallery-screenshots.py \
  --binary /absolute/path/to/ckmotif-gallery \
  --out /tmp/ckmotif-gallery-screenshots \
  --appearance modern-dark --scale 2.0
```

On macOS, output must be below `/Volumes/S8T/tmp` or
`/Volumes/PRO-BLADE/tmp` in accordance with the repository temporary-workspace
policy. The generated PNGs are publication inputs only after the receipt has
`result: "pass"` and its binary hash has been recorded with the documentation
build evidence.

The source-level safety contract is checked by
`tests/automated/check-gallery-screenshot-runner.py`.

The complete profile/scale capture is orchestrated by
`tests/automated/run-gallery-screenshot-matrix.py`. Its default matrix is the
four public appearance profiles crossed with scales 1.0, 1.5, 2.0 and 3.0;
`--appearances` and `--scales` accept explicit comma-separated subsets. Each
cell is a separate child receipt and the matrix fails or skips as soon as a
child does, while retaining the completed cells and their exact commands.
By default the XWD/PNG captures are retained. On constrained qualification
hosts, `--discard-captures` removes only those generated image files after
their hashes are recorded in the child receipt; the matrix receipt records
that the run was hash-only rather than presenting it as publication imagery.
The source-level matrix contract is checked by
`tests/automated/check-gallery-screenshot-matrix.py`.

For example, a full Linux qualification uses a fresh output directory:

```sh
DISPLAY=:121 python3 tests/automated/run-gallery-screenshot-matrix.py \
  --binary /absolute/path/to/ckmotif-gallery \
  --out /tmp/ckmotif-gallery-screenshot-matrix
```

The checked-in publication set uses the current `modern-light` profile at
1.0x scale with SVG dialog artwork enabled. The profile and scale are
selected explicitly at startup; the binary hash in the publication manifest must match the gallery
binary used to generate it.

Published PNGs are projected into `docs/gallery-screenshots/manifest.json` by
`tests/automated/generate-gallery-screenshot-manifest.py`. The companion
`tests/automated/check-gallery-screenshot-assets.py` verifies the manifest
hash, binary fingerprint, profile/DPI capture labels, PNG dimensions, file
hashes, and catalogue links. This prevents a locally replaced image from
silently becoming gallery evidence.

## `modern-macos-ui-audit.md`

# Modern appearance audit on macOS

The gallery's `--audit-modern DIRECTORY` mode visits every manifest entry in
modern light and modern dark, inventories render-table font types and visible
label extents, and records normal, focused, disabled and supported popup states.
The directory must already exist. Captures come from the production gallery's
actual X windows; each standalone popup has its own capture. Service classes and
optional components remain explicitly identified in the manifest.

Use an installed native build with Xft, Unicode text and SVG support, including
`--enable-demos`. Keep a single gallery instance on the user desktop. Automated
interaction fixtures run on a separate Xvfb display and do not send desktop input.
A build receipt must identify source/assets, executable and loaded library hashes;
an isolated fixture does not replace inspection of the running XQuartz gallery.

## Control behavior

- DataField accepts Xft renditions in UTF-8 locales instead of rejecting them for
  lacking an XFontSet. Editing, selection and character conversion continue to
  use the existing Unicode path.
- Compound-string drawing honors disjoint clip rectangles as well as partially
  intersecting ones, so separate contrast passes cannot repaint each other. See
  [Compound-string clip rectangles](xmstring-clip-regions.md).
- ProgressBar exposes `XmNrenderTable`, copies its supplied table, measures its
  preferred size and percentage text, and uses a rounded track in modern profiles.
  Text over the filled region chooses black or white from the actual fill color's
  luminance; the remaining text uses the ordinary foreground. Existing widget
  record layouts are unchanged.
- DatePicker and TimePicker use measured preferred sizes, flat field frames and
  visible disclosure/spinner controls. TimePicker's spinner changes its selected
  hours, minutes or seconds field; keyboard editing remains available. The focused
  field is underlined. DatePicker opens a full-sized calendar aligned below the
  owner, flipping above it when space requires, and clamps to the screen.
- Calendar sizes heading, weekdays and day cells from font metrics and scale.
  Modern selected dates use an accent surface with readable text. Header chevrons
  navigate months using the same range checks as keyboard navigation.
- ProgressRing and Spinner use the profile accent for theme-owned normal roles.
  Explicit foreground colors and semantic appearance roles retain precedence.
- Links use the profile's link color. InfoBar supplies severity-specific surfaces
  and text for normal-role defaults; explicit application colors still win. Toast
  has a flat frame. IconGadget borders use the modern flat renderer.
- Tab labels restore their own drawing color after disabled painting. No static
  saved color is shared between tab widgets or displays.
- Empty FormSection footers stay hidden and reserve no footer space.
- Automatic scrollbar drag scrolling registers zero data targets. It retains
  enter/leave timer behavior without treating a native pointer as an X Atom.
  Navigator notifications use a watched snapshot, so a callback can remove a
  navigator or destroy its frame without visiting freed list entries. ScrollBar
  dispatches a public callback snapshot and stops after widget destruction,
  including immediate destruction from timers; see
  [callback ownership](scrollbar-callback-snapshot.md).
- Gallery feedback starts with measured content and spans its available row;
  switching away from passive examples cannot leave a one-pixel label.
- ColorSelector accepts the valid Xt constructor form with no argument list;
  a null list is rejected only when its count is nonzero.

Classic geometry and painting remain available through the classic profile.
Application-specified dimensions remain authoritative; use preferred geometry
rather than a fixed legacy height for controls with modern font metrics.

The [macOS validation record](validation/macos-modern-ui-audit-20261102.json)
records native test groups, source/output fingerprints and actual-window findings.

## Acceptance

`check-modern-audit-controls.c` covers light/dark at 1× and 2×, UTF-8 Xft
DataField text, preferred field/calendar sizing, month navigation, time spinner
interaction and date popup open/close. `check-tab-color-state.c` reproduces the
previous disabled-to-normal tab contrast failure. Existing button, field, menu,
container, avatar, font-selector and profile regressions supplement these checks.

Font inventory and clipping checks are necessary but cannot judge layout or
appearance alone. Review the normal/disabled/focus captures and popup captures,
then inspect the exact verified process on XQuartz. Native clipboard, input-method
and accessibility qualification are separate from this visual audit.

Run the native suite with an installed prefix and its matching build tree:

```sh
python3 tests/automated/run-modern-macos-audit.py \
  --prefix /Volumes/S8T/tmp/your-build/install \
  --build /Volumes/S8T/tmp/your-build/build \
  --out /Volumes/S8T/tmp/your-build/native-audit
```

The output directory must be fresh. The runner includes the existing autoscroll
lifetime cases, color-override tests and light/dark/high-contrast UIL role tests.

## `modern-toggle-appearance.md`

# Modern checkbox and radio appearance

This work is being qualified in the development branch.

Modern light and dark share a native indicator renderer for ToggleButton and
ToggleButtonGadget. The default checkbox uses a rounded outline with a check
when selected and a dash when indeterminate. The default radio uses a circular
outline and selected center dot. Explicit diamond and cross indicators retain
those shapes. Classic and high-contrast use their existing renderer.

The renderer uses the control's resource colors, bounds every drawing operation
inside its allocation, and owns its mutable graphics context. Selected glyph
colors are chosen from the actual foreground/background in the control's
colormap. Widget/gadget state machines, callbacks and keyboard activation stay
in their original classes.

Modern defaults remove the outer toggle bevel, use a two-logical-pixel focus
outline and reserve 18/16 logical pixels for comfortable/compact indicators.
Margins provide natural control heights of at least 34/28 logical pixels and
8/6 logical pixels between the indicator and text. Larger fonts can increase
the natural height. Explicit dimensions, indicator variants, spacing and
unselected colors remain application-owned during live profile changes.

Run `tests/automated/run-modern-toggles.py --prefix INSTALL --out FRESH_DIR`
for native state/geometry and keyboard qualification. The fixture checks
widget/gadget checkboxes and radios, selected/mixed/disabled states, callback
counts, natural height, RTL, tiny allocations, Space activation, explicit
resource preservation and 1/1.5/2/3 display scales in both modern palettes and
densities. Actual application captures are additionally required for visual
acceptance; the fixture is not a substitute for them.


Modern checkbox and radio captions use beginning alignment, placing text beside
the indicator in both layout directions even when a parent stretches the
control. Indicator-less toggles retain their original alignment. Changing
`XmNindicatorOn` updates toolkit-owned alignment immediately. Explicit
`XmNalignment` arguments and resource-database choices remain authoritative;
switching back to classic restores the original alignment for owned controls.

## Gadget margin consistency

ToggleButtonGadget uses `marginWidth` for natural width and both horizontal
origins, while `marginHeight` controls vertical layout. RTL label coordinates
remain relative to the gadget, including gadgets positioned away from the
parent origin. The gallery now uses natural widget/gadget dimensions without
fixed width or height overrides.

`run-toggle-gadget-margins.py --build BUILD --out FRESH_DIR` checks asymmetric
margins, margin updates and nonzero positions across classic/light/dark profiles,
three scales and both directions. This supplements the state/pixel fixture.

## `motif-colors-loader-allocation.md`

# Color selector allocation handling

The RGB color loader now keeps `ColorInfo` growth transactional and aborts
cleanly when a realloc or size calculation fails. Building the temporary
XmString list also checks its allocation before filling it. The existing color
selector state is left intact instead of dereferencing a null allocation.

Both allocator byte counts are checked against Xt's `Cardinal` size limit
before conversion. The loader also uses `size_t` for name lengths and caps
accepted entries at the signed-short range used by the legacy `num_colors`
field, so an oversized database cannot wrap the stored count or its sort/list
lengths.

The focused source regression is
`tests/automated/check-colors-loader-allocation.py`.

## `motif-combobox-child-args.md`

# ComboBox child-argument allocation

The optional `FIX_1473` child-argument filter now falls back to the original
argument list if its temporary array cannot be allocated. ComboBox child
creation therefore remains functional under allocation pressure, while the
height-filtering behavior is unchanged when the temporary array is available.

## `motif-combobox-text-trait.md`

# ComboBox textual trait validation

ComboBox edit-box helpers now check both the access-textual trait and the
specific callback they need. A class with a partial textual trait returns no
value or skips the update safely, while complete traits retain the existing
XmString behavior.

## `motif-container-trait-callbacks.md`

# Container trait callback validation

Container item and navigator traits are extension points. A class may expose
a trait record with only part of the callback surface implemented. Container
now checks each callback before invoking it, using the existing fallback view,
width, and selection values when an item callback is absent and skipping a
navigator that cannot be read and updated. This keeps partial third-party
traits from causing null function-pointer calls while preserving the existing
trait ABI.

## `motif-filesb-convert.md`

# FileSB conversion target allocation

When a File Selection Box advertises file and file-name conversion targets,
the temporary atom array is now checked before initialization.  Allocation
failure refuses the conversion cleanly instead of dereferencing null storage.

The regression check is
`tests/automated/check-filesb-convert-allocation.py`.

## `motif-filesb-directory-list-allocation.md`

# FileSB directory-list allocation failure

File Selection Box directory navigation converts matching directory names into
an `XmString` array before updating the directory list.  The array allocation
is now checked before any entries are written.  If it fails, all temporary
directory-name strings are released and the callback returns safely without
passing an invalid array to the list widget.

The regression check is
`tests/automated/check-filesb-directory-list-allocation.py`.

## `motif-filesb-empty-text.md`

# FileSB empty text fallbacks

File Selection Box filter and directory text creation now use a static empty
string when their one-byte fallback allocation fails.  The widget still gets
created, and the cleanup path only frees buffers owned by the allocator.

The regression check is
`tests/automated/check-filesb-empty-text.py`.

## `motif-filesb-list-allocation.md`

# FileSB directory-list allocation failure

File Selection Box directory searches convert each matching filename into an
`XmString` array before updating the file list.  The array allocation is now
checked before any entries are written.  If it fails, the temporary filename
list is released and the existing widget list remains unchanged while the
search callback completes safely.

The regression check is
`tests/automated/check-filesb-list-allocation.py`.

## `motif-fonts-family-allocation.md`

# Font selector family-list allocation

Font selector family refresh now checks the temporary `XmString` array size
against both host `size_t` and Xt `Cardinal` byte limits, and checks the
allocation before filling it, passing the bounded size explicitly as an Xt
`Cardinal`. If the array cannot be created, the refresh
returns with the existing family list intact instead of dereferencing a null
pointer.

## `motif-fonts-list-growth.md`

# Font selector raw-list growth

Font selector XLFD and alternate-font collection now grows its arrays
transactionally. Capacity is updated only after a successful `realloc`, and a
failed string copy is skipped safely. This prevents null-pointer writes while
preserving all successfully collected font entries.

## `motif-fonts-metadata-allocation.md`

# Font selector metadata allocation

Font selector initialization now checks its `FontInfo` and family-table
allocations. Family-table growth is transactional, and initialization returns
cleanly when metadata cannot be built instead of dereferencing failed storage.
The existing widget ABI and normal font discovery behavior are unchanged.

## `motif-fonts-size-list-allocation.md`

# Font selector size-list allocation

Font selector size and alternate-family list refreshes now check their
temporary `XmString` arrays before filling them. Negative or oversized counts
are rejected before multiplication, and the bounded arrays are passed
explicitly as Xt `Cardinal` sizes. Allocation failure returns without updating
the corresponding list, preventing null-pointer writes while preserving normal
behavior for valid lists.

## `motif-iconbutton-stipple.md`

# IconButton grey-stipple cache allocation

`GetGreyStipple` now handles failure to allocate its cache record by returning
`None` before dereferencing it.  The widget can therefore continue without a
stipple instead of crashing during cache initialization.

The regression check is
`tests/automated/check-iconbutton-stipple-allocation.py`.

## `motif-iconfile-cache-copies.md`

# Cached icon path copies

When the icon-file cache is enabled, directory and leaf names are copied into
the cache record.  Each copy is now checked; a failure releases all partial
fields and skips optional caching without affecting the resolved icon path.

The regression check is
`tests/automated/check-iconfile-cache-copies.py`.

## `motif-iconfile-cache-directory.md`

# Icon-file cache directory allocation

Derived icon cache entries now release their copied key and record when the
resolved directory-name buffer cannot be allocated.  The optional cache is
skipped without writing through null storage, while icon resolution continues.

The regression check is
`tests/automated/check-iconfile-dir-allocation.py`.

## `motif-iconfile-cache-entry.md`

# Icon-file cache entry allocation

Icon path resolution treats name caching as optional.  If the cache entry
allocation fails, the resolved icon filename is now returned through the
normal path without dereferencing null cache storage.

The regression check is
`tests/automated/check-iconfile-entry-allocation.py`.

## `motif-iconfile-cache-key.md`

# Icon-file cache key allocation

The optional icon-name cache now checks the copied key string before filling
other cache fields.  If copying the key fails, the temporary cache record is
released and path resolution continues without caching.

The regression check is
`tests/automated/check-iconfile-key-allocation.py`.

## `motif-iconfile-cache-leaf.md`

# Icon-file cache leaf allocation

Derived icon cache entries now release their directory, key, and record if the
leaf-name copy fails.  The optional cache is skipped while the resolved icon
path remains available to the caller.

The regression check is
`tests/automated/check-iconfile-leaf-allocation.py`.

## `motif-iconfile-cache-table.md`

# Icon-file cache-table allocation fallback

Icon path resolution no longer assumes the optional icon-name hash table was
allocated successfully.  Lookup is skipped when the table is unavailable, and
newly built cache entries are released instead of being inserted into null
storage; path resolution continues normally.

The regression check is
`tests/automated/check-iconfile-hash-fallback.py`.

## `motif-icong-container-trait.md`

# IconGadget container-trait validation

IconGadget now checks that its container-parent hook returns a widget and that
the container trait provides `getValues` before using it. Color lookup keeps
its existing fallback colors when the optional trait is absent or partial.
This protects custom container implementations without changing the trait ABI.

## `motif-list-drag-targets.md`

# List drag target allocation failure

List drag conversion builds target arrays for TARGETS and export-target
requests.  Both allocations are now checked before atoms are appended.  If
allocation fails, conversion reports `XmCONVERT_REFUSE` without writing
through null storage.

The regression check is
`tests/automated/check-list-drag-target-allocation.py`.

## `motif-list-elements.md`

# List internal element allocation failure

List item insertion now handles failure while constructing an internal element
record.  Any records already allocated for the insertion are released, the
internal pointer array is restored to its original order, and cached extents
are restored before the operation returns.  This keeps the existing list
usable instead of dereferencing null storage or leaving a partial insertion.

The regression check is
`tests/automated/check-list-element-allocation.py`.

## `motif-list-translations.md`

# List translation allocation failure

List class initialization combines its built-in translation strings in a
temporary local buffer.  The local allocation is now checked before copying.
If it fails, class initialization returns safely instead of dereferencing a
null buffer.

The regression check is
`tests/automated/check-list-translation-allocation.py`.

## `motif-navigator-slider-callbacks.md`

# Navigator slider callback validation

List and ScrolledW slider callbacks now validate the navigator trait and its
`getValue` callback before reading the current position. A partial or missing
trait is ignored safely, preserving normal behavior for complete navigator
implementations and avoiding a null function-pointer call during scrolling.

## `motif-resconvert-token.md`

# Resource converter token allocation

`GetNextToken` now checks both its empty-token and normal-token allocations
before writing into the returned buffer. The normal token span is measured in
`size_t`, rejected if it cannot fit Xt's `Cardinal` range, and passed
explicitly to the allocator. Allocation failure returns `NULL` without
dereferencing invalid storage; callers retain their existing end-of-input
behavior.

The regression check is
`tests/automated/check-resconvert-token-allocation.py`.

## `motif-resencod-iconv.md`

# Compound-text iconv output allocation

`ConvertWithIconv` now checks the initial output allocation and performs buffer
growth through a temporary pointer.  If either allocation fails, conversion
returns `NULL` after releasing the buffer instead of continuing with invalid
storage or losing the existing allocation.

The regression checks are
`tests/automated/check-iconv-output-allocation.py` and
`tests/automated/check-iconv-growth-bounds.py`.

## `motif-resencod-octets.md`

# Compound-text encoding buffer allocation

When a compound-text segment combines a registered encoding prefix with the
current item, the temporary octet buffer is now checked before copying.  An
allocation failure returns a null conversion result instead of dereferencing
invalid storage.

`tests/automated/check-resencod-octet-allocation.py` protects this path.

## `motif-resencod-prefix.md`

# Compound-text custom charset prefix allocation

`processCharsetAndText` constructs an escape prefix for non-standard compound
text character-set tags.  The prefix length is now checked against both the
`unsigned int` output-length contract and `size_t` allocation limits before
allocation.  A failed allocation returns a conversion failure while leaving
the existing output buffer owned by the caller.

The regression check is
`tests/automated/check-resencod-prefix-allocation.py`.

## `motif-resencod-registry.md`

# Segment-encoding registry allocation

Segment-encoding registration now allocates replacement strings before changing
an existing entry, and rolls back partial allocations for new entries.  Registry
export checks its output allocation before writing and resets the reported
length on failure.

`tests/automated/check-resencod-registry-allocation.py` protects these paths.

## `motif-resencod-segments.md`

# Extended compound-text segment buffers

Both compound-text extended-segment decoders now check temporary charset and
text buffer allocations.  A failed text allocation releases the charset copy
and returns a conversion error before either buffer is written.

`tests/automated/check-resencod-segment-allocation.py` protects these paths.

## `motif-scrollframe-add-input.md`

# ScrollFrame navigator addition input validation

`_XmSFAddNavigator` now rejects a null frame, navigator, or scroll-frame state
before trait lookup and callback registration.  This keeps navigator setup
safe during partial initialization and teardown.

The regression check is
`tests/automated/check-scrollframe-add-input.py`.

## `motif-scrollframe-callbacks.md`

# ScrollFrame navigator callback validation

`_XmSFAddNavigator` now validates the navigator trait callbacks before it
registers a navigator. A trait with no `setValue` callback cannot be
initialized, and a configured move callback cannot be installed when the
navigator does not provide `changeMoveCB`. In either case the function emits
the existing warning and leaves the navigator list unchanged.

This preserves the public Motif API while preventing a malformed or partial
trait implementation from causing a null function-pointer call during widget
creation.

## `motif-scrollframe-nav-growth.md`

# ScrollFrame navigator-list growth

ScrollFrame navigator registration grows its widget list in two-slot steps.
The reallocation is now transactional: the slot count and pointer are updated
only after `XtRealloc` succeeds.  If growth fails, registration returns while
the existing navigator list remains valid.

The regression check is
`tests/automated/check-scrollframe-nav-growth.py`.

## `motif-scrollframe-remove-input.md`

# ScrollFrame navigator removal input validation

`_XmSFRemoveNavigator` now rejects a null navigator or scroll-frame state
before querying the navigator trait or changing callbacks.  This keeps the
convenience API from dereferencing invalid pointers during teardown and
partial initialization.

The regression check is
`tests/automated/check-scrollframe-remove-input.py`.

## `motif-scrollframe-update-guards.md`

# ScrollFrame navigator update guards

`_XmSFUpdateNavigatorsValue` now checks its scroll-frame and navigator traits,
the returned navigator list, and each navigator entry before invoking callback
methods.  Partially initialized or tearing-down widgets are skipped safely.

The regression check is
`tests/automated/check-scrollframe-update-guards.py`.

## `motif-spinb-text-trait.md`

# SpinBox textual trait validation

SpinBox now validates the child widget before looking up its access-textual
trait and checks the specific `setValue` or `getValue` callback before use.
Partial traits therefore leave the child unchanged or report the existing
current-value fallback instead of causing a null function-pointer call.

## `motif-text-source-progress.md`

# Text source forward-progress validation

Text source reads can fail by returning the current position without a block.
String extraction now clears the block length before each read and requires a
strictly advancing position in both byte and wide-character paths. A failed
or non-progressing source read therefore returns the partial result safely
instead of reusing stale block data or looping forever.

## `motif-text-textual-trait.md`

# Text textual trait validation

The public Text string helpers now check the access-textual trait callback
before invoking it for TextField widgets. A partial trait safely leaves a get
result empty or skips a set operation; normal Text and complete TextField
traits retain their existing behavior and ABI.

## `motif-textdest-location-lifetime.md`

# Text drop location lifetime

Text drops can continue through an asynchronous transfer callback.  The drop
location is now stored in heap memory owned by the transfer instead of pointing
at a stack-local `XPoint`.  The transfer done callback releases it for both
primary and Motif-drop transfers, and allocation failure rejects the transfer
cleanly.

The regression check is
`tests/automated/check-textdest-location-lifetime.py`.

## `motif-textdrop-record.md`

# Text drop transfer-record allocation failure

Text drop handling stores asynchronous transfer state in a temporary record.
The allocation is now checked before the record is initialized.  If it fails,
the transfer is marked failed and the callback returns without dereferencing
null storage.

The regression check is
`tests/automated/check-textdrop-record-allocation.py`.

## `motif-textout-navigator-trait.md`

# TextOut navigator callback validation

TextOut now checks the navigator trait’s `getValue` callback before reading
horizontal, vertical, or slider positions. A partial navigator trait is
ignored safely, while complete navigator implementations keep the existing
scroll and slider behavior.

## `motif-textsel-compound-value.md`

# Text compound-text value allocation failure

Text selection conversion copies Xlib compound-text property bytes into an
Xt-owned buffer.  The destination allocation is now checked before copying.
If it fails, the Xlib property is released, the conversion reports zero
length, and the request is refused safely.

The regression check is
`tests/automated/check-textsel-compound-value-allocation.py`.

## `motif-textsel-primary-state.md`

# Text primary-selection state allocation failure

Primary-selection handling allocates a small state record before requesting
the selected target.  The allocation is now checked while the process lock is
held.  On failure, the lock is released, callback data is cleaned up, and the
paste request returns without dereferencing null state.

The regression check is
`tests/automated/check-textsel-primary-state-allocation.py`.

## `motif-textsel-string-value.md`

# Text XA_STRING value allocation failure

Text selection conversion copies the Xlib text-property bytes into an Xt-owned
buffer for `XA_STRING` requests.  The destination allocation is now checked
before copying.  If it fails, the Xlib property is released, the conversion
reports zero length, and the request is refused safely.

The regression check is
`tests/automated/check-textsel-string-value-allocation.py`.

## `motif-textsel-text-value.md`

# Text/encoding value conversion failure

Text selection conversion obtains a temporary string for text and encoding
requests before calculating its length.  The result is now checked before
`strlen`.  If source conversion fails, the request reports zero length and is
refused without dereferencing null storage.

The regression check is
`tests/automated/check-textsel-text-value.py`.

## `motif-textsel-timestamp.md`

# Text selection timestamp allocation failure

Text selection conversion returns the selection timestamp through a temporary
`Time` object.  The allocation is now checked before the timestamp is written.
If it fails, conversion returns `False` with a null value so the request is
refused cleanly instead of dereferencing null storage.

The regression check is
`tests/automated/check-textsel-timestamp-allocation.py`.

## `motif-textsel-utf8-value.md`

# Text UTF-8 value allocation failure

Text selection conversion copies Xlib UTF-8 property bytes into an Xt-owned
buffer.  The destination allocation is now checked before copying.  If it
fails, the Xlib property is released, the conversion reports zero length, and
the request is refused safely.

The regression check is
`tests/automated/check-textsel-utf8-value-allocation.py`.

## `motif-txtpropcv-asn1-table.md`

# Non-empty ASN.1 string-table allocation

The non-empty Motif compound-string branch now checks its second-pass
`XmStringTable` allocation against Xt's `Cardinal` byte range before decoding
ASN.1 items and passes the checked byte count explicitly at the Xt allocation
boundary. Allocation failure returns `BadAlloc` with the application lock
balanced instead of entering the conversion loop with null storage.

The regression check is
`tests/automated/check-txtpropcv-asn1-table.py`.

## `motif-txtpropcv-asn1.md`

# ASN.1 compound-string output allocation

The `XmSTYLE_COMPOUND_STRING` conversion path now checks its final ASN.1
output buffer before converting items into it.  Allocation failure returns
`BadAlloc` with the application lock balanced instead of dereferencing a null
buffer.

The regression check is
`tests/automated/check-txtpropcv-asn1-allocation.py`.

## `motif-txtpropcv-compound-table.md`

# Compound-text table conversion buffers

`XmCvtXmStringTableToTextProperty` now checks both the per-item pointer table
against host `size_t` and Xt's `Cardinal` byte limits, including explicit
sizes for the pointer table and final null-separated compound-text buffer. A failed allocation frees
already converted items, releases the table, and returns `BadAlloc` while the
application lock is balanced.

The regression check is
`tests/automated/check-txtpropcv-compound-table.py`.

## `motif-txtpropcv-concat.md`

# Text-property list concatenation allocation

When an X text property yields multiple strings,
`TextPropertyToSingleTextItem` joins them into one result.  Result buffers are
now checked against both host `size_t` and Xt's `Cardinal` byte range before
initialization; allocation failure releases the Xlib string list and reports
`XNoMemory` to the caller.

The regression check is
`tests/automated/check-txtpropcv-concat-allocation.py`.

## `motif-txtpropcv-empty-asn1-table.md`

# Empty ASN.1 string-table allocation

When a Motif compound-string text property contains no items,
`XmCvtTextPropertyToXmStringTable` still returns an allocated empty table.  The
allocation is now checked and reports `BadAlloc` with the application lock
balanced if it fails.

The regression check is
`tests/automated/check-txtpropcv-empty-asn1-table.py`.

## `motif-txtpropcv-empty-plain-table.md`

# Empty plain text-property table allocation

For empty locale, ISO-8859-1, or UTF-8 text properties,
`XmCvtTextPropertyToXmStringTable` returns one generated empty `XmString`.  The
result table allocation is now checked and reports `BadAlloc` with balanced
application locking if it fails.

The regression check is
`tests/automated/check-txtpropcv-empty-plain-table.py`.

## `motif-txtpropcv-empty-segment.md`

# Empty text segment separators

`GetTextSegment` can receive an `XmNO_TEXT` segment with tabs or a separator
attached.  Separator insertion now treats a null segment buffer as an empty
string, preserving the existing tab/newline formatting without calling
`strlen` or `strcpy` on null storage.

The regression check is
`tests/automated/check-txtpropcv-empty-segment.py`.

## `motif-txtpropcv-final.md`

# Final text-property conversion buffer

After converting each item to usable text, the locale, text, string, and
standard ICC styles allocate one null-separated result buffer.  The allocation
is now checked; failure frees every per-item string, releases the pointer
table, balances the application lock, and returns `BadAlloc`.

The regression check is
`tests/automated/check-txtpropcv-final-allocation.py`.

## `motif-txtpropcv-generated-strings.md`

# Plain text-property XmString generation failures

When converting locale, ISO-8859-1, or UTF-8 text properties,
`XmStringGenerate` failures are now detected for both the first and subsequent
items.  Already generated entries and the table are released, application
locking is balanced, and the conversion returns `BadAlloc` instead of a
successful table containing null entries.

The regression check is
`tests/automated/check-txtpropcv-generated-strings.py`.

## `motif-txtpropcv-growth.md`

# Text-property conversion result growth

`GetUseableText` accumulates converted segments in `final_string`.  Growth is
now performed through a temporary pointer and checked before the result is
replaced.  On failure, the current segment, partial result, and string context
are released and the conversion reports `XNoMemory`.

The regression check is
`tests/automated/check-txtpropcv-growth-allocation.py`.

## `motif-txtpropcv-plain-table.md`

# Non-empty plain text-property table allocation

After counting NUL-separated locale, ISO-8859-1, or UTF-8 items,
`XmCvtTextPropertyToXmStringTable` now validates the table-size multiplication
and passes the checked byte count explicitly as an Xt `Cardinal` before
generating each `XmString`. Invalid sizes or allocation failure return
`BadAlloc` with balanced locking.

The regression check is
`tests/automated/check-txtpropcv-plain-table.py`.

## `motif-txtpropcv-segment-copy.md`

# Bounded compound-text segment copies

`XmCvtTextPropertyToXmStringTable` copies each bounded compound-text segment
into a temporary NUL-terminated buffer.  A failed segment allocation now frees
already converted `XmString` entries, releases the table, balances the
application lock, and returns `BadAlloc`.

The regression check is
`tests/automated/check-txtpropcv-segment-copy.py`.

## `motif-txtpropcv-segment.md`

# Text segment temporary buffers

`GetTextSegment` creates temporary buffers for locale and ISO-8859-1 segment
copies and for separator or tab insertion.  Each allocation is now checked;
failures return `_INVALID_SEGMENT` after releasing any partial result instead
of writing through null storage.  A negative length from the legacy signed
segment-length output is rejected before allocation, preventing an
unrepresentable long component from becoming a huge unsigned size.  The
validated segment byte count is passed explicitly as an Xt `Cardinal` for all
three copy paths.

The regression check is
`tests/automated/check-txtpropcv-segment-allocation.py`.

## `motif-txtpropcv-single.md`

# Single text-property result allocation

When an X text property produces exactly one string,
`TextPropertyToSingleTextItem` copies that item with `XtNewString`.  A failed
copy now releases the Xlib string list and returns `XNoMemory` instead of
reporting a successful conversion with a null result.

The regression check is
`tests/automated/check-txtpropcv-single-allocation.py`.

## `motif-txtpropcv-string-table.md`

# Compound-text string-table allocation

The compound-text branch of `XmCvtTextPropertyToXmStringTable` now checks its
result table allocation before handling empty or non-empty properties.  A
failure returns `BadAlloc` with the application lock balanced instead of
writing through a null table.

The regression check is
`tests/automated/check-txtpropcv-string-table-allocation.py`.

## `motif-txtpropcv-useable-table.md`

# Usable-text table allocation

The locale, text, string, and standard ICC conversion styles now check the
`useable_text` pointer table allocation against Xt's `Cardinal` byte range
before filling it.  Non-empty tables return `BadAlloc` with the application
lock balanced instead of entering the conversion loop with null storage.

The regression check is
`tests/automated/check-txtpropcv-useable-table.py`.

## `motif-txtpropcv-value.md`

# Fallback text-property value allocation

The fallback path in `XmCvtXmStringToText` converts a compound string to a
text property before asking Xlib for a text list.  Its temporary property value
is now checked before copying and passes its bounded byte count explicitly as
an Xt `Cardinal`; allocation failure frees the compound text, releases the
string context, and returns `XNoMemory`.

The regression check is
`tests/automated/check-txtpropcv-value-allocation.py`.

## `motif-xmstring-rendition.md`

# XmString rendition input validation

`XmStringPutRendition` now rejects a null XmString or rendition tag before
calling `strlen` or creating rendition components.  It returns `NULL` instead
of dereferencing invalid input.

The regression check is
`tests/automated/check-xmstring-rendition-input.py`.

## `motif-xmstring-segment-tags.md`

# XmString rendition-tag validation

Rendition-tag copying now rejects a null tag table or null entry before calling
`XtNewString`. Previously malformed input could reach `strlen` through the
allocator and crash. Cleanup remains transactional, and borrowed tag pointers
are never freed when the caller requested a non-copying operation.

## `motif-xmstring-tab-growth.md`

# XmString tab-list growth allocation failure

While scanning strings, `XmStringTableProposeTablist` may create additional
tabs.  Each new tab is now checked before it is linked into the list.  On
failure, the string context and partial tab list are released, the process
lock is balanced, and `NULL` is returned.

The regression check is
`tests/automated/check-xmstring-tab-growth-allocation.py`.

## `motif-xmstring-table-concat.md`

# XmString table concatenation input validation

`XmStringTableToXmString` now rejects a null table and empty table before
entering its concatenation loop.  The Motif process lock is released on this
path, and the function returns `NULL` instead of dereferencing a missing
table.

The regression check is
`tests/automated/check-xmstring-table-concat-input.py`.

## `motif-xmstring-table-parse.md`

# XmString table parse allocation failure

`XmStringTableParseStringArray` allocates the result table before parsing each
input string. The bounded byte size is passed explicitly as an Xt `Cardinal`
while the Motif process lock is held. If allocation fails, the lock is
released and the function returns `NULL` without writing through invalid
storage.

The regression check is
`tests/automated/check-xmstring-table-parse-allocation.py`.

## `motif-xmstring-table-split.md`

# XmString table splitting allocation and indexing

`XmStringToXmStringTable` now checks both output-table allocations and passes
the bounded counted-table size explicitly as an Xt `Cardinal`, while balancing
the process lock when allocation fails.  The no-break-component path also
writes the copied string through `(*table)[0]`, fixing the old precedence error
that wrote through an uninitialized pointer.

The regression check is
`tests/automated/check-xmstring-table-split.py`.

## `motif-xmstring-table-unparse.md`

# XmString table unparse allocation failure

`XmStringTableUnparse` allocates an output pointer table before converting each
XmString. The bounded byte size is passed explicitly as an Xt `Cardinal` while
the Motif process lock is held. If allocation fails, the lock is released and
`NULL` is returned without writing through invalid storage.

The regression check is
`tests/automated/check-xmstring-table-unparse-allocation.py`.

## `motif-xmstring-tablist.md`

# XmString tab-list construction failure

`XmStringTableProposeTablist` now checks the initial tab and tab-list objects
before using them.  If either creation fails, the temporary tab is released,
the process lock is balanced, and the function returns `NULL` safely.

The regression check is
`tests/automated/check-xmstring-tablist-create.py`.

## `motion-buffer-growth.md`

# Bound motion-buffer growth

Motion-event batching now checks both size arithmetic and Xt's `Cardinal`
allocation limit when promoting and expanding its buffer.  Initial heap
promotion and later `XtRealloc` failure return without writing through an
invalid pointer; count overflow is rejected before calculating the new size.

The focused source regression is
`tests/automated/check-motion-buffer-growth.py`.

## `motion-buffer-handoff.md`

# Preserve motion-buffer growth ownership

Motif batches pointer-motion events in a stack buffer and promotes it to a
heap buffer when the stack capacity is reached.  The update routine now
receives the caller's buffer pointer by reference, so that promotion and later
reallocation are visible to the event loop.  Previously the promoted pointer
was local to the helper; subsequent events continued writing through the old
stack address and could corrupt the stack.

The focused source regression is
`tests/automated/check-motion-buffer-handoff.py`.

## `mrm-alloc-string-bounds.md`

# Mrm shared string allocation bounds

The shared Mrm string helper now keeps the source length in `size_t`, checks
the terminator-inclusive allocation against Xt's `Cardinal` width, and copies
exactly the validated string bytes. Existing callers receive the same
null-terminated copy and `NULL` behavior for allocation failure, while
malformed sizes cannot wrap the allocation request.

The source-level regression is
`tests/automated/check-mrm-alloc-string-bounds.py`.

## `mrm-arg-bounds.md`

# Mrm argument-list bounds

Mrm value updates and widget creation now check Arg-list counts before adding
UID arguments or allocating the Xt argument arrays. Both host `size_t` and
Xt's `Cardinal` byte limits are enforced. Unrepresentable counts return
`MrmFAILURE` without wrapping; ordinary UID argument conversion and widget
creation are unchanged.

The source-level regression is `tests/automated/check-mrm-arg-bounds.py`.

## `mrm-arg-callback-record-bounds.md`

# Mrm callback-valued argument bounds

Callback-valued argument descriptors now pass through the same legacy/current
record-size validation as creation callbacks before translation and fixup.  A
malformed callback offset is skipped through the existing argument conversion
failure path; valid callback arguments retain their registration and deferred
widget-reference behavior.

The source-level regression is
`tests/automated/check-mrm-arg-callback-record-bounds.py`.

## `mrm-arg-descriptor-bounds.md`

# Mrm argument-list descriptor bounds

Mrm argument-list initialization now keeps descriptor sizes in `size_t` and
rejects counts whose inline descriptor cannot fit the format's 16-bit
`MrmSize` field. The existing zero-count rejection remains explicit, preventing
subtractive underflow before record allocation.

The source-level regression is `tests/automated/check-mrm-arg-descriptor-bounds.py`.

## `mrm-arg-tag-string-bounds.md`

# Mrm unknown argument-tag string bounds

Unknown argument tags now require a valid, NUL-terminated string inside the
widget record before conversion, deferred-reference handling, or final Xt
argument construction uses the tag offset.  Compressed known tags retain their
existing lookup path, and valid unknown tags remain compatible.

The source-level regression is
`tests/automated/check-mrm-arg-tag-string-bounds.py`.

## `mrm-arg-value-offset-bounds.md`

# Mrm argument-value offset bounds

`Urm__CW_CreateArglist` now validates non-immediate argument offsets before
converting them to pointers.  Offsets must point inside the serialized widget
record; integer, boolean, single-float, and null representations retain their
immediate-value behavior.  Invalid arguments are skipped through the existing
conversion-failure path, preserving valid UID argument ordering.

The source-level regression is
`tests/automated/check-mrm-arg-value-offset-bounds.py`.

## `mrm-arglist-offset-bounds.md`

# Mrm reader arg-list offset bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now verify that a nonzero
`arglist_offs` points inside the widget record, after its header and before the
record end. Malformed offsets fail before descriptor fields are read.

The source-level regression is
`tests/automated/check-mrm-arglist-offset-bounds.py`.

## `mrm-arglist-reader-counts.md`

# Mrm reader arg-list count bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now reject argument-list
counts or extra-runtime counts above `RGMListSizeMax` before size arithmetic,
allocation, and argument iteration. Valid legacy and current arg lists retain
their existing behavior.

The source-level regression is
`tests/automated/check-mrm-arglist-reader-counts.py`.

## `mrm-arglist-record-bounds.md`

# Mrm argument-list record bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now validate the complete
serialized argument-list descriptor before reading its fields.  They reject
negative or oversized counts and ensure every serialized argument descriptor
fits inside the widget record before the existing conversion and callback
logic runs.  Valid UID files retain the same argument and override behavior.

The source-level regression is
`tests/automated/check-mrm-arglist-record-bounds.py`.

## `mrm-btree-record-bounds.md`

# Mrm B-tree child lookup bounds

`Idb__INX_GetBtreeRecord` now requires a node record, validates its count and
heap metadata, bounds the requested entry, and rejects empty child links before
asking the buffer manager to load a record.

The source-level regression is
`tests/automated/check-mrm-btree-record-bounds.py`.

## `mrm-buffer-input.md`

# Mrm record-buffer input validation

Mrm buffer acquisition, record lookup, and record initialization now reject null
file and output-buffer pointers before touching the shared buffer pool. Valid
read, write, and initialization behavior is unchanged; malformed calls return
`MrmFAILURE` instead of dereferencing invalid context.

The source-level regression is `tests/automated/check-mrm-buffer-input.py`.

## `mrm-buffer-null-source.md`

# Mrm in-memory record source validation

In-memory record reads allow the header record to bypass the record-number upper
bound because the header is needed to establish file metadata. That exception
must not bypass the source-buffer check: a malformed descriptor with a null UID
buffer would otherwise copy from address zero. The null check now applies to all
in-memory records while the header-number exception remains intact.

## `mrm-buffer-output-init.md`

# Mrm record-buffer output initialization

The record-buffer acquisition and initialization entry points now set their
output pointer to `NULL` before doing further work. A failed allocation,
validation, or read therefore cannot leave a caller with a stale buffer from a
previous operation. `Idb__BM_GetBuffer` also rejects a non-positive pool size
before it searches or dereferences the pool, and clears a selected pool entry
from the output if record storage cannot be allocated. Successful calls retain
the existing buffer-selection behavior.

## `mrm-buffer-pool-size.md`

# Mrm buffer-pool size validation

The Mrm buffer pool has an externally visible size setting so embedders can
choose an appropriate cache capacity. A non-positive value cannot provide a
valid pool and would otherwise make later buffer acquisition dereference an
empty allocation. Initialization now rejects such values before allocation,
leaving the pool uninitialized and returning `MrmFAILURE`.

Initialization also rejects a positive count whose multiplication by the
record-buffer size would overflow either `size_t` or Xt's `Cardinal` allocation
width, protecting 32-bit and constrained builds before allocation.

## `mrm-buffer-postconditions.md`

# Mrm buffer postconditions

`Idb__BM_GetRecord` and `Idb__BM_InitRecord` now verify that successful buffer
acquisition returned both a buffer object and record storage before reading or
initializing record headers.  Incomplete allocator results fail cleanly.

The source-level regression is
`tests/automated/check-mrm-buffer-postconditions.py`.

## `mrm-buffer-valid-macro.md`

# Mrm buffer-validity predicate

The shared `Idb__BM_Valid` predicate now checks for a null buffer before
reading its validation field.  Marking and decommitting APIs can therefore
reject missing handles safely instead of crashing in the validation macro.

The source-level regression is
`tests/automated/check-mrm-buffer-valid-macro.py`.

## `mrm-cache-hit-record.md`

# Mrm buffer cache-hit validation

`Idb__BM_GetRecord` scans the buffer pool before loading a record from disk. A
cache entry is considered a hit only when it belongs to the requested file and
contains record storage. The storage check precedes access to the record
header, so a partially initialized or reclaimed entry is treated as a miss
instead of being dereferenced.

This preserves the existing cache behavior for valid entries while making the
scan safe during cleanup and recovery paths.

## `mrm-callback-bounds.md`

# Mrm legacy callback bounds

Legacy Mrm callback descriptor translation now rejects null descriptors,
negative callback counts, and counts whose descriptor-size arithmetic cannot be
represented by either `size_t` or Xt's `Cardinal` allocation width before
allocating the translated callback record.
Valid 1.1 callback descriptors retain their existing item-copy behavior.

The source-level regression is `tests/automated/check-mrm-callback-bounds.py`.

## `mrm-callback-cleanup-bounds.md`

# Mrm callback cleanup bounds

Callback-value cleanup now tolerates a null or empty ownership list and limits
iteration to complete type/value pairs.  If an allocation or append failure
leaves a final type tag without a value, that tag is discarded before the list
is released.  Corrupt pointer counts are rejected before traversal as well,
avoiding an out-of-bounds read while retaining normal callback value
destruction.

The source-level regression is
`tests/automated/check-mrm-callback-cleanup-bounds.py`.

## `mrm-callback-context-init.md`

# Mrm callback context-list initialization

Widget-instance creation and automatic-child setup now propagate failure when
their callback context list cannot be initialized.  Callback fixup no longer
receives a null list after an ignored allocation failure.

The source-level regression is
`tests/automated/check-mrm-callback-context-init.py`.

## `mrm-callback-copy-bounds.md`

# Mrm callback descriptor copy bounds

SetValues callback descriptors copied from Mrm records now reject null
pointers, negative counts, and descriptor-size arithmetic that cannot be
represented by either `size_t` or Xt's `Cardinal` allocation width before
allocating or copying. The copy-size variable also remains native `size_t`,
avoiding truncation through the legacy `MrmSize` type. Valid callback
descriptors retain their existing behavior.

The source-level regression is `tests/automated/check-mrm-callback-copy-bounds.py`.

## `mrm-callback-descriptor-bounds.md`

# Mrm callback descriptor bounds

Mrm argument and creation callback constructors now keep descriptor sizes in
`size_t` and reject counts whose complete records cannot fit in the format's
16-bit `MrmSize` field. Valid callback counts retain their existing record
layout and terminating runtime entry.

The source-level regression is `tests/automated/check-mrm-callback-descriptor-bounds.py`.

## `mrm-callback-fixup-validation.md`

# Mrm callback fixup validation

`Urm__CW_FixupCallback` now validates callback descriptor metadata and its
record buffer before iterating items, rejects missing resource descriptors,
and skips callback installation when value conversion fails. Valid callback
records retain their existing resolution and unresolved-reference behavior.

The source-level regression is
`tests/automated/check-mrm-callback-fixup-validation.py`.

## `mrm-callback-item-binding.md`

# Mrm callback item binding failures

The callback item setters now check context validation and every callback
descriptor binding. This matters after appending routine names or callback
values, because appends can relocate the resource record. A failed rebind is
returned to the caller instead of dereferencing stale callback-item pointers.

The source-level regression is
`tests/automated/check-mrm-callback-item-binding.py`.

## `mrm-callback-name-allocation.md`

# Mrm deferred callback-name allocation

When a callback resource refers to a widget that has not been created yet,
Mrm keeps a copy of the referenced widget name for later resolution.  Callback
fixup now checks that copy operation and returns `MrmFAILURE` immediately when
memory cannot be allocated, before publishing an unresolved callback entry.
This keeps deferred-reference cleanup and retry paths from receiving a null
name while preserving the existing successful resolution behavior.

The source-level regression is
`tests/automated/check-mrm-callback-name-allocation.py`.

## `mrm-callback-record-bounds.md`

# Mrm creation callback record bounds

Mrm widget creation now validates both legacy and current serialized callback
descriptors before translating or fixing them up.  The descriptor header, the
signed callback count, and the count-plus-terminator item storage must fit
inside the widget record.  Valid callback records keep their existing callback
lookup and invocation behavior.

The source-level regression is
`tests/automated/check-mrm-callback-record-bounds.py`.

## `mrm-callback-routine-offset.md`

# Mrm callback routine offsets

`Urm__CW_FixupCallback` now rejects callback items with a zero routine-name
offset before treating the record header as a string. Valid callbacks retain
their existing hierarchy lookup behavior.

The source-level regression is
`tests/automated/check-mrm-callback-routine-offset.py`.

## `mrm-callback-routine-string-bounds.md`

# Mrm callback routine-string bounds

`Urm__CW_FixupCallback` now validates each serialized callback routine offset
before converting it to a string.  The offset must point beyond the widget
record header, stay within the record, and reach a NUL terminator before the
hierarchy lookup runs.  Valid callback registration and invocation remain
unchanged.

The source-level regression is
`tests/automated/check-mrm-callback-routine-string-bounds.py`.

## `mrm-callback-value-offset-bounds.md`

# Mrm callback-value offset bounds

`Urm__CW_FixupCallback` now validates non-immediate callback datum offsets
before converting them to pointers.  Integer, boolean, single-float, and null
values retain their immediate behavior; other values must point inside the
widget record.  Valid callback resource and literal values continue through
the existing conversion path.

The source-level regression is
`tests/automated/check-mrm-callback-value-offset-bounds.py`.

## `mrm-child-context-init.md`

# Mrm child-tree context initialization

Widget-tree creation now checks the resource-context allocation used to load
children.  If that allocation fails, the tree operation returns an error before
attempting child reads through a null context.  Successful child loading and
all public Mrm interfaces remain unchanged.

The source-level regression is
`tests/automated/check-mrm-child-context-init.py`.

## `mrm-child-repair-bounds.md`

# Mrm child-repair bounds

`Idb__INX_FixNodeChildren` now rejects impossible node counts, heap offsets, and
free-space metadata before traversing child entries.  Its loop counter uses
`MrmCount`, avoiding truncation when processing large valid metadata values.

The source-level regression is
`tests/automated/check-mrm-child-repair-bounds.py`.

## `mrm-children-bounds.md`

# Mrm child-list bounds

Mrm child-list initialization now rejects zero counts before subtracting one
for the inline descriptor entry, and checks the resulting descriptor size
against the format's 16-bit `MrmSize` field. Positive valid child counts retain
their existing record layout and initialization behavior.

The source-level regression is `tests/automated/check-mrm-children-bounds.py`.

## `mrm-class-comment-validation.md`

# Mrm class and comment validation failures

`UrmCWRSetClass` and `UrmCWRSetComment` now return the result of context
validation before dereferencing the widget record. Invalid or detached writer
contexts therefore fail consistently instead of continuing into record access.

The source-level regression is
`tests/automated/check-mrm-class-comment-validation.py`.

## `mrm-clobber-close.md`

# Mrm clobber-open close handling

When write mode replaces an existing file, Mrm now checks the close result for
the original descriptor before calling `creat`. A close failure returns
immediately instead of proceeding with an uncertain descriptor state.

## `mrm-close-decommit-failure.md`

# Mrm close retry after decommit failure

`UrmIdbCloseFile` now returns the decommit error while retaining the valid
`IDBFile` descriptor when a dirty buffer cannot be written. The associated
buffer remains attached and can be retried by the caller. Class and resource
table pointers are cleared immediately after their storage is released, so a
later close attempt cannot free those tables twice.

## `mrm-close-delete.md`

# Mrm close-failure deletion

`Idb__FU_CloseFile` now attempts to unlink the file even when the descriptor
close fails and the caller requested deletion. This keeps failed temporary or
newly created databases from being retained solely because close reported an
error.

## `mrm-compression-inputs.md`

# Mrm compression-table input checks

The compression helpers are internal boundaries shared by widget creation and
resource-reference handling. They now reject null file, output, and return
pointers before dereferencing them, and reject table entries that do not contain
a string or class descriptor. Subtree checks return false for an unavailable or
invalid table. Valid compressed resource and class lookups keep their existing
results and diagnostics.

Class-descriptor and resource-name decompression lookups now clear their
validated output pointers before examining the requested table entry.  A
missing table, invalid code, or empty entry cannot leave a stale class or
string pointer in the caller's output.

## `mrm-confirm-node-space.md`

# Mrm node-space validation

`Idb__INX_ConfirmNodeSpace` now requires a node record and validates its count,
heap offset, free-space range, and vector footprint before deciding whether a
split is needed.  Corrupt metadata is returned as `MrmBAD_RECORD` instead of
being treated as available space.

The source-level regression is
`tests/automated/check-mrm-confirm-node-space.py`.

## `mrm-context-output-pointer.md`

# Mrm resource-context output pointer

`UrmGetResourceContext` rejects a missing context output pointer before
selecting allocators or dereferencing the caller's storage. Once validated, it
clears the output before allocation and clears it again after releasing a
partially allocated context when its data buffer cannot be allocated. Failed
construction therefore cannot leave a stale or freed context handle with the
caller.

The source-level regression is `tests/automated/check-mrm-context-output-pointer.py`.

## `mrm-context-realloc.md`

# Mrm resource-context resizing

`UrmResizeResourceContext` now keeps the old buffer owned by the context until
`XtRealloc` succeeds.  A failed resize returns `MrmFAILURE` without publishing
a null pointer or changing the recorded buffer size.  Its validated public
`int` request is first retained as `MrmSize`, so the reallocation and recorded
context size use the same bounded representation.

The source-level regression is
`tests/automated/check-mrm-context-realloc.py`.

## `mrm-context-resize-bounds.md`

# Mrm context resize bounds

`UrmResizeResourceContext` now rejects negative requested sizes before it
applies its minimum-buffer rule. A negative value is invalid input; it must not
silently become a successful minimum-size allocation. The focused regression is
`tests/automated/check-mrm-context-resize-bounds.py`.

An equal-size request is also a no-op: existing sufficient storage is retained
instead of performing an unnecessary allocation that could fail.

## `mrm-context-valid-macro.md`

# Mrm resource-context validation macro

`UrmRCValid` now checks for `NULL` before reading the context validation
field. All existing valid-context checks keep the same result, while callers
that pass no context receive their normal bad-context path instead of a null
pointer dereference.

## `mrm-conversion-allocation.md`

# Mrm conversion allocation failures

Legacy callback and font-list conversion now checks every variable-size
allocation before dereferencing the result. Null translated callbacks and
font lists return the existing failure path, while valid UID conversion and
callback setup retain their previous behavior.

The source-level regression is
`tests/automated/check-mrm-conversion-allocation.py`.

## `mrm-conversion-error-bounds.md`

# Mrm conversion diagnostic bounds

Widget creation, value conversion, callback, color, and resource lookup paths
format diagnostics into fixed 300-byte Mrm buffers. All 37 formatting sites in
`Mrmwcrw.c` now use `snprintf`, preserving normal short messages while safely
truncating long resource names, font strings, and other input-derived text.

The source-level regression is
`tests/automated/check-mrm-conversion-error-bounds.py`.

## `mrm-convert-char8-null.md`

# Mrm Char8 conversion null sources

`Urm__CW_ConvertValue` now rejects a null Char8 value before passing it to
`XmStringGenerate` or `XtParseTranslationTable`. Valid strings and existing
conversion behavior remain unchanged.

The source-level regression is
`tests/automated/check-mrm-convert-char8-null.py`.

## `mrm-convert-font-validation.md`

# Mrm font conversion validation

`Urm__CW_ConvertValue` now rejects null font records, invalid font-list
markers, over-sized font lists, and missing font strings before calling
`strlen` or the X font loaders. Valid font and font-list conversions retain
the existing display-cache behavior.

The source-level regression is
`tests/automated/check-mrm-convert-font-validation.py`.

## `mrm-convert-pointer-null.md`

# Mrm pointer-backed conversion null values

`Urm__CW_ConvertValue` now rejects null CString, address-name, icon-image, and
bitmap-file values before invoking toolkit or hierarchy helpers. Valid
pointer-backed resources retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-convert-pointer-null.py`.

## `mrm-convert-primitive-pointer-validation.md`

# Mrm primitive conversion pointer validation

`Urm__CW_ConvertValue` now rejects missing color descriptors, wide-character
entries, unit values, and string-backed translation, class-name, and keysym
values before dereferencing or passing them to X/Motif helpers. Valid resource
values retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-convert-primitive-pointer-validation.py`.

## `mrm-convert-vector-validation.md`

# Mrm conversion vector validation

`Urm__CW_ConvertValue` now validates Char8, compound-string, and integer
vector pointers, validation markers, and counts before repacking them for Xt.
Malformed resource values are rejected cleanly while valid vectors retain the
same pointer-table representation.

The source-level regression is
`tests/automated/check-mrm-convert-vector-validation.py`.

## `mrm-create-tree-child-index-bounds.md`

# Mrm widget-tree child index bounds

`UrmCreateWidgetTree` now validates the complete serialized child descriptor
region before reading its count or iterating descriptors.  Index children must
point into the widget record and contain a terminating NUL before the lookup
helpers receive the string.  Malformed records release the temporary child
context before returning, while valid resource files keep the existing public
and private lookup behavior.

The source-level regression is
`tests/automated/check-mrm-create-tree-child-index-bounds.py`.

## `mrm-create-tree-children-bounds.md`

# Mrm widget-tree child bounds

`UrmCreateWidgetTree` now checks that a nonzero child-list offset remains inside
the widget record and that the child count stays within `RGMListSizeMax` before
iterating child descriptors. Valid widget trees retain their existing lookup
and creation behavior.

The source-level regression is
`tests/automated/check-mrm-create-tree-children-bounds.py`.

## `mrm-createarg-color-fontfixup.md`

# Mrm color and font reader boundaries

The special color handling in `Urm__CW_CreateArglist` now skips a missing color
descriptor safely and stops before storing a color pixel when named/RGB color
conversion fails. `Urm__CW_FixupValue` rejects null font records or buffers
before swapping offsets. Valid color and font resources retain their existing
conversion behavior.

The source-level regression is
`tests/automated/check-mrm-createarg-color-fontfixup.py`.

## `mrm-createarg-resource-null.md`

# Mrm argument-list resource descriptors

`Urm__CW_CreateArglist` now skips a missing resource descriptor before reading
its swap flag, group, or conversion type. Valid resource arguments retain their
existing widget-reference and literal resolution behavior.

The source-level regression is
`tests/automated/check-mrm-createarg-resource-null.py`.

## `mrm-cstring-append-bounds.md`

# Mrm compound-string append bounds

Mrm compound-string appends now retain the encoded byte length in `size_t` and
reject encoded data larger than `MrmMaxResourceSize` before passing it to the
16-bit record allocator. Empty and valid compound strings keep their existing
behavior.

The source-level regression is `tests/automated/check-mrm-cstring-append-bounds.py`.

## `mrm-cstring-conversion-failure.md`

# Mrm compound-string conversion failures

Mrm converts serialized compound-string literals through
`XmCvtByteStreamToXmString`.  Invalid ASN.1 data can make that conversion
return `NULL`.  Mrm now returns `MrmFAILURE` instead of passing that null value
to widget construction or recording it for later cleanup.

The conversion guard is covered by
`tests/automated/check-mrm-conversion-allocation.py`.

## `mrm-cstring-stream-cleanup.md`

# Mrm compound-string stream cleanup

`UrmCWR__AppendCString` now releases the temporary byte stream returned by
`XmCvtXmStringToByteStream` after copying it into the resource record.  Empty,
oversized, and failed-resize paths release the temporary allocation as well.

The source-level regression is
`tests/automated/check-mrm-cstring-stream-cleanup.py`.

## `mrm-cstring-vector-cleanup.md`

# Mrm compound-string vector cleanup

Mrm converts every serialized compound-string vector entry independently.  If
one malformed entry fails conversion, it now frees the entries converted before
it and returns `MrmFAILURE`, rather than leaking a partial vector.

## `mrm-data-context-null.md`

# Mrm data-entry context null guard

`Idb__DB_GetDataEntry` now checks for a null resource context before invoking
the validation macro. Public indexed-resource and RID retrieval calls with a
null context return the existing bad-context error instead of dereferencing a
null pointer. Valid contexts follow the historical path unchanged.

## `mrm-data-offset.md`

# Mrm data-entry offset validation

Data handles carry a record number and an offset to an entry inside the fixed
size data-record payload. The readers now reject offsets that cannot even hold
a complete `IDBDataEntryHdr` before forming the entry pointer. This prevents
malformed handles or corrupted files from indexing past the record while
preserving all valid handles produced by the writer.

## `mrm-data-payload.md`

# Mrm data-payload validation

Mrm data records store either a simple payload or a chain of fixed-size
overflow segments. The reader now checks simple payload capacity against the
record boundary and validates every overflow segment header, payload size,
segment number, and cumulative byte count before copying. Malformed records
therefore fail with `MrmNOT_VALID` instead of reading beyond the record or the
resource context.

## `mrm-decommit-all-failure.md`

# Mrm decommit-all failure retention

`Idb__BM_DecommitAll` now keeps a buffer attached to its `IDBFile` when
`Idb__BM_Decommit` cannot write it. The buffer remains marked modified, and
the first failure is returned after the remaining buffers are attempted. This
preserves the pending record for a retry and avoids reporting a failed close as
though its data had been released. Buffers that decommit successfully retain
the historical release behavior.

## `mrm-decommit-all-input.md`

# Mrm decommit-all input

`Idb__BM_DecommitAll` now rejects a null file handle before scanning the shared
buffer pool, avoiding accidental matches against unused entries whose owner is
also null.

The source-level regression is
`tests/automated/check-mrm-decommit-all-input.py`.

## `mrm-decommit-record.md`

# Mrm buffer decommit record validation

A modified write buffer must retain both its owning file and its record storage
until decommit completes. `Idb__BM_Decommit` now rejects a missing record before
calling the low-level writer, preventing malformed buffer state from reaching a
null dereference while preserving the normal write/decommit path.

## `mrm-decommit-state.md`

# Mrm buffer decommit state

`Idb__BM_Decommit` now rejects modified write buffers that have lost their file
owner, and clears activity/modified state after every successful decommit,
including read-only buffers.  Released buffers are therefore reusable without
silently retaining stale activity.

The source-level regression is `tests/automated/check-mrm-decommit-state.py`.

## `mrm-display-string-bounds.md`

# Mrm display-string conversion bounds

Font and font-set conversion prefixes a display pointer to a font name before
using the result as a cache key. The helper now rejects null or invalid size
inputs, checks the terminator-inclusive source length and allocation addition,
and copies the suffix directly. Valid calls retain the same cache-key bytes and
allocation sizing while malformed inputs return `NULL` safely.

The source-level regression is
`tests/automated/check-mrm-display-string-bounds.py`.

## `mrm-entry-read-inputs.md`

# Mrm data-entry reader inputs

The data-entry readers now reject a null `IDBFile` before handling header
records or asking the buffer manager to load a data record. This keeps invalid
public calls on the normal failure path and avoids dereferencing the file
handle in lower-level helpers.

## `mrm-error-allocation.md`

# Mrm error-message allocation

Memory-mode `Urm__UT_Error` now allocates the replacement saved message before
freeing the previous one.  If the allocation fails, the error status still
returns without dereferencing a null pointer and the prior saved message stays
valid.

The source-level regression is
`tests/automated/check-mrm-error-allocation.py`.

## `mrm-error-format-bounds.md`

# Mrm error formatting bounds

Mrm error reporting uses fixed 300-byte buffers for the latest diagnostic and
for hierarchy-open messages. Those buffers now use `snprintf`, so long module,
resource, or filename text is truncated safely instead of overwriting stack
storage. Normal short diagnostics retain their existing text and status.

The source-level regression is
`tests/automated/check-mrm-error-format-bounds.py`.

## `mrm-extra-args.md`

# Mrm extra-argument counts

Mrm extra-argument metadata now rejects counts above the signed record limit
before storing them in the descriptor's `MrmCount` field. This prevents large
`Cardinal` inputs from wrapping negative and preserves valid runtime arg-list
behavior.

The source-level regression is `tests/automated/check-mrm-extra-args.py`.

## `mrm-fetch-fontlist-bounds.md`

# Mrm fetched font-list bounds

`UrmFetchSetValues` translates the variable-sized font-list record used by
legacy (URM 1.1 and earlier) UID files before converting it into a widget
argument. The record count is signed input from the file. The translation now
rejects zero, negative, and too-large counts before calculating the allocation
size. Invalid values are released and reported as conversion failures, while
valid font lists retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-fetch-fontlist-bounds.py`.

## `mrm-fetch-index-bounds.md`

# Mrm fetch-set-values index bounds

`UrmFetchSetValues` builds a reusable resource descriptor from the longest
literal index supplied by the application. The helper now rejects names that
would exceed the 16-bit Mrm resource-size limit or wrap the allocation
addition before allocating the descriptor. Valid literal fetches retain their
existing lookup and conversion behavior.

The source-level regression is
`tests/automated/check-mrm-fetch-index-bounds.py`.

## `mrm-fetch-literal-output.md`

# Mrm literal output validation

`MrmFetchLiteral` returns both a literal value and its `MrmCode` through caller
owned output pointers. The legacy implementation entered the application and
process locks, fetched the resource, and dereferenced those pointers without
checking them. Passing either output as `NULL` could therefore crash the
process instead of returning the documented failure status.

The routine now rejects missing output pointers before acquiring locks or
allocating a resource context, and clears both outputs before attempting the
fetch. An unsupported icon, color, color table, or bitmap-file literal now
also clears both outputs after releasing its backing context, rather than
returning a dangling value pointer with `MrmWRONG_TYPE`. Existing callers that
provide valid outputs retain the same successful lookup and conversion
behavior. Failed Char8, compound-string, and integer vector conversions now
release the primary and auxiliary resource contexts and clear both outputs
before returning failure. The source-level regression is
`tests/automated/check-mrm-fetch-literal-output.py`.

## `mrm-fetch-setvalues-allocation.md`

# Mrm set-values allocation handling

`UrmFetchSetValues` now rejects an empty argument list before creating
scratch state, checks the local argument array and pointer-list setup, checks
the reusable resource descriptor allocation, and handles legacy font-list
allocation failure without entering fixup code with a null pointer.

The source-level regression is
`tests/automated/check-mrm-fetch-setvalues-allocation.py`.

## `mrm-file-io-input.md`

# Mrm low-level file I/O validation

Mrm close, read-block, and write-block helpers now reject null descriptors or
buffers before system calls. Read and write operations also reject nonpositive
record numbers, preventing seeks before the UID file. Valid file I/O retains
its existing behavior and return codes.

The source-level regression is `tests/automated/check-mrm-file-io-input.py`.
