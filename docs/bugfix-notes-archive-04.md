---
nav_exclude: true
---
# Archived Bugfix and Hardening Notes (Part 4)

Consolidated archive of standalone bugfix and hardening validation notes.

## Icon visibility guards

`IconVisible` now returns `False` when client, screen, icon-box, clip-widget,
or scrolled-widget state is incomplete.  Initialized icon boxes continue to
use the existing geometry visibility checks.

`tests/automated/check-icon-visible-guards.py` protects the contract.

## IconBox cell-span bounds

IconBox hit testing divides pointer coordinates by the cell size plus its
margin.  Applications may set those `Dimension` resources to zero, and a
wrapped or zero sum previously made `GetCellFromXY` divide by zero.  The
conversion now computes the spans in an unsigned type and substitutes a
one-pixel span when a sum is zero, keeping pointer hit testing defined for
degenerate resource values.

`tests/automated/check-iconbox-cell-span.py` keeps the nonzero-span guard in
the coordinate conversion path.

## IconBox cell-query inputs

`XmIconBoxIsCellEmpty` now rejects a null widget before entering the
application context. Invalid optional handles return `False`; valid cell
occupancy queries are unchanged.

The focused regression check is `tests/automated/check-iconbox-inputs.py`.

## Icon-box resize guards

`CheckIconBoxResize` now rejects missing clip-widget or scrollbar-policy state,
invalid placement dimensions, and negative content sizes before resizing.  Valid
resize behavior remains unchanged.

`tests/automated/check-iconbox-resize-guards.py` protects the contract.

## Icon-box size-check guards

`CheckIconBoxSize` now returns `False` when its icon-box widget, client data,
or cell increments are unavailable, avoiding invalid Xt queries and division
by zero.  Valid resize checks remain unchanged.

`tests/automated/check-iconbox-size-guards.py` protects the contract.

## Mwm icon-box title conversion

The icon-box shell now supplies `XmNtitle` and `XmNiconName` only when the
workspace title converts successfully from `XmString`.  A malformed title no
longer creates a shell with null title resources; the shell still opens with
its normal defaults.  Valid titles and the existing resource values are
unchanged.

`tests/automated/check-iconbox-title-conversion.py` protects this fallback.

## IconButton argument-count bounds

`XmIconButton::SetValues` now scans its Xt argument list with a `Cardinal`
counter matching `num_args`. Resource-change detection and button behavior are
unchanged while large valid argument lists avoid signed-counter wraparound.

## IconButton text clipping geometry

IconButton clips label text against the available width and height after its
spacing margins.  Those bounds now subtract spacing in signed integers before
comparing with the text rectangle, so a narrow button cannot turn a negative
available size into a large wrapped clipping limit.

## IconButton label initialization rollback

IconButton initialization now checks the private copy of an explicitly
provided `XmNlabelString`.  If copying the compound string fails, it rebuilds
the caption from `XmNlabel` or the widget name instead of leaving the button
with a NULL visible label.

The focused regression is
`tests/automated/check-iconbutton-label-init-rollback.py`.

## IconButton label rollback

`XmIconButton` now copies changed label and label-string resources before
releasing their current values.  Allocation failure preserves the existing
label, while NULL remains an explicit clear value and the existing fallback
to the widget name remains intact.

The focused source regression is
`tests/automated/check-iconbutton-label-rollback.py`.

## IconButton pixel-cache allocation handling

IconButton’s pixel-cache insertion now checks the cache entry, list creation,
and list-element insertion allocations.  Partial entries and a newly-created
empty list are released on failure, so cache updates do not dereference null
state or leak a failed insertion.

The regression check is `tests/automated/check-iconbutton-pixcache-allocation.py`.

## IconG horizontal clip bounds

IconG now computes the main icon and detail clip widths in signed arithmetic,
clamping negative available space to zero before converting to `Dimension`.
Right-to-left clip origins are derived from the bounded width.  Collapsed
gadgets therefore avoid wrapped clip rectangles and accidental drawing across
the window.

The source-level regression is `tests/automated/check-icong-clip-bounds.py`.

## IconG clip Y origins

IconG applies highlight and margin dimensions to the gadget's Y position when
setting pixmap-mask and text clip rectangles. Those dimensions are unsigned,
so the old expression could wrap before reaching X when an icon was positioned
near an edge or had a narrow layout.

The four affected clip paths now promote each operand to `int` before the
addition and convert the bounded result at the `XRectangle` boundary. Existing
horizontal clipping and public behavior remain unchanged. The regression
`tests/automated/check-icong-clip-origins.py` covers all four paths.

## IconGadget detail-table rollback

`XmIconGadget` now copies `XmNdetail` tables through one checked helper.  The
helper validates the table byte count against both host `size_t` and Xt's
`Cardinal` range, releases partial `XmString` copies, and reports failure by
returning no table.  During `SetValues`, the old table is released only after
the requested replacement has been copied successfully; failed updates keep
the existing detail data intact.

The focused regression is
`tests/automated/check-icong-detail-table-rollback.py`.

## IconGadget geometry-trait validation

`XmIconGadget`'s private baseline, display-rectangle, and margin traits now
reject missing widget or output arguments before accessing gadget state.  The
display rectangle also reports zero width or height when a caller constrains an
IconGadget below twice its highlight thickness, rather than wrapping the
unsigned `Dimension` subtraction to a large rectangle.

Valid geometry and trait behavior is unchanged.  The focused source-level
regression is `tests/automated/check-icong-geometry-traits.py`.

## IconG vertical render bounds

IconG now computes pixmap copy heights and detail clip heights in signed
arithmetic and clamps negative available space to zero.  Collapsed gadgets
therefore avoid wrapped `Dimension` heights that could request oversized
copies or expose detail text outside the gadget.

The source-level regression is `tests/automated/check-icong-height-bounds.py`.

## IconG hook allocation handling

IconG's secondary-object, set-values, and get-values hooks now check every
extension-object and extension-data allocation.  Partial objects are released
before returning, and the get-values posthook tolerates a missing record when
its prehook could not establish temporary state.  Normal successful hook
behavior and cache ownership remain unchanged.

The regression check is `tests/automated/check-icong-hook-allocation.py`.

## IconGadget label initialization rollback

IconGadget now checks the private copy of an initial `XmNlabelString`.  If the
copy fails, it rebuilds the label from the gadget name and guards geometry
measurement when no fallback can be allocated, preventing NULL compound-string
use during construction.

The focused regression is
`tests/automated/check-icong-label-init-rollback.py`.

## IconG pixmap metadata guards

IconG now initializes pixmap depth before redisplay queries, so an unresolved
pixmap cannot select a copy operation using an undefined depth.  Background GC
creation only adds stipple or tile state for a positive resolved depth.  When
icon pixmaps change, their temporary width and height outputs are reset before
each query, preventing stale dimensions from being retained.

The focused regression is `tests/automated/check-icong-pixmap-metadata.py`.

## IconG reordered-table bounds

Icon gadget detail headers can be returned in an application-supplied order.
The order is one-based, but the old code accepted zero and indexed the source
table at `order[i] - 1`, reading before the table.  Its static reordered-table
buffer was also replaced without checking allocation failure.

Zero is now treated as an invalid order entry and produces a null output slot.
Buffer growth checks both host `size_t` and Xt `Cardinal` byte counts, and
commits the replacement only after allocation succeeds.

## IconG right-to-left icon placement

The exported IconG icon-position helper now computes right-to-left small and
large icon origins in signed arithmetic before returning a `Position`.  Icons
that exceed a narrow gadget remain off-edge instead of wrapping to a distant
positive coordinate.

The source-level regression is `tests/automated/check-icong-rtl-icon.py`.

## IconG right-to-left label placement

IconG now computes the right-to-left label origin in signed arithmetic before
converting it to `Position`.  Labels wider than a collapsed gadget remain
off-edge instead of wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-icong-rtl-label.py`.

## IconG right-to-left render placement

IconG's `Redisplay` routine now computes right-to-left large and small icon
origins in a signed temporary before converting them to `Position`.  This
keeps clipped and unclipped icons at their intended off-edge coordinates when
a gadget is narrower than its icon, instead of allowing unsigned arithmetic to
wrap the draw origin to the opposite side of the window.

The source-level regression is `tests/automated/check-icong-rtl-render.py`.

## IconGadget visual-resource rollback

`XmIconGadget` now obtains replacement render tables and label strings before
releasing the current values during `SetValues`.  A failed copy keeps the
existing resource, preventing an allocation failure from leaving the gadget
with a dangling or unusable visual resource.  Geometry measurement also skips
extent calculation when no render table or label is available.

The focused regression is
`tests/automated/check-icong-visual-resource-rollback.py`.

## Iconv output growth bounds

The UTF-8 resource converter grows its output buffer when `iconv` reports
`E2BIG`. The old code doubled the allocation without checking for `size_t`
overflow, and compared an unsigned length against zero as if it were signed.

The converter now rejects lengths that cannot fit the Xt allocator, checks each
doubling before reallocating, and frees the partial result on overflow.

`tests/automated/check-iconv-growth-bounds.py` guards these contracts.

## Iconv output-buffer growth bounds

The compound-text iconv helper starts with an Xt-sized output buffer and grows
it when conversion reports `E2BIG`.  Growth now rejects a doubling that would
exceed either `size_t` or Xt's `Cardinal` range, preventing a truncated
`XtRealloc` request while preserving normal conversion and retry behavior.

`tests/automated/check-iconv-output-bounds.py` protects the boundary.

## Image-array size validation

`XmImageArrayFits` now rejects a zero element size before dividing the maximum
representable allocation size.  Normal image-table checks remain unchanged,
while malformed or future direct calls cannot trigger division by zero.

The regression check is
`tests/automated/check-image-array-fit-inputs.py`.

## Decoded image background fallback

When SVG or PNG decoding needs a background but no explicit appearance color
is available, ImageCache now uses `WhitePixelOfScreen(screen)`. Explicit
application background resources remain authoritative. This removes the old
pixel-zero fallback that could produce unexpected black regions in ordinary
Motif image and dialog-icon rendering.

## Image-cache byte counter

The image conversion path that copies inverted image data can allocate a byte
buffer larger than `INT_MAX`. Its copy loop now uses `size_t`, matching the
checked byte count and preventing signed-counter wraparound on large images.

Normal image conversion and cleanup behavior are unchanged. The regression is
covered by `tests/automated/check-image-conversion-cleanup.py`.

## Image-cache cleanup callbacks

The public cache-cleanup entry point now returns immediately for a null screen.
GC and pixmap cache cleanup callbacks also return safely when the hash-table
entry, cleanup key, or cached screen is missing.  The same validation is used
by the optional debug printers.  This keeps display/print-shell teardown from
dereferencing incomplete cleanup state while preserving the normal removal
path for valid entries.

The regression check is
`tests/automated/check-image-cache-cleanup-callbacks.py`.

## Image-cache color callback inputs

The XPM image-cache callbacks now reject null displays, color structures,
screens, and pixel arrays (as well as non-positive pixel counts) before cache
or X11 access.  This keeps malformed image metadata and teardown-time cleanup
from dereferencing incomplete callback state.

The regression check is
`tests/automated/check-image-cache-color-inputs.py`.

## Image-cache format restoration

After a cached image is uploaded, temporary format conversion is now undone by
restoring the recorded original format directly. The cleanup no longer asserts
that the original format was specifically `XYPixmap`; valid bitmap conversion
behavior is unchanged and unusual metadata no longer aborts the caller.

The regression check is
`tests/automated/check-image-cache-format-restoration.py`.

## Image-cache GC creation inputs

`GetGCForPutImage` now rejects a null screen, null source image, or `None`
destination pixmap before consulting the GC cache or deriving a display.  This
keeps failed or incomplete image conversions from reaching `DisplayOfScreen`
and Xlib with invalid state.

The regression check is `tests/automated/check-image-cache-gc-inputs.py`.

## Image-cache pixmap creation failure

Scaled image construction temporarily locks shared image state when it needs
to normalize a cached depth-one image.  If `XCreatePixmap` fails, cleanup must
release that lock even when no replacement image buffer was allocated.  The
failure path now uses the same cleanup helper as other construction failures,
which restores any shared image state, releases cached pixels, frees the cache
record, and always releases the process lock that was acquired for conversion.

`tests/automated/check-image-cache-pixmap-failure.py` keeps this failure path
centralized so future changes cannot reintroduce the conditional unlock.

## Scaled image table cleanup

Scaled image rendering builds four coordinate tables for source pixels.  The
cleanup path now releases the x, y, width, and height tables exactly once.  A
previous typo released the y table twice and leaked the x table, so allocation
failure and normal rendering could either double-free memory or retain the
x-coordinate table.

The focused regression is
`tests/automated/check-image-cache-table-cleanup.py`.  It checks the cleanup
block in `_XmPutScaledImage` for one release of each table and rejects the
old duplicate-y pattern.

## Image-cache unsupported-format cleanup

Scaled pixmap creation now treats an unsupported `XImage` format as a normal
failure. It uses the existing cleanup path to restore image ownership, release
allocated pixels, and return `XmUNSPECIFIED_PIXMAP`; supported bitmap and
pixmap formats are unchanged.

The regression check is
`tests/automated/check-image-cache-unsupported-format.py`.

## ImageCache color-cache growth

The color cache grows in fixed-size batches.  Growth now validates the count
and byte-size arithmetic against both host `size_t` and Xt's `Cardinal` range,
keeps the old cache until the replacement allocation succeeds, and reports
failure to the color lookup path.  A newly allocated X11 color is released
when it cannot be cached, avoiding both a null write and a resource leak under
allocation pressure.

`tests/automated/check-image-color-cache-growth.py` protects this contract.

## ImageCache color reference counts

Cached colors are released by reference count.  A duplicate release or stale
cache entry must not wrap the unsigned counter to its maximum value, because
that would prevent the color from ever being removed and freed.  Releases now
reject an already-zero counter before decrementing it.

Lookups also refuse to increment a counter that has reached `UINT_MAX`.
This avoids wrapping a live reference count to zero, which could otherwise
allow a still-referenced color to be released. The same focused regression
checks both RGB and symbolic-name lookup paths.

Both lookup helpers also reject a missing display or output color record before
walking the cache, keeping malformed internal calls from dereferencing null
state.

`tests/automated/check-image-color-refcount.py` protects this invariant.

## Scaled-pixmap depth input

The scaled-pixmap entry point now rejects depth zero before image loading or
X11 resource creation. X11 pixmaps require a positive depth; rejecting this
malformed input keeps callers on the normal `XmUNSPECIFIED_PIXMAP` failure
path instead of provoking an avoidable server-side error.

Regression coverage: `tests/automated/check-image-depth-input.py`.

## Image installation failure rollback

`_XmInstallImage` now checks the hash-table insertion result. If bucket
allocation fails, it frees the copied image name and entry and reports failure
instead of claiming the image was installed while leaving an unreachable
allocation behind.

The focused regression is `tests/automated/check-image-install-failure.py`.

## JPEG and PNG image allocation bounds

The JPEG and PNG loaders validate decoded dimensions and row sizes before
forming pixel-buffer allocations.  This prevents width×height or rowbytes×height
wraparound from producing undersized buffers, and makes allocation failure a
normal image-load error instead of a later null-buffer dereference.  The
public `XmGetPixmap` image-loading behavior and decoded image formats remain
unchanged for valid images.

The native image regression suite covers malformed image handling and image
scaling after rebuilding the installed library.

## ImageCache pixmap reference counts

`XmDestroyPixmap` releases a cached pixmap by decrementing its reference
count.  A stale or duplicate release now rejects an already-zero or invalid
count before decrementing, preserving cache ownership and avoiding integer
underflow.

`tests/automated/check-image-pixmap-refcount.py` protects this lifecycle
invariant.

## Scaled image row offset types

The fast 8-bit scaling path forms source and destination row addresses for
each pixel.  Those products now use `size_t` before multiplication, matching
the checked allocation sizes and preventing 32-bit unsigned wraparound for
large valid strides or malformed dimensions.

`tests/automated/check-image-row-offset-types.py` protects these offset
contracts.

## Image-scaling helper outputs

The image-cache scaling helpers now reject null output pointers before writing
rounded dimensions.  This keeps malformed or future direct helper calls from
turning otherwise bounded scaling input into a null-pointer write.

The regression check is
`tests/automated/check-image-scale-output-pointers.py`.

## image-scaled-failure-cleanup
Avoid double-free during scaled-image rollback
===============================================

When scaled-pixmap construction fails after temporarily replacing an image
buffer, rollback restores the caller-owned `old_image_data`.  The failure
cleanup now detaches that restored pointer before destroying the `XImage`, then
frees it once with Xt.  Converted buffers continue to use the matching
allocator path.  This prevents a double-free in the uncached failure path.

The focused regression is:

```text
python3 tests/automated/check-image-converted-cleanup.py
```

## Image-scaling target depth

Scaled images are rendered into a temporary `XImage` before being copied to
the destination drawable.  The temporary image now uses the destination
drawable's server-reported depth as well as its screen visual.  This avoids a
visual/depth mismatch on displays whose drawable depth differs from the
source image.  If geometry cannot be queried, the source image depth remains
the compatibility fallback.

The focused regression is
`tests/automated/check-image-scaling-depth.py`.

## Image-scaling strip progress

Scaled images are copied in destination-height strips.  Rounding can collapse
several source rows to one destination coordinate for extreme dimensions.  A
final strip must still advance; if the row table reaches its end without
progress, the copy now exits through the normal cleanup path instead of
repeating the same strip indefinitely.

The focused regression is `tests/automated/check-image-scaling-progress.py`.

## Image-scaling target screen

Scaled images are rendered into a temporary `XImage` before being copied to
the destination drawable. The temporary image now uses the visual of that
drawable's screen, discovered from its root window, instead of always using
the display default. Single-screen displays retain the fast default-screen
path.

The focused regression is
`tests/automated/check-image-scaling-screen.py`.

## Image-scaling table bounds

The scaled-image copy loop walks the source-to-destination row table while
rendering each strip.  It now explicitly stops at `image_height`, rather than
depending only on the final rounded coordinate to terminate the loop.  This
keeps malformed or precision-edge scaling inputs inside the allocated table
even when a server reports unusual image dimensions.

The focused regression is
`tests/automated/check-image-scaling-table-bounds.py`.

## Built-in image-cache insertion failure

`InitializeImageSet` now treats a failed hash-bucket insertion as initialization
failure. It does not publish a partially populated built-in image table; the
existing cleanup path releases all allocated entries and the private hash
table, allowing a later retry to start from a clean state.

The focused regression is
`tests/automated/check-image-set-insertion-failure.py`.

## Scaled image source bounds

Scaled image conversion reads source pixels directly for the fast 8-bit path
and through Xlib helpers for other formats.  It now rejects an image with no
pixel data and rejects a height/row-stride product that cannot fit in
`size_t` before any row offset is formed.  Malformed images therefore fail
cleanly instead of dereferencing null data or wrapping a row address.

`tests/automated/check-image-source-stride-bounds.py` protects these checks.

## Image-cache string hash width

The ImageCache string hash now retains the full `strlen` width and folds any
high bits into the existing integer hash value. Long image names therefore do
not silently truncate their length before hashing, while ordinary names keep
the same compact cache-key behavior.

The focused regression is
`tests/automated/check-image-string-hash-width.py`.

Pixmap and GC cache keys also fold native-width pointer and XID values before
returning the legacy `XmHashValue` type. They no longer depend on signed
`long` arithmetic or implementation-defined narrowing on 64-bit systems.

## Image uninstall callback inputs

The image-cache uninstall hash callback now returns safely when the hash value
or requested image pointer is null, before dereferencing the cached image.
Valid uninstall operations retain their existing removal behavior.

The regression check is `tests/automated/check-image-uninstall-callback.py`.

## Image uninstall locking

`XmUninstallImage` serializes access to the image cache while it maps the
cache and removes the matching entry. The map callback therefore relies on
that caller-held `_XmProcessLock`; it must not acquire the same lock again.
This avoids a nested-lock deadlock on builds whose process lock is not
recursive, while preserving the existing cache serialization.

The regression check is
`tests/automated/check-image-uninstall-locking.py`.

## InfoBar coordinate bounds

InfoBar clamps prefix, message, and action-label positions before passing them
to Motif's signed `Position` drawing arguments.  Large widget dimensions or
font metrics therefore cannot wrap text to an unrelated screen coordinate.

`tests/automated/check-info-bar-coordinate-bounds.py` protects this drawing
contract.

## Info-bar text geometry bounds

`XmInfoBar` now bounds prefix positioning and text lengths before passing them
to Xlib. Normal messages retain their existing layout; unusually long strings
no longer wrap signed coordinates or length arguments.

The focused regression check is
`tests/automated/check-info-bar-text-bounds.py`.

## InfoBar close-button geometry

The InfoBar close button now uses an 18-pixel square only when the widget is
at least 24 by 18 pixels, keeping the frame and X glyph inside the widget.
Pointer activation uses the same geometry threshold and horizontal origin.
Font-list replacement is transactional when `SetValues` cannot copy the new
render table.

The focused source check is
`tests/automated/check-infobar-close-geometry.py`.

## InfoBar close icon geometry

The InfoBar close control now draws a centered, symmetric framed X. Both
diagonals use the same four-pixel inset around the 18-pixel button center, so
the icon remains balanced at every supported bar size.

The focused source contract is checked by
`tests/automated/check-infobar-close-icon.py`.

## InfoBar event guards

InfoBar now validates its widget type and event pointer before handling close,
action, and Escape input. Teardown or synthetic dispatch is ignored safely;
normal live-bar interactions are unchanged.

## InfoBar hit-zone bounds

InfoBar reserves fixed-width close and action zones.  On a narrow widget, the
old `width - zone` comparisons became negative and treated almost every
button press as a command.  The event handler now clamps each zone origin to
zero, so the full narrow bar is a deliberate target only when its corresponding
control is present.

`tests/automated/check-infobar-hit-zones.py` keeps the clamped hit-zone
comparisons in the event path.

## Icon placement initialization guards

`InitIconPlacement` now rejects missing workspace data and non-positive icon
dimensions before calculating screen grid sizes.  It disables automatic icon
placement through the existing failure path instead of dividing by zero.

`tests/automated/check-init-placement-guards.py` protects this startup path.

## Input-method locale modifier bounds

`get_xim_info` applies the optional `XmNinputMethod` resource by prepending
`@im=` and calling `XSetLocaleModifiers`. The old code concatenated the
application string into a fixed `BUFSIZ` stack buffer and asserted after the
write.

The modifier string is now allocated from the input length with Xt ownership,
with an allocator-size check before construction. The existing locale modifier
value is preserved for representable inputs; an unrepresentable value is
ignored safely.

`tests/automated/check-input-method-modifier-bounds.py` guards this contract.

## Icon insertion geometry guards

`InsertIconInfo` now rejects incomplete icon-box geometry and missing widgets
before placement insertion.  This prevents divide-by-zero and invalid Xt
accesses when initialization has not produced usable increment or widget state.

`tests/automated/check-insert-icon-geometry-guards.py` protects the contract.

## Icon insertion guards

`InsertIconInfo` now rejects missing icon-box or client state and verifies that
placement remains available after a growth attempt.  This prevents invalid
placement indices from reaching the icon-info array while preserving normal
insertion and expansion behavior.

`tests/automated/check-insert-icon-info-guards.py` protects the contract.

## Insert-selection request guard

The insert-selection conversion callback now returns `False` when Xt cannot
provide a selection request event.  This is valid when the callback is
invoked outside an active conversion request and avoids dereferencing a null
event before the existing serial and property handling.

Regression: `tests/automated/check-insert-selection-request-guard.py`

## XmInstallImage locking

`XmInstallImage` now delegates directly to `_XmInstallImage`, which owns the
process-locking and allocation contract. The public wrapper no longer acquires
the same process lock around the synchronized helper, avoiding a deadlock when
Xt is configured with a non-recursive process lock.

`XmGetPixmap` follows the same rule: it calls the synchronized scaled-pixmap
helper directly instead of calling `XmGetPixmapByDepth` while holding the
application lock. This prevents a second lock acquisition on the public
compatibility wrapper.

`XmeGetMask` follows the same rule when converting the generated mask name.
It now invokes the synchronized helper directly rather than reacquiring the
application lock through `XmGetPixmapByDepth`.

The focused regressions are `tests/automated/check-xm-install-locking.py` and
`tests/automated/check-xm-get-pixmap-locking.py`.

The same ownership rule applies to text-field insertion: the
`XmTextFieldInsert` wrappers do not acquire the application lock before
delegating to the public replacement wrappers, which already own that lock.
This is covered by `tests/automated/check-textfield-insert-locking.py`.

Command-dialog text convenience wrappers follow the same ownership rule:
`XmCommandSetValue` delegates to `XmTextFieldSetString`, and
`XmCommandAppendValue` delegates to the text-field getters/replacement APIs,
while `XmCommandError` delegates to the public list APIs, without taking an
outer application lock. Their focused check is
`tests/automated/check-command-locking.py`.

`XmTextFieldSetStringWcs` also unlocks before delegating to the public narrow
setter. Invalid wide-character conversion now clears the allocated buffer
instead of replacing it with, and later freeing, a string literal. The
conversion result is retained as `size_t`; the regression is
`tests/automated/check-textfield-wide-set-locking.py`, which also verifies the
temporary conversion allocation is checked.

Selection extraction now checks position ordering, `Cardinal`-sized allocation
limits, conversion-output bounds, and allocator results for both narrow and
wide output. The focused source check is
`tests/automated/check-textfield-selection-allocation.py`.

The full string getters apply the same checked allocation and conversion
termination rules; their focused check is
`tests/automated/check-textfield-string-allocation.py`.

That check also covers the empty-wide-string allocation path, which now
returns `NULL` cleanly if the one-character allocation fails.

The wide modify-verify path in `XmTextFieldSetString` now uses checked
`size_t` arithmetic and handles temporary-buffer allocation failure; this is
covered by `tests/automated/check-textfield-set-string-allocation.py`.

The post-callback converted buffer uses the same checked allocation and
cleanup path, including failed `wcstombs()` conversion.

The setter also rejects a narrow or multibyte input whose byte count cannot be
represented by its callback `int` length before modifying the widget. This is
covered by `tests/automated/check-textfield-set-string-range.py`.

`XmTextFieldReplace` now applies the same checked conversion-buffer policy to
wide-to-multibyte and multibyte-to-wide replacement. It checks allocation and
size arithmetic, preserves conversion counts as `size_t` until they are known
to fit the public `int` length, and clears an allocated buffer on invalid input
instead of freeing a string literal. The focused source check is
`tests/automated/check-textfield-replace-allocation.py`.

Conversion result counts are retained as `size_t` through the getter and
substring paths, so large successful conversions cannot wrap through an
intermediate `int`.

Substring extraction now saturates a source range larger than the public
`int` count at `INT_MAX` and reports truncation instead of narrowing a large
`XmTextPosition`. The focused check is
`tests/automated/check-textfield-substring-range.py`.

The multibyte byte-count helper also saturates instead of wrapping its `int`
result; this is covered by
`tests/automated/check-textfield-byte-count-bounds.py`.

## Resource integer parser overflow handling

`isInteger` now bounds-checks each digit against the signed `int` range before
accumulation, including the valid `INT_MIN` case. Overlong or out-of-range
values return `False` without signed overflow or undefined behavior.

The focused regression check is
`tests/automated/check-integer-parser-overflow.py`.

## JPEG XImage bounds

The JPEG decoder now rejects decoded dimensions that cannot be represented by
the signed `XImage` fields before allocating or constructing an image.  It also
rejects an invalid root depth before converting it to Xlib's unsigned API.

Grayscale expansion uses libjpeg's unsigned `JDIMENSION` counters and a
pre-decrement index, avoiding a signed conversion or an underflow at the start
of a row.  Ordinary supported JPEGs retain the same output pixels and layout.

## Icon-box keypress callback guards

`HandleIconBoxIconKeyPress` now returns when its icon widget or key event is
missing, before reading the widget window or writing event fields.  Valid key
handling remains unchanged.

`tests/automated/check-keypress-callback-guards.py` protects the contract.

## KeySym resource conversion allocation

The string-to-KeySym-table resource converter allocates a table and a mutable
copy of the input string. Both allocations are now checked. If either fails,
the converter returns `False` and releases any table already allocated, while
successful parsing and invalid-name cleanup remain unchanged. Input length and
comma-count growth are validated before either Xt `Cardinal` allocation, so an
unrepresentable resource string cannot truncate the mutable copy or table.

## Unicode mnemonic keysyms

Label and label-gadget mnemonics now recognize the X11 Unicode keysym form
(`0x01000000 | UCS`) and encode valid Unicode scalar values as UTF-8. Legacy
keysyms retain the historical low-byte behavior, while invalid scalar values
are rejected safely.

The Label and LabelG drawing paths reserve five bytes for this conversion:
four for the largest UTF-8 scalar and one terminating NUL.  They therefore do
not depend on a platform's `MB_LEN_MAX` when a non-BMP mnemonic is rendered.

Regression check:

```text
python3 tests/automated/check-keysym-unicode-encoding.py
```

## Label accelerator allocation

Menu labels own a private copy of their accelerator string.  `SetValues` now
publishes a replacement only after `XtNewString` succeeds; if allocation fails,
the currently installed accelerator remains active and the requested binding
is discarded without freeing the valid old copy.  The `GetValues` hook also
returns `NULL` directly when a label has no accelerator instead of passing a
null pointer to `XtNewString`.

Successful accelerator replacement and the existing non-menu behavior are
unchanged.  This keeps menu bindings usable during allocation pressure and
makes querying an absent accelerator safe.

## Label baseline allocation

`XmLabelGetBaselines` caches an extra baseline offset and returns a
caller-owned copy. Cache growth now uses a temporary pointer with checked
size arithmetic, and the returned copy is checked before it is filled.

Missing or unrepresentable baseline data returns `False`; an empty baseline
set returns success with a null copy. Successful baseline values and offsets
remain unchanged.

## Label baseline count bounds

Label and LabelGadget cache one extra baseline offset after the baselines
returned by the compound-string code.  Their overflow check now tests the
original `Cardinal` count before adding that extra element, and checks both
host `size_t` and Xt's `Cardinal` allocation width.  This avoids the old
`count + 1` wraparound being evaluated in the narrow `Cardinal` type and then
incorrectly accepted for reallocation.

The offset-adjustment loop also uses a `Cardinal` index, so a valid wide
baseline table cannot wrap a signed counter while applying the cached delta.

Counts that cannot be returned through the historical `int *line_count` API
are rejected before the `Cardinal` value is narrowed.  The same validation is
also applied when the cached-baseline path recounts the compound string, so a
changed or malformed label cannot bypass the allocation and indexing bounds.

Normal baseline measurement and the public `XmWidgetGetBaselines` behavior are
unchanged.  The allocation regressions are covered by
`tests/automated/check-label-baseline-allocation.py` and
`tests/automated/check-labelg-baseline-allocation.py`.

## Label geometry-trait output validation

`XmLabelGetBaselines` and `XmLabelGadgetGetBaselines` reject a null widget or
output pointer before accessing widget state. Valid baseline calculation and
caching remain unchanged, while malformed generic geometry queries no longer
crash one class while succeeding safely for the other. The Label path also
rejects an invalid negative line count before converting it to its internal
unsigned count.

The matching display-rectangle traits now apply the same null widget and
`XRectangle` output validation, so generic geometry clients get consistent
safe failure from Label and LabelGadget.

The focused regression check is
`tests/automated/check-label-baseline-outputs.py`.

## Label conversion allocation guards

Label and LabelGadget drag conversion now checks target-list, pixmap,
background, and foreground result allocations. Allocation failure reports
`XmCONVERT_REFUSE` before any result is dereferenced or published.

Locale conversion also measures the caller's source string before invoking the
converter; it never reads the result pointer before that pointer is populated.

The focused regression is
`tests/automated/check-label-conversion-allocation.py`, with the source-length
contract covered by `tests/automated/check-label-locale-length-source.py`.

## Label gadget display-rectangle validation

`XmLabelGadgetGetDisplayRect` now rejects a null widget or output rectangle
before reading geometry. Valid display rectangles are returned unchanged,
while malformed geometry queries no longer crash.

The focused regression check is
`tests/automated/check-label-display-rect-outputs.py`.

## Label pixmap drag-icon input validation

`GetPixmapDragIcon` now returns no icon for a null widget before querying its
screen or pixmap. Normal Label pixmap drag-icon creation is unchanged.

The focused regression check is
`tests/automated/check-label-drag-icon-input.py`.

## Label drag-icon pixmap metadata

Label and LabelG drag-icon creation now initializes pixmap width, height, and
depth outputs before querying metadata.  An invalid or unresolved pixmap thus
produces deterministic zero metadata rather than arbitrary drag-icon resource
values; valid pixmaps retain their existing dimensions and depth.

The focused regression is
`tests/automated/check-label-drag-pixmap-metadata.py`.

## Label gadget accelerator allocation

Menu label gadgets now replace their private accelerator copy only after
`XtNewString` succeeds.  If a replacement cannot be allocated, the existing
binding remains installed and the requested value is discarded.  The
accelerator `GetValues` hook returns `NULL` directly when no accelerator is
present, avoiding a null string copy.

Successful replacements, non-menu gadgets, and the public LabelGadget ABI are
unchanged.

## Label initialization rollback

`XmLabel` now also checks the initial copy of a valid menu accelerator string.
If that copy cannot be allocated, the original valid accelerator remains in
place instead of being replaced by NULL.

The core Label widget now checks the private copy of a valid initial
`XmNlabelString`.  If copying fails, it rebuilds the caption from the widget
name instead of leaving a NULL label that later baseline and geometry code
could dereference.

The focused regression is
`tests/automated/check-label-init-rollback.py`.

## Label layout geometry

Label and LabelG center label and accelerator text by subtracting margins,
highlight/shadow insets, and text extents from unsigned toolkit dimensions.
The old expressions performed the subtraction before converting to `int`, so
small or temporarily collapsed labels could wrap to large positive offsets.
Accelerator X placement had the same mixed signed/unsigned problem.

The layout paths now convert each geometry term before subtraction. Normal
labels retain their existing positions while small labels and gadgets remain
stable during resize and realization.

## Label gadget margins input validation

`XmLabelGadgetMarginsProc` now treats a missing widget or margins record as a
safe no-op. Valid baseline-margin reads and writes retain their existing
behavior.

The focused regression check is
`tests/automated/check-label-margins-inputs.py`.

## Label padding hook guards

Label's orientation-dependent padding converters now validate widget and value
inputs before reading pixmap placement or forwarding unit conversion. Invalid
imports return `XmSYNTHETIC_NONE`; invalid exports return without touching
caller storage.

The focused source-contract check is:

```sh
python3 tests/automated/check-label-padding-hook-guards.py
```

## Label pixmap metadata

Label redisplay now copies sensitive and insensitive pixmaps only after
`XmeGetPixmapData` succeeds.  The insensitive stipple fallback is inside the
same success path, so an unresolved pixmap cannot be interpreted through an
uninitialized depth or used for a fill operation.

Successful depth-matched copies and bitmap planes retain their existing
behavior.  The focused regression is
`tests/automated/check-label-pixmap-metadata.py`.

## Label resize bounds

Label natural geometry, accelerator margins, and text/accelerator drawing
coordinates are now calculated in wide types and clamped explicitly at the
legacy `Dimension` and `Position` boundaries. Large compound strings, fonts,
or margins no longer silently wrap a Label's requested size or drawing origin.

The focused regression is `tests/automated/check-label-resize-bounds.py`.

## Label synthetic getter guards

Label's synthetic label, mnemonic, accelerator, and accelerator-text exports
now validate the output pointer and widget class before reading private Label
state. Invalid callback inputs return a deterministic null value; valid Label
subclasses retain their copied-string behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-label-synthetic-getter-guards.py
```

## Label visual-string rollback

`XmLabel` now copies replacement label and accelerator strings before freeing
the current widget-owned values.  If an `XmString` allocation fails, the
current text or accelerator remains installed and the requested external
pointer is discarded without leaving a partially updated resource.

The focused regression is
`tests/automated/check-label-visual-string-rollback.py`.

## Label wide-string value conversion

`LabelSetValue` now checks the temporary byte-buffer allocation and the
`wcstombs` result before constructing an `XmString`. Allocation or invalid
locale conversion failure releases the input and returns without updating the
label, while valid conversions retain existing behavior.

## LabelG background-box bounds

LabelG now computes its background-box origin and inset width/height in signed
arithmetic before `LabelDrawBackground` clamps negative spans.  Collapsed
labels therefore avoid unsigned wraparound in the fill rectangle.

The source-level regression is `tests/automated/check-labelg-background-bounds.py`.

## LabelG background pixmap metadata

LabelG background GC setup now adds stipple or tile fields only when
`XmeGetPixmapData` successfully resolves the parent background pixmap.  A
failed lookup falls back to the ordinary foreground/background GC instead of
using an uninitialized depth or an invalid pixmap resource.

Successful bitmap and pixmap metadata retain their existing opaque stipple and
tile behavior.  The focused regression is
`tests/automated/check-labelg-background-pixmap-metadata.py`.

## LabelGadget baseline allocation

`XmLabelGadgetGetBaselines` caches one extra baseline offset and returns a
caller-owned copy. Cache growth now uses a temporary pointer and rejects
missing or unrepresentable baseline data; the returned copy is also checked
before it is filled. Empty baseline sets return success with a null copy.

Allocation failure leaves the gadget cache in a safe state and returns
`False`, preserving the existing class-method contract.

## LabelG clip origins

LabelG narrow-layout clip origins now add highlight, shadow, and margin
insets in signed arithmetic before conversion to the X rectangle coordinate.
Labels positioned partly outside a parent therefore preserve their intended
off-edge clip origin.

The source-level regression is `tests/automated/check-labelg-clip-origins.py`.

## LabelG get-values allocation handling

`GetValuesPrehook` creates a temporary cache extension object and pushes an
extension-data record for the matching posthook.  The prehook now checks both
allocations and releases a partial object on failure.  The posthook also
accepts an absent record, so an allocation failure cannot turn into a null
 dereference while unwinding the get-values operation.

The regression check is `tests/automated/check-labelg-getvalues-allocation.py`.

## LabelGadget label initialization rollback

`XmLabelGadget` now checks its initial copies of valid label and menu
accelerator strings.  A failed label copy falls back to the widget name when
that can be converted; otherwise the original valid input remains in place.
A failed accelerator copy likewise retains the original string, avoiding NULL
propagation during construction and subsequent cleanup.

The focused regression is
`tests/automated/check-labelg-init-rollback.py`.

## LabelGadget padding hook guards

LabelGadget's orientation-dependent padding converters now validate widget and
value inputs before reading pixmap placement or forwarding unit conversion.
Invalid imports return `XmSYNTHETIC_NONE`; invalid exports return without
touching caller storage.

The focused source-contract check is:

```sh
python3 tests/automated/check-labelg-padding-hook-guards.py
```

## LabelG pixmap metadata

LabelG redisplay now copies sensitive and insensitive pixmaps only after
`XmeGetPixmapData` succeeds.  The insensitive stipple fallback is inside the
same success path, so an unresolved pixmap cannot be interpreted through an
uninitialized depth or used for a fill operation.

Successful depth-matched copies and bitmap planes retain their existing
behavior.  The focused regression is
`tests/automated/check-labelg-pixmap-metadata.py`.

## LabelG secondary-object allocation

LabelG creates secondary and request extension objects before fetching cache
resources. The setup now checks both objects and the extension-data record,
releasing partial allocations before returning. Cache pointers are only updated
after all required storage exists.

## LabelG set-values allocation handling

`SetValuesPrehook` prepares replacement and request-side cache extension objects
before applying resource changes.  Both extension-object allocations and the
widget extension-data record are now checked.  If allocation fails, any
partially created objects are released and the hook leaves the existing cache
untouched, preserving the normal `FALSE` set-values result without dereferencing
null state.

The regression check is `tests/automated/check-labelg-setvalues-allocation.py`.

## LabelGadget SetValues rollback

LabelGadget now retains its current label or accelerator text when copying a
changed valid `XmString` fails.  The previous value is no longer freed while
the replacement is unavailable, preventing labels and menu accelerators from
disappearing during transient allocation pressure.

The focused regression is
`tests/automated/check-labelg-setvalues-rollback.py`.

## LabelGadget synthetic getter guards

LabelGadget's synthetic label, mnemonic, accelerator, and accelerator-text
exports now validate the output pointer and gadget class before reading cached
compound-string state. Invalid callback inputs return deterministic null values;
valid LabelGadget subclasses retain their copied-string behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-labelg-synthetic-getter-guards.py
```

## LabelG text geometry bounds

LabelG now computes the right text edge in signed arithmetic before applying
alignment.  Narrow labels whose insets exceed their width therefore keep a
bounded negative/off-edge coordinate instead of wrapping to a distant positive
position.

The source-level regression is
`tests/automated/check-labelg-text-geometry-bounds.py`.

## Legacy translation-buffer bounds

The compatibility `_XmGetRealXlations` path now rejects a used-length value at
`SIZE_MAX` before evaluating the append subtraction.  This matches the modern
ClipWindow translation builder and prevents boundary wraparound while
retaining the existing fail-closed allocation behavior.

`tests/automated/check-real-xlations-growth.py` verifies both translation
builders' width and growth guards.

## Link coordinate bounds

Link clamps its label coordinates before passing them to Motif's signed
`Position` drawing arguments.  Large widget heights or font metrics therefore
cannot wrap the label to an unrelated screen coordinate.

`tests/automated/check-link-coordinate-bounds.py` protects this drawing
contract.

## Link event guards

Link validates its widget type and event pointer before processing pointer or
keyboard activation events. Invalid dispatch is ignored safely while normal
activation and callback delivery remain unchanged.

## Link geometry and resource guards

`XmLink` now clamps underline rules to the widget width, skips focus frames
that cannot fit, and keeps the existing render table when a `SetValues` copy
fails. The active render-table metrics remain the source for label geometry.

The focused source check is `tests/automated/check-link-text-metrics.py`.

## Link activation API inputs

`XmLinkActivate` now rejects a null widget before checking sensitivity or
reading the widget's destruction state. Invalid optional handles remain a
no-op; valid activation, visited-state, and callback behavior is unchanged.

The focused regression check is `tests/automated/check-link-inputs.py`.

## Link resource and drawing safety

`XmLink` now skips redisplay until its window and private GC are available.
Label and target replacements release old owned values only after successful
copies; failed copies retain the prior values. Valid rendering, activation,
and resource behavior are unchanged.

`tests/automated/check-link-resource-safety.py` protects these paths.

## List base-position bounds

List now clamps its margin, highlight, and shadow sums before storing BaseX
and BaseY in Motif's signed `Position` fields.  Initialization, redraw, and
scrollbar-visibility paths use the same bounded conversion, preventing large
application margins from wrapping content geometry.

`tests/automated/check-list-base-position-bounds.py` protects this contract.

## List button-transfer fallback

List button-transfer actions now ignore invalid `enableBtn1Transfer` resource
values safely. The documented `XmOFF`, `XmBUTTON2_TRANSFER`, and
`XmBUTTON2_ADJUST` values retain their existing behavior; only malformed values
avoid an assertion-driven process termination.

The regression check is
`tests/automated/check-list-button-transfer-fallback.py`.

## List callback item-copy failure

List selection and default-action callbacks now require a successful private
copy of the primary item string.  If allocation fails, the interaction is
cleanly abandoned and its down-count is reset rather than dispatching a
callback with a NULL item or borrowing storage that may be changed by the
callback.

The focused regression is
`tests/automated/check-list-selection-callback-allocation.py`.

## List clear geometry

List highlight clipping and empty-list cleanup derive rectangles by subtracting
insets from unsigned widget dimensions. A collapsed list could wrap the clip
height or clear width and ask X to process a much larger region than the
widget.

Both paths now calculate available spans in signed integers and skip empty
clear operations when no positive area remains. Normal highlight and cleanup
behavior is unchanged for usable list sizes.

## List click-state guard

List selection handling now checks that the internal item table exists before
building a selection callback. A missing table is treated as a no-op, which
avoids assertion failure during widget teardown or reentrant callbacks while
leaving normal click behavior unchanged.

The regression check is `tests/automated/check-list-click-state.py`.

## XmList conversion callback guards

`ListConvert` now validates its widget and conversion callback pointers before
querying the encoding atom or reading drag state. Invalid callback dispatch
returns safely; normal list conversion keeps the same target and payload
behavior.

## list-copy-allocation-bounds
List selection-copy allocation bounds
=====================================

List item and selection-copy helpers now check both `size_t` and Xt's
`Cardinal` byte limits before allocating `XmString` pointer arrays.  This
prevents a large internal count from producing a byte size that the Xt
allocator interface cannot represent while preserving the existing
allocation-failure behavior.

## List copy allocation bounds

List copy-on-write helpers now validate count products and allocation results
before copying items or selected positions. A failed copy leaves the original
array owned by the widget instead of dereferencing a null temporary buffer.

The source-level regression is
`tests/automated/check-list-copy-bounds.py`.

## XmList copy-helper rollback

The `XmList` initialization and resource-update paths copy caller-provided
item, selected-item, and selected-position arrays into widget-owned storage.
The item helpers now release every earlier `XmString` copy if a later copy
fails, clear the failed resource, and reset its count instead of publishing a
partially initialized table.  Selected-position byte counts are checked both
against host `size_t` and Xt's `Cardinal` allocation range before conversion.
All three copy-helper byte counts are passed explicitly as Xt `Cardinal`
values after those checks.

The focused regression is
`tests/automated/check-list-copy-helper-rollback.py`.

## XmListDeleteItems copy failure handling

`XmListDeleteItems` now treats its temporary item snapshot transactionally. If
an `XmString` copy fails, already-copied entries are released, the temporary
array is freed, the application lock is restored, and deletion returns before
any null snapshot entry is inspected.

## List deletion widget guards

The six public List deletion APIs now reject non-List widgets before entering
application context or reading list-private mutation state. Valid deletion
behavior and historical position handling are unchanged.

The source contract is checked by
`tests/automated/check-list-delete-widget-class.py`.

## XmList deletion allocation failures

The list deletion helpers compact their arrays and then ask Xt to shrink the
allocation.  The previous code stored `XtRealloc`'s return value directly in
the widget.  A failed shrink therefore discarded the still-valid pointer and
left later list operations with a null or dangling array.

Each deletion path now computes the byte count with a checked `size_t`
conversion and commits the returned pointer only when it is non-null.  This
includes both public item deletion and position-based internal-element
deletion.  If a shrink cannot be satisfied, the larger allocation is retained;
the compacted list and its public count remain valid, preserving existing
Motif behavior.

## List drag conversion allocation

List drag initiation now checks the conversion record and temporary string-array
allocations before publishing `drag_conv`. Allocation failure cancels that drag
attempt and clears the widget pointer, leaving later drag attempts consistent.

The source-level regression is
`tests/automated/check-list-drag-allocation.py`.


The selected-position string-array product is checked before multiplication,
so a representationally impossible selection count cancels the drag without
wrapping the allocation size.

## XmList Motif-drop concatenation guards

Motif-drop conversion now handles a missing drag payload, null item entries,
separator-copy failure, and compound-string concatenation failure without
dereferencing or leaking partial data. Valid payloads keep their original
order and separators.

## XmList drag selected-position validation

Drag conversion now treats selected-position arrays as untrusted state. It
skips zero, negative, out-of-range, and null-item entries, and releases
partially built compound strings when an `XmString` copy fails. Valid selected
items retain their original order and separator behavior.

## List drag selection bounds

List drag-and-drop snapshots are built from the widget's selected-position
array.  That array can contain stale entries during selection updates, so the
snapshot now validates every one-based position against the current item
count, compacts the valid entries, and falls back to the item that initiated
the drag when no valid selected position remains.  Valid multi-selection and
the public drag protocol are unchanged.

The source contract is checked by
`tests/automated/check-list-drag-selection-bounds.py`.

## List drag snapshot rollback

List drag initiation now checks every selected-item `XmStringCopy`.  A failed
copy releases already-copied entries, the temporary snapshot, and the drag
context instead of starting a drag with NULL payload entries.  Completion also
clears the context after cleanup and safely ignores a duplicate completion
callback.

The focused regression is
`tests/automated/check-list-drag-allocation.py`.

## List draw-origin bounds

List now clamps the combined BaseX/XOrigin draw origin in both left-to-right
and right-to-left layouts.  Large horizontal scroll offsets therefore cannot
wrap the row origin through Motif's signed `Position` type.

`tests/automated/check-list-draw-origin-bounds.py` protects this contract.

## Empty List keyboard position

An empty List has no current keyboard item, even if its item-array pointer has
been allocated. The getter now returns zero for that state, and the setter
rejects all positions before attempting highlight or visibility work. This
prevents a phantom position and avoids a `-1` internal index.

Regression coverage: `tests/automated/check-list-empty-keyboard-position.py`.

## List geometry bounds

List traversal highlights and horizontal scrollbar sizing now calculate widths
and heights with signed intermediates before checking for non-positive values.
Collapsed lists therefore clamp or skip invalid geometry instead of allowing
unsigned subtraction to wrap into large draw or scrollbar dimensions.

The focused regression is `tests/automated/check-list-geometry-bounds.py`.

## List highlight geometry

List traversal highlighting derives its width and height by subtracting margin
and shadow insets. Although the outer widget dimension was cast to `int`, an
unsigned shadow thickness inside the inner expression could still force the
subtraction through unsigned arithmetic and wrap a collapsed highlight.

All highlight inset terms now participate in signed arithmetic. Existing
positive-area checks continue to suppress drawing when no highlight span fits.

## List hit-test bounds

List hit-testing now compares the event Y coordinate with the lower content
boundary using signed `long long` arithmetic.  A tall list can no longer turn
`core.height - BaseY` into a negative value through a `Position` cast and
misclassify clicks near the bottom.

`tests/automated/check-list-hit-test-bounds.py` protects this contract.

## List hit-test edge bounds

`WhichItem` receives X event coordinates as `int`, but previously narrowed
them to legacy 16-bit `Position` before testing the row. On a large List that
could wrap an event coordinate and select a different row. It now keeps the
event coordinate wide, compares it directly with the widget height, and keeps
the subsequent row calculation in `long long` until it has proved the result
falls within the List's `int` item range.

The top-edge comparison also widens both operands before subtraction. Normal
row selection is unchanged, while large-window and transient geometry remain
deterministic. The contract is protected by
`tests/automated/check-list-hit-test-edge-bounds.py`.

The same audit found that `XmListSetHorizPos` stored its public `int` scroll
position directly in the legacy `Position` drawing origin. The scrollbar state
remains wide, while the X11 drawing coordinate is now explicitly clamped at
that representation boundary instead of wrapping.

## List hit-test geometry

List hit testing compares event Y coordinates with the bottom of the content
area, derived by subtracting the signed `BaseY` origin from the unsigned widget
height. When a list is temporarily shorter than its insets, the old expression
wrapped before comparison and could select an item outside the visible area.

The comparisons now perform signed subtraction, preserving normal hit testing
while keeping collapsed-list selection bounded.

## List horizontal maximum bounds

List computes the horizontal content maximum in a wide intermediate type and
clamps it to the navigator’s signed range. This prevents `BaseX * 2` or the
content width from overflowing before scrollbar range normalization.

## List horizontal-origin bounds

List now clamps the `MaxWidth - listwidth` scroll-origin calculation before
storing it in the signed `Position` XOrigin field.  Very wide content cannot
wrap the horizontal origin and shift every row to an unrelated coordinate.

`tests/automated/check-list-horizontal-origin-bounds.py` protects this
scrolling contract.

## Native List horizontal-scroll regression

`tests/automated/run-list-horizontal-scroll.py` builds an installed-library
fixture and runs it under Xvfb. The fixture scrolls long List content to the
far right, replaces it with short content, changes the margin, and verifies
that the horizontal scrollbar keeps a positive slider/increment/page size and
an in-range value.

## List horizontal scroll range

List now clamps its horizontal scrollbar maximum, slider extent, and origin
as a single range whenever content, padding, or font metrics change. A stale
origin can no longer make the slider size zero or leave the value beyond the
new maximum after a shrink.

## List horizontal-scrollbar fit

`XmList` decides whether an `XmAS_NEEDED` horizontal scrollbar is required
from the drawable width left after margins, highlight, and shadow thickness.
That width is signed because a transiently tiny allocation can leave no
positive viewport.  The decision now clamps only that comparison to zero
instead of converting a negative value to `Dimension`; long content therefore
continues to show its scrollbar rather than being mistaken for content that
fits.  The state snapshot and return value also track the horizontal scrollbar
itself, so a visibility change is reported to the surrounding ScrolledWindow.

The source contract is checked by
`tests/automated/check-list-horizontal-scrollbar-fit.py`.

## XmList internal-list growth

`XmList` keeps an internal `ElementPtr` array alongside its public item
array.  Insertion grows that array with `XtRealloc`.  The old code assigned
the result directly to the widget field, then moved existing entries.  If
allocation failed, the old pointer was lost and the move dereferenced a null
pointer.  The byte count was also formed without checking the `int` item
count against `size_t`.

The growth path now checks the count against both host `size_t` and Xt's
`Cardinal` byte limits before multiplication, and commits the new pointer only
after `XtRealloc` succeeds. The same width checks protect deletion shrink
paths. Existing Motif callers and resource interfaces are unchanged; an
allocation failure returns without corrupting the existing internal array.

## List item geometry

`XmList` item geometry queries derive the returned item width by subtracting
list margins and shadow thickness from the unsigned widget width. A collapsed
list could wrap that result and report a huge item rectangle to callers.

The query now computes available width in signed arithmetic and returns zero
when no positive content span remains. Normal item geometry is unchanged for
usable list sizes.

## XmList item mutation rollback

List insertion now checks every `XmStringCopy`.  If a copy fails, already
copied entries are released, the pre-existing items are moved back into place,
and the operation reports failure without publishing a partial list.  The
replacement path copies the new item before freeing the old value, so a failed
copy leaves both the primary and selected-item representations intact.

The insertion allocation is also checked against Xt's `Cardinal` byte range.
The focused regression is `tests/automated/check-list-item-mutation-rollback.py`.

## List keyboard-visibility percentage

The `ListItemVisible` translation action now rejects malformed or out-of-range
parameters, clamps valid percentages to 0–100, and computes the visible-item
offset without multiplying the full item count by the percentage first.

The focused regression check is
`tests/automated/check-list-item-visible-percentage.py`.

## List keyboard page geometry

List horizontal page navigation derives its movement from the viewport width,
character-width estimate, and border/margin insets. The old expressions mixed
the unsigned widget width with signed constants before storing the result in an
`int`; a very narrow list could wrap the page step and jump to an unrelated
horizontal position.

The four keyboard paging paths now convert the viewport width before
subtraction. Normal page movement is unchanged while collapsed lists keep
bounded navigation positions.

## List navigation widget guards

The four public List viewport/navigation setters now reject non-List widgets
before entering application context or reading list-private geometry state.
Valid scrolling and item visibility behavior is unchanged.

The source contract is checked by
`tests/automated/check-list-navigation-widget-class.py`.

## List position-to-bounds range

`XmListPosToBounds` accepts the historical zero value to mean the last item,
but other values are one-based positions. The API now rejects negative and
out-of-range values before converting a valid nonzero position to zero-based
form, preventing signed overflow for extreme `int` inputs. Its visible-row
comparison also uses widened arithmetic, so extreme `topPosition` and
`visibleItemCount` resource values cannot wrap the visible-range end.

Regression coverage: `tests/automated/check-list-pos-to-bounds-range.py`.

## List position-bounds widget guard

`XmListPosToBounds` now rejects non-List widgets before realization checks or
list-private geometry access. Valid list bounds queries are unchanged, while
malformed callers no longer reinterpret unrelated widget memory as list state.

The source contract is checked by
`tests/automated/check-list-pos-to-bounds-widget-class.py`.

## `XmListPosToBounds` position bounds

`XmListPosToBounds` returns the rectangle for a visible list item.  Its X origin
subtracts highlight thickness from `BaseX`, and its Y origin adds the row offset
to `BaseY`.  Direct assignment to X11 `Position` could wrap these values for
large row offsets or transiently oversized margins.

Both returned origins now use the List bounded-position helper after widening
the arithmetic.  Valid ordinary coordinates are unchanged; extreme values
clamp to the representable X11 range.  The contract is protected by
`tests/automated/check-list-pos-to-bounds.py`.

## List position arithmetic

List add and delete APIs now validate one-based positions before converting
them to zero-based indexes. Negative add positions retain the historical
append behavior without evaluating an overflowing subtraction; delete
positions at or below zero are rejected. Delete counts are bounded with
subtraction instead of overflowing index-plus-count arithmetic.

Regression coverage: `tests/automated/check-list-position-arithmetic.py`.

## XmList top-position conversion guards

The synthetic `XmNtopItemPosition` resource converts between Motif's public
one-based position and XmList's internal zero-based position. The conversion
callbacks now reject null output storage and unrelated widgets before touching
list state. Invalid exports return `XmINVALID_POSITION`; invalid imports leave
the requested value unchanged.

The source-contract regression check is:

```sh
python3 tests/automated/check-list-position-conversion-guards.py
```

## List top-position import guards

The List synthetic top-position converter now validates the widget class and
rejects `INT_MIN` before converting the public one-based position to Motif’s
internal zero-based value. This prevents signed underflow while preserving the
existing conversion for valid inputs.

The focused source-contract check is
`python3 tests/automated/check-list-position-import-guards.py`.

## List replacement position bounds

The public List replacement APIs use one-based `int` positions. Replacement
now rejects a position beyond the current item count before converting it to a
zero-based index, and truncates the requested count using subtraction rather
than an overflow-prone index-plus-count expression.

Regression coverage: `tests/automated/check-list-replace-position-bounds.py`.

## List replacement widget guards

The five public List replacement APIs now reject non-List widgets before
entering application context or accessing list-private item and selection
state. Valid replacement behavior is unchanged.

The source contract is checked by
`tests/automated/check-list-replace-widget-class.py`.

## List result allocation failures

`XmListGetMatchPos` and `XmListGetSelectedPos` now check their caller-owned
result allocations against both host `size_t` and Xt's `Cardinal` limits before
writing or copying positions. On allocation failure or an unrepresentable
result, both APIs leave the initialized output pointers and counts unchanged
and return `False`; successful result contents and ownership remain unchanged.

## List row-position bounds

List row-height accumulation now uses signed `long long` arithmetic and clamps
the resulting redraw positions before storing them in `Position` variables.
Large item counts, row heights, or spacing values therefore cannot wrap a row
to an unrelated coordinate.

`tests/automated/check-list-row-position-bounds.py` protects this contract.

## List selection bounds

`XmListGetMatchPos` and `XmListGetSelectedPos` check result sizes before
allocating caller-visible arrays. The internal selection builders also reject
invalid counts before constructing cached selection arrays.

List keeps a one-based selected-position cache while selection updates are in
flight. Keyboard, API, and deselection consumers now share a bounds check before
converting a position to an internal zero-based item index, so a stale zero or
out-of-range entry is ignored rather than dereferencing outside `InternalList`.
Valid selection behavior and the public List API remain unchanged.

The source contract is checked by
`tests/automated/check-list-selection-internals.py` and
`tests/automated/check-list-selection-consumer-bounds.py`.

## List selection callback allocation failures

Multiple and extended List selection callbacks receive temporary copies of
selected items and positions. The widget now checks both temporary allocations
before populating them. If either allocation fails, both arrays are released
and the callback receives the existing empty-array state rather than a partial
or invalid pointer.

## List selection-callback snapshot rollback

List selection callbacks now copy their selected-item snapshots
transactionally. If any `XmStringCopy` fails, already-copied entries and the
temporary position array are released and the callback reports zero selected
items rather than exposing NULL entries under a nonzero count.

The focused regression is
`tests/automated/check-list-selection-callback-allocation.py`.

## List selection-copy rollback

List selection snapshots now treat each selected-item `XmStringCopy` as part of
one transaction.  If a copy fails, all earlier copies and the snapshot array
are released, and the widget publishes an empty selection snapshot rather than
passing NULL entries with a non-zero count to callbacks.

The focused regression is
`tests/automated/check-list-selection-copy-rollback.py`.

## List selection rebuild allocation failures

The List widget now checks the allocations used while rebuilding selected item
and selected position arrays against both host `size_t` and Xt's `Cardinal`
limits. If either allocation fails or is unrepresentable, the corresponding
selection count is reset to zero and the widget keeps an empty, consistent
selection state instead of writing through a null pointer. Successful rebuilds
retain their existing ordering and selection behavior.

The bounded byte counts are passed explicitly as Xt `Cardinal` values at both
selection-array allocation sites.

## List selection widget guards

The five public List selection APIs now reject non-List widgets before
entering application context or reading list-private selection state. Valid
selection and callback behavior is unchanged.

The source contract is checked by
`tests/automated/check-list-selection-widget-class.py`.

## List single-selection normalization

An application can change a List from a multiple-selection policy to single
or browse selection while several positions remain selected internally.  The
next item insertion used to assert that this transient state was impossible.

List insertion now keeps the first existing selection, clears stale extras,
and then applies the normal single-selection insertion rule.  Valid single,
browse, multiple, and extended selection behavior is unchanged.  The static
regression check is `tests/automated/check-list-single-selection.py`.

## List Y-to-position saturation

`XmListYToPos` uses an out-of-range position marker below the list. The marker
now saturates at `INT_MAX` for a maximum-sized List, and the public conversion
does not increment that saturated value. This preserves normal positions while
avoiding signed overflow on extreme list sizes.

Regression coverage: `tests/automated/check-list-y-to-pos-saturation.py`.

## MainWindow clip-child compaction

When automatic scrolling discovers a child in the clip window that belongs
to the main window, `XmMainWindow` moves it to the main-window child list and
removes it from the clip list. The compaction loop now writes each trailing
sibling to `j - 1` and revisits the same slot after removal. This prevents
duplicate or skipped children when adjacent clip-window entries are moved.
The related arithmetic regression check accepts the maintained declaration's
additional local width variable instead of depending on an obsolete exact
declaration prefix.

## MainW child-list growth failures

`XmMainWindow` occasionally moves an unexpected clip-window child into its
own composite child list.  The old expansion path updated `num_slots` before
checking `XtRealloc`, so allocation failure could discard the child array and
leave the next store out of bounds.

The migration now checks slot arithmetic and both host `size_t` and Xt
`Cardinal` byte limits, commits the new pointer and count only after successful
allocation by passing the bounded size explicitly as an Xt `Cardinal`, and
returns from that change-managed pass if growth cannot be completed.

The host-size test is compiled only where `size_t` is narrower than Xt's
`Cardinal` allocation boundary; on ordinary 64-bit hosts that condition is
already implied by the Xt check and is intentionally not emitted as a
misleading always-false diagnostic.

## MainWindow layout bounds

MainWindow layout now performs width and height subtraction with signed
operands before checking for negative space and applying its existing minimum
fallbacks.  Very small windows no longer wrap unsigned dimensions into large
child or separator sizes.

The focused regression is `tests/automated/check-mainw-layout-bounds.py`.

## MainWindow auto-scroll rectangle guard

`GetVertRects` now initializes its output arguments to an empty set and checks
the two-rectangle allocation. If allocation fails, callers receive zero
rectangles and no invalid memory is written; successful geometry calculation is
unchanged.

## MainWindow geometry bounds

MainWindow now saturates geometry-manager and preferred-size width and height
calculations before placing them in X11 Dimension fields. Large child requests,
borders, margins, highlights, scrollbars, or separators therefore cannot wrap
into a smaller parent request.

The focused regression check is
tests/automated/check-mainwindow-geometry-bounds.py.

## MainWindow child height geometry

MainWindow now derives parent heights from signed intermediates when a child
changes height.  Collapsed or shrinking children can no longer make the
unsigned `Dimension` subtraction wrap to a huge request.  The corresponding
scrolled-window Y offset update also uses a signed height delta, preserving the
content position during a shrink.

The source-level regression is
`tests/automated/check-mainwindow-height-arithmetic.py`.

## MainWindow width query bounds

When a work window requests a resize, MainWindow queries its menu bar using
the available width after both margins.  That subtraction now uses signed
arithmetic and clamps collapsed windows to zero, preventing a wrapped query
width when the margins exceed the current window width.

The source-level regression is
`tests/automated/check-mainwindow-width-query-bounds.py`.

## MakeIcon argument guards

Icon creation can be requested while workspace membership is being removed.
`MakeIcon` now rejects a missing workspace or client before touching icon
geometry or workspace-client data.  Existing callers already treat a false
return as an icon creation failure and unwind management, so normal behavior
and compatibility are unchanged.

## `makestrs` comment check invocation

`tests/automated/check-makestrs-comments.py` needs the built `makestrs`
executable and a new absolute results directory, because it runs the binary
against several input cases and records a receipt. When invoked without those
arguments as part of the repository-wide source-contract sweep, it now prints
a `SKIP` result and exits successfully; an explicit invocation still requires
both paths and performs the full check.

## Client withdrawal placement cleanup

Client withdrawal now validates workspace records, placement storage, and
stored icon indices before clearing placement entries in WSM and non-WSM paths.
Valid cleanup remains unchanged.

`tests/automated/check-manage-place-cleanup.py` protects the contract.

## Manager child-count traversal

`CheckRemoveMotionHandlers` now uses a `Cardinal` index while scanning the
manager's composite children. This matches Xt's child-count field and avoids
signed-index wraparound for large, valid manager child lists.

The focused source regression is
`tests/automated/check-manager-child-count-bounds.py`.

## Manager class-extension allocation

Manager class initialization now checks the inherited composite extension and
the manager extension allocation before copying or initializing either
record.  Under allocation pressure, class initialization returns without
dereferencing a null extension instead of crashing the process during widget
class setup.  The normal inheritance and resource behavior is unchanged when
the allocations succeed.

The focused source-level regression is
`tests/automated/check-manager-class-allocation.py`.

## Manager coordinate getter guards

Manager's synthetic `x` and `y` exports now validate widget and output storage
before querying parent/shell geometry. Invalid calls return deterministic zero
values; normal manager coordinate conversion remains unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-manager-coordinate-getter-guards.py
```

## Manager trait getter guards

Manager’s direction, unit-type, and access-colors trait callbacks now validate
the widget class and color-output structure before reading manager state.
Invalid inputs return safely (`XmDEFAULT_DIRECTION` or `XmPIXELS`, or no color
write); valid manager subclasses retain their configured values.

The focused source-contract check is
`python3 tests/automated/check-manager-trait-getter-guards.py`.

## Key-event mapping array allocation

`_XmMapKeyEvents` returns three parallel arrays while parsing a virtual-key
translation. Each array resize is now checked before its new element is
written, and count-to-byte arithmetic is bounded. A failure marks the parse
as unsuccessful so the existing cleanup path releases every partial array and
returns an empty result.

Successful mappings keep the same array layout and ordering. The change only
prevents allocation pressure or integer limits from becoming a null-pointer
write or mismatched parallel arrays.

## MapEvents parallel-array growth

Event translation parsing returns three parallel arrays: event types,
keysyms, and modifiers.  The old parser resized them one at a time.  If a
later resize failed, the output arrays no longer described the same number of
events.

Each parsed event now grows all three arrays together, checks both host
`size_t` and Xt `Cardinal` byte limits, copies the existing prefix, and commits
the replacement set only after every allocation succeeds.
The parser reports failure with the previous prefix still consistent.

## MapEvents token bounds

Translation-table parsing accepts key symbols, modifier names, and event types
from application resources. The parser previously copied each token into a
100-byte stack array with `strncpy` and no length check.

The parser now rejects a token that cannot fit, reports the translation as
invalid through its existing status path, and keeps all valid token behavior
unchanged. This prevents malformed or unexpectedly long resource strings from
corrupting the parser stack.

`tests/automated/check-mapevents-token-bounds.py` guards the bounded-copy
contracts.

## Image mask-name size checks

Image mask names reserve three extra bytes for the generated prefix and
terminator. The cache now checks the `strlen` addition before allocating that
buffer, both for embedded XPM masks and `XmeGetMask`. An unrepresentable size
returns the existing failure result and releases any temporary mask image.

The focused regression is `tests/automated/check-mask-name-size.py`.

## Multibyte XmString allocation bounds

`_XmGetMBStringFromXmString` accumulates component text in a wide length type,
then allocates through Xt.  Both conversion paths now reject a final length
whose terminator would exceed Xt’s `Cardinal` range, preventing size truncation
while preserving ordinary multibyte conversion results.
The fallback conversion path also releases its current component and context
before returning on that failure.

`tests/automated/check-mb-string-cardinal-bounds.py` protects both paths.

## Menu button border-clear bounds

Menu leave and disarm callbacks can run while a button has less space than its
highlight frame.  CascadeButton, PushButton, and ToggleButton now skip
`XmeClearBorder` unless both interior dimensions are positive, and convert the
unsigned core dimensions before subtracting the frame.

Normal-sized menu entries retain their existing border clearing and callback
ordering.  The focused regression is
`tests/automated/check-menu-border-clear-bounds.py`.

## Menu command-match allocation

Menu command matching now treats its temporary `MatchList` allocation and
both copied strings as one operation.  A failure leaves the existing match
list unchanged and releases any partial allocation.

`tests/automated/check-menu-match-allocation.py` protects this path.

## Menu recreation allocation

Dynamic menu updates now check the temporary `MenuSpec` pointer array before
initializing it.  If the allocation fails, Mwm reports the condition and
leaves the existing menu widgets untouched.
The element-count multiplication is also bounded before allocation, so an
unrepresentable match-list size is rejected without wrapping the byte count or
exceeding Xt's `Cardinal` allocation width.

`tests/automated/check-menu-recreate-allocation.py` protects this path.

## MenuShell child-count bounds

MenuShell expose, popup-child lookup, and managed-child focus traversal now
use Xt `Cardinal` indices for composite child arrays. This preserves menu
ordering and focus behavior while avoiding signed-index wraparound for large
menu hierarchies.

## MenuShell direction trait guard

MenuShell’s layout-direction trait now validates the widget class before
reading shell state. Invalid inputs return `XmDEFAULT_DIRECTION`; valid
MenuShell subclasses retain their configured direction.

The focused source-contract check is
`python3 tests/automated/check-menushell-direction-guard.py`.

## MenuShell empty-child guards

MenuShell popdown helpers historically assumed that every shell had a first
child. During private-shell teardown or an interrupted menu construction, an
empty shell can reach those helpers and dereference `children[0]`.

Fast popdown now unmaps an empty shell, normal popdown safely closes it when
needed, and recursive child traversal returns immediately. Normal one-child
menu behavior is unchanged. The regression
`tests/automated/check-menushell-empty-guards.py` covers all three paths.

## MenuShell placement geometry

Menu placement computes the right and bottom edges by adding a signed screen
position to unsigned border and widget dimensions.  On a large or temporarily
out-of-range menu, the old expression could wrap before the on-screen checks,
causing incorrect repositioning.

The edge calculations now use signed integer intermediates. Existing clamping
keeps the final menu origin on screen while making oversized-menu placement
deterministic.

## MenuShell popup placement bounds

MenuShell popup edge placement now computes screen clamping in signed
arithmetic.  Oversized or temporarily collapsed menus no longer wrap through
unsigned `Position` and `Dimension` subtraction when moved back on screen.

The source-level regression is `tests/automated/check-menushell-placement.py`.

## Posted MenuShell child guard

Recursive menu popdown can find a posted shell whose child has already been
removed. `PopdownKids` must check that nested shell before reading its first
child, just as it checks the current shell.

The nested empty-shell case now returns safely and leaves the existing normal
popdown recursion unchanged. The regression
`tests/automated/check-menushell-posted-child-guard.py` covers this teardown
boundary.

## Menu cascade traversal guard

Keyboard menu traversal can observe a popup shell while it is being torn down.
`FindNextMenuBarCascade` checked only that the shell pointer was non-null before
reading its first child, so an empty shell could crash traversal.

The function now requires at least one child before deriving the active cascade
button. With an empty shell it continues through the existing menu-bar search
fallback. The regression `tests/automated/check-menuutil-cascade-guard.py`
covers the lifecycle guard.

## MessageBox getter class guards

MessageBox’s message, symbol-pixmap, OK-label, Cancel-label, and Help-label
synthetic getters now validate the widget class before accessing private child
state. Invalid callback inputs use the existing return path, while valid
MessageBox reads preserve their current child-resource behavior.

The focused source-contract check is
`python3 tests/automated/check-messagebox-getter-class-guards.py`.

## MessageBox synthetic getter guards

Message-string, symbol-pixmap, OK-label, cancel-label, and help-label exports
now validate the widget and output pointers before reading child state. This
keeps Unicode label and dialog-icon resource queries safe during partial
initialization.

## XmeMicroSleep duration handling

`XmeMicroSleep` converts a microsecond duration to a `timeval` for `select`.
Negative durations previously reached the system call and produced an invalid,
platform-dependent timeout.

Nonpositive durations now return immediately with success. Positive durations
retain the existing second and microsecond conversion.

`tests/automated/check-microsleep-duration.py` guards the duration check.

## Modal-tree guards

Modal bookkeeping can run while a transient subtree is being removed.  The
recursive mark, unmark, and count-fixup helpers now reject missing nodes and
non-positive decrement counts before touching modal state or pointer-grab
state.  Existing modal behavior for valid trees is unchanged.

## Icon-info move guards

`MoveIconInfo` now treats missing placement storage, invalid source or
destination indices, self-moves, and empty source entries as no-ops.  This
prevents malformed workspace state from indexing outside the placement array
or dereferencing a missing client while leaving valid icon moves unchanged.
When workspace metadata is enabled, a missing per-workspace client record is
also tolerated while the placement list is updated.

`tests/automated/check-move-icon-info-guards.py` protects the validation
contract.

## Mrm argument floating-point input

`UrmCWRSetArgValue` receives floating-point argument values by address. It now
rejects a null address before changing the argument descriptor, so a failed
write cannot leave a stale argument with a newly assigned representation type.

Valid `MrmRtypeFloat`, `MrmRtypeHorizontalFloat`, and
`MrmRtypeVerticalFloat` values retain the existing serialized format.

## Mrm argument resource-reference state

`UrmCWRSetArgResourceRef` now assigns the argument’s resource representation
only after `UrmCWR__AppendResource` succeeds and the argument pointers have
been rebound. A failed resource append therefore cannot leave a partially
updated argument descriptor.

## Mrm callback floating-point input

`UrmCWRSetCallbackItem` receives floating-point callback values as pointers.
The writer now rejects a null pointer before appending the callback routine or
touching the callback record. This avoids a null dereference and keeps a failed
write from leaving a partially modified resource context.

The guard applies to `MrmRtypeFloat`, `MrmRtypeHorizontalFloat`, and
`MrmRtypeVerticalFloat`; valid by-address values retain the existing serialized
format and callback behavior.

## Mrm callback resource-reference state

`UrmCWRSetCallbackItemRes` now appends the routine and resource descriptor
before updating the callback item. If the resource append fails, the existing
callback fields remain unchanged; successful references retain their routine,
resource representation, and offset serialization.

## Mrm callback item type preflight

`UrmCWRSetCallbackItem` now validates its representation type before appending
the routine name or updating the callback item. Unsupported vector and unknown
types therefore fail without leaving record data behind, while the existing
scalar, string, wide-character, floating-point, and null callback forms remain
unchanged.

## Mrm callback-writer terminators

`UrmCWRSetArgCallback` and `UrmCWRSetCreationCallback` append a null callback
item for the in-memory Xt callback list.  Their descriptor allocation now
includes that terminator (`nitems + 1`) and validates the complete size before
writing it.  Valid UID writer output remains unchanged apart from eliminating
the one-item overwrite at the boundary.

## Mrm Char8 copy bounds

Mrm's safe resource-copy path now keeps a `Char8` string length in `size_t`
until it has been checked against Xt's `Cardinal` allocation width. An
unrepresentable string is rejected cleanly; valid strings retain the same
null-terminated copy behavior.

The source contract is covered by
`tests/automated/check-mrm-char8-copy-bounds.py`.

## Mrm Char8 vector span

Mrm byte-string vectors carry both a string-pointer table and string storage.
Before copying and rebasing the pointers, `Urm__CW_SafeCopyValue` now verifies
that the advertised byte span contains every pointer entry.  Truncated or
malformed resource data is rejected before the copy or pointer walk; valid
vectors retain their existing representation and ownership.

Each embedded string pointer must also fall within that span and reach a NUL
terminator before its end. Only then is it rebased into the destination copy.
Null, dangling, unterminated, or out-of-span entries therefore cannot produce
an invalid destination pointer.

## Mrm child-reference state

`UrmCWRSetChild` now validates the key and appends an index string before
updating the child descriptor. Manage/access flags, key type, and key value are
committed together only after the operation succeeds, so invalid child
references cannot leave partially updated records.

## Mrm color-table arguments

Color-table arguments now use their validated entry count when calculating
serialized storage, reject invalid or oversized tables, and copy the complete
table into the resource record before storing its offset. This prevents the
previous fixed-size allocation from omitting entries and avoids leaving the
record uninitialized.

## Mrm value-conversion destination

`Urm__CW_ConvertValue` now rejects a null value pointer before entering its
representation-specific conversion branches. This prevents immediate null
dereferences while leaving valid argument, vector, font, color, and pixmap
conversions unchanged.

## Mrm compound-string conversion output

`UrmCWR__AppendCString` now rejects an inconsistent converter result where the
encoded length is positive but the returned byte-stream pointer is null. This
prevents a null `memcpy` while preserving empty-string handling and normal
bounded compound-string serialization.

## MRM data-handle failure outputs

The internal MRM helpers that produce an `IDBDataHandle` now clear its record
number and item offset before validating inputs, accessing IDB records, or
allocating storage. A failed lookup or write therefore cannot cause a caller to
reuse an earlier successful handle as though it described the failed operation.

The source-level regression is
`tests/automated/check-mrm-data-handle-output-init.py`.

## Mrm deferred bitmap arguments

`Urm__CW_CreateArglist` uses a legacy ten-entry staging array while deferring
icon-image and bitmap-file resource conversion until foreground and background
values are known.  Every bitmap-file path now applies the same capacity guard
as icon images.  Additional deferred bitmap resources report the established
Mrm failure and are skipped instead of writing past that array.

The focused regression is
`tests/automated/check-mrm-deferred-bitmap-arguments.py`.

## Mrm deferred callback-list sizing

Deferred SetValues callback lists retain a callback descriptor until widget
references resolve.  The retained copy now allocates and copies `count + 1`
items, including the terminator consumed by the resolver, and validates the
descriptor marker before copying.  Valid callback-list behavior is unchanged.

## Mrm literal-fetch context setup

All public literal fetchers now propagate resource-context allocation failure
before attempting to load or free through that context, and release their MRM
locks on that early return. Icon literal loading validates its context-list
output, propagates pointer-list allocation failure, and reuses a supplied list
instead of replacing it. Valid literal, icon, bitmap, and color fetches retain
their existing conversion and ownership behavior.

The source-level regression is
`tests/automated/check-mrm-fetch-literal-context-setup.py`.

## Mrm FetchSetValues argument counts

`MrmFetchSetValues` accepts a public `Cardinal` count but uses an internal
pointer list and loop counters represented as `int`.  It now rejects counts
larger than `INT_MAX` before allocation or conversion to those internal
interfaces.  Representable argument lists retain their existing behavior;
unrepresentable counts return `MrmFAILURE` deterministically.

The focused regression is
`tests/automated/check-mrm-fetch-set-values-argument-count.py`.

## Mrm FetchSetValues failure cleanup

`MrmFetchSetValues` allocates an internal pointer list before determining the
largest literal index.  If that index is too large for an Mrm resource record,
the routine now releases both the local argument list and the pointer list
before returning `MrmFAILURE`.

The focused regression is
`tests/automated/check-mrm-fetch-set-values-failure-cleanup.py`.

## Mrm FetchSetValues icon arguments

`MrmFetchSetValues` now retains deferred icon arguments in checked dynamic
storage sized for the supplied argument list.  The historic fixed ten-entry
temporary array could overflow when a UID-driven `XtSetValues` operation
resolved more than ten icon-image literals.  Larger valid lists now retain all
their icon arguments; invalid or unrepresentable allocation sizes fail with
the established `MrmFAILURE` result.

The focused regression is
`tests/automated/check-mrm-fetch-set-values-icon-arguments.py`.

## Mrm callback fixup count

Callback fixup now rejects negative descriptor counts as well as counts beyond
the supported list limit.  This prevents malformed descriptors from being
treated as empty callback lists while preserving valid callback ordering and
resolution.

## Mrm value-fixup pointer inputs

Font-list fixup now validates the file metadata pointer before checking the
database version. Unit-valued integer and floating-point fixup branches also
reject null values before dereferencing them. Valid resource fixups retain
their existing byte-swapping and conversion behavior.

## Mrm value-fixup swap state

`Urm__CW_FixupValue` now rejects a missing byte-swap state pointer before any
representation-specific conversion.  This protects scalar and vector paths
from dereferencing invalid state while leaving valid native and byte-swapped
resource conversion unchanged.

## Mrm text-vector fixup guards

Text-vector fixup now rejects a null value buffer, null record buffer, or null
byte-swap flag before dereferencing them.  It also rejects negative vector
counts instead of treating malformed vectors as empty.  Valid Char8 and
XmString vectors retain their existing fixup and conversion behavior.

## Mrm font-list swap validation

Current-format font-list fixup now validates the `URMFontListValid` marker and
the signed count after cross-endian byte swapping, before walking font entries.
Malformed swapped records are rejected without iteration; valid font lists keep
their existing relocation behavior.

## Mrm header RID handle validation

Resource-ID entries in the header store a record number and an entry offset.
`Idb__HDR_EnterItem` now rejects non-positive record numbers and applies the
appropriate offset limit for header versus data records before writing the RID
table. Valid handles produced by Mrm remain unchanged.

## Mrm header RID allocation inputs

`Idb__HDR_NextRID` rejects a null open-file handle or null output pointer
before reading or updating the next-RID state. Once its output is validated, it
is cleared before every header-state failure path. Valid allocation behavior
and resource-ID layout remain unchanged.

## Mrm header RID return validation

`Idb__HDR_ReturnItem` now validates the stored record number and applies the
header/data-record offset limit before copying a resource handle to the
caller. Empty entries retain their existing `MrmNULL_DATA` behavior, while
partially corrupt RID entries return `MrmBAD_RECORD` safely. A supplied output
handle is initialized to zero before input validation; only the validated
stored handle is later published.

## Mrm hierarchy file-list bounds

`Urm__OpenHierarchy` allocates one file-list entry for each requested UID
file, and a second vector for each resource group. The count is now checked
against both `size_t` and Xt's `Cardinal` byte limits before either allocation,
so a large or malformed file count cannot wrap the allocation argument.

The focused regression check is
`tests/automated/check-mrm-hierarchy-counts.py`.

## MRM index-search failure outputs

`Idb__INX_FindIndex` and `Idb__INX_SearchIndex` now initialize their output
values before fallible work. When an index search fails because the file or an
index record is invalid, callers cannot reuse a stale buffer or index from an
earlier lookup. The normal `MrmSUCCESS`, `MrmINDEX_LT`, and `MrmINDEX_GT`
results retain their documented buffer/index outputs for insertion and lookup
callers.

The source-level regression is
`tests/automated/check-mrm-index-search-output-init.py`.

## Mrm interface-module record bounds

`MrmFetchInterfaceModule` obtains its topmost-widget names from a serialized
UID resource. Before iterating that list it now validates the descriptor's
signed count and proves that the requested number of `RGMTopmostDesc` entries
fits in the loaded resource context. A truncated or malformed module returns
the existing `MrmBAD_IF_MODULE` result instead of reading beyond its record.

The focused source regression is
`tests/automated/check-mrm-interface-module-bounds.py`.

## Mrm legacy font-list marker

The 1.1 compatibility path now validates the legacy font-list validation
marker before copying entries into the current representation. Valid legacy
lists continue to be converted as before; malformed lists are rejected before
their offsets are dereferenced.

## Mrm literal fixup failure status

`Urm__FetchLiteral` now propagates failures from its shared literal-value
fixup routine for current-format font lists and other literal types. Previously
those calls could fail while the loader reported success, exposing a partially
fixed serialized value to its callers. Existing successful native and
cross-endian literal conversion is unchanged. A failed icon-image fixup also
releases its auxiliary literal contexts at the loader boundary, while its
caller retains ownership of the primary context.

The focused source regression is
`tests/automated/check-mrm-literal-fixup-status.py`.

## Mrm legacy callback translation count

Legacy Mrm callback descriptors include a terminating item.  Translation now
allocates and validates space for `count + 1` callback items, matching the
existing inclusive copy loop.  It also rejects descriptors with an invalid
validation marker before reading the count, and the record validator rejects
the same marker for both old and current descriptor formats (including a null
record pointer).  Valid callback descriptors keep their legacy layout and
ownership; malformed or unrepresentable counts are rejected.

## Mrm create-close handling

The write-open path now checks the `close` result for the descriptor returned by
`creat`. If closing the newly created file fails, Mrm unlinks the partial file
and returns failure instead of proceeding with an ambiguous descriptor state.

## Mrm public parameter validation

`MrmOpenHierarchyPerDisplay` now rejects a non-null OS-extension list whose
first entry is null before assigning its display.  It returns the established
`MrmFAILURE` result and releases the process lock, matching
`Urm__OpenHierarchy`’s existing invalid-parameter contract.  Valid explicit
and default OS-extension paths are unchanged.

The hierarchy-open and name-registration public entry points also reject
negative and oversized wide-prototype counts before passing them to internal
`MrmCount` (`short`) interfaces.  This prevents an `int` supplied by a
wide-prototype caller from narrowing before the existing validation runs.

The focused regression is
`tests/automated/check-mrm-per-display-open-parameters.py`.

## Mrm reader negative counts

Mrm text vectors, integer vectors, font lists, and legacy font-list fixups use
signed count fields. Their conversion paths now reject negative values before
iterating or exposing the item arrays, while retaining the existing
`RGMListSizeMax` upper bound for valid resource data.

## Mrm resource-reference append inputs

The shared `UrmCWR__AppendResource` helper now rejects a null resource context
or output-offset pointer before entering its key-type handling. Argument and
callback resource-reference writers both use this helper, so malformed calls
fail safely without passing invalid pointers into record allocation.

## Mrm RID allocator output initialization

The direct header and RID-record resource-ID allocators now clear a supplied
output ID before validating file state or attempting to extend the RID map. A
failed allocation therefore cannot leave callers with an older ID that could be
mistaken for a newly reserved resource. Successful resource-ID allocation and
on-disk layout are unchanged.

The source-level regression is
`tests/automated/check-mrm-rid-output-init.py`.

## Mrm callback-value null guard

`Urm__CW_SafeCopyValue` copies resource values into callback-owned storage.
The routine now rejects a null value pointer before inspecting the value or its
vector dimensions.  Valid scalar and vector callback values retain the same
copying and ownership behavior.

## Mrm byte-swapped index-count bounds

Before byte-swapping each entry of a cross-endian IDB index leaf or node,
Mrm now validates the swapped signed count against the number of entries that
fit in the fixed-size database record. Malformed counts fail with the existing
`MrmFAILURE` result rather than causing the conversion loop to write past the
record buffer. Valid cross-endian UID index records retain their existing
conversion behavior.

The focused source regression is
`tests/automated/check-mrm-swap-index-count-bounds.py`.

## Mrm text-vector terminators

Char8 and XmString vector writers reserve a null entry so the vector can be
used as an in-memory pointer list.  Both writers now allocate and validate
`count + 1` entries before writing that terminator.  Valid vector contents and
wire representation remain unchanged.

## Mrm translated callback destination

When an old-format Mrm callback descriptor is converted, callback records are
now materialized through the newly translated descriptor.  The previous path
used the original resource pointer as the destination, which could write using
the wrong legacy layout.  Current-format descriptors keep the same behavior
because their translated pointer is the original descriptor.

## Mrm global name-registration validation

Global MRM name registration now validates every supplied name before it
touches the registry. A null name therefore fails the complete registration
request without registering earlier entries from the same vector. This matches
the existing hierarchy-local registration behavior and preserves all valid
name/value registrations.

The source-level regression is
`tests/automated/check-mrm-wci-hash-callers.py`.

## Mrm byte-swapped callback conversion status

Cross-endian widget records may contain callback descriptors either as an
argument value or as the widget creation callback.  Mrm now propagates a
failure from either descriptor conversion to the enclosing widget conversion.
Previously, an unsupported callback representation could make the descriptor
conversion fail while the widget record still reported `MrmSUCCESS` and was
subsequently treated as valid.  Valid callback descriptors and native-endian
widget records retain their existing behavior.

The focused source regression is
`tests/automated/check-mrm-widget-callback-swap-status.py`.

## Mrm widget-fetch parameters

`MrmFetchWidgetOverride`, and therefore `MrmFetchWidget`, now reject a null
widget index, widget result pointer, or class result pointer.  A nonzero
override count also requires a non-null override argument list.  The entry
point returns the established `MrmFAILURE` result before accessing application
context, acquiring locks, or dereferencing caller-owned pointers. A non-null
`*w_return` remains the documented “already fetched” marker: the call returns
`MrmSUCCESS` without constructing a duplicate widget or changing either
output. For an actual fetch (`*w_return == NULL`), the class output is cleared
before hierarchy lookup, so a failed fetch cannot expose a stale class code;
the widget output remains null until construction succeeds. Successful fetches
retain their established widget and class results.

The focused regression is
`tests/automated/check-mrm-widget-fetch-parameters.py`.

## Mrm byte-swapped widget list bounds

Before converting a cross-endian widget record's argument, child, or callback
list, Mrm now verifies the record header, descriptor offset, fixed descriptor
prefix, and swapped item count against the serialized record size. Embedded
callback/resource offsets are also checked before a pointer is formed. A
malformed UID record therefore fails with `MrmFAILURE` before conversion can
walk outside its own storage. Correctly formed records retain the existing
in-place byte-swap format and behavior.

The focused source regression is
`tests/automated/check-mrm-widget-swap-list-bounds.py`.

## Mrm byte-swapped widget conversion status

The hierarchy, indexed-resource, and RID widget readers now return the status
from `Urm__SwapRGMWidgetRecord`. Previously each caller discarded a failed
byte-swap conversion and reported `MrmSUCCESS`, leaving a malformed
cross-endian widget record available to later code. Successful native and
byte-swapped widget reads retain their existing behavior. On a failed in-place
conversion, the readers now clear the widget validation tag before returning
the error, preventing a later retry from accepting a partially swapped record.

The focused source regression is
`tests/automated/check-mrm-widget-swap-status.py`.

## MRM widget-reference lookup validation

`Urm__CW_FindWRef` now rejects null outputs, invalid contexts, and null widget
names before examining the reference buffer. It clears a supplied widget output
on failure and validates descriptor counts, heap bounds, name offsets, and
terminators before comparing a stored name. Corrupt or incomplete widget
reference data therefore fails safely instead of reading outside the resource
context.

The source-level regression is
`tests/automated/check-mrm-wref-lookup-inputs.py`.

## MultiList child allocation

`XmMultiList` creates its title, scrollbars, list frame, and optional search
controls as separate Xt children. The widget now tolerates an individual
creation failure: callbacks and geometry are installed only for children that
exist, list/frame sizing is skipped until its required children are present,
and search operations safely become no-ops when the search controls are
unavailable. Normal construction and layout are unchanged.

The contract is protected by
`tests/automated/check-multilist-child-allocation.py`.

## MultiList optional-child updates

MultiList can remain usable when allocation pressure prevents its optional
find controls or title widget from being created.  Later `SetValues` calls now
manage or unmanage only children that exist, and label changes update the
find/title controls only when their widgets are available.

The normal fully constructed layout and resource behavior is unchanged.  This
keeps deferred configuration safe after partial construction instead of
passing null widget handles to Xt.

## MultiList geometry request bounds

MultiList now computes parent resize requests with signed width and height
intermediates, clamps each result to one pixel before placing it in the Xt
geometry request, and saturates the frame's child-plus-border size during
construction. Child shrink requests and large border resources cannot wrap
into huge or unrelated parent dimensions.

The focused regression is `tests/automated/check-multilist-geometry-bounds.py`.

## MultiList selection API inputs

MultiList selection, deselection, visibility, and selected-row query helpers
now reject a null widget before entering the application context. Array
queries return `NULL` (and reset the optional row count); mutators remain
no-ops. Valid selection behavior is unchanged.

The focused regression check is `tests/automated/check-multilist-inputs.py`.

## MultiList label initialization rollback

MultiList now checks its initial title, titleString, and find-label copies.
Failed copies fall back to empty or default find labels before child widgets
are created, preventing NULL label propagation during construction.

The focused regression is
`tests/automated/check-multilist-label-init-rollback.py`.

## MultiList label rollback

MultiList now copies changed find and title labels before releasing their
current `XmString` values. Failed copies preserve the existing label and skip
the child refresh; NULL find labels still fall back to the default label when
that fallback can be allocated.

The focused source regression is
`tests/automated/check-multilist-label-rollback.py`.

## MultiList layout with an optional find control

When the find controls are requested but the find-text child could not be
created, MultiList now lays out the remaining frame and list without reading
the missing child’s geometry.  A complete find control still receives the
same positioning and width calculation as before.

## MultiList search allocation failure

`XmMultiList` now abandons a search safely when the text-field string cannot
be allocated.  It releases the optional wide-string result and leaves the
previous search term unchanged instead of passing NULL into comparison and
row-search code.

The focused source regression is
`tests/automated/check-multilist-search-allocation.py`.

## MultiList search callback safety

Search activation now validates its widget, callback payload, find controls,
and internal I18List before reading callback fields or requesting text.  A
partial MultiList therefore treats a search request as a no-op instead of
passing missing children to the text or list implementation.

The callback still reports found and not-found results exactly as before for a
fully constructed MultiList.

## MultiList search-row geometry bounds

The optional `XmMultiList` search row derives child sizes, origins, and the
remaining text width from widget dimensions and border widths. Narrow
arithmetic could wrap child-plus-border sizes or turn a negative text origin
into a huge unsigned width.

The layout now uses wide intermediate arithmetic and clamps Position and
Dimension conversions. Ordinary search rows retain their existing geometry;
extreme dimensions remain bounded and deterministic.

The focused source regression is
`tests/automated/check-multilist-search-geometry-bounds.py`.

## MultiList selection after partial construction

The public MultiList selection helpers now validate both the outer widget and
the internal I18List child.  If list creation failed, selection, visibility,
and deselection requests return without passing a null child to the I18List
implementation.  Row-array queries also return an empty result and reset the
reported count.

Fully constructed MultiLists retain their existing selection and callback
behavior; this only defines the failure behavior for partial construction.

## MultiList widget-class guards

All public MultiList selection, row-query, and visibility APIs now validate
that their widget is an `xmMultiListWidgetClass` instance before inspecting
the embedded internal List child. Invalid or null widgets return safely,
while valid MultiList behavior is unchanged.

The source contract is checked by
`tests/automated/check-multilist-widget-class-guards.py`.

## Mwm menu-warning bounds

Mwm’s menu diagnostics include resource-defined menu names. The warning helper
now always emits a bounded message through `snprintf`; names longer than the
fixed diagnostic buffer are truncated safely instead of suppressing the
warning altogether. Existing short warning text is unchanged.

`tests/automated/check-mwarning-bounds.py` guards this behavior.

## MWM action-argument parser failures

MWM parses `f.action` arguments into an owned action descriptor.  It does not
allocate an argument array until it reads the first argument, so a valid
zero-argument action has no needless allocation requirement.  A failed
argument allocation, an unrepresentable argument length, an unavailable
environment-variable expansion, or failed argument-array growth must not leave
the descriptor usable with only the arguments parsed so far.  The parser
releases that partial descriptor, clears the returned argument pointer, and
reports conversion failure.  This prevents an action from running with a
silently shortened argument list.

Regression: `tests/automated/check-mwm-action-argument-failures.py`.

## MWM automation input guard

`GetAutomationData` now rejects a null request payload before unpacking its
window and operation identifiers. Together with reply initialization, malformed
automation requests return an empty reply without dereferencing invalid input;
valid requests retain their existing protocol behavior.

The focused regression check is
`tests/automated/check-mwm-automation-input-guard.py`.

## MWM automation reply allocation guards

All `GetAutomationData` operation cases now use one checked allocation path.
The helper rejects size multiplication overflow and allocation failure before
the operation-specific packer runs, returning an empty reply while preserving
the existing wire layout for successful requests.

The focused regression check is
`tests/automated/check-mwm-automation-reply-allocation.py`.

## MWM automation reply initialization

`GetAutomationData` now initializes the output type, buffer, length, and
format before looking up a client window, and returns safely when any output
argument is null. Invalid window or operation requests therefore produce an
empty reply instead of leaving callers with stale output state; valid replies
retain their existing wire format.

The focused regression check is
`tests/automated/check-mwm-automation-reply-initialization.py`.

## MWM automation reply type

Successful `GetAutomationData` conversions now set `outputType` to
`_MOTIF_WM_AUTOMATION`, matching the target-typed replies produced by the WSM
conversion path. Empty failure replies continue to use `None`; the automation
payload format remains `WSM_PROTO_FMT`.

The focused regression check is
`tests/automated/check-mwm-automation-reply-type.py`.

## MWM backdrop color-set bounds

MWM validates a configured backdrop color-set ID and falls back to the
workspace default. The default is now validated too before indexing the pixel
set array, so an invalid or partially initialized workspace cannot turn
`colorSetId - 1` into an out-of-bounds access. Valid color-server behavior is
unchanged.

The source contract is checked by
`tests/automated/check-mwm-backdrop-colorset-bounds.py`.

## MWM backdrop pixmap request input

The backdrop-change request now requires a complete hexadecimal pixmap ID that
fits the platform's unsigned-long conversion. Malformed and overflowing IDs
are rejected with a warning and never reach `SetNewBackdrop` as an accidental
zero or truncated value.

## MWM backdrop request diagnostics

A CDE backdrop-change request must use the exact request token, followed by the
end of the request or a space-separated argument. Prefixes of unrelated request
names are ignored instead of being mistaken for backdrop changes.

A valid backdrop-change request without a path reports only the missing-path
error. MWM reports allocation failure only when a present path cannot be copied.

```sh
python3 tests/automated/check-mwm-backdrop-request-diagnostics.py
```

## MWM base-window metric validation

`BaseWindowWidth` and `BaseWindowHeight` return zero for a null
`ClientData` pointer.  These unsigned metric helpers have no error channel;
zero is the safe no-window value.  Valid size calculations remain unchanged.

The source-contract regression is
`tests/automated/check-mwm-base-window-metrics.py`.

## MWM binding-string growth

MWM expands virtual-key bindings by replacing matching names with longer
names. The expansion now computes prefix, suffix, and replacement sizes without
modifying the live string first, checks the Xt size limit, and commits the
replacement pointer only after `XtRealloc` succeeds.

If growth cannot be represented or allocated, the current binding string is
returned unchanged and the caller can continue using the prior bindings.

## MWM border-metric input validation

`UpperBorderWidth` and `LowerBorderWidth` return zero for a null
`ClientData` pointer.  This avoids reading decoration, matte, or bevel state
from an absent client; valid border metrics are unchanged.

The source-contract regression is
`tests/automated/check-mwm-border-metrics-inputs.py`.

## MWM client-metadata refresh inputs

The MWM client-metadata refresh entry points accept a `ClientData *` supplied
by event and session-management paths.  They now treat a null record as a
no-op before accessing the display, client window, cached strings, hints, or
title state.

This applies to class, session-client ID, save-hint, WM-hint, normal-hint,
window-title, icon-title, and Motif-hint processing.  A valid client record
continues through the established property and resource processing unchanged.

The focused check is:

```
python3 tests/automated/check-mwm-client-metadata-inputs.py
```

It verifies that every covered entry point has its null guard before its first
client-record member access.

## MWM client workspace input validation

MWM's session workspace-name aggregation now validates its `ClientData` input
before reading the associated screen pointer.  A null or incomplete client
record produces no workspace list instead of dereferencing incomplete session
state.

The exported icon-position loader, proxy-property reader, and XSMP resource
lookup apply the same boundary: incomplete client, screen, workspace-list,
session-ID, or format state is ignored safely instead of being dereferenced.
The public session-match entry point and both XSMP and proxy match paths also
reject a missing client; database lookup requires a client ID, format string,
and initialized resource database.  This makes partial session restore state a
safe non-match rather than a window-manager failure.

Regression: `tests/automated/check-mwm-client-workspace-inputs.py`.

## MWM client-command character classification

MWM command parsing now casts command-stream bytes to `unsigned char` before
calling `isalnum`. This avoids signed-char undefined behavior for high-bit
localized menu text while preserving the accepted command-name grammar.

## MWM command-removal list capacity

When MWM removes commands belonging to a client, matching command IDs are
collected in a fixed internal array. The collector now checks that array's
capacity before appending an ID and reports when an excess entry is skipped,
preventing a large command tree from overwriting adjacent state.

## MWM corner-metric input validation

`CornerWidth` and `CornerHeight` return zero for a null client record.  Their
three-corner proportionality checks use a wide intermediate so large unsigned
metric values cannot wrap during comparison.  Valid metric results are
unchanged.

The source-contract regression is
`tests/automated/check-mwm-corner-metrics-inputs.py`.

## MWM `DefineCommand` strings

MWM now decodes DefineCommand names only when each length-prefixed string fits
inside the received IPC payload. Truncated string data is rejected before
command-tree mutation, and temporary decoded strings are released after the
tree copies them. Complete requests retain their existing behavior.

## MWM depressed-gadget geometry input validation

`GetDepressInfo` returns `FALSE` when its client record, geometry outputs, or
inversion-width output is null.  The guard runs before decoration state or
bevel metrics are read.  Existing valid gadget geometry and bevel calculations
are unchanged.

The source-contract regression is
`tests/automated/check-mwm-depressed-gadget-inputs.py`.

## MWM text drawing length bounds

`XTextWidth`, `XDrawString`, and `XDrawImageString` take signed `int` text
lengths.  MWM's `DrawStringInBox` now measures the input string once, rejects
lengths above `INT_MAX`, and uses the checked `int` value consistently for
measurement and drawing.  `WmDrawString` has the matching signed interface and
also rejects a negative length.

This preserves normal 8-bit MWM title and feedback drawing while preventing a
large string from narrowing into an invalid Xlib count.

Focused check:

```
python3 tests/automated/check-mwm-draw-string-length.py
```

## MWM enable/disable message lengths

MWM now validates the command ID, window-count, and window-list bytes before
processing `EnableCommand` or `DisableCommand`. Truncated IPC messages are
rejected before any count-driven unpack, while complete messages retain their
existing behavior.

## MWM event-parser input validation

The exported MWM button and key event parsers now reject missing cursor or
output pointers before parsing.  Button-release parsing also validates the
parsed button number before indexing the fixed modifier-mask table.  Valid
button and key bindings keep their existing parsing and modifier behavior.

The focused regression is
`tests/automated/check-mwm-event-parser-inputs.py`.

## MWM `f.exec` multibyte command endings

The MWM resource parser retains the established `f.exec` behavior of appending
one trailing `&` when a command does not already end with that delimiter.  The
multibyte parsing path now applies that rule to an empty command and to a valid
command whose last character occupies more than one byte.  Existing commands
that already end in `&` are unchanged.

The multibyte parser keeps no redundant state between character reads, so this
path remains warning-clean under the project’s targeted static analysis.

The focused regression is
`tests/automated/check-mwm-exec-multibyte-delimiter.py`.

## MWM frame/client conversion input validation

`FrameToClient` and `ClientToFrame` are legacy `void` helpers, so malformed
calls cannot report an error.  They now return without modifying any argument
when the `ClientData` record or one of the four coordinate/size pointers is
null.  Existing valid calls use the same conversion arithmetic.

The source-contract regression is
`tests/automated/check-mwm-frame-client-conversion-inputs.py`.

## MWM frame-coordinate metric validation

`BaseWindowX`, `BaseWindowY`, `FrameX`, and `FrameY` return zero for a null
`ClientData` pointer.  They are integer coordinate helpers without an error
channel; zero is the safe no-frame result.  Valid coordinate calculations are
unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-coordinate-metrics.py`.

## MWM frame exposure input validation

`FrameWindow` returns `FALSE` for a null client record.  `FrameExposureProc`
and `BaseWinExposureProc` return without drawing for the same input.  These
guards run before window, appearance, or display-list access; valid framing and
exposure behavior is unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-exposure-inputs.py`.

## MWM frame-metric input validation

`FrameWidth` and `FrameHeight` return zero for a null `ClientData` pointer.
These unsigned metric helpers have no separate error channel, and zero is the
safe “no frame” value.  Existing valid frame calculations are unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-metrics-inputs.py`.

## MWM frame-part hit-test input validation

`IdentifyFramePart` returns `FRAME_NONE` when passed a null `ClientData`
pointer.  This is the existing “no frame part” sentinel, so malformed event
dispatch cannot dereference a missing client while valid hit testing remains
unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-part-hittest-inputs.py`.

## MWM frame-part input validation

`GetFramePartInfo` returns `FALSE` when its `ClientData` input or any geometry
output pointer is null.  The guard runs before `TitleBarHeight` reads client
state and before the frame-part switch writes results.  Valid calls keep the
existing decoration and geometry calculations unchanged; callers can treat a
`FALSE` result as “no frame-part geometry was returned.”

The source-contract regression is
`tests/automated/check-mwm-frame-part-inputs.py`.

## MWM frame-regeneration input validation

`RegenerateClientFrame` returns without changing windows or geometry when its
client record is null.  The guard precedes decoration lookup; valid frame
recalculation and subwindow resizing are unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-regeneration-inputs.py`.

## MWM frame-state input validation

`SetFrameInfo` and `SetClientOffset` return without changing state for a null
client record.  `XBorderIsShowing` returns `False` for the same input.  These
are the safe no-state/no-visible-border results for the legacy APIs; valid
state updates and queries remain unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-state-inputs.py`.

## MWM function-table index bounds

The WSM-facing MWM helpers that locate `f.exec`, `f.action`, and `f.nop` in
the built-in function table now reject null result pointers and clamp a
caller-provided table length to the actual table.  A negative, zero, or
oversized length now produces the safe default index instead of leaving an
output undefined or reading beyond the static table.

Valid callers that pass the built-in table length retain the same indices. The
focused source-level regression is
`tests/automated/check-mwm-function-index-bounds.py`.

## MWM function-parser input pointers

The exported `ParseWmFunction` API now validates its parser-cursor and output
arguments before dereferencing either one.  A missing cursor retains the
existing invalid-function behavior by producing `F_Nop` and `F_NOP_INDEX`; a
missing output destination returns `F_NOP_INDEX` without dereferencing it.
Valid resource parsing is unchanged.

The focused regression is
`tests/automated/check-mwm-function-input-pointers.py`.

## MWM gadget hit-test array validation

`GadgetID` returns `FRAME_NONE` when its gadget-array pointer is null.  This
keeps malformed hit-test requests safe even when their count is nonzero;
normal iteration and gadget ID results are unchanged for valid arrays.

The source-contract regression is
`tests/automated/check-mwm-gadget-array-inputs.py`.

## MWM gadget-info control flow

Mwm automation replies describe title-bar and resize gadgets. The fallback
paths that fill unavailable gadget records are now explicitly braced in the
maximize, move, and resize reply builders. This makes the fallback scope
unambiguous to maintainers and strict compilers while leaving the packed reply
layout and valid-gadget behavior unchanged.

Move and window-menu reply builders additionally initialize their temporary
records to the same invalid sentinel that `FillInvalidInfo` emits. A reply
therefore cannot expose uninitialized geometry even if a later maintenance
change bypasses a fallback branch.

`tests/automated/check-wmcmd-control-flow.py` protects these control-flow
boundaries. The focused Mwm compilation uses
`-Werror=misleading-indentation` against the real source.
