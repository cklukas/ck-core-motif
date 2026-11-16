---
nav_exclude: true
---
# Archived Bugfix and Hardening Notes (Part 2)

Consolidated archive of standalone bugfix and hardening validation notes.

## Empty accessibility cache snapshots

The AT-SPI cache `GetItems` method now treats an empty accessibility tree as a
valid empty result. It avoids calling `XtMalloc(0)` and only reports an
out-of-memory error when a nonempty snapshot allocation fails.

Nonempty snapshots and their failure cleanup are unchanged. The regression is
covered by `tests/automated/check-accessibility-cache-snapshot-allocation.py`.

The menu-change snapshot path uses the same zero-item handling, so an empty
menu does not turn a notification into a spurious allocation failure.
The menu snapshot also rejects counts that cannot fit Xt's `Cardinal` byte-size
argument before performing the allocation.

## Accessibility creation input guards

`XmAccessibilityCreate` now rejects a NULL root before asking Xt for an
application context, and returns cleanly when allocating the accessibility
state fails.

Valid roots and the public ABI are unchanged.  The static regression check is
`tests/automated/check-accessibility-create-inputs.py`.

Display-hook registration also rejects a missing accessibility context or
non-object widget before querying Xt display state.

## Accessibility D-Bus memo rollback

The AT-SPI D-Bus transport now allocates replacement copies of cached
accessible names and value text before releasing the previous copies.  A
failed `XtNewString` therefore leaves the memo intact, preserving the
screen-reader name/value and the baseline used for subsequent change
notifications.

The focused regression is
`tests/automated/check-accessibility-dbus-memo-rollback.py`.

## Accessibility D-Bus property input validation

Unknown or incomplete D-Bus property requests now return without dereferencing
a missing property signature. Known properties retain their existing values and
wire signatures.

## Accessibility display-hook allocation guard

Accessibility watches display-level resource changes through a small hook
record.  If that record cannot be allocated, registration now returns without
installing a callback or dereferencing NULL.

Normal display-hook registration is unchanged.  The static regression check is
`tests/automated/check-accessibility-hook-allocation.py`.

## Accessibility menu snapshots

Accessibility menu notifications snapshot child IDs before invoking providers
or transport callbacks. The snapshot allocation checks the child count against
both `size_t` and Xt's `Cardinal` width before multiplying by the ID size,
preventing a wrapped allocation on platforms where those widths differ. The
accepted byte count is passed explicitly as an Xt `Cardinal`.


The snapshot allocation is also checked for failure before callback dispatch,
so an unavailable buffer leaves the notification path unchanged and avoids a
null dereference. The source regression is
`tests/automated/check-accessibility-menu-snapshot-allocation.py`.


The AT-SPI cache `GetItems` and initial tree-seeding snapshots also reject
unrepresentable count arithmetic. `GetItems` reports a D-Bus `NoMemory` error
when its ID array cannot be allocated, rather than dereferencing a null
buffer. The source regression is
`tests/automated/check-accessibility-cache-snapshot-allocation.py`.


Initial AT-SPI tree seeding now abandons the seeding pass if its ID snapshot
allocation fails, leaving the transport alive for later retries instead of
writing through a null buffer. The source regression is
`tests/automated/check-accessibility-seed-allocation.py`.

## Accessibility menu snapshot allocation guard

Menu notifications snapshot child accessibility IDs before callbacks can
mutate the tree.  The snapshot now checks both host and Xt allocation limits
and returns safely when the ID array cannot be allocated.

Valid menu notifications are unchanged.  The static regression check is
`tests/automated/check-accessibility-menu-snapshot-allocation.py`.

## Accessibility text snapshot allocation

The D-Bus accessibility transport builds a bounded UTF-8 snapshot before
comparing text changes. The snapshot buffer allocation is now checked before
it is used, so an allocation failure abandons the snapshot cleanly instead of
writing through a null pointer. Normal text snapshots and their lifecycle are
unchanged.

The source-level regression is
`tests/automated/check-accessible-snapshot-allocation.py`.

## Accessibility text extraction allocation guard

Text extraction now checks the temporary locale buffer against the Xt
`Cardinal` limit and returns cleanly when allocation fails, before invoking
the text or text-field substring APIs.

Valid text extraction is unchanged.  The static regression check is
`tests/automated/check-accessibility-text-allocation.py`.

## Accessibility text-length bounds

The accessibility snapshot builder now bounds InfoBar, SearchBox, and Prompt
text lengths before storing them in signed metadata fields. Normal accessibility
text is unchanged; unusually long values no longer wrap the reported length.

The focused regression check is
`tests/automated/check-accessibility-text-length-bounds.py`.

## Accessibility text-span arithmetic

The accessibility text provider accepts integer start/end positions.  After
validating ordering and the widget’s text length, it now widens both endpoints
before subtracting them.  This prevents signed `int` overflow from defeating
the byte-capacity check for a very large requested range; the span is narrowed
only after it has been proven to fit the underlying Text/TextField API.

The locale's maximum multibyte width is explicitly widened to `size_t` before
capacity arithmetic, avoiding implementation-dependent signed conversion on
platforms where `MB_CUR_MAX` is an `int`.

The source contract is checked by
`tests/automated/check-accessible-text-span-arithmetic.py`.

## Accessibility UTF-8 allocation guards

Accessibility text conversion allocates a wide-character buffer and a UTF-8
result buffer.  The conversion now checks both host `size_t` limits and the
Xt `Cardinal` limit used by `XtMalloc`, and returns cleanly when either
allocation fails.

Valid conversions are unchanged, and no public ABI changes.  The static
regression check is `tests/automated/check-accessible-utf8-bounds.py`.

## Icon activation callback guards

`IconActivateCallback` now returns for a missing widget and ignores a context
lookup that yields no client record before dispatching the restore action.
Valid icon activation remains unchanged.

`tests/automated/check-activate-callback-guards.py` protects the contract.

## Active workspace icon placement

Non-WSM icon-hint processing can run while the active workspace is being
replaced.  Placement now checks `pSD->pActiveWS` before accessing its grid.
When the workspace is unavailable, the requested icon coordinates are kept
and the icon remains unassigned (`NO_ICON_PLACE`) until normal management can
place it safely.

## AddClientToList argument guards

Client management can observe a workspace lookup failure while a workspace is
being removed.  `AddClientToList` now returns without touching list or screen
state when either its workspace or client argument is missing.  Valid calls
follow the existing stacking and modal-window behavior unchanged.

## AddEntryToList guards

Window-list linking can be reached while a workspace is being detached.
`AddEntryToList` now returns when the workspace, its screen, or the entry is
missing, preventing partial list writes and null dereferences.  Valid list
ordering behavior is unchanged.

## Appearance Xrm string bounds

Appearance profiles convert resource text through Xt's `XrmValue` structure,
whose size member is 32-bit.  The conversion path now rejects strings that
cannot include their terminating byte in that field before passing them to Xt.

Tab-mode, enumerated, and color profile properties share that guard.  Xrm
resource lookup and inherited pixel transport also use explicit Motif/Xt
boolean and argument-value conversions, avoiding implicit narrowing while
preserving the existing resource values.

## Icon-box increment guards

`UpdateIncrements` and `ResetArrowButtonIncrements` now return when their
icon-box, client, scrollbar, or increment state is incomplete.  Initialized
icon boxes continue to update scrollbar increments normally.

`tests/automated/check-arrow-increment-guards.py` protects the contract.

## ArrowButton gadget origin bounds

ArrowButton gadget redisplay computes its fill and shadow origins from X11
`Position` coordinates and unsigned highlight/shadow dimensions. C's usual
integer conversions can otherwise wrap a negative or near-edge origin before
it reaches the X drawing call.

`ArrowBG.c` now promotes each operand to a wide signed type before adding the
dimensions, then saturates the final origin and extent at the drawing
boundary. The matching ArrowButton widget path follows the same rule. This
preserves the Motif API and the existing drawing geometry, while keeping
narrow or near-edge controls from receiving a wrapped origin or extent.

The source regression `tests/automated/check-arrow-drawing-insets.py` keeps
both widget and gadget inset calculations on the bounded-arithmetic path.

## Atom-list resource conversion allocation

The atom-list converter now checks dynamic token-array growth and the final
atom-array allocation against both host `size_t` and Xt's `Cardinal` byte
range. Any failure releases token strings and heap-backed name storage before
returning `False`. A successful conversion now also releases the temporary
heap-backed name vector used after more than 128 atom names have been parsed;
only the caller-owned atom result remains. `XInternAtoms` conversion and output
ownership remain unchanged.

## Atom-list converter input validation

The atom-list converter now rejects missing displays, source values, or
destinations before tokenization and `XInternAtoms`. Valid atom-list parsing
and output-size negotiation remain unchanged.

The focused regression check is
`tests/automated/check-atom-list-converter-inputs.py`.

## Auto-suggest string-copy allocation guards

`XmAutoSuggestBox` now copies retained item labels directly as `XmString`
values, preserving compound-string charset segments and multilingual text.
The separate text conversion used for filtering and callbacks remains
allocation-checked.

The focused regression check is
`tests/automated/check-auto-suggest-string-copy-guards.py`.

## Mwm automation menu-label sizing

Mwm’s automation replies include menu labels in variable-length buffers. The
menu-item check-info path now sizes the complete label, including its first
character and terminating byte, matching the other menu reply paths. This
preserves the wire format while preventing underallocation for nonempty
labels.

`tests/automated/check-automation-label-size.py` guards the sizing expression.

## AutoSuggestBox no-results label replacement

`XmNautoSuggestNoResultsLabel` is an owned AutoSuggestBox string.  A resource
update now copies its requested text and, when the no-results popup is visible,
constructs the replacement compound string before retiring the current label.
If either allocation fails, the original resource and list row stay intact.

An initial string-copy failure leaves the owned field empty but retains the
documented `"No suggestions"` fallback for later no-results presentation.
This preserves normal resource semantics and avoids borrowing application
storage after `XtSetValues` returns.

## AutoSuggestBox popup dimensions

AutoSuggestBox derives its popup width and height as signed working values so
it can perform edge and available-space calculations.  The dimensions are
bounded by the screen's signed coordinate domain before they cross the Xt
`Dimension` boundary, and the final conversion is explicit when configuring
the scroller.  This prevents signed-to-unsigned narrowing from making a popup
unexpectedly huge.

`tests/automated/check-autosuggest-popup-dimensions.py` guards that boundary.

## Avatar initials geometry bounds

`XmAvatar` now bounds initials text-width arithmetic before converting it to an
`int` for drawing. Normal initials retain their existing centered appearance;
extremely long values no longer wrap the calculated drawing position.

The focused regression check is
`tests/automated/check-avatar-initials-geometry-bounds.py`.

## WSM backdrop search-path bounds

The WSM backdrop-path setup prepends the user and administrative directories
to a resource-defined search path. It now computes the formatted length with
`snprintf`, rejects unrepresentable lengths, checks `XtMalloc`, and writes the
result with a bounded second pass. The old path is released only after the new
path is complete, preserving path order while avoiding integer-wrap and
allocation-copy failures.

`tests/automated/check-backdrop-path-bounds.py` guards this WSM-only path.

## Legacy background color override

The private `_XmSetDefaultBackgroundColorSpec` entry point is used by legacy
integrations that need to select a background color outside the resource
database. Its previous implementation wrote to a function-local variable that
was never consulted by `_XmGetDefaultBackgroundColorSpec`, so the setter had no
effect.

Both functions now use one owned override string. The getter returns that
override when present and otherwise keeps the existing resource-database and
built-in fallback behavior. Replacing the override releases the previous copy,
and a null setter input selects the standard Motif default.

`tests/automated/check-background-color-override.py` guards the shared-storage
contract.

## Badge coordinate bounds

Badge clamps its label origin before passing it to Motif's signed `Position`
drawing arguments.  Large badge heights or font metrics therefore cannot wrap
the label to an unrelated screen coordinate.

The modern inner face is skipped when the outer allocation cannot contain it.
Its label clip rectangle also bounds X11 coordinates and extents before the
conversion to `XRectangle`, preventing large margins from wrapping the clip.

`tests/automated/check-badge-coordinate-bounds.py` protects this drawing
contract.

## Badge resource and drawing safety

`XmBadge` now skips redisplay until its window and foreground GC are
available. Label replacement releases the previous owned `XmString` only
after a successful copy, retaining the old label if allocation fails.
Successful drawing and resource behavior are unchanged.

`tests/automated/check-badge-resource-safety.py` protects these paths.

## BaseClass indirection-table bounds

`CreateIndirectionTable` now uses a `Cardinal` resource index and checks the
resource-count-to-byte conversion against both host `size_t` and Xt `Cardinal`
limits before allocating the pointer table. Invalid or unrepresentable sizes
return `NULL`; normal resource indirection is unchanged.

## BreadcrumbBar RTL hit testing

BreadcrumbBar renders segments in reverse order when `XmNbreadcrumbRtl` is
enabled.  Hit testing previously always traversed the forward order, so the
segment selected by a click did not match the segment under the pointer.  The
hit-test path now follows the same traversal as rendering in both directions.

`tests/automated/check-breadcrumb-rtl-hit-test.py` keeps the two traversal
orders aligned.

## BreadcrumbBar size bounds

Breadcrumb labels and separators contribute fixed pixel estimates to hit
testing and preferred geometry.  The previous signed `int` arithmetic could
wrap for very long labels or many segments, producing negative cursor
positions.  Width calculation and cursor advancement now saturate at
`INT_MAX`, and preferred-width accumulation stops at that bound.

`tests/automated/check-breadcrumb-size-bounds.py` keeps the saturating
arithmetic in the sizing and hit-test paths.

## Breadcrumb text-length bounds

`XmBreadcrumbBar` now bounds measured label and separator widths before adding
padding, and avoids overflowing the final text position when the cursor reaches
its limit. Normal breadcrumb placement is unchanged.

The focused regression check is
`tests/automated/check-breadcrumb-text-bounds.py`.

## BulletinBoard dialog-title initialization rollback

`XmBulletinBoard` now checks the private copy of an initial dialog title
before publishing it to the parent shell.  If allocation fails, the existing
resource value remains in place and the window-manager title is not updated
with NULL.  Changed-title `SetValues` rollback behavior remains unchanged.

The focused regression is
`tests/automated/check-bulletin-dialog-title-init-rollback.py`.

## BulletinBoard dialog-title rollback

`XmBulletinBoard` now copies a changed dialog title before releasing the
current `XmString`. Allocation failure preserves the existing window-manager
title; NULL still explicitly clears it.

The focused source regression is
`tests/automated/check-bulletin-dialog-title-rollback.py`.

## ButtonBox layout bounds

ButtonBox still follows its established equal-size, fill, orientation, and
right-to-left layout policies.  Its internal major/minor coordinate arithmetic
now remains wide until the final X11 `Position` assignment, where it is
saturated to the representable signed range.  This prevents a large child or
margin calculation from wrapping to the opposite side of the box.

The geometry-request mask is narrowed deliberately to Xt's width and height
bits, and query-only requests are represented as a Boolean rather than an
arbitrary bit-mask value.

## ButtonBox size bounds

ButtonBox accumulated child sizes, borders, and margins directly in
`Dimension`.  Large child counts or dimensions could wrap totals and make the
parent request an unrelated small geometry.  Child totals, preferred sizes,
and layout requests now compute in `size_t` and saturate at the maximum
representable `Dimension`.

`tests/automated/check-buttonbox-size-bounds.py` keeps the bounded paths in
place.

### Geometry negotiation

Proportional fill uses the original requested preferred dimensions while a
geometry compromise is outstanding. When a child accepts that offer, ButtonBox
uses the same allocation rather than treating the accepted share as a new
preferred size and shrinking it again. Subsequent layout or competing geometry
requests invalidate the offer. Query-only acceptance leaves geometry and the
offer unchanged. This requires the pending ABI-9 constraint record.

`run-buttonbox-negotiation.py` exercises both orientations, equal and unequal
sizing, every fill mode, and zero/small/large requests. Every offered compromise
must be accepted on the next request, including a preceding query-only check.
The current native matrix passes 123 compromise acceptances in each build.

## Button-type resource conversion allocation

The string-to-button-type converter allocates a result table and a mutable
copy of the input. Both allocations are now checked. If the input copy fails,
the result table is released before returning `False`; successful parsing and
invalid-name cleanup retain the existing behavior. A malformed token sequence
(for example, an empty trailing token) now also releases both the table and
that mutable input copy before returning `False`.

## Cached pixmap insertion failure

`_XmCachePixmap` now publishes an entry transactionally across its pixmap and
pixmap-data tables. If either hash insertion fails, it removes the first
insertion and frees the copied name, color record and cache entry. The caller's
X11 pixmap remains owned by the caller, and the function reports failure.

The focused regression is
`tests/automated/check-xm-cache-pixmap-insertion.py`.

## Calendar month bounds

Calendar’s month-length and month-name helpers now validate the one-based
month before indexing their fixed tables. Invalid months return no days or an
empty name, while valid leap-year and localized calendar rendering behavior is
unchanged.

The source contract is checked by
`tests/automated/check-calendar-month-bounds.py`.

## Callback size-limit header

The internal callback-list growth guard uses `SIZE_MAX` to reject allocation
sizes that cannot be represented. `CallbackI.c` now includes `<stdint.h>`, the
standard header that defines that macro on supported C implementations, so the
guard compiles consistently on platforms where `<limits.h>` does not provide
it.

The focused regression is
`tests/automated/check-callback-size-limit-header.py`.

## Cardinal-list resource conversion allocation

The Cardinal-list resource converter now checks its initial array and every
growth reallocation. A failed allocation follows the existing conversion
warning path and releases the array already built, while successful numeric
conversion remains unchanged.

## Cardinal-list resource conversion bounds

The cardinal-list resource converter grows its output array while parsing
values.  Capacity doubling now checks the `int` limit and the resulting byte
count against both host `size_t` and Xt's `Cardinal` allocation range before
reallocation, passing the accepted byte count explicitly to Xt. Invalid or
unrepresentably large input uses the existing
cleanup path, preventing a wrapped capacity from causing an undersized array.

## CascadeButton armed fill guard

The etched-in `XmCascadeButton` redisplay path fills the realized window when
its menu item is armed.  During resource replacement or a zero-sized geometry
transition, the arm GC or dimensions can be unavailable.

The fill request now validates those prerequisites.  Label exposure, cascade
pixmap drawing, and shadow rendering continue through their existing paths.

## CascadeButtonGadget cascade glyph guards

The option-menu cascade glyph subtracts shadow margins from its configured
width.  A small or incomplete gadget could underflow that unsigned arithmetic
and pass invalid dimensions or missing GCs to the X drawing helpers.

The glyph path now performs the subtraction in a signed temporary, rejects
non-positive results or insufficient height, and requires a realized parent
and all drawing GCs.  Normal cascade glyph rendering is unchanged for valid
geometry.

## CascadeButtonGadget pixmap guards

`XmCascadeButtonGadget` copies its cascade pixmap into the parent manager
window.  During resource replacement or manager transitions, the parent may
not be realized, the pixmap may be unspecified, or the label GC may be absent.

The copy path now validates those prerequisites before querying pixmap data or
issuing `XCopyArea`/`XCopyPlane`.  Valid cascade pixmaps retain their existing
position, size, and depth handling.

## CascadeButton legacy fill guard

The legacy etched-in `XmCascadeButton` build branch selected either the armed
or background GC inline and passed it directly to `XFillRectangle`.  Resource
replacement and zero-sized transitions could therefore issue an invalid fill.

That branch now stores the selected GC and validates it and the widget
 dimensions before filling.  The behavior of valid legacy and `FIX_1509`
 builds remains the same.

## CascadeButton pixmap draw guards

`XmCascadeButton` copies its cascade pixmap into the widget window.  The old
path could reach the X copy helpers with an unrealized window, zero height,
an unspecified pixmap, or a missing label GC during resource replacement.

The copy path now validates those prerequisites before querying pixmap data or
issuing `XCopyArea`/`XCopyPlane`.  Valid cascade pixmaps retain their existing
position, size, and depth selection.

## CascadeButton pixmap metadata

`DrawCascade` now checks the result of `XmeGetPixmapData` before using the
returned depth.  If a pixmap is unavailable or its cache metadata cannot be
resolved, the cascade glyph draw is skipped safely instead of reading an
uninitialized depth value.

Successful metadata lookups retain the existing depth-matched `XCopyArea` or
bitmap `XCopyPlane` behavior.  The focused regression is
`tests/automated/check-cascade-pixmap-metadata.py`.

## CascadeButtonGadget pixmap metadata guard

CascadeButtonGadget drawing now checks `XmeGetPixmapData` success before using
the returned depth.  Invalid or unavailable cascade pixmaps are skipped safely
while valid gadget rendering is unchanged.

The static regression check is
`tests/automated/check-cascade-gadget-pixmap-metadata.py`.

## CascadeButton pixmap binding metadata

CascadeButton and CascadeButtonGadget now distinguish a valid uncached pixmap
metadata fetch from an unresolved pixmap.  Their glyph drawing paths validate
the initialized depth instead of treating `XmeGetPixmapData`'s cache-status
Boolean as an error signal, so the first draw of a valid glyph is retained.

The focused regression is `tests/automated/check-cascade-pixmap-binding.py`.

## CascadeButton pixmap metadata guard

CascadeButton drawing now checks whether `XmeGetPixmapData` succeeded before
using the returned depth.  Unavailable or invalid pixmap metadata now skips
drawing safely; valid pixmap rendering is unchanged.

The static regression check is
`tests/automated/check-cascade-pixmap-metadata.py`.

## Cascade gadget position geometry

`XmCascadeButtonGadget` centers its submenu indicator inside the menu entry.
The old calculation used unsigned widget dimensions throughout.  If a menu
entry was temporarily smaller than the indicator, margins, or highlight
insets, horizontal subtraction wrapped and vertical centering produced a huge
positive offset.

The position calculation now uses signed intermediates, treats a negative
available vertical span as zero, and clamps the resulting coordinates to the
X11 `Position` range before storing them.  Normal menu layouts retain their
existing coordinates while tiny entries remain drawable and stable during
resize and realization.

## CascadeButton gadget highlight bounds

CascadeButton gadget menu-leave highlighting now passes signed inset
dimensions to `XmeDrawHighlight`.  A collapsed gadget therefore reaches the
shared zero-area guard instead of passing a wrapped `Dimension` span.

The source-level regression is
`tests/automated/check-cascadebg-highlight-bounds.py`.

## CascadeButton gadget shadow origins

CascadeButton gadget shadow and highlight origins now add highlight thickness
to `Position` coordinates in signed arithmetic.  Gadgets positioned partly
outside a parent therefore retain their negative/off-edge origin instead of
wrapping to a distant positive coordinate.

The source-level regression is
`tests/automated/check-cascadebg-origin-bounds.py`.

## Character-set table conversion allocation

The string-to-character-set-table converter uses a mutable copy of the input
and one combined result allocation. Both allocations are now checked. A failed
input copy returns `False`, and a failed result allocation releases the input
copy before returning, while normal parsing and result ownership remain
unchanged. The input-copy length is checked against Xt's `Cardinal` width
before allocation, so malformed resource data cannot truncate the copy.

## Charset-table resource conversion bounds

The string-to-charset-table resource converter builds one allocation containing
the table and all copied charset names.  It now checks each name length, the
number of table entries, and the combined allocation size before allocating.
The combined size is also checked against Xt's `Cardinal` range and passed
explicitly at the allocator boundary.
Malformed or unrepresentably large resource values return conversion failure
after releasing the temporary input copy instead of wrapping a size calculation.
The pointer-table multiplication and temporary input-copy width are both
validated before their allocations are evaluated.

## Chip geometry bounds

`XmChip` now clamps text, pixmap, and accessory geometry calculations to the
representable `Dimension` range. Long labels and unusually large pixmaps no
longer wrap into undersized geometry requests. Preferred-size measurement also
skips `XmStringExtent` when render-table setup failed, matching redisplay's
safe fallback.

The focused regression check is
`tests/automated/check-chip-geometry-bounds.py`.

## Chip size arithmetic

Chip natural-size accumulation now widens its non-negative metrics explicitly.
Pixmap geometry is narrowed back to layout coordinates only after limiting it
to the available widget extent, preventing signed/unsigned conversions from
altering the rendered icon layout.

## XmeClearBorder bounds

`XmeClearBorder` now clamps the requested border thickness to the available
width and height before computing bottom and right clear origins.  Oversized
borders therefore clear only the widget-sized area instead of wrapping their
origins through unsigned subtraction.

The source-level regression is `tests/automated/check-clearborder-bounds.py`.

## Clipboard conversion header failure handling

Clipboard header creation can fail when serialized clipboard storage cannot be
allocated. The conversion callback and next-item ID helper now stop cleanly
when `ClipboardOpen` returns no header instead of dereferencing a null pointer.
Normal clipboard conversion and ID updates are unchanged.

The focused regression check is
`tests/automated/check-clipboard-convert-header-failure.py`.

## Clipboard copy length truncation guard

The legacy clipboard retrieval helper accepts its extension length as an
`int`, while the public copy APIs accept `unsigned long`. `XmClipboardCopy`
and `XmClipboardCopyByName` now reject lengths above `INT_MAX` before the
conversion, preventing a wrapped length from producing an undersized property
buffer. Normal clipboard copies are unchanged.

The focused regression check is
`tests/automated/check-clipboard-copy-length-guard.py`.

## Clipboard delete-format record validation

`ClipboardDeleteFormat` reads serialized format and data-item records while
marking a cut-by-name format as cancelled. It now checks lookup status and
record sizes before reading either record, and releases the format record if
the parent data item is unavailable or malformed. Valid clipboard records keep
the existing cancellation and cleanup behavior.

The focused regression check is
`tests/automated/check-clipboard-delete-format-validation.py`.

## Clipboard item lookup output guards

`ClipboardFindItem` now initializes its result pointers, rejects null output
arguments, and stops when an item ID cannot be converted to an X11 atom. This
prevents invalid IDs from reaching property lookup and gives callers
deterministic empty outputs on failure; valid lookups are unchanged.

The focused regression check is
`tests/automated/check-clipboard-find-item-output-guards.py`.

## Clipboard format enumeration bounds

`ClipboardFindFormat` walks a serialized data-item record and its format-ID
list. It now validates the header, record length, list offset/count, and each
format record before reading fields. Malformed clipboard properties are
rejected and released without changing enumeration of valid records.

The focused regression check is
`tests/automated/check-clipboard-format-enumeration-bounds.py`.

## Clipboard format enumeration null-name handling

`ClipboardFindFormat` supports both named matching and name-less enumeration.
The latter is used by `XmClipboardInquireCount`; it now skips `XInternAtom`
when no name is supplied, avoiding a null atom-name request while preserving
the existing enumeration behavior.

The focused regression check is
`tests/automated/check-clipboard-format-enumeration-null-name.py`.

## Clipboard format-length property request bound

Clipboard format metadata stores its length in the first 32-bit property
value. `ClipboardGetLenFromFormat` now requests one property unit instead of a
large fixed maximum, reducing memory and protocol work for malformed or
unexpectedly large properties while preserving the value used by valid
clipboard clients.

The focused regression check is
`tests/automated/check-clipboard-format-length-request.py`.

## Clipboard format-unit validation

Clipboard format metadata stores the X11 property unit size. The metadata
reader now accepts only 8, 16, or 32 bits, returning the existing default
8-bit fallback for unsupported values. This prevents malformed metadata from
reaching property replacement and copy arithmetic while preserving valid
Motif formats.

The focused regression check is
`tests/automated/check-clipboard-format-unit-validation.py`.

## Clipboard header list bounds

Clipboard headers contain an offset and count for the serialized data-item ID
list. `ClipboardOpen` now validates that the offset fits the property and that
the complete list fits in the retrieved record before returning the header.
Malformed headers are discarded safely; valid headers and their compatibility
layout are unchanged.

The focused regression check is
`tests/automated/check-clipboard-header-list-bounds.py`.

## Clipboard header validation in copy/message paths

Clipboard data-request messaging and copy-by-name operations now use the same
header validator as `ClipboardOpen`. They reject malformed data-item list
offsets or counts before touching the header, while valid legacy clipboard
headers continue through the existing paths.

The focused regression check is
`tests/automated/check-clipboard-header-validation-callers.py`.

## Clipboard item atom-name validation

Clipboard item IDs are encoded into root-property atom names. The helper now
rejects negative IDs and checks bounded formatting before interning the name,
so malformed IDs cannot produce truncated or overflowing atom names. Existing
header, next-ID, and valid item-ID names remain unchanged.

The focused regression check is
`tests/automated/check-clipboard-item-atom-name-validation.py`.

## Clipboard lock-marker format validation

The lock-access marker is written as an 8-bit property. `ClipboardWindowExists`
now requires that format when checking a surviving lock window, so a stale
property with an incompatible X11 representation cannot be mistaken for a
valid Motif lock marker.

The focused regression check is
`tests/automated/check-clipboard-lock-marker-format.py`.

## Clipboard lock-marker property bound

`ClipboardWindowExists` only needs to know whether the lock-access marker
property exists. It now requests one property unit and checks the X11 return
status, avoiding a large allocation for a tiny marker and treating failed or
empty reads as an absent marker.

The focused regression check is
`tests/automated/check-clipboard-lock-marker-request.py`.

## Clipboard lock record validation

The clipboard lock property is serialized data and may be missing, truncated,
or otherwise invalid after an interrupted or incompatible client operation.
`ClipboardLock` now checks the lookup result and record size before reading the
lock fields. `ClipboardUnlock` applies the same checks and rejects non-positive
lock levels. Invalid records fail safely after releasing any allocated record;
the existing behavior for a missing property and valid lock records is
unchanged.

The focused regression check is
`tests/automated/check-clipboard-lock-record-validation.py`.

## Clipboard header allocation failure handling

`ClipboardOpen` may need to create a header record when the clipboard property
is absent or malformed. If allocation fails, the helper now returns `NULL`
instead of initializing through a null pointer. Existing callers can release
their lock and report the operation failure, while successful clipboard opens
retain the existing behavior.

The focused regression check is
`tests/automated/check-clipboard-open-allocation-failure.py`.

## Clipboard pending-items format guard

`XmClipboardInquirePendingItems` requires a format name to find matching
formats. It now rejects a null name before locking or enumerating the
clipboard, avoiding an invalid atom lookup while preserving valid pending-item
queries.

The focused regression check is
`tests/automated/check-clipboard-pending-items-format-guard.py`.

## Clipboard format-registration atom guard

`RegisterFormat` now checks the atom returned for a format name before writing
the registration property. Allocation or interning failure returns
`ClipboardFail` without issuing an invalid X11 request; valid format
registrations are unchanged.

The focused regression check is
`tests/automated/check-clipboard-register-format-atom-guard.py`.

## Clipboard property replacement input validation

`ClipboardReplaceItem` converts byte lengths into X11 property units. It now
rejects formats other than 8, 16, and 32 bits, null buffers for non-empty
properties, and lengths that are not an exact number of format units before
performing that conversion. Valid replacements retain their existing chunking
and ownership behavior.

The focused regression check is
`tests/automated/check-clipboard-replace-input-validation.py`.

## Clipboard retrieval-size overflow guard

`ClipboardRetrieveItem` extends a serialized clipboard property by a caller
requested number of bytes. It now rejects negative extensions and arithmetic
overflow before allocation, preventing a wrapped size from causing an
undersized buffer followed by an oversized copy. Valid retrievals retain their
existing allocation and copy behavior.

The focused regression check is
`tests/automated/check-clipboard-retrieve-size-overflow.py`.

## ClipWindow RTL resize geometry

When a right-to-left `XmClipWindow` is resized, each managed child keeps its
distance from the right edge.  The old implementation subtracted a signed
child `Position` from an unsigned widget `Dimension`; a child with a negative
or otherwise out-of-range position could therefore wrap the distance and move
to an unrelated location.

The resize path now performs the calculation in signed integers and clamps the
result to the X11 `Position` range before configuring the child.  Normal RTL
resizes retain their existing coordinates while transient or clipped child
positions remain deterministic.

## Dynamic default-color cache allocation

The Mwm dynamic color cache now checks growth reallocation before publishing a
new cache pointer.  If growth fails, the cache remains valid and the dynamic
default conversion falls back to the requested background pixel instead of
dereferencing a null result.

`tests/automated/check-color-cache-allocation.py` protects this path.

## Color cache growth failures

Motif keeps small process-wide caches for default colors and calculated color
sets.  Their growth paths previously assigned `XtRealloc`'s result directly
to the cache pointer, then continued writing entries.  A failed allocation
could therefore discard the old cache and dereference a null pointer.

Initial allocation and growth now pass explicit Xt `Cardinal` byte sizes after
checking the count-to-byte conversion against both host `size_t` and Xt
`Cardinal` limits. The code calculates the new capacity without signed
overflow, and updates the cache pointer and capacity only after the allocation
succeeds.
Existing cache entries remain available when the toolkit cannot grow the
cache.

## Color-cache property cleanup

`ColorCachePropertyExists` reads the private pixel-set property from the
selection owner.  Xlib may return an allocated buffer even when the request
fails or the property has an unusable format.  The failure path now releases
that buffer before returning, preventing a leak while retaining the existing
`False` result for malformed cache properties. A buffer that
parses is handed to `FetchPixelData`, which copies it into a private bounded
buffer and releases the Xlib buffer on every path; the caller must not release
it again. `run-color-object-property-ownership.py` publishes the property of a
color server and creates a Motif shell under AddressSanitizer.

## Color object atom-name bounds

Color objects create one per-screen atom name from the fixed Motif resource
prefix and the screen index. The name slots are 24 bytes; formatting now uses
`snprintf` with that capacity, preserving normal atom names while preventing an
unexpectedly large index from writing beyond a slot.

`tests/automated/check-color-object-atom-bounds.py` guards the formatting path.

## Color object property serialization

Color-object properties encode numeric color fields as hexadecimal text before
updating the resource database. The parser uses a fixed 256-byte scratch buffer;
its numeric serialization now uses `snprintf` with that capacity, preserving the
property format while preventing an oversized value representation from writing
past the buffer.

`tests/automated/check-color-object-property-bounds.py` guards the serialization
paths.

## Color object property input bounds

Color objects receive serialized pixel sets from another X11 owner. The old
parser treated the property as an unbounded C string and repeatedly called
`sscanf`, so truncated or malformed properties could read beyond the received
payload and update colors from uninitialized data.

The parser now copies exactly the received length into a terminated scratch
buffer, validates each numeric record, and rejects malformed payloads before
updating state. Empty properties and invalid version markers are released
safely.

`tests/automated/check-color-object-property-input.py` guards these contracts.

## ColorPicker X11 boundaries

The ColorPicker keeps layout arithmetic in signed working values and now makes
the final X11 boundary explicit.  Empty spans stay empty, positive rectangle
extents are bounded before unsigned X requests, and invalid drawable depths
are rejected before pixmap or image construction.

For indexed visuals, an empty or invalid visual map no longer becomes a
negative XQueryColors count.  Palette allocation results are normalized to
Motif `Boolean` values, and the bounded fallback map is queried only when it
contains entries.  HSV conversion also tests its floating-point maximum
explicitly before division.

The result is unchanged normal rendering with safe behavior for constrained
widgets, malformed visual metadata, and unusually large resource values.

## Color selector label bounds

The color selector displays the selected color name together with its RGB
value. Color names can come from application resources or the X color database,
and the old `sprintf` calls wrote them into a fixed `BUFSIZ` label buffer.

Label construction now uses `snprintf` with the destination capacity. Normal
names and generated RGB labels retain their existing presentation, while an
unusually long name is safely clipped instead of overwriting the stack.

`tests/automated/check-color-selector-label-bounds.py` guards the label paths.

## ColorSelector label initialization rollback

`XmColorSelector` now retains each existing label when an initialization
`XmStringCopy` fails.  Previously, a failed copy replaced the string-table
entry with NULL, even though child widgets had already received the original
label.  Keeping the original avoids inconsistent labels and makes later
updates and destruction safe under allocation pressure.

The focused regression is
`tests/automated/check-color-selector-label-init-rollback.py`.

## ColorSelector list allocation

The color selector lazily creates its scrolled color list while loading the
RGB database. If that list cannot be created, loading now releases the merged
argument list and returns before managing the child, updating list resources,
or installing callbacks. Normal list creation and color-file loading remain
unchanged.

The contract is protected by
`tests/automated/check-color-selector-list-allocation.py`.

## ColorSelector equal-name copy rollback

When a ColorSelector receives a distinct string containing the same color
name, it now copies that string before releasing the old allocation. A failed
copy preserves the existing name instead of replacing it with NULL.

The focused source regression is
`tests/automated/check-color-selector-name-copy-rollback.py`.

## ColorSelector color-name rollback

`color_name_changed` now checks the replacement allocation before updating the
widget.  A failed copy leaves the previous color name and color state intact;
the existing NULL operation still clears the name explicitly.

The focused source regression is
`tests/automated/check-color-selector-name-rollback.py`.

## ColorSelector RGB-file rollback

ColorSelector now copies a changed RGB-file path before releasing the current
path. Allocation failure preserves the current file and color list instead of
leaving an unusable NULL path; a successful changed path still rebuilds the
list as before.

The focused source regression is
`tests/automated/check-color-selector-rgb-file-rollback.py`.

## WM_COLORMAP_WINDOWS allocation bounds

Mwm adds one slot for the client window while processing the
`WM_COLORMAP_WINDOWS` property. The row now allocates `(nitems + 1)` complete
`Window` and `Colormap` elements, checks the size before converting to Xt’s
`Cardinal` allocator, and cleans up either partial allocation. The colormap
flags array uses the same checked element count.

`tests/automated/check-colormap-property-allocation.py` guards this contract.

## Mwm colormap-reference allocation failures

Mwm keeps per-window arrays of client references for colormap management. The
add and remove paths now check each `XtMalloc` before copying or publishing the
array. On allocation failure they report the condition and leave the existing
X context untouched, avoiding a null dereference during client property or
focus updates.

`tests/automated/check-colormap-reference-oom.py` guards all three allocation
failure paths.

## ColorObj property parse status

ColorObj property lookup now reports whether the serialized pixel data was
actually parsed.  Malformed data or allocation failure returns failure to the
cache lookup, allowing the existing selection-based fallback to run.

Valid properties retain the same update behavior.  The static regression check
is `tests/automated/check-colorobj-property-parse-status.py`.

## ColorSelector string-resource rollback

`XmColorSelector` now copies replacement slider labels, mode labels, and error
strings before releasing the current widget-owned values.  Failed copies keep
the previous resource and avoid updating a child control with a NULL string.

The focused regression is
`tests/automated/check-colors-selector-string-rollback.py`.

## Column label allocation handling

When a Column creates its implicit label child, it first builds a generated
name and then creates the label widget. Both the generated-name byte size and
allocation, and child creation, are now checked. Allocation failure returns cleanly
without registering callbacks or dereferencing a null label widget; successful
Column layout behavior is unchanged.

The regression check is `tests/automated/check-column-label-allocation.py`.

## Column constraint-label initialization rollback

Column now checks the private copy of a child constraint’s label string.  If
the copy fails, it installs an empty compound string (or NULL if that fallback
also fails), so the constraint never retains a borrowed resource pointer that
its destroy path could later free.

The focused regression is
`tests/automated/check-column-label-init-rollback.py`.

## Column label rollback

`XmColumn` constraint updates now copy a replacement label before releasing
the current widget-owned string.  Failed copies keep the current label and do
not update the child with a NULL value.

The focused regression is `tests/automated/check-column-label-rollback.py`.

## ComboBox action fallback

ComboBox action dispatch now treats an unknown direction or action value as a
safe no-op. The supported up/down, page, and data-navigation actions are
unchanged, while malformed action parameters no longer terminate the process.

The regression check is
`tests/automated/check-combobox-action-fallback.py`.

## ComboBox arrow helper guard

ComboBox arrow hit-area and drawing helpers now no-op for the plain ComboBox
type. Plain ComboBoxes do not expose an arrow; drop-down list and drop-down
combo-box arrow behavior is unchanged.

The regression check is
`tests/automated/check-combobox-arrow-helper.py`.

## ComboBox child-argument bounds

During ComboBox initialization, Motif filters `XmNheight` out of the caller's
argument list before creating the private list and text children. The filter
now checks both host `size_t` and Xt `Cardinal` byte limits before multiplying
by `sizeof(Arg)`, and it uses a full-width `Cardinal` index while copying
arguments. An unrepresentable
or failed temporary allocation follows the existing compatibility fallback and
passes the original arguments to child creation instead of writing through a
wrapped allocation.

This preserves normal resource propagation and the public ComboBox ABI while
making oversized or malformed argument lists fail safely.

The source-level regression is `tests/automated/check-combobox-child-args.py`.

## ComboBox collapsed geometry

ComboBox now clamps the dropdown shell width to one pixel when the control is
smaller than its highlight insets.  Its right-side arrow hit-area x coordinate
also uses signed arithmetic, so narrow controls do not wrap popup or hit-test
geometry to a distant positive position.

The source-level regression is
`tests/automated/check-combobox-collapsed-geometry.py`.

## ComboBox default-action copy failure

The ComboBox keyboard default-action bridge now dispatches its List callback
only after the edit value has been copied successfully.  Allocation failure
therefore cannot expose a NULL callback item or a pointer whose lifetime ends
when the bridge returns; the ComboBox still performs its normal selection
cleanup afterward.

The focused regression is
`tests/automated/check-combobox-default-action-copy.py`.

## ComboBox hit and drawing bounds

ComboBox hit-area, arrow, and shadow rendering now clamp available dimensions
after insets to at least one pixel.  Collapsed controls no longer pass wrapped
unsigned heights or shadow spans to hit testing and drawing code.

The source-level regression is
`tests/automated/check-combobox-drawing-bounds.py`.

## ComboBox edit-field width bounds

ComboBox edit-field width and height now use signed available-space arithmetic
after borders, highlights, and dropdown arrow extents.  Collapsed controls
keep a one-pixel child geometry instead of wrapping to a huge unsigned
dimension.

The source-level regression is `tests/automated/check-combobox-edit-width.py`.

## ComboBox geometry negotiation bounds

When the scrolled list replies with `XtGeometryAlmost`, ComboBox now computes
the height difference in signed arithmetic and clamps the resulting edit-box
height.  A reply larger than the available edit area cannot wrap the next
geometry request to a huge unsigned height.

The source-level regression is
`tests/automated/check-combobox-geometry-diff.py`.

## ComboBox scrolled-list geometry

ComboBox scrolled-list layout now computes available width and height in
signed arithmetic and clamps collapsed dimensions to one pixel.  Large border,
highlight, or margin settings cannot wrap the list geometry into huge
unsigned dimensions.

The source-level regression is
`tests/automated/check-combobox-list-geometry.py`.

## ComboBox type fallback

ComboBox initialization now skips type-specific translation setup when the
internal type value is invalid. Shared accelerator installation still runs;
the documented combo-box, drop-down-list, and drop-down-combo-box modes are
unchanged.

The regression check is
`tests/automated/check-combobox-type-fallback.py`.

## Command-palette item allocation

`XmCommandPaletteSetItems` now rejects a `NULL` item array for a nonzero
count, counts beyond the legacy `XmList` `int` item domain, and item-array
allocation failure before populating entries.  Invalid replacement requests
leave the currently displayed command set intact.
The internal XmString copy helper also rejects a failed empty-string fallback
before calling the Motif constructor.
Filtering also handles failure to allocate its visible-index array before
writing matches, and skips an individual item if its copied display strings,
combined display buffer, or `XmString` cannot be allocated.  It also rejects
size-overflowing combined display lengths.  The palette remains usable and
can be populated again later; valid arrays and the public item-copying
contract are unchanged.

`tests/automated/check-command-palette-item-allocation.py` protects these
failure paths.

## CommandPalette replacement input

`XmCommandPaletteSetItems` validates non-empty replacement input before freeing
the current item model. A NULL item array or an oversized count is rejected
without erasing the visible palette. Passing a zero count remains the explicit
operation for clearing/rebuilding the palette.

The focused source-contract check is
`tests/automated/check-command-palette-input-preservation.py`.

`XmCommandPaletteSetFilter` likewise becomes a no-op once Xt has started
destroying the palette.  It therefore cannot call into the SearchBox child
during teardown.  The lifecycle check is
`tests/automated/check-command-palette-set-filter-destroy-guard.py`.

## CommandPalette selection bounds

`XmCommandPalette` receives selected row numbers from `XmList`.  The invoke
callback now rejects an empty, zero, or out-of-range row before converting the
one-based list position to the zero-based visible-item index.  This keeps a
malformed or stale selection from underflowing the index or reading beyond the
filtered command map while preserving normal callback dispatch.

The source contract is checked by
`tests/automated/check-command-palette-selection-bounds.py`.

## CommandPalette size-limit header

The CommandPalette text-combination allocation guard uses `SIZE_MAX` to reject
unrepresentable lengths. `CommandPalette.c` now includes `<stdint.h>`, making
that guard compile portably on platforms where `<string.h>` and the other
existing headers do not expose the macro.

The focused regression is
`tests/automated/check-command-palette-size-limit-header.py`.

## CommandPalette visible-index allocation and stale-state guards

Filtering maintains a second array mapping visible List rows back to stored
commands. Its allocation now checks both the host `size_t` limit and Xt's
`Cardinal` allocation limit before calling `XtCalloc`. An unrepresentable item
count is rejected without changing the existing command model or callback ABI.

The palette now clears a released visible-index pointer before an oversized
request or allocation failure returns, so a later rebuild cannot free stale
storage. Invocation also verifies that a selected visible index still refers
to a current item. The public filter getter returns `NULL` if its SearchBox
child could not be constructed.

The source contract is checked by
`tests/automated/check-command-palette-visible-allocation.py`.
The related focused checks are
`tests/automated/check-command-palette-selection-bounds.py` and
`tests/automated/check-command-palette-filter-child-guard.py`.

## Composite child geometry requests

`XmToast`, `XmCommandPalette`, and `XmSidebar` now report an accepted child
geometry request (including a child retrying the parent's proposal) as
`XtGeometryDone`, and a matching query-only request as `XtGeometryYes`.  They
continue to return `XtGeometryAlmost` only when the parent's layout requires
different dimensions.

This lets ordinary Xt children and application subclasses distinguish an
accepted request from an actual counterproposal.  The controls retain their
existing layout ownership and reject unsupported position, border, sibling, and
stacking requests.

The installed-prefix checks are exercised by:

    MOTIF_TEST_PREFIX=/path/to/prefix python3 tests/automated/run-toast.py
    MOTIF_TEST_PREFIX=/path/to/prefix python3 tests/automated/run-command-palette.py
    MOTIF_TEST_PREFIX=/path/to/prefix python3 tests/automated/run-sidebar.py

## Composite geometry replies

Sidebar, Toolbar, StatusBar, BreadcrumbBar, and CommandPalette calculate their
same existing natural width and height.  Their `QueryGeometry` methods now use
`XmeReplyToQueryGeometry` to compare those values against the request-mode
bits supplied by the parent.  Width-only, height-only, and query-only
negotiations therefore no longer depend on unrequested fields in an
`XtWidgetGeometry` record.

The same request-mode-aware reply contract also applies to Disclosure,
EmptyState, Flyout, FormSection, RowActions, and Toast.  Their existing
natural dimensions and popup sizing policies remain unchanged.

DropHighlight and Skeleton also use that contract.  A Skeleton therefore no
longer unconditionally accepts a parent size that differs from its current or
default preferred dimensions.

DropHighlight calculates its label-and-border preferred size in a wide
intermediate and saturates it before assigning Motif's legacy `Dimension`
fields, so unusually large labels or border resources cannot wrap to a small
geometry offer.

## Compound-text compatibility length tracking

The legacy `XmCvtCTToXmString(char *)` entry point receives a NUL-terminated
string rather than an explicit byte count.  Its parser now records that bound
before processing, rejects NULL input, and checks the optional four-byte
version header against it.

This makes the compatibility path safe for short or absent input while
preserving valid conversion behavior and the public ABI.  The static
regression check is `tests/automated/check-compound-text-compatibility-length.py`.

## Compound-text control-sequence bounds

Malformed compound text can end an ESC or CSI sequence before its second byte.
The dispatchers now reject those truncated sequences before indexing the
sequence buffer.

Valid control sequences and the public Motif ABI are unchanged.  The static
regression check is `tests/automated/check-compound-text-control-sequence-bounds.py`.

## Compound-text empty-segment guard

Compound-text decoding can produce an empty segment before a separator.  The
non-paired charset fallback previously inspected `ctx->item[0]` unconditionally
and could read beyond a zero-length segment.

The encoder now emits the empty segment through the existing concatenation path
and preserves separator behavior.  Non-empty segments and the public Motif ABI
are unchanged.  The static regression check is
`tests/automated/check-compound-text-empty-segment.py`.

## Compound-text extended-segment bounds

The compatibility compound-text parser scans an extended segment for its
charset/text separator.  Malformed input without that separator previously
continued past the declared segment length.

The scan now stops at the segment boundary and rejects the segment when the
separator is absent.  The two-octet length header is also checked before
access.  Valid extended segments and the public Motif ABI are unchanged.  The static regression check is
`tests/automated/check-compound-text-extended-segment-bounds.py`.

## Compound-text segment lengths

Compound-text decoding now checks the combined encoding-prefix and segment
length before allocating or passing the size to Xlib. An unrepresentable
length is rejected cleanly; ordinary `XmRString` to `XmRXmString` conversion
remains unchanged.

The compound-text encoding registry export also checks the aggregate size of
all tag and encoding pairs before allocating the NUL-separated target. An
unrepresentable registry returns no target data instead of wrapping its length
or allocation size; see `tests/automated/check-encoding-registry-bounds.py`.

## Mwm config-file path bounds

Mwm searches several resource, locale, and system configuration paths in a
fixed `MAXWMPATH` buffer. `FopenConfigFile` now builds that buffer through
checked set/append helpers using `snprintf`; failed or unrepresentable paths
clear the candidate instead of allowing `strcpy`/`strncat` truncation or size
underflow. The existing search order and filenames remain unchanged.

`tests/automated/check-config-path-bounds.py` guards the helper contract and
ensures the config-file search has no direct unbounded copies.

## constructor-argument-allocation-failure
Convenience-constructor allocation failures
============================================

The argument arrays used by the Command and message-dialog convenience
constructors are temporary allocations owned by the constructor.  If Xt
cannot provide that storage, the constructors now return `NULL` before the
array is copied or indexed.  This keeps allocation failure on the normal
widget-creation error path instead of dereferencing a null temporary array.

The change does not alter the public argument-list ABI or the successful
creation path.

## Container cell geometry

Grid and cell layouts derive their cell counts by subtracting twice the
container margins from unsigned widget dimensions. A container narrower than
its margins could wrap those values before division, producing a huge cell
count and unnecessary placement work.

The layout now performs the subtraction in signed arithmetic and exits when
either available dimension cannot contain a cell. Normal grids keep their
existing counts while collapsed containers remain stable.

## Container cell-array growth

When a Container changes only its minor layout dimension, the cell array is
grown in place. Growth now checks the cell-count arithmetic against host
`size_t` and Xt's `Cardinal` limits and uses a temporary pointer for
`XtRealloc`; allocation failure leaves the existing cell array and count
intact, so the next layout pass can retry safely.

The focused regression is `tests/automated/check-container-cell-growth.py`.
It checks the overflow guards, temporary allocation, failure check, and the
absence of the old direct assignment pattern.

## Container cell index bounds

Container coordinate conversion can briefly see an invalid cell while a
closest-placement request or geometry reset is in progress.  Closest
placement now rejects negative and still-out-of-range cells before indexing
the occupancy array.  Cell-to-coordinate conversion and `SnapCwid` also
return the container margin origin for invalid cells, preventing negative
coordinates and stale-grid dereferences while preserving valid placement.

`tests/automated/check-container-cell-index-bounds.py` protects these
contracts.

## Container cell occupancy counts

Container removal decrements the occupancy count for the child cell.  A
partially rebuilt grid can temporarily disagree with a child's recorded cell,
so removal now decrements only a valid, positive counter.  Valid grids retain
the existing behavior while stale state cannot produce a negative occupancy
count that would make later placement treat a busy cell as free.

`tests/automated/check-container-cell-occupancy-underflow.py` protects this
invariant.

## Container cell placement guards

Container placement and hit testing can run while a geometry reset is
incomplete, or after a coordinate falls outside the current cell grid.  The
occupancy array is now checked before placement and removal, and hit testing
rejects `NO_CELL` and out-of-range indices before indexing it.  This keeps a
failed allocation or transient layout state from becoming a null or
out-of-bounds access while preserving the existing placement result for valid
grids.

`tests/automated/check-container-cell-placement-guards.py` protects these
contracts.

## Container CELLS region lifecycle

`XmCELLS` placement tracks occupied rectangles in a persistent X11 region.
That region can fail to allocate during widget creation, and teardown or a
later layout reset must tolerate the missing state.  CELLS hit testing and
placement now fail safely when the region is unavailable, while destruction
and reset skip null-region operations.  GRID and valid CELLS behavior remain
unchanged.

`tests/automated/check-container-cells-region-lifecycle.py` protects these
allocation and lifecycle contracts.

## Container child-list allocation

`XmContainerGetItemChildren` snapshots the linked children of a container into
an Xt-allocated array returned to the caller. The byte-count guard covers both
host `size_t` and Xt `Cardinal` limits, passes the checked byte count explicitly
to Xt, and is followed by an allocation check;
a failed snapshot unlocks the application context and returns zero without
touching the missing array.

## Container conversion allocation guards

Container selection/drag conversion now checks target arrays, drag offsets,
selected-widget snapshots, and pixmap result arrays. Empty or unrepresentable
selection state refuses conversion before dereferencing a missing result.

The focused regression is
`tests/automated/check-container-conversion-allocation.py`.

## Container detail-header cache guard

When an icon header changes, Container now clears its old cached detail-header
table before allocating the replacement. Allocation failure leaves the cache
empty and returns safely instead of retaining a dangling pointer and writing
through it.

## Container detail-header allocation

Container copies icon detail headers into an internal table with a leading
label entry. The count is now checked against both host `size_t` and Xt's
`Cardinal` width before adding that entry and multiplying by the `XmString`
pointer size; an unrepresentable table is rejected without a wrapped
allocation.

The same checked multiplication is used when copying the `XmNdetailOrder`
array during initialization and `SetValues`.

`XmContainerGetItemChildren` now initializes its output to `NULL` and safely
returns zero for a missing output pointer or an unrepresentable child count.

## Container detail-order allocation guards

Container now checks detail-order array allocations both while initializing a
widget and while applying resource changes. The byte products are checked
against both host `size_t` and Xt's `Cardinal` width. A failed or
unrepresentable copy clears the array and count, leaving the widget in a valid
default-order state instead of dereferencing a null pointer.

## Container drag-icon pixmap metadata

Container drag-icon creation now initializes the width, height, and depth
outputs passed to `XmeGetPixmapData`.  An invalid or unresolved pixmap thus
produces zero-sized metadata instead of propagating uninitialized values into
the drag-icon resources.  Valid pixmap metadata follows the existing path.

The focused regression is
`tests/automated/check-container-drag-pixmap-metadata.py`.

## Container empty-heading guard

Container detail headings can be absent while resources are rebuilt, and an
application may intentionally provide an empty heading list.  Icon-header
creation now leaves the header absent in that state rather than asserting.  If
allocation fails during a valid update, the follow-on management and layout
steps likewise skip the missing header instead of asserting.

Valid heading arrays still create and update the same icon header.  The public
Container resources and ABI are unchanged.  The static regression check is
`tests/automated/check-container-empty-heading.py`.

## Container and Notebook control-flow clarity

Container placement, icon-header access, and traversal now make the scope of
their guarded statements explicit.  Notebook tab selection likewise brackets
the active major and minor tab switches.  These changes preserve established
layout and traversal behavior while ensuring that maintenance edits cannot
silently move coordinate updates, constraint reads, or page-loop advancement
into the wrong conditional branch.

The focused regression check verifies the relevant source scopes.  The native
library compilation is also run with `-Werror=misleading-indentation` for the
two affected translation units.

The adjacent Container paths also avoid retaining locals whose values are
never observed.  The geometry-manager request is still issued and explicitly
discarded, while managed-child and traversal code no longer suggests state that
does not exist.  A focused check and native compilation with unused-local
diagnostics promoted to errors guard that contract.

## Container outline allocation rollback

Container detail layout now allocates a replacement outline segment array
before releasing the current one. If allocation fails, the current outline
remains available and the layout is marked not ready for this pass instead of
discarding valid geometry.

The focused source regression is
`tests/automated/check-container-outline-allocation.py`.

## Container outline count bounds

Container detail layout now rejects outline-segment counts that cannot fit in
the legacy `int` count before adding or allocating segments. The current
outline remains intact and the pass is marked incomplete rather than wrapping
the count into a small or negative allocation.

The focused source regression is
`tests/automated/check-container-outline-count-bounds.py`.

## Container region allocation

Removing a child from a `XmCELLS` Container temporarily allocates an X11
region.  Region allocation can fail under memory pressure; removal now skips
the geometric subtraction when either the temporary region or the persistent
cell region is unavailable, while continuing to release the child occupancy
state.  This prevents an allocation failure from becoming a null dereference.

`tests/automated/check-container-region-allocation.py` protects this failure
path.

## Container reorder allocation

`XmContainerReorder` snapshots position indexes into a temporary integer array
before resequencing sibling items. The public entry point now rejects a null
widget list and checks both host arithmetic and Xt's `Cardinal` width before
sorting or dereferencing it. Failure releases the application lock and leaves
the current order unchanged.

## Container selection callback allocation guards

Container selection callbacks now tolerate allocation failure. Single-item
callbacks report zero selected items when their temporary array cannot be
allocated, and multi-item collection rejects oversized requests, checks the
allocation against both `size_t` and Xt `Cardinal` byte limits, and frees an
incomplete result before returning. The checked multi-item byte count is passed
explicitly as an Xt `Cardinal` value.

## Container dynamic tab-pool allocation

Dynamic detail-tab generation now checks both the temporary tab-array
allocation and every `XmTabCreate` result. The array size is checked against
host `size_t` and Xt's `Cardinal` width. If a tab cannot be created, the
already-created tabs and array are released and the Container reports that no
dynamic tab list is available. Existing per-call ownership and tab-list
compatibility remain unchanged.

`tests/automated/check-container-tab-pool-guards.py` protects this cleanup
path.

## Container dynamic tab-list rebuild failures

`XmContainer` now builds a replacement dynamic detail tab list before releasing
the current one. If tab allocation fails, the current list remains available so
the container can keep its last usable column layout. Geometry that is not yet
valid still returns no dynamic list as before.

## Container dynamic tab width geometry

Dynamic detail tabs derive their available width by subtracting twice the
container margin and the first-column width from the unsigned widget width.
The old expression performed those subtractions as unsigned arithmetic before
assigning to an `int`; a narrow container could therefore appear to have a
large positive tab width and allocate an unusable tab list.

The calculation now converts each dimension to signed arithmetic before
subtraction. The existing non-positive checks then discard the tab list when
the container cannot provide usable detail space.

## Container zero-sized cell guards

Container dynamic sizing can temporarily report a zero cell width or height
before a child has supplied measurements.  Grid/cell layout now returns without
performing coordinate division until both dimensions are positive.  Snapping
keeps the already computed point, while layout defers placement until a later
pass can measure the cells.

The focused regression is
`tests/automated/check-container-zero-cell-guards.py`.  It checks both
`SnapCwid` and `LayoutSpatial` for guards before their cell-width and
cell-height divisions.

## Control timer and teardown guards

Several additive controls receive input or deferred delivery after an Xt
destruction sequence has begun.  Their public entry points now reject that
state without examining child data or scheduling more work.

- Badge and SegmentedControl return their established neutral failure values
  from affected public queries and mutators.
- Link, Toast, TeachingTip, and Spinner reject late event or timer delivery.
- A delayed TeachingTip also checks that its owner and popup shell are still
  alive before using them.
- Toast cancels and clears an old timeout identifier before starting a new
  display interval, and changing `XmNtoastTimeout` while shown restarts that
  interval using the replacement value.

These changes preserve normal control behavior while making repeated show,
pause, resource-update, and destruction sequences deterministic.

## Converter destructor input validation

Converter cleanup callbacks now return safely when Xt supplies no destination
value or address. Valid cached string, font, table, tab-list, and render-table
cleanup remains unchanged.

The focused regression check is
`tests/automated/check-converter-destructors.py`.

## CutPaste conversion allocation guards

The legacy CutPaste converter now validates clipboard target counts before
computing the target-array size and checks both target-array and timestamp
allocations. Conversion reports failure cleanly when the request cannot be
represented or memory is unavailable.

The focused regression is
`tests/automated/check-cutpaste-conversion-allocation.py`.

## Data-grid text-length bounds

`XmDataGrid` now bounds cell and header text lengths before passing them to
`XDrawString`. Normal rendering is unchanged; unusually long strings no longer
wrap the signed Xlib length argument.

The focused regression check is
`tests/automated/check-data-grid-text-bounds.py`.

## DataField destination ownership

When a DataField is asked to relinquish its destination, Motif now calls
`XtDisownSelection` only if that widget actually owns the destination
selection.  This prevents an unrelated destination owner from being disowned
and avoids clearing the display-wide destination pointer for another widget.
Normal destination acquisition and release behavior is unchanged.

The warning-enabled DataField build verifies the corrected branch structure;
the DataField Xvfb regression remains the runtime smoke check.

## DataField DnD callback inputs

The legacy DataField drag-motion and drop callbacks now validate both the
widget and callback payload before reading drag context, drop action, or
coordinates. Malformed or teardown-time DnD notifications therefore return
safely.

Regression coverage: `tests/automated/check-datafield-dnd-callback-inputs.py`.

## DataField DnD drag context

DataField drag-motion and drop callbacks now reject callback payloads without
a drag context before passing that context to Xt or Motif transfer APIs.

Regression coverage: `tests/automated/check-datafield-dnd-context.py`.

## DataField DnD cleanup widgets

DataField DnD destroy cleanup now checks the callback widget before deleting
its drop context. Completion during widget teardown remains safe while the
client-data allocation is still released.

Regression coverage: `tests/automated/check-datafield-dnd-destroy-widget.py`.

## DataField Unicode drawing lengths

DataField Unicode drawing now checks length multiplication before allocating
temporary conversion buffers and passes the checked `size_t` capacity to
`wcstombs`. Oversized or negative lengths return without overflowing legacy
`int` and `Cardinal` conversions.

Regression coverage: `tests/automated/check-datafield-draw-length.py`.

## DataField drawing conversion

Wide-character DataField rendering now preserves `wcstombs` results as
`size_t` and rejects conversion failure or lengths that cannot be passed to
the legacy Xft/Xlib drawing APIs. Invalid text is skipped instead of being
drawn with a wrapped length.

Regression coverage:

```text
python3 tests/automated/check-datafield-drawing-conversion.py
```

## DataField drop-transfer length bounds

DataField drop transfers can arrive with an X11 selection length wider than
the widget's legacy `int` text API.  The callback now accumulates converted
payload sizes in `size_t`, rejects values that cannot be represented by the
DataField API, and checks the wide-character allocation and conversion result
before replacing text. Ordinary drops retain their existing behavior.

`tests/automated/check-datafield-drop-length-bounds.py` is the focused source
regression for this boundary.

## DataField malformed transfer metadata

DataField drop-transfer callbacks now release incoming values even when target
metadata pointers are malformed. When a widget is available, the callback also
marks the transfer failed before returning.

Regression coverage: `tests/automated/check-datafield-drop-metadata-cleanup.py`.

## DataField drop-transfer teardown

When a DataField drop-transfer closure no longer has a widget, the callback
now marks the transfer failed and releases any incoming value before returning.
This avoids leaking selection data during widget teardown.

Regression coverage: `tests/automated/check-datafield-drop-transfer-cleanup.py`.

## DataField drop-transfer inputs

DataField drop-transfer callbacks now validate the widget, transfer closure,
target type, length pointer, and transfer widget before reading or converting
incoming data. Malformed transfer notifications therefore return safely.

Regression coverage: `tests/automated/check-datafield-drop-transfer-inputs.py`.

## DataField drop-transfer allocation

`df_HandleDrop` now handles failure to allocate its drop-transfer request
record by starting a failure transfer with zero transfers. The callback state
is never dereferenced or registered after allocation failure, and the drop
protocol receives an explicit failure result.

The focused source contract is checked by
`tests/automated/check-datafield-drop-transfer.py`.

## DataField empty substring

`XmDataFieldGetSubstring` now treats a zero-length, in-range request as a
successful copy and writes an empty string to the caller's buffer. This matches
the wide-character API and keeps empty field selections usable.

The focused source check is
`tests/automated/check-datafield-substring-range.py`.

## DataField export targets

DataField drop handling now rejects a drag context that reports export targets
but supplies no target array. The drop transfer is marked failed before any
target iteration occurs.

Regression coverage: `tests/automated/check-datafield-export-targets.py`.

## DataField GC context allocation

`df_GetTextFGCData` now checks its per-screen context record and GC-data
allocations before initialization or X context registration. A failed second
allocation releases the temporary context record and returns `NULL`, avoiding
partial registration and null dereferences.

The focused source contract is checked by
`tests/automated/check-datafield-gc-context.py`.

## DataField graphics-context resources

`df_LoadGCs` now handles failed `TextFGCData` lookup and failed save, text, or
image GC allocation. Earlier GCs are released when a later allocation fails,
and no context record is dereferenced when lookup cannot provide one.

The focused source contract is checked by
`tests/automated/check-datafield-gc-resources.py`.

## DataField replacement growth arithmetic

Replacement growth now checks the resulting character count, including the
signed length-field range, wide-character byte multiplication, doubled
capacity, and legacy allocation field limits before reallocating. Overflow or
unrepresentable growth fails the replacement without changing the existing
buffer.

Regression coverage: `tests/automated/check-datafield-growth-arithmetic.py`.

## DataField keyboard conversion

DataField keyboard insertion now checks `mbstowcs` failure for both individual
key input and action-string insertion. Invalid multibyte input is rejected
without passing a negative character count to replacement, and the insertion
point is restored before returning.

Regression coverage:

```text
python3 tests/automated/check-datafield-input-conversion.py
```

## DataField insert-string action bounds

The `insert-string` action now checks the complete action parameter before
copying it into the fixed-size conversion buffer.  Oversized parameters are
ignored safely, while normal action strings retain their existing filtering,
conversion, replacement, and callback behavior.

`tests/automated/check-datafield-insert-action.c` exercises the oversized
parameter case against the installed library.

## DataField invalid wide-character warnings

Invalid wide-character conversion results can be negative, and a successful
conversion can fill the complete `MB_LEN_MAX` buffer. The warning path now
terminates its scratch buffer only when the returned length is in range;
otherwise it emits an empty diagnostic safely.

Regression coverage: `tests/automated/check-datafield-invalid-wchar-warning.py`.

## DataField modify-verify cleanup

DataField now validates the wide modify-verify callback's text record and
payload before reading it. Invalid callback data follows the existing cleanup
label, releasing any temporary narrow or wide conversion buffers instead of
returning early with leaked state or dereferencing a null record.

Regression coverage: `tests/automated/check-datafield-modify-verify-cleanup.py`.

## DataField modify-verify conversion

The DataField modify-verify bridge now treats `wcstombs` and `mbstowcs`
results as `size_t`, as required by the C library contract. Conversion failure
(`(size_t)-1`) and lengths that cannot fit Motif's signed callback lengths are
rejected instead of being tested as negative `int` values or wrapped into a
small length. Invalid insertion lengths are rejected before allocation and
character-width multiplication.

Regression coverage:

```text
python3 tests/automated/check-datafield-modify-verify-conversion.py
```

## DataField modify-verify result allocation

DataField narrow and wide modify-verify callback results now check the
allocation before copying or converting the returned buffer. Callback records
and lengths are validated first. Allocation failure clears the temporary result
and follows the existing cleanup path, so it cannot become a null write or
leak the callback-conversion buffer. The wide result size is formed in
`size_t` before narrowing to Xt's `Cardinal`.

Regression coverage: `tests/automated/check-datafield-modify-verify-result-allocation.py`.

## DataField modify-verify temporary sizes

The wide-character modify-verify path now checks string length, character
width, multiplication, and Xt allocation width before creating its temporary
multibyte buffer. Invalid capacities fail the verification safely instead of
wrapping into a small allocation.

Regression coverage: `tests/automated/check-datafield-modify-verify-size.py`.

## DataField modify-verify snapshot conversion

Before invoking modify-verify callbacks, a wide DataField value is copied into
a temporary multibyte snapshot. That conversion now checks `wcstombs` for
failure and for output that consumes the entire buffer, explicitly terminates
valid output, and releases the snapshot on every path. Allocation-failure
paths also start with initialized pointers, so cleanup cannot inspect
uninitialized state.

Regression coverage:

```text
python3 tests/automated/check-datafield-modify-verify-snapshot.py
```

## DataField paste conversion safety

`XmDataFieldPaste` now bounds the accumulated clipboard strings before narrowing
them to Xt's `Cardinal` allocation size. Both narrow and wide conversion
buffers handle allocation failure without dereferencing a null pointer. Wide
conversion also checks `mbstowcs` using its `size_t` error sentinel and rejects
character counts that cannot be passed through the existing `int` replacement
API.

The focused source contract is checked by
`tests/automated/check-datafield-paste-conversion.py`.

## DataField picture verification allocation

`PictureVerifyCallback` now rejects a failed splice-buffer allocation before
copying the current value or proposed edit into it. The callback marks the
edit as rejected and releases the current string, preserving the callback's
existing failure semantics without dereferencing a null buffer.

The focused source contract is checked by
`tests/automated/check-datafield-picture-allocation.py`.

## DataField picture splice buffer bounds

Picture verification now sizes its temporary splice buffer in bytes, using
the actual character length rather than pointer-size multiplication.  It
rejects malformed negative lengths and detects size addition overflow before
allocation, while preserving accepted and rejected picture-edit behavior.

The warning-enabled DataField build and Xvfb DataField regression provide the
native smoke checks for this path.

## DataField picture callback input

`PictureVerifyCallback` now validates its widget and callback data before
reading the text verification record. A missing widget or callback record is
ignored, while a record without its text edit payload is rejected by clearing
`doit`. This keeps malformed embedder callbacks from dereferencing null input
and leaves normal picture verification unchanged.

The focused source contract is checked by
`tests/automated/check-datafield-picture-callback-input.py`.

## DataField picture initialization

During widget initialization, DataField now transfers ownership of a picture
source only after `XtNewString` succeeds. Allocation failure disables the
picture and modify-verify callback cleanly instead of parsing or later freeing
a null/non-owned pointer.

The focused source contract is checked by
`tests/automated/check-datafield-picture-init.py`.

## DataField picture-source ownership

DataField `SetValues` now duplicates a changed picture source before releasing
the old owned string. If `XtNewString` fails, the old source remains owned and
the new widget state reuses it instead of losing the picture definition or
freeing storage that is still needed.

The focused source contract is checked by
`tests/automated/check-datafield-picture-source.py`.

## DataField picture verification lifetime

Picture verification now releases the temporary current-value string on both
accepted and rejected edits.  Rejected edits also release the splice buffer
and picture state before returning.  This keeps repeated picture edits from
leaking widget-owned conversion state while preserving validation callbacks,
autofill behavior, and the public DataField interface.

The warning-enabled DataField build and the DataField action regression are
the native smoke checks for this path.

## DataField pixel measurement lengths

DataField pixel measurement now checks temporary Unicode conversion capacity
before calling `wcstombs` in Xft and core-font paths. Oversized or negative
lengths return a zero measurement instead of overflowing the temporary buffer.

Regression coverage: `tests/automated/check-datafield-pixel-length.py`.

## DataField pixmap context allocation

`df_LoadGCs` now checks the per-screen pixmap context record before writing its
metadata or registering the destruction callback. If allocation fails, the
helper returns without dereferencing a null context record.

The focused source contract is checked by
`tests/automated/check-datafield-pixmap-context.py`.

## DataField cursor pixmap names

DataField caches its I-beam and add-mode cursors under names containing the
cursor height and line width.  The old I-beam buffer was only 17 bytes even
though `_XmDataF_%d_%d` needs more space for ordinary 16-bit dimensions; the
unbounded `sprintf` could overwrite the stack when geometry values grew.

Both cursor-name paths now use a 64-byte buffer and bounded `snprintf` writes.
The cache keys and cursor behavior are unchanged for normal values, while
large values are truncated safely rather than corrupting the widget stack.

`tests/automated/check-datafield-pixmap-names.py` protects the storage and
formatting contract.

## DataField portability warnings

DataField passed a null pointer through an integer parameter cast when
registering its input method, and initialized a wide-character value by
casting a pointer constant to `wchar_t`.  Those expressions are undefined or
diagnostic-prone on 64-bit builds even though they happened to produce zero on
common targets.

The calls now pass integer zero to the unsigned `XmImRegister` parameter and
wide-character zero to the `wchar_t` storage.  Runtime behavior and the public
ABI are unchanged, while ARM64 and other 64-bit compilers no longer need to
diagnose these paths.

`tests/automated/check-datafield-portability-warnings.py` protects the
type-correct constants.

## DataField primary-selection conversion allocation

The narrow primary-selection conversion in `df_DoStuff` now checks its
aggregate buffer before writing the concatenated text. On allocation failure,
the converted text list, callback value, and request state are released, and a
temporary selection-move max-length override is restored.

The focused source contract is checked by
`tests/automated/check-datafield-primary-conversion.py`.

## DataField primary-selection fallback bounds

The raw wide primary-selection fallback now rejects lengths that cannot pass
through the existing `int` replacement API or cannot be multiplied safely for
an Xt `Cardinal` allocation. Rejection restores temporary selection state and
releases the callback request records before returning.

The focused source contract is checked by
`tests/automated/check-datafield-primary-fallback-bounds.py`.

## DataField raw primary-selection fallback

The raw wide-character primary-selection fallback now checks its conversion
buffer before calling `mbstowcs`. If allocation fails, the temporary
selection-move max-length override and callback state are restored/released
before returning, avoiding a null dereference.

The focused source contract is checked by
`tests/automated/check-datafield-primary-fallback.py`.

## DataField primary-selection request allocation

The primary-selection request path now checks its `_XmTextPrimSelect`
allocation before initializing the request state. If allocation fails, the
selection target result and temporary action/event records are released before
returning, preventing both a null dereference and a leaked request snapshot.

The focused source contract is checked by
`tests/automated/check-datafield-primary-selection.py`.

## DataField primary-selection wide conversion allocation

The wide primary-selection conversion in `df_DoStuff` now checks its temporary
wide buffer before conversion. On allocation failure it releases the converted
text list, callback value, and request state, and restores the temporary
selection-move max-length override.

The focused source contract is checked by
`tests/automated/check-datafield-primary-wide-conversion.py`.

## DataField printable-text progress

DataField printable-text validation now rejects zero-length multibyte
decodings as well as negative `mblen()` results.  This guarantees that the
validation loop always advances or returns failure, preventing a hang on NUL
or malformed locale input.  Valid printable text and wide-character checks
retain their existing behavior.

The warning-enabled DataField build and Xvfb DataField regression are the
native smoke checks for this path.

## DataField public conversion bounds

The public wide-string setter now terminates only successful conversions and
turns a failed or capacity-filling `wcstombs` result into an empty value
before handing it to the normal setter. Selection export rejects the same
capacity-filling result before indexing its output buffer, returning `NULL`
instead of writing past the allocation.

Regression coverage:

```text
python3 tests/automated/check-datafield-public-conversion-bounds.py
```

## DataField replacement allocation failures

Text replacement now keeps the existing character buffer intact when
`XtRealloc` fails. The temporary result is checked before replacing the live
widget pointer, and the operation reports failure without losing existing
text.

Regression coverage: `tests/automated/check-datafield-realloc-failure.py`.

## DataField selection-array allocation

`df_Validates` now checks the dedicated selection-scan array allocation before
copying the configured entries. If allocation fails, it restores the original
resource/static array pointer and returns without a null `memcpy`.

The focused source contract is checked by
`tests/automated/check-datafield-selection-array.py`.

## DataField selection callback inputs

The DataField selection-reply callback now validates its widget, selection
closure, type, length, format, and non-empty payload before using them. It
defers display access until validation succeeds, releases rejected Xt-owned
data, and balances the pending selection reference on every rejected path.
Non-empty data tagged with the delete/NULL target atom is rejected as an
inconsistent reply rather than being interpreted as a delete operation.
Multivalued converted text uses checked `size_t` accumulation and rejects
unrepresentable narrow or wide allocations instead of overflowing an `int`.
Wide-character conversion now rejects `mbstowcs` failure and unrepresentable
character counts before updating selection state.
The temporary converted string list is released on both successful and failed
wide-character conversion paths. Conversion capacity is expressed in wchar
slots rather than raw allocation bytes.

Regression coverage:

```text
python3 tests/automated/check-datafield-selection-callback-inputs.py
```

## DataField selection export conversion

When exporting a multibyte DataField selection, the conversion loop now treats
zero or negative `mblen()` results as a conversion failure instead of treating
them as progress.  This prevents an invalid locale sequence from causing an
infinite loop or unsigned length underflow.  Valid multibyte selections retain
their existing exported text and ownership behavior.

The `check-datafield-wcs` regression covers valid UTF-8 selection export after
the wide access-text conversion checks.

## DataField selection lengths

DataField selection conversion now rejects external byte lengths that cannot
be represented by its legacy `int` replacement-length API. Oversized values
are discarded without truncating the replacement count.

Regression coverage: `tests/automated/check-datafield-selection-length.py`.

## DataField selection-target allocation failures

`XmDataField` allocates response values for the `TARGETS` and `TIMESTAMP`
selection targets.  Those allocations now return `False` cleanly when memory
is unavailable, instead of dereferencing a null pointer while answering an
X11 selection request.  Existing successful responses and their ABI remain
unchanged.

The focused regression is
`tests/automated/check-datafield-selection-target-allocation.py`.

## DataField selection-value allocation bounds

Text, locale, and compound-text selection responses allocate a temporary
buffer before copying or converting the selected value.  Those paths now share
a checked helper that rejects lengths outside the `Cardinal` range and returns
cleanly on allocation failure.  This prevents a truncated allocation or a
null dereference while preserving the existing selection formats and ABI.

The focused regression is
`tests/automated/check-datafield-selection-value-allocation.py`.

## DataField wide selection export bounds

`XmDataFieldGetSelectionWcs` now checks the selection length before multiplying
by `sizeof(wchar_t)`.  A range that cannot be represented by the allocation
size fails cleanly and releases the application lock; valid wide selections
retain their existing contents and termination behavior.

The DataField wide-text regression covers both UTF-8 selection export and WCS
selection export after rebuilding the library.

## DataField SetValues selection-array replacement

When a new selection-scan array is supplied through `SetValues`, DataField now
allocates the dedicated replacement before releasing the old widget array. A
failed allocation leaves the old array and the new resource pointer valid,
avoiding a null `memcpy` and preventing loss of the existing widget state.

The focused source contract is checked by
`tests/automated/check-datafield-setvalues-selection-array.py`.

## DataField string getter guards

DataField's narrow and wide synthetic string exports now validate widget and
output storage before requesting a copied value. Invalid calls return a
deterministic null result; valid string and wide-string exports are unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-datafield-string-getter-guards.py
```

## DataField wide-string conversion failures

`XmDataFieldGetStringWcs` now returns a valid empty wide string when the
current multibyte value cannot be converted in the active locale. This avoids
returning an unterminated or partially initialized buffer after `mbstowcs`
reports an encoding error.

Regression coverage: `tests/automated/check-datafield-string-wcs-conversion.py`.

## DataField wide-string allocation sizes

`XmDataFieldGetStringWcs` now validates the document length before computing
the wide-character allocation size and before converting it to Xt's
allocation-width type. Oversized lengths return `NULL` instead of wrapping the
allocation size.

Regression coverage: `tests/automated/check-datafield-string-wcs-size.py`.

## DataField substring conversion

DataField substring exports now preserve the `size_t` return type of
`wcstombs`/`mbstowcs` and compare against the proper `(size_t)-1` failure
sentinel. Invalid multibyte conversions no longer appear successful because
an unsigned failure value was compared as a negative `int`.

Regression coverage:

```text
python3 tests/automated/check-datafield-substring-conversion.py
```

## DataField target callback inputs

The DataField target-list callback now validates its widget, selection atom,
closure, event, and target payload before using them. It releases malformed
Xt-owned target data and uses the full X11 selection-length width when walking
the target list. Display and atom lookup occur only after widget validation.
The follow-up primary-selection request uses the normalized event timestamp,
including keyboard-originated requests.
The action helper also returns before atom/display access when its widget is
missing.

Regression coverage:

```text
python3 tests/automated/check-datafield-target-callback-inputs.py
```

## DataField validation capacity arithmetic

DataField validation now bounds the extra storage reserved for validated
strings against both the legacy signed field and the Xt allocation width,
including the wide-character multiplication. This closes the remaining
near-limit wraparound after source-length conversion.

Regression coverage: `tests/automated/check-datafield-validate-capacity.py`.

## DataField validation lengths

`df_ValidateString` now rejects source strings whose byte or wide-character
length cannot be represented by its legacy integer fields or Xt allocation
width. This prevents truncation before validation and temporary-buffer sizing
for very large values.

Regression coverage: `tests/automated/check-datafield-validate-length.py`.

## DataField value conversion

When constructing a wide-character DataField value, `mbstowcs` failure or an
unrepresentable result now releases the allocated buffer and resets the value
state before returning. The widget no longer publishes a negative string
length after invalid multibyte input.
The wide-string setter also keeps the `wcstombs` result in its native
`size_t` width before checking failure.
The narrow/wide string setter also checks measured lengths and rejects invalid
multibyte input before narrowing to its legacy `int` callback length.
The public string and selected-text getters likewise preserve conversion
result widths and test the correct failure sentinel.

Regression coverage:

```text
python3 tests/automated/check-datafield-value-conversion.py
```

## DataField modify-verify allocation

When a DataField has a narrow-character modify-verify callback,
`df_ModifyVerify` creates a temporary text block for the callback. The helper
now returns `False` immediately if that allocation fails, before copying input
text into the missing buffer. This preserves the edit-rejection contract and
avoids a null dereference under memory pressure.

The focused source contract is checked by
`tests/automated/check-datafield-verify-allocation.py`.

## DataField warning formatting

`XmDataField` reports characters that cannot be represented by the active font.
Those diagnostics include localized message text and, for wide-character input,
a converted string. They are now sized with `snprintf` before allocation, so a
long translation cannot overwrite a fixed stack buffer. Allocation failure in
the formatter is still handled by the normal Xt allocator contract, and a
formatting failure falls back to the unformatted catalog message.

The source-contract regression is
`tests/automated/check-datafield-warning-bounds.py`. It verifies that all
validation warning paths use the bounded helpers and that the former fixed
buffer and unbounded `sprintf` calls are absent.

## DataField wide modify-verify allocation

`df_ModifyVerify` now checks the temporary wide-character callback buffer in
both conversion branches before `mbstowcs`, `memcpy`, or terminator writes.
When the narrow callback ran first, its temporary buffer is released before
rejecting the edit; the direct wide-input branch has no temporary ownership to
release. This keeps callback failure safe under allocation pressure.

The focused source contract is checked by
`tests/automated/check-datafield-wcs-verify-allocation.py`.

## DataField wide insertion allocation

The DataField modify-verify callback now validates the wide-character count
before evaluating `(insert_length + 1) * sizeof(wchar_t)`. Counts that cannot
fit the allocator's `Cardinal` size are rejected without wrapping the addition
or creating an undersized buffer.

Regression check:

```text
python3 tests/automated/check-datafield-wide-insert-allocation.py
```

## DataField wide-value export

When a wide DataField value is rebuilt into its narrow storage, the
`wcstombs` result is now checked before the buffer becomes the widget's live
value. Conversion failure and output that fills the allocation are rejected,
the owned buffer is released, and the value is reset instead of exposing
unterminated or partially converted text.

Regression coverage:

```text
python3 tests/automated/check-datafield-wide-value-export.py
```

## DataField insertion callback inputs

The DataField selection-insertion and target-list callbacks now reject an
incomplete widget, closure, selection atom, event, type, length, or format
tuple before dereferencing it. Any Xt-owned selection result supplied with
that malformed callback is released first. Normal insertion and empty-value
handling are unchanged. Target-list iteration keeps the X11 length width and
rejects a nonzero length without a target array. The consumed target array is
released before the follow-up selection request is queued. Empty insertion
results are discarded before inspecting their first byte, and display access
is deferred until after widget validation. Invalid X11 format widths and
non-empty typeless results are rejected before conversion.
Invalid multibyte input is also rejected when converting locale text to wide
characters, before replacement receives the character count. Both conversion
paths preserve the native `size_t` result before narrowing.

Regression coverage:

```text
python3 tests/automated/check-datafsel-insert-callback-inputs.py
```

## Data-field insert-selection property validation

`ConvertInsertSelection` receives an `ATOM_PAIR` property containing the
selection and target atoms.  The reader now requires the expected property
type, 32-bit format, and two items before casting the Xlib buffer to
`_XmTextInsertPair`.  Invalid or truncated selection requests are released
and rejected instead of being dereferenced as a short structure.  A buffer
returned alongside an Xlib error is released as well.

## DataField selection export conversion

Selection export for `XA_STRING`, locale text, and `COMPOUND_TEXT` now keeps
`wcstombs` results in `size_t` and checks the correct `(size_t)-1` failure
sentinel. Invalid wide selections are reported as empty conversion results
instead of being published with a wrapped length.

Regression coverage:

```text
python3 tests/automated/check-datafsel-selection-conversion.py
```

## DataGrid begin-edit rollback

`XmDataGridBeginEdit` now allocates the selected cell's edit text before
releasing an existing edit buffer or error. If allocation fails, the current
editing state remains intact and the operation reports failure.

The focused source regression is
`tests/automated/check-datagrid-begin-edit-rollback.py`.

## DataGrid live color updates

`XmDataGrid` now refreshes its drawing GC and requests a repaint when foreground
or background resources change through `SetValues`. Theme changes therefore
update existing data-view widgets without requiring recreation.

The focused source contract is checked by
`tests/automated/check-datagrid-color-updates.py`.

## DataGrid geometry negotiation

`XmDataGrid` uses 480x240 only as its initial fallback allocation.  Its Xt
`QueryGeometry` method now reports the widget's actual current dimensions and
uses `XmeReplyToQueryGeometry` to answer the caller's intended geometry.
Applications can therefore assign an explicit DataGrid size without a parent
query negotiating it back to the fallback dimensions.

## DataGrid render-table rollback

When a live DataGrid receives a new render table, it now keeps the previous
valid table if `XmFontListCopy` cannot allocate a replacement. The old table is
released only after a successful copy, preventing a transient allocation
failure from leaving the widget unable to render text.

The focused source contract is checked by
`tests/automated/check-datagrid-render-table-rollback.py`.

## DataGrid resize capacity

`XmDataGrid` derives visible-row capacity from its allocated height and uses
that capacity to clamp `XmNdataGridTopRow`.  Its core resize method now
recomputes that state after a parent geometry change.  The `SetValues` path
also requests repaint when width or height changes, keeping the header and
cell-column geometry current even when the resize originates in Xt resource
updates.

## DataGrid selection bounds

`XmDataGridGetSelectedCell` and `XmDataGridBeginEdit` now validate both sides
of the selected row/column pair against the current model counts. Stale
selection state after a model change is rejected safely instead of indexing
past the row or column arrays; valid selections behave as before. The focused
regression anchors on the function definitions so unrelated call sites cannot
produce a false failure.

## DataGrid text metrics

DataGrid cell and header text is measured with the active XmFontList through
`XmStringExtent`, then vertically centered before `XmStringDraw`. The focused
regression checks this render-table contract rather than the retired direct
`XQueryFont` path.

The check is implemented by
`tests/automated/check-datagrid-text-metrics.py`.

## DataGrid top-row bounds

DataGrid keyboard and wheel navigation now saturate `top_row` before passing it
to the existing viewport clamp. This prevents Page Down or wheel-down from
wrapping a large `Cardinal` row index back to the beginning. Selection-driven
visibility checks also subtract only after proving the row is at or below the
current top, avoiding an overflowing sum while preserving normal scrolling.

The source contract is checked by
`tests/automated/check-datagrid-top-row-overflow.py`.

## DatePicker callback guards

DatePicker validates both its callback closure and the calendar widget before
accepting a date-change notification. Foreign or stale callback inputs are
ignored safely while normal popup and date-change behavior remains unchanged.

## DatePicker popup destruction

The DatePicker registers a destroy callback on its transient popup shell.  If
the shell is destroyed by its parent or by application code, the DatePicker
clears both its popup-shell and calendar pointers and marks the popup closed.

This prevents a later `XmDatePickerOpen` call from reusing stale child widget
addresses.  Normal close and DatePicker teardown keep their existing behavior.

## DatePicker popup positioning

DatePicker computes popup coordinates in wide intermediates and clamps them to
the target screen before converting to X11 `Position` values. Large or
negative owner coordinates can no longer wrap and misplace the popup; the
final coordinates are also bounded by the signed `Position` representation.

## DatePicker range updates

`XmDatePickerSetRange` updates the optional calendar child before committing the
new range to the DatePicker itself.  If the child rejects the range, the API
returns `False` and the parent retains its previous minimum and maximum rather
than exposing a partially applied update.

Successful updates keep the calendar and DatePicker ranges synchronized.

## DatePicker resource synchronization

When an application changes the DatePicker year, month, or day resources with
`XtSetValues` while its popup calendar exists, the new date is now applied to
that calendar child as well.  The child update is performed without emitting a
second date-changed callback.

If the child cannot accept the requested date, the parent keeps its previous
date instead of displaying a state that disagrees with the popup.

## DatePicker popup teardown

DatePicker calendar callbacks and public open/close operations can overlap
popup destruction. They now check popup and calendar availability and the
popup shell's `being_destroyed` state before popping down or destroying it.

The contract is protected by
`tests/automated/check-datepicker-teardown-guards.py`.

## DatePicker event guards

DatePicker validates its widget type and event pointer before handling mouse
or keyboard activation. Invalid dispatch is ignored safely; normal popup
opening and closing for live pickers is unchanged.

## DatePicker popup creation

The DatePicker now checks lazy popup-shell and calendar creation before using
either widget.  If either creation fails, any partial shell is destroyed and
the picker remains closed; successful popup creation and calendar behavior are
unchanged.

`tests/automated/check-datepicker-popup-creation.py` protects this path.

## DatePicker popup positioning

DatePicker popup positioning now treats a popup wider or taller than its
screen as an oversized case and anchors that axis at zero.  Normal right and
bottom edge clamping is unchanged, while small-screen layouts no longer
receive negative coordinates.

`tests/automated/check-datepicker-popup-position.py` protects this geometry.

## DatePicker popup test contract

The DatePicker popup implementation uses wide `x_pos` and `y_pos` values
before clamping to the screen and converting to Motif `Position`. The focused
regression now checks those current contracts directly; it no longer expects
the removed narrow `Position` arithmetic.

The contract check is
`tests/automated/check-datepicker-popup-position-contract.py`.

## Default font-list cache allocation

The per-display default font-list cache now grows transactionally. Capacity is
updated only after the new cache block has been allocated and initialized; a
failed allocation returns `NULL` while preserving the existing cache and its
capacity.

## Default render-table entry parsing

`XmeGetDefaultRenderTable` now processes every comma-separated entry in the
default font specification. Previously, the loop stopped as soon as the first
entry was loaded because it treated a non-empty render table as a stop
condition. Existing fallback behavior for entries that fail to load is
unchanged.

The focused regression check is
`tests/automated/check-default-render-table-entries.py`.

## DeleteClientFromList guards

Client withdrawal can reach list cleanup after workspace state has been
partially detached.  `DeleteClientFromList` now validates the workspace,
client, and screen pointers before touching list links.  Under WSM it also
treats a missing workspace-client record as having no icon entry, while still
removing the valid global client-list links.

## DeleteEntryFromList guards

Window-list removal can occur after workspace teardown has begun.  The
low-level unlink helper now validates the workspace, screen, and list-entry
pointers before updating sibling links or screen list heads.  Normal unlinking
is unchanged for valid list state.

## Icon-info deletion guards

`DeleteIconInfo` now ignores missing clients, icon-box state, or empty
placement storage before scanning the list.  Valid deletion still clears the
matching icon record.

`tests/automated/check-delete-icon-info-guards.py` protects the contract.

## Icon-box deletion guards

`DeleteIconFromBox` now rejects incomplete icon-box/client geometry before Xt
queries or division, validates WSM workspace metadata, and skips teardown calls
when the child widget is already absent.  Valid deletion remains unchanged.

`tests/automated/check-delete-iconbox-guards.py` protects the contract.

## Data-field delete-selection request guard

The data-field `DELETE` selection conversion now returns `False` when no
active request event is available for either a primary-selection or drop
operation.  Normal requestor, timestamp, and replacement handling remains
unchanged, while out-of-context callbacks no longer dereference a null event.

Regression: `tests/automated/check-delete-selection-request-guard.py`

## Desktop child-list growth bounds

`XmDesktop` now checks `Cardinal` slot-count growth and both host `size_t` and
Xt `Cardinal` byte limits before reallocating its child list. It also preserves the old list
when `XtRealloc` fails. Existing insertion and deletion ordering is unchanged,
but extreme counts and allocation failures no longer risk wraparound or a
lost child list.

## Desktop color-cell capacity validation

`XmeGetDesktopColorCells` now rejects non-positive `n_colors` values before
performing color-object or X-server work. Positive capacities and the existing
optional result-count behavior remain unchanged.

The focused regression check is
`tests/automated/check-desktop-color-cells-capacity.py`.

## Destination handler allocation

Destination transfers now check the callback structure, pooled transfer
context, and drag-and-drop callback copy before initialization. Failed setup
returns `False` after releasing any partial transfer state, while
`GetTransferID` leaves its pool unchanged when a new context cannot be
allocated.

Successful selection and DnD transfers retain their existing callback and
ownership behavior.

## Destination handler inputs

`_XmDestinationHandler` is the common entry point for regular selection and
drop destination transfers.  It now rejects a null destination widget or a
`None` selection atom before touching the widget display or class.  This
keeps malformed or teardown-time external-owner callbacks from entering the
transfer state machine with no valid destination.

The regression check is
`tests/automated/check-destination-handler-inputs.py`.

## Dialog argument counts

Command and MessageBox convenience constructors add one internal argument to
the caller's list. They now reject maximum counts and byte sizes that cannot
represent the additional argument in the host or Xt allocation widths before
that addition and allocation can wrap. Non-empty lists also require a valid
source pointer; an empty null list remains valid. Normal argument lists and
dialog resources are unchanged.

## Dialog argument allocation bounds

`XmeCreateClassDialog` appends one internal shell argument to the caller's
argument list. It now rejects counts whose `(count + 1) * sizeof(Arg)` byte
size cannot fit both `size_t` and Xt's `Cardinal` allocator width before
allocating or copying the merged list.

Normal dialog construction and the public argument ABI are unchanged. The
regression is covered by `tests/automated/check-class-dialog-allocation.py`.

## Dialog child lookup widget guards

`XmSelectionBoxGetChild` and `XmMessageBoxGetChild` now reject non-matching
widget classes before entering application context or reading private child
fields. Valid dialog child identifiers and returned children are unchanged.

The source contract is checked by
`tests/automated/check-dialog-child-widget-class.py`.

## Dialog constructor argument counts

`XmeCreateClassDialog` adds one shell argument to the caller's argument list.
It now rejects the maximum `Cardinal` count before the addition and allocation
can wrap. Ordinary dialog construction is unchanged.

## Dialog parent-input guards

`XmCreateDialogShell` and `XmeCreateClassDialog` now return `NULL` when their
parent widget is null, before shell creation or argument allocation. This
prevents malformed dialog-construction requests from reaching Xt with an
invalid parent while preserving valid dialog behavior.

The focused regression is
`tests/automated/check-dialog-parent-inputs.py`.

## DialogShell resource merge guard

DialogShell class initialization now checks the merged resource-table size and
allocation. On failure it skips the optional merge and still runs the normal
class-initialization hook, avoiding a null write or integer-overflowed request.

## Dialog zero-argument construction

`XmeCreateClassDialog` now copies the caller’s argument list only when
`bb_n` is nonzero. A zero-argument dialog can therefore pass a null argument
list without invoking zero-length `memcpy` on a null pointer, while non-empty
argument lists retain the same behavior.

The focused regression is `tests/automated/check-dialog-zero-args.py`.

## DialogShell count bounds

DialogShell unmanage cleanup now uses Xt `Cardinal` counters for both the
child popup list and each nested shell's composite child list. Popup cleanup
ordering is unchanged while signed-counter wraparound is avoided for large
valid lists.

## DialogShell input-method geometry

DialogShell includes the input-method area in its shell-height negotiation.
That addition is now performed in a wide unsigned type and saturated before
assignment to Motif's legacy `Dimension` field.  The reverse adjustment to
the child height is guarded against underflow.  Ordinary input-method sizing
and shell negotiation are unchanged.

Default dialog placement also calculates parent-centering and display-edge
adjustments in a wide signed type, then saturates only at the `Position`
output boundary.  Large virtual desktops and shell dimensions therefore do
not wrap a centered dialog to an unrelated location.

Coordinate translation now occurs only when the parent is realized.  An
unrealized parent retains its already-calculated centered coordinates instead
of reading the uninitialized translation outputs, so creating a dialog before
its parent maps has deterministic placement.

## Layout-direction converter guards

The shared layout-direction default and import/export converters now reject
null widgets or null value storage before invoking widget-class predicates or
converting the direction. Invalid exports leave caller storage untouched;
invalid imports return `XmSYNTHETIC_LOAD` without changing widget state.

The focused source-contract check is:

```sh
python3 tests/automated/check-direction-converter-guards.py
```

## Disclosure child allocation

Disclosure creates indicator, header, and content children during
initialization.  Geometry, state transitions, callbacks, and resource updates
now tolerate an individual child failing to allocate, so later redraws and
resource changes remain safe.

The contract is protected by
`tests/automated/check-disclosure-child-failure-guards.py`.

## Disclosure position bounds

`XmDisclosure` lays out its arrow, header, and optional content from margins,
child dimensions, and spacing. Direct narrowing of those sums to `Position`
could wrap extreme geometry.

The layout now clamps each derived origin to the representable signed
`Position` range. Normal disclosure sections retain their existing geometry,
while unusually large resources cannot place children at unrelated locations.

The focused source regression is
`tests/automated/check-disclosure-position-bounds.py`.

## Selection-disown callback inputs

`DisownCallback` is registered on a widget's destruction callback while that
widget owns a selection.  Destruction and callback removal can race with
cleanup, so the callback now returns immediately when the widget pointer is
null or when its selection payload is `None`.  This keeps teardown from
dereferencing an invalid display or issuing an invalid `XtDisownSelection`
request, while preserving the normal path for a valid selection atom.

The regression check is
`tests/automated/check-disown-callback-inputs.py`.

## Divider coordinate bounds

Divider clamps its centered horizontal label before passing it to Motif's
signed `Position` drawing arguments.  Large divider dimensions or font metrics
therefore cannot wrap the label to an unrelated screen coordinate.

`tests/automated/check-divider-coordinate-bounds.py` protects this drawing
contract.

Divider line spans also pass through an explicit non-negative extent helper
before reaching X11's unsigned rectangle parameters.  Small allocations cannot
turn a signed intermediate into a very large painted span.

## Divider geometry bounds

`XmDivider` saturates both natural dimensions after measured text, spacing,
line thickness and scaled padding are combined. Initialization, live changes
and QueryGeometry share the same calculation, preventing Dimension wraparound
for large resource values. Explicit axes retain their application-owned sizes.

Drawing bounds line thickness to the cross-axis allocation and omits reversed
or empty segments when a label is wider than the available space. Text is
clipped to the widget. Native coverage in `run-divider-geometry.py` includes
maximum spacing/thickness, tiny allocations and exact labeled-line thickness;
`check-divider-label-width.py` additionally guards the saturated calculation.

## Divider label width

Divider preferred-width calculation now performs label-length multiplication
in a wide type and clamps it to the platform `Dimension` maximum. Long labels
no longer wrap to a narrow geometry request, while ordinary labels retain the
same sizing.

`tests/automated/check-divider-label-width.py` protects this calculation.

## Divider resource and drawing safety

`XmDivider` now skips redisplay until its window and foreground GC are
available. Label replacement releases the previous owned `XmString` only
after a successful copy, retaining the old label if allocation fails.
Successful divider rendering and resource behavior are unchanged.

`tests/automated/check-divider-resource-safety.py` protects these paths.

## DnD destination callback inputs

The Container, Text, and TextField destination callbacks now validate their
widget, callback structure, and transfer ID before touching Xt or X11 state.
This makes cancellation and widget-destruction callbacks safe even when the
toolkit receives an incomplete destination notification.

The regression check is
`tests/automated/check-dnd-destination-callback-inputs.py`.

## DnD destination callback data

Drop callbacks from an external owner may be cancelled or arrive after the
destination data has been cleared. Container, Text, and TextField now reject
missing `XmDropProcCallbackStruct` data and complete the transfer with
`XmTRANSFER_DONE_FAIL` before reading coordinates or the drop action.

The regression check is
`tests/automated/check-dnd-destination-data.py`.

## DnD drop failure cleanup

Text and TextField allocate a local drop-transfer record before inspecting an
external owner's targets. Unsupported operations and missing targets now free
that record, complete the transfer with failure, and return immediately. This
avoids both a leak and continued work after transfer completion.

The regression check is
`tests/automated/check-dnd-drop-failure-cleanup.py`.

## DnD failure returns

Destination callbacks reject transfers when their widget is insensitive. They
now return immediately after `XmTransferDone(...XmTRANSFER_DONE_FAIL)`,
because completion can release the transfer callback structure and invoke
reentrant cleanup. Continuing to inspect that structure after completion was
a use-after-free risk.

The regression check is
`tests/automated/check-dnd-failure-returns.py`.

## Text DnD conversion failure

When a Text or TextField drop cannot convert the owner's payload to the
widget's locale, the transfer is now completed with
`XmTRANSFER_DONE_FAIL` before returning. This prevents a conversion failure
from silently leaving the transfer in its default state.

The regression check is
`tests/automated/check-dnd-text-conversion-failure.py`.

## DnD transfer callback inputs

Text and TextField drop-transfer callbacks now reject missing widgets,
selection callback structures, closures, and transfer records before
dereferencing them. Their completion callbacks also tolerate null widgets or
callback data during teardown. Invalid requests complete with failure when a
transfer ID is available.

The regression check is
`tests/automated/check-dnd-transfer-callback-inputs.py`.

## Completion callback allocation fallback

Transfer completion normally snapshots its callback list before invoking any
callback, which permits reentrant callback registration and avoids holding the
process lock during user code. If that snapshot allocation fails, completion
now falls back to fetching each callback pointer under the lock and invoking it
unlocked. Drop cleanup and user completion notifications therefore still run
under allocation pressure.

Regression coverage: `tests/automated/check-done-proc-allocation-fallback.py`.

## Drag atom table bounds

Motif drag-and-drop state is shared through properties on the per-display Motif
window. Property counts are untrusted X server data, so table-size arithmetic
is checked before allocation and before serializing a property. This prevents a
malformed or corrupted count from wrapping an allocation size. The source
contract is covered by `tests/automated/check-drag-atom-bounds.py`.

Generated `_MOTIF_ATOM_` names are formatted with a bounded operation into
the fixed atom-name buffer, keeping the property-table path safe even when
the entry index is supplied by a large or corrupted table.

The per-display `XContext` IDs are initialized and tested as integer IDs,
avoiding pointer-to-integer diagnostics on 64-bit builds.

The drag-icon context uses the same integer-zero convention, so related
per-display state also compiles without pointer-to-integer diagnostics.
The atom-table reader also checks the serialized entry count against Xt's
`Cardinal` allocation limit before growing its cached entry array.  This keeps
malformed or cross-platform-sized properties from narrowing the multiplication
at the `XtRealloc` boundary.  Valid atom tables retain the existing wire and
cache behavior.

## Drag bootstrap property validation

The Motif drag bootstrap readers now reject truncated X properties and require
non-null returned buffers before reading window, atom-table, target-table, or
proxy-window data.  This keeps malformed or oversized root/window properties
on the existing recovery path instead of dereferencing incomplete data.

Regression: `tests/automated/check-drag-bootstrap-property-validation.py`

## Drag client-window property status

The drag client-window probe now checks the return status from
`XGetWindowProperty` before treating a window as a client-window owner.  X
errors continue through the existing child-window search instead of using
undefined property metadata.

Regression: `tests/automated/check-drag-client-property-status.py`

## Drag conversion failure cleanup

`XmeDragSource` records source and location data before asking the widget to
convert its export targets.  When conversion fails or returns an invalid
target list, the drag never starts; those context fields are now cleared on
both early exits.  This prevents a later Motif-drop conversion from observing
stale data from a failed drag attempt.

The regression check is
`tests/automated/check-drag-conversion-failure-cleanup.py`.

## Drag conversion callback inputs

The drag conversion wrapper is called by Xt while a drag context is being
created, cancelled, or torn down.  It now validates the context and all
conversion output pointers before obtaining the display, and refuses a
conversion whose originating widget has not been installed in the context
block.  This makes reentrant and cancellation paths fail safely instead of
dereferencing incomplete callback state.

The incremental and non-incremental drop conversion frontends also validate
their selection, target, and result arguments before consulting the X selection
request.  Teardown or malformed callback invocation therefore returns
conversion failure safely; normal Xt conversion requests are unchanged.

The focused regression is `tests/automated/check-drag-convert-inputs.py`.

## ICC drag property pointer validation

ICC drag initiator and receiver property readers now require a non-null
returned data pointer in addition to the existing type, format, length, and
byte-order checks.  Malformed property replies are rejected before their
serialized structures are dereferenced.

Regression: `tests/automated/check-drag-icc-property-pointer.py`

## DnD message subtype bounds

Motif drag-and-drop client messages carry a compact subtype in the event
payload. The decoder now rejects values outside the internal callback table,
and the shared subtype mapper returns the private invalid-reason sentinel
instead of indexing past the table. This keeps malformed external-owner
traffic from reaching callback decoding with an out-of-bounds read.

The focused regression is
`tests/automated/check-drag-message-type-bounds.py`.

## Drag protocol window creation

Motif keeps a small persistent, invisible X11 window for drag-and-drop target
properties.  Its creation path now checks `XCreateWindow` before mapping the
window or publishing its atom.  On failure it explicitly releases the server
grab, closes the auxiliary display, and returns `None`.

## Drag motion-buffer bounds

The drag motion buffer begins with 120 entries embedded in the stack frame.
When the boundary is reached, the heap replacement now reserves one additional
entry before recording the next motion event.  Later growth retains the existing
chunked capacity policy, and the valid motion sequence is unchanged.

## Bound DragBS property lengths

The atom and target-list property writers now check both protocol storage
(`CARD32`) and Xlib's signed property-length parameter before publishing a
buffer.  Explicit casts occur only after those checks, preventing large tables
from being silently truncated on 64-bit hosts.

## Validate DragBS property offsets

DragBS atom and target-list readers now verify the serialized `heap_offset`
falls after the fixed header/table records and within the received property.
Malformed external-owner properties are rejected before table reconstruction,
while the existing protocol layout remains unchanged.

## Bound DnD receiver property sizes

The Motif drag receiver protocol stores its heap offset in a 32-bit field and
passes property lengths to Xlib as `int`.  `_XmSetDragReceiverInfo` now rejects
buffers that cannot be represented in either field, releases temporary heap
storage, and publishes only checked lengths.  This prevents truncation of a
large drop-site tree and keeps the receiver's property layout coherent.

## Bound DnD buffer reads

`_XmReadDragBuffer` now performs its cursor and remaining-size arithmetic in
`size_t` and uses subtraction-based bounds checks.  A wrapped `numCurr + size`
calculation can no longer bypass the clamp and copy past the received property
buffer.
The drag-property read and write helpers reject a null property buffer, and a
null payload when the requested transfer is nonzero, before copying bytes.
Zero-length transfers remain harmless no-ops.  This protects malformed or
failed conversion paths while preserving valid transfer behavior.

## Keep DnD read counts representable

`_XmReadDragBuffer` returns a `CARD16` byte count for ABI compatibility.  It
now caps each copy at `UINT16_MAX`, so the returned count always describes the
bytes actually copied instead of wrapping after a larger copy.

## Drag-source setup allocation

`XmeDragSource` copies caller arguments and obtains export targets before
starting a drag. It now validates argument-count arithmetic, rejects a
missing argument array when entries were requested, checks the copied-array
allocation, and initializes the target pointer before conversion.

A failed export-target conversion therefore follows the existing cleanup path
without freeing an uninitialized pointer. Successful drag setup and argument
ordering remain unchanged.

## Drag-source target-count conversion

The export-target converter reports its result length as an `unsigned long`,
while `XmNnumExportTargets` is a `Cardinal` resource.  `XmeDragSource` now
rejects counts that cannot be represented and performs an explicit `Cardinal`
conversion for accepted counts.  This avoids silent truncation on 64-bit
systems while preserving the existing drag-source API.

The focused regression is
`tests/automated/check-drag-source-target-count.py`.

## Drag-start failure cleanup

`XmeDragSource` stores the source widget and location payload in the Motif
drop conversion context before calling `XmDragStart`.  If the drag cannot be
created, the context now clears its payload immediately.  This prevents a
later conversion callback from observing stale source data for a drag that
never existed.

The regression check is
`tests/automated/check-drag-start-failure-cleanup.py`.

## Drag export-target conversion results

`XmeDragSource` obtains the exported target list through the normal Motif
conversion callback.  Before passing it to `XmDragStart`, the result is now
required to be an `XA_ATOM` list in 32-bit format, with a non-null payload
when its count is non-zero.  Invalid conversion results are released and
reject the drag instead of being reinterpreted as a target array.

The regression check is
`tests/automated/check-drag-target-conversion-result.py`.
