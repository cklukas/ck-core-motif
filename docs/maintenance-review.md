---
nav_exclude: true
---
# Maintenance review and suggested work

## DatePicker typography

`XmDatePicker` now owns a copy of its `XmNfontList`/`XmNrenderTable` and draws
the formatted date with `XmStringExtent` and `XmStringDraw`. This removes the
fixed-core-font `XDrawString` path while preserving the existing date format,
left inset, and public resources. The regression check is
`tests/automated/check-datepicker-render-table.py`.

## TimePicker typography

`XmTimePicker` now owns a copy of its `XmNfontList`/`XmNrenderTable` and draws
both 24-hour and 12-hour formatted values with `XmStringExtent` and
`XmStringDraw`. This removes the fixed-core-font `XDrawString` path while
preserving the public value and format resources. The regression check is
`tests/automated/check-timepicker-render-table.py`.

## Calendar typography

`XmCalendar` now owns a copy of its `XmNfontList`/`XmNrenderTable` and draws
the month heading, weekday names, and day numbers with measured `XmString`
geometry. This removes the fixed-core-font paths while preserving date
selection and navigation. The regression check is
`tests/automated/check-calendar-render-table.py`.

## Scale value-label rendering

`XmScale` now routes its non-Xft value-label draw through the widget's Motif
render table whenever one is available. This preserves compound-string and
locale conversion instead of sending the localized bytes directly to
`XDrawImageString`; the raw draw remains only as a compatibility fallback for
widgets without a render table. The regression check is
`tests/automated/check-scale-render-table-draw.py`.

## SegmentedControl relayout redraw

`XmSegmentedControl` now reports a redraw when its RTL, spacing, or
clear-selection resources change. The child buttons were already relaid out,
but returning `False` from `SetValues` could leave the parent display stale.
The regression check is `tests/automated/check-segmented-setvalues-redraw.py`.

## Compound-string label ownership

StatusBar, Toolbar, Toast, and Skeleton now use `XmStringCopy` when retaining
caller-provided labels, and StatusBar progress suffixes and Toolbar shortcuts
are concatenated as `XmString` segments. The previous unparse/recreate round
trip could discard compound-string charset segments before rendering. The
regression check is `tests/automated/check-compound-label-copy.py`.

## Toast SetValues redraw

`XmToast` now returns its changed state from `SetValues` after updating child
labels and negotiating geometry. It also uses `XmStringEmpty` to determine
whether an action label is present, preserving compound-string content. The
regression check is `tests/automated/check-toast-setvalues-redraw.py`.

## CommandPalette compound-string ownership

`XmCommandPalette` now copies item labels and shortcuts directly as
`XmString` values, preserving compound-string charset segments in its stored
item model. The regression check is
`tests/automated/check-command-palette-string-copy.py`.

## IconLabel typography and natural geometry

`XmIconLabel` now measures its `XmString` through the render table for both
layout and drawing, preserving right-to-left placement. When dimensions are
omitted, initialization derives natural geometry from the original requested
size before the primitive superclass's minimum defaults can mask it. The
regression check is `tests/automated/check-iconlabel-render-table.py`.

## DataGrid typography

`XmDataGrid` now owns a copy of its `XmNfontList`/`XmNrenderTable` and draws
headers and cells with measured `XmString` geometry. RTL layout and selection
painting remain unchanged. The regression check is
`tests/automated/check-datagrid-render-table.py`.

## Breadcrumb geometry

Breadcrumb labels now begin after the measured separator width, selected
backgrounds match the actual label, and the overflow ellipsis uses the
configured render table. The regression check is
`tests/automated/check-breadcrumb-render-metrics.py`.

## DropDown width allocation

DropDown now preserves its preferred label width while reducing the editable
text field during narrow layouts, avoiding clipped labels and overflow-prone
proportional arithmetic. The regression check is
`tests/automated/check-dropdown-width-allocation.py`.

## Link and Prompt natural geometry

Link and Prompt now calculate their preferred dimensions from measured label
text when the application omits a size, using the pre-superclass request
instead of inheriting a six-pixel placeholder. The regression check is
`tests/automated/check-natural-label-geometry.py`.

## Render-table update redraws

The migrated DatePicker, TimePicker, IconLabel, and DataGrid widgets now mark
themselves for redisplay when `XmNfontList` or `XmNrenderTable` changes at
runtime. The regression check is
`tests/automated/check-render-table-redraw.py`.

Prompt also marks its own render-table changes for redisplay; this is covered
by `tests/automated/check-prompt-render-redraw.py`.

Avatar, Badge, and Divider now do the same; coverage is in
`tests/automated/check-small-render-redraw.py`.

Link preferred-size calculation now measures the complete `XmString` directly
instead of truncating through a fixed unparse buffer. This is covered by
`tests/automated/check-link-geometry-unparse.py`.

The first widget review was a bounded starting point. The age of the inherited
code is a reason to continue targeted maintenance; neither a short patch list
nor a large test count establishes that the toolkit is finished.

## Screen font resource safety

`XmScreen` accepts an `XmNfont` resource, but Xlib font conversion can return
`NULL` when the requested server font is unavailable. `SetValues` now compares
the pointers before reading `fid` and delegates null cases to
`GetUnitFromFont`, which supplies the existing default unit values. This keeps
font changes compatible while preventing a null dereference during resource
updates. The focused structural regression is
`tests/automated/check-screen-font-null-guard.py`.

## Chip drawing metrics

`XmChip` now obtains label extents from its render table before calling
`XmStringDraw`; previously the redisplay-local extents remained zero and
labels could be clipped at the pill's lower edge. Removable chips use a
fixed eight-pixel close glyph centered on the control, independent of its
height. The focused regression is
`tests/automated/check-chip-label-render-metrics.py`.

## DropHighlight label rendering

`XmDropHighlight` now honors its existing `XmNdropHighlightLabel` resource:
the label is measured and drawn centered inside the highlight border using the
widget's owned render table. Query geometry includes that measured label size,
so a label is no longer silently accepted and then omitted from redisplay.
The focused regression is
`tests/automated/check-drophighlight-label-render.py`.

## Divider typography

`XmDivider` labels now use an owned render table and measured `XmStringDraw`
placement instead of fixed-width `XDrawString` assumptions. This keeps the
label centered for proportional, Xft and multilingual fonts. The focused
regression is `tests/automated/check-divider-render-table.py`.

## Breadcrumb selected-label contrast

`XmBreadcrumbBar` now draws labels through an owned render table. For the
selected segment it paints the selection rectangle first, then uses the
background color for the label and restores the normal foreground for the
following segments. This prevents the selected path from becoming unreadable.
The focused regression is
`tests/automated/check-breadcrumb-selected-contrast.py`.

## Avatar typography

`XmAvatar` initials now use an owned render table and measured
`XmStringDraw` placement instead of fixed-width X11 text. This keeps fallback
initials centered for proportional, Xft and multilingual fonts. The focused
regression is `tests/automated/check-avatar-render-table.py`.

## Stepper control glyphs

`XmStepper` now draws distinct centered increment (`+`) and decrement (`−`)
glyphs in the two button regions using measured render-table text. This fixes
the previous single oversized mark that crossed both hit regions. The focused
regression is `tests/automated/check-stepper-control-glyphs.py`.

## TabBox background fallback

Standalone `XmTabBox` instances now start redisplay with their own
`XmNbackground`/`XmNbackgroundPixmap` resources. Previously the fallback pixel
was zero unless a TabStack parent supplied an override, producing a black
unused region in basic tab layouts. The focused regression is
`tests/automated/check-tabbox-background-fallback.py`.

## Concrete storage and image corrections

The next source review identified the following independently useful fixes.
Their regression procedure and compatibility contracts are documented under
[clipboard storage and image input](storage-and-images.md).

| Finding | Effect | Correction |
| --- | --- | --- |
| Clipboard property allocation combines native first-chunk bytes with wire bytes remaining | Multi-request format-32 reads overrun storage on LP64 | Assemble checked native-size chunks; retain wire offsets separately |
| Conversion merges narrow unsigned item counts to signed int byte counts | Overflow can underallocate the output or read an enormous source length | Bound the complete result to Xt's allocation limit before arithmetic |
| Merge reallocates before copying a source alias | The appended source can refer to freed storage | Keep the old value alive until both copies complete |
| Unified-transfer destruction uses call data as the selection atom | It disowns atom zero rather than the owned selection | Read the closure argument and retain one callback per selection |
| XPM comment parsing does not stop at EOF/NUL | Malformed image input can hang or read past its buffer | Bound comment and string/token scans at the actual input end |
| XPM memory character reads advance beyond NUL and treat high bytes as signed | Truncated pixel tokens overrun; valid high-byte symbols can fail | Return EOF at NUL and read bytes unsigned |
| XPM unsigned decimal fields wrap | Oversized dimensions can be accepted as smaller values | Reject overflow before multiplying and adding |
| Buffer-to-file writing ignores final flush failure and rejects empty output | Success can leave incomplete output; an empty buffer fails unnecessarily | Check byte writes and close; accept zero bytes |
| File-to-buffer read count uses int and a size failure closes only its descriptor | Large-file comparison truncates; stream storage leaks | Retain size_t counts and close the owning FILE |

## Continuing review priorities

These are proposed work packages with explicit acceptance criteria, not claims
that all listed behavior is broken. Independent reproducers should decide each
change. Preserve the public resource/callback model; document intentional
behavior changes, and version any unavoidable binary layout changes.

| Priority | Area | Useful work and acceptance evidence |
| --- | --- | --- |
| High | Large-text editing and layout | Measure loading, edits and redraw separately at 1 MiB and larger sizes. Compare incremental results with fresh layout, including CRLF, bidi, wrapping and font changes. Legacy line-table reflow and TextField modify-verify snapshots must validate their allocations and retain safe state after failure. The current incremental-layout work has its own source and native qualification. |
| High | Unicode across the whole widget set | Extend real input, selection, search and mnemonic tests to List, ComboBox, labels, menus, file dialogs and compound strings. Cover combining sequences, non-BMP characters, mixed scripts and invalid locale input; text-editor shaping alone does not establish these widgets. |
| High | Selection and drag/drop lifecycle | Exercise external owners, cancellation, owner replacement, reentrant callbacks, destruction and property changes, with leak checks. Include 8/16/32-bit formats and request boundaries. Invalid null-widget and `None`-atom ownership requests must fail before Xt state access. Keep dependency defects distinct from toolkit defects. |
| High | Geometry at large fonts and display scales | Audit Dimension/Position/int conversions, overflow, clipping and hit tests across scrollable widgets and very long labels. Test real widget windows at 1x/2x/4x and explicit application dimensions. List and zero-based ComboBox APIs that expose one-based `int` positions must reject values whose compatibility offset is not representable. |
| High | Widget lifetime and resource ownership | Audit every timeout/work procedure, registered callback, borrowed resource and cached display object. Callback arrays and Xlib-reported window lists must be checked against both `size_t` and Xt `Cardinal` limits. Repeated create/change/destroy cycles and callback-driven destruction must pass instrumented native tests. |
| Medium | DPI and monitor changes | Define an opt-in live scaling policy and test font/control metrics, geometry negotiation and monitor changes together. Explicit application sizes must remain predictable; changing all legacy pixel resources implicitly would be disruptive. |
| Medium | Image usability | Consider scale-aware image lookup and optional modern image/vector decoding behind clear ownership/cache APIs. Test alpha, masks, symbolic colors and cache invalidation at several sizes before adding formats. Existing XPM safety comes first. |
| Medium | Keyboard and focus | Review traversal, popup dismissal, modality, menu accelerators, repeat behavior and visible focus using native keyboard/pointer dispatch and destruction during callbacks. |
| Medium | Accessibility completeness | Continue native list/menu/table semantics and precise editing notifications with a real external observer and assistive client. Provider availability alone is insufficient. |
| Medium | Portability and maintenance tooling | Qualify x86_64 as well as ARM64, optional feature-off builds, current compiler diagnostics, installed C/C++ consumers, deterministic generators and malformed resource/image inputs. |

A general replacement of X11 geometry, a complete Unicode conformance claim or
an incompatible default scaling policy needs a separate design and evidence.
The immediate maintenance path is concrete fixes plus reusable tests, followed
by the next reproducible issue in these work packages.

## TreeView typography and selection

TreeView labels now use the active X font's ascent, descent, and measured width
for their baseline and RTL placement.  This keeps text readable when an
application supplies a larger or smaller font and avoids the former fixed
eight-pixel width estimate.  `XmTreeViewSetSelectedId` also scrolls a visible
selected node into view, which makes programmatic navigation behave like the
existing keyboard path.  The focused source check is:

```sh
python3 tests/automated/check-treeview-text-metrics.py
```

DataGrid headers and cells use the same font-metric approach for their
baselines; fixed sixteen-pixel baselines previously clipped text in short rows
and left larger fonts visually off-centre.  The focused check is:

```sh
python3 tests/automated/check-datagrid-text-metrics.py
```

Calendar headings, weekday labels, and day numbers now use measured font
baselines as well.  The focused check is:

```sh
python3 tests/automated/check-calendar-text-metrics.py
```

NumberBox and Stepper value text use the same measured-baseline rule, keeping
compact numeric controls aligned with adjacent labels at different font sizes.
The focused check is:

```sh
python3 tests/automated/check-number-stepper-text-metrics.py
```

Skeleton loading placeholders now use the theme's shadow colors for a muted
base and a moving highlight stripe.  Reduced-motion and explicit stop/start
semantics remain unchanged.  The focused source check is:

```sh
python3 tests/automated/check-skeleton-rendering.py
```

ColorPicker's hexadecimal value label now uses the active font's measured
baseline, keeping it vertically aligned beside the swatch at different font
sizes.  The focused check is:

```sh
python3 tests/automated/check-colorpicker-text-metrics.py
```

DatePicker's ISO date value uses the same measured-baseline rule.  The focused
check is:

```sh
python3 tests/automated/check-datepicker-text-metrics.py
```

Badge labels and counts now use an owned copy of `XmNrenderTable` for extent
measurement and drawing. The focused check is:

```sh
python3 tests/automated/check-badge-text-metrics.py
```

InfoBar text now uses an owned render table for prefix, message, and action
extents/drawing; it no longer frees a live font during Redisplay. The focused
checks are:

```sh
python3 tests/automated/check-infobar-font-guard.py
python3 tests/automated/check-info-bar-text-bounds.py
```

Link labels and their visited-state rules now use measured font geometry.  The
focused check is:

```sh
python3 tests/automated/check-link-text-metrics.py
```

Chip labels now use measured active-font width and ascent/descent for both
preferred geometry and drawing.  The focused check is:

```sh
python3 tests/automated/check-chip-text-metrics.py
```

NumberBox now draws value text through an owned copy of its `XmNrenderTable`,
so application font resources and localized metrics are used consistently. The
focused check is:

```sh
python3 tests/automated/check-numberbox-font-fallback.py
```

Stepper now draws its value through an owned copy of `XmNrenderTable`, keeping
numeric text aligned with the active Motif typography. The focused check is:

```sh
python3 tests/automated/check-stepper-font-fallback.py
```

ColorPicker now draws its value through an owned copy of `XmNrenderTable`,
keeping color text aligned with active Motif typography. The focused check is:

```sh
python3 tests/automated/check-colorpicker-font-fallback.py
```

Link now draws labels through an owned copy of `XmNrenderTable`, preserving
active typography and localized metrics. The focused check is:

```sh
python3 tests/automated/check-link-font-fallback.py
```

Chip now draws labels through an owned copy of `XmNrenderTable`, preserving
active typography and localized metrics. The focused check is:

```sh
python3 tests/automated/check-chip-font-fallback.py
```

Avatar now installs a managed fallback font for its initials GC.  The focused
check is:

```sh
python3 tests/automated/check-avatar-font-fallback.py
```

TreeView disclosure indicators now center themselves within the configured row
height instead of relying on a fixed offset.  The focused check is:

```sh
python3 tests/automated/check-treeview-text-metrics.py
```

Wide-text modify-verify callbacks now bound their byte allocation and reject
`wcstombs()` results that cannot fit Motif's text-block length.  The focused
check is:

```sh
python3 tests/automated/check-text-wide-conversion.py
```

DataField selection exports now validate `XmTextPosition` ranges before passing
them to the legacy `int` byte-count helper.  The focused check is:

```sh
python3 tests/automated/check-datafield-selection-span.py
```

TextField selection exports apply the same range validation before calling
their legacy byte-count helper.  The focused check is:

```sh
python3 tests/automated/check-textfield-selection-span.py
```

TreeView labels now use an owned copy of the widget render table for extent
measurement and drawing, including localized `XmString` content.  The focused
check is:

```sh
python3 tests/automated/check-treeview-text-metrics.py
```
Prompt text now uses an owned copy of `XmNrenderTable` for drawing and
preferred-width measurement. The focused checks are:

```sh
python3 tests/automated/check-prompt-text-bounds.py
python3 tests/automated/check-prompt-resource-safety.py
```

I18List GC creation now tolerates Xft-only render tables without dereferencing
an absent `XFontStruct`. The focused check is:

```sh
python3 tests/automated/check-i18list-xft-gc.py
```
