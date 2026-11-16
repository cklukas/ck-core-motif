---
nav_exclude: true
---
# Archived Bugfix and Hardening Notes (Part 8)

Consolidated archive of standalone bugfix and hardening validation notes.

## Mwm workspace-info property width

Mwm serializes workspace backdrop values into `_DT_WORKSPACE_INFO_*` properties.
The numeric scratch area now sizes each slot for the full native
`unsigned long` hexadecimal representation (`0x` plus all digits and the
terminator), rather than assuming a 16-byte slot. Each value is written with
`snprintf`, so 64-bit Atom and Window values cannot overwrite the following
property field on x86_64 while the property format remains unchanged.

`tests/automated/check-workspace-info-width.py` guards the slot-width and
bounded-formatting contract.

## Workspace-list allocation

Workspace-list parsing now checks its initial array of parsed-name pointers
before filling it.  Allocation failure follows the same warning and controlled
window-manager exit path as later workspace-data allocation failures.

`tests/automated/check-workspace-list-allocation.py` protects this startup
path.

## Mwm workspace-list limit

Mwm has always limited the resulting workspace set to `MAX_WORKSPACE_COUNT`
(64). Workspace-list parsing now applies that same limit while collecting
resource names, so extra entries are ignored before the temporary name-pointer
array grows. Existing configurations with 64 or fewer workspaces keep the same
names and ordering, while oversized resource values no longer cause needless
allocation and parsing work.

`tests/automated/check-workspace-list-limit.py` guards the bounded parser loop.

## Workspace property guards

Workspace property updates now validate screen and workspace state before
building `_DT_WORKSPACE_LIST` and `_DT_WORKSPACE_CURRENT`. An empty workspace
set publishes an empty list without treating `XtMalloc(0)` as an error, while
missing active state safely skips the current-workspace update.

## Mwm workspace property-name bounds

Mwm derives `_DT_WORKSPACE_INFO_*` atom names from the workspace resource
name. The constructor now performs the length calculation in `size_t`, checks
both arithmetic overflow and the `XtMalloc` `Cardinal` limit, and reports an
unrepresentable name to its callers before allocating. The two property
callers skip the X request safely when the name cannot be represented.

`tests/automated/check-workspace-property-name-bounds.py` guards this checked
allocation contract.

## WSM workspace-property allocation failures

WSM workspace hints and workspace-list properties now check their atom-array
sizes before converting them to Xt’s `Cardinal` allocator and handle NULL
returns without copying or publishing invalid data. Hint callers receive
`BadAlloc`; list publication is skipped with a warning when it cannot be
represented or allocated.

`tests/automated/check-workspace-property-oom.py` guards these failure paths.

## WSM per-match attribute allocation failure

During `WM_WINDOWS` request unpacking, each match's combined allowed and
prohibited attribute block is now checked before use. If a later block fails,
already allocated blocks and the pointer array are released, the count is
cleared, and unpacking returns without partial state.

`tests/automated/check-wsm-attr-pair-oom.py` protects this rollback path.

## WSM unsupported attribute sizes

WSM window-data unpacking now maps unsupported attribute widths to an inert
`WSM_NONE` record with zero length. This prevents malformed wire metadata from
leaving an undefined type that later dispatch or cleanup could interpret as a
valid list.

`tests/automated/check-wsm-attr-size.py` protects this malformed-input path.

## WSM display and screen cache allocation

WSM keeps display and screen metadata in lazily allocated linked lists.  The
old code immediately stored and dereferenced newly allocated nodes, so a
failed allocation could crash before callers had a chance to handle the
missing configuration.  The cache now checks every new node, leaves the list
unchanged on failure, emits an Xt warning, and returns `NULL` through the
existing lookup APIs.

Configuration cleanup also returns immediately when the display record is
unavailable, so teardown after a failed cache allocation cannot dereference a
null display entry.

`tests/automated/check-wsm-cache-allocation.py` protects these failure paths.

## WSM configuration attribute allocation failure

Configuration-format reply unpacking now checks each screen attribute-list
allocation. If one fails, it clears the failed count, releases any earlier
lists through the existing screen cleanup helper, and returns before writing
through a null pointer. Successful replies retain the existing cache and
reply-pointer behavior.

`tests/automated/check-wsm-config-attr-oom.py` protects this rollback path.

## WSM config-format reply allocation failure

WSM configuration-format reply unpacking now returns cleanly when its screen
metadata record cannot be created. This avoids dereferencing a null screen
record after display-cache allocation failure; normal configuration-format
replies continue through the existing attribute-list path.

`tests/automated/check-wsm-config-reply-oom.py` protects this boundary.

## WSM config-format debug output

The WSM diagnostic formatter now passes its caller-provided buffer to the
initial `sprintf` in `PrintConfigFormat`.  The previous call omitted the
destination argument, so printing a configuration-format reply had undefined
behavior even though the rest of the formatter expected the buffer to contain
the first line.

All debug sections now append through a bounded formatter.  Oversized
extension, attribute, or value lists are truncated at the existing static
buffer boundary instead of overwriting adjacent state; normal output remains
unchanged while it fits.

`tests/automated/check-wsm-debug-config-format.py` protects both the original
call site and the bounded formatter contract.

## Empty WSM configuration formats

An empty global, window, or icon attribute list is a valid WSM configuration
format.  The reply decoder records it as a null list with a zero count without
performing a zero-byte allocation.  This keeps the peer-compatible empty
format valid on allocators that return null for a zero-size request.

Regression: `tests/automated/check-wsm-empty-collection-unpack.py`.

## WSM empty reply validation

`UTMReplyReceived` now rejects a successful selection conversion with no value
for request types whose reply decoder reads a payload. This prevents the
decoder from dereferencing a null value after a malformed or incomplete
conversion. `WSM_SET_STATE` remains allowed to return an empty payload because
its reply has no encoded fields.

The focused regression check is
`tests/automated/check-wsm-empty-reply-guards.py`.

## WSM empty request validation

`WSMProcessProtoTarget` now rejects an empty selection value for request types
whose decoders read fields from the wire. This prevents malformed requests from
reaching `UnpackListNum` and related helpers with a null value. The protocol's
payload-less `WSM_CONFIG_FMT` request remains accepted.

The focused regression check is
`tests/automated/check-wsm-empty-request-guards.py`.

## WSM extension unpack allocation failure

WSM request and reply extension lists now check their counts against Xt's
`Cardinal` allocation width before filling or marking the structures owned.
The request's connection-version array applies the same width check before
copying versions. On failure the count and pointer are cleared and unpacking
returns, so dispatch and cleanup cannot walk a null array.

`tests/automated/check-wsm-extension-unpack-oom.py` protects both unpackers.

## WSM get-state unpack allocation failure

WSM `GET_STATE` reply unpacking now checks its window-info count against Xt's
`Cardinal` allocation width before initializing entries or marking the reply
owned. A failed or unrepresentable allocation clears the count and returns, so
reply dispatch and cleanup remain safe.

`tests/automated/check-wsm-get-state-unpack-oom.py` protects this behavior.

## WSM match-attribute allocation failure

WSM `WM_WINDOWS` request unpacking now checks both the match-attribute pointer
array and each nested property-array count against Xt's `Cardinal` width before
indexing or allocating. A failed or unrepresentable allocation clears the
count and returns, preventing a null-array write while preserving the existing
protocol layout.

`tests/automated/check-wsm-match-attrs-oom.py` protects this failure path.

## WSM packed-size guards

The workspace-manager protocol computes message sizes before allocating a
packed request or reply.  Those calculations now reject negative data lengths
and use checked additions for request and reply window data, extension strings,
window entries, and nested arrays.
If a malformed or extreme count cannot be represented by the `int` sizing
contract, packing fails before allocating or writing a truncated buffer.  The
wire format and valid message handling are unchanged.

The public packer uses a `long` byte-count accumulator because it returns the
length through an `unsigned long` out parameter.  The internal per-window
helpers retain their `int` result contract, but now accumulate through the
same checked `long` helper before their proven-safe result conversion.  This
keeps the x86_64 build type-correct and avoids taking the address of a
`register` variable.  The focused check compiles and executes the production
checked-add helper under strict pointer-type warnings.

## WSM packer input validation

The internal WSM request and reply packers initialize any supplied length and
error outputs before validating their required inputs.  A missing request,
reply, length output, error output, or request display now returns no message
without dereferencing the incomplete caller state.

All malformed request payload, count, pointer, size, and allocation failures
also report `WSM_ERROR_INTERNAL` instead of leaving the caller with a null
message and an erroneous `WSM_SUCCESS` status.  Protocol-state failures retain
their more specific configuration error codes.

The WSM list maximum is the full `CARD16` range (65,535).  Its expression casts
the complemented value back to `CARD16` before widening it, avoiding signed
integer promotion that would otherwise turn the limit into `-1`.

Window-data lists are also rejected at the public request and reply boundaries
when their count exceeds the WSM packer's bounded data-list domain.  This
prevents malformed data from reaching the lower-level size and packing helpers.

Regression: `tests/automated/check-wsm-packer-inputs.py`,
`tests/automated/check-wsm-request-counts.py`, and
`tests/automated/check-wsm-reply-counts.py`.

## WSM paired-property cleanup

The MWM WSM request unpacker consumes two wire properties for each match
attribute.  Allocation-failure cleanup now performs two bounded reads per
attribute instead of comparing an index against `num * 2`.  This avoids signed
integer overflow for hostile or corrupted list counts while preserving the
protocol layout and normal requests.

`tests/automated/check-wsm-paired-property-cleanup.py` protects the cleanup
loops.

## WSM reply packing failure handling

`WSMProcessProtoTarget` now initializes conversion outputs before processing a
  request and checks the result of `_WSMPackReply`. A failed non-empty reply
  releases request/reply state and returns `False` instead of reporting a
  successful conversion with a null buffer. Valid replies retain their wire
  format and target type.

The focused regression check is
`tests/automated/check-wsm-reply-packing-guards.py`.

## WSM reply payload validation

`UTMReplyReceived` now rejects a selection reply that reports a non-zero
length with a null value before passing it to `_WSMUnpackReply`. The requester
receives `WSM_ERROR_INTERNAL`, and valid replies retain their existing callback
and wire behavior.

The focused regression check is
`tests/automated/check-wsm-reply-payload-guards.py`.

## WSM request callback guard

`WSMProcessProtoTarget` now checks both the screen metadata and its registered
request callback before dispatching an incoming protocol request.  If either
is unavailable, the unpacked request is released and the conversion returns
`False` instead of dereferencing a null callback.

Callback registration also tolerates a missing screen record, which can occur
when WSM display metadata allocation fails. The registration request is then
ignored safely and no partially initialized record is dereferenced.

Selection ownership also stops when the selection atom cannot be obtained. This
avoids sending `None` to `XGetSelectionOwner` or claiming a selection without
usable WSM display metadata.

`tests/automated/check-wsm-request-callback.py` protects this lifecycle path.

## WSM send failure paths

`WSMSendMessage` now checks widget realization before packing a request, so an
unrealized widget cannot leave a packed message allocated on an early return.
Packing failure and request-state allocation failure both release the packed
message and notify a non-null reply callback with a null reply. A null callback
is accepted safely for callers that only need the Boolean result.

`tests/automated/check-wsm-send-failures.py` protects these failure paths.

## WSM set-state unpack allocation failure

WSM `SET_STATE` request unpacking now checks its window-info count against
Xt's `Cardinal` allocation width before initializing entries. A failed or
unrepresentable allocation clears the count and returns before the request is
marked owned; successful requests mark the matching `set_state.allocated`
field for the existing cleanup path.

`tests/automated/check-wsm-set-state-unpack-oom.py` protects this behavior.

## WSM per-attribute list allocation failure

WSM 8, 16, and 32-bit list attributes now check their data-array allocations
before reading wire values. A non-empty allocation failure resets the record to
`WSM_NONE` with zero length, allowing enclosing cleanup to proceed safely.
Empty lists retain their valid zero-length representation.

`tests/automated/check-wsm-single-data-oom.py` protects all three paths.

## WSM target cache guards

Target validation and target-list construction now handle an unavailable WSM
display cache.  Validation returns `False`; target-list construction reports
an empty list through `len_ret` and returns `NULL`.  It also rejects a null
length output pointer and propagates target-array allocation failure safely.
This preserves normal target behavior while making failure cases safe.

`tests/automated/check-wsm-target-cache-guards.py` protects both callers.

## WSM transient helper guards

WSM transient stacking can run while workspace membership is being detached.
`FindSubLeaderToTop`, `MakeTransientFamilyStackingList`, and `LeaderOnTop`
now reject missing tree nodes or output buffers, returning neutral results
instead of dereferencing stale state.  Valid WSM stacking behavior is
unchanged.

## WSM WM_WINDOWS reply allocation failure

WSM `WM_WINDOWS` reply unpacking now checks its entry count against Xt's
`Cardinal` allocation width before initializing entries or marking the reply
owned. A failed or unrepresentable allocation clears the count and returns
safely.

`tests/automated/check-wsm-win-reply-oom.py` protects this failure path.

## WSM window-data unpack allocation failure

The shared WSM window-data unpacker now initializes its output pointers,
rejects missing configuration metadata, checks the bit-mask allocation, and
checks the resulting data-record allocation. Failure clears the output count
and releases temporary storage before returning, so callers can clean up
safely.

`tests/automated/check-wsm-windata-oom.py` protects these contracts.

## WSM WM_WINDOWS property allocation failure

WSM `WM_WINDOWS` request unpacking now checks both the window-property and
match-attribute counts against Xt's `Cardinal` allocation width before copying
wire values. A failed or unrepresentable allocation clears the corresponding
state and returns before writing through a null pointer.

`tests/automated/check-wsm-wm-windows-oom.py` protects this failure path.

## Xft cache initialization

When the Xft draw cache grows, all newly allocated slots are now zeroed in
every build. This prevents uninitialized display/window pairs from being
treated as cached draws when the optional `FIX_1449` macro is not defined.

## Xft draw-cache creation failures

`_XmXftDrawCreate` now returns immediately when Xft cannot create a draw
object, rather than caching a null entry for the drawable. Subsequent calls can
therefore retry after the drawable or display state becomes usable.

## Xft draw consumers

Text, label, compound-string, and clip consumers now handle a failed Xft draw
creation without dereferencing null. Existing rendering remains unchanged when
the drawable can be created successfully.

## Xft draw-cache teardown

`_XmXftDrawDestroy` now destroys the object owned by the matching cache entry,
not the caller’s pointer. The cache slot is cleared first, and null cached
objects are skipped, making repeated or delayed teardown safe when rotated
label pixmaps are released.

## Xft drawable eviction

`_XmXftDrawEvict(Display *, Window)` clears and destroys an existing cached
draw without creating a replacement. Drawable owners can call it immediately
before releasing a pixmap or window, preventing stale Xft objects from being
returned if an XID is later reused.

## Xft draw inputs

`_XmXftDrawCreate` now rejects a null display or `None` drawable before cache
lookup and Xlib queries. This makes teardown and error paths safe while valid
display/drawable pairs retain their existing cache and rendering behavior.

## Xft drawable depth selection

The Xft draw cache now checks the drawable geometry before selecting a draw
implementation. Depth-one pixmaps use `XftDrawCreateBitmap`, avoiding the
lazy `BadMatch` that can occur when a default color visual is used for a
rotated-label bitmap. Other drawables use their realized window visual and
colormap when available, with the existing default-screen fallback retained.

## Xft pixmap visual selection

The Xft drawable helper uses `XGetGeometry` for information common to windows
and pixmaps. Depth-one pixmaps use the bitmap path; non-default-depth color
drawables select a visual matching both the drawable depth and the screen that
owns its root; default visual and colormap selection likewise uses that screen.
Window-only attribute queries are never issued for arbitrary drawables, and
temporary visual-query results are released.

## Xft compound-string line height

Xft-backed compound-string geometry now uses `XftFont.height` for the line
height while retaining ascent and descent for baseline calculations.  This
preserves font leading and avoids clipping or compressing labels whose Xft
metrics are taller than ascent plus descent.

Core-font and font-set geometry are unchanged, and the existing underline
adjustments still apply after the Xft height is selected.

## Xft UTF-16 underline lengths

The compound-string underline matcher stores wide-character segment lengths
in bytes, while `XftTextExtents16` consumes a count of 16-bit characters.  The
Xft calls now convert the byte offsets to character counts before measuring
the prefix and underline text.  This prevents reads beyond the segment and
keeps underline placement correct for wide-character Xft strings.

## Xft UTF-8 compound-string drawing allocation

When a two-byte core font draws a UTF-8 compound-string segment, Motif first
converts the segment to UCS-2.  Both image and normal drawing paths now skip
the draw if that temporary conversion cannot allocate, rather than passing a
null pointer to Xlib.  Successful conversions are released with `XtFree`,
matching `_XmUtf8ToUcs2`.

## Xft UTF-8 compound-string metrics allocation

The ISO-10646 core-font path used for UTF-8 compound strings converts the
input to UCS-2 before measuring it.  If that conversion cannot allocate its
temporary buffer, metrics now return without calling `XTextExtents16` with a
null pointer.  Successful conversions are released with `XtFree`, matching
the allocator used by `_XmUtf8ToUcs2`.

## XIM input-policy validation

XmIm now validates input-policy values before allocating an XIC. Invalid
values are ignored by registration, unregistration, and update dispatch rather
than reaching assertion-only branches. The supported per-shell, per-widget,
and inherited policies retain their existing behavior.

The regression check is `tests/automated/check-xim-input-policy.py`.

## XIM preedit callback guards

The preedit-start and preedit-done callbacks now reject missing XIM client data
before looking up widget state. This makes malformed or late external input
method callbacks harmless while preserving normal callback dispatch.

The regression check is
`tests/automated/check-xim-preedit-callbacks.py`.

## XIM reference removal

XIM display data tracks widgets that share input contexts. The old removal
helper decremented the reference count before checking that the widget was in
the list, so a duplicate or stale destroy callback could underflow the count
and corrupt the arrays.

The helper now verifies the reference first and treats an absent widget as an
idempotent no-op. Registered widgets keep the existing swap-and-release cleanup
behavior.

`tests/automated/check-xim-ref-removal.py` guards the lifecycle checks.

## Xm argument-filter allocation guard

`_XmFilterArgs` now checks both `size_t` and Xt `Cardinal` byte limits before
allocating a filtered argument list. If allocation fails, it leaves the
returned list NULL with a zero count, allowing callers to handle the same
empty result safely.

The focused regression is
`tests/automated/check-xm-filter-args-allocation.py`.

## Xm argument filtering validation

`_XmFilterArgs` now initializes its returned list and count before processing,
rejects null output pointers, null argument arrays, and unrepresentable
allocation sizes, and treats a null filter list as no filtering. Valid argument
filtering remains unchanged. This prevents malformed private helper calls from
crashing or wrapping the temporary `Arg` allocation while retaining the Motif
ABI and existing valid-call behavior.

## Xm hash-table allocation

Motif's private hash tables are used by image, pixmap, GC, and widget caches.
Bucket-pool allocation now fails cleanly instead of dereferencing a null pool,
and its free-list setup no longer forms a one-past-the-end pointer.  Resizing
builds a replacement bucket array, passes its bounded byte count explicitly as
an Xt `Cardinal`, and publishes it only after allocation succeeds, preserving
the live table on failure.  Null table frees and insert attempts are also
harmless.

`tests/automated/check-xm-hash-allocation.py` protects these contracts.

`_XmAddHashEntry` now reports insertion failure.  The trait registry uses that
result to free its temporary key and return `False`, so an allocation failure
does not silently lose the registration or leak memory.

Lookup, mapping, and removal now tolerate empty or null tables.  Removal saves
the key before returning its bucket to the pool and clears iterator state,
preventing use-after-free results and stale iterator pointers.

Bucket indices normalize signed callback results through an unsigned domain,
so pointer-derived hashes cannot produce negative array subscripts on 64-bit
hosts.

## Legacy XmPaned margin bounds

The older `XmPaned` widget adds twice the horizontal or vertical margin to a
negotiated child size before temporarily applying the parent geometry.  That
sum now uses widened arithmetic and clamps to the X11 `Dimension` range in
both orientations, preventing a large margin from wrapping the temporary
layout size.

The focused source regression is
`tests/automated/check-xm-paned-margin-bounds.py`.

## xm-va-count-arithmetic
Simple-menu varargs count arithmetic
=====================================

The six simple-menu convenience constructors now check the addition of their
fixed internal argument count to the caller-derived count before storing it in
the legacy `int` `num_args` variable.  An unrepresentable sum returns `NULL`
after releasing the application lock, avoiding signed wraparound before the
allocation checks run.

## xm-va-count-bounds
Simple-menu varargs counter bounds
==================================

The nested-list and top-level varargs counters now saturate at `INT_MAX`
instead of overflowing signed counters.  The public counting helper also
returns safely when an output pointer is missing.  Saturated counts are then
rejected by the constructor arithmetic and allocation checks, preserving a
fail-closed result for malformed or unrepresentably large lists.

## xm-va-nested-bounds
Nested typed varargs bounds
===========================

Nested `XtVaNestedList` entries can expand into more records than their outer
list contributes.  The typed-varargs converter now carries the remaining
capacity through recursive expansion and rejects a nested list that would
exceed the allocated array.  It also rejects null nested lists, frees the
temporary array, and reports an empty result on failure.

Successful nested-list conversion remains unchanged.

## xm-va-nested-null
Null nested varargs lists
=========================

The internal nested-varargs counting and conversion helpers now treat a null
`XtTypedArgList` as an empty list.  This keeps malformed or optional nested
list inputs from being dereferenced while preserving the normal conversion
result for valid lists.

## xm-va-process-allocation
Simple-menu varargs allocation failures
========================================

The shared simple-menu varargs processor now validates its normalized `Arg`
array size and every button metadata array count before allocation. It passes
explicit Xt `Cardinal` byte sizes, checks every temporary allocation, frees
partial results on failure, and returns failure to
the six public simple-menu constructors, which now unlock and return `NULL`
without dereferencing incomplete arrays.

Successful menu construction and public function signatures are unchanged.

## xm-va-typed-allocation
Typed varargs argument-list allocation
=======================================

The private `_XmVaToTypedArgList` helper now initializes both output values,
rejects non-positive or unrepresentable capacities before byte multiplication,
and returns an empty result when Xt cannot allocate the typed-argument array.
This prevents malformed internal counts from wrapping the allocation size or
causing a null-array write while preserving the existing successful conversion
format.

## xme-vlcreate-allocation
XmeVLCreateWidget argument-list allocation
===========================================

`XmeVLCreateWidget` is the shared implementation behind many Motif
variable-argument convenience constructors. It now rejects negative or
unrepresentable counts before multiplying by `sizeof(Arg)`, passes the bounded
byte size explicitly as an Xt `Cardinal`, avoids an unnecessary zero-byte
allocation for an empty list, and returns `NULL` if Xt cannot allocate the
temporary list. Allocation failure also releases the application lock before
returning.

The successful varargs conversion and widget-creation ABI are unchanged.

## Input-method area geometry bounds

The input-method integration now clamps shell-height removal and status or
preedit rectangle placement when the IM area is taller than its shell.  This
prevents unsigned `Dimension` subtraction from wrapping shell requests or XIM
area coordinates during provider teardown and reconfiguration.

The separator drawn above the IM area uses the same guarded subtraction and is
saturated at Motif's legacy `Position` boundary, so a very large shell cannot
wrap the separator to an unrelated vertical coordinate. Xlib's instantiate
callback result is normalized to Motif's Boolean state before it is retained.

The source-level regression is `tests/automated/check-xmim-area-bounds.py`.

## Input-method callback registration guards

Real XIM callbacks are now installed only for an existing widget reference and
an allocated callback array. Stale widget callbacks and allocation failures
leave the input-method state unchanged.

## Input-method display initialization allocations

Display input-method state and locale-modifier scratch storage are now checked
before use. Allocation failure leaves the display state uninitialized or skips
the optional modifier update without dereferencing a null buffer.

## XmIm missing-shell-data update guard

`XmIm` resource updates can race widget teardown: the display-level XIM may
still be available after the shell's private `XmImShellInfo` has been freed.
The update routine now returns safely when that per-shell record is absent.

Normal realized-shell updates are unchanged, and no public ABI or resource
semantics change.  The static regression check is
`tests/automated/check-xmim-missing-shell-data.py`.

## XmIm oversized nested-list guard

`XmIm` builds `XVaNestedList` values from a portable, fixed spelling of ten
name/value pairs.  Resource updates can legitimately accumulate more entries
than that.  The old assertion aborted the application when the count exceeded
ten, even in normal release builds.

`VaCopy` now rejects only that unsupported conversion size and returns `NULL`.
Its existing callers already treat a failed nested-list construction as a
recoverable fallback, so valid smaller lists and the public Motif ABI are
unchanged.  The static regression check is
`tests/automated/check-xmim-oversized-nested-list.py`.

## Input-method preedit allocation failures

Preedit replacement and feedback buffers are now checked as a pair before
copying or replacing the active buffer. The optional callback conversion buffer
is also checked; if it cannot be allocated, the callback keeps the original
counted `XIMText` view instead of copying through `NULL`.

## XIM preedit conversion

The XIM preedit bridge now handles `mbstowcs` failure explicitly and requires
the conversion to produce exactly the number of wide characters advertised by
the input method. Invalid locale text is rejected before the replacement
buffer is published, so a failed preedit update cannot leave partially
converted text in the widget.

Regression check:

```text
python3 tests/automated/check-xmim-preedit-conversion.py
```

## Input-method preedit-position bounds

The XIM preedit-position path now computes focused-widget space in signed
arithmetic and clamps it at zero before sizing the preedit rectangle.  Narrow
widgets with large primitive margins no longer produce wrapped unsigned
widths or heights.

The source-level regression is
`tests/automated/check-xmim-preedit-position-bounds.py`.

## Input-method preedit width bounds

The input-method setup path now computes preedit width from a signed available
width after the status area and clamps it at zero.  A status area wider than a
collapsed shell therefore cannot wrap the preedit width into a large unsigned
value.

The source-level regression is
`tests/automated/check-xmim-preedit-width-bounds.py`.

## Input-method reference-table growth

XIM widget references now grow their widget and callback arrays as one
transaction. If either allocation fails, both existing arrays remain unchanged
and `add_ref` reports failure without inserting a partial reference. Growth
checks `Cardinal` and `size_t` arithmetic before allocating, and guarantees
progress even if a damaged or legacy record has a one-entry capacity.

## XmIm reference-list shrinking

The input-method display record keeps parallel widget-reference and callback
arrays. The old shrink path resized them independently and reduced the
recorded capacity before checking either allocation, allowing a failed second
resize to leave inconsistent metadata.

Shrinking now allocates both replacement arrays, copies the active prefix,
and commits the pair only when both allocations succeed. Otherwise the
larger existing arrays remain valid.

Removal searches with the same unsigned `Cardinal` type as the stored count,
so a valid large list is not truncated by an `int` comparison. The shrink
threshold is expressed as `n <= (max - 1) / 3` instead of multiplying `n`,
which avoids unsigned wraparound while preserving the historical `3*n < max`
threshold. The public XIM/Motif interfaces remain unchanged.

## XmIm registration allocation

XmIm keeps per-widget resource snapshots across input-method transport
restarts. Registration creation, value copies, and argument-array growth now
check allocation results before dereferencing or publishing new state.

Failed updates leave existing snapshots intact and skip only the resource that
could not be copied. Successful registrations retain their resource ordering
and restoration behavior.

## xmim-registration-growth
Input-method registration cache growth
=======================================

When the input-method registration cache records a new resource, its `Arg`
array grows by one element.  The growth path now checks the resulting byte
count against both `size_t` and Xt's `Cardinal` limits before calling
`XtRealloc`, preserving the existing failure behavior for unrepresentable or
unallocatable growth.

## XIM registration lifecycle

XIM widget registration is optional state backed by Xt allocations.  If a
registration record or a widget-reference array cannot be allocated, the
input-method code now exits the current operation before dereferencing the
missing record or saving an Xlib context.  This keeps failed registration from
leaving a widget mapped to an XIC that has no corresponding lifetime reference.

The focused static regression is
`tests/automated/check-xmim-registration-lifecycle.py`.  It checks the
registration guards, the reference-growth failure check, and the destruction
callback path that walks saved widget state.

## XmIm stale shell-reference guard

Input-method shell references and vendor-shell extension data are released by
different teardown paths.  During that window, reconstructing an application-
created XIC can encounter a reference whose `XmImShellInfo` is already gone.

The reconstruction walk now skips that stale entry and continues searching.
XIM shutdown also removes a stale reference when its extension data is already
gone, allowing close to complete.
Live shell records retain the same lookup behavior, and the public Motif ABI is
unchanged.  The static regression check is
`tests/automated/check-xmim-stale-shell-reference.py`.

## XmIm stale-XIM fallback guard

An input method can be closed and reopened while an existing input context is
still referenced.  The unrecognized-resource fallback previously asserted
that the XIC belonged to the cached XIM, which could abort during this normal
lifecycle transition.

The fallback now returns without recreating the XIC when the ownership differs.
The existing context remains intact, valid same-XIM updates are unchanged, and
the public Motif ABI is unchanged.  The static regression check is
`tests/automated/check-xmim-stale-xim-fallback.py`.

## XmIm VaArg-list growth

VaArg-list expansion now checks both host `size_t` and Xt `Cardinal` byte
limits before calling `XtRealloc`.  If the requested capacity is not
representable, the existing list remains unchanged and the new argument is
ignored, matching the existing allocation-failure behavior.

Motif’s input-method adapter builds temporary `XVaNestedList` arguments in a
small growable array.  The old helper updated the capacity and pointer before
checking `XtRealloc`, then wrote the new pair unconditionally.

The helper now checks the capacity-to-byte conversion and commits the new
array only after allocation succeeds.  A failed growth leaves the existing
argument list intact.

## Input-method varargs allocation failures

`XmImVaSetFocusValues` and `XmImVaSetValues` now stop safely when their
temporary argument array cannot be allocated or the varargs counter reaches
its signed representation limit. The existing zero-argument case continues
without allocating an array, and validated counts are explicitly converted to
the public `Cardinal` argument count.

## xmim-varargs-count-bounds
Input-method varargs count bounds
=================================

The input-method varargs helper now checks the counted argument list against
both `size_t` and Xt's `Cardinal` byte limits before calling `XtCalloc`.  This
prevents a large or malformed count from wrapping the temporary `Arg` array
size and keeps the existing null result for invalid or failed allocations.

## Internal list allocation failures

The internal `XmList` node allocator now checks its `XtCalloc` result before
initializing a free-node batch. If allocation fails, `_XmListAddAfter` and
`_XmListAddBefore` return `NULL` and leave the list links unchanged.

## Internal list lifecycle inputs

The internal queue and list helpers now tolerate null handles. Empty counts,
pop operations, removal, execution and insertion return their existing empty
or failure results without dereferencing a missing list.

## Internal list-stack allocation failures

The internal cleanup stack now checks null stack handles and failed growth
reallocations. Growth checks both host `size_t` and Xt's `Cardinal` byte limits
and passes an explicit Xt-sized allocation. A failed growth leaves the
existing stack and its top index unchanged, so cleanup code does not write
through a null buffer.

## Xmos basename conversion rollback

The compatibility path in `_XmOSGetDirEntries` now allocates the basename
copy before releasing the full-path temporary.  If that allocation fails, the
temporary is released exactly once and the scan stops without publishing a
partial entry or dereferencing a lost pointer.

The focused regression is
`tests/automated/check-xmos-basename-rollback.py`.

## Xmos directory-cache allocation

The file-selection directory cache grows in fixed blocks and stores a copy of
each entry name. Growth now uses a temporary pointer and publishes the new
capacity only after `XtRealloc` succeeds; entry-size arithmetic and entry
allocation are checked before the cache is indexed.

A failed cache update returns the existing no-match result without corrupting
the cache or dereferencing a null entry.

## Xmos directory-entry list allocation

Directory scans grow the caller-owned entry-pointer array in fixed blocks and
allocate one string per accepted candidate. Growth now publishes the new
capacity only after `XtRealloc` succeeds, and both full-path and basename
string allocations are checked.

On memory pressure the scan stops with all previously collected entries and
their original capacity intact; successful enumeration is unchanged.

## Xmos mask-name construction

`_XmOSGenerateMaskName` derives a pixmap mask name by copying the measured
prefix of an image path, then inserting `_m` before its suffix.  The old
`strncpy` call intentionally copied a non-terminated prefix and relied on the
following writes, which triggered compiler truncation diagnostics and made the
copy contract unclear.

The prefix is now copied with exact-length `memcpy`; the existing suffix and
terminator writes remain unchanged.  Normal mask names and caller-owned
storage requirements are preserved.

`tests/automated/check-xmos-mask-name.py` protects the exact-copy contract.

## Xmos path construction bounds

The path helpers use caller-provided `MAX_DIR_PATH_LEN` storage. The previous
implementation copied a trusted-looking `PWD` environment value and appended
relative path text without checking either length.

`GetCurrentDir` now ignores an overlong `PWD` and falls back to `getcwd`, while
`_XmOSAbsolutePathName` checks both `..` replacement and ordinary relative-path
appends before copying. Valid paths retain their existing resolution behavior;
unrepresentable paths return failure without writing past the buffer.

`tests/automated/check-xmos-path-bounds.py` guards the capacity checks.

## Xmos pattern allocation

`GetFixedMatchPattern` now checks its output-buffer allocation before writing
the regular-expression prefix and converted pattern. Allocation failure
returns `NULL`, matching its existing invalid-input behavior without changing
successful pattern conversion.

Multibyte characters are copied with their validated `mblen` byte count via
`memcpy`, avoiding a signed-length conversion in the bounded copy path.

## Xmos pattern conversion bounds

The file-selection path converts a match pattern into a regular expression.
Escaping and wildcard expansion can require up to twice the input length, so
the old `XtCalloc(2, strlen(pattern) + 4)` expression could wrap its element
size for an extremely large resource string.  A missing pattern also reached
`strlen` directly.

The converter now rejects null input and checks the doubled allocation size
before calling Xt.  Normal pattern syntax and matching behavior are unchanged;
only unrepresentable or missing input is rejected safely.

`tests/automated/check-xmos-pattern-bounds.py` protects the input and arithmetic
contract.

## Xmos qualified-path allocation guards

`GetQualifiedDir` now checks each path-buffer allocation before writing the
home-directory, absolute, relative, or fallback path. Allocation failure
returns NULL, matching the function’s existing failure result without a
partial or corrupted pathname.

`_XmOSInitPath` also bounds the copy of an environment-provided resource path
before converting its `size_t` length to Xt's `Cardinal` allocation size. An
unrepresentable value now fails cleanly instead of truncating the allocation.

`XmeGetHomeDirName` applies the same bound to `$HOME` and passwd-derived home
directories, returning the existing empty-name fallback when the length cannot
be represented by Xt.

The focused regression is
`tests/automated/check-xmos-qualified-path-allocation.py`.

## Xmos cache reset allocation

Resetting the file-selection directory cache now checks the path-buffer
allocation before copying the qualified directory. If setup cannot be
completed, the caller disables caching for that scan and continues directory
enumeration without dereferencing a null path buffer.

Successful cache resets preserve the existing path and entry-cache behavior.

## XmRenderTable rendition-merge tag bounds

Rendition merging accepts an unsigned tag count. The reverse traversal now
counts down from that unsigned value instead of narrowing `tag_count - 1` to an
`int`, and a nonzero count requires a tag array. This preserves normal merge
ordering while avoiding implementation-defined large-count conversions and
null-array dereferences.

The source contract is checked by
`tests/automated/check-xmrender-merge-tag-bounds.py`.

## XmRenderTable property-buffer growth

Render-table serialization appends formatted properties to a dynamically
grown character buffer.  The previous macro assigned `XtRealloc`'s result
directly and did not enforce Xt's `Cardinal` size limit.

The macro now checks the limit, commits the replacement only after successful
allocation, and uses its existing cleanup return when growth fails.

## XmRenderTable rendition-array growth

Property conversion accumulates created renditions in a dynamic array.  The
old code grew that array without checking the new capacity or `XtRealloc`'s
result, then continued to append.

The initial allocation and each growth are now checked.  A failed expansion
uses the existing parser cleanup path, retaining the already parsed prefix.

## XmRenderTable resource-cache allocation

`XmRenderTable` caches quarkized resource names and the per-call `found`
flags while resolving rendition resources.  Initial allocation failures now
return cleanly, argument-driven quark-cache growth checks both host `size_t`
and Xt `Cardinal` byte limits before committing a replacement, and Xrm
search-list growth rejects overflow or allocation failure through the existing
cleanup path.  The compiled resource table also validates its byte count and
returns cleanly when its allocation fails.

The focused regression is
`tests/automated/check-xmrender-resource-cache-allocation.py`.

## XmScreen child-count bounds

`XmGetXmScreen` now keeps the display child count and scan index in Xt's
`Cardinal` type instead of narrowing them to `int`. Screen lookup behavior is
unchanged while large composite child lists remain representable.

## XmStack growth bounds

The internal XmStack grows by a fixed increment.  Its old push path formed
`top + 1` and `alloc + STACK_INC` in signed `int` fields without checking for
overflow.  A wrapped capacity could then produce a too-small allocation and
an out-of-bounds write.

Push now rejects invalid or exhausted counters, checks the new capacity in
both `size_t` and Xt's `Cardinal` byte-size range, and commits the capacity
only after `XtRealloc` succeeds.  The stack API and normal growth behavior
remain unchanged on platforms where the allocation is representable.

## XmString array-entry copy bounds

Copying an array entry allocates a pointer table for all child segments and
then recursively copies each child.  The old path multiplied the segment
count without a `size_t` bound and assumed every recursive copy succeeded.

The copy now checks the table byte count, handles allocation failure, and
frees already-copied children before returning.  Normal copies retain the
same entry layout and ownership behavior.

## XmString baseline allocation failures

`_XmStringGetBaselines` now validates its output pointers and checks the line
count against Xt's `Cardinal` allocation width before allocating baseline
arrays for single-line or multi-line strings. On failure it leaves a null
baseline array and reports zero lines; successful baseline values and ownership
remain unchanged.

## XmStringByteCompare conversion failure

`XmStringByteCompare` now checks both ASN.1 conversion results before calling
`memcmp` or releasing the buffers. A failed conversion returns `False` instead
of allowing null or undefined pointers to reach the comparison path. Normal
comparison and null-input behavior are unchanged.

The regression check is
`tests/automated/check-xmstring-byte-compare-failure.py`.

## XmString byte-compare length width

`XmCvtXmStringToByteStream` reports an `unsigned int` byte count.  The legacy
`XmStringByteCompare` helper previously stored those counts in `unsigned
short`, truncating compound strings larger than 65,535 bytes before comparing
them.  The local lengths now retain the converter's full width, so differences
after the old 16-bit boundary are not silently ignored.

`tests/automated/check-xmstring-byte-compare-width.py` protects this API
contract.

## XmString byte-stream allocation failures

`XmCvtXmStringToByteStream` now checks component-size arithmetic and the output
allocation before writing the encoded stream. On overflow or allocation failure
it clears the caller's output pointer and returns zero; successful encoding and
the size-only query remain unchanged.

## XmString byte-stream bounds validation

`XmCvtByteStreamToXmString` now validates every component header and declared
payload against the outer compound-string length. Truncated or inconsistent
streams return `NULL` instead of reading beyond the supplied byte stream;
well-formed streams retain their existing conversion behavior.

The regression check is
`tests/automated/check-xmstring-byte-stream-bounds.py`.

## XmString byte-stream length validation

`XmStringByteStreamLength` checks the three-byte compound-string ASN.1 identifier
before reading length bytes. Null input and a mismatched identifier return zero.
The API has no buffer-length argument; importers must establish that the encoded
header is available before calling it.

The returned size includes the header actually encoded in the input. Previously
the function recomputed the shortest header for the payload length, undercounting
long-form headers with payloads of 0–127 bytes by two. Property importers could
then advance to the wrong position or accept an outer length check that was too
small. Canonical short and long encodings retain their previous sizes; encoded
long headers for small payloads now return the correct size.

Component readers use the same rule. An external compound string may contain a
long-form component length field even when that component's payload is at most
127 bytes. Import, optimization, debug inspection, and truncation now advance
over the header actually present in the stream (two bytes for a short component
header or four bytes for a long one), rather than deriving its width from the
decoded payload length. This preserves canonical streams and valid
non-canonical long-form streams without shifting component payloads.

The regression check is
`tests/automated/check-xmstring-byte-stream-length.py`.

The executable production-function regression covers 266 cases: both outer-header
forms for all lengths 0–127, long-form lengths 128/255/256/65535, short and long
component headers for 0/127-byte payloads, invalid identifier and null input. It
also verifies process-lock balance under ASan/UBSan and records
source hashes, commands, timings and results in `receipt.json`:

```sh
python3 tests/automated/run-xmstring-stream-length-unit.py --out /absolute/fresh/results
```

## XmString charset comparison

`_XmStringIsCurrentCharset` treats a `NULL` charset tag as a non-match. This
keeps optional or malformed application metadata from reaching `strcmp` and
crashing the process, while preserving the existing comparison for valid
charset strings.

The focused regression check is
`tests/automated/check-xmstring-charset-null.py`.

## XmString optimized clone allocation

Optimized `XmStringCopy` clones now validate the computed payload size and
handle allocation failure before copying. This prevents arithmetic wraparound
and null-pointer dereferences for malformed or allocation-starved strings.

The focused regression check is
`tests/automated/check-xmstring-clone-allocation.py`.

## XmString malformed-state handling

The public component reader no longer uses assertions for malformed text types,
unknown states, or unreachable segment transitions. It marks the context
failed and returns `XmSTRING_COMPONENT_END`, allowing applications to handle
invalid compound strings without process termination.

The regression check is
`tests/automated/check-xmstring-component-assertions.py`.

## XmString component-copy allocation

`XmStringGetNextComponent` now checks the temporary buffer used when copying
text components.  Overflow or allocation failure marks the iterator error and
returns `XmSTRING_COMPONENT_END` instead of copying through a null or wrapped
buffer.  The existing zero-copy path and successful component results are
unchanged.

## XmString component insertion failure

`XmStringComponentCreate` now validates its final non-optimized segment
insertion. Allocation failure releases the partial compound string and returns
`NULL` instead of returning an empty or malformed result.

The regression check is
`tests/automated/check-xmstring-component-insertion.py`.

## XmString optimized rendition indices

`XmeStringGetComponent` now validates optimized rendition indices and cache
entries before indexing the shared tag cache. Malformed or stale values mark
the context failed and return `XmSTRING_COMPONENT_END` instead of asserting or
performing out-of-bounds pointer arithmetic.

The regression check is
`tests/automated/check-xmstring-component-rendition-index.py`.

## XmString component structure guards

`XmeStringGetComponent` now rejects malformed multi-entry strings with null
backing strings, lines, segments, or non-empty rendition arrays. The context is
marked failed and iteration ends before invalid fields are dereferenced.

The regression check is
`tests/automated/check-xmstring-component-structure.py`.

## XmString component tag-cache failures

`XmStringComponentCreate` now verifies that each charset, locale, and
rendition cache index resolves to the requested tag before constructing the
component. Cache allocation failure no longer aliases the default tag or
indexes a missing locale entry.

The regression check is
`tests/automated/check-xmstring-component-tag-cache.py`.

## XmString component text pointers

`XmeStringGetComponent` now rejects optimized and unoptimized segments whose
nonzero byte count has a null text pointer. This prevents direction detection,
copying, and text iteration from dereferencing malformed component storage.

The regression check is
`tests/automated/check-xmstring-component-text.py`.

## XmString optimized concatenation length bound

Optimized compound-string concatenation now checks the byte-count limit using
subtraction before adding the two unsigned lengths.  This prevents a wrapped
`a_len + b_len` from bypassing the compact-string limit and producing an
undersized result.

## XmString context-copy allocation

`_XmStringContextCopy` now checks both the size calculation and allocation of
the active rendition-tag array.  Counts are promoted before multiplication and
the byte count is bounded by the `Cardinal` allocator interface and passed
explicitly at the allocation boundary.  If the size is unrepresentable or the copy cannot allocate that optional array, the copied
context keeps its string and scalar state but exposes an empty active-rendition
set instead of overflowing or dereferencing a null pointer.

## XmString context initialization allocation

`XmStringInitContext` now checks its context-record allocation and returns
`FALSE` when the record cannot be created.  It no longer initializes through a
null pointer under allocation pressure.

## XmString context initialization guards

Context reinitialization now handles a null string without invoking optimized
representation access. Context copying also rejects a nonzero rendition count
with a null rendition array before copying it.

The regression check is
`tests/automated/check-xmstring-context-init.py`.

## XmString context utility null guards

Context reinitialization, copying, and cleanup now tolerate null or identical
context pointers without assertion termination. Valid context behavior is
unchanged; malformed cleanup paths become harmless no-ops.

The regression check is
`tests/automated/check-xmstring-context-null-utils.py`.

## XmString context rendition growth

Active-rendition growth is now transactional.  The context checks count and
`Cardinal` size arithmetic, retains the existing tag array when `XtRealloc`
fails, and marks the context error instead of copying through a null or
wrapped buffer.

## XmString conversion guard

`WmXmStringToString` now checks `XmStringInitContext` before reading segments
or freeing the context.  Invalid or uninitializable compound strings return
`NULL` safely; valid strings retain the existing conversion behavior.

`tests/automated/check-xmstring-conversion-guards.py` protects the contract.

## String-to-`XmString` converter input validation

The compound-string converter now returns `False` when its source or
destination `XrmValue` is missing, avoiding an invalid dereference. Valid
string generation and converter cleanup behavior remain unchanged.

The focused regression check is
`tests/automated/check-xmstring-converter-inputs.py`.

## XmString creation insertion failure

The non-optimized `_XmStringNCreate` path now verifies that its initial
segment insertion succeeded. Allocation failure returns `NULL` after freeing
the partial string instead of exposing an empty or malformed compound string.

The regression check is
`tests/automated/check-xmstring-create-insertion.py`.

## XmString creation tag-cache failures

`_XmStringNCreate` now verifies that the tag-cache index resolves to the
requested tag and that non-optimized tag caching succeeds. Cache allocation
failure no longer silently substitutes the default charset or returns a
segment with a null tag.

The regression check is
`tests/automated/check-xmstring-create-tag-cache.py`.

## XmString debug allocation

When `_XmDEBUG_XMSTRING_MEM` is enabled, XmString storage reserves a prefix
for allocation diagnostics. The allocator now checks the size before adding
that prefix and returns `NULL` when `XtMalloc` fails, rather than performing
pointer arithmetic on a null result. The matching debug free helper also
ignores null pointers before removing the prefix. Normal and debug allocation
interfaces remain private and unchanged for callers.

The focused contract is covered by
`tests/automated/check-xmstring-debug-allocation.py`.

Compound-string rendition copying also validates its output pointer before
initializing it, so malformed internal calls fail cleanly instead of
dereferencing `NULL`.

## XmString direction-component copy allocation

`XmStringGetNextComponent` now checks both direction-component copy buffers.
If a caller requests copied direction data and allocation fails, the iterator
marks its error and returns `XmSTRING_COMPONENT_END` instead of dereferencing
a null `XtNew` result.

## XmString implicit direction-pattern allocation

`XmStringParseText` now checks creation of its cached implicit direction
mapping. If allocation fails, the initial result is freed and parsing returns
`NULL` before the missing mapping can be dereferenced.

The regression check is
`tests/automated/check-xmstring-direction-pattern.py`.

## XmString entry-conversion allocation

`EntryCvtToOpt` and `EntryCvtToUnopt` now check their new-entry, text, and
rendition-array allocations. Any failed conversion releases the partial entry
and returns `NULL`, allowing callers to keep the original segment intact.

## XmString entry-copy index width

Compound `XmString` entries store their segment count in a size-compatible
allocation path.  Entry copying now uses a `size_t` loop index for that count,
instead of narrowing it to `int` after allocation.  This keeps large
representable segment arrays fully traversable while preserving normal copy and
cleanup behavior.

`tests/automated/check-xmstring-entry-index-width.py` protects this contract.

## XmString entry text input validation

The internal `_XmEntryTextSet` helper now treats a null entry as a no-op and a
null value for an immediate segment as an empty string. Pointer-backed and
unoptimized segments retain their existing ownership behavior. This prevents
the immutable-segment path from passing a null source to `strcpy`.

The focused source check is:

```sh
python3 tests/automated/check-xmstring-entry-text-input.py
```

## XmString entry-type fallback

Internal XmString entry helpers now treat null or unknown entry types as empty
data. Segment extraction stops safely for an unexpected component type. Valid
optimized, array, and unoptimized entries retain their existing behavior.

The regression check is
`tests/automated/check-xmstring-entry-type-fallback.py`.

## XmStringExtent output-pointer guards

`XmStringExtent` now validates its `width` and `height` output pointers before
writing them. A call with either pointer missing returns safely; valid calls
still receive the same dimensions, and non-null outputs are initialized to zero
before other inputs are checked.

The regression check is
`tests/automated/check-xmstring-extent-output.py`.

## XmString rendition wrapping allocation

`XmStringGenerate` now checks conversion and growth while wrapping rendition
begin/end components around an unoptimized string.  If a segment conversion or
tag-array growth fails, it unlocks and returns the original string without
publishing partial rendition state.

## XmStringGenerate fallback rendition decoration

When rendition caching cannot use the optimized representation,
`XmStringGenerate` now checks both rendition-component allocations and both
concatenations. Any failure frees the partial result and returns `NULL` rather
than returning an undecorated string.

The regression check is
`tests/automated/check-xmstring-generate-rendition-fallback.py`.

## XmStringGenerate rendition cache failures

Rendition decoration is now transactional. If the requested rendition cannot
be resolved in the tag cache, `XmStringGenerate` frees the generated result
and returns `NULL` instead of attaching a null rendition or silently changing
the output.

The regression check is
`tests/automated/check-xmstring-generate-rendition.py`.

## XmStringGenerate parse failure

`XmStringGenerate` now returns immediately when `XmStringParseText` fails,
before inspecting or decorating the result with a rendition. This prevents a
null representation dereference while preserving successful generation.

The regression check is
`tests/automated/check-xmstring-generate-result.py`.

## Generated XmString parse-table cleanup

The internal generated parse table now releases each temporary newline
substitute and rolls back the cached table if either mapping allocation fails.
This prevents a permanent null-entry table and removes the repeated-call leak.

The regression check is
`tests/automated/check-xmstring-generated-table-cleanup.py`.

## XmString generated parse-table publication

The parse mappings used by `XmStringGenerate` and the internal reverse helper
are now built in a private candidate table. The shared pointer is published
only after both mappings and their substitutes have been validated. This
preserves the existing lazy initialization and API behavior while preventing a
concurrent caller from using a partially initialized table.

The regression check is
`tests/automated/check-xmstring-generated-table-publication.py`.

## Generated XmString parse-table substitutes

Generated tab and newline mappings now require successful temporary compound
strings and copied substitutes before entering the shared cache. Failed
substitute allocation rolls back the partial table instead of caching a
mapping that can never be applied.

The regression check is
`tests/automated/check-xmstring-generated-table-substitutes.py`.

## XmString context helper guards

`XmStringGetNextComponent` and `XmStringPeekNextComponent` now reject a null
`XmStringContext` with `XmSTRING_COMPONENT_END`. This keeps invalid or already
released contexts from reaching the internal component reader while preserving
normal iteration behavior.

The regression check is
`tests/automated/check-xmstring-get-context-guard.py`.

## XmStringGetLtoR output initialization

`XmStringGetLtoR` now clears its text output as soon as the output pointer is
validated. Null strings or tags therefore leave `*text` as `NULL` instead of
returning with a stale caller-owned pointer.

The regression check is
`tests/automated/check-xmstring-getltor-output.py`.

## XmString immediate-segment copies

Immediate optimized segments store exactly their declared `byte_count` bytes;
the compact representation does not reserve a trailing C-string terminator.
The old `_XmEntryTextSet` path used `strcpy`, which could write one byte past a
compact allocation and treated wide-character payloads as narrow strings.

The setter now copies exactly the declared byte count with `memcpy`.  This
preserves the existing representation and supports both narrow and wide text
without an overrun.

The regression check is `tests/automated/check-xmstring-immediate-copy.py`.

## XmParseMapping substitute replacement

`XmParseMappingSetValues` now replaces an existing substitute transactionally:
the old compound string is freed after a new copy succeeds. Clearing a
substitute is supported, while copy failure preserves the previous value
instead of leaking or partially updating it.

The regression check is
`tests/automated/check-xmstring-mapping-substitute.py`.

## XmString null parse patterns

`match_pattern` now treats mappings with a null pattern as non-matches after
handling the special direction-change marker. Invalid mapping input no longer
reaches wide-character dereferences or `strlen`.

The regression check is
`tests/automated/check-xmstring-match-pattern.py`.

## XmString multibyte conversion

`_XmGetMBStringFromXmString` now sizes its output with `size_t` and copies
components by length rather than repeatedly concatenating into an allocation
calculated with a signed `int`. Component-length overflow is rejected before
allocation, while ordinary compound-string conversion remains unchanged.

## XmStringNConcat allocation failure

The legacy `XmStringNConcat` wrapper now checks the result of its initial
`XmStringConcat` before measuring and copying it. If concatenation fails, it
returns `NULL` immediately instead of passing a null result to the length and
bounded-copy helpers.

The regression check is
`tests/automated/check-xmstring-nconcat-failure.py`.

## XmStringNCopy input validation

`XmStringNCopy` now rejects a null source string or negative byte limit before
attempting ASN.1 conversion. This avoids passing invalid input to truncation
and conversion helpers while leaving valid bounded-copy behavior unchanged.

The regression check is
`tests/automated/check-xmstring-ncopy-input.py`.

## XmString substring line allocation

Substring construction now creates line entries and grows the line array
transactionally. Allocation failure is reported through the string context and
the partial result is released instead of being dereferenced or published.

The focused regression check is
`tests/automated/check-xmstring-new-line-allocation.py`.

## XmString newline entry-count bounds

Newline insertion appends one entry to an internal `int`-counted array.  It
now rejects an existing `INT_MAX` entry count before allocating or incrementing,
avoiding signed overflow while preserving normal newline insertion.

`tests/automated/check-xmstring-newline-count.py` protects the guard.

## XmString next-segment input validation

`XmStringGetNextSegment` now rejects a null context or output pointer before
entering the segment conversion path. Valid callers and allocated text/tag
ownership remain unchanged; malformed calls return `False` without touching
the output objects.

## Legacy XmString constructor failure handling

`XmStringLtoRCreate` and `XmStringSegmentCreate` now stop and return `NULL`
when a component or separator concatenation fails. Previously a failed
allocation could be followed by another concat, allowing the constructor to
return a later fragment while silently losing the earlier text. The public
legacy APIs and successful-path behavior remain unchanged.

The regression check is
`tests/automated/check-xmstring-obsolete-concat-failure.py`.

## XmString optimized-entry copy bounds

Optimized XmString entries can carry inline text whose byte count is stored
in the entry.  The copy path previously formed the allocation size in `int`
and then copied through unchecked allocation results.  Large values could
wrap before the allocation or cause a null-pointer copy after allocation
failure.

The path now computes the size in `size_t`, rejects values that exceed Xt's
`Cardinal` allocation contract, and releases a partially built entry when
out-of-line text allocation fails.

## XmString parser failure handling

`XmStringParseText` now tolerates null mapping slots while scanning direction
patterns and aborts immediately when a substitution concatenation fails. This
prevents null mapping dereferences and avoids continuing with a lost partial
result.

The regression check is
`tests/automated/check-xmstring-parse-failure.py`.

## XmString parse-mapping allocation failures

`XmParseMappingCreate` now checks its `XtNew` result before initialization.
When allocation fails it returns `NULL`, matching the existing setter's
failure-safe behavior and avoiding a null dereference during parser setup.

## XmString parse-mapping input validation

`XmParseMappingSetValues` now ignores invalid calls with a nonzero argument
count and no argument list, and skips entries whose argument name is null.
Valid mappings retain their existing updates; malformed entries no longer reach
`strcmp` through a null pointer.

## XmString parser substitute-copy failure

When a non-procedure parse mapping has a substitute, `XmStringParseText` now
aborts if copying that substitute fails. The partial result is freed rather
than silently omitting the requested replacement; parse procedures that
intentionally return no insertion remain unchanged.

The regression check is
`tests/automated/check-xmstring-parse-substitute-copy.py`.

## XmString parse-table allocation failures

The lazy parse table shared by `XmStringGenerate` and `_XmStringUngenerate`
now validates its output pointer and checks the table allocation before
publishing it. Allocation failure returns a zero table size with a null output
table, allowing the caller's normal conversion failure path to run without a
null dereference.

## XmString parser rendition-array sizing

The non-optimized compound-string parser now allocates rendition-begin and
rendition-end arrays using element count multiplied by `sizeof(XmStringTag)`.
The previous count-only allocation under-allocated on normal 64-bit builds;
size and allocation failures now return `NULL` before the array is written.

## XmString active-rendition bookkeeping

Active rendition updates now reject null source lists and inconsistent context
storage. Failed bookkeeping marks the context failed instead of copying from
or traversing null pointers.

The regression check is
`tests/automated/check-xmstring-rendition-bookkeeping.py`.

## XmString rendition-end tag bounds

Compound-string rendering removes rendition-end tags by scanning the active
tag list in reverse. The tag count is unsigned, so the scan now counts down
without narrowing `tag_count - 1` into a signed loop index. Normal rendition
ordering and end-tag removal are unchanged, while large counts no longer risk
implementation-defined conversion or loop underflow.

The source contract is checked by
`tests/automated/check-xmstring-rendition-end-bounds.py`.

## XmString rendition entry validation

`XmeStringGetComponent` now rejects null individual rendition tags even when
the surrounding rendition arrays are present. Iteration ends cleanly before
copying, measuring, or updating active rendition state.

The regression check is
`tests/automated/check-xmstring-rendition-entry.py`.

## XmString rendition merge allocation failures

Concatenating XmStrings can merge rendition begin and end tag arrays.  The
merge helpers previously stored `XtRealloc`'s return value immediately and
then copied the second array into it.  Allocation failure could therefore
turn a valid rendition into a null-pointer write.

The helpers now compute the combined count in `int`, check its conversion to
an allocation byte count including Xt's `Cardinal` limit, and assign the new
pointer only after `XtRealloc` succeeds.  A failed merge leaves the original
tag array and count unchanged.

## XmString rendition-stack extents

Segment extent calculation maintains a temporary stack of rendition tags.
The old begin/end paths assigned `XtRealloc` results directly before copying
or compacting the stack.

The paths now check the combined tag count against both host `size_t` and Xt's
`Cardinal` byte limits, and commit growth only after success. A failed shrink
keeps the existing larger buffer, which remains valid for the reduced tag
count.

## XmString rendition-tag growth

Rendition begin/end tag arrays grow from an internal `int` count. The growth
paths now reject a count that cannot be incremented or whose resulting byte
size cannot be represented by either `size_t` or Xt's `Cardinal`, and perform
the multiplication only after those checks. Existing tag ordering and public
XmString behavior are unchanged for representable inputs.

The focused regression is
`tests/automated/check-xmstring-rendition-tag-growth.py`.

Rendition accessors also reject negative indices before touching the internal
tag arrays. This keeps malformed internal requests from indexing before an
array (or from starting a removal loop at `-1`) while leaving all valid
indices and public XmString behavior unchanged. The focused bounds regression
is `tests/automated/check-xmstring-rendition-tag-bounds.py`.

The ASN.1 parser applies the same pre-increment `INT_MAX` guard before growing
begin/end rendition arrays, so a malformed stream cannot wrap its count before
the allocation-width checks. `tests/automated/check-xmstring-parser-rendition-size.py`
covers that parser contract.

## RTL compound-string scratch allocation

The RTL compound-string renderer now returns safely when its temporary buffer
allocation fails, instead of writing through a NULL pointer. Successful RTL
rendering and existing error returns remain unchanged.

The focused regression is
`tests/automated/check-xmstring-rtl-allocation.py`.

## XmString segment-length bounds

`_XmStringGetSegment` exposes component lengths through the legacy `short`
output used by Motif's segment APIs. It now rejects components larger than
`SHRT_MAX` before assigning that output, preventing unsigned lengths from
wrapping negative and reaching allocation or copy code. Existing representable
segments retain their behavior.

The regression check is
`tests/automated/check-string-segment-copy-guards.py`.

## XmString self-concatenation

`XmStringConcatAndFree` now handles the same XmString passed as both operands
by taking a second reference before any in-place conversion. This preserves the
function’s ownership contract and prevents aliasing-induced mutation or
assertion failure; distinct operands behave as before.

The regression check is `tests/automated/check-xmstring-self-concat.py`.

## XmString separator allocation

`XmStringSeparatorCreate` now checks its cached entry table and each of the
two optimized entries.  A partial cache is released and `NULL` returned on
allocation failure, rather than initializing or cloning through null storage.

## XmString single-segment allocation handling

`_XmStringSingleSegment` now validates copied tag, locale, and text component
results.  Failed component copies or default-tag allocation return `FALSE`
and clear the partial outputs instead of reporting a segment with a null
payload.

## XmString tab conversion guards

The internal tab conversion helper now validates its display, screen-storage,
and drawable inputs before querying X. Invalid input returns zero pixels;
normal screen-aware tab conversion is unchanged.

The regression check is `tests/automated/check-xmstring-tab-conversion.py`.

## XmString-table conversion allocation

The XmString-table resource converter now checks each per-item string
allocation's input length against Xt's `Cardinal` range, then passes the
accepted size explicitly. Initial table allocation, growth reallocation, and
generated XmString creation are also checked. Any failure releases the
partial table and strings before returning `False`; successful conversion and
normal destroy ownership remain unchanged.

## XmString table count bounds

The public XmString table conversion helpers allocate arrays from caller or
content-derived `Cardinal` counts.  The table-size products now use `size_t`
checks and reject values that cannot be represented by the Xt allocator's
`Cardinal` byte-count interface.  Content-derived splitting also uses a wide
counter and returns an empty result instead of overflowing an `int` counter.
The public parse, unparse, and concatenation loops use `Cardinal` indices to
match their full input count range, avoiding signed-index wraparound for large
but representable tables. Normal table conversions and their ownership rules
are unchanged.

## XmString table growth bounds

The string-to-`XmString` resource converter grows its pointer table as it
parses comma-separated entries.  Capacity doubling now checks for `int`
overflow first and passes the checked initial, growth, and terminating table
sizes explicitly as Xt `Cardinal` byte counts.  If the limit is reached,
already-created strings and the table are released and conversion fails
instead of wrapping the capacity and writing past the allocation.

## XmString-table conversion rejects invalid multibyte input

The shared `GetNextXmString` parser now reports an invalid locale multibyte
sequence as a failed component instead of returning a truncated component to
`CvtStringToXmStringTable`. Its temporary component is freed and set to `NULL`,
so the existing table-conversion rollback path releases earlier components and
does not publish partial Unicode data.

The focused source-contract check is
`tests/automated/check-string-table-converter-allocation.py`.

## XmString-table parser input validation

The internal `GetNextXmString` parser now rejects missing state pointers before
reading the input cursor, output component, or delimiter state. This keeps a
malformed conversion request fail-closed while preserving the existing parser
and rollback behavior for valid XmString tables.

The focused source-contract check is
`tests/automated/check-string-table-converter-allocation.py`.

## XmString table component pointers

`XmStringToXmStringTable` now checks both component pointers before comparing
non-empty component data with `memcmp`.  Empty components retain their existing
matching behavior, while malformed non-empty components no longer cause a NULL
dereference.

`tests/automated/check-xmstring-table-pointers.py` protects both match loops.

## XmString tab-list traversal failure

`XmStringTableProposeTablist` now checks the string context after each input
has been scanned. If traversal reports an internal representation error, the
partial tab list is released and the function returns `NULL`; callers no
longer receive a silently incomplete proposal.

The regression check is
`tests/automated/check-xmstring-tablist-context-failure.py`.

## XmString tag-cache input validation

`_XmStringIndexCacheTag` now rejects null tags and negative lengths before
locking, comparing, or allocating cache entries. It returns the existing
sentinel index `0` for invalid input, preventing null string operations and
oversized length conversions.

The regression check is
`tests/automated/check-xmstring-tag-cache-input.py`.

## XmString tag-cache bounds and growth

The internal tag cache uses a zero-based index and `_cache_count` as its
element count.  The lookup helper previously accepted `_cache_count` itself,
which is one past the last element.  Cache initialization and growth also
used unchecked allocation results.

Lookups now reject negative and out-of-range indices. Initial and expanded
cache storage uses explicit Xt `Cardinal` byte sizes and is committed only
after allocation succeeds, and the public internal cache helper returns `NULL`
when no tag can be stored. Existing valid tag indices and string
representations are unchanged.

## XmString tag length bounds

The compatibility tag-cache API keeps its historical `int` length parameter,
but sentinel-length calls derive that value from `strlen`.  The cache now
checks the measured `size_t` length against both `INT_MAX` and Xt’s allocation
range before converting it, rejecting unrepresentable tags without wrapping.
The resulting allocation also widens `length + 1` before arithmetic, so the
largest representable legacy length cannot trigger signed overflow.

`tests/automated/check-xmstring-tag-length.py` protects the conversion guard.

## XmString text concatenation allocation

`_XmStringGetTextConcat` now checks output-length arithmetic and the temporary
result of `XtRealloc` before copying a component.  A failed or unrepresentable
growth releases the partial output and context state and returns `NULL`.

## XmString empty text concatenation

`_XmStringGetTextConcat` now skips `memcpy` for zero-length text components.
Empty components with null values remain valid, without relying on undefined
zero-size null-pointer copy behavior.

The regression check is
`tests/automated/check-xmstring-text-concat-empty.py`.

## XmString text concatenation null values

`_XmStringGetTextConcat` now rejects malformed non-empty text components whose
value pointer is `NULL`. It releases the partial output and string context
before returning `NULL`, avoiding a crash in `memcpy` while preserving normal
empty-component behavior.

The regression check is
`tests/automated/check-xmstring-text-concat-null-value.py`.

## XmString triple-reader context guard

`XmeStringGetComponent`, the shared implementation behind
`XmStringGetNextTriple`, now rejects a null context before reading context
state. It returns `XmSTRING_COMPONENT_END` just like other invalid-output
cases, preventing a null dereference for malformed or already-released
contexts.

The regression check is
`tests/automated/check-xmstring-triple-context.py`.

## XmString ASN.1 truncation boundaries

`_XmStringTruncateASN1` now checks the component pointer against the encoded
string end before reading a component header. This prevents an out-of-bounds
read when the final component ends exactly at the truncation boundary while
preserving the existing short-header compaction behavior. It also rejects
component lengths that would extend beyond the encoded buffer before calling
the component reader.

The focused regression check is
`tests/automated/check-xmstring-truncate-boundary.py`.

## XmString ASN.1 truncation overflow

`_XmStringTruncateASN1` now checks the accumulated component size before
adding it to the 16-bit legacy length accumulator. An oversized compound
string therefore fails cleanly instead of wrapping `used` and resizing to an
undersized buffer. Existing valid encodings and truncation behavior are
unchanged.

The regression check is
`tests/automated/check-xmstring-truncate-overflow.py`.

## XmString ASN.1 truncation allocation failures

`_XmStringTruncateASN1`, used by `XmStringNCopy`, now checks both replacement
and resizing allocations before copying or rewriting the byte stream. On
failure it releases the incomplete stream and returns `NULL`, allowing the
public copy operation to fail cleanly without dereferencing an invalid buffer.

## XmString unoptimization allocation

The non-destructive `Unoptimize` path now checks its segment-array size and
each converted/copied segment.  Failed conversion releases all partial
segments and the new array entry, returning `NULL` without modifying the
source string.

## XmString unoptimized-entry copy bounds

Unoptimized XmString entries own optional text and rendition begin/end tag
arrays.  The copy path previously copied the source pointers into the new
entry and then allocated replacement arrays without checking the count or
allocation result.  Failure could expose source ownership to cleanup or
leave a null array for the subsequent copy loop.

The new entry clears its owned array pointers before allocation, checks each
count against both host `size_t` and Xt's `Cardinal` byte limits, and releases
the partially built entry on failure. Normal copy ownership and representation
remain unchanged.

## XmString unparse allocation safety

`unparse_text` now checks temporary conversion buffers, result growth, and
length arithmetic before copying or converting a component.  A failed or
unrepresentable component is skipped without replacing the existing result
buffer, so malformed or allocation-limited compound strings do not cause a
null dereference or wrapped allocation.

## XmString unparse comparison values

Unparse pattern comparison now treats non-empty components with null values as
non-matches before calling `memcmp`. Malformed compound strings no longer crash
the parser while comparing substitution patterns.

The regression check is
`tests/automated/check-xmstring-unparse-compare.py`.

## XmString unparse context-end guard

Unparse pattern matching now treats an unexpected end of the source context as
a malformed-input boundary. It stops skipping matched components without an
assertion failure; valid strings and parse mappings retain their existing
behavior.

The regression check is
`tests/automated/check-xmstring-unparse-context-end.py`.

## XmString unparse length bounds

XmString unparse conversion keeps its public `int` length contract, but the
locale conversion routines return `size_t`. The implementation now checks
conversion failure and representability before adding byte counts to the
output length, preventing truncation or signed overflow for very large
compound strings.

`tests/automated/check-xmstring-unparse-length.py` is the focused source
regression for the conversion totals.

## XmString unparse null values

The unparse helper now ignores malformed non-empty components with null value
pointers before conversion or copying. This avoids a crash while retaining the
existing partial-output behavior used for allocation and conversion failures.

The regression check is
`tests/automated/check-xmstring-unparse-null-value.py`.

## XmString unparse output validation

`XmStringUnparse` now accepts only the documented charset, multibyte, wide
character, and no-text output types. Invalid values return `NULL` before any
component processing, so callers cannot receive an unterminated partial result.

The regression check is `tests/automated/check-xmstring-unparse-output.py`.

## XmStringUnparse parse-table validation

`XmStringUnparse` now rejects a nonzero parse count with a null table, and its
component matcher skips null mapping entries. Invalid parse-table inputs no
longer dereference mapping state while valid tables behave as before.

The regression check is
`tests/automated/check-xmstring-unparse-table.py`.

## XPM array end-of-input guard

Array-backed XPM data is a null-terminated list of strings.  The scanner now
checks the input object, array pointer, and next line pointer before
dereferencing them.  Malformed or truncated arrays and buffers therefore
terminate parsing as invalid input instead of turning a missing line into a
null-pointer read.

Regression: `tests/automated/check-xpm-array-end-guard.py`

## XPM decimal-parser null guards

The internal XPM decimal parser now rejects null input and output pointers
before scanning or storing a value, preventing malformed parser calls from
dereferencing invalid addresses.

Regression: `tests/automated/check-xpm-atoui-null-guards.py`

## XPM color-metadata bounds

`XpmCreateXpmImageFromImage` now validates caller-supplied color-table
metadata before scanning pixels.  When `XpmColorTable` or legacy `XpmInfos`
is requested, a non-empty table requires both the color and pixel arrays, and
an advertised transparent-color index must be inside `ncolors`.  Invalid
metadata returns `XpmFileInvalid` before any dereference while valid exported
color tables retain their existing behavior.

Regression: `tests/automated/check-xpm-attribute-color-bounds.py`

## XPM bitmap format input

Both XPM image-conversion paths now accept only `XYBitmap` or `ZPixmap` when
`XpmBitmapFormat` is explicitly supplied.  Invalid format values return
`XpmFileInvalid` before X11 image construction; omitted and valid format
settings retain their existing behavior.

Regression: `tests/automated/check-xpm-bitmap-format.py`

## XPM buffer color-token width

The XPM buffer serializer now rejects a zero `cpp` value and any color table
entry whose primary token is missing or shorter than the declared token width.
Pixel rows use the same validation and copy exactly the declared number of
bytes, avoiding `strncpy` padding from malformed input.

Regression: `tests/automated/check-xpm-buffer-color-token-width.py`

## XPM buffer comment formatting

Pixel comments in the XPM buffer serializer now check `snprintf` errors and
capacity before advancing the output cursor. This keeps malformed or
unexpectedly long comment data from desynchronizing the subsequent pixel and
extension writes.

Regression: `tests/automated/check-xpm-buffer-comment-format.py`

## XPM buffer zero-`cpp` guard

XPM buffer serialization now rejects an image with zero characters per pixel
before checking `width * cpp`, preventing malformed metadata from reaching a
divide-by-zero operation.

Regression: `tests/automated/check-xpm-buffer-cpp-guard.py`

## XPM buffer format bounds

XPM buffer headers now use bounded `snprintf` formatting with unsigned
dimension specifiers. This removes the prior signed-format mismatch and
rejects an unexpectedly truncated comment or values line. The serializer also
checks the remaining capacity before writing its final `};` terminator, so a
malformed or internally inconsistent size calculation fails cleanly instead of
writing past the allocation.

Regressions: `tests/automated/check-xpm-buffer-format-bounds.py` and
`tests/automated/check-xpm-buffer-final-terminator-bounds.py`

## Validate XPM buffer-export arguments

Pixmap-, image-, and raw-image-to-buffer constructors now reject null output
records and empty source inputs before X11 extraction or buffer allocation.
Shape-only conversion remains supported, and optional `XpmInfo` metadata is
unchanged.

## XPM in-memory string length

The buffer-backed `xpmGetString` path now checks the scanned span before
converting it to the library’s `unsigned int` length and allocating the
record. Oversized in-memory records return `XpmNoMemory` safely.

Regression: `tests/automated/check-xpm-buffer-string-length.py`

## XPM buffer total guard

XPM buffer serialization now checks the header, extension, and comment sizes
before adding them, preventing a wrapped total from reaching allocation and
copy operations.

Regression: `tests/automated/check-xpm-buffer-total-guard.py`

## XPM color callback attributes

The raw XPM-image conversion path now rejects `XpmAllocColor` and
`XpmFreeColors` attribute flags when their callbacks are null.  This prevents
malformed caller state from reaching a null function-pointer call during color
allocation or cleanup; complete callback configurations retain the existing
custom-color behavior.

Regression: `tests/automated/check-xpm-color-callbacks.py`

## XPM color helper input guards

The private XPM color helpers now return safely when their screen, color
record, or output symbol array is absent. Valid color-object processing is
unchanged; invalid optional state no longer reaches field assignments or X11
color queries.

The focused regression is `tests/automated/check-xpm-color-input-guards.py`.

## XPM masked color-table validation

`ScanOtherColors` now validates color and pixel arrays before applying the
transparent-mask offset.  An empty masked table can no longer underflow its
unsigned color count or advance null pointers; malformed non-empty inputs are
also rejected before allocation and lookup.

The regression check is `tests/automated/check-xpm-color-mask-count.py`.

## XPM color-symbol attributes

The raw XPM-image conversion path now rejects `XpmColorSymbols` with a
nonzero symbol count but no symbol array.  This prevents malformed caller
state from reaching the color-symbol scan and preserves the existing behavior
for empty or fully populated symbol lists.

Regression: `tests/automated/check-xpm-color-symbols.py`

## XPM color-table input guard

XPM array and buffer serialization now reject a nonzero color count paired
with a null color table before iterating the table.  This keeps malformed
caller input on the normal `XpmFileInvalid` path instead of dereferencing a
null pointer.

Regression: `tests/automated/check-xpm-color-table-input.py`

## XPM color-token width

XPM color-table serialization now receives the declared `cpp` width and
rejects primary color tokens shorter than that width before writing them.

Regression: `tests/automated/check-xpm-color-token-width.py`

## XPM color-write status

XPM file output now validates the color table and each primary color token
before dereferencing them. `WriteColors` returns allocation/input/stream status
to the top-level writer instead of silently continuing after malformed input.

Regression: `tests/automated/check-xpm-color-write-status.py`

## XPM returned-comment status

When comment-return mode is enabled, all three comment extraction calls in
both the parser and image-create paths now propagate allocation failures
through their cleanup paths.  Neither path reports success with silently
missing comment data after `XpmNoMemory`.

Regression: `tests/automated/check-xpm-comment-status.py`

## XPM compressed-file fallback bounds

The z-pipe implementation now includes the POSIX declarations it uses. When
an XPM file is not found and compressed suffixes are tried, the fallback
filename allocation also checks `strlen(filename) + 4` before allocating or
writing the `.Z`/`.gz` suffix.

Regression: `tests/automated/check-xpm-compressed-name-bounds.py`

## XPM automatic chars-per-pixel growth

When an XPM image does not provide enough characters per pixel for its color
count, the scanner derives the minimum width by repeatedly multiplying the
printable-character capacity.  That multiplication is now checked before it
can wrap an unsigned counter.  An unrepresentable color count returns
`XpmNoMemory` through the existing cleanup path instead of looping with a
corrupted capacity.

Regression: `tests/automated/check-xpm-cpp-growth.py`

## XPM data color-token width

The `XpmImage`-to-data serializer now rejects zero `cpp` and missing or short
primary color tokens in both the color table and pixel rows. Valid tokens are
copied exactly, avoiding `strncpy` padding from malformed input.

Regression: `tests/automated/check-xpm-data-color-token-width.py`

## XPM data dimension entry guard

`XpmCreateDataFromXpmImage` now rejects zero-width or zero-height images before
allocating its header table.  This prevents invalid input from reaching the
header-pointer copy, which assumes at least one pixel row is available.

Regression: `tests/automated/check-xpm-data-dimension-entry.py`

## XPM data extension inputs

The data serializer now validates extension output pointers, the extension
array, names, line arrays, and individual lines before measuring them. Empty
extension sets remain valid and produce zero size/count outputs.

Regression: `tests/automated/check-xpm-data-extension-inputs.py`

## XPM data extension write status

Extension serialization now returns a status. The data builder checks its
formatted extension records and rejects missing metadata or formatting
failures instead of silently returning partially written data.

Regression: `tests/automated/check-xpm-data-extension-write-status.py`

## XPM data serialization failure cleanup

`XpmCreateDataFromXpmImage` now owns its final data block until serialization
has succeeded.  Pixel or extension failures release that block through the
common exit path; successful calls transfer it to `data_return` and clear the
local owner so it is not freed twice.

Regression: `tests/automated/check-xpm-data-failure-cleanup.py`

## XPM data format bounds

XPM data serialization now uses checked `snprintf` calls for dimensions and
hotspots, with unsigned dimension formats and an explicit extension-literal
capacity check.

Regression: `tests/automated/check-xpm-data-format-bounds.py`

## Validate XPM image-to-data arguments

`XpmCreateDataFromImage` now rejects a null display, output-array pointer, or
both image inputs before conversion.  A shape-only conversion remains valid,
matching the lower-level image scanner's existing behavior.

## Validate XPM data-export arguments

`XpmCreateDataFromXpmImage` now rejects a null output-array pointer or source
image before writing through the output or reading image metadata.  Optional
`XpmInfo` metadata remains supported exactly as before.

## Validate XPM pixmap-to-data inputs

`XpmCreateDataFromPixmap` now rejects a null display, output-array pointer, or
empty pixmap/shape-mask pair before attempting X11 extraction.  Shape-mask-only
conversion remains supported.

## XPM default-color allocation

When XPM attributes provide default color strings, scanning duplicates those
strings into the output color record. A failed duplication now releases the
temporary pixel data and RGB-name table and returns `XpmNoMemory` instead of
leaving a partially initialized record.

The source contract is covered by
`tests/automated/check-xpm-default-allocation.py`.

## XPM explicit depth input

Both XPM image-conversion paths now reject an `XpmDepth` attribute with value
zero before passing it to X11 image construction.  Calls that omit the flag
continue to use the display's default depth, and valid explicit depths retain
their existing behavior.

Regression: `tests/automated/check-xpm-depth-input.py`

## XPM extension error status

Malformed extension metadata is caller input, not an allocation failure.  The
array and buffer serializers now report `XpmFileInvalid` when extension names
or line arrays are missing or inconsistent; genuine allocation failures retain
their existing `XpmNoMemory` status.

Regression: `tests/automated/check-xpm-extension-error-status.py`

## XPM extension growth bounds

Extension and extension-line arrays now check count-plus-one multiplication
before `XpmRealloc`, and extension counts stay representable by the public
`int` cleanup API. Initial extension allocation failure is also reported
immediately, preventing wrapped sizes or reliance on allocator-specific
`realloc(NULL, ...)` behavior.

Regression: `tests/automated/check-xpm-extension-growth-bounds.py`

## XPM extension-line initial allocation

The parser now checks the initial extension-line pointer-array allocation and
reports `XpmNoMemory` immediately, rather than relying on
`XpmRealloc(NULL, ...)` behavior.

Regression: `tests/automated/check-xpm-extension-lines-initial-allocation.py`

## XPM extension-name termination

Parsed `XPMEXT` names are now length-validated, allocated with space for the
terminating NUL, and explicitly terminated. Malformed records with an invalid
name span are rejected before allocation.

Regression: `tests/automated/check-xpm-extension-name-termination.py`

## XPM extension-parser entry validation

`xpmParseExtensions` now rejects null parser state and output pointers with
`XpmFileInvalid` before scanning or allocating extension records.

Regression: `tests/automated/check-xpm-extension-parser-entry.py`

## XPM extension-write status

XPM file output now validates extension arrays, names, line arrays, and line
strings before serialization. `WriteExtensions` returns input and stream
errors to the top-level writer.

Regression: `tests/automated/check-xpm-extension-write-status.py`

## XPM file-buffer arguments

The convenience APIs for reading a file into a buffer and writing a buffer to
a file now reject null filenames, output pointers, and input buffers before
calling libc file functions.

The source contract is covered by
`tests/automated/check-xpm-file-buffer-arguments.py`.

## XPM file-to-data output

`XpmReadFileToData` requires a destination pointer for its allocated data
table. A null output pointer is rejected before reading the file or invoking
the data-construction path.

The source contract is covered by
`tests/automated/check-xpm-file-data-return.py`.

## Validate XPM file-to-image arguments

`XpmReadFileToImage` now rejects a null display, filename, or call with no
image outputs before opening the file or entering Xlib conversion.  Optional
shape-mask-only output remains valid for compatibility.  Invalid public input
returns `XpmFileInvalid` deterministically instead of relying on a downstream
dereference or file-open failure.

## XPM file-to-image display validation

`XpmReadFileToImage` requires an X display for image creation and color
queries. A null display is rejected before file parsing or Xlib calls.

The source contract is covered by
`tests/automated/check-xpm-file-image-display.py`.

## Validate raw XPM-image output records

`XpmReadFileToXpmImage` now rejects a null filename, `XpmImage`, or
`XpmInfo` pointer before initializing caller-owned output records.  This keeps
the raw parser's invalid-input behavior deterministic and prevents a malformed
caller from crashing in the initialization helpers.

## Validate XPM file-image writes

`XpmWriteFileFromXpmImage` now rejects a null source image before opening the
destination.  A null filename remains the documented stdout output path, so
that compatibility behavior is unchanged.

## Validate XPM image-to-file inputs

`XpmWriteFileFromImage` now rejects a null display or a call with neither an
image nor a shape image before entering X11 image conversion.  Shape-only
exports remain supported, and the null-filename stdout behavior is unchanged.

## Validate XPM pixmap-to-file inputs

`XpmWriteFileFromPixmap` now rejects a null display or a call with neither a
pixmap nor a shape mask before attempting X11 extraction.  Shape-mask-only
exports remain supported.

## XPM file-write status

File-backed XPM output now checks stream errors after the final write and
propagates `fclose()`/`stdout` flush failures through the public writer result.

Regression: `tests/automated/check-xpm-file-write-status.py`

## XPM hash growth failure state

Hash-table growth now publishes the new size, limit, and atom-table pointer
only after replacement allocation succeeds.  An allocation failure therefore
leaves the existing table metadata coherent and retryable.

Regression: `tests/automated/check-xpm-hash-growth-state.py`

## XPM hash interning inputs

Hash interning now rejects null tags and uninitialized tables with
`XpmFileInvalid` before hashing.  Valid initialized tables and non-null tags
retain the existing lookup and growth behavior.

Regression: `tests/automated/check-xpm-hash-inputs.py`

## XPM header-table allocation

The temporary pointer table used while serializing an `XpmImage` is now
allocated with `header_nlines` elements.  The previous code passed the table's
byte count as the element count, causing an unnecessary extra multiplication
inside `calloc`; the resulting table contents and public behavior are
unchanged.

Regression: `tests/automated/check-xpm-header-calloc.py`

## XPM header-copy bounds

Array serialization now copies exactly the allocated header-line table and
only writes a successor pointer when another header entry exists.  The final
header pointer is reserved for the pixel section, avoiding an out-of-table
successor write on edge dimensions.

Regression: `tests/automated/check-xpm-header-copy-bounds.py`

## XPM header and color size guards

XPM data serialization now checks accumulated header and color-line sizes
before adding each new line, freeing the just-created line on overflow instead
of publishing a wrapped total.

Regression: `tests/automated/check-xpm-header-size-guards.py`

## XPM raw-image conversion inputs

`XpmCreateImageFromXpmImage` now validates its display, source image, and
requested output pointers before reading image fields or allocating color
buffers.  Null or output-less calls return `XpmFileInvalid`, matching the
other public XPM conversion entry points and preserving initialized null
outputs.

Regression: `tests/automated/check-xpm-image-inputs.py`

## XPM image offset guard

XPM image serialization now checks zero `cpp` and the maximum safe
`width * cpp + 1` value before computing the row offset, preventing wrapped
dimensions from reaching allocation and pixel-row logic.

Regression: `tests/automated/check-xpm-image-offset-guard.py`

## Validate in-memory XPM image arguments

The data- and buffer-backed XPM constructors now reject null display, input,
or calls with no output target before opening the in-memory source or invoking
the parser.  Raw `XpmImage` constructors also validate their image and info
records.  Optional shape-mask-only output remains valid for compatibility.

## XPM next-string lookahead initialization

When scanning buffer-backed XPM data with natural comment markers, the parser
can encounter the end of the buffer before entering its lookahead loop. The
lookahead character is now initialized to `NUL`, so the post-loop check is
defined and does not consume an indeterminate value. Normal non-empty scans
retain their existing behavior.

The focused regression is `tests/automated/check-xpm-next-string-init.py`.

## XPM one-row pixel serialization

Both array and buffer serializers reject zero dimensions.  Height-one images
with nonzero width follow the regular final-row path, so their pixels are
included in the generated XPM data instead of being silently omitted.

Regression: `tests/automated/check-xpm-one-row-pixels.py`

## XPM parse-and-create inputs

The direct XPM parse-and-create path now validates its display, parser state,
image/output targets, and explicit visual, color-callback, and color-symbol
attributes before setup.  This keeps it consistent with the raw-image path and
prevents malformed state from reaching parser allocation or cleanup calls.

Regression: `tests/automated/check-xpm-parse-create-inputs.py`

## XPM parser entry validation

`xpmParseData` now rejects null parser-state or output-image arguments with
`XpmFileInvalid` before dereferencing them or starting header parsing. Parsed
zero-width, zero-height, zero-color, or zero-chars-per-pixel image headers are
rejected before comment, color-table, or pixel allocation work begins.

Regression: `tests/automated/check-xpm-parse-entry.py`

## XPM compressed-reader wait status

The z-pipe reader now checks the result of `waitpid` after retrying interrupted
waits. A non-interrupted wait failure closes the pipe through the existing
failure path instead of interpreting an uninitialized child status.

Regression: `tests/automated/check-xpm-pipe-wait-status.py`

## XPM pixel final-row bounds

`WritePixels` now checks space for each row's opening quote and for the closing
quote of the final pixel row before writing them.  It also checks the resulting
size before updating the caller's unsigned `used_size`.  Normal XPM output is
unchanged; malformed or undersized internal buffers now fail cleanly instead
of advancing past their bound.

The regression check is
`tests/automated/check-xpm-pixel-final-bounds.py`.

## XPM pixel-token bounds

XPM file output now rejects zero `cpp` and color tokens shorter than the
declared characters-per-pixel width. Valid tokens are copied exactly, avoiding
uninitialized padding from `strncpy`.

Regression: `tests/automated/check-xpm-pixel-token-bounds.py`. The check tracks
the current status-returning `WriteExtensions` declaration so it remains
effective after error-propagation changes.

## XPM pixmap-import status

The internal pixmap-to-`XImage` helper now reports failed geometry queries and
failed `XGetImage` calls.  The data, buffer, file, and direct `XpmImage`
conversion paths propagate that `XpmFileInvalid` result and release any image
already acquired from a paired pixmap or mask.  Invalid server pixmaps
therefore cannot continue as null images into the conversion code.

Regression: `tests/automated/check-xpm-pixmap-import-status.py`

## XPM pixmap resource creation

The XPM image conversion helper now validates its input image and output
pointer, checks `XCreatePixmap`, and checks `XCreateGC` before calling
`XPutImage`.  If GC creation fails, the pixmap is released and the result is
reset to `None`; callers therefore receive a clean failure rather than an
invalid server request.

## XPM raw-image structure

The raw XPM-image conversion path now rejects zero geometry, zero
characters-per-pixel or color count, and missing color-table or pixel-data
arrays before entering color allocation or image emission.  Valid parsed and
constructed `XpmImage` values retain their existing behavior; malformed
structures return `XpmFileInvalid` instead of reaching a null dereference.

Regression: `tests/automated/check-xpm-raw-image-structure.py`

## XPM file-to-buffer close status

`XpmReadFileToBuffer` now treats a failed `fclose()` as an unsuccessful read,
while retaining the existing short-read and VMS empty-record checks.
The validated file size is also converted once to `size_t` before allocation
and reading, avoiding signed-width diagnostics on large files.

Regression: `tests/automated/check-xpm-read-close-status.py`

## XPM reader close status

The file-to-image and file-to-`XpmImage` readers now propagate a failed
`fclose()` as `XpmOpenFailed` when parsing itself succeeded. Existing parse
errors remain the primary result, while standard input remains unclosed.

Regression: `tests/automated/check-xpm-reader-close-status.py`

## XPM RGB lookup allocation

RGB-name lookup duplicates the caller's input before normalization. If that
allocation fails, lookup now returns failure instead of dereferencing a null
temporary buffer.

Null input and output pointers are rejected before any string or result
access.

The RGB table binary search uses an overflow-safe midpoint calculation when
the table grows large.

RGB-file loading and table lookup also reject null pointers and non-positive
table sizes before touching the supplied storage.

The cleanup routine applies the same rule and becomes a no-op for an absent
table or non-positive count.

The source contract is covered by
`tests/automated/check-xpm-rgb-allocation.py`.

## XPM image-scan entry validation

`XpmCreateXpmImageFromImage` now rejects null display/output arguments and a
call with neither a source image nor a shape image.  This keeps direct callers
on the documented `XpmFileInvalid` path before any Xlib geometry, zero-sized
allocation, or scan arithmetic begins.
It also rejects source `XImage` objects without pixel data, preventing the
reader callbacks from dereferencing a missing buffer.
Nonpositive row strides are rejected as well, so row and plane offsets cannot
underflow in the readers.
Nonpositive bitmap-unit and bits-per-pixel fields are rejected too, preventing
modulo or shift operations from using invalid image geometry. Bits-per-pixel
values wider than the host `unsigned long` temporary are rejected before
generic reader copies.
Nonpositive depth values are rejected before plane traversal and mask
calculation as well.
Depths above 32 are rejected because the optimized reader’s low-bit mask table
is defined only for the 32-bit range.
Negative x-offsets are rejected before bit and byte indexing can underflow.
When a shape image is provided, its dimensions must match the source image so
the shared scan geometry cannot overrun either image.

Regression: `tests/automated/check-xpm-scan-entry.py`

## XPM serialization structure

The data, buffer, and file serializers now reject malformed `XpmImage`
structures before calculating output sizes or walking color and pixel arrays.
They require positive dimensions and chars-per-pixel, a nonzero color count,
and present color-table and pixel-data arrays.  Valid images retain their
existing output formats.

Regression: `tests/automated/check-xpm-serialization-structure.py`

## XPM serializer width checks

Serializer pointer-difference lengths now use explicit `unsigned int` casts
after their fixed-buffer bounds are established. Extension string lengths are
also rejected when they exceed the legacy 32-bit size fields before addition.

Regression: `tests/automated/check-xpm-serializer-widths.py`

## XPM string growth bounds

File-backed XPM string parsing now checks accumulated record lengths before
growing its temporary buffer. An oversized record returns `XpmNoMemory`
instead of allowing `unsigned int` length arithmetic to wrap.

Regression: `tests/automated/check-xpm-string-growth-bounds.py`

## XPM string initial allocation

File-backed `xpmGetString` now checks its initial one-byte allocation before
entering the record-growth loop, returning `XpmNoMemory` directly instead of
depending on `XpmRealloc(NULL, ...)` behavior.

Regression: `tests/automated/check-xpm-string-initial-allocation.py`

## XPM token bounds

`xpmNextWord` now rejects tokens that exceed the caller-provided buffer rather
than returning a truncated prefix and leaving the parser in the middle of the
same token. It also rejects tokens that exactly fill the destination, since
callers need room for a terminator. It consumes the remainder for both memory
and file inputs, and rejects null or zero-length destinations safely.

Regression: `tests/automated/check-xpm-token-bounds.py`

## XPM token byte preservation

Both memory- and file-backed XPM token scanners now cast input through
`unsigned char` before storing it in the token buffer. High-bit symbols retain
their byte values consistently across platforms and strict conversion builds.

Regression: `tests/automated/check-xpm-token-byte-casts.py`

## XPM transparent-only images

The XPM image scanner now treats a pixel map containing only its transparent
mask color as a valid zero non-mask-color case.  It skips the zero-size
`XColor` allocation and returns success after the transparent color has been
recorded, so allocator behavior for `malloc(0)` cannot reject valid images.

Regression: `tests/automated/check-xpm-transparent-only.py`

## XPM explicit visual input

`XpmCreateImageFromXpmImage` now rejects an `XpmVisual` attribute flag with a
null `visual` pointer before color setup reads visual fields.  Calls that omit
the flag continue to use the display's default visual, and valid explicit
visuals retain their existing behavior.

Regression: `tests/automated/check-xpm-visual-input.py`

## XPM whitespace byte safety

XPM token scanning now classifies whitespace through an EOF-safe helper that
casts input bytes to `unsigned char`. Extension-name whitespace uses the same
byte-safe cast, avoiding undefined behavior for high-bit input bytes.

Regression: `tests/automated/check-xpm-whitespace-byte-safety.py`

## XPM pixel-writer dimensions

The XPM file writer rejects zero-width or zero-height images before deriving
the final row count. This prevents unsigned underflow of `height - 1` from
turning malformed input into an enormous write loop.

The source contract is covered by
`tests/automated/check-xpm-write-pixels-dimensions.py`.

## XPM zero-dimension output

Array and buffer serializers now reject zero-width or zero-height images at
their public entry points and before writing pixel rows.  This matches the
file serializer and avoids partial work for malformed XPM output; valid
one-row images remain supported.

Regression: `tests/automated/check-xpm-zero-dimensions.py`

## XPM zero-height image cleanup

`CreateXImage` rejects zero-height images because there is no usable image
buffer to allocate. It also rejects dimensions whose row-stride multiplication
would overflow. Both rejection paths now destroy the `XImage` created by
`XCreateImage` and clear the caller's output pointer first. This keeps the
existing `XpmNoMemory` result and valid-image behavior while preventing a
resource leak or dangling output pointer for malformed XPM input.

The focused regression is `tests/automated/check-xpm-zero-height-cleanup.py`.

## XPM ZPixmap fallback dispatch

The optimized scanner handles 8-, 16-, and 32-bit ZPixmap images directly.
Other valid widths now explicitly use the general `GetImagePixels` reader,
which preserves pixel scanning instead of accidentally retaining a prior status
value and returning an empty result.

Regression: `tests/automated/check-xpm-zpixmap-fallback.py`
