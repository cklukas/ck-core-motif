---
nav_exclude: true
---
# Archived Bugfix and Hardening Notes (Part 3)

Consolidated archive of standalone bugfix and hardening validation notes.

## Drag top-window property bounds

The multi-screen drag path receives top-window lists from an X property.  It
now validates the property pointers and count before allocating the receiver
array, and only replaces the previous array after allocation succeeds.  Valid
window lists retain their existing tracking behavior; malformed or oversized
lists leave the current drag state unchanged.

## Keep DnD write offsets representable

`_XmWriteDragBuffer` retains its `CARD16` ABI return type but now saturates
offsets above `UINT16_MAX` instead of wrapping them.  The full buffer remains
available to the protocol writer; only the legacy diagnostic/result offset is
bounded.

## Drag atom-table count validation

The `_MOTIF_DRAG_ATOMS` header stores `num_atoms` in a 16-bit field.
`WriteAtomsTable` now rejects an in-memory entry count above `UINT16_MAX`
instead of silently truncating it when publishing the cache.

## Drag atom ordering

The DragBS target-table sort comparator now orders `Atom` values with relational
comparisons instead of subtracting them into an `int`. Large 64-bit atom IDs
therefore cannot overflow the comparator or be mistaken for equal values.

The focused regression is
`tests/automated/check-drag-property-length-bounds.py`.

## Drag atom wire-width validation

`WriteAtomsTable` serializes X11 `Atom` and `Time` values into Motif's
32-bit atom-table record. Values that cannot be represented by that wire
format are now rejected instead of being silently truncated on 64-bit
platforms. The writer also makes the already-bounded allocation and header
field conversions explicit.

The focused regression is
`tests/automated/check-drag-property-length-bounds.py`.

## Drag atom-table allocation handling

`ReadAtomsTable` lazily creates and grows the process-local atom cache.  The
reader now checks both the lazy table allocation and the entry-array
reallocation before parsing entries.  A failed allocation leaves the existing
cache untouched and rejects the update instead of dereferencing or replacing
it with a null pointer.

## Drag atom-table offset validation

The serialized atom-table header stores `heap_offset` in a 32-bit field.
`WriteAtomsTable` now rejects a computed property size above `UINT32_MAX`
before assigning that field, preventing truncation of the serialized layout.

## Drag atom-table property validation

`ReadAtomsTable` reads the `_MOTIF_DRAG_ATOMS` cache property as an 8-bit
serialized record.  It now requires the expected property type and format,
then checks that the advertised atom count fits completely within the bytes
returned by Xlib before indexing any entries.  Malformed cache data is
discarded while the existing table remains usable.

## Drag atom-table serialization guards

`WriteAtomsTable` now rejects a non-empty cache with no entry array and checks
the heap allocation used for large serialized properties.  Failed or
incomplete internal state is reported and skipped before any serialization or
dereference occurs.

## Default drag atom-table allocation handling

`CreateDefaultAtomsTable` creates the initial process-local atom cache.  It
now checks both the table and entry-array allocations, releasing the table if
the entry allocation fails and returning `NULL` instead of dereferencing an
incomplete cache.

## Default drag target-table allocation handling

`CreateDefaultTargetsTable` builds the initial process-local target cache.
It now checks the table, entry-array, and default `XA_STRING` target-list
allocations, releasing earlier allocations and returning `NULL` when memory
is unavailable.  Callers can retain their existing null-table handling
instead of entering the cache with partially initialized pointers.

## Drag target-list count validation

Each `_MOTIF_DRAG_TARGETS` list stores its atom count in a 16-bit wire field.
`WriteTargetsTable` now rejects a list whose in-memory count exceeds that
representation instead of silently truncating it and publishing a cache that
cannot be read consistently.

## Drag target-entry allocation handling

`ReadTargetsTable` allocates an atom array for each newly discovered target
list.  If one allocation fails, the reader now rejects the update and rolls
back entries added during that update, freeing their atom arrays and restoring
the previous entry count.  This avoids writing through a null array and keeps
the existing cache coherent after low-memory failures.

## Drag target-table offset validation

The serialized target-table header stores `heap_offset` in a 32-bit field.
`WriteTargetsTable` now rejects a computed property size above `UINT32_MAX`
before assigning that field, preventing truncation of the serialized layout.

## Drag target wire-width validation

`WriteTargetsTable` serializes target `Atom` values into Motif's 32-bit
target-list records. Values outside that wire representation are now rejected
instead of being silently truncated on 64-bit systems. The count, offset, and
allocation conversions are explicit after their existing bounds checks.

The focused regression is
`tests/automated/check-drag-property-length-bounds.py`.

## Drag target-table property validation

`ReadTargetsTable` reads `_MOTIF_DRAG_TARGETS` as a byte-serialized sequence
of target-list counts followed by 32-bit atoms.  The reader now verifies the
property type and format and walks every advertised list to ensure its count
and atom payload fit in the bytes returned by Xlib.  Truncated or malformed
cache data is rejected before the existing table parser advances through it.

## Drag target-table serialization guards

`WriteTargetsTable` now rejects tables that cannot be represented by the
serialized 16-bit list count, non-empty lists with null atom arrays, and size
arithmetic that would overflow `size_t`.  Large-property allocation is also
checked before the buffer is filled, preventing wrapped sizes and null
dereferences during cache publication.

## DragICC write-buffer growth

Drag-and-drop ICC serialization appends data to either a stack-backed or
heap-backed byte buffer.  The old path added only 1000 bytes, allowed size
wraparound, and overwrote the active pointer before checking allocation
failure.

The writer now checks the required byte count, expands until the whole write
fits within the `Cardinal` capacity contract, and commits the replacement
buffer only after successful allocation.  Failed growth returns zero without
altering the existing buffer.

## Drag ICC initiator metadata bounds

Initiator metadata now rejects a target-table index above the ICC property's
16-bit wire range before serialization. Xt argument counters use `Cardinal`,
and the ICC handle conversion is explicit at the 32-bit property boundary.

Regression: `tests/automated/check-dragicc-initiator-bounds.py`

## Drag initiator-info property validation

`_XmReadInitiatorInfo` reads the byte-serialized
`_MOTIF_DRAG_INITIATOR_INFO` property.  It now requires the expected atom,
8-bit format, a complete X property, and enough bytes for the fixed header
before interpreting the record.

## Drag ICC property request length

`_XmGetDragReceiverInfo` now stores its fixed XGetWindowProperty request
length in `long`, matching Xlib's API. Returned byte counts remain unsigned,
so the change does not alter property validation or the wire format.

Regression: `tests/automated/check-dragicc-property-length.py`

## Drag ICC reason-table conversions

Drag-and-drop ICC reason lookup now iterates the static table with an unsigned
index matching `XtNumber`, and explicitly converts the validated table result
to the unsigned public return type. Invalid message types retain the existing
`_XmNUMBER_DND_CB_REASONS` sentinel.

Regression: `tests/automated/check-dragicc-reason-table.py`

## Drag receiver state allocation handling

After validating receiver-info data, `_XmGetDragReceiverInfo` allocates the
drop-site state wrapper that owns the X property buffer.  It now checks that
allocation before initialization and releases the property buffer on
failure, returning `False` with no partially initialized receiver state.

## Drag ICC receiver count bounds

Receiver drop-site counts now preserve the signed result from the drop-site
manager, reject a negative result, and narrow to the 16-bit ICC property only
after the existing upper-bound check. This prevents an error return or large
local count from becoming a wrapped protocol value.

Regression: `tests/automated/check-dragicc-receiver-count.py`

## Drag receiver-info property validation

`_XmGetDragReceiverInfo` reads the byte-serialized
`_MOTIF_DRAG_RECEIVER_INFO` property and exposes data and heap views to the
drop-site parser.  It now requires the expected type, 8-bit format, and a
complete X property, then checks that `heap_offset` starts after the header
and ends within the returned bytes.  Invalid properties are rejected before
any pointer arithmetic or parsing.

## Drag ICC receiver resource widths

Receiver drag-site resources now enter their 32-bit ICC fields explicitly.
Pixmap-depth serialization rejects values above the protocol's 16-bit range
before writing the property, preventing silent truncation while preserving the
existing property layout.

Regression: `tests/automated/check-dragicc-resource-widths.py`

## Drag ICC 16-bit byte swapping

The ICC `swap2bytes` macro now swaps the two object-representation bytes with
`memcpy`, preserving the established wire transformation for both signed
`INT16` coordinates and unsigned `CARD16` fields. It no longer assigns an
unsigned bit-field temporary back through a type-dependent conversion.

Regression: `tests/automated/check-dragicc-swap16.py`

## Drag ICC wire widths

Callback timestamps, windows, and ICC handles are serialized into Motif's
32-bit client-message fields. The conversion is now explicit at every callback
serialization site, preserving the established ICC wire format while making
the protocol boundary visible to strict compilers and reviewers.

Regression: `tests/automated/check-dragicc-wire-widths.py`

## Drag-over colormap bounds

The drag-over shell's colormap-window allocation remains bounded to Xt's
`Cardinal` width. Its teardown loop uses a distinct saved-colormap index,
avoiding shadowing of the active window-list index and making the two cleanup
passes unambiguous.

The focused regression is
`tests/automated/check-dragover-colormap-bounds.py`.

## Drag-over cursor allocation

Drag-over cursor creation now rejects an X cursor-creation failure and returns
`None` so the existing drag-mode fallback can run.  When caching is enabled,
the cache record allocation is checked before initialization; if it fails, the
new cursor is released and the same fallback is used instead of dereferencing
a null cache record.

The focused source-level regression is
`tests/automated/check-dragover-cursor-allocation.py`.

## Drag-under animation allocation

Drag-under animation now checks its save-state allocation before initialization,
and `AnimateEnter` stops cleanly when that state cannot be created. The caller
still records the null state for the matching leave path, while no animation
GC, region, or window fields are dereferenced from a null record.

The focused source contract is checked by
`tests/automated/check-dragunder-animation-allocation.py`.

## DragUnder argument-count width

`CreateAnimationSaveData` now tracks its `XtSetArg`/`XmDropSiteRetrieve`
argument count as `Cardinal`, matching Xt's API and avoiding a signed-to-
unsigned conversion at the retrieval call.

Regression: `tests/automated/check-dragunder-arg-count.py`

## Drag-under shared drawing context

`CreateAnimationSaveData` now treats failure to create its shared drawing GC as
initialization failure. It releases the save record and returns `NULL`, which
lets the drag callback abort before any animation path can issue requests with
an invalid context.

The focused source contract is checked by
`tests/automated/check-dragunder-draw-gc.py`.

## DragUnder region geometry bounds

DragUnder now skips single-region highlight and shadow drawing when the region
is no larger than twice its border offset.  Width and height subtraction is
performed after signed bounds checks, preventing unsigned wraparound from
creating oversized X draw requests for collapsed drop sites.

The focused regression is `tests/automated/check-dragunder-geometry-bounds.py`.

## DragUnder geometry narrowing

Drag-under highlight and shadow regions use X11 `Position`, a signed 16-bit
coordinate type.  Region origins are now offset through a range-checked helper
before narrowing, preventing wraparound when a drop-site region is near the
coordinate limit.  Pixmap strip heights use explicit `Dimension` narrowing
after the existing thickness checks.

Regression: `tests/automated/check-dragunder-geometry.py`

## Drag-under highlight graphics context

`DrawHighlight` now checks the graphics context returned by `XCreateGC` before
applying a clip region or drawing. If the server cannot create the context,
highlight rendering is skipped safely instead of issuing requests through a
null GC.

The focused source contract is checked by
`tests/automated/check-dragunder-highlight-gc.py`.

## Drag-under composite masks

`DrawPixmap` now validates the temporary composite mask and its graphics
context before using either resource. If allocation fails, the temporary mask
is released and drawing falls back to the ordinary clip region, avoiding X
requests against an invalid drawable while preserving the animation.

The focused source contract is checked by
`tests/automated/check-dragunder-mask.py`.

## DragUnder pixmap dimensions

The internal saved-pixmap record now stores width and height as Motif
`Dimension`, matching the non-wide `_XmAllocScratchPixmap` interface and the
source `SaveAll`/`SaveSegments` parameters. This removes avoidable narrowing
while preserving the existing internal layout and rendering behavior.

Regression: `tests/automated/check-dragunder-pixmap-dimensions.py`

## DragUnder pixmap metadata guards

DragUnder now initializes metadata depth before configuring highlight, top
shadow, and bottom shadow GCs.  Tile or stipple state is added only for a
positive resolved depth; an unresolved pixmap leaves the solid color GC
configuration intact.

The focused regression is
`tests/automated/check-dragunder-pixmap-metadata.py`.

## Drag-under backing-store cleanup

`SaveAll` and `SaveSegments` now reset `numSavedPixmaps` when their backing
store record allocation fails. Later animation cleanup therefore skips the
missing array instead of treating a null pointer as a saved-pixmap list.

The focused source contract is checked by
`tests/automated/check-dragunder-save-cleanup.py`.

## Drag-under scratch pixmap allocation

`SaveAll` now validates the scratch pixmap before issuing `XCopyArea`. A
failed or unspecified pixmap releases the saved-record allocation and resets
the cleanup count, so animation code reports failure without sending an X
request to an invalid drawable.

The focused source contract is checked by
`tests/automated/check-dragunder-save-pixmap.py`.

## Drag-under segment backing stores

`SaveSegments` now validates every scratch pixmap before copying window pixels
into it. If any strip cannot be allocated, already-created strips are released,
the saved-pixmap list is cleared, and the caller reports that no highlight or
shadow was drawn. This keeps partial backing stores from leaking and prevents
`XCopyArea` from receiving an invalid drawable.

The focused source contract is checked by
`tests/automated/check-dragunder-save-segments.py`.

## Drag-under shadow graphics contexts

`DrawShadow` now handles both graphics-context allocation failures. A missing
top context aborts before configuration; a missing bottom context releases the
top context and aborts. `FreeAnimationData` also checks optional contexts before
freeing them, so partial setup remains safe.

The focused source contract is checked by
`tests/automated/check-dragunder-shadow-gc.py`.

## DrArrow geometry-array growth

`XmeDrawArrow` keeps three rectangle arrays for the arrow’s top, center, and
bottom sections.  The old code resized them independently and updated the
shared capacity even if one allocation failed.  Later geometry writes could
then use a null or shorter array.

Growth now allocates all three arrays, copies the previous contents, and
commits the replacement set only when every allocation succeeds.  A failed
growth leaves the previous geometry buffers and capacity intact.

## Arrow drawing GC guards

`XmeDrawArrow` treats the center GC as optional for flat arrows, but shadowed
arrows require both top and bottom GCs. The old implementation only checked
the drawable and passed missing shadow GCs to `XFillRectangles`, which can
produce an Xlib error during partial widget initialization or teardown.

The helper now returns before allocating or drawing when a shadowed arrow lacks
either required GC. Flat arrows retain their existing optional-center behavior;
the public API and normal geometry are unchanged.

## Arrow drawing size guards

`XmeDrawArrow` derives its internal raster size by subtracting two from the
requested width or height. Those parameters are public Motif `Dimension`
values, so widths or heights of one or two used to underflow before the later
positive-size check. The result could cause oversized temporary allocations
and unnecessary drawing work for a collapsed widget.

The helper now returns before that subtraction when either dimension is below
three pixels. Normal arrow geometry and the public API remain unchanged. The
regression `tests/automated/check-drarrow-size-guards.py` keeps the early guard
in place.

## XmeDrawHighlight bounds

`XmeDrawHighlight` now clamps the requested highlight thickness to the
provided drawable dimensions and computes right/bottom coordinates in signed
arithmetic.  Oversized highlights therefore remain inside the drawable
instead of wrapping rectangle coordinates.

The source-level regression is `tests/automated/check-draw-highlight-bounds.py`.

## Highlight coordinate bounds

`XmeDrawHighlight` stores four rectangles using X11 signed 16-bit origins.
The right and bottom origins are derived from the widget origin and its
dimensions; direct casts could wrap for extreme geometry.

The derived origins now use the shared bounded coordinate conversion. Normal
highlight rendering is unchanged, while oversized geometry cannot wrap to an
unrelated location.

The focused source regression is
`tests/automated/check-draw-highlight-coordinate-bounds.py`.

## XmeDrawHighlight graphics-context guard

`XmeDrawHighlight` now returns when its graphics context is missing.  Widgets
being initialized or torn down therefore avoid invalid Xlib fill calls while
retaining the existing highlight behavior for valid resources.

The source-level regression is `tests/automated/check-draw-highlight-gc-guards.py`.

## Draw shadow segment-buffer growth

The Motif shadow drawing helper keeps a shared `XSegment` array sized for the
current border thickness.  The old path assigned `XtRealloc`'s result and
updated the capacity before checking for failure.

The growth path now checks the segment count, allocates a replacement, and
updates the shared pointer and capacity only after success.  A failed growth
returns without touching the existing drawing buffer.

## Separator coordinate bounds

`XmeDrawSeparator` derives line centers and endpoints from widget origins,
dimensions, and margins before storing them in X11 `short` coordinates.
Direct narrowing could wrap oversized geometry and place a separator at an
unrelated coordinate.

The center and all single- and double-line endpoints now use the shared bounded
coordinate conversion. Normal separators retain their existing appearance;
extreme geometry remains within the X11 coordinate range.

The focused source regression is
`tests/automated/check-draw-separator-coordinate-bounds.py`.

## Separator drawing guards

`XmeDrawSeparator` is shared by menus, lists, scrollbars, and other widgets.
Its simple-line paths previously subtracted an unsigned margin from the
requested span before checking whether any line could fit. Shadowed paths also
passed missing top or bottom GCs into Xlib.

The helper now computes the line span in signed arithmetic, skips collapsed
spans, and checks the required separator or shadow GC before drawing. Valid
separator geometry and the public API are unchanged. The regression
`tests/automated/check-draw-separator-guards.py` covers these guards.

## Shared separator shadow span bounds

The shared `XmeDrawSeparator` implementation now computes its horizontal or
vertical shadow span in signed arithmetic and returns when the requested
margin consumes the span.  Callers with collapsed or unusually inset
separators therefore avoid wrapped dash lengths and coordinates.

The source-level regression is
`tests/automated/check-draw-separator-span-bounds.py`.

## Shared shadow coordinate bounds

The shared shadow renderer stores endpoints in X11 `short` coordinates. Its
endpoint calculations combine widget origins, dimensions, and shadow offsets;
direct casts could wrap an oversized result into an unrelated screen location.

`DrawSimpleShadow` now clamps every computed endpoint to the representable X11
coordinate range before populating `XSegment`. Ordinary widget geometry is
unchanged, while extreme geometry remains bounded and deterministic.

The focused source regression is
`tests/automated/check-draw-shadow-coordinate-bounds.py`.

## XmeDrawShadows graphics-context guards

`XmeDrawShadows` now returns when either shadow graphics context is missing.
Partially initialized or teardown-time widgets therefore avoid issuing Xlib
draw calls with an invalid GC, while normal shadow rendering is unchanged.

The source-level regression is `tests/automated/check-draw-shadow-gc-guards.py`.

## Shared shadow origins

`DrawSimpleShadow` builds the segment geometry used by `XmeDrawShadows` and
separator shadows. Its old expressions combined signed `Position` origins with
unsigned `Dimension` sizes directly. A negative or off-edge origin could wrap
before the value was assigned to an XSegment coordinate.

The helper now performs origin and size arithmetic in signed `int` locals and
casts only when storing the final XSegment coordinates. Shadow thickness
clamping and the public drawing API remain unchanged. The regression
`tests/automated/check-draw-shadow-origins.py` covers the conversion boundary.

## Shared etched shadow span bounds

`XmeDrawShadows` now computes the inner etched-shadow rectangle in signed
arithmetic and skips that second shadow when the outer rectangle is too small.
Tiny widgets therefore avoid wrapped `Dimension` spans while retaining the
existing two-pass etched rendering for normal sizes.

The source-level regression is `tests/automated/check-draw-shadow-span-bounds.py`.

## DrawnButton menu bounds

DrawnButton menu activation and unhighlight callbacks can run while a widget
has a very small allocated size.  Their border-clearing rectangles now require
positive interior dimensions and convert the unsigned core dimensions before
subtraction.  The menu arm shadow path also checks both shadow GCs before
calling the X11 shadow helper.

Normal menu feedback keeps the existing colors, timing, and shadow geometry.
Only undersized or partially initialized widgets skip the affected drawing
operation.  The focused regression is
`tests/automated/check-drawn-button-menu-bounds.py`.

## DrawUtils bevel-buffer growth

`XmDrawBevel` uses a static rectangle buffer for small bevels and a dynamic
buffer for larger ones.  The previous growth path updated its capacity before
checking `XtRealloc`, so allocation failure could leave a null buffer that
the drawing loop immediately dereferenced.

The dynamic path now checks the byte count against both host `size_t` and Xt
`Cardinal` limits, allocates a replacement, and commits the pointer and
capacity only after success. The existing buffer is retained when growth
cannot be completed.

## Drop callback cleanup

Drop completion cleanup now clears `destination_data` immediately after freeing
it. Repeated completion or cancellation callbacks therefore become harmless
instead of double-freeing the same drop allocation.

Regression coverage: `tests/automated/check-drop-callback-cleanup.py`.

## DnD callback teardown inputs

Drop callbacks can run while a drag is being cancelled or its owner is being
destroyed.  `DropDestinationHandler` now validates its widget and callback
structure before obtaining the display, and `DeleteDropCBStruct` validates
the transfer ID and callback structure before freeing destination data.
Malformed teardown callbacks therefore return safely without null
dereferences or invalid frees.

The focused regression is `tests/automated/check-drop-callback-inputs.py`.

## Drop callback widget lifetime

Drop selection callbacks arrive with the drag context as their callback
widget.  Motif then rebinds the callback to the destination widget stored in
the transfer context before dispatching destination callbacks and completing
the transfer.  During destruction or cancellation that stored widget can be
cleared first.  `SelectionCallbackWrapper` now returns before using the
rebinding when `tc->widget` is null, releasing any Xt-owned result first and
preventing invalid `XtClass`/display access during external-owner teardown.
The other incomplete-state exits release the same result buffer as well.

The regression check is `tests/automated/check-drop-callback-widget.py`.

## Drop-context validation

Destination setup copies the drop callback structure before reading the drag
context's ICC handle.  It now rejects a missing `dragContext` together with a
missing callback structure, so cancellation or malformed external drop events
cannot reach `XtGetValues` or later drop-transfer calls with a null widget.

The focused regression is `tests/automated/check-drop-context-input.py`.

## DropHighlight coordinate bounds

DropHighlight clamps its centered label coordinates before passing them to
Motif's signed `Position` drawing arguments.  Large highlight dimensions or
font metrics therefore cannot wrap the label to an unrelated screen
coordinate.

`tests/automated/check-drop-highlight-coordinate-bounds.py` protects this
drawing contract.

## DropHighlight rendering graphics context

`XmDropHighlight` creates a short-lived graphics context for each expose
operation. The callback now returns when that allocation fails, before any
rectangle or label drawing request is sent to X. This avoids a `BadGC` while
preserving normal highlight rendering when resources are available.

The focused source contract is checked by
`tests/automated/check-drop-highlight-render-gc.py`.

## Drop-site setup allocation

`XmeDropSink` copies caller arguments before registering the drop site. It now
validates the added argument slots, rejects a missing input array when entries
were requested, and checks the temporary `Arg` allocation before copying.

Failed setup returns without registering a partial drop site. Successful
registrations keep the original argument order and callback behavior.

## Drop-transfer start failure

`XmTransferValue` now treats a failed `XmDropTransferStart` as failed request
setup. The newly appended transfer block is removed, the transfer status is
set to failure, and an already-exited transfer is finalized immediately.
When the request is made from an active destination callback, the caller
finishes the context after that callback returns. `outstanding`/`count` are
incremented only after a drop-transfer object exists, so a request with no
possible callback cannot wedge the transfer lifecycle.

The public `XmDropTransferAdd` ABI remains `void`; toolkit transfer code uses
an internal checked form that observes whether the transfer-list count grew.
If an append allocation fails, the newly created request block is rolled back
and the transfer follows the same failure teardown path.

Regression coverage: `tests/automated/check-drop-start-failure.py`.
Append-failure coverage: `tests/automated/check-drop-transfer-add-failure.py`.

## Drop-transfer callback input validation

Drop-transfer completion, requestor-destruction, and drag-destruction
callbacks now reject a missing transfer object before touching transfer state.
Normal teardown and cancellation behavior is unchanged; malformed or late
callback delivery fails closed instead of dereferencing null client data.

The focused source-contract check is
`tests/automated/check-drop-transfer-drag-destroy.py`.

## drop-transfer-drag-destroy-cleanup
Always clean up after drag-context destruction
===============================================

`DragContextDestroyCB` now clears pending timers and selection requests even
after the delayed-start timer has fired.  The old `CR1146` early return used a
process-wide timer flag and could skip cleanup when that flag had already been
cleared, leaving requestor callbacks and transfer state registered during drag
teardown.

The focused regression is:

```text
python3 tests/automated/check-drop-transfer-drag-destroy.py
```

## Drop-transfer name bounds

Drop-transfer objects receive internal names derived from a process-local
counter. The counter is unsigned and the name is written with `snprintf` into
the fixed widget-name buffer, avoiding signed overflow and unbounded writes.

The source contract is covered by
`tests/automated/check-drop-transfer-name-bounds.py`.

## Drop-transfer payload validation

`DropTransferSelectionCB` now fails a transfer before invoking application
code when Xt reports a nonzero payload length with a `NULL` value pointer.
It also rejects nonempty payloads with a format other than X11's 8, 16, or
32-bit formats, or with no target type atom.
Zero-length callbacks remain valid, including incremental-transfer completion;
well-formed payloads retain their existing callback and progression behavior.

The focused source-contract check is
`tests/automated/check-drop-transfer-callback-selection.py`.

## drop-transfer-rejected-value-cleanup
Release rejected drop-transfer values
=====================================

Malformed drop-transfer selection callbacks now release any non-null Xt-owned
selection buffer before failing the transfer.  This covers missing callback
state, typeless/null-payload/invalid-format results, and exhausted transfer
lists, including zero-length results that still carry an allocated pointer.
Invalid format widths are rejected even for empty results. Values delivered to
the application callback retain the existing ownership contract.

The focused regression is:

```text
python3 tests/automated/check-drop-transfer-rejected-value-cleanup.py
```

## Drop-transfer size-limit header

The drop-transfer table growth guard uses `SIZE_MAX` to reject unrepresentable
allocation sizes. `DropTrans.c` now includes `<stdint.h>`, the standard header
that defines the macro, so this guard compiles portably alongside the existing
allocation checks.

The focused regression is
`tests/automated/check-drop-transfer-size-limit-header.py`.

## drop-transfer-source-notification-guard
Guard source-notification teardown reentrancy
==============================================

The source-notification callback now returns safely when its transfer object is
already marked for destruction.  Any Xt-owned result buffer is still released,
but the callback no longer clears state or schedules a second widget destroy
during deferred teardown.

The focused regression is:

```text
python3 tests/automated/check-drop-transfer-source-notified.py
```

## drop-transfer-timer-guard
Guard delayed drop-transfer timer state
=======================================

The delayed drop-transfer timer now returns safely when Xt invokes it without
its transfer object client data.  This keeps timer teardown consistent with
the other drop-transfer callbacks and prevents a null dereference before the
drag-context validity check.

The focused regression is:

```text
python3 tests/automated/check-drop-transfer-timer-input.py
```

## DropDown callback lifecycle

DropDown interaction callbacks now reject incomplete callback data and missing
internal children before reading popup, text, or list state.  Arrow posting,
unposting, text verification, and custom verification hooks therefore become
safe no-ops after partial construction.

Fully constructed DropDowns retain the existing popup, verification, and
callback behavior.

## DropDown child allocation

`XmDropDown` creates label, text, and arrow children independently. Creation
now checks each child before resource updates, callback registration, and
translation installation. Geometry queries return zero size for a missing
child, and placement configures only children that exist. Normal DropDown
behavior and public child accessors are unchanged.

The contract is protected by
`tests/automated/check-dropdown-child-allocation.py`.

## DropDown child geometry

DropDown child placement subtracts vertical spacing, shadow, borders, and
label/arrow widths from unsigned dimensions. During a collapsed resize those
subtractions could wrap, producing oversized child heights or unrelated
horizontal positions.

Placement now uses signed intermediates, clamps the available child height, and
saturates each child span after its border inset. Normal layouts retain their
existing geometry while transiently tiny controls remain bounded.

The calculated child origins now remain signed until passed to the legacy X11
configure boundary, where they are explicitly clamped. The width split also
clamps its wide remainder before it is stored in a legacy `Dimension`, so a
constrained control cannot wrap a child to an unrelated location or size.

Geometry negotiation also distinguishes a refused parent height from a valid
width compromise. A refusal restores speculative child dimensions and returns
`XtGeometryNo`; it never reads an uninitialized reply or sends its fields to X.
Child preferred-size queries honor `request_mode`, using allocated geometry for
fields omitted by the child.

The native Linux/Xvfb regression is
`tests/automated/run-dropdown-negotiation.py`; it poisons partial query replies,
denies height and combined resize requests, and exercises FontSelector theme,
density, and scale transitions against the installed library.

Geometry negotiation also distinguishes a refused parent height from a valid
width compromise. A refusal restores speculative child dimensions and returns
`XtGeometryNo`; it never reads an uninitialized reply or sends its fields to X.
Child preferred-size queries honor `request_mode`, using allocated geometry for
fields omitted by the child.

The source contract is protected by
`tests/automated/check-dropdown-negotiation-contract.py`. A native Linux/Xvfb
runner can be added when an installed prefix is available for ABI-level
qualification.


Geometry negotiation also distinguishes a refused parent height from a valid
width compromise. A refusal restores speculative child dimensions and returns
`XtGeometryNo`; it never reads an uninitialized reply or sends its fields to X.
Child preferred-size queries honor `request_mode`, using the allocated size and
border for fields omitted by the child.

`tests/automated/run-dropdown-negotiation.py --prefix PREFIX --out OUTPUT`
checks poisoned partial query replies and 24 denied height/combined requests,
including query-only calls, across classic/light/dark at 1x and 1.5x. It also
recreates FontSelector twelve times while exercising 48 theme/density/scale
transitions with allocator memory poisoning. FontSelector embeds DropDown and
exposed the invalid ConfigureWindow request in a native lifecycle probe.

## DropDown child getters

The public `XmDropDownGetLabel`, `XmDropDownGetArrow`, `XmDropDownGetText`, and
`XmDropDownGetList` helpers return the control's actual internal child widgets.
They first reject a null or non-DropDown widget and return `NULL` when the
requested child has not been created.

The list getter uses the stored child pointer rather than looking up a child by
the default resource name.  Applications may therefore rename the list child
without making the public getter lose it.  Existing fully constructed controls
keep the same child identity and behavior.

## DropDown child lifecycle

DropDown keeps its list, text field, arrow, and optional label as internal
children.  Initialization, `GetValues`, and later `SetValues` operations now
check those handles before registering handlers, forwarding resources,
changing visual metrics, or managing labels.

This preserves normal behavior for a complete DropDown while making resource
updates safe after allocation pressure leaves one of the optional children
unavailable.

## DropDown conversion lifecycle

Popup placement and text/list conversion now validate the required internal
children before querying or updating them.  A partially constructed DropDown
returns conversion failure or an empty value safely; popup placement no longer
reads missing text or arrow geometry.

Fully constructed controls retain their existing selection, text conversion,
and popup behavior.

## DropDown conversion-size bounds

`XmDropDown` passes the text-field contents to the list converter through an
`XrmValue`. The conversion-size field is Xt/Xrm `Cardinal`-sized, while
`strlen` returns `size_t`. The conversion now rejects text whose byte length
plus its terminating byte cannot be represented, preventing truncation on
wide-size platforms and releasing the retrieved text before returning.

The focused regression check is
`tests/automated/check-dropdown-conversion-size-bounds.py`.

## DropDown grabbed-event guards

`XmDropDown` receives button events while its popup grab is active. A grab may
surface an event for a foreign window or a window destroyed during dispatch.
The handler now treats an unmapped widget and an incomplete parent chain as a
no-op, preserving normal popup behavior without a null dereference.
The list-selection callback applies the same rule to missing callback payloads
or widget closures.

## DropDown expose geometry

The DropDown visual-style expose handler positions its shadow from a child
origin minus child border and manager shadow thickness.  Those origins are
signed `Position`s, but the handler stored the subtraction in an unsigned
`Dimension`; a child temporarily positioned near or before zero could wrap the
shadow origin.

The handler now performs the subtraction in signed arithmetic and stores the
result as a `Position`, preserving normal shadows while keeping collapsed or
transient layouts bounded.

## DropDown frame drawing bounds

The modern-style DropDown frame now calculates its child extents in a wide
intermediate type. Its drawing origin and dimensions are clamped explicitly
when they cross Motif's legacy `Position` and `Dimension` interfaces, so a
large child or margin cannot wrap the frame around the control.

The focused regression is `tests/automated/check-dropdown-frame-bounds.py`.

## DropDown geometry and action lifecycle

DropDown preferred-size calculation now supplies zero geometry for a missing
optional label, text, or arrow child instead of reading a null `child` pointer.
Cancel actions, combo-box lookup, and shell-handler registration likewise
reject missing widgets before walking parents or restoring text.

Complete DropDowns retain their existing sizing and popup behavior.

## DropDown secondary interaction lifecycle

DropDown’s text-modification, value-change, list-selection, shell-event, and
focus-loss callbacks now reject incomplete callback data and missing list/text
children before reading fields or dispatching actions.  This keeps keyboard,
focus, and popup interactions safe after partial construction or destruction.

Normal interactions on a complete DropDown retain their existing traversal,
selection, and popup behavior.

## DropDown popup width geometry

`XmDropDown` computes the popup width when the list is opened.  The combo-box
width and popup offset use different signedness, so subtracting a large offset
from the `Dimension` directly could wrap to a near-maximum unsigned width.
The popup now performs the subtraction in a signed temporary and clamps the
minimum width to one pixel.  Normal offsets retain their existing width; an
offset that reaches past the combo box produces a small usable popup instead
of a giant geometry request.

## DropDown popup lifecycle

DropDown now checks popup-shell creation before constructing its scrolled list,
and checks list creation before registering callbacks or scroll-bar handlers.
Partial construction is destroyed cleanly when either allocation fails.

The popup shell also clears the stored shell, list, and scrollbar pointers when
destroyed externally, so later popup operations cannot reuse stale widgets.

## DropDown popup position bounds

DropDown popup placement now keeps translated coordinates, popup offsets, and
screen-fit arithmetic in `int`/wide expressions. The requested popup width and
final shell coordinates are explicitly clamped at Motif's legacy `Dimension`
and `Position` boundaries, preventing wrapped popups on large/high-DPI
desktops.

The focused regression is
`tests/automated/check-dropdown-popup-position-bounds.py`.

## DropDown selected-item tables

DropDown now checks the converted selected-item count against host `size_t`
and Xt's `Cardinal` byte limits before allocating its temporary `XmStringTable`.
The checked byte count is passed explicitly at the Xt allocation boundary.
An unrepresentable count reports the existing
selection error instead of wrapping the allocation size; normal text-to-list
selection behavior is unchanged.

Selected-item text insertion now obtains the exact locale character count
without copying through a fixed `BUFSIZ` wide-character buffer, so long
localized entries keep correct insertion positions.

The converted selection-table allocation is checked before entries are copied;
allocation failure uses the existing selection error cleanup path instead of
dereferencing a null table. The source regression is
`tests/automated/check-dropdown-selection-allocation.py`.

For multiple or extended selection, the resulting comma-separated field text
is now allocated to its exact byte count and assembled with linear copies.
This avoids repeatedly rescanning the growing result while retaining the same
text, separator, conversion-failure cleanup, and allocation limits.

## Drop-site record allocation guards

Drop-site manager bookkeeping creates regions and private records for shell
placeholders, clipping nodes, and registered drop sites.  Those allocations
are now checked before the records are initialized.  Failed region creation
or record allocation releases any partial region and stops the update cleanly;
failed shell and clipper creation no longer reaches registration or child-link
macros with a null record.  Existing drop-site behavior is unchanged when the
allocations succeed.

## DropSite descendant traversal

DropSite descendant discovery now uses a `Cardinal` index for composite child
counts. This matches Xt's count field and avoids signed-index wraparound while
searching large widget trees for drag-and-drop targets.

The focused source regression is
`tests/automated/check-dropsite-child-count-bounds.py`.

## Drop-site stacking-order input safety

`XmDropSiteQueryStackingOrder` now rejects a null widget or output pointer
before entering the application context. Its child traversal uses `Cardinal`
indices throughout, including a decrement-before-access reverse loop, so
large child counts cannot be narrowed to `int` or underflow the reverse
iteration. Existing stacking order and output ownership are unchanged.

The companion configure/get-visuals APIs now reject null widgets before app
context lookup, use full-width indices for reordering, and return cleanly if
the active-visuals record cannot be allocated.

The focused regression check is
`tests/automated/check-dropsite-stacking-order-input.py`.

## DropSMgr pixmap depth storage

Drop-site animation pixmap depth is stored as a `Cardinal`, while
`XmeGetPixmapData` writes through an `int *`.  DropSMgr now queries into a
local `int` and assigns the result to the resource field afterward.  This
avoids type-punning through the field address and keeps the public structure
layout unchanged.

The focused regression is `tests/automated/check-dropsmgr-pixmap-depth.py`.

## Toggle drawing GC guards

The shared diamond and circle drawing helpers can be reached while a widget is
partially initialized or being destroyed. Their old paths passed missing
graphics contexts into Xlib: diamonds could dereference a null top or bottom
GC, while circles unconditionally queried both shadow GCs when a shadow was
requested.

`DrTog.c` now returns safely when a required GC is absent. A diamond still
allows its existing small-width top-only cases, and circles retain their
optional center GC behavior. The public drawing APIs and normal geometry are
unchanged. `tests/automated/check-drtog-gc-guards.py` covers the guards.

## Toggle indicator bounds

The shared `XmeDrawIndicator` implementation draws checkmarks and crosses
inside a widget rectangle. Its old scaling and endpoint arithmetic subtracted
unsigned margins from unsigned dimensions before checking whether any drawing
area remained. A very small indicator could therefore underflow to a large
scale or wrapped segment coordinates.

`DrTog.c` now performs the fit calculation in signed `int` locals and returns
without drawing when the margin consumes the available area. This preserves
the public API and normal indicator geometry while making undersized gadgets
safe. `tests/automated/check-drtog-indicator-bounds.py` covers both paths.

## Indicator drawing GC guard

`XmeDrawIndicator` dispatches checkmark and cross rendering through a shared
public helper. A partially initialized widget could reach it with a missing GC,
which would pass the null handle into Xlib.

The helper now returns before dispatch when either the drawable or indicator GC
is absent. Valid indicator rendering and the public API are unchanged. The
regression `tests/automated/check-drtog-indicator-gc-guard.py` covers the
boundary guard.

## Indicator drawing size guard

On wide-prototype builds, `XmeDrawIndicator` receives signed integer width and
height parameters but forwards them to internal `Dimension` parameters. A
negative value could therefore become a very large unsigned dimension before
the checkmark or cross renderer saw it.

The public helper now rejects non-positive dimensions before dispatch. Valid
indicator geometry and the Motif API remain unchanged. The regression
`tests/automated/check-drtog-indicator-size-guard.py` covers the boundary.

## DT window-management request cleanup

`GetDtWmRequest` keeps the X property buffer between calls while it extracts
the NUL-separated requests.  If the property is absent, the X request fails,
or the returned format is not 8-bit, the buffer must still be released before
the static state is reset.  The invalid-property path now frees that buffer,
preventing a leak each time a malformed request is received.

## EditRes fallback formatting

The edit-resources protocol converts widget resources to strings. When a
resource converter fails, the integer fallback appends the resource type to a
caller-supplied buffer. Resource type names are application-defined and can be
long.

`GetResourceString` receives the capacity of its buffer, formats numeric
values with `snprintf` and appends the type within the remaining space. The
value is read into storage of the size the resource declares, so the
conversion sees the bytes of the right width on any byte order.

`tests/automated/check-editres-fallback-bounds.py` guards the interface and
formatting contracts.

## Edit-resources widget-list allocation

The hierarchy of a widget written to a reply is checked against the 16-bit
wire field, uses a small stack array for ordinary depths and marks the reply
failed instead of writing a partial path when memory is short. Successful
protocol serialization is unchanged.

The focused regression is
`tests/automated/check-editres-widget-list-allocation.py`.

## EmptyState callback guards

EmptyState validates the action callback closure before reading private widget
state. Foreign or stale callback data is ignored safely while normal action
invocation remains unchanged.

## EmptyState child allocation

EmptyState creates optional icon, message, and action children.  Initialization,
layout, and `SetValues` now guard each child before querying geometry, changing
resources, registering callbacks, or configuring it.  A failed internal child
creation therefore cannot turn a later redraw or resource update into a null
widget call.

The contract is protected by
`tests/automated/check-empty-state-child-failure-guards.py`.

## EmptyState layout bounds

EmptyState accumulates vertical layout positions in a wide type and bounds
them when converting to X11 `Position` coordinates. Large child sizes or
margins can no longer wrap later controls to an incorrect location.

## EmptyState text updates

EmptyState now copies replacement message and action strings before releasing
the current values. If either allocation fails, the existing visible string
remains owned and in use, and the widget avoids reporting a successful text
change for that resource.

The focused source contract is checked by
`tests/automated/check-empty-state-setvalues-transaction.py`.

## Encoding-registry target length

The encoding-registry helper reports a validated non-negative `int` length,
while selection callbacks store lengths as `unsigned long`. The standard
converter now widens that value explicitly, preserving the checked contract
without an implicit sign conversion.

The focused regression is
`tests/automated/check-encoding-registry-length.py`.

## Icon-box virtual-space expansion bounds

Virtual-space expansion now validates its icon-box state, dimensions, and
placement increments before division.  It also rejects products that cannot
fit in the placement-count integer, avoiding divide-by-zero and arithmetic
wraparound during resize and packing.

`tests/automated/check-expand-space-bounds.py` protects these resize bounds.

## ExtObject allocation

`XmExtObject` initialization now checks the `XmWidgetExtData` allocation
before registering or filling the record. If allocation fails, initialization
returns without dereferencing a null extension record; successful extension
objects retain the existing initialization order and behavior.

## Mwm feedback text bounds

The move/resize feedback overlay formats coordinates and dimensions into
internal text buffers. Those buffers now have room for full-width signed
coordinates and unsigned dimensions, and both updates use `snprintf`. The
size path also uses unsigned conversion specifiers matching its API types, so
large 32-bit dimensions are represented correctly without overwriting adjacent
state.

`tests/automated/check-feedback-format-bounds.py` guards the buffer and format
contract.

## File-selection directory cleanup

File-selection directory enumeration can reject an oversized allocation or a
failed `XmStringGenerate` conversion after entering its shared cleanup path.
The directory-list pointer now starts as `NULL`, so cleanup safely handles
those failures without reading an uninitialized stack value. Successful
directory listing behavior is unchanged.

The focused regression is
`tests/automated/check-file-selection-dir-cleanup.py`.

## FileSelectionBox child-count bounds

FileSelectionBox geometry setup now traverses its composite child array with
an Xt `Cardinal` index, including the reverse traversal used for right-to-left
button layout. Existing widget ordering and geometry behavior are unchanged.

## FileSelectionBox conversion callback guards

FileSelectionBox drag/selection conversion now rejects missing callback data,
missing client widgets, and client widgets of the wrong class before reading
search procedures or the display. Unsupported dispatch reports conversion
refusal without touching partial state.

## FileSelectionBox programmatic search allocation

`XmFileSelectionDoSearch` now checks copied and generated search strings before
measuring them. Allocation failure leaves the matching length at zero and
still follows the existing search cleanup path, avoiding a null
`XmStringLength` call for programmatic searches.

The focused source contract is checked by
`tests/automated/check-filesb-do-search-allocation.py`.

## FileSelectionBox programmatic search widget validation

`XmFileSelectionDoSearch` now verifies that its widget argument is a
FileSelectionBox subclass before entering the application lock or reading
private dialog state. Null and wrong-class callers return safely; valid
programmatic searches are unchanged.

The focused source contract is checked by
`tests/automated/check-filesb-do-search-class.py`.

## FileSelectionBox child lookup widget validation

`XmFileSelectionBoxGetChild` now verifies that its widget argument is a
FileSelectionBox subclass before entering the application lock or reading
private child fields. Null and wrong-class callers return `NULL`; valid child
lookups are unchanged.

The focused source contract is checked by
`tests/automated/check-filesb-get-child-class.py`.

## FileSelectionBox getter class guards

FileSelectionBox’s synthetic directory, pattern, label, list, count, and mask
getters now validate the widget class before accessing private state. Invalid
callback inputs keep their existing null/zero return behavior; valid
FileSelectionBox reads and child-resource ownership remain unchanged.

The focused source-contract check is
`python3 tests/automated/check-filesb-getter-class-guards.py`.

## FileSelectionBox initialization-copy rollback

`XmFileSelectionBox` now checks its initial copies of the no-match, directory,
and pattern strings. Failed copies retain the original resource instead of
publishing NULL or measuring it. A failed directory-mask copy falls back to
the standard `*` mask when possible, and string lengths are only measured for
non-NULL copies.

The focused regression is
`tests/automated/check-filesb-init-copy-rollback.py`.

## FileSelectionBox list callback input

The FileSelectionBox list callback now ignores a missing callback or client
payload before accessing the selected item. It also leaves callback lengths at
zero when mask or directory compound-string construction fails, avoiding null
`XmStringLength` calls while preserving normal directory and file selection.

The focused source contract is checked by
`tests/automated/check-filesb-list-callback-input.py`.

## FileSelectionBox no-match string rollback

`XmFileSelectionBox` now copies a replacement `XmNnoMatchString` before
freeing the current widget-owned value.  If the copy cannot be completed, the
existing message remains installed rather than leaving the dialog with a null
or dangling message string.

The focused regression is
`tests/automated/check-filesb-no-match-string-rollback.py`.

## FileSelectionBox push-button callback input

`FileSelectionPB` now rejects missing callback widgets, callback data, and
parent FileSelectionBox widgets before accessing dialog state. It also keeps
mask and directory lengths at zero if compound-string generation fails.
Normal Apply/OK/Cancel button behavior is unchanged.

The focused source contract is checked by
`tests/automated/check-filesb-pushbutton-callback-input.py`.

## FileSelectionBox push-button qualifier guard

The FileSelectionBox push-button path now checks its qualify-search procedure
before invoking it. If the procedure is absent, temporary mask and directory
strings are released and the action returns safely instead of dereferencing a
null callback slot or using an uninitialized qualified record.

The focused source contract is checked by
`tests/automated/check-filesb-pushbutton-qualify-guard.py`.

## FileSelectionBox search qualification input

`QualifySearchDataProc` now rejects missing input or output callback records
before accessing their fields. It also leaves the corresponding length at
zero when compound-string generation fails, rather than measuring a null
`XmString`. The documented nullable search fields and normal qualification
behavior remain unchanged.

The focused source contract is checked by
`tests/automated/check-filesb-qualify-input.py`.

## FileSelectionBox search-list cleanup

File and directory searches now check result-table sizes against both host
`size_t` and Xt's `Cardinal` range.  A failed `XmStringGenerate` releases all
already-generated entries and the native filename list before returning, so a
partial list is never passed to `XmList`.  The normal file-search path also
clears its native list pointer after freeing it, preventing the shared exit
cleanup from freeing the same array twice.

The focused regression is
`tests/automated/check-filesb-search-list-cleanup.py`.

## FileSelectionBox search procedure guards

`FileSelectionBoxUpdate` now checks the qualify and directory-search procedure
slots before invoking them. The optional file-search procedure is invoked only
when present. This prevents partially initialized or customized dialogs from
calling through null procedure pointers while preserving normal search flow.

The focused source contract is checked by
`tests/automated/check-filesb-search-procedure-guards.py`.

## FileSelectionBox search-resource rollback

`XmFileSelectionBox::SetValues` now checks copies of the directory, pattern,
and filter-mask strings before reading their lengths or scheduling a search.
For in-progress searches, replacement copies are installed before the old
values are released.  A failed copy keeps the previous search resource and
does not start a search with incomplete input.

The focused regression is
`tests/automated/check-filesb-search-resource-rollback.py`.

## FileSelectionBox synthetic getter guards

Directory, pattern, no-match, label, list, count, and directory-mask exports
now validate callback outputs and required children before reading state. Missing
children return null or zero outputs instead of dereferencing partial dialog
state, preserving normal Unicode compound-string exports.

## FileSelectionBox update input

`FileSelectionBoxUpdate` now rejects missing dialog state or search data before
accessing list widgets and search callbacks. Normal directory and file search
updates are unchanged; malformed internal or embedder calls return safely.

The focused source contract is checked by
`tests/automated/check-filesb-update-input.py`.

## FileSelectionBox update-resource rollback

`FileSelectionBoxUpdate` now prepares replacement copies of the Directory and
Pattern resources before releasing the current values.  If an `XmStringCopy`
allocation fails, the current search resources remain intact and the existing
values continue to drive filter-text refresh and subsequent searches.  An
explicit NULL value still clears the corresponding resource.

The focused regression is
`tests/automated/check-filesb-update-resource-rollback.py`.

## FileSelectionBox navigation action arguments

The legacy `_XmFileSelectionBoxUpOrDown` translation action accepts command
arguments `0` (up), `1` (down), `2` (home), and `3` (end). The argument is now
parsed as an exact bounded decimal value. Missing, malformed, overflowing, or
out-of-range values are ignored instead of being converted with unchecked
`atoi` arithmetic.

This keeps valid translation tables compatible while making malformed resource
or application-provided action arguments harmless.

## Alternate icon-place bounds

The alternate-position search in `FindIconPlace` now validates each converted
place before reading the placement list.  Invalid negative or oversized
coordinates are skipped rather than becoming an out-of-bounds access.

`tests/automated/check-find-place-alternates.py` protects the lookup contract.

## Icon-place search guards

`FindIconPlace` now rejects missing clients, placement storage, and invalid
cell dimensions before searching or extending the icon grid.  Root-window
position conversion also returns `(0, 0)` when coordinate arrays are absent.

`tests/automated/check-find-place-guards.py` protects these paths.

## Icon-place sentinel handling

`FindIconPlace` now rejects the `NO_ICON_PLACE` sentinel returned by coordinate
conversion before reading the placement array or attempting to grow it.  This
keeps negative or otherwise invalid coordinates from becoming a negative array
index and avoids changing the grid for an invalid request.

`tests/automated/check-find-place-sentinel.py` protects this contract.

## Final transfer teardown owns drop cleanup

Drop transfers allocate a private copy of their callback data. The usual done
callback releases that copy, but callback-list growth can fail under memory
pressure. `FinishTransfer` now performs the same idempotent release after all
callbacks, so the transfer context remains the authoritative owner and cannot
leak the drop data when callback registration fails.

Regression coverage: `tests/automated/check-finish-transfer-drop-cleanup.py`.

## Flyout event guards

Flyout validates its callback closure and event pointer before processing
owner or popup events. Invalid teardown-time dispatch is ignored safely while
normal dismissal behavior remains unchanged.

## Flyout visibility API inputs

The public Flyout show, hide, and visibility APIs now reject a null widget
before invoking the Xt subclass check. Invalid optional handles return the
existing no-op or false result; valid popup, positioning, and dismissal
callback behavior is unchanged.

The focused regression check is `tests/automated/check-flyout-inputs.py`.

## Flyout owner teardown

Flyout may receive an owner-destroy callback while its popup or label is also
being torn down. Owner callbacks, popup destruction, and label resource updates
now check widget validity and `being_destroyed` state before touching those
objects.

The contract is protected by
`tests/automated/check-flyout-owner-teardown.py`.

## Flyout popup destruction

Flyout popup shells register a destroy callback.  If application code or a
parent shell destroys the transient shell, the Flyout clears its shell and
label pointers and marks itself hidden.

The next `XmFlyoutShow` therefore creates a fresh popup instead of attempting
to reuse freed widget addresses.

## Flyout popup safety

Flyout popup creation now checks shell, label-string, and label-widget
creation, destroying partial state on failure. Updating the popup label also
retains the existing label if a replacement `XmString` cannot be allocated.
Oversized popups are anchored at zero on screens too small to contain them.

`tests/automated/check-flyout-popup-safety.py` protects these paths.

## Icon-box focus callback guards

`ChangeActiveIconboxIconText` now returns for a missing icon widget or focus
event before reading the widget window or event type.  Valid focus transitions
remain unchanged.

`tests/automated/check-focus-callback-guards.py` protects the contract.

## Focus callback context guard

`ChangeActiveIconboxIconText` now checks that the window-context lookup
returned a client record before evaluating `P_ICON_BOX`.  Valid focus handling
is unchanged.

`tests/automated/check-focus-context-guard.py` protects this contract.

## Focus-shell query null-widget guard

`XmeFocusIsInShell` now validates its widget before finding the top-level
shell. A null query therefore returns `False` without entering traversal or
application-context code, while valid focus checks retain their existing
behavior.

The focused regression is
`tests/automated/check-focus-shell-input.py`.

## Font fallback name size bounds

The scaled-font fallback in `lib/Xm/FontS.c` constructs an XLFD by joining
two pieces of the requested font name with `-140`.  The resulting allocation
must fit both `size_t` arithmetic and Xt's `Cardinal` allocation size.  The
fallback now checks those bounds before allocating and formatting the name;
an unrepresentable name follows the existing unavailable-font error path
instead of wrapping an allocation size.

`tests/automated/check-font-fallback-size-bounds.py` keeps the checked size
calculation and matching `snprintf` capacity coupled in source review.

## FontSelector family-table growth

`XmFontSelector` grows its internal family table while processing the server's
XLFD inventory.  Growth now checks signed-capacity arithmetic and both host
`size_t` and Xt `Cardinal` byte limits before changing the capacity or calling
`XtRealloc`.  If the next table cannot be represented, processing stops with
the already-built table intact.

The focused regression is `tests/automated/check-fonts-family-growth.py`.

## Font-list input-copy allocation

Font-list conversion parses a mutable copy of the resource string. The normal
converter and default-render-table fallback now check that copy before passing
it to the parser. Allocation failure returns safely while preserving existing
font-list ownership and successful parsing behavior.

## Font selector XLFD fallback bounds

The font selector has a compatibility fallback for scaled XLFD fonts whose
reported metrics have zero height. Malformed or unusually long XLFD names used
to be copied into fixed `BUFSIZ` arrays and combined with `sprintf`, allowing a
font name to corrupt the selector process.

The fallback now validates the separator before use, allocates the derived
parts from their actual lengths, and builds the replacement name with bounded
`snprintf`. User-facing error messages are also bounded; the existing font
selection and fallback behavior is preserved.

`tests/automated/check-font-selector-bounds.py` guards these source-level
contracts.

## FontSelector family-encoding growth

Each discovered font family keeps a dynamically grown table of encoding
quarks. The table grows in `NUM_BUTTONS` entries when a family is updated.

Growth now checks signed and byte-size limits, allocates through a temporary
pointer, and commits the new pointer and capacity only after success. A failed
family update leaves all previously recorded encodings intact.

## Font-selector formatting bounds

Font selector resource fields and encoding-menu button names are written into
fixed local buffers. They now use snprintf, keeping formatting bounded even
when unusual font metrics or a large encoding index are supplied.

The source contract is covered by
tests/automated/check-font-selector-format-bounds.py.

## FontSelector value-hook input

`GetValuesHook` now validates the widget class and the `ArgList`/count inputs
before reading private FontSelector state. Malformed hook calls return safely;
normal resource queries and current-font packaging are unchanged.

The focused source contract is checked by
`tests/automated/check-font-selector-get-values-input.py`.

## FontSelector value-hook resource names

`GetValuesHook` now skips requests with a null resource name as well as null
output pointers. This prevents malformed `Arg` records from reaching string
comparison while leaving valid resource lookup unchanged.

The focused source contract is checked by
`tests/automated/check-font-selector-get-values-names.py`.

## FontSelector value-hook output pointers

`GetValuesHook` now skips a requested resource whose output pointer is null
before dispatching resource-specific packaging. This protects every supported
FontSelector value branch, while valid `Arg` outputs retain their existing
behavior.

The focused source contract is checked by
`tests/automated/check-font-selector-get-values-pointers.py`.

## FontSelector list growth

FontSelector's XLFD and alternate-font lists grow through temporary Xt
allocations. Their capacity checks now cover both host `size_t` and Xt
`Cardinal` byte limits, so an unrepresentable growth request leaves the
existing list unchanged.

## FontSelector scaled-font allocation

The scaled-font fallback in `XmFontSelector` now checks each temporary string
allocation before copying or formatting through it. Allocation failure reports
the existing unavailable-font error path instead of dereferencing a null
temporary buffer; successful font loading is unchanged.

## FontSelector SetValues input

`SetValues` now validates all widget records and the argument list/count before
filtering child resources or comparing private state. Malformed calls return
`False` without changing widget state; normal Xt resource updates retain their
existing behavior.

The focused source contract is checked by
`tests/automated/check-font-selector-set-values-input.py`.

## FontSelector SetValues resource names

`SetValues` now preflights every requested resource name before passing the
argument list to `_XmFilterArgs`. A nameless `Arg` returns `False` without
entering child-resource filtering; valid resource updates are unchanged.

The focused source contract is checked by
`tests/automated/check-font-selector-set-values-names.py`.

## FontSelector widget-list growth

FontSelector keeps the widgets that are visible or sensitive in XLFD mode in
two internal arrays. Their count and capacity fields are legacy `char` values,
so growth must remain within the representable signed range.

Both list builders now reject malformed or unrepresentable counters, check the
byte count before calling Xt, and publish the replacement pointer and capacity
only after allocation succeeds. A failed growth leaves the existing list and
its counters unchanged.

## FontSelector numeric fields

`XmFontSelector` reads the numeric fields in an XLFD name into signed
`short` members.  The parser now uses a checked `strtol` conversion for point
size and both resolutions.  Empty fields and the XLFD `*` wildcard continue to
map to zero; malformed, out-of-range, or overflowing values also map to zero
instead of wrapping into a different font.

The size drop-down uses the same validation before converting a displayed
point size to Motif's tenths-of-a-point representation.  Values that cannot be
represented in the existing `short` field are treated as invalid, preserving
the public resource and widget ABI while preventing undefined selection and
font-list behavior.

`tests/automated/check-font-size-parsing.py` checks the conversion contract
and guards against reintroducing the unchecked `atoi` paths.

## Font-list converter input validation

The string-to-font-list converter now returns `False` when its source or
destination `XrmValue` is missing, avoiding invalid dereferences. Valid font
list parsing, fallback loading, and conversion warnings remain unchanged.

The focused regression check is
`tests/automated/check-fontlist-converter-inputs.py`.

## Font family encoding allocation handling

Font selector family creation now checks the initial encoding-table allocation.
If it fails, the family keeps an empty encoding table and returns cleanly;
encoding enumeration and matching already treat an empty table as unavailable,
so later font discovery cannot dereference null storage.

The regression check is `tests/automated/check-fonts-family-encoding-allocation.py`.

## FontS list-size bounds

The font selector grows its XLFD and miscellaneous font arrays in fixed
increments.  Their signed counters were checked for increment overflow, but
the resulting slot count was not checked before multiplication by the element
size.

Both growth helpers now reject byte counts that cannot be represented by
`size_t`, preserving the existing arrays and list counters.

## FontSelector bold and italic allocation

The bold/italic control builder now checks argument merging, its button box,
and both toggle buttons before adding callbacks or storing child pointers.
Partial construction returns cleanly and releases merged arguments.

## FontSelector bold and italic refresh guards

The FontSelector bold/italic refresh now validates its selector state, current
font, family storage, and both toggle children before reading or updating
them. A partially initialized or partially destroyed selector therefore
returns safely instead of dereferencing incomplete widget state.

## FontSelector font-string construction guards

`BuildFontString` now rejects incomplete selector/font/buffer inputs and
converts missing family, weight, or encoding quarks to the existing XLFD
wildcard. Valid font selections keep the same output format, while malformed
state no longer reaches a null `%s` argument or an invalid buffer write.

## FontSelector encoding callback guards

Encoding callbacks now tolerate stale widget ancestry, incomplete font state,
missing encoding lists, and out-of-range menu indices. Valid menu selections
retain the existing encoding replacement and font-refresh behavior.

## FontSelector mode-transition guards

`ChangeMode` now rejects incomplete selector/font state, tolerates absent
sensitivity-list entries, and skips missing mode toggles. XLFD transitions also
stop safely if the temporary “any” string cannot be created. Fully constructed
selectors retain the existing mode-switch behavior.

## FontSelector encoding matching guards

Encoding matching now handles missing selector/family state and an unset
encoding safely. It checks the family encoding capacity before indexing, so
malformed or truncated encoding arrays cannot be read out of bounds.

## FontSelector child allocation

FontSelector now checks merged argument-list allocation, its button-box child,
and its scrolled text child before managing or registering callbacks. Partial
child construction returns cleanly without passing a null widget into Xt.

## FontSelector font-choice allocation

The XLFD/other-font choice builder now checks argument merging, its button box,
and both toggle buttons before callback registration and child storage. Partial
construction releases merged arguments and returns safely.

## FontSelector destroy lifecycle

FontSelector teardown now handles a widget whose font information failed to
initialize, avoiding a null dereference. Encoding-list cleanup also walks its
own pointer with an explicit null guard, making partial-initialization cleanup
deterministic.

## FontSelector encoding-menu allocation

Encoding-menu rebuilds now reject a missing option-menu child before applying
resources or managing it. Individual encoding labels are also checked before
being passed to push-button creation, so a failed compound-string allocation
cannot publish an invalid label or enter callback setup.

## FontSelector encoding rollback

FontSelector now preserves the current encoding when a replacement string
cannot be allocated, both during SetValues migration and when a user selects a
new encoding. Successful changes retain the existing family refresh and menu
history behavior.

The focused source regression is
`tests/automated/check-fontselector-encoding-rollback.py`.

## FontSelector family allocation

The family DropDown builder now checks argument-list merging and child
creation before registering its verification and update callbacks. Temporary
multibyte text and merged arguments are released on every failure path.

## FontSelector family-change callback guards

The family-change callback now validates its selector, source widget, font
state, and dropdown value. The no-change path releases the retrieved value
before returning, avoiding a repeated-selection leak.

## FontSelector fixed/proportional refresh guards

The spacing refresh now validates the selector, font state, and family table
before resolving family data or changing spacing flags. Missing optional
spacing toggles remain supported, while a partially initialized selector
returns safely.

## FontSelector font-string construction

FontSelector builds XLFD strings into caller-provided buffers. The builder now
honors the supplied buffer size and always leaves a terminated string, so long
font family or encoding names are truncated safely instead of overflowing the
widget-owned buffer. Existing callers continue to use the same XLFD format.

The installed-library widget regression constructs and realizes a
`XmFontSelector` through the normal public API.

## FontSelector current-font retrieval

The `XmNcurrentFont` get-values hook now handles partial initialization. If
font information, the current font record, or the caller's output pointer is
unavailable, the query returns without dereferencing incomplete widget state.
The XLFD output buffer is required only when XLFD mode is active; family-mode
queries remain available without it.

## FontSelector label initialization rollback

FontSelector now uses a checked copy helper for its built-in compound-string
labels.  A failed copy falls back to an empty compound string rather than
publishing a NULL label to its child controls and later queries.

The focused regression is
`tests/automated/check-fontselector-label-init-rollback.py`.

## FontSelector list-refresh rollback

`XmFontSelector` now checks every temporary family and size string generated
during list refresh.  If a copy or localized-string allocation fails, all
partial entries are released and the child list is not updated with a NULL
entry.  Successful refreshes retain the existing filtering and selection
behavior.

The focused regression is
`tests/automated/check-fontselector-list-refresh-rollback.py`.

## FontSelector encoding-menu allocation

The encoding option-menu builder now checks merged argument-list allocation and
each popup/pulldown/button creation before registering callbacks or applying
resources.  A partial new menu is destroyed without disturbing the existing
option menu, and the usable option-menu widget is returned.

## FontSelector middle-pane allocation

FontSelector now checks merged arguments and both middle-area paned containers
before constructing dependent family, size, spacing, and choice controls.
Allocation failure returns cleanly without using a null parent widget.

## FontSelector name-label allocation

The optional font-name label is now checked immediately after creation. A
failed label allocation releases the merged argument list and returns before
management or publication of the invalid child pointer.

## FontSelector secondary-choice allocation

The secondary-choice controls now validate their merged arguments, button box,
and scaling/name toggles before callbacks or child pointers are published.
The optional encoding menu is added to the sensitivity list only when its
construction succeeds, so a partial FontSelector remains safe to use.

## FontSelector Other-list guards

The Other-font list builder now validates selector/font/list state and the
family child before use. It handles a missing current-font string safely and
only publishes successfully allocated compound-string items.

## FontSelector Other-mode callback guards

The Other-mode callback now rejects a missing selector or toggle callback
record before touching sibling toggles or changing modes. Valid selections
retain the existing mode transition behavior.

## FontSelector text-error callback guard

The text error-removal callback now returns safely when invoked without a
FontSelector instance, before reading the current-text state.

## FontSelector resolution allocation

The resolution control builder now checks its button box, merged arguments,
and each resolution toggle before registering callbacks or storing child
pointers. If a merge or child creation fails, the temporary argument list is
released and construction returns without dereferencing an invalid widget.

## FontSelector SetValues guards

Resource updates now tolerate partial child construction. Toggle state and
text-row updates require their target widgets, encoding-menu rebuilds require
an existing option menu, and label refreshes use a shared null-safe helper.
Normal, fully constructed FontSelector updates retain their existing behavior.

## FontSelector size allocation

The size DropDown builder now checks argument-list merging and child creation
before adding the control to the XLFD-only list or registering callbacks.
Temporary text and merged arguments are released on failure.

## FontSelector size-change callback guards

The size-change callback now validates its selector, source widget, current
font, and dropdown value before parsing or updating the point size. Valid
values retain the existing range checks and display refresh behavior.

## FontSelector slant-copy bounds

Bold/italic refresh now copies the selected slant through the existing bounded
XLFD field helper. This preserves the fixed `SLANT_LEN` storage contract,
always terminates the field, and treats an unset family quark as an empty
slant instead of dereferencing a null string.

## FontSelector spacing allocation

The spacing control builder now checks its button box, merged arguments, and
all three spacing toggles before registering callbacks or storing child
pointers. Partial construction releases merged arguments and returns safely.

## FontSelector spacing-field bounds

The fixed-width spacing fields in `FontData` and `FamilyInfo` now use the
existing bounded `StoreString` helper for every assignment. This preserves
the XLFD spacing values while preventing malformed or future-longer values
from overrunning the three-character storage.

## FontSelector string rollback

FontSelector now copies each changed label string before releasing its current
value. Allocation failure preserves the old selector label and skips the
corresponding widget refresh; NULL remains an explicit clear operation.

The focused source regression is
`tests/automated/check-fontselector-string-rollback.py`.

## FontSelector 100-DPI callback guards

The 100-DPI callback now validates callback data and current font state before
touching sibling toggles or resolution fields. Valid selections retain the
existing family, size, and display refresh behavior.

## FontSelector 75-DPI callback guards

The 75-DPI callback now validates callback data and current font state before
touching sibling toggles or resolution fields. Valid selections retain the
existing family, size, and display refresh behavior.

## FontSelector bold callback guards

The bold callback now rejects missing toggle data and incomplete font/family
state before changing weight flags or rebuilding the displayed font. Valid
callbacks retain the existing bold/medium selection behavior.

## FontSelector both-DPI callback guards

The both-DPI callback now validates callback data and current font state before
touching sibling toggles or resolution fields. Valid selections retain the
existing “any resolution” family, size, and display refresh behavior.

## FontSelector any-spacing callback guards

The any-spacing callback now validates callback data and current font state
before touching sibling toggles or refreshing the font. Valid selections retain
the existing fixed/proportional state and display update behavior.

## FontSelector fixed-spacing callback guards

The fixed-spacing callback now validates callback data and current font state
before touching sibling toggles or refreshing the font. Valid selections retain
the existing fixed/proportional state and display update behavior.

## FontSelector italic callback guards

The italic callback now rejects missing toggle data and incomplete font/family
state. It also handles missing quark strings and bounds the slant copy to the
FontSelector field before rebuilding the displayed font.

## FontSelector middle-pane callback guards

The middle-pane toggle now rejects missing callback data, an invalid selector,
or a pane that was not constructed before calling the manage/unmanage APIs.
Valid toggles retain their existing visibility behavior.

## FontSelector name-window callback guards

The name-window callback now tolerates missing callback data, label/family
children, and incomplete font state. Display refreshes occur only when the
needed value is available; valid toggles retain their visibility behavior.

## FontSelector proportional callback guards

The proportional-spacing callback now validates callback data and current font
state before touching sibling toggles or refreshing the font. Valid selections
retain the existing family, size, and display update behavior.

## FontSelector scaling callback guards

The scaling callback now rejects missing callback data, incomplete font state,
and an unavailable family table. Resetting the size control is conditional on
the size widget existing, while valid callbacks retain the normal update and
font-refresh behavior.

## FontSelector top-pane allocation

FontSelector now checks argument-list merging, the top paned container, and the
option toggle button before constructing dependent controls or registering its
callback.  A failed allocation returns cleanly without using a null parent or
button.

## FontSelector family refresh guards

Family-list refreshes now validate selector/font/family state, reject negative
or missing family storage, and stop safely when the temporary “any” value cannot
be converted. Valid refreshes retain their existing filtering and selection
behavior.

## FontSelector size refresh guards

Size-list refreshes now validate selector/font/family state and the size child
before use. Failed temporary “any” conversion is handled with complete
compound-string cleanup, while valid refreshes retain their filtering and
selection behavior.

## FontSelector XLFD-mode callback guards

The XLFD-mode callback now rejects a missing selector or toggle callback record
before touching sibling toggles or changing modes. Valid selections retain the
existing mode transition behavior.

## Form child-count bounds

`XmForm` stores its child count as an Xt `Cardinal`. Form layout, geometry
queries, managed-child updates, and dependency sorting now use `Cardinal`
indices when traversing the composite child array. Attachment-type loops
remain signed because they iterate the fixed attachment enum, not the child
array.

This keeps traversal types aligned with the Xt count while retaining the
existing layout and dependency-ordering behavior.

## Form measured-size geometry

Form layout measures a child against the remaining width or height through
`Dimension *fwidth` and `Dimension *fheight`. Several attachment paths
subtracted offsets before converting the result to `int`; a small form could
therefore wrap the measured remainder and request an incorrect attachment.

All measured-size subtraction now converts the dimensions before arithmetic,
preserving normal attachment behavior while making narrow forms deterministic.
Measured width and height growth now saturates as well, so repeated attachment
corrections cannot wrap a Dimension after a valid intermediate calculation.

## Form fallback offset geometry

When Form computes a child attachment without a temporary layout width or
height, it subtracts the attachment offset from the Form’s widget dimension.
Those fallback calculations now convert the dimension to `int` before
subtraction, preserving negative out-of-bounds positions instead of wrapping
them through unsigned arithmetic.

## Form offset getter guards

Form's synthetic top, bottom, left, right, and margin exports now validate the
child/widget class and output storage before reading attachment or margin
state. Invalid calls return deterministic zero values; normal offset and
margin conversion is unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-form-offset-getter-guards.py
```

## FormSection child allocation

FormSection creates header, content, and footer children during initialization.
Geometry and resource updates now tolerate an individual child failing to
allocate, including the optional footer mapping path.

The contract is protected by
`tests/automated/check-form-section-child-failure-guards.py`.

## Form-section margin comparison

`XmFormSection` now treats the available inner width as zero when margins are
larger than the natural width. Footer sizing therefore expands the section when
needed instead of relying on an unsigned subtraction that could wrap around.

The focused regression check is
`tests/automated/check-form-section-margin-bounds.py`.

## FormSection position bounds

`XmFormSection` positions its header, content, and optional footer from
margin, spacing, and child dimensions. Directly narrowing those sums to
Motif's signed `Position` type could wrap a large layout.

The layout now clamps each origin to the representable `Position` range.
Normal sections retain their existing geometry, while extreme resource or
child sizes cannot place a child at an unrelated coordinate.

The focused source regression is
`tests/automated/check-form-section-position-bounds.py`.

## Frame dimension bounds

Frame sizing now saturates width, height, title extent, and title spacing at
the X11 Dimension limit. Title and shadow origins also remain representable
Positions. Large child borders, margins, shadows, or preferred sizes therefore
cannot wrap a Frame into unexpectedly small geometry.

The focused regression check is
tests/automated/check-frame-dimension-bounds.py.

## Frame child position bounds

Frame geometry computes child positions in `int` and then stores them in the
X11 `Position` type.  A wide frame or a large title/border combination can
produce a value outside the signed 16-bit range; a direct cast wraps the child
to the opposite side of the frame.

Child X positions and work-area Y origins now pass through a saturating helper
before conversion.  Ordinary layouts are unchanged, while extreme transient
geometry clamps to the representable X11 range.  The contract is protected by
`tests/automated/check-frame-position-bounds.py`.

## Frame title placement bounds

Frame title placement now computes beginning, center, and end horizontal
coordinates in signed arithmetic before converting to the X toolkit
`Position`.  A title wider than a collapsed frame therefore remains a valid
off-edge position instead of wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-frame-title-placement.py`.

## FreeIcon teardown guards

`FreeIcon` can run while a client is being withdrawn or while workspace
membership is being torn down.  The cleanup routine now returns when the
client or screen state is unavailable, and checks that the workspace list has
at least one entry before examining its first icon-box pointer.  This avoids a
null dereference and an out-of-bounds read while retaining the existing icon
window and placement cleanup for fully initialized clients.

## Gadget access-colors guards

The Gadget access-colors trait now validates the gadget, output structure, and
manager parent before reading manager color state. Invalid trait calls return
without dereferencing malformed state; valid gadgets receive the same color
mask and values as before.

The focused source-contract check is
`python3 tests/automated/check-gadget-access-colors-guards.py`.

## Gadget color getter guards

Gadget’s synthetic highlight, top-shadow, and bottom-shadow color getters now
validate the gadget, output slot, parent, and manager class before reading
manager state. Invalid callback inputs return zero deterministically; valid
gadgets retain the inherited manager-color behavior.

The focused source-contract check is
`python3 tests/automated/check-gadget-color-getter-guards.py`.

## Gadget and DrawingArea child-count bounds

Gadget redisplay and DrawingArea hit testing now use Xt `Cardinal` indices for
composite child arrays. DrawingArea's reverse hit-test loop explicitly checks
the count before decrementing, so an empty child list cannot underflow while
retaining topmost-child hit-test ordering.

## Gadget secondary-argument merging

The generic Gadget secondary-object hook merges the caller's arguments with
`XmNlogicalParent` before creating the secondary object.  The merged list is
now rejects the only count that would overflow after adding that internal
argument, and checks the result before passing it to Xt.  If Xt cannot allocate
the merged list, the hook returns without dereferencing a null argument list;
normal successful argument merging and ownership are unchanged.

The regression check is `tests/automated/check-gadget-secondary-args.py`.

## Gadget tooltip hook guards

Gadget's tooltip synthetic import/export hooks now validate widget and value
storage before calling tooltip APIs. Invalid exports return a deterministic
null string; invalid imports return `XmSYNTHETIC_NONE` without changing state.

The focused source-contract check is:

```sh
python3 tests/automated/check-gadget-tooltip-hook-guards.py
```

## Gadget unit and direction trait guards

Gadget’s unit-type and layout-direction trait getters now validate the widget
class before reading instance state. Invalid inputs return the established
safe defaults (`XmPIXELS` and `XmDEFAULT_DIRECTION`); valid gadget subclasses
retain their configured values.

The unit-type fallback is explicitly braced.  This makes the invalid-widget
path unambiguous to maintainers and to strict compiler warning configurations:
the instance dereference is visibly outside the fallback branch, rather than
being misleadingly indented beneath it.  There is no public API or valid-widget
behavior change.

The focused source-contract check is
`python3 tests/automated/check-gadget-trait-getter-guards.py`.

## Gallery view-tab outlines

The component gallery's **Example**, **Source**, and **Built-in Icons** views
are all view tabs, not an unlabeled stack. Each tab receives a one-pixel
`#aeb7c2` border plus matching Motif shadow colors. The border remains visible
when a tab is inactive (ToggleButton shadows are state-dependent), while the
selected tab keeps Motif's selected fill. This makes the available views clear in the light
appearance without relying on an X window border that can disappear under a
theme or X server.

`tests/automated/check-gallery-tab-outlines.py` protects the three-tab styling
contract.

## GC cache insertion failure

`GetGCForPutImage` now checks insertion of a newly created GC into the GC
cache. If bucket allocation fails, it unlocks the process cache, frees the X11
GC and its pending entry, and returns failure so the caller can roll back the
pixmap build.

The focused regression is `tests/automated/check-xm-gc-insertion-failure.py`.

## Uncached image GC ownership

When the image GC cache cannot be initialized or a cache entry cannot be
allocated, `GetGCForPutImage` may still create a temporary GC. It now reports
that ownership to its caller, and the scaled-pixmap path releases the GC after
the image transfer. Cached GCs retain their existing lifecycle and are freed
with the cache.

The focused regression is `tests/automated/check-xm-gc-ownership.py`.

## Geometry-area clearing input validation

`_XmGeoClearRectObjAreas` now returns safely when the rectangle object, old
geometry record, or parent widget is missing. Valid expose-area clearing is
unchanged.

The focused regression check is
`tests/automated/check-geo-clear-areas-input.py`.

## Managed-child counting input validation

`_XmGeoCount_kids` now returns zero for a missing composite or child array and
skips null child slots. Valid managed-child counts remain unchanged, while
partially initialized composite state no longer crashes geometry setup.

The focused regression check is
`tests/automated/check-geo-count-kids-input.py`.

## Geometry comparison input validation

`_XmGeometryEqual` now returns `False` when the widget or either geometry
record is missing. Valid geometry comparisons retain their existing behavior,
while incomplete requests no longer reach request-mask macros with null
pointers.

The focused regression check is
`tests/automated/check-geo-equal-inputs.py`.

## Child geometry-list allocation

`_XmGetKidGeo` now validates the parent pointer, child-count arithmetic, and
`XtMalloc` result before populating the geometry list. Allocation failure or
an unrepresentable list size returns `NULL` instead of dereferencing partial
state. The checked byte count is passed explicitly as an Xt `Cardinal`.

The focused regression check is
`tests/automated/check-geo-kid-allocation.py`.

## Geometry value-loading input validation

`_XmGeoLoadValues` now returns safely when the widget or output record is
missing, or when an instigator has no request record. Valid preferred and
actual geometry loading remains unchanged.

The focused regression check is
`tests/automated/check-geo-load-values-input.py`.

## Geometry reply comparison input validation

`_XmGeoReplyYes` now returns `False` when either the desired or response
geometry record is missing. Valid geometry replies retain their existing
comparison behavior.

The focused regression check is
`tests/automated/check-geo-reply-yes-inputs.py`.

## Geometry request input validation

`_XmMakeGeometryRequest` now returns `XtGeometryNo` when its widget or
geometry request is missing, rather than passing invalid pointers into Xt.
Normal request and “almost” retry behavior is unchanged.

The focused regression check is
`tests/automated/check-geo-request-input.py`.

## Child geometry application input

`_XmSetKidGeo` now treats a null geometry list as a safe no-op before
iterating it. Valid geometry application remains unchanged.

The focused regression check is
`tests/automated/check-geo-set-kid-input.py`.

## Geometry child setup input validation

`_XmGeoSetupKid` now returns `False` when its destination geometry record is
missing. Valid managed-child setup remains unchanged.

The focused regression check is
`tests/automated/check-geo-setup-kid-input.py`.

## Geometry and tracking child-count bounds

The shared `GMUtils` geometry helpers and `TrackLoc` input hit-test helper now
use Xt `Cardinal` indices for manager composite child arrays. Geometry,
overlap, margin, and hit-test behavior are unchanged while large child lists
avoid signed-index wraparound.

## GeoUtils empty-row dimensions

`_XmGeoGetDimensions` now treats a row with zero child boxes as having no
between-box spacing.  The old `numBoxes - 1` expression underflowed for an
empty row and could produce a huge wrapped fill width.  The helper also
returns safely for an incomplete matrix descriptor.

Box-plus-border extents and accumulated row/fill widths are also saturated to
the maximum representable `Dimension`, preventing large borders or many
children from wrapping layout dimensions.
Vertical fill and row-height totals use the same saturating arithmetic, and the
final margin reduction is guarded against unsigned underflow.

Nonempty rows retain their existing spacing calculation.  The regression is
`tests/automated/check-geoutils-dimensions.py`.

## GeoUtils fill spacing guards

The private fill-spacing helper now handles empty rows and missing output
pointers without dividing by zero or writing through NULL.  Its spacing sum
uses `unsigned long` intermediates, avoiding `Dimension` wraparound when a row
contains many boxes or large spacing values.  Normal nonempty layouts retain
the existing proportional spacing behavior.

The regression is `tests/automated/check-geoutils-fill-guards.py`.

## GeoUtils box-fitting allocation

`FitBoxesAveraging` temporarily sorts child geometry pointers while reducing a
row to fit its requested width.  It now returns without modifying the row when
there are no boxes, when the pointer-table size cannot be represented by
`size_t` or Xt's `Cardinal`, or when the temporary allocation fails.  This
prevents a null-table dereference and preserves the caller's existing geometry
on allocation failure.

Normal nonempty layouts retain the same sorting and width-fitting behavior.
The source-level regression is
`tests/automated/check-geoutils-fit-boxes.py`.

## GeoUtils matrix allocation bounds

`_XmGeoMatrixAlloc` builds one contiguous allocation for row layouts, child
geometry records, and an optional extension.  Its size intermediates are now
`size_t`, with checked additions and multiplications before the request is
narrowed to Xt's `Cardinal` allocation size.  Failed or unrepresentable
requests return `NULL` before any pointer arithmetic or dereference.
The matching `_XmGeoMatrixFree` cleanup is NULL-safe, allowing allocation
failure paths to use the normal cleanup routine.
The SelectionBox, FileSelectionBox, and MessageBox matrix constructors also
propagate a failed allocation as `NULL` instead of dereferencing it.
The shared query, geometry-manager, and size-update entry points now handle a
NULL matrix from any custom matrix creator without entering layout code.
The lower-level get, set, adjust, and arrange helpers also return safely for a
NULL matrix (or missing output dimensions in the arrange helper).

Normal matrix layout and the private API signature are unchanged.  The
source-level regression is
`tests/automated/check-geoutils-matrix-allocation.py`.

## GrabShell geometry initialization

`XmGrabShell` accumulates shadow and border extents into the geometry request
used when its first managed child is absent or does not provide one dimension.
The request structure now starts zeroed, so those dimensions remain defined and
the shell does not depend on an uninitialized stack value. Child-provided
widths, heights, and border widths retain their existing precedence.

The focused regression is `tests/automated/check-grab-shell-geometry-init.py`.

## GrabShell child-count bounds

`_XmFastExpose` traverses the GrabShell composite child array using the Xt
`Cardinal` child-count type. Its index now uses `Cardinal`, preserving expose
ordering while avoiding signed-index wraparound for large child lists.

## GrabShell layout guards

`GrabShell` layout previously accessed `composite.children[0]` unconditionally.
An empty grab shell can exist during construction or teardown, so that access
could dereference a missing child. The shadow and child-border offset was also
formed from unsigned dimensions.

`DoLayout` now returns for an empty shell and computes its offset in signed
arithmetic before deriving the child size. Existing one-child layout behavior,
resources, and the public API are unchanged. The regression
`tests/automated/check-grabshell-layout-guards.py` covers both protections.

## Hash-index native-width preservation

Hash-table bucket selection now converts `XmHashValue` to `unsigned long`
before taking the modulo. This preserves the upper bits of pointer-derived
hashes on 64-bit systems, reducing avoidable collisions without changing the
table API or lookup semantics.

The focused regression check is
`tests/automated/check-hash-index-width.py`.

The bucket-size table and all bucket indexes now use `Cardinal` consistently;
they no longer pass through signed `int` temporaries during resize, lookup,
or iteration.
Hash-table growth also checks the allocation width relevant to Xt's
`Cardinal` allocator, avoiding a redundant host-`SIZE_MAX` comparison.

## Hash-table resize chain linking

Hash-table resize now always records the current bucket predecessor when an
entry remains in its bucket. Previously that update was hidden behind the
optional `FIX_1264` macro; normal builds could therefore unlink the bucket
head when a later entry moved during rehash, losing entries from the table.

The focused regression is `tests/automated/check-xm-hash-resize-linking.py`.

## Hash-table allocation

The shared `_XmAllocHashTable` constructor now checks its table-record
allocation before initializing callbacks or bucket storage.  Allocation failure
returns `NULL` cleanly, allowing screen, image, and other caches to remain
uninitialized rather than dereferencing a missing table.

The focused regression is `tests/automated/check-hash-table-allocation.py`.

## Hierarchy child-list snapshots

`XmHierarchyGetChildNodes` checks both sentinel-count and
`(child_count + 1) * sizeof(Widget)` overflow before adding its terminating
`NULL` entry and passes the checked byte count explicitly as an Xt `Cardinal`
to the allocator. It also iterates with a `Cardinal` index, matching the full
child-count range. An unrepresentable count returns `NULL` without wrapping
the allocation size, and allocation failure returns cleanly.

## Hierarchy child-count loops

Hierarchy child-list traversal, insertion, removal, state propagation, and
layout-table construction now use `Cardinal` indices for their `Cardinal`
counts. Reverse insertion shifts count down explicitly instead of relying on a
negative signed sentinel. This preserves behavior while keeping large valid
hierarchies within the width of their Xt count fields.

The focused source regression is
`tests/automated/check-hierarchy-count-bounds.py`.

## Hierarchy disclosure-button allocation

Hierarchy nodes may have an optional open/close icon button. If Xt cannot
create that internal child, the node remains a valid hierarchy node: mapping
and callback setup are skipped, and later folder-pixmap updates ignore the
missing button. Other hierarchy operations continue without dereferencing a
null child, and a later reset can retry creation.

The contract is protected by
`tests/automated/check-hierarchy-disclosure-allocation.py`.

## Hierarchy top-node allocation

Hierarchy widgets create a top constraint node during initialization and then
initialize its state and parent pointers. The initializer now checks the Xt
allocation before those writes and raises an Xt error when memory is
unavailable, rather than leaving a hierarchy without its required root state.

## I18List coordinate bounds

I18List clamps column-header and row text coordinates before passing them to
Motif's signed `Position` drawing arguments.  Large list geometry or scrolling
offsets therefore cannot wrap text to an unrelated screen coordinate.

`tests/automated/check-i18-list-coordinate-bounds.py` protects this drawing
contract.

## I18List position conversion declaration

The I18List pixel-to-row/column conversion now declares its `pixel_y`
accumulator exactly once. A duplicate declaration in the same block prevented
the source from compiling with conforming C compilers.

The focused source regression is
`tests/automated/check-i18-list-position-declaration.py`.

## I18List row conversion bounds

`XmI18List` converts pixel coordinates into row indices during hit testing.
The computed division result and the current first-row offset can exceed the
legacy signed `short` row representation when geometry or scrolling state is
extreme.

Both conversion paths now use the existing bounded-short helper, preserving
normal row selection while preventing wrapped row indices.

The focused source regression is
`tests/automated/check-i18-list-row-conversion-bounds.py`.

## I18List column geometry bounds

Column widths are stored in Motif's legacy `short` table.  `XmI18List` now
resets each column's measured maximum, clamps a measured width before storing
it, and accumulates the horizontal scrollbar range in `int64_t` with an
`INT_MAX` ceiling.  This prevents one column's measurement from leaking into
the next column and prevents a large column set from wrapping the scrollbar
maximum.

The focused source regression is
`tests/automated/check-i18list-column-geometry-bounds.py`.

## I18List column-title allocation

I18List now copies column titles transactionally.  The title-pointer array is
checked against host `size_t` and Xt's `Cardinal` byte limits, and failures of
the array or an individual `XmStringCopy` release partial data and clear the
widget-owned pointer instead of dereferencing or publishing incomplete data.

The focused regression is
`tests/automated/check-i18list-column-title-allocation.py`.

## I18List column-width allocation bounds

`XmI18List` derives its column count from widget state and uses that count to
allocate the `short` column-width array.  The allocation now rejects negative
counts and checks both `size_t` multiplication and Xt's `Cardinal` limit before
narrowing the byte count.  An unrepresentable request returns without
discarding the existing widths or writing through an invalid replacement.

The focused source regression is
`tests/automated/check-i18list-column-width-rollback.py`.

## I18List column-width allocation rollback

I18List now allocates replacement column-width storage before releasing the
current array. Allocation failure leaves the existing widths intact, and an
initial allocation failure returns before any column writes through NULL.

The focused source regression is
`tests/automated/check-i18list-column-width-rollback.py`.

## I18List drag snapshot rollback

I18List drag initiation now checks its context, row-array, and `XmString`
allocations. Partial row snapshots are released on failure, and column-header
drags copy their title so completion owns every string it frees. Cleanup is
NULL-safe and clears the widget's drag context after release.

The focused regression is
`tests/automated/check-i18list-drag-snapshot-rollback.py`.

## I18List pixmap conversion allocation guard

I18List drag conversion now validates the active drag context and the
temporary `Pixmap` result allocation.  Allocation failure reports
`XmCONVERT_REFUSE` without dereferencing NULL or returning an incomplete
conversion result.

The focused regression is
`tests/automated/check-i18list-pixmap-conversion-allocation.py`.

## I18List selected-row allocation

`XmI18ListGetSelectedRows` now checks its result-array count against host
`size_t` and Xt's `Cardinal` byte limits before writing the terminator or
selected-row pointers. Allocation failure returns `NULL`, matching the
no-selection result and avoiding a null-pointer crash. The integer row-array
helper now validates its inputs and allocation size as well.

## I18List target-list conversion allocation

I18List now checks `XmeStandardTargets` before writing the target list.  Its
export/clipboard target list also reserves space for the optional pixmap atom;
previously that atom could write past a five-entry allocation.

The focused regression is
`tests/automated/check-i18list-target-list-allocation.py`.

## I18List visible-size bounds

`XmI18List` previously multiplied row height by the visible-row count in an
`int` and then narrowed the result to an X11 `Dimension`.  Large or malformed
row metrics could therefore wrap and produce a too-small widget.  The layout
now computes content and title extents in `uint64_t`, treats non-positive row
counts as empty, and clamps the final height to the representable
`Dimension` range.

The focused source regression is
`tests/automated/check-i18list-visible-size-bounds.py`.

## I18List widget-class guards

The exported I18List selection convenience APIs now validate the
`xmI18ListWidgetClass` before reading row data or toggling selection. The
single-row helper also rejects negative and out-of-range row numbers before
indexing the row array. Null and unrelated widgets return safely; valid
MultiList-backed I18List behavior is unchanged.

The source contract is checked by
`tests/automated/check-i18list-widget-class-guards.py`.

## I18List null-widget guards

The public I18List search and selection helpers now return safely when passed
a null widget: `XmI18ListFindRow`, `Xm18IListUnselectAllItems`,
`Xm18IListUnselectItem`, and `XmI18ListToggleRow`. The guard precedes the
internal list-record access, preventing malformed optional calls from
crashing the process while valid list behavior is unchanged.

The focused regression is
`tests/automated/check-i18list-widget-inputs.py`.

## IconButton coordinate bounds

IconButton clamps normal and insensitive-shadow label coordinates before
passing them to Motif's signed `Position` drawing arguments.  Oversized icon
layouts therefore cannot wrap text to an unrelated screen coordinate.
Pixmap and cached text origins are bounded during layout as well as at draw
time, so later clipping and hit-testing see the same safe coordinates.
Text origins derived from pixmap size, padding, and fallback spacing are also
clamped before being stored in the widget record.

`tests/automated/check-icon-button-coordinate-bounds.py` protects this drawing
contract.

## IconButton redisplay bounds

`XmIconButton` stores its core width and height as unsigned `Dimension`
values.  The redisplay path now checks each dimension before subtracting the
highlight frame and converts the operands before subtraction.  A widget whose
size is smaller than its frame therefore produces a zero drawing extent rather
than an unsigned wraparound that could request a huge X11 shadow region.

The shadow helper is called only when both extents are positive and both shadow
GCs exist.  Normal-sized buttons retain the existing geometry and shadow
ordering; the guard only makes the tiny or partially initialized state a safe
no-op.

The focused source regression is
`tests/automated/check-icon-button-redisplay-bounds.py`.

## IconFile cache comparator input guards

The icon-name cache comparator now validates both entry pointers and their key
strings before comparing them. Valid cache keys retain pointer and string
comparison behavior; malformed optional cache state now reports no match
instead of dereferencing null data.

The focused regression is `tests/automated/check-icon-cache-comparator.py`.

## IconFile cache directory input guard

`MakeCachedDirEntry` now rejects null and empty directory names before calling
`opendir`. Valid directory names retain the existing cache behavior, while
optional lookup state cannot reach the filesystem API with invalid input.

The focused regression is
`tests/automated/check-icon-cache-directory-input.py`.

## IconFile cache hash and insertion failures

IconFile cache hashing now preserves the full key-name length and handles null
hash keys safely. When a constructed icon-name entry cannot be inserted into
the cache, all owned strings and the entry are released instead of leaking an
unreachable cache record.

The focused regression is `tests/automated/check-icon-cache-hash-failure.py`.

## Icon-directory cache offset bounds

The icon-directory cache stores one 16-bit offset per filename plus a final
offset that marks the end of the last name.  The cache now allocates and
validates all `numFiles + 1` offsets before placing the name heap after them.
This prevents the terminal-offset write and subsequent heap placement from
running past the allocation while preserving the existing lookup format.

The combined name heap is still bounded by `USHRT_MAX`; directories whose
offsets cannot be represented use the existing uncached fallback rather than
silently truncating offsets.

The regression check is
`tests/automated/check-icon-cache-offset-allocation.py`.

## `XmeGetIconControlInfo` output validation

`XmeGetIconControlInfo` now returns `False` when any output pointer is missing,
before taking the process lock or writing fallback values. Calls that provide
all three outputs retain the existing color-object and fallback behavior.

The focused regression check is
`tests/automated/check-icon-control-info-outputs.py`.

## IconFile path scanner input guard

`find_slash` now returns immediately for a null path before entering either
single-byte or multibyte scanning logic. This keeps internal icon lookup paths
safe regardless of the active locale and preserves behavior for valid strings.

The focused regression is `tests/automated/check-icon-find-slash-input.py`.

## IconG column-width arithmetic

IconG now normalizes detail-column width with signed `long long` intermediates.
RTL parent-width subtraction no longer wraps through unsigned `Dimension`
arithmetic, and comparisons no longer narrow the column width to signed
`Position`.  Non-positive available space uses the existing minimum-width
fallback.

`tests/automated/check-icon-g-column-width-arithmetic.py` protects this
normalization contract.

## IconG coordinate bounds

IconG clamps label and detail text coordinates, including right-to-left detail
placement and the insensitive shadow pass, before passing them to Motif's
signed `Position` drawing arguments.  Large gadget layouts therefore cannot
wrap text to an unrelated screen coordinate.  Its XRectangle clip origins use
the same bounded conversion instead of an explicit wrapping `short` cast.
The large-icon mask clip origin follows the same rule.
RTL icon and label placement also uses bounded `Position` conversion for
computed origins.

`tests/automated/check-icon-g-coordinate-bounds.py` protects this drawing
contract.

## Icon-box initialization bounds

Icon-box initialization now handles zero or negative icon dimensions and
screen/layout products that cannot fit in the placement-count integer.  It
falls back to one placement slot before allocating the placement array and
rejects any unrepresentable allocation size.

`tests/automated/check-icon-init-bounds.py` protects these startup bounds.

## IconLabel coordinate bounds

IconLabel clamps its label origin in both left-to-right and right-to-left
layout modes before passing it to Motif's signed `Position` drawing arguments.
Large icons, spacing, or font metrics therefore cannot wrap text to an
unrelated screen coordinate.

`tests/automated/check-icon-label-coordinate-bounds.py` protects this drawing
contract.

## Icon-label geometry bounds

`XmIconLabel` now clamps text measurements and combined icon-plus-label widths
to the representable `Dimension` range. Long labels no longer wrap around to a
small geometry request, while normal labels retain the existing sizing.

The focused regression check is
`tests/automated/check-icon-label-geometry-bounds.py`.

## IconLabel resource transactions

`XmIconLabel` now skips text drawing when its render table is unavailable and
preserves the existing label and render table if a `SetValues` copy fails.
Preferred-size measurement and redisplay therefore share the same safe
fallback.

The focused source check is
`tests/automated/check-icon-label-geometry-bounds.py`.

## Icon-list growth bounds

Icon-box virtual-space growth now rejects null state, non-positive increments,
signed integer overflow, and sizes that cannot fit in an Xt allocation before
performing pointer arithmetic or allocation.

`tests/automated/check-icon-list-growth.py` protects these layout bounds.

## Icon-list source guard

`ExtendIconList` now rejects an inconsistent icon-box state where existing
placement entries are reported but the source list is absent.  This prevents a
null source pointer from reaching the copy operation while leaving valid list
growth unchanged.

`tests/automated/check-icon-list-source.py` protects the validation contract.

## Icon path bounds

Icon lookup no longer copies the arbitrary-length home directory into its fixed
`MAX_DIR_PATH_LEN` scratch buffer. The path helper fills that buffer itself when
an absolute path is needed, so the old copy was both unnecessary and an overflow
for unusually long `HOME` values. The source contract is covered by
`tests/automated/check-icon-path-bounds.py`.

## Mwm icon-placement allocation bounds

Mwm’s icon-placement grid allocates row, column, and slot arrays from display
and icon dimensions. The allocation sizes now use `size_t` arithmetic, reject
values that cannot fit the Xt `Cardinal` allocator or the existing integer slot
count, and clean up earlier arrays on failure. Normal grid sizing and placement
order remain unchanged.

`tests/automated/check-icon-placement-allocation-bounds.py` guards these size
checks.

## Mwm icon placement on small displays

Icon placement derives its grid from display size, icon size, and the optional
placement margin. When those values leave fewer than two slots, spacing is
zero and the grid remains one-dimensional; the spacing calculation no longer
divides by zero while reducing the grid. This keeps one valid placement slot
for small displays and preserves the existing multi-slot spacing behavior.

`tests/automated/check-icon-placement-small-display.py` guards the minimum-slot
and loop conditions.

## Icon-box policy fallback

Icon-box initialization now handles a missing or invalid scrollbar display
policy without calling string functions on `NULL` or writing into an unknown
resource buffer.  It uses a writable static `all` fallback and keeps the
existing `all`, `vertical`, and `horizontal` policy values unchanged.

`tests/automated/check-icon-policy-fallback.py` protects this path.

## XmGetIconFileName screen input guard

`XmGetIconFileName` now returns no filename when its screen argument is null,
before asking Xt for the display. Valid screen-based icon lookup is unchanged.

The focused regression is `tests/automated/check-icon-public-screen-input.py`.

## Icon-box resize guards

`CheckIconBoxResize` now ignores events received before the icon-box state is
complete or when width/height increments are invalid.  This prevents null
dereferences and divide-by-zero during resize handling while preserving normal
resize behavior.

`tests/automated/check-icon-resize-guards.py` protects this path.

## Icon root-coordinate guards

`GetIconBoxIconRootXY` now initializes valid output pointers to `(0, 0)` and
returns safely for missing outputs, clients, screen data, or icon-box widgets.
Valid icon-box coordinate translation remains unchanged.

`tests/automated/check-icon-rootxy-guards.py` protects the contract.

## Icon compound-string getter guards

IconGadget and IconButton now validate their synthetic label-string export and
padding-conversion inputs before reading widget state. Null output pointers,
null widgets, and unrelated widget classes return safely with deterministic
null/failed results; valid icon widgets retain their existing behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-icon-string-getter-guards.py
```

The padding-converter check is:

```sh
python3 tests/automated/check-iconbutton-padding-hook-guards.py
```
