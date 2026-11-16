---
nav_exclude: true
---
# Archived Bugfix and Hardening Notes

Consolidated archive of standalone bugfix and hardening validation notes.

## Accessibility D-Bus state allocation

The AT-SPI D-Bus transport keeps per-node memo records for cache entries,
text snapshots, and state-change notifications.  All paths now treat a failed
memo-record allocation as a recoverable transport failure instead of
dereferencing a null record.  Cache publication is skipped and the current
notification is abandoned while the native accessibility tree remains intact.

This keeps allocation pressure from crashing the UI process or the D-Bus
bridge, while allowing later notifications to retry record creation.

Initial tree seeding follows the same rule: a node whose memo record cannot be
created is skipped and the remaining tree continues to be announced.

Method-return message construction is also checked before initializing an
iterator, so D-Bus memory pressure produces a handled request rather than a
null-message crash.

AT-SPI state, window, cache, hierarchy, and property-change signals now use
the same fail-closed rule: a missing signal is skipped without touching an
uninitialized iterator or changing the native widget state.

Session-bus address lookup and registry embedding also handle failed method
message construction, releasing the partially initialized transport cleanly.

## Accessibility D-Bus memo allocation handling

The accessibility D-Bus transport memo table now reports allocation failure
from table resizing and record creation.  Callers leave the memo table
unchanged and return no record when growth cannot be completed, avoiding null
table or record dereferences while preserving existing live-node memo behavior.

The regression check is `tests/automated/check-accessibility-dbus-memo-allocation.py`.

## Accessibility event guards

Accessibility event hooks validate their widget, provider state, and event
inputs before reading event fields. Native key dispatch also treats the
optional continuation pointer safely during teardown or unusual dispatch.

## Accessibility list count bounds

The accessibility bridge rebuilds a list's semantic child array when native
rows are inserted, removed, or reordered. The synchronizer now rejects
unrepresentable `count + custom` byte sizes before allocating or copying the
temporary array, and rejects an overflowing `old_count * 2` hash-table growth
calculation. This keeps malformed or hostile native counts fail-closed rather
than wrapping a `size_t` and corrupting the accessibility child state.

`tests/automated/check-accessible-list-count-bounds.py` is the focused source
regression for these guards.

## Accessibility child-removal bounds

`XmAccessibilityRemove` now validates the parent child array, the recorded
child index and identity, and the count/capacity relationship before
decrementing the count or shifting survivors. Removing the final child
performs no shift.
Malformed or stale semantic-tree metadata therefore leaves the tree intact
instead of underflowing the move length and passing an oversized range to
`memmove`; a stale in-range index cannot shift a different sibling.

The focused source-contract check is
`tests/automated/check-accessible-remove-child-bounds.py`.

## Accessibility UTF-8 byte-range guard

The AT-SPI Unicode boundary scan now rejects UTF-8 strings whose byte length
cannot be represented by ICU's `int32_t` iterator cursor. This prevents large
multibyte text from being truncated before paragraph or word-boundary
calculation.

Regression check:

```text
python3 tests/automated/check-accessibility-utf8-byte-range.py
```

## AT-SPI object-path parsing

The optional D-Bus accessibility bridge accepts object paths below
`/org/a11y/atspi/accessible/` by parsing their numeric identifier. The parser
now requires a decimal digit at the start, checks `strtoul` for `ERANGE`, and
only then narrows the value to `XmAccessibleId`. This keeps valid paths
unchanged and prevents signed, empty, or overflowing identifiers from being
accepted differently on 32-bit and 64-bit hosts.

## AccessibleDBus break-array growth

Unicode text boundary calculation accumulates break positions in a dynamic
integer array.  The old macro assigned `XtRealloc`'s result directly and
continued to append even when allocation failed.

The macro now commits a replacement array only after success and jumps to its
existing cleanup path on failure, preserving valid ownership and preventing a
null-pointer write.

## ArrowButton default-size bounds

When an `XmArrowButton` receives a zero requested width or height, Motif adds
its default 15-pixel extent.  That addition now saturates at the largest
representable X11 `Dimension` instead of wrapping a large existing value to a
small or zero size.  Ordinary zero-sized initialization remains unchanged.

The focused source regression is
`tests/automated/check-arrow-button-default-size-bounds.py`.

## AutoSuggestBox batch allocation

`XmAutoSuggestBoxSetSuggestions` now checks the item-array byte count against
host `size_t` and Xt's `Cardinal` width, then checks the allocation and every
ID/label copy before publishing the batch. Failed installation releases
partial items and leaves the list in an explicit no-results state instead of
dereferencing a null array.

The focused source check is
`tests/automated/check-auto-suggest-string-copy-guards.py`.

## AutoSuggestBox callback guards

AutoSuggestBox validates callback closures before reading private search/list
state. Foreign or stale closures are ignored safely across query, commit,
selection, and event callbacks while normal suggestions remain unchanged.

## AutoSuggestBox child allocation

AutoSuggestBox creates a search field and a popup list for suggestions.
Popup creation, geometry, callbacks, keyboard navigation, dismissal, and
commit operations now tolerate either child being unavailable and clean up a
partially created popup safely.

The contract is protected by
`tests/automated/check-auto-suggest-child-failure-guards.py`.

## AutoSuggestBox suggestion-array allocation failure

`XmAutoSuggestBoxSetSuggestions` validates the suggestion-count multiplication
before opening or replacing the popup model. An oversized request is rejected
without erasing the current suggestions. Allocation failures after replacement
begins retain the existing behavior and leave the widget in a valid empty state
that can be repopulated by a later generation.

The focused source-contract check is
`tests/automated/check-autosuggest-input-preservation.py`.

## Avatar teardown input

Avatar destruction now validates the widget class before freeing its graphics
context, fallback font, and render table. Invalid teardown dispatch returns
safely; normal Avatar destruction is unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-destroy-input.py`.

## Avatar initialization input

Avatar initialization now validates the widget class before copying initials,
loading fonts, or creating its graphics context. Malformed lifecycle calls
return safely; normal Avatar construction remains unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-initialize-input.py`.

## Avatar geometry-query input

`QueryGeometry` now validates the widget class and geometry reply pointer
before writing the requested size. Malformed geometry negotiation returns
`XtGeometryNo`; valid Avatar sizing remains unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-query-geometry-input.py`.

## Avatar rendering input

Avatar `Redisplay` now validates the widget class before retrieving display and
window state or drawing. Invalid expose dispatch returns safely; normal Avatar
rendering remains unchanged.

Expose processing also requires the widget's graphics context. If X resource
allocation leaves the context unavailable, the callback returns before issuing
`XDrawArc` or `XCopyArea`, avoiding an X error while the widget remains safe to
recreate or destroy.

The focused source contract is checked by
`tests/automated/check-avatar-redisplay-input.py`.

## Avatar resource transactions

`XmAvatar` now preserves its current initials and render table when a
`SetValues` replacement cannot be copied. Successful replacements remain
owned by the widget; failed replacements do not leave redisplay or geometry
using a null resource.

The focused source check is
`tests/automated/check-avatar-initials-geometry-bounds.py`.

## Avatar SetValues input

Avatar `SetValues` now validates its old and new widget records before
comparing font and initials state. Malformed lifecycle calls return `False`
without touching private state; valid resource updates are unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-set-values-input.py`.

## Badge teardown input

Badge destruction now validates the widget class before freeing its drawing
context, font, and label state. Invalid teardown dispatch returns safely;
normal widget destruction is unchanged.

The focused source contract is checked by
`tests/automated/check-badge-destroy-input.py`.

## Badge initialization input

Badge initialization now validates the widget class before copying labels,
fonts, or creating its drawing context. Malformed lifecycle calls return
safely; normal Badge construction is unchanged.

The focused source contract is checked by
`tests/automated/check-badge-initialize-input.py`.

## Badge count API inputs

`XmBadgeGetDisplayedCount` now rejects a null or non-Badge widget before
casting it to the private Badge instance. It keeps the existing `-1` neutral
result for invalid handles; valid count and maximum handling is unchanged.

The focused regression check is `tests/automated/check-badge-inputs.py`.

## Badge geometry-query input

`QueryGeometry` now validates the widget class and geometry reply pointer
before reading badge state or writing sizing results. Malformed negotiation
returns `XtGeometryNo`; valid Badge sizing remains unchanged.

The focused source contract is checked by
`tests/automated/check-badge-query-geometry-input.py`.

## Badge rendering input

Badge `Redisplay` now validates the widget class before reading label/count
state or issuing X drawing calls. Invalid expose dispatch returns safely;
normal Badge rendering is unchanged.

The focused source contract is checked by
`tests/automated/check-badge-redisplay-input.py`.

## Badge resource guards

`XmBadge` now preserves its render table when a `SetValues` copy fails and
skips the decorative frame when the widget is too small for a valid rectangle.
Text measurement and drawing continue to use the active render table.

The focused source check is `tests/automated/check-badge-text-metrics.py`.

## Badge SetValues input

Badge `SetValues` now validates the old and new widget records before comparing
font, label, and severity state. Malformed lifecycle calls return `False`
without touching private state; valid resource updates are unchanged.

The focused source contract is checked by
`tests/automated/check-badge-set-values-input.py`.

## BaseClass extension allocation handling

BaseClass lazily allocates class-extension records, secondary-resource data,
resource indirection tables, and widget-extension association nodes.  These
allocations are now checked before initialization or dereference.  Partial
secondary-resource records are released, and resource transformation reports
an empty result when an allocation fails.  Existing successful extension
setup and context ownership are unchanged.

The regression check is `tests/automated/check-baseclass-extension-allocation.py`.

## Base-class wrapper allocation

Motif installs wrapper records lazily while Xt initializes widget classes.
Those records are allocated independently of the class extension itself.  The
class initializer and leaf-wrapper hooks now stop safely when a wrapper record
cannot be allocated, rather than dereferencing a null record while copying
realize, resize, geometry, or class-part hooks.  Normal wrapper installation
is unchanged when the private records are available.

## BreadcrumbBar segment allocation handling

`XmBreadcrumbBarSetSegments` now requires a segment array whenever `count` is
non-zero, checks the segment-array size and every item label allocation. Partial
label arrays are released and the current index is reset when replacement
cannot be completed, preventing later drawing or input from observing invalid
item storage. Successful segment replacement keeps the existing selection
behavior; a zero count still clears the bar without an array.

All public segment-count, current-index, and setter entry points also reject a
null widget before invoking the Xt subclass check, making invalid optional
handles return a neutral result instead of dereferencing them.

The regression check is `tests/automated/check-breadcrumb-allocation.py`.

## BreadcrumbBar event guards

BreadcrumbBar validates its widget type and event pointer before processing
pointer and keyboard navigation. Invalid dispatch is ignored safely while
normal breadcrumb selection and activation remain unchanged.

## BreadcrumbBar coordinate bounds

BreadcrumbBar keeps path-width and hit-test calculations in `int` values, but
now clamps coordinates when handing them to `XmStringDraw`, whose Xt position
arguments are signed 16-bit values.  Very long paths remain safely clipped
instead of wrapping labels to unrelated coordinates.

`tests/automated/check-breadcrumb-coordinate-bounds.py` protects this drawing
boundary.

## BulletinBoard dialog-title getter guard

The synthetic `XmNdialogTitle` getter now validates the BulletinBoard widget
and output slot before reading the dialog-title field. Invalid callback inputs
return a deterministic null value; valid reads retain the existing copied
`XmString` ownership contract.

The focused source-contract check is
`python3 tests/automated/check-bulletin-dialog-title-guard.py`.

## ButtonBox constraint hook input

ButtonBox child constraint initialization and SetValues now reject a missing
`set` widget before accessing per-child geometry. Valid child layout and
preferred-size invalidation remain unchanged.

The focused source contract is checked by
`tests/automated/check-buttonbox-constraint-input.py`.

## ButtonBox initialization input

The ButtonBox initialization method now validates its request/set widgets
before reading geometry or writing default dimensions. Malformed lifecycle
calls return safely; valid ButtonBox construction retains its existing
one-pixel minimum defaults.

The focused source contract is checked by
`tests/automated/check-buttonbox-initialize-input.py`.

## Gadget cache allocation handling

`_XmCachePart` now checks cache-record size arithmetic and both cache-record
allocations before linking or copying data.  Failed allocations leave the
class cache unchanged and return `NULL`, preventing null cache-record
initialization while preserving normal cache sharing on success.

The regression check is `tests/automated/check-cache-part-allocation.py`.

## Calendar coordinate bounds

Calendar cell and heading positions are calculated in ordinary integers, then
clamped before being passed to `XmStringDraw`'s signed Xt coordinate
parameters.  Wide controls therefore clip safely instead of wrapping text
coordinates.

`tests/automated/check-calendar-coordinate-bounds.py` protects this drawing
boundary.

## Calendar day arithmetic

Calendar keyboard navigation now performs ordinal arithmetic in a wide type
and clamps to the supported 1–9999 date range. Moving beyond either boundary
no longer overflows an intermediate integer or produces an invalid date.

## Calendar event guards

Calendar validates its widget type and event pointer before processing
keyboard and pointer date navigation. Invalid dispatch is ignored safely
while normal date selection and activation remain unchanged.

## Calendar render-table updates

Calendar now replaces its render table transactionally. If copying the
requested table fails, the existing table remains owned and in use; the old
table is released only after a successful replacement.

The focused source contract is checked by
`tests/automated/check-calendar-render-table-transaction.py`.

## CareVisual child-count bounds

`_XmNotifyChildrenVisual` traverses a composite widget's child array. The
loop now uses the Xt `Cardinal` child-count type, preserving the existing
care-parent-visual notification behavior while avoiding signed-index
wraparound for large child lists.

## Cascade-arrow resources

Cascade buttons cache armed and unarmed arrow pixmaps.  The cache records,
temporary graphics contexts, and server pixmaps are now checked before use.
Pixmap creation failures leave the cache entry unavailable rather than issuing
draw requests against `None`; temporary GC ownership is also released on all
failure and success paths.

## CascadeButton excluded-pane initialization

Cascade buttons record the submenu being posted in shared excluded-parent-pane
storage used by tear-off restoration. The storage is normally initialized on
the first cascade, but a failed allocation or a malformed zero-capacity state
must not be dereferenced.

Initialization now uses a temporary replacement pointer and commits the pointer
and four-entry capacity only after Xt allocation succeeds. The cascade returns
without changing menu state when storage cannot be obtained.

## Cascade-gadget highlight null-widget guard

`XmCascadeButtonGadgetHighlight` now returns immediately for a null widget,
before application-context lookup. This prevents an invalid highlight request
from crashing the caller while preserving the existing behavior for valid
cascade-button gadgets.

The focused regression is
`tests/automated/check-cascade-highlight-input.py`.

## CascadeButtonGadget hook allocation handling

CascadeButtonGadget's secondary-object, set-values, and get-values hooks now
check extension-object and extension-data allocations.  Partial objects are
released before returning.  Initialize, set-values, and get-values posthooks
also tolerate missing extension records when a prehook could not establish
temporary state, preventing cleanup-time null dereferences.

The regression check is `tests/automated/check-cascadebg-hook-allocation.py`.

## Charset-table population index width

The `XmRCharSetTable` converter counts entries as an `unsigned int`, matching
the bounds used before its combined allocation is made.  Population now uses
that same width instead of casting the count to `int`; a large representable
table therefore cannot make the loop condition negative or skip entries.

The change is internal and preserves the resource format and all ordinary
conversion results.  `tests/automated/check-charset-table-index-width.py`
protects the source-level contract.

## Charset-table allocation bounds

The string-to-charset-table resource converter stores both the pointer table
and copied charset names in one Xt allocation.  It now checks that the table
itself fits in the allocator's `Cardinal` width before subtracting its size
from `UINT_MAX`; this prevents unsigned wraparound from admitting an
unrepresentable allocation on wide or malformed input.

`tests/automated/check-charset-table-size-bounds.py` is the focused source
regression for the guard.

## Chip event guards

Chip validates its widget type and event pointer before processing pointer or
keyboard activation and removal. Invalid dispatch is ignored safely while
normal chip interaction remains unchanged.

## Chip and IconLabel rendering graphics contexts

`XmChip` and `XmIconLabel` now require a valid foreground graphics context
before their expose callbacks clear or draw the window. Graphics-context
allocation can fail under X resource pressure; returning without issuing
drawing requests avoids a server-side `BadGC` while leaving the widget safe to
recreate or destroy.

The focused source contract is checked by
`tests/automated/check-chip-iconlabel-render-gc.py`.

## Chip action input guards

`XmChipInvoke` and `XmChipRemove` now reject null widget handles before calling
the subclass predicate.  Valid sensitive Chip actions are unchanged, while
invalid public calls no longer risk dereferencing a null Xt object.

`tests/automated/check-chip-input-guards.py` protects the guard order.

## Class dialog allocation handling

`XmeCreateClassDialog` constructs a dialog-shell name and a temporary
argument array before creating the shell and child.  It now validates the
caller argument pointer when arguments are present, checks both allocations,
and stops cleanly if shell creation fails.  Successful dialog construction and
ownership of the temporary buffers are unchanged.

The regression check is `tests/automated/check-class-dialog-allocation.py`.

## ClipWindow translation-buffer bounds

ClipWindow assembles generated virtual-key translations into a reusable
buffer.  The append helper now rejects a wrapped used-length calculation and
rejects results wider than Xt's `Cardinal` allocation size before growing the
buffer.  Existing failure behavior remains fail-closed: an allocation or
representability failure leaves the prior buffer valid.

`tests/automated/check-clip-window-translation-allocation.py` checks the
boundary guards alongside the existing allocation-failure checks.

## Clipboard atom-name ownership

`XmClipboardInquireFormat` receives format names from `XGetAtomName`. Those
strings are Xlib-owned and must be released with `XFree`; using `XtFree` can
trigger allocator mismatches in applications that use separate Xt and Xlib
allocation hooks. The query now preserves the Xlib ownership contract.

## Clipboard named-conversion result width

`ClipboardGetByNameItem` now stores the result of
`ClipboardRequestDataAndWait` in an `int`, matching the callee’s return type
instead of narrowing it to `short` before testing success.

Regression: `tests/automated/check-clipboard-by-name-result-width.py`

## Clipboard callback cleanup

When a clipboard conversion returns an invalid format, an overflowing size,
or a null value for non-empty data, `ClipboardCallback` now records a failed
copy with an empty clipboard payload and continues through the normal request
completion path.  The callback no longer returns before sending the final
`XmIDONE` conversion and freeing the snapshot request.

The regression check is
`tests/automated/check-clipboard-callback-cleanup.py`.

## Clipboard-manager callback inputs

`ClipboardCallback` is invoked by the clipboard manager for snapshot and
delete notifications.  It now validates the widget, data ID, reason, and
target-dependent arguments before deriving the display or conversion context.
The dictionary-empty case is also handled after acquiring the process lock.
This prevents malformed or teardown-time callbacks from dereferencing a null
widget or entering the request table without valid input.

The regression check is
`tests/automated/check-clipboard-callback-inputs.py`.

## Deferred clipboard callback payloads

Deferred clipboard callbacks convert element counts to byte lengths before
writing the snapshot item. The callback now validates the 8/16/32-bit format,
checks multiplication overflow, and rejects a missing value for non-empty
payloads before copying.

The focused regression is
`tests/automated/check-clipboard-callback-payload.py`.

## Clipboard callback-table allocation width

The callback-table growth path in `XmClipboardStartCopy` now computes
replacement byte sizes in `size_t`, converts the already-bounded sizes
explicitly to Xt’s `Cardinal`, and uses wide byte counts for table copies.

Regression: `tests/automated/check-clipboard-callback-table-width.py`

## Clipboard cancellation lock status

`XmClipboardCancelCopy` now stops on every non-success result from
`ClipboardLock`, preserving the original status before deleting clipboard
records.

Regression: `tests/automated/check-clipboard-cancel-lock-status.py`

## Keep clipboard chunks consistent

`GetWindowProperty` now records the first chunk's property type and format and
rejects later chunks that change either value.  This protects INCR reads from
an external owner replacing the selection mid-transfer and prevents mixed
format data from being exposed to callers.

## Clipboard client-message Atom payloads

`ClipboardSendMessage` now rejects Atom IDs that cannot fit the signed
`long` payload slots of `XClientMessageEvent` and rejects unknown message
types instead of sending an uninitialized payload.

Regression: `tests/automated/check-clipboard-client-message-atoms.py`

## Clipboard conversion format cleanup

The selection conversion helper temporarily owns the format record returned by
`ClipboardFindFormat`.  It now releases that record after any cut-by-name
callback request completes, preserving the callback result while avoiding a
leak on every conversion.

Regression: `tests/automated/check-clipboard-convert-format-cleanup.py`

## Clipboard conversion target-list cleanup

The `TARGETS` conversion path obtains its first format record before sizing and
allocating the returned atom list.  Allocation or count validation failures
now release that record before leaving the conversion callback.
The iterator's final record is also released if inconsistent count metadata
leaves it non-NULL when the loop exits.

Regression: `tests/automated/check-clipboard-convert-targets-cleanup.py`

## Clipboard copy allocation width

`XmClipboardCopy` now rejects format-record and format-data lengths that do
not fit Xt’s `Cardinal` allocator size, and converts bounded lengths
explicitly before allocation.

Regression: `tests/automated/check-clipboard-copy-allocation-width.py`

## Clipboard copy lock status

`XmClipboardCopy` now stops on every non-success result from `ClipboardLock`,
including lock-property or allocation failures.  It returns the original
status before opening or modifying clipboard records.

Regression: `tests/automated/check-clipboard-copy-api-lock-status.py`

## Clipboard copy-by-name length bounds

`XmClipboardCopyByName` now applies the same signed-range checks as the
ordinary copy path when updating a format record. Negative serialized lengths
and additions that exceed `LONG_MAX` fail cleanly and release the active copy
buffers before returning.

Regression: `tests/automated/check-clipboard-copy-by-name-length.py`

## Clipboard copy lock status

`XmClipboardCopyByName` now treats every non-success result from
`ClipboardLock` as a failed operation.  It releases the retrieved header and
returns the original status, instead of continuing to modify clipboard data
after a lock-allocation or lock-property failure.

Regression: `tests/automated/check-clipboard-copy-lock-status.py`

## Clipboard target-count narrowing

`XmClipboardInquireCount` now divides the returned target-byte length into an
atom count before converting to the public `int` result. The existing
`INT_MAX` check proves that conversion is safe and keeps strict builds from
reporting a misleading signed-conversion warning.

Regression: `tests/automated/check-clipboard-count-narrowing.py`

## Clipboard format deletion cleanup

`ClipboardDeleteFormats` now releases the owning data-item buffer when a
format header cannot be retrieved or is too short.  Successful deletion keeps
the existing per-format cleanup and record removal behavior; malformed
format records no longer leak the outer clipboard item allocation.

Regression: `tests/automated/check-clipboard-delete-formats-cleanup.py`

## Clipboard deletion index width

`ClipboardDeleteItem` now uses an unsigned-long traversal counter matching
`header->currItems`, while keeping a separate signed index for the older/newer
candidate search that intentionally uses `-1`. Large item lists no longer
depend on signed-counter wraparound.

Regression: `tests/automated/check-clipboard-delete-index-width.py`

## Clipboard end-copy lock status

`XmClipboardEndCopy` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening clipboard
state and finalizing the copy transaction.

Regression: `tests/automated/check-clipboard-end-copy-lock-status.py`

## Clipboard end-copy offset bounds

`XmClipboardEndCopy` now rejects invalid item IDs and checks the item-list
byte count, offset addition, and final conversion before forming the new item
pointer. This keeps malformed or overflowing clipboard headers from turning
signed arithmetic into an out-of-range write.

Regression: `tests/automated/check-clipboard-end-copy-offset.py`

## Clipboard event-message Atom comparisons

`ClipboardEventHandler` now rejects request/delete Atom IDs that cannot fit
the signed client-message payload representation and compares representable
IDs explicitly as `long` values.

Regression: `tests/automated/check-clipboard-event-message-atoms.py`

## Clipboard format-atom allocation size

`ClipboardGetAtomFromFormat` now rejects combined prefix/name lengths that do
not fit Xt’s `Cardinal` allocator size before converting and allocating them.
The existing `size_t` overflow check remains in place.

Regression: `tests/automated/check-clipboard-format-atom-size.py`

## Clipboard format-count bounds

`ClipboardFindFormat` now rejects impossible cancelled-format counts and
saturates the internal wide count at `INT_MAX` before returning through the
legacy `int *count` API. Malformed or very large records no longer wrap the
count into a negative value.

Regression: `tests/automated/check-clipboard-format-count-bounds.py`

## Clipboard format-index comparison

`XmClipboardInquireFormat` rejects non-positive format indexes before looking
up a target. The subsequent atom-count comparison now converts that validated
index to `unsigned long`, avoiding signedness ambiguity without changing the
public API or accepted indexes.

Regression: `tests/automated/check-clipboard-format-index.py`

## Clipboard format-list offset validation

Clipboard data-item readers now require `formatIdList` to point after the
fixed `ClipboardDataItemRec`, as well as within the returned record length.
Malformed items cannot make format-list traversal read header bytes or memory
before the record.

Regression: `tests/automated/check-clipboard-format-list-offset.py`

## Clipboard format-atom literal formatting

`ClipboardGetAtomFromFormat` now passes its fixed internal format string
directly to `snprintf`, preserving the existing checked allocation while
allowing the compiler to validate the format and argument types.

Regression: `tests/automated/check-clipboard-format-literal.py`

## Clipboard header invariants

Clipboard header validation now requires a non-zero item capacity, ensures
the current item count does not exceed that capacity, and requires the item
list offset to begin after the fixed header record.  Corrupt headers are
therefore rejected before later clipboard operations form list pointers or
grow the item count.

Regression: `tests/automated/check-clipboard-header-invariants.py`

## Keep clipboard INCR chunks positive

The clipboard transfer chunk calculation now clamps to one byte when an X11
server reports an unusually small maximum request size.  This avoids negative
or zero lengths reaching `XGetWindowProperty` and prevents an append loop from
making no progress.

## Clipboard inquiry count bounds

`XmClipboardInquireCount` now validates external `TARGETS` data before
converting its byte length to the legacy `int` format count.  Non-atom-aligned
or unrepresentably large responses are rejected after releasing the owner
buffer and clipboard lock.

Regression: `tests/automated/check-clipboard-inquire-count-bounds.py`

## Clipboard inquiry name-length width

`XmClipboardInquireCount` now keeps the result of `strlen` in `size_t` while
computing the maximum format-name length. This avoids narrowing a source
length to `int` before comparing it with the unsigned API result.

Regression: `tests/automated/check-clipboard-inquire-count-length-width.py`

## Clipboard count inquiry lock status

`XmClipboardInquireCount` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening clipboard
state. Failed inquiries no longer continue as if the clipboard were ready.

Regression: `tests/automated/check-clipboard-inquire-count-lock-status.py`

## Clipboard format inquiry buffer cleanup

`XmClipboardInquireFormat` now releases the external `TARGETS` buffer whether
the requested 1-based format exists or the owner returned too few targets.
The latter remains a normal no-data result without leaking the conversion
buffer.

Regression: `tests/automated/check-clipboard-inquire-format-buffer.py`

## Clipboard format inquiry indexes

`XmClipboardInquireFormat` uses a 1-based format index.  It now rejects zero
and negative indexes before consulting an external selection owner, preventing
pointer arithmetic before the returned `TARGETS` array while preserving the
existing public indexing convention.

Regression: `tests/automated/check-clipboard-inquire-format-index.py`

## Clipboard format inquiry lock status

`XmClipboardInquireFormat` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening clipboard
state. A failed format inquiry no longer proceeds with an invalid lock.

Regression: `tests/automated/check-clipboard-inquire-format-lock-status.py`

## Clipboard length inquiry lock status

`XmClipboardInquireLength` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening clipboard
state. A failed length inquiry no longer proceeds with an invalid lock.

Regression: `tests/automated/check-clipboard-inquire-length-lock-status.py`

## Clipboard pending-item inquiry lock status

`XmClipboardInquirePendingItems` now stops on every non-success result from
`ClipboardLock`, preserving the original status before reading or allocating
pending-item state.

Regression: `tests/automated/check-clipboard-inquire-pending-lock-status.py`

## Clipboard item-length bounds

`XmClipboardCopy` now checks the converted format-unit increment against the
existing signed `itemLength` before updating the serialized format record.
Overflow leaves the clipboard transaction cleaned up and returns failure.

Regression: `tests/automated/check-clipboard-item-length-bounds.py`

## Clipboard label ID bounds

`ClipboardDeleteItemLabel` now rejects serialized label IDs above `LONG_MAX`
before converting the unsigned record field to Motif’s signed `itemId` type.
Malformed clipboard records can no longer wrap into a different deletion ID.

Regression: `tests/automated/check-clipboard-label-id-bounds.py`

## Clipboard compound-string label length width

`XmClipboardStartCopy` now keeps the compound-string byte count in an
unsigned type matching Motif conversion output and explicitly passes the
already-validated label ID through the signed `itemId` API.

Regression: `tests/automated/check-clipboard-label-length-width.py`

## Clipboard length bounds

`XmClipboardInquireLength` now rejects a negative serialized format length and
checks the 32-bit format-unit multiplication before producing the public
unsigned length. Corrupt or overflowing records therefore fail cleanly rather
than becoming a large wrapped length.

Regression: `tests/automated/check-clipboard-length-bounds.py`

## Clipboard mark-item cleanup

`ClipboardMarkItem` now frees the Xt-owned data-item buffer after writing the
updated delete-pending state back to the clipboard property.  Error cleanup
was already present; successful mark operations no longer leak their
retrieved record.

Regression: `tests/automated/check-clipboard-mark-item-cleanup.py`

## Clipboard next-item ID cleanup

`ClipboardGetNewItemId` receives a temporary property buffer from
`ClipboardFindItem`, increments and persists the next ID, then now releases
that buffer before returning.  Invalid-property handling is unchanged.

Regression: `tests/automated/check-clipboard-next-id-cleanup.py`

## Clipboard ownership status

When a clipboard manager is present, `XmeClipboardSource` reports the result
of `XtOwnSelection` to its caller.  Ownership failure now returns `False`
instead of incorrectly reporting success, so applications can retry or show
an error without assuming the source is active.

The focused regression is
`tests/automated/check-clipboard-owner-status.py`.

## Clipboard paste lock cleanup

`XmClipboardCopyByName` retrieves a temporary clipboard header before acquiring
the clipboard lock.  When another client owns that lock, the function now
releases the retrieved header before returning `ClipboardLocked`; successful
and failure paths retain their existing protocol behavior.

Regression: `tests/automated/check-clipboard-paste-lock-cleanup.py`

## Clipboard payload length conversion

Clipboard converters report element counts while `XmClipboardCopy` consumes a
byte length. The 8/16/32-bit conversion now checks the format unit before
multiplication, discarding an unrepresentable payload rather than allowing the
length to wrap.

The focused regression is
`tests/automated/check-clipboard-payload-multiplication.py`.

## Clipboard pending-item allocation

`XmClipboardInquirePendingItems` checks the pending-record count against
`UINT_MAX / sizeof(XmClipboardPendingRec)` before allocating. The validated
byte count is now passed to Xt with an explicit `Cardinal` conversion, making
the allocation contract clear on both 32-bit and 64-bit builds.

Regression: `tests/automated/check-clipboard-pending-allocation.py`

## Clipboard pending-item count bounds

Clipboard headers store item counts wider than the legacy `int` loop and Xt
allocation types used by pending-item enumeration.  Header validation now
rejects counts above `INT_MAX`, and the pending-list allocation rejects sizes
above `UINT_MAX / sizeof(XmClipboardPendingRec)` before casting to `Cardinal`.

Regression: `tests/automated/check-clipboard-pending-count-bounds.py`

## Clipboard pending-item count width

`XmClipboardInquirePendingItems` now uses unsigned-long loop and result
counters to match `header->currItems` and the public count output. Large,
valid pending-item collections no longer risk signed counter overflow during
the traversal.

Regression: `tests/automated/check-clipboard-pending-count-width.py`

## Clipboard private-ID width

The CutPaste API represents its private identifier as `long`, while target
lists carry X11 `Atom` values. The deferred clipboard path now performs that
widening explicitly when registering snapshot requests, preserving the
32-bit-compatible protocol value without an implicit conversion warning.

The focused regression is
`tests/automated/check-clipboard-private-id-width.py`.

## Clipboard property-name comparison width

`GetWindowProperty` now keeps the property-name length and comparison index in
`size_t`, matching `strlen` and avoiding a narrowing conversion before the
legacy clipboard type comparison.

Regression: `tests/automated/check-clipboard-property-name-width.py`

## Clipboard property offset bounds

`GetWindowProperty` now checks its unsigned property offset against
`LONG_MAX` before passing it to Xlib’s signed `long` offset parameter. This
prevents malformed or oversized property walks from wrapping during the X11
conversion.

Regression: `tests/automated/check-clipboard-property-offset-bounds.py`

## Clipboard assembly allocation cleanup

`XmClipboardRetrieve` assembles an incremental X selection into an Xt-owned
buffer.  Growth uses a temporary pointer for `XtRealloc`: if allocation fails,
the existing buffer remains reachable by the common failure path and is freed.
This avoids leaking a partially assembled selection while preserving the
existing success and failure return contracts.

Regression: `tests/automated/check-clipboard-realloc-cleanup.py`

## Clipboard record-type length guard

`ClipboardFindItem` now verifies that a returned property contains at least
the record-type field before checking its value.  Short or empty properties
are rejected and cleaned up without dereferencing beyond the returned X
buffer.

Regression: `tests/automated/check-clipboard-record-type-length.py`

## Clipboard replacement request size

`ClipboardReplaceItem` now computes its incremental request size in unsigned
wide arithmetic and caps it at `INT_MAX`, matching `XChangeProperty`’s element
count parameter before each chunk is submitted.

Regression: `tests/automated/check-clipboard-replace-request-size.py`

## Clipboard request format width

`ClipboardReplaceItem` now makes the validated format divisor explicitly
unsigned when computing incremental request sizes, avoiding an unintended
signed conversion in the X11 arithmetic.

Regression: `tests/automated/check-clipboard-request-format-width.py`

## Clipboard retrieve format cleanup

When `XmClipboardRetrieve` finds a format in the local clipboard, it updates
the format record in the property after copying data.  The temporary record is
now released immediately after that replacement, preventing a leak on every
successful local retrieve.

Regression: `tests/automated/check-clipboard-retrieve-format-cleanup.py`

## Clipboard retrieval length width

`ClipboardRetrieveItem` now accepts unsigned-long additional lengths, so
large format-data requests are not truncated through an `int` cast. Before
calling Xt’s `Cardinal` allocator, the combined length is checked explicitly
and rejected if it cannot be represented.

Regression: `tests/automated/check-clipboard-retrieve-length-width.py`

## Clipboard retrieval lock status

The internal transaction behind `XmClipboardRetrieve` now stops on every
non-success result from `ClipboardLock`, preserving the original status before
opening clipboard state or processing a requested format.

Regression: `tests/automated/check-clipboard-retrieve-lock-status.py`

## Clipboard retrieve re-check cleanup

When a cut-by-name callback changes or removes a format, the retrieve path
re-reads that format before copying data.  If the re-check fails, it now closes
the open clipboard header and releases the clipboard lock before returning the
corruption error.  This prevents a failed external-owner transfer from
leaving the clipboard locked.

Regression: `tests/automated/check-clipboard-retrieve-recheck-cleanup.py`

## Clipboard selection format bounds

`ClipboardGetSelection` now validates the stored X selection format against
the only supported values (8, 16, and 32) before narrowing it to the legacy
`int *format` output.

Regression: `tests/automated/check-clipboard-selection-format-bounds.py`

## Clipboard request header cleanup

The data-request branch of `ClipboardSendMessage` now frees the retrieved
clipboard header after persisting its recopy ID.  Invalid-header cleanup was
already present; valid request messages no longer leak the temporary header
allocation.

Regression: `tests/automated/check-clipboard-send-header-cleanup.py`

## Clipboard next-item ID setter cleanup

`ClipboardSetNextItemId` receives a temporary `XM_NEXT_ID` property buffer,
persists the newly selected ID, and now releases that buffer before returning.
The ID selection and invalid-property behavior are unchanged.

Regression: `tests/automated/check-clipboard-set-next-id-cleanup.py`

## Invalid clipboard snapshot cleanup

When deferred clipboard setup receives a snapshot result with an invalid
count or format, the result buffer is now released before the clipboard copy
is ended.  This complements the atom-format validation and prevents malformed
snapshot conversions from leaking an allocated value.

The regression check is
`tests/automated/check-clipboard-snapshot-cleanup.py`.

## Clipboard snapshot formats

The Motif clipboard source path treats target lists and the `_MOTIF_SNAPSHOT`
identifier as 32-bit atom data.  It now requires that format explicitly: target
lists must be `XA_ATOM`/32-bit, and a snapshot must contain exactly one 32-bit
item before the value is cast to an atom.  Invalid conversion results are
discarded through the existing clipboard failure cleanup.

The regression check is
`tests/automated/check-clipboard-snapshot-format.py`.

## Clipboard-source widget validation

`XmeClipboardSource` now rejects a null widget before deriving the application
context or touching the display.  This matches the guarded primary, named,
secondary, and sink entry points and makes invalid clipboard-source setup a
safe `False` result.

The focused regression is
`tests/automated/check-clipboard-source-input.py`.

## Clipboard start-copy allocation width

`XmClipboardStartCopy` now passes its fixed item-record allocation through an
explicit `Cardinal` conversion for Xt, matching the allocator’s API width and
avoiding an implicit narrowing warning.

Regression: `tests/automated/check-clipboard-start-copy-allocation-width.py`

## Clipboard start-copy ID bounds

`XmClipboardStartCopy` now rejects nonpositive IDs returned by the internal ID
allocator and explicitly converts the validated label ID into the serialized
unsigned field. A negative allocator result can no longer wrap into a large
clipboard record ID.

Regression: `tests/automated/check-clipboard-start-copy-id-bounds.py`

## Clipboard start-copy lock status

`XmClipboardStartCopy` now stops on every non-success result from
`ClipboardLock`, including lock-property or allocation failures.  It returns
the original status instead of opening or modifying the clipboard after lock
acquisition failed.

Regression: `tests/automated/check-clipboard-start-copy-lock-status.py`

## Clipboard start-retrieve lock status

`XmClipboardStartRetrieve` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening the clipboard
header. This keeps callers informed about display, ownership, and lock
failures instead of continuing with an invalid retrieval transaction.

Regression: `tests/automated/check-clipboard-start-retrieve-lock-status.py`

## Clipboard-start status handling

`XmeClipboardSource` now requires `XmClipboardStartCopy` to return
`XmClipboardSuccess` before storing or using the returned item ID.  All other
failure statuses—including lock, bad format, and internal failures—return
`False` without entering the copy protocol with an uninitialized ID.

The focused regression is
`tests/automated/check-clipboard-start-status.py`.

## Clipboard target buffers

Clipboard target converters must return storage when reporting a non-empty
`XA_ATOM` list. Both ordinary and deferred target paths now reject a missing
buffer before casting or iterating it, and close the active copy cleanly.

The focused regression is
`tests/automated/check-clipboard-target-buffer.py`.

## Clipboard target-list count bounds

Clipboard target lists arrive through converter callbacks with an unsigned
length. The source path now rejects lengths that cannot be represented by its
legacy `int` loop counter before narrowing, and closes the active clipboard
copy cleanly on that failure.

The focused regression is
`tests/automated/check-clipboard-target-count-bounds.py`.

## Clipboard TARGETS size arithmetic

The `TARGETS` conversion now computes its atom-array byte size in `size_t`,
passes the checked value explicitly to Xt allocation, and widens the returned
element count explicitly. This keeps allocation and X11 property-size
arithmetic aligned without changing the selection format.

Regression: `tests/automated/check-clipboard-targets-size-arithmetic.py`

## Clipboard undo-copy lock status

`XmClipboardUndoCopy` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening the clipboard
header. A failed lock can no longer be treated as a usable clipboard state.

Regression: `tests/automated/check-clipboard-undo-lock-status.py`

## Clipboard window-search index width

`ClipboardSearchForWindow` now uses an unsigned loop index matching
`XQueryTree`’s `unsigned int` child count, avoiding signed/unsigned traversal
ambiguity while recursively searching descendants.

Regression: `tests/automated/check-clipboard-window-search-index.py`

## Clipboard window-tree ownership

`ClipboardSearchForWindow` receives its child-window list from Xlib's
`XQueryTree`. The list is now released with `XFree`, matching the Xlib
allocation contract and avoiding allocator mismatches when clipboard ownership
is checked recursively.

## Clipboard withdrawal lock status

`XmClipboardWithdrawFormat` now stops on every non-success result from
`ClipboardLock`, preserving the original status before deleting a format.

Regression: `tests/automated/check-clipboard-withdraw-lock-status.py`

## ClipWindow translation construction

`XmClipWindow` expands virtual key bindings into a real Xt translation table
when the class is initialized. The generated text is retained for the life of
the process because Xt keeps using the returned table. Its storage grows as
needed instead of relying on a fixed 1000-byte buffer, so longer key names or
action strings cannot overwrite adjacent state.

The installed-library widget regression constructs and realizes an
`XmScrolledWindow`, which initializes the ClipWindow path:

```sh
python3 tests/automated/run-widget-regressions.py /path/to/prefix /path/to/results
```

## Complete color-cache properties

Color-selection cache discovery now rejects a property when X reports
remaining bytes after the requested read.  This prevents an oversized or
truncated cache record from entering pixel-data parsing; malformed data still
follows the existing cleanup and cache-miss path.

Regression: `tests/automated/check-color-cache-property-complete.py`

## Color conversion allocation safety

The black-and-white pixmap conversion duplicates a color-table entry before
inspecting it. Allocation failure now leaves the conversion path safe: the
duplicated string is checked before its first-byte test.

The source contract is covered by
`tests/automated/check-color-conversion-allocation.py`.

## Color-name digit suffix detection

Color matching checks whether a name ends in a digit.  Empty and missing names
now return false before pointer arithmetic, avoiding a one-before-start pointer
and making malformed color entries safe to process.

## Color database final lines

Color database files may end without a newline.  The name scanner now treats
that as a valid line terminator instead of dereferencing a null result from
`strchr` and crashing while loading the selector.

## Color-name length bounds

Color database names are copied into fixed-size selector records.  Names at or
above the record limit are truncated, and the loop length is reduced to the
truncated size before normalization.  This prevents an overlong source name
from writing past either fixed destination buffer.

## ColorObj atom-name allocation

ColorObj initialization builds one customization-property name per screen and
interns them in one X request. For displays beyond the inline ten-name buffer,
the names are individually allocated.

The setup now checks the name-vector allocation, tracks each dynamic name, and
releases all names already allocated if a later name fails. Atom interning is
skipped unless the complete vector exists; normal fallback behavior then
continues without dereferencing partial storage.

## ColorObj pixel-data allocation

The ColorObj selection path copies a server-provided pixel-data property into
an Xt-owned, NUL-terminated buffer before parsing it. The allocation is now
checked before the copy; failure releases the X property buffer and returns
without entering the parser with a null input.

## ColorPicker value API inputs

The public ColorPicker committed-value and preview APIs now reject a null
widget before invoking the Xt subclass check. Invalid optional handles return
the existing neutral value or no-op result; valid value, redraw, and callback
behavior is unchanged.

The focused regression check is
`tests/automated/run-colorpicker-native.py`.

## ColorPicker live resource updates

The native ColorPicker applies value, render-table and appearance changes together.
A value update synchronizes the hex editor, visual fields, model channels and
alpha swatch. Owned child fonts follow a replacement render table while explicit
child overrides remain intact. A failed render-table copy retains the valid table.

Mode switching preserves pending text and the current color. Geometry is measured
from native controls, including the actual hint font; explicit axes remain owned
by the caller. Cached field pixels are invalidated for changes to the color-space
parameter, allocation, sensitivity, background or colormap.

Native checks: `run-colorpicker-native.py` and `run-colorpicker-modes.py`.

## Color database RGB parsing

The color selector now checks the result of each RGB channel scan.  Lines that
do not contain three numeric channel values are skipped before their unassigned
storage can be used, so malformed database entries cannot produce arbitrary
colors.

## ColorSelector child allocation

`XmColorSelector` creates slider, mode-toggle, and color-preview children as
separate widgets. Failed creation now skips callback registration and parent
management safely; slider updates ignore missing scales, and layout returns
until its required presentation children exist. Normal color-list and slider
behavior is unchanged.

The contract is protected by
`tests/automated/check-color-selector-child-allocation.py`.

## ColorSelector optional-child updates

ColorSelector can be partially constructed when an internal slider, mode
toggle, or color list cannot be allocated.  Resource changes must remain safe
in that state: label updates now change the stored `XmString` values while
updating only children that exist, and selecting the current color becomes a
no-op when the list is unavailable.

Normal fully constructed selectors still update every child and preserve the
existing list/slider mode behavior.  The guards only make deferred resource
updates and mode changes recoverable after a partial construction failure.

## ColorSelector initialization input

`Initialize` now validates its request/set widgets and creation argument list
before filtering child resources. A malformed initialization call returns
safely; normal Xt construction remains unchanged.

The focused source contract is checked by
`tests/automated/check-color-selector-initialize-input.py`.

## ColorSelector list selection

Selecting a named color now treats `XmStringUnparse` as a fallible
allocation. A failed conversion leaves the current color name untouched;
successful conversions go through the same transactional resource update used
by the public color-name path.

The focused source check is
`tests/automated/check-color-selector-name-allocation.py`.

## ColorSelector mode lifecycle

The selected mode toggle is an optional internal child.  ColorSelector now
keeps initialization and later resource changes safe when the radio box or a
toggle cannot be created: mode selection still updates the widget state, while
the toggle-state update is skipped for a missing child.

The mode callback also rejects incomplete callback data before reading its
reason or set fields.  Fully constructed selectors retain the existing mode
switching and callback behavior.

## ColorSelector mode values

`XmNcolorMode` accepts the public `XmListMode` and `XmScaleMode` values.  A
caller can still bypass the string converter and supply an arbitrary integer
through an `Arg` list.  The mode transition path now normalizes such a value
to `XmScaleMode` before indexing the two-element toggle array or changing
managed children.

Valid resource values and the existing string converter are unchanged; this
only makes direct malformed values recoverable instead of allowing an
out-of-bounds array access.

The string converter now also uses Xt's native `Cardinal *` converter-count
parameter rather than an incompatible value parameter and function-pointer
cast. This keeps resource conversion ABI-correct on platforms where the two
calling conventions differ.

The converter forward declaration also uses `Cardinal *`; a mismatched
prototype is rejected by current Clang before the library can be built.

## ColorSelector color-name allocation

`XmColorSelector` now allocates a replacement color name before swapping it
into the widget state. If `XtNewString` fails, the update returns `False` and
the previous name remains valid; successful color-name display and slider
updates are unchanged.

## ColorSelector preview allocation

ColorSelector treats its color preview as an optional internal child.  If a
resource hook or an allocation failure prevents that child from being created,
color-name parsing and colormap bookkeeping still complete without calling Xt
with a null widget.  Preferred-size measurement skips the missing preview
parent, and layout and color-display updates simply omit the visual update.

This preserves the selector's recoverable behavior during partial construction:
the list, sliders, and radio controls remain usable, and a later relayout does
not turn a failed preview allocation into a crash.

## ColorSelector SetValues input

`SetValues` now validates its current/request/set widget records and the
argument list/count before filtering child resources or reading private color
selector state. Malformed calls return `False`; normal resource updates are
unchanged.

The focused source contract is checked by
`tests/automated/check-color-selector-set-values-input.py`.

## ColorSelector string getters

The ColorSelector synthetic string getters now check their internal label and
error strings before calling `XmStringCopy`. During partial initialization or
allocation failure they return a null value through the requested output slot
instead of dereferencing a missing compound string.

The focused source contract is checked by
`tests/automated/check-color-selector-string-getters.py`.

## Color-object property parsing

Color-object pixel properties are supplied as underscore-delimited
hexadecimal fields from an X-server selection. The parser now uses checked
`strtoul` conversion, rejects signs, overflow, and missing delimiters, and
validates the color-use value before storing it.

The focused regression check is
`tests/automated/check-colorobj-property-hex-parse.py`.

## ColorObj screen allocation cleanup

ColorObj allocates three per-screen arrays for colors, atoms, and usage.  If
any allocation fails, the partially allocated arrays are now released and
cleared before initialization returns.  The object remains in its normal
non-running state without retaining incomplete screen storage.

The regression check is `tests/automated/check-colorobj-screen-allocation.py`.

## ColorSelector argument merging

ColorSelector builds child widgets by merging caller arguments with internal
resource defaults.  The shared merge helper first rejects a `Cardinal` count
that would wrap, then checks the allocated merged list before use.  If Xt
cannot allocate a merged list, the affected helper returns cleanly instead of
passing or dereferencing a null list; successful child construction and
argument ownership remain unchanged.

The regression check is `tests/automated/check-colors-argument-merge.py`.

## Color-name character classification

Color database names are normalized by filtering spaces and changing case.
The normalization and lookup paths now pass unsigned byte values to all C
ctype functions, so non-ASCII bytes in a database entry cannot trigger
undefined behavior.

## ColorSelector RGB-file parsing

ColorSelector database lines now parse each decimal RGB channel with checked
`strtoul` conversion. Malformed, overflowing, or wider-than-16-bit channels
are rejected before they reach the color table; the existing color-name
handling remains unchanged.

The focused regression check is
`tests/automated/check-color-rgb-parse.py`.

## ColorSelector string getter guards

ColorSelector’s synthetic label and error-string getters now validate the
widget class and output slot before reading internal string storage. Invalid
callback inputs return a deterministic null value, while valid reads preserve
the existing copied `XmString` contract for all seven resources.

The focused source-contract check is
`python3 tests/automated/check-colors-selector-string-guards.py`.

## Column default-size bounds

`XmColumn` derives its default width and height from twice the manager shadow
and margin.  The calculation now uses widened arithmetic and clamps to the
X11 `Dimension` range before assignment, preventing large resource values from
wrapping the initial geometry.

The focused source regression is
`tests/automated/check-column-default-size-bounds.py`.

## Column flush-fill height bounds

`XmColumn` gives flush-fill children the column’s inner height after subtracting
shadow and margin space.  A very small allocation can make that signed
calculation negative; converting it directly to Xt’s unsigned `Dimension`
would otherwise create an enormous child.  The layout now clamps the inner
height to one pixel before the conversion, preserving a valid geometry request
while retaining ordinary sizing behavior.

The source contract is checked by
`tests/automated/check-column-fill-height-bounds.py`.

## Column label teardown guard

The Column label-destroy callback now validates its client field widget before
clearing the associated label pointer. Null or wrong-class callback payloads
are ignored safely; normal label destruction still clears the owning Column
field state.

The focused source-contract check is
`python3 tests/automated/check-column-label-destroy-guard.py`.

## Column label getter guard

The Column synthetic label getter now validates the widget class and output
slot before reading its constraint label. Invalid callback inputs return a
deterministic null value; valid Column children retain the existing copied
`XmString` ownership contract.

The focused source-contract check is
`python3 tests/automated/check-column-label-getter-guard.py`.

## ComboBox argument merging

ComboBox child and convenience constructors merge caller arguments with
internal defaults before creating widgets.  The shared merge helper rejects a
`Cardinal` count that would wrap, and allocation failure returns a null widget
result instead of passing a null argument list into Xt.  The edit-box path also
releases its temporary selected-item string on a failed merge and waits for a
successful child before inheriting its font.  Successful construction and
argument ownership are unchanged.

The regression check is `tests/automated/check-combobox-argument-merge.py`.

## ComboBox callback class guards

All ComboBox synthetic import and export callbacks now validate that the
callback widget is a ComboBox before accessing its private child state. Null
and non-ComboBox inputs are rejected using the existing callback sentinel or
return path, while valid ComboBox behavior is unchanged.

The focused source-contract check is
`python3 tests/automated/check-combobox-callback-class-guards.py`.

## ComboBox edit-field allocation

ComboBox now stops initialization when its text-field child cannot be created.
The constructor returns cleanly before managing or asserting on the missing
child or inheriting its font, and initialization registers the value-change
callback only when the edit field exists.

## ComboBox item-setting API inputs

`XmComboBoxSetItem` now rejects a null widget before looking up the Xt
application context. Invalid optional handles remain a no-op; valid item
selection, scrolling, and callback behavior is unchanged.

The focused regression check is `tests/automated/check-combobox-inputs.py`.

## ComboBox natural-size bounds

ComboBox now accumulates natural width and height in a wide integer while
combining text/list preferred sizes, borders, margins, and arrow metrics. The
result is explicitly saturated to Motif's `Dimension` range instead of
wrapping through a 16-bit intermediate. The default arrow-size calculation
uses the same boundary.

The focused regression is
`tests/automated/check-combobox-collapsed-geometry.py`.

## ComboBox popup allocation

ComboBox now checks popup-shell creation before registering shell callbacks and
event handlers.  Its scrolled-list constructor also checks the returned list
before managing it, reading its parent, or querying scrollbar children.

Normal ComboBox and drop-down construction keeps the existing child names and
selection callbacks; allocation failures now return cleanly instead of
dereferencing a null child.

## ComboBox popup position bounds

Drop-down ComboBox popup placement now performs its root-coordinate arithmetic
in `int`. The final shell `XmNx`/`XmNy` values are explicitly clamped when
crossing the legacy X11 `Position` boundary, rather than silently wrapping
during the intermediate calculation. This preserves ordinary popup placement
and keeps oversized/high-DPI geometry on screen.

The focused regression is
`tests/automated/check-combobox-popup-position-bounds.py`.

## ComboBox selected-item import guards

Synthetic selected-item import now rejects null values and incomplete list or
edit-box children before lookup. A failed edit-box snapshot also returns
without comparing or updating the selection, preserving the existing state.

## ComboBox selected-position bounds

Zero-based ComboBox compatibility converts the public selected position to the
List widget's one-based position. `INT_MAX` has no representable one-based
successor, and negative values other than `XmINVALID_POSITION` are not valid
positions. ComboBox construction now leaves the child List unselected for
those inputs instead of wrapping or indexing with an invalid position.

Regression coverage: `tests/automated/check-combobox-selected-position-bounds.py`.

## ComboBox selected-position export guards

The synthetic `XmNselectedPosition` export now returns before querying the
list child when invoked before ComboBox initialization has created children.
This matches the existing import guard and prevents pre-initialization resource
access from dereferencing incomplete widget state.

## ComboBox synthetic accessor guards

ComboBox synthetic selected-item, selected-position, column, item, count, and
visible-count accessors now validate widget and value pointers before touching
child state. Import paths also return safely before initialization when the
list child is unavailable.

## ComboBox synthetic accessor defaults

Before child creation, selected-item export now returns a null `XmString` and
selected-position export returns `XmINVALID_POSITION`. Selected-position
import also requires the list child. These deterministic defaults avoid
leaving caller output uninitialized during resource setup.

## Command callback input

The Command widget's list and text callbacks now validate their callback and
client payloads before reading them. Invalid delivery is ignored, preserving
normal selection and value-change behavior while preventing teardown or
embedder errors from becoming null-pointer dereferences.

The focused source contract is checked by
`tests/automated/check-command-callback-input.py`.

## CommandPalette child allocation

CommandPalette creates a search box and command list during initialization.
Filtering, layout, activation, callbacks, and showing the palette now remain
safe when either internal child fails to allocate.

The contract is protected by
`tests/automated/check-command-palette-child-failure-guards.py`.

## CommandPalette event guards

CommandPalette validates its widget type and event pointer before handling
Escape dismissal. Invalid dispatch is ignored safely while normal live-palette
behavior is unchanged.

## Command-palette item transactions

`XmCommandPaletteSetItems` now installs an item batch transactionally. If an
ID, label, or shortcut copy fails, all partial records are released and the
palette remains empty rather than exposing partially initialized callbacks or
filter entries.

The focused source check is
`tests/automated/check-command-palette-item-allocation.py`.

## CommandPalette display-label assembly

Command-palette filtering already computes the exact label-plus-shortcut size.
It now fills that buffer with offset-based copies instead of repeated string
scans, preserving the displayed text while keeping rebuild work linear for
large command lists.

The source contract is checked by:

```sh
python3 tests/automated/check-command-palette-linear-copy.py
```

## Command text-string allocation

`CommandCallback` now stops cleanly when retrieving the command text fails to
allocate a string. It avoids passing a null value into `XmStringGenerate` and
leaves the existing value-change callback behavior unchanged for valid text.

The focused source contract is checked by
`tests/automated/check-command-string-allocation.py`.

## Command compound-string allocation

`CommandCallback` now checks the result of `XmStringGenerate` before measuring
or delivering the value. If compound-string allocation fails, the callback
returns after releasing the retrieved text instead of dereferencing a null
value. Successful command notifications are unchanged.

The focused source contract is checked by
`tests/automated/check-command-xmstring-allocation.py`.

## Toolbar and StatusBar layout totals

Toolbar and StatusBar selection compare accumulated child widths with the
available widget width.  On 32-bit platforms, an unusually large item set could
wrap the `unsigned long` total and make an item appear to fit.  Both widgets
now saturate each width-plus-spacing addition at `ULONG_MAX` before comparing
or placing items.  Normal-sized layouts retain their previous geometry.
Scratch-array byte counts are also checked against Xt's `Cardinal` allocation
width before `XtCalloc` is called.
Preferred-size queries and internal-child mapping are also null-safe, so a
failed child creation cannot crash a later relayout.

Runtime spacing changes now request a fresh preferred-size negotiation for both
widgets, so the parent can resize them instead of leaving the new spacing
clipped inside the old allocation.

The contract is protected by
`tests/automated/check-composite-layout-total-bounds.py`.

## Compound-text conversion failures

When importing a COMPOUND_TEXT property, every bounded segment must convert to
an `XmString`. A failed segment conversion now releases previously converted
entries and rejects the entire table instead of returning success with a NULL
entry. This keeps the public result internally valid for callers that iterate
the returned string table.

The focused regression check is
`tests/automated/check-compound-text-conversion-failure.py`.

## Compound-text extended segment length

The compound-text extended-segment wire format encodes its payload length in
two seven-bit fields, so a single segment can represent at most 16,383 bytes.
When exporting a non-standard charset component, Motif now rejects a component
that cannot fit that encoding rather than wrapping the wire length and emitting
malformed compound text. Its temporary header allocation is also proved to fit
Xt's `Cardinal` size before allocation.

The focused regression is
`tests/automated/check-ctext-extended-segment-length.py`.

## Compound-text tab prefixes

`GetTextSegment` can prepend tab characters without marking the segment as a
line separator. That path now leaves the text terminated only by its normal
NUL byte; it does not append a newline unless the compound-text separator flag
is set. This prevents a one-byte overwrite of the allocated output buffer and
preserves the distinction between indentation and a new line.

The focused regression check is
`tests/automated/check-compound-text-tab-prefix.py`.

## Convenience-constructor argument counts

Scrolled Text and List constructors add internal arguments to the caller's
argument count. They now reject counts that would overflow the `Cardinal`
addition before allocating or merging argument arrays. Normal argument lists
retain the existing behavior; an overflow returns `NULL` without reading the
caller-provided array.

## Container default-action callback input validation

The Container default-action callback now returns safely when its child,
parent, or constraint record is unavailable during teardown or unusual
dispatch. Valid default-action callbacks are unchanged.

The source contract is checked by
tests/automated/check-container-action-callback-inputs.py.

## Container activation input guards

The Container activate action now returns safely when its event or
parameter-count pointer is missing before forwarding outline-button or item
activation.  Normal keyboard and pointer activation remains unchanged.

The source contract is checked by
`tests/automated/check-container-activate-input-guards.py`.

## Container anchor-toggle constraint guards

When extended selection toggles the anchor item, Container now returns safely
if the anchor has no private constraint record before reading its selection
state.  Valid anchor toggles and marquee setup remain unchanged.

The source contract is checked by
`tests/automated/check-container-anchor-toggle-constraint-guards.py`.

## Container begin-extend input guards

The Container begin-extend action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button arming and
range-selection hit testing from malformed programmatic action calls while
leaving normal pointer-driven extension unchanged.

The source contract is checked by
`tests/automated/check-container-begin-extend-input-guards.py`.

## Container begin-select input guards

The Container begin-selection action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button hit testing
and callback dispatch from malformed programmatic action calls while leaving
normal pointer-driven selection unchanged.

The source contract is checked by
`tests/automated/check-container-begin-select-input-guards.py`.

## Container begin-toggle input guards

The Container begin-toggle action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button traversal
and toggle selection from malformed programmatic action calls while leaving
normal pointer-driven behavior unchanged.

The source contract is checked by
`tests/automated/check-container-begin-toggle-input-guards.py`.

## Container button-down constraint guards

The Container button-down action now requires a valid child constraint before
reading selection state for a transfer or selection decision.  A hit-tested
widget can briefly lack private constraint storage during child lifecycle
changes; such input follows the existing non-transfer path instead of
dereferencing unavailable state.

The source contract is checked by
`tests/automated/check-container-button-down-constraint-guards.py`.

## Container button-motion input validation

Button-motion selection now returns safely when its widget or event is absent,
and handles a missing current-item constraint without dereferencing it. Valid
drag-selection behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-button-motion-inputs.py`.

## Container button-up parameter guards

The Container button-up action now handles a missing action-parameter count as
an invalid invocation before dereferencing it.  Valid Xt action dispatch keeps
the existing timer, drag-cancellation, and follow-up action behavior.

The source contract is checked by
`tests/automated/check-container-button-up-parameter-guards.py`.

## Container button-two parameter guards

The button-two down and motion actions now reject a missing action-parameter
count before validating or using their parameter list.  This keeps malformed
or partially initialized action dispatches on the existing warning/no-op path;
normal Xt dispatch is unchanged.

The source contract is checked by
`tests/automated/check-container-button2-parameter-guards.py`.

## Container cancel input guards

The Container cancel action now returns safely when its event or
parameter-count pointer is missing.  This protects parent-gadget cancellation
and primary-selection timestamp handling from malformed action calls while
leaving normal cancellation behavior unchanged.

The source contract is checked by
`tests/automated/check-container-cancel-input-guards.py`.

## Container cell-count bounds

Container preferred-size calculation accumulates the number of cells needed
for grid and cell layouts in an `int` used by the integer square-root helper.
The accumulation now saturates at `INT_MAX` instead of overflowing when a large
set of children or a large child cell footprint exceeds that range.  Normal
small layouts retain the same result.

`tests/automated/check-container-cell-count-bounds.py` protects the saturating
arithmetic.

## Container cell-layout allocation

The cell-layout trial pass allocates a temporary occupancy grid from its
requested width and height.  It now checks positive dimensions and both
multiplications before allocating, verifies the resulting byte count against
both `size_t` and Xt's `Cardinal` limit, returns safely when the grid cannot
be created, and stops before incrementing dimensions past `INT_MAX`.  Normal
layout placement and recursive fitting remain unchanged.

`tests/automated/check-container-cell-layout-allocation.py` protects these
bounds and the allocation-failure return.

## Container cell metrics

The cell-style preferred-size calculation divides child dimensions by the
current cell width and height.  During early resource initialization those
metrics can still be zero.  The calculation now returns without changing the
caller’s requested size until valid metrics are available, avoiding a
process-killing division-by-zero while preserving the normal layout path.

`tests/automated/check-container-cell-metrics.py` protects this guard.

## Container cell-position bounds

Spatial cell coordinates are ultimately stored in X11 `Position`/`XPoint`
fields.  `GetCoordFromCell` now uses saturating products for the cell index,
cell dimension, and margin instead of allowing large grids to wrap into a
negative or unrelated location.  Invalid or incomplete grids retain the
existing safe fallback coordinate.

The focused source contract is covered by
`tests/automated/check-container-cell-position.py`.

## Container cell-position guards

Cell coordinate conversion can run while a Container is between layouts.  The
helpers now reject zero cell metrics or zero current grid dimensions before any
division.  Coordinate-to-cell conversion returns `NO_CELL`; cell-to-coordinate
conversion returns the container margin origin.  A null output point is also
handled safely.

`tests/automated/check-container-cell-position.py` protects these fallback
contracts.

## Container cell-grid reset

`PlaceItemReset` now validates cell metrics before dividing, rejects an
unrepresentable cell-grid product, verifies the occupancy-array byte size
against both host `size_t` and Xt's `Cardinal` allocation limit, and checks
the new allocation.  A failed reset clears the active grid dimensions and
returns without publishing a partially initialized cell layout, so later
placement cannot dereference a missing grid.

`tests/automated/check-container-cell-reset.py` protects these checks.

## Container cell-size dispatch input validation

Cell-size dispatch now returns safely when no Container widget is supplied,
before querying spatial style or icon view state. Valid small- and large-cell
dispatch is unchanged.

The source contract is checked by
`tests/automated/check-container-cell-size-dispatch-inputs.py`.

## Container CELLS cell-size guard

The spatial CELLS resize path divides child dimensions by the computed cell
width and height.  A zero metric can occur before cell metrics are established
or after an invalid resource update.  The path now initializes both values,
rejects each requested zero dimension before it is used, and only then
performs the divisions.  GRID behavior and public resource compatibility are
unchanged.

The focused source contract is checked by
`tests/automated/check-container-cell-size-guard.py`.

## Container child constraint guard

`XmContainerGetItemChildren` now rejects a child whose constraint storage is
missing before reading its Container constraint record. The existing parent
check remains in place, and valid child enumeration is unchanged.

The source contract is checked by
`tests/automated/check-container-child-constraint-guard.py`.

## Container clipboard widget-class guards

`XmContainerCut`, `XmContainerCopy`, `XmContainerPaste`,
`XmContainerCopyLink`, and `XmContainerPasteLink` now reject `NULL` and
non-Container widget handles before reading selection state or entering the
clipboard transfer path. Invalid callers receive `False`; valid
clipboard operations are unchanged.

The source contract is checked by
`tests/automated/check-container-clipboard-widget-class-guards.py`.

## Container constraint lifecycle input validation

Container child initialization now handles a missing parent constraint without
dereferencing it, and constraint destruction returns safely when its child or
parent state is unavailable. Valid hierarchy initialization and teardown are
unchanged.

The source contract is checked by
`tests/automated/check-container-constraint-lifecycle-inputs.py`.

## Container control-flow clarity

Two Container branches mixed deep indentation with single-statement `if` and
`else` bodies.  The outline-visibility and keyboard-traversal paths now use
explicit blocks, preserving behavior while making the executed scope clear to
humans and compilers.  This removes the corresponding misleading-indentation
warnings from strict builds.

## Container conversion callback input validation

The Container selection conversion callback now rejects a null widget or
conversion callback structure before resolving atoms or reading target data.
Malformed callback delivery therefore returns safely without changing normal
selection conversion behavior.

The source contract is checked by
`tests/automated/check-container-convert-inputs.py`.

## Container conversion refusal input validation

The private Container refusal callback now returns safely when the conversion
callback payload is absent, instead of writing through a null pointer. Normal
conversion refusal still sets the existing refusal status and empty result.

The source contract is checked by
`tests/automated/check-container-convert-refuse-input.py`.

## Container cursor-navigation input validation

Cursor navigation now returns safely for missing widgets or direction strings,
and `_LAST` navigation checks that the relevant child nodes and widget pointers
exist before traversal. Valid spatial and outline navigation is unchanged.

The source contract is checked by
`tests/automated/check-container-cursor-navigation-inputs.py`.

## Container node deletion input validation

Node deletion now returns safely for a missing child widget or constraint
record before recursing through descendants. Valid recursive tree teardown is
unchanged.

The source contract is checked by
`tests/automated/check-container-delete-node-inputs.py`.

## Container deselection input validation

Deselect-all now returns safely when no Container widget is supplied, before
reading selection counters or traversing nodes. Valid selection clearing is
unchanged.

The source contract is checked by
`tests/automated/check-container-deselect-inputs.py`.

## Container destination prehook input validation

The Container destination prehook now rejects a null widget or destination
callback structure before reading selection or transfer fields. This keeps
malformed callback delivery from dereferencing invalid input while preserving
normal drag-and-drop transfers.

The source contract is checked by
`tests/automated/check-container-destination-prehook-inputs.py`.

## Container drag-icon cache allocation

Container drag initiation keeps one cached drag-icon record per display. The
record is allocated before it is saved in the X context and initialized.

`DragStart` now checks this allocation and returns before touching the context
or record when memory is unavailable. Existing cached records and normal drag
behavior are unchanged.

## Container drag-icon resources

Container drag-and-drop creates two server-side pixmaps, a graphics context,
and a drag-icon widget for the temporary state indicator.  The construction
path now checks every X resource before drawing or publishing the per-display
context.  Partial resources are released when creation fails, so a server
resource exhaustion or an unrealized/invalid drawing target cannot turn into a
null graphics call or a half-published drag icon.

## Container marquee drawing input validation

`DrawMarquee` now returns safely when called without a widget before checking
realization or reading Container state. Valid realized marquee drawing is
unchanged.

The source contract is checked by
`tests/automated/check-container-draw-marquee-input.py`.

## Container drop-finish callback input validation

The private drop-finish callback now returns safely when no Container payload
was supplied, instead of dereferencing a null client pointer. Valid drag
completion still clears the Container's drag context.

The source contract is checked by
`tests/automated/check-container-drop-done-input.py`.

## Dynamic Container tab-list input validation

Dynamic detail-tab generation now returns safely when called without a
Container widget. Existing tab sizing, allocation, and fallback behavior for
valid widgets is unchanged.

The source contract is checked by
`tests/automated/check-container-dynamic-tab-inputs.py`.

## Container end-extend input guards

The Container end-extend action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button activation
and primary-selection timestamp handling from malformed action calls while
leaving normal range-selection completion unchanged.

The source contract is checked by
`tests/automated/check-container-end-extend-input-guards.py`.

## Container end-select focus guards

When finishing selection, Container now treats a focused child without a
constraint record as outside the selected set instead of dereferencing null
state.  Focus restoration and valid selection behavior remain unchanged.

The source contract is checked by
`tests/automated/check-container-end-select-focus-guards.py`.

## Container event-handler input validation

Container enter and leave handlers now return safely for missing widgets,
missing crossing events, or a missing clipping parent before accessing event
or widget geometry during teardown and unusual dispatch.

The source contract is checked by
`tests/automated/check-container-event-handler-inputs.py`.

## Container expand/collapse constraint guards

The Container expand/collapse action now returns safely when the focused child
has no Container constraint record.  Its outline state is private constraint
data; valid focus changes retain their existing expand/collapse behavior.

The source contract is checked by
`tests/automated/check-container-expand-constraint-guards.py`.

## Container cursor-extension input guards

The Container cursor-extension action now rejects missing events, parameter
counts, empty parameter lists, and null parameter storage before using the
direction token or forwarding range selection.  Normal keyboard extension is
unchanged.

The source contract is checked by
`tests/automated/check-container-extend-cursor-input-guards.py`.

## Container keyboard-extend event guards

The Container keyboard extend action now returns safely when invoked without an
event.  Range selection and primary-selection notification both require the
event timestamp; malformed programmatic calls no longer dereference it.

The source contract is checked by
`tests/automated/check-container-extend-event-guards.py`.

## Container first-column bounds

Container outline/detail layouts derive the first column from an item width,
the outline-button width, and the item depth multiplied by the indentation.
Those values are ultimately represented by Motif's `Dimension` type.  The
calculation now saturates at the largest representable `Dimension` instead of
wrapping to a small width when a deeply nested tree or large display metric
exceeds that range.  Existing resource types, layout policy, and public ABI
are unchanged.

The source contract is checked by
`tests/automated/check-container-first-column-bounds.py`; the changed object is
also compiled in the native rpi4 incremental build.

## Dynamic Container first-column input validation

Dynamic first-column sizing now returns safely for a missing Container widget
and skips malformed child nodes or missing constraint records. Valid icon
measurement and column sizing remain unchanged.

The source contract is checked by
`tests/automated/check-container-first-column-inputs.py`.

## Container fit-test constraint guards

`TestFitItem` now returns `False` when the candidate child has no Container
constraint record.  Grid and cell placement consult `cell_idx` from that
record; rejecting malformed input keeps hit testing and placement checks from
dereferencing unavailable private state.  Valid candidates retain their
existing fit behavior.

The source contract is checked by
`tests/automated/check-container-fit-constraint-guards.py`.

## Container fit edge arithmetic

`TestFitItem` rejects a candidate outside the right-to-left content edge before
it consults the occupancy grid.  Width and margin are unsigned Motif geometry
fields; subtracting them first and then casting to `Position` could wrap when a
transient allocation was smaller than its margin, causing an invalid candidate
to pass the edge test.

The right-to-left comparison now widens both operands before subtraction.  The
valid placement behavior is unchanged for ordinary allocations, while tiny
allocations are rejected deterministically.  The source contract is protected
by `tests/automated/check-container-cell-placement-guards.py`.

## Container graphics-context input validation

Graphics-context refresh now returns safely when no Container widget is
provided, before releasing or allocating graphics contexts. Valid GC refresh
behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-gc-inputs.py`.

## Container geometry input validation

Container geometry negotiation now rejects a missing child, request, parent, or
constraint record before reading widget state. Valid geometry requests keep
their existing negotiation behavior.

The source contract is checked by
`tests/automated/check-container-geometry-inputs.py`.

## Container child-list constraint guards

`XmContainerGetItemChildren` now treats a missing private constraint record as
an empty result and releases the application lock before returning.  Valid
parent and child lookups are unchanged, while teardown-time or malformed item
handles no longer reach `node_ptr` through a null record.

The source contract is checked by
`tests/automated/check-container-get-item-children-constraint-guards.py`.

## Container closest-placement growth edges

When a Container uses closest inclusion, a requested child position beyond the
current content edge contributes enough cell space for the child to fit.  The
content edge is formed from unsigned Motif dimensions and margins, so subtracting
those values before widening can wrap when a transient allocation is smaller than
its margins.  The remainder test also must apply to the widened excess, rather
than to a mixed-precedence unsigned expression.  None-style growth has the
corresponding risk when a signed child origin is added to unsigned dimensions.

`RequestSpatialGrowth` now widens both content edges and excess distances before
division.  This preserves the existing cell-growth policy while preventing
underflow and incorrect extra-cell calculations during resize and closest
placement.  `tests/automated/check-container-growth-bounds.py` protects the
contract and the none-style signed-origin path.

## Container outline-hiding input validation

The outline-hiding helper now returns safely for a child without a Container
constraint and skips malformed child-list entries before recursive hiding.
Valid outline visibility behavior is unchanged.

The source contract is checked by
tests/automated/check-container-hide-inputs.py.

## Container hit-test input validation

Hit testing now returns no item when called without a Container widget, before
accessing the node list or layout state. Valid coordinate hit testing is
unchanged.

The source contract is checked by
`tests/automated/check-container-hit-test-inputs.py`.

## Container icon-header input validation

Icon-header creation now returns safely for a missing Container widget and
skips optional scrolled-parent trait probing when the parent is unavailable.
Normal header creation and scrolled-container integration are unchanged.

The source contract is checked by
`tests/automated/check-container-icon-header-inputs.py`.

## Container icon-header update validation

Icon-header updates now return safely when the header or heading data is
missing, and restore the internal update flag on both failure and success.
Valid heading updates retain their existing labels and detail counts.

The source contract is checked by
`tests/automated/check-container-icon-header-update-inputs.py`.

## Container icon-width edge arithmetic

`ResizeIconWidths` derives a common right edge from the current allocation,
margins, icon-header geometry, and queried child widths.  Those inputs mix
signed X11 positions with unsigned Motif dimensions.  Computing the edge or a
child width directly in `Position`/`Dimension` arithmetic could wrap on a tiny
allocation, producing a very wide icon or a negative-looking right-to-left
origin.

The routine now keeps edge calculations signed and widens each queried edge
before comparison.  Available child widths are saturated to `Dimension` only
after checking that the edge is positive, so an undersized allocation yields a
zero width rather than wrapped geometry.  The contract is protected by
`tests/automated/check-container-icon-width-bounds.py`.

## Container ideal-geometry bounds

Container geometry negotiation accumulates header and child extents into
`Dimension` fields.  It also derives child `Position` values from outline
depth and indentation.  Those calculations now saturate instead of wrapping
when a large font, deeply nested tree, or unusually wide child exceeds the
legacy coordinate range.  This keeps negotiation monotonic and prevents a
wrapped value from producing a misleadingly small requested geometry.

The public resource types and ABI remain unchanged.  The focused source
contract is checked by
`tests/automated/check-container-ideal-geometry-bounds.py`.

## Container insertion sibling-constraint guards

Container node insertion now skips siblings whose private constraint record is
unavailable while resequencing.  This keeps malformed or partially torn-down
siblings from crashing position ordering; valid sibling ordering remains
unchanged.

The source contract is checked by
`tests/automated/check-container-insert-sibling-constraint-guards.py`.

## Container item-trait input validation

Visual-emphasis and view-type trait helpers now return safe defaults for a
missing child widget, and visual-emphasis updates return when the child has no
Container parent. Valid trait dispatch is unchanged.

The source contract is checked by
`tests/automated/check-container-item-trait-inputs.py`.

## Container keyboard-selection input validation

The Container keyboard-selection action now rejects missing widgets or events
before focus lookup, and safely handles a focused child without a constraint
record. Valid keyboard selection and primary-transfer behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-kbselect-inputs.py`.

## Container large-cell input validation

Large-cell sizing now returns safely for a missing Container widget and falls
back to screen-based defaults when the node list contains no usable child
widget. Valid dynamic sizing remains unchanged.

The source contract is checked by
`tests/automated/check-container-large-cell-inputs.py`.

## Container outline-layout allocation

Container detail layout builds temporary outline segments and depth points.
Those allocations are now checked before the layout loop dereferences them.
When geometry storage cannot be allocated, the children are still positioned
and the outline is omitted for that pass; the persistent segment count is
cleared so later painting cannot read a missing array.

`tests/automated/check-container-layout-allocation.py` protects the checked
geometry allocations and the outline fallback.

## Container location-cursor input validation

Location-cursor restoration now returns safely when a child has no parent
Container before querying focus or requesting traversal. Valid focus restoration
is unchanged.

The source contract is checked by
`tests/automated/check-container-location-cursor-inputs.py`.

## Container managed-child constraint guards

`XmContainer`'s `ChangeManaged` method now skips a managed child when its
Container constraint record is unavailable.  The method can be reached while
children are being added or removed; a missing record must not be passed to
`NodeIsActive` or outline-state checks.  Valid children keep the existing
visibility and layout behavior.

The source contract is checked by
`tests/automated/check-container-managed-constraint-guards.py`.

## Container marquee input validation

Marquee selection now skips null child nodes and missing constraints, while
`InMarquee` returns safely when a child has no Container parent. Valid marquee
selection behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-marquee-inputs.py`.

## Container marquee recalculation input validation

Marquee geometry recalculation now returns safely without a Container widget,
and the extend-both path requires an actual anchor child before reading its
geometry. Teardown-time selection callbacks cannot dereference missing anchor
state.

The source contract is checked by
`tests/automated/check-container-marquee-recalc-inputs.py`.

## Container max-depth input validation

The optional max-depth traversal now returns safely for missing container or
constraint state and skips malformed child nodes. Valid depth accumulation is
unchanged when `FIX_1425` is enabled.

The source contract is checked by
`tests/automated/check-container-max-depth-inputs.py`.

## Container cursor-movement input guards

The Container cursor-movement action now rejects missing events, parameter
counts, empty parameter lists, and null parameter storage before using the
direction token or forwarding keyboard selection.  Normal keyboard traversal
is unchanged.

The source contract is checked by
`tests/automated/check-container-move-cursor-input-guards.py`.

## Container DnD placement callback guards

`MoveItemCallback` now rejects missing callback data, transfer value data, and
child Container constraints before adjusting the drop point or placing the
item.  Valid drag-and-drop placement is unchanged; malformed or teardown-time
callbacks now return safely.

The source contract is checked by
`tests/automated/check-container-move-item-callback-input-guards.py`.

## Container traversal-node activity validation

`NodeIsActive` now rejects nodes without widgets, parents, or required outline
constraints before querying management or visibility state. Traversal helpers
therefore skip malformed or teardown-time nodes consistently.

The source contract is checked by
`tests/automated/check-container-node-active-inputs.py`.

## Container node allocation

`XmContainer` records each icon child in a private `CwidNode` linked list.
The constraint initializer now treats failure to allocate that node as a
normal initialization failure: it leaves the child unlinked and returns before
the node is dereferenced.  The existing container and child resources remain
owned by Xt, and destruction, layout, selection, and query paths tolerate the
missing private node.  If a new child refers to a parent that has no node, the
child is retained as a root item rather than being linked through a null
parent.

This preserves the public Container resource and child-widget contract while
avoiding a null dereference when an application creates a child during memory
pressure.

## Container node lifecycle input validation

Node creation and insertion now reject missing children or constraints,
normalize an unavailable parent constraint to a root entry, and keep view
traversal from calling into malformed nodes. Valid tree construction and view
changes are unchanged.

The source contract is checked by
`tests/automated/check-container-node-lifecycle-inputs.py`.

## Container outline action state validation

The private outline action now returns safely when its child has no parent
Container or no constraint record, including teardown-time callback delivery.
Normal outline state transitions remain unchanged.

The source contract is checked by
`tests/automated/check-container-outline-action-inputs.py`.

## Container outline-button input validation

Outline-button creation now validates the source child, parent, and
constraint, and unwinds cleanly if gadget creation or its constraint record
fails. Valid outline-button creation is unchanged.

The source contract is checked by
tests/automated/check-container-outline-button-inputs.py.

## Container outline callback input validation

The private outline-button callback now rejects missing client widgets and
callback payloads before looking up or dereferencing the child constraint. It
also safely handles a child without a constraint record.

The source contract is checked by
`tests/automated/check-container-outline-callback-inputs.py`.

## Container outline-button refresh input validation

Outline-button refresh now returns safely when no Container widget is supplied,
before traversing child constraints or updating pixmaps. Valid refresh behavior
is unchanged.

The source contract is checked by
`tests/automated/check-container-outline-change-inputs.py`.

## Container outline layout input validation

Outline layout now returns safely for a missing Container widget, skips
malformed child or constraint records, and omits connector lines whose parent
constraint is unavailable. Valid outline geometry and connector rendering are
unchanged.

The source contract is checked by
`tests/automated/check-container-outline-layout-inputs.py`.

## Container outline-detail position bounds

Outline-detail placement combines unsigned widget dimensions and margins with
signed X11 positions, depth indentation, and outline-button widths.  The old
right-to-left calculations performed those operations before converting to
`Position`, so a small or transient allocation could wrap an item to the wrong
side of the Container.

`LayoutOutlineDetail` now widens edge calculations and uses the existing
saturating position helpers for indentation, outline-button offsets, first
column limits, and final child placement.  Normal geometry is unchanged; values
outside the representable X11 position range clamp instead of wrapping.  The
contract is protected by `tests/automated/check-container-outline-position-bounds.py`.

## Container outline geometry request validation

Outline-detail geometry requests now return safely when the Container widget or
requested geometry record is missing. Valid layout negotiation and redraw
behavior are unchanged.

The source contract is checked by
`tests/automated/check-container-outline-request-inputs.py`.

## Container outline-button sizing input validation

Outline-button sizing now returns safely for a missing Container widget or a
failed temporary gadget allocation, restoring the internal update flag on the
failure path. The outline-column synthetic resource also initializes its
container pointer only after validating the widget. Valid size measurement is
unchanged.

The source contract is checked by
`tests/automated/check-container-outline-size-inputs.py`.

## Container outline-state input validation

Expand and collapse now validate their Container parent and constraint, and
skip malformed child nodes or missing child constraints before updating outline
visibility. Valid outline-state transitions remain unchanged.

The source contract is checked by
tests/automated/check-container-outline-state-inputs.py.

## Container parent-constraint guards

When an icon changes outline parent, `ConstraintSetValues` now checks that the
new parent still has a Container constraint record.  If it does not, the icon
is detached to the root level and its depth/visibility state is reset instead
of dereferencing stale parent state.  Existing parent relationships retain
their normal depth and visibility behavior.

The source contract is checked by
`tests/automated/check-container-parent-constraint-guards.py`.

## Container grid/cell placement constraint guards

`PlaceItemGridCells` now returns when the candidate child has no Container
constraint record.  Grid and cell placement require the record's logical
coordinates for closest-cell selection and snapping; valid placement is
unchanged.

The source contract is checked by
`tests/automated/check-container-place-grid-constraint-guards.py`.

## Container none-style placement constraint guards

`PlaceItemNone` now returns when the candidate child has no Container
constraint record.  Placement state (`cell_idx`) is private constraint data;
valid none-style placement remains unchanged.

The source contract is checked by
`tests/automated/check-container-place-none-constraint-guards.py`.

## Container placement position bounds

`PlaceCwid` clamps children to Container margins and, for right-to-left
layouts, to the right content edge.  The old code cast unsigned dimensions to
`Position` before adding or subtracting margins.  `HideCwid` had the same issue
when moving a child to a negative off-screen coordinate.

Both paths now widen dimensions and margins first, then use the Container
saturating position helper.  Ordinary placement is unchanged; undersized or
oversized transient geometry no longer wraps to an unrelated screen position.
The contract is protected by
`tests/automated/check-container-place-position-bounds.py`.

## Container real-header input validation

Real icon-header lookup now returns safely for a missing Container widget, a
non-composite stored header, or a composite header with no child. Valid header
lookup remains unchanged.

The source contract is checked by
`tests/automated/check-container-real-header-inputs.py`.

## Container traversal redirection input validation

Traversal redirection now returns the requested focus unchanged when the
Container has no clip-window parent, instead of passing a null parent to the
clip-widget class test. Normal traversal wrapping is unchanged.

The source contract is checked by
`tests/automated/check-container-redirect-traversal-inputs.py`.

## Container removal constraint guards

`RemoveItem` now returns `False` when the child has no Container constraint
record before checking placement or updating occupancy.  Valid grid, cell, and
none-style removal remains unchanged.

The source contract is checked by
`tests/automated/check-container-remove-item-constraint-guards.py`.

## Container reorder constraint guards

`XmContainerReorder` now validates the first item’s constraint and node record,
and skips list entries whose constraints are unavailable.  The reorder API
continues to preserve valid sibling ordering while malformed or teardown-time
items are ignored safely.

The source contract is checked by
`tests/automated/check-container-reorder-constraint-guards.py`.

## Container reorder parent guards

`XmContainerReorder` now verifies that the first child belongs to the target
Container and that every listed child has constraint storage before reading
its reorder metadata. Later entries from another parent are ignored, while a
foreign first entry aborts the operation without changing order. Valid
reorders are unchanged.

The source contract is checked by
`tests/automated/check-container-reorder-parent-guards.py`.

## Container node resequencing input validation

Node resequencing now returns safely for a missing Container or unavailable
entry-parent constraint and skips malformed nodes or missing child constraints.
Valid position-index resequencing is unchanged.

The source contract is checked by
`tests/automated/check-container-resequence-inputs.py`.

## Container icon-width input validation

Icon-width resizing now returns safely for a missing Container widget and skips
malformed child nodes or missing constraint records in both measurement and
configuration passes. Valid layout behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-resize-inputs.py`.

## Container scroll arithmetic

Auto-scroll navigator positions now apply increments through a wide,
saturating helper before converting back to `int`. Extreme increments cannot
overflow signed arithmetic before the normal range clamp runs.

The source contract is checked by
`tests/automated/check-container-scroll-arithmetic.py`.

## Container scroll timer input validation

The Container auto-scroll timer now returns safely when its closure is gone or
the reparented scroll-frame ancestor is unavailable during teardown. Normal
auto-scroll processing is unchanged when the hierarchy is intact.

The source contract is checked by
`tests/automated/check-container-scroll-inputs.py`.

## Container scroll navigator validation

The auto-scroll timer now rejects an inconsistent scroll-frame result that
reports navigators without an array, and skips null navigator entries before
trait lookup. Valid navigator lists continue to scroll normally.

The source contract is checked by
`tests/automated/check-container-scroll-navigator-inputs.py`.

## Container scroll range bounds

Container auto-scroll now clamps each navigator axis to its minimum when the
reported range is invalid or the slider already covers the entire range. This
avoids subtracting a larger slider from the maximum and sending a wrapped
position to the navigator.

The source contract is checked by
`tests/automated/check-container-scroll-range-bounds.py`.

## Container select-all input validation

Select-all now returns safely when no Container widget is supplied, before
reading selection state or traversing nodes. Valid selection behavior is
unchanged.

The source contract is checked by
`tests/automated/check-container-select-all-inputs.py`.

## Container select input guards

The Container select action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button activation
and keyboard selection dispatch from malformed programmatic action calls while
leaving normal selection behavior unchanged.

The source contract is checked by
`tests/automated/check-container-select-input-guards.py`.

## Container select-all event guards

The Container select-all and deselect-all actions now return safely when
invoked without an event.  Their primary-selection timestamps and callbacks
require that event; valid keyboard and pointer actions retain their existing
behavior.

The source contract is checked by
`tests/automated/check-container-selection-all-event-guards.py`.

## Container selection input validation

Container selection callbacks now return safely for a missing container, and
selected-item collection skips malformed nodes or missing constraint records
instead of dereferencing them. Valid selection behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-selection-inputs.py`.

## Container selection-range input validation

Selection-range, marquee, and visual-reset helpers now return safely when no
Container widget is supplied. Valid range and marquee selection behavior is
unchanged.

The source contract is checked by
`tests/automated/check-container-selection-range-inputs.py`.

## Container marked-state constraint guards

`SetMarkedCwids` now skips nodes whose Container constraint record is
unavailable while synchronizing selection state.  This protects selection
cleanup during child teardown; valid nodes retain their existing state
transitions.

The source contract is checked by
`tests/automated/check-container-set-marked-constraint-guards.py`.

## Container SetValues child-constraint guards

When a Container changes from outline/detail to spatial layout, its
`ConstraintSetValues` method now skips children whose private Container
constraint record is unavailable.  This protects the transition-time
visibility update from dereferencing `entry_parent` on malformed or partially
initialized children.  Valid child transitions retain their existing behavior.

The source contract is checked by
`tests/automated/check-container-setvalues-constraint-guards.py`.

## Container sizing constraint guards

Closest-layout sizing now skips a node when its Container constraint record is
unavailable instead of reading `user_x` or `user_y` through a null pointer.
Other nodes continue through the normal sizing pass, and valid closest-layout
coordinates are unchanged.

The source contract is checked by
`tests/automated/check-container-size-constraint-guards.py`.

## Container size input validation

Container natural-size calculation now returns safely when its widget or
dimension outputs are absent, and skips malformed child nodes or missing
constraint records. Normal geometry negotiation is unchanged.

The source contract is checked by
`tests/automated/check-container-size-inputs.py`.

## Container small-cell input validation

Small-cell sizing now returns safely for a missing Container widget and falls
back to screen-based defaults when the node list contains no usable child
widget. Valid dynamic sizing remains unchanged.

The source contract is checked by
`tests/automated/check-container-small-cell-inputs.py`.

## Container snap-position bounds

Container snapping and centering derives signed `XPoint` coordinates from cell
counts, cell dimensions, and child extents.  The calculations now use signed
saturating adjustments, so oversized cells cannot wrap a coordinate during
SNAP, NONE, or CENTER placement.  Normal layouts and the public resource/ABI
contract are unchanged.

The focused source contract is checked by
`tests/automated/check-container-snap-position-bounds.py`.

## Container spatial-size adjustment input validation

Spatial-size adjustment now returns safely for missing output pointers, an
invalid item count, or a missing item array when items are requested. Valid
cell-placement calculations are unchanged.

The source contract is checked by
`tests/automated/check-container-spatial-adjustment-inputs.py`.

## Container spatial-growth constraint guards

`RequestSpatialGrowth` now returns `False` when the requested child has no
Container constraint record.  Spatial growth uses the record's logical
coordinates for closest-cell placement; continuing without it could
dereference invalid state while computing the geometry request.  Valid child
growth remains unchanged.

The source contract is checked by
`tests/automated/check-container-spatial-growth-constraint-guards.py`.

## Container spatial layout input validation

Spatial layout now returns safely for a missing Container widget and skips
malformed nodes without dereferencing a missing child widget. Valid placement,
growth, and recursion behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-spatial-layout-inputs.py`.

## Container spatial-size bounds

Spatial Container negotiation combines child positions, child extents, cell
counts, and margins into legacy `Dimension` values.  The calculation now uses
saturating arithmetic for those extents and cell-count products, preventing a
large icon grid or an unusually positioned child from wrapping into a smaller
requested size.  Existing layout policy and public resource types are
unchanged.

The focused source contract is checked by
`tests/automated/check-container-spatial-size-bounds.py`.

## Container selection dispatch input validation

Selection start now returns safely when its widget or event is absent and no
longer dereferences a missing child constraint while handling a selected item.
Valid selection behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-start-select-inputs.py`.

## Container synthetic getter guards

Container’s detail-header, detail-header-count, and outline-column-width
synthetic getters now validate the widget class and output slot before reading
or rebuilding internal state. Invalid callback inputs return deterministic null
or zero values; valid Container reads retain their existing ownership and
allocation behavior.

The focused source-contract check is
`python3 tests/automated/check-container-synthetic-getter-guards.py`.

## Container tab-value arithmetic

Container detail tabs convert the dimension and column index to `float`
before multiplication. This avoids the old `int`-by-`Cardinal` promotion,
which could wrap a large detail count before `XmTabSetValue` received it.

## Container traversal child validation

Next and previous traversal now return safely when the focus child has no
Container constraint or node record. Detached children cannot crash keyboard
focus traversal during teardown or unusual focus transitions.

The source contract is checked by
`tests/automated/check-container-traversal-child-inputs.py`.

## Container traversal input validation

First-node lookup now handles an absent Container or empty node list, and
next-node traversal rejects nodes without widgets or parent Containers before
following layout-specific links. Valid tree traversal is unchanged.

The source contract is checked by
`tests/automated/check-container-traversal-inputs.py`.

## Container tree-helper input validation

Recursive depth reset now skips malformed child nodes or missing constraints.
Descendant checks return false for missing widgets or parent constraint records.
Valid tree depth and descendant behavior are unchanged.

The source contract is checked by
`tests/automated/check-container-tree-helper-inputs.py`.

## Container view and detail input validation

View-type updates now return safely for missing children or parents, icon-width
lookup returns zero for a missing child, and default-detail counting skips
malformed nodes or an absent Container widget. Valid trait and detail counting
behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-view-type-inputs.py`.

## Container widget-class guards

The public `XmContainerGetItemChildren`, `XmContainerRelayout`, and
`XmContainerReorder` helpers now reject `NULL` and non-Container widget
handles before looking up an application context, constraints, or child
storage. This keeps malformed caller input on the documented no-op path
instead of treating an unrelated widget as an `XmContainerWidget` and
dereferencing incompatible private state. Valid Container calls are
unchanged.

The source contract is checked by
`tests/automated/check-container-widget-class-guards.py`.

## Conversion completion inputs

`_XmConvertComplete` now handles a null callback structure by releasing the
returned conversion value and returning safely.  The destination callback
lookup helper likewise returns `NULL` for a missing transfer ID.  These guards
keep cleanup and callback-destruction paths safe without changing ownership
for valid conversions.

The focused regression is
`tests/automated/check-convert-complete-inputs.py`.

## Central conversion-handler inputs

`_XmConvertHandler` is the common entry point for clipboard, selection, and
drag conversion.  It now validates the widget, selection/target atoms, and
all output pointers before obtaining the display or looking up context state.
Direct and reentrant callers therefore receive a safe `False` result instead
of a null-pointer crash.

The focused regression is
`tests/automated/check-convert-handler-inputs.py`.

## Conversion-merge callback input

`XmeConvertMerge` now rejects a null conversion callback structure before
entering the process lock or reading merge state.  Malformed callback paths
therefore return safely without dereferencing incomplete conversion state.

The focused regression is `tests/automated/check-convert-merge-input.py`.

## Conversion callback parameter defaults

`_XmConvertHandler` can run callbacks without an active X selection request,
including local conversion paths.  The callback structure now initializes its
parameter payload to `NULL`, length zero, 8-bit format, and `None` type before
any request-specific overrides.  Convert callbacks therefore see deterministic
empty parameters instead of stack residue.

The regression check is
`tests/automated/check-convert-parameter-defaults.py`.

## Shared conversion result validation

`_XmConvertHandler` is the common boundary used by selection, clipboard, and
drag conversion paths.  Before returning success it now requires an X
selection format of 8, 16, or 32 bits and rejects a nonzero result length with
no value buffer.  Invalid callback results become the same deterministic empty
failure used by other conversion errors, preventing malformed data from
reaching consumers.

The regression check is
`tests/automated/check-convert-result-validation.py`.

## Conversion trait function pointers

The shared conversion handler obtains the widget’s optional transfer trait
before dispatching class-specific conversion.  It now requires both the trait
and its `convertProc` member to be present.  A partially populated trait is
treated as no conversion provider instead of being called through a null
function pointer.

The regression check is `tests/automated/check-convert-trait-proc.py`.

## Resource converter null inputs

Button-type, keysym-table, and charset-table resource converters now reject a
missing source string before scanning or copying it.  This keeps malformed
`XrmValue` input on the normal conversion-failure path instead of crashing
while dereferencing a null address.

## Empty compound-text components

Both compound-text resource encoders now skip the temporary-buffer `memcpy`
when a text component has zero length.  This permits an empty component with no
source pointer without invoking `memcpy` on NULL, while preserving all
non-empty component conversions.

`tests/automated/check-ctext-component-null.py` protects both conversion paths.

## Compound-text zero-length append

The internal compound-text encoder accepts a zero-length append without a
source buffer.  Its concatenation helper now skips `memcpy` for that case,
avoiding an invalid NULL pointer operation while preserving the resulting
terminator and all non-empty appends.

`tests/automated/check-ctext-concat-null.py` protects this contract.

## Compound-text CSI lookahead bounds

CSI decoding examines two parameter bytes after the current byte.  Both
compound-text decoder implementations now verify that at least three bytes
remain before that lookahead, so truncated CSI sequences fail cleanly instead
of reading beyond the input buffer.

`tests/automated/check-ctext-csi-bounds.py` protects both paths.

## Extended compound-text segment bounds

Both extended-segment decoder paths now verify that the encoded segment length
fits between the current octet pointer and the input end before scanning or
advancing it.  Malformed input with a truncated segment therefore fails
cleanly instead of reading beyond the compound-text buffer.

`tests/automated/check-ctext-extended-bounds.py` protects both decoder paths.

## Clipboard format atom-name bounds

Clipboard format atoms are assembled from a fixed Motif prefix and an
application-provided format name. The allocation length is checked before
allocation, and the resulting string is written with bounded formatting.

The source contract is covered by
`tests/automated/check-cutpaste-format-name-bounds.py`.

## DataGrid coordinate bounds

DataGrid keeps its column and row layout calculations in signed `int`
arithmetic, then clamps cell-label coordinates before passing them to Motif's
signed `Position` drawing arguments.  Large grids and row heights therefore
cannot wrap text to an unrelated screen coordinate.

`tests/automated/check-data-grid-coordinate-bounds.py` protects this drawing
contract.

## DataGrid event guards

DataGrid validates its widget type and event pointer before processing
keyboard, pointer, and wheel input. Invalid dispatch is ignored safely while
normal selection and editing interaction remain unchanged.

## DataField action insertion conversion

Keyboard and action-table insertion now preserve the native `size_t` result
from `mbstowcs`, reject conversion failure or lengths too large for Motif's
signed text-position APIs, and only then pass a checked `int` length to the
replacement routine. Key lookup also rejects a result that fills the entire
temporary buffer, preserving room for the required NUL terminator.

Regression coverage:

```text
python3 tests/automated/check-datafield-action-insertion-conversion.py
```

## DataField class binding allocation

`df_ClassPartInitialize` now checks the temporary translation-table buffer
before assembling the class bindings. If allocation fails, class initialization
returns without dereferencing a null buffer; the inherited translation table
remains intact.

The focused source contract is checked by
`tests/automated/check-datafield-class-bindings.py`.

## DataField clear-selection span

The clear-selection action now validates the selected character span before
narrowing it to the legacy `int` count used by replacement and temporary-space
buffers. Oversized spans are rejected without wrapping the count or allocating
an undersized buffer. The wide-character path also rejects spans whose byte
capacity cannot fit Xt's allocation-size type.

Regression coverage: `tests/automated/check-datafield-clear-selection-span.py`.

## DataField wide clear-selection allocation

When clearing a wide-character selection, DataField now checks the temporary
space-buffer allocation before filling it. On failure it restores the
insertion-point drawing state and rejects the operation without dereferencing
or freeing a null buffer.

The focused source contract is checked by
`tests/automated/check-datafield-clear-selection.py`.

## DataField clip-mask X resources

`df_GetClipMask` now checks both the pixmap and fill-GC creation results before
using them. A failed GC creation releases the pixmap, and a failed pixmap
creation returns immediately, avoiding invalid X requests and null-resource
dereferences.

The focused source contract is checked by
`tests/automated/check-datafield-clip-mask.py`.

## DataField context cleanup guards

DataField's X11 context cleanup callback now validates callback-owned context
data and its screen before calling `DisplayOfScreen` or X context APIs. A null
context is ignored; a context without a screen is released safely.

The focused source-contract check is:

```sh
python3 tests/automated/check-datafield-context-cleanup-guards.py
```

## DataField destination context allocation

`df_GetTextFDestData` now checks its per-screen context and destination-data
allocations before initialization or X context registration. If the second
allocation fails, the temporary context record is released and the helper
returns `NULL`; no partially initialized context is retained.

The focused source contract is checked by
`tests/automated/check-datafield-destination-context.py`.
