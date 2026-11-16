---
nav_exclude: true
---
# Archived Bugfix and Hardening Notes (Part 6)

Consolidated archive of standalone bugfix and hardening validation notes.

## ScrollBar resize spans

ScrollBar arrow and slider spans now use signed, bounded arithmetic.  A
control that is smaller than its highlight and shadow insets keeps a one-pixel
span instead of wrapping an unsigned `Dimension` subtraction into a huge
rectangle.  The behavior is applied consistently to horizontal and vertical
orientation, with and without arrows.

The source-level regression is `tests/automated/check-scrollbar-resize-spans.py`.

## ScrollBar slider pixmap

ScrollBar renders its slider into an off-screen pixmap before copying it into
the widget window.  If X11 cannot create that pixmap, the draw path now stops
before issuing fill, shadow, or mark requests against `None`; the existing
window-copy path continues to treat the missing optional pixmap as unavailable.

## ScrollBar value hook guards

ScrollBar's synthetic value export/import callbacks and processing-direction
default now validate widget and output storage before reading or changing
scrollbar state. Invalid exports return zero, invalid imports return
`XmSYNTHETIC_NONE`, and invalid defaults leave the `XrmValue` untouched.

The focused source-contract check is:

```sh
python3 tests/automated/check-scrollbar-value-hook-guards.py
```

## Scrolled-window auto-drag allocation

Auto-drag support in `XmScrolledWindow` keeps rectangles used by scrollbar
drop callbacks.  The rectangle state is now checked when it is allocated and
before callbacks or geometry updates dereference it.  If the state cannot be
allocated, auto-drag is disabled for that instance.  Rectangle-provider
failures and empty results are also ignored safely, including provider methods
implemented by subclasses.

## ScrolledWindow auto-scroll rectangle bounds

Auto-scroll hotspots are represented by `XRectangle`, whose origins are signed
16-bit fields.  The old rectangle builders assigned work-area and scrollbar
coordinate differences directly, so large offsets could wrap the hotspot.  A
width/height conversion could also truncate an oversized extent.

Horizontal and vertical hotspot origins now use the bounded position helper,
and extents are clamped to the rectangle field range.  Normal autoscroll
behavior is unchanged; extreme layouts no longer place the hotspot at an
unrelated coordinate.  The contract is protected by
`tests/automated/check-scrolled-autoscroll-rect-bounds.py`.

## ScrolledWindow page arithmetic

Page actions derive their target from the current horizontal or vertical origin
and either a scrollbar page increment or the work-area dimension.  These values
are stored in `int`; adding or subtracting without widening can overflow before
`MoveWindow` applies the configured range.

Page-left/right/up/down now perform the arithmetic in `long long` and saturate
to `int` before range clamping.  Normal scrolling is unchanged, while extreme
content sizes cannot reverse a page action through integer overflow.  The
contract is protected by `tests/automated/check-scrolled-page-arithmetic.py`.

## ScrolledWindow position bounds

ScrolledWindow computes scrollbar and viewport positions from widget dimensions
and padding, then stores them in X11's signed `Position` type.  Very wide
allocations can exceed that range; direct casts wrap the scrollbar or clip
origin to the opposite side of the window.

Scrollbar positions, all placement-policy branches, and the bounded viewport
helper now use a saturating conversion.  Ordinary layouts remain unchanged,
while extreme dimensions clamp to the representable X11 range.  The contract is protected by
`tests/automated/check-scrolled-position-bounds.py`.

## ScrolledWindow scrollbar size extents

`GetVariableSize` combines scrollbar dimensions with padding and highlight
thickness before using the result in parent-size negotiation.  The sum can
exceed the 16-bit `Dimension` range for large resources; direct assignment
wraps the requested extent and can make the scrolled window too small.

Scrollbar extents now use the bounded extent helper before entering layout
negotiation.  Ordinary sizes are unchanged, while oversized resources clamp to
the representable `Dimension` range.  The contract is protected by
`tests/automated/check-scrolled-size-extents.py`.

## ScrolledWindow auto-drag allocation

The horizontal and vertical auto-drag rectangle helpers now initialize their
outputs to an empty set and check `XtMalloc` before filling the rectangles.
Allocation failure therefore disables that auto-drag geometry for the call
without dereferencing a null pointer or exposing a nonzero count with no
storage. Successful behavior remains unchanged.

## ScrolledWindow child allocation

`XmScrolledWindow` creates an internal clip window and scrollbars during
initialization. The scroll-frame trait is now invoked only when the clip child
and trait callback exist. If the clip child cannot be created, resize and
automatic-layout paths return safely, and slider callbacks ignore a missing
closure instead of dereferencing it. Normal clip and scrollbar construction is
unchanged.

The contract is protected by
`tests/automated/check-scrolled-child-allocation.py`.

## ScrolledWindow scale negotiation

When a ScrolledWindow uses a toolkit-owned default size, a live display-scale
change must request the new preferred size through Xt. Calling `XtResizeWidget`
directly can violate a parent Form or other fixed allocation: the child shrinks
even though its parent still owns the previous allocation. `ScaleChanged` now
uses `XtMakeResizeRequest`, accepts an `Almost` reply through the normal second
request, and records the actual accepted core dimensions for subsequent scale
changes. Explicit application dimensions remain untouched.

The source contract is checked by
`tests/automated/check-scrolled-scale-negotiation.py`; the installed live-scale
consumer is `tests/automated/run-display-scale-scrolled.py`.

## ScrolledWindow viewport bounds

Automatic and application-defined ScrolledWindow layouts now clamp the clip
and work-area origin and extents to the widget's actual allocation. This keeps
scrollable content and visible scrollbar geometry inside very small panes,
including one-pixel width or height allocations, while preserving normal
layout at larger sizes. Automatic scrollbars whose complete thickness cannot
coexist with a positive viewport are temporarily suppressed and restored when
the allocation grows.

The focused source-contract check is
`tests/automated/check-scrolled-viewport-bounds.py`. The installed live-scale
consumer remains `tests/automated/run-display-scale-scrolled.py`.
Navigator-fit policy is covered by
`tests/automated/check-scrolled-navigator-fit.py`.

## ScrolledWindow scroll-frame allocation handling

ScrolledWindow’s scroll-frame trait initializer now checks its private data
allocation before initializing fields.  On failure it clears the initialized
flag and returns, allowing later queries to report an unavailable frame rather
than dereferencing null state.

The regression check is `tests/automated/check-scrolledw-scrollframe-allocation.py`.

## ScrolledWindow auto-scroll rectangles

Automatic scrolling builds edge rectangles from the scrolled-window size and
the translated work-area origin. The old width and height subtractions mixed
unsigned dimensions with signed rectangle positions, so a work area beyond a
collapsed window could wrap into a huge auto-scroll region.

The horizontal and vertical edge spans now use signed intermediates and retain
the existing minimum span of two pixels. Normal auto-scroll behavior is
unchanged while collapsed windows remain bounded.

## ScrolledWindow constant layout geometry

Constant-policy ScrolledWindow layout derives available work-area dimensions by
subtracting offsets, padding, and shadows from unsigned widget dimensions. A
collapsed scrolled window could wrap those values and configure an oversized
clip area or scrollbar region.

The layout now performs the available-size subtraction in signed arithmetic and
clamps the no-work-window area to zero. Normal scrolling geometry is unchanged
while transiently tiny windows remain bounded.

## ScrolledWindow scrollbar placement bounds

Bottom/right scrollbar placement now computes the available coordinate in
signed arithmetic and clamps it to zero when a collapsed ScrolledWindow is
smaller than the scrollbar.  This prevents unsigned subtraction from wrapping
the scrollbar into a distant position while retaining the existing placement
for normally sized windows.

The source-level regression is
`tests/automated/check-scrolledwindow-scrollbar-placement-bounds.py`.

## ScrolledWindow scrollbar resize requests

When a scrollbar changes size, ScrolledWindow derives the parent request by
removing the old scrollbar extent and adding the new one.  Those operands are
Motif dimensions (unsigned), so a scrollbar larger than a collapsed window
could previously wrap the parent request to a huge size.  The calculation now
uses signed intermediates and clamps a negative result to zero before sending
the request upstream.

The source-level regression is
`tests/automated/check-scrolledwindow-scrollbar-request-bounds.py`.

## ScrollFrame navigator-list growth

Navigator registration now checks cardinal and byte-size limits before adding
capacity. A wrapped slot count or allocation size is rejected without
publishing a corrupted list, while normal growth remains unchanged.

The focused regression check is
`tests/automated/check-scrollframe-nav-growth.py`.

## ScrollFrame navigator removal

Navigator removal now checks that a nonempty navigator count has a backing
list before searching it. Inconsistent or partially initialized state is
ignored safely; normal removal and callback cleanup are unchanged.

The focused regression check is
`tests/automated/check-scrollframe-nav-remove.py`.

## ScrollFrame navigator callback removal

Navigator removal now checks that the navigator trait provides
`changeMoveCB` before invoking it. A partially implemented trait no longer
causes a null function-pointer call; the navigator is still removed from the
ScrollFrame list.

The focused regression check is
`tests/automated/check-scrollframe-remove-callback.py`.

## SearchBox callback guards

SearchBox validates its widget type before query and commit callback delivery,
and validates event data before clear-button or Escape handling. Invalid
teardown or synthetic dispatch is ignored safely.

## SearchBox value API inputs

The public SearchBox get, set, clear, and commit APIs now reject a null widget
before invoking the Xt subclass check. Invalid optional handles return the
existing `NULL` or no-op result; valid text-field updates and callback
behavior are unchanged.

The focused regression check is `tests/automated/check-searchbox-inputs.py`.

## Search-box placeholder lifecycle

`XmSearchBox` owns its copied placeholder `XmString`.  During `XtSetValues`,
the old owned copy is now released only after a replacement is created; if
copying fails, the previous placeholder remains in use.  This prevents leaks
from repeated placeholder updates without changing valid resource behavior.

`tests/automated/check-searchbox-placeholder-lifecycle.py` protects this
ownership and allocation-failure path.

## Secondary conversion callback inputs

`SecondaryConvertHandler` receives a selection-conversion callback and an
`ATOM_PAIR` parameter payload.  It now refuses callbacks with no widget or
callback structure, and refuses a missing or undersized pair before reading
the selection request or casting the payload.  Valid requests continue to
use the existing synchronous secondary-selection flow.

The regression check is
`tests/automated/check-secondary-convert-inputs.py`.

## Secondary conversion cleanup

`SecondaryDone` can run after the secondary conversion context has been
cleared by selection teardown.  The callback may still contain an allocated
selection value even though no conversion callback can be dispatched.  The
cleared-context path now releases that value before disowning the selection,
matching the normal completion path and preventing a teardown-time leak.

The regression check is `tests/automated/check-secondary-done-cleanup.py`.

## Secondary transfer lock failure

Secondary selection conversion is synchronous and uses a process-level lock
while it dispatches the destination transfer.  If `_XmDestinationHandler`
rejects that transfer, the conversion callback now invokes
`ReleaseSecondaryLock` before returning `XmCONVERT_REFUSE`.  Without this
cleanup, one rejected request left the lock set permanently and caused every
subsequent secondary conversion to be refused.

The regression check is `tests/automated/check-secondary-lock-failure.py`.

## secondary-result-payload-guards
Reject malformed secondary-selection results
=============================================

`SecondaryDone` now fails closed when the selection owner reports an invalid
format or a non-empty result without both a type atom and payload pointer.
Rejected payloads are released before the secondary selection is disowned.
The valid empty cancellation result and 8/16/32-bit typed results retain their
existing behavior.

The focused regression is:

```text
python3 tests/automated/check-secondary-result-payload.py
```

## Secondary-selection transfer inputs

Secondary-selection transfer now rejects a `None` target before creating a
request.  Its completion callback validates the widget and output pointers
before obtaining atoms, releasing any returned value when the callback state
is incomplete.  Invalid and cancellation paths therefore fail without
dereferencing stale selection state.

The focused regression is
`tests/automated/check-secondary-transfer-inputs.py`.

## Segmented-control button creation

`XmSegmentedControlSetItems` now checks each toggle-button creation before
installing callbacks or managing the child.  If creation fails, already
created buttons and copied item data are released and the control is reset to
an empty selection, avoiding a null-child dereference during layout.

`tests/automated/check-segmented-button-creation.py` protects this failure
path.

## SegmentedControl callback guards

Toggle-child notifications and key handlers can be delivered during widget
teardown or synthetic dispatch. The control now treats missing closure,
child, or event data as a no-op; normal selection and keyboard navigation are
unchanged for live controls.

The focused contract check tolerates normal C line wrapping while requiring
both NULL-guard clauses: `tests/automated/check-segmented-control-callback-guards.py`.

## SegmentedControl child-key guards

SegmentedControl validates that child-change and child-key callback closures
refer to the expected widget class before reading private state. Foreign or
stale closures are ignored safely while normal selection and keyboard
navigation remain unchanged.

## SegmentedControl state API inputs

The public SegmentedControl item and selection APIs now reject a null widget
before checking the Xt subclass. Invalid optional handles return the existing
neutral values (`False`, `0`, `-1`, or `NULL`) without changing valid item,
selection, callback, or layout behavior.

The focused regression check is
`tests/automated/check-segmented-control-inputs.py`.

Item replacement also rejects a widget already being destroyed.  This prevents
the public model API from creating or destroying segment children during Xt
teardown while retaining its existing result for live controls.

## Segmented-control geometry bounds

Segmented-control preferred width now accumulates child widths and spacing in
a wider type and clamps the result to the platform `Dimension` maximum. Large
segment sets no longer wrap into unexpectedly narrow geometry requests;
ordinary layouts are unchanged.

Modern drawing also verifies that the inner rounded face fits after its stroke
inset before passing its dimensions to X11. Very small segments now retain
their outer face instead of narrowing a negative inner width or height through
an unsigned drawing parameter. Focus and separator rectangles use explicit
representable unsigned extents.

`tests/automated/check-segmented-geometry-bounds.py` protects this calculation.
`tests/automated/check-segmented-render-geometry.py` protects the drawing
bounds.

## SegmentedControl layout bounds

When left-to-right segments exceeded the available width, the old layout
code computed `available - x` after `x` had already passed the edge.  Because
`Dimension` is unsigned, that underflow could configure a trailing button with
an enormous width.  The layout now clamps widths when the cursor reaches the
edge and keeps the cursor bounded while applying spacing.

`tests/automated/check-segmented-layout-bounds.py` keeps the bounded-span path
in place.

## Select-color converter input validation

The select-color converter now validates display, argument, source, and
destination inputs before handling special constants or invoking Xt's pixel
converter. Converter-data writes are conditional, while valid special-color
and ordinary pixel conversions remain unchanged.

The focused regression check is
`tests/automated/check-select-color-converter-inputs.py`.

## Transfer selection-callback inputs

Selection callbacks can race cancellation and widget destruction.  The
bookkeeping wrapper now validates the callback widget, transfer context,
selection/type pointers, and length/format outputs before reading the request
queue or interned atoms.  Invalid callback state is ignored safely instead of
dereferencing a stale transfer context.

The focused regression is
`tests/automated/check-selection-callback-inputs.py`.

## selection-dialog-argument-allocation
Selection and prompt dialog argument allocation bounds
========================================================

`XmCreateSelectionDialog` and `XmCreatePromptDialog` append an internal
`XmNdialogType` argument to the caller's SelectionBox argument list.  The
temporary list now rejects counts that cannot represent the extra element or
its byte size in the legacy `Cardinal` allocator interface before any
multiplication or allocation.  Allocation failure and invalid non-empty source
lists return `NULL` before the temporary list is accessed.

As with the other Motif convenience constructors, an empty list is valid with
a null pointer.  The copy is skipped in that case, avoiding an undefined
zero-length `memcpy` while preserving the normal empty-argument behavior.

## Selection-loss callback inputs

`ClipboardLoseProc` and `LoseProc` run when an owner loses a selection.  They
now reject a null widget, null selection pointer, or `None` selection atom
before deriving atoms, converting data, or removing destruction callbacks.
This keeps ownership teardown safe when a selection loss arrives during widget
destruction or cancellation.

The regression check is `tests/automated/check-selection-loss-inputs.py`.

## SelectionBox child-count bounds

SelectionBox geometry setup now traverses its composite child array with an
Xt `Cardinal` index, including the reverse traversal used for right-to-left
button layout. Existing widget ordering and geometry behavior are unchanged.

## SelectionBox synthetic getter guards

SelectionBox exposes several compound-string and list resources through
synthetic `XtGetValues` entries. Their callbacks now validate both the output
`XtArgVal *` and the source widget before touching SelectionBox instance data.
Calls made with a null or unrelated widget return a deterministic null/zero
value instead of dereferencing invalid state. Existing child-widget behavior
and public resource names are unchanged.

The source-contract regression check is:

```sh
python3 tests/automated/check-selectionbox-synthetic-getter-guards.py
```

## Separator gadget geometry bounds

Separator gadget redisplay now clamps the inset fill and separator dimensions
to zero when the gadget is narrower or shorter than its highlight insets.
Collapsed gadgets skip those drawing calls instead of converting negative
spans to large unsigned rectangles.

The source-level regression is `tests/automated/check-separatog-geometry-bounds.py`.

## Separator geometry bounds

`XmSeparator` derives its default and recalculated spans from twice the
highlight thickness plus a separator-specific extra.  All orientation and
separator-type paths now use widened arithmetic and clamp that span to the
X11 `Dimension` range, preventing large resource values from wrapping during
initialization or `SetValues`.

The focused source regression is
`tests/automated/check-separator-geometry-bounds.py`.

## SeparatorGadget hook allocation handling

SeparatorGadget's secondary-object, set-values, and get-values hooks now check
extension-object and extension-data allocations.  Partial objects are
released before returning.  Initialize, set-values, and get-values posthooks
also tolerate missing extension records when a prehook could not establish
temporary state.

The regression check is `tests/automated/check-separatorg-hook-allocation.py`.

## Icon-box bounds recalculation guards

`SetNewBounds` now handles partially initialized icon boxes and invalid icon
increments by resetting the tracked last row/column to an empty state.  It
avoids dereferencing missing widgets and divide-by-zero during layout updates.

`tests/automated/check-set-bounds-guards.py` protects this path.

## Icon-box bounds child guards

`SetNewBounds` now handles a missing composite child list and skips null child
entries while recalculating icon-box bounds.  Valid child geometry continues to
drive the same bounds.

`tests/automated/check-set-new-bounds-children.py` protects the contract.

## Shell title and callback input guards

`XmeSetWMShellTitle`, `XmeAddFocusChangeCallback`, and
`XmeRemoveFocusChangeCallback` now return for a null widget before
application-context lookup. Valid shell title updates and vendor focus
callback operations are unchanged.

The focused regression is
`tests/automated/check-shell-callback-inputs.py`.

## Client icon-state guard

`ShowClientIconState` now returns for a missing client record before reading
the icon window handle.  Existing state transitions for valid clients are
unchanged.

`tests/automated/check-show-client-icon-state.py` protects the contract.

## Minimized-icon placement sentinel guard

`ShowIconForMinimizedClient` now rejects a negative or oversized placement
returned by coordinate conversion before indexing the workspace placement list.
The client is left without a placement when no valid slot can be derived.

`tests/automated/check-show-icon-placement-sentinel.py` protects this contract.

## Minimized-icon state placement guards

`ShowIconForMinimizedClient` now rejects missing client/workspace state and
invalid stored placement indices before reading the workspace placement list.
Valid minimized-icon placement continues unchanged.

`tests/automated/check-show-icon-state-place-guards.py` protects the contract.

## Sidebar callback guards

Sidebar validates its action callback closure before forwarding the toggle
request. Foreign or stale callback data is ignored safely while normal action
button behavior remains unchanged.

## Sidebar child allocation

Sidebar creates a content row-column and a toggle button during initialization.
Either child may be unavailable after an allocation failure. Initialization and
later relayout now guard management, callbacks, preferred-size queries, and
geometry configuration so the widget remains safe and can recover on a later
layout pass.

The contract is protected by
`tests/automated/check-sidebar-child-failure-guards.py`.

## Sidebar event guards

Sidebar validates its widget type and event pointer before processing the
keyboard toggle shortcut. Invalid dispatch is ignored safely while normal
keyboard toggling remains unchanged.

## Sidebar natural height bounds

Sidebar now accumulates toggle and content heights in a wide intermediate and
saturates at the representable `Dimension` limit. Large expanded sidebars can
no longer wrap to an unexpectedly small height.

## Sidebar title replacement

`XmSidebar` owns a copy of its `XmNlabelString` title.  On an `XtSetValues`
title change, the Sidebar now creates the replacement XmString before releasing
the existing title.  If allocation fails, it restores the old pointer and
leaves the existing header label and layout unchanged.

This keeps a Sidebar usable under allocation pressure without borrowing a
caller-owned XmString or clearing its visible title.

## Sidebar toggle child allocation

Sidebar expansion and collapse now tolerate a missing toggle button after an
internal child allocation failure. Focus traversal and label updates are
skipped safely while content state and callbacks continue to be maintained.

The contract is protected by
`tests/automated/check-sidebar-toggle-child-guards.py`.

## SimpleMenu creation-failure handling

SimpleMenu assembly now stops safely when its RowColumn widget is unavailable
and only registers callbacks for children that Xt actually created. This
prevents an allocation or widget-creation failure from becoming a secondary
NULL dereference while preserving successful menu construction.

The focused regression is
`tests/automated/check-simple-menu-create-failures.py`.

## SimpleMenu creation parent guards

The four SimpleMenu convenience constructors now return `NULL` for a null
parent before subresource lookup or application-context access:
`XmCreateSimpleMenuBar`, `XmCreateSimplePopupMenu`,
`XmCreateSimplePulldownMenu`, and `XmCreateSimpleOptionMenu`. Valid parent
widgets retain the existing convenience-menu behavior.

The focused regression is
`tests/automated/check-simple-menu-parent-inputs.py`.

## Simple mutator teardown guards

`XmPromptSetVisible` and `XmDropHighlightSetState` now reject calls after Xt
has begun widget destruction. They no longer mutate redraw state or request an
X11 expose against teardown state. The behavior of live widgets, including
DropHighlight's `Boolean` result for valid and invalid states, is unchanged.

The focused check is
`tests/automated/check-simple-mutator-destroy-guards.py`.

## SimpleSpinBox child allocation

`XmSimpleSpinBox` creates its text-field child during initialization. If Xt
cannot allocate that child, initialization now releases the temporary name and
returns without querying a null widget. The public add, delete, and set-item
APIs treat the incomplete control as a no-op until a text child exists, while
normal spin-box behavior is unchanged.

The contract is protected by
`tests/automated/check-simplespinbox-child-allocation.py`.

## SimpleSpinBox synthetic getter guards

The shared SimpleSpinBox synthetic getter now initializes deterministic output
and validates the output pointer, widget class, and text-field child before
forwarding resource reads. Calls made during partial construction or with an
unrelated widget no longer dereference invalid state; valid resource exports
remain unchanged.

The source-contract regression check is:

```sh
python3 tests/automated/check-simplespinbox-getter-guards.py
```

## SM client ID property handling

`ProcessSmClientID` receives the `SM_CLIENT_ID` value as an X property.  X
property data is length-delimited and is not required to include a C-string
terminator.  The reader now rejects empty or oversized values, copies valid
bytes into an Xt-managed buffer, appends a terminator, and releases the raw
Xlib buffer.  Existing consumers can therefore continue using the stored
value as a normal string without reading past the property.

## Source-contract refreshes

Two focused checks were updated to describe the current, already-landed
implementations rather than removed intermediate forms:

- AutoSuggestBox callback checks now accept the combined null-and-widget-class
  guard used by every callback path.
- Calendar text-metric checks now require the wide baseline arithmetic used
  before the bounded Position conversion.
- ColorPicker redisplay checks now require its wide text baseline arithmetic
  instead of the removed narrow expression.

No runtime behavior changed in this maintenance update.

## Spin box bounds


Spin-box item insertion rejects a maximum-count or unrepresentable string-table
size before incrementing the count, preserving the existing list on failure.

## SpinBox constraint value-table bounds

SpinBox string-child constraints now validate the requested value count before
allocating an `XmString` table. Counts that cannot be represented in either
`size_t` or Xt's historical `Cardinal` byte-size argument are treated as
allocation failures and follow the existing rollback path.

The focused regression is
`tests/automated/check-spinbox-value-init-rollback.py`.

## SpinBox constrained-height layout

When a SpinBox is shorter than its preferred height, vertical arrow spacing is
computed from the available height.  The calculation must not use the widget's
width; doing so made the arrows overflow or move when only the width changed.
The constrained-height regression covers classic, modern, and high-contrast
profiles, scale overrides, arrow layouts, and right-to-left direction.

## SpinBox position hook guards

SpinBox position import/export callbacks now validate the child constraint and
output pointer before reading position state. Numeric index conversion also
requires a non-zero increment, preventing division by zero while preserving
normal position/value behavior. Invalid imports return `XmSYNTHETIC_NONE`.

The focused source-contract check is:

```sh
python3 tests/automated/check-spinbox-position-hook-guards.py
```

## SpinBox value-list initialization rollback

SpinBox now rolls back a partially copied string-child value list when an
`XmStringCopy` fails during constraint initialization.  Earlier copies and the
array are released, and the child publishes an empty list instead of exposing
NULL entries to position and text-update logic.

The focused regression is
`tests/automated/check-spinbox-value-init-rollback.py`.

## SimpleSpinBox value-list rollback

`XmSimpleSpinBoxAddItem` and `XmSimpleSpinBoxDeletePos` now build temporary
value arrays transactionally.  Each `XmStringCopy` is checked and partial
copies are released on failure; the child’s values and current position are
updated only after the complete array is ready.  Allocation sizes are bounded
against both host `size_t` and Xt’s `Cardinal` range.

The focused regression is
`tests/automated/check-spinbox-value-list-rollback.py`.

## SpinBox value-list SetValues rollback

SpinBox now keeps the current string-child value list when a replacement array
or any of its `XmString` copies cannot be allocated during `SetValues`.  A
partial replacement is cleaned up, and the old list is not freed, preventing
NULL entries and preserving the current text/value behavior.

The focused regression is
`tests/automated/check-spinbox-value-setvalues-rollback.py`.

## SimpleSpinBox null-widget guards

`XmSimpleSpinBoxAddItem`, `XmSimpleSpinBoxDeletePos`, and
`XmSimpleSpinBoxSetItem` now return immediately when called with a null
widget. The guard runs before widget-part or application-context lookup, so
an invalid optional operation cannot crash the caller while valid widget
behavior remains unchanged.

The focused regression is
`tests/automated/check-spinbox-widget-inputs.py`.

## Spinner event and timer guards

Spinner event and timer callbacks can race widget destruction. The callbacks
now validate the widget type, event pointer, and timer data before touching
Spinner state; normal visibility synchronization and animation continue
unchanged for live widgets.

## Spinner coordinate bounds

Spinner keeps its ring center in signed arithmetic for X drawing and clamps
the label origin before converting it to Xt's signed `Position` type.  Large
spacing or DPI-derived dimensions therefore clip safely instead of wrapping
the spinner to an unrelated coordinate.

`tests/automated/check-spinner-coordinate-bounds.py` protects this contract.

## Spinner state API inputs

The public Spinner active-state and phase APIs now reject a null widget before
invoking the Xt subclass check. Invalid optional handles return the existing
neutral values or no-op result; valid timer, redraw, and callback behavior is
unchanged.

The focused regression check is `tests/automated/check-spinner-inputs.py`.

## Spinner interval rescheduling

When an active Spinner's `XmNspinnerInterval` changed, the old timeout stayed
armed until it fired once more.  The widget now removes that pending timeout
and schedules a new one immediately, so runtime interval changes take effect
without a stale tick.

`tests/automated/check-spinner-interval-reschedule.py` keeps the replacement
path in `SetValues`.

## Spinner label centering

Spinner now computes the label's vertical offset in signed arithmetic. A label
taller than the widget remains centered with a negative offset rather than
wrapping the unsigned `Dimension` subtraction to a distant position.
The focused check tracks the widened, clamped expression used by the helper.

## Spinner label initialization rollback

Spinner now checks the private copy of its initial label.  If `XmStringCopy`
fails, it installs an empty compound string and avoids measuring a NULL label,
keeping construction and geometry calculation safe under allocation pressure.

The focused regression is
`tests/automated/check-spinner-label-init-rollback.py`.

## Spinner label rollback

`XmSpinner` now copies a changed label before releasing the current
`XmString`. Allocation failure preserves the existing label, while NULL still
explicitly clears it.

The focused source regression is
`tests/automated/check-spinner-label-rollback.py`.

## Spinner geometry negotiation

`XmSpinner` supports independent automatic dimensions.  During construction
and later `XtSetValues`, a zero width or height requests the content-derived
value for that axis.  A nonzero dimension remains an application constraint.
`QueryGeometry` now reflects the same contract, rather than always returning
the preferred size.

## Spinner preferred-size bounds

Spinner's preferred width and height include label metrics and twice the
spacing resource.  The old additions were performed in `Dimension` and could
wrap when applications supplied large spacing or font metrics.  Initialization
and `QueryGeometry` now compute in `size_t` and saturate at the maximum
representable `Dimension`.

`tests/automated/check-spinner-size-bounds.py` keeps the bounded sizing helper
in both paths.

## SimpleSpinBox callback-widget guard

The SimpleSpinBox callback-widget helper now validates its widget class before
reading the text-field child. Invalid inputs return `NULL`; valid
SimpleSpinBox widgets continue to expose their text field to callback code.

The focused source-contract check is
`python3 tests/automated/check-sspinb-callback-widget-guard.py`.

## StackTransientWindow guards

Transient restacking can run after a parent or sibling has been removed.
`StackTransientWindow` now returns for a missing client or leader and stops
when no predecessor is found in the sibling chain.  Valid X stacking remains
unchanged.

## StackWindow guards

Window restacking can race client withdrawal and workspace list teardown.
`StackWindow` now validates the workspace, screen, target entry, and optional
stack reference before reading client windows.  Bottom stacking also returns
when the screen list is empty, avoiding a null tail dereference.  Valid X
restacking behavior is unchanged.

## Standard conversion WM_CLASS ownership

`XGetWindowProperty` returns `WM_CLASS` bytes owned by Xlib. Standard Motif
conversion now copies valid format-8 data into Xt-owned storage and releases
the Xlib buffer. Malformed formats or allocation failure produce an empty
conversion instead of exposing a buffer with the wrong allocator contract.

Regression coverage: `tests/automated/check-standard-convert-class-ownership.py`.

## Standard conversion defaults

`XmeStandardConvert` is commonly called as the final converter in a callback
chain.  It now clears `value`, `type`, `format`, and `length` before examining
the target.  Unsupported targets and widgets without a render table therefore
produce a deterministic empty conversion instead of accidentally reusing
fields left by an earlier conversion.

The focused regression is
`tests/automated/check-standard-convert-defaults.py`.

## Standard conversion WM_NAME ownership

The `WM_NAME` property returned by `XGetWindowProperty` is owned by Xlib. The
standard `XA_WM_NAME` conversion now releases that property after converting
it to the toolkit-owned locale string, including conversion-failure paths.

Regression coverage: `tests/automated/check-standard-convert-name-free.py`.

## Standard WM property status handling

The standard conversion paths for `WM_CLASS` and `WM_NAME` now require a
successful, complete `XGetWindowProperty` response.  Failed or truncated
properties are released and the ancestor search continues, preventing
partial X data from entering the text or class conversion paths.

Regression: `tests/automated/check-standard-convert-property-status.py`

## Standard conversion scalar allocation

The standard transfer converter allocates one scalar for foreground,
background, or colormap requests. Each allocation is now checked before Xt
reads the widget resource. Failure returns an empty conversion with the
appropriate type and format while releasing the application lock.

Successful conversions retain their existing values, lengths, and atom types.

## Standard render-table conversion length

`XmRenderTableCvtToProp` returns an `unsigned int` byte length. The standard
converter now preserves that width until assigning the selection callback's
length, avoiding a narrowing conversion on large render-table properties.

The focused regression is
`tests/automated/check-standard-render-length.py`.

## Standard target count width

`XmeStandardTargets` returns a validated non-negative `int` count for its
legacy API, while selection callbacks store lengths as `unsigned long`. The
standard conversion now widens that count explicitly before publication.

The focused regression is
`tests/automated/check-standard-target-count-width.py`.

## Standard transfer target allocation

`XmeStandardTargets` returns the built-in target atoms with capacity reserved
for the caller's private targets. It now computes that final capacity once and
checks the allocation before filling the atom list. Allocation failure unlocks
the application context, reports zero built-in targets, and returns `NULL`.

Successful callers receive the same atom ordering and spare capacity; the
change removes an unnecessary resize and prevents a null write under memory
pressure.

## Standard target allocation width

`XmeStandardTargets` validates its target count against Xt's `Cardinal`
allocation limit and now passes the checked byte size with an explicit
`Cardinal` conversion. This makes the allocation contract clear on 32-bit and
64-bit builds.

The focused regressions are
`tests/automated/check-standard-targets-cardinal-allocation.py` and
`tests/automated/check-standard-targets-allocation.py`.

## Status-bar label allocation guards

`XmStatusBar` now handles failed text conversion and allocation while composing
plain and progress-item labels. Progress text keeps its existing formatting;
only invalid or unrepresentable allocations are rejected.

The focused regression check is
`tests/automated/check-status-bar-label-guards.py`.

## StatusBar item replacement

`XmStatusBarSetItems` now builds and validates the replacement item array and
its label children before releasing the current presentation.  Allocation,
label-construction, or child-creation failure therefore leaves existing items
visible and usable instead of clearing the status bar.

The setter also ignores calls made after the widget has entered Xt destruction,
so teardown cannot create replacement children or negotiate layout against a
partially destroyed bar.

The contract is protected by
`tests/automated/check-statusbar-transactional-replacement.py`.

## Stepper arithmetic bounds

Stepper range and value resources are `int`, but the old step operation used
`long` for its intermediate.  On platforms where `long` is also 32-bit,
stepping at an extreme value could overflow before the range clamp ran.  The
operation now uses `int64_t`, preserving the existing wrap and clamp behavior
across 32-bit and 64-bit platforms.

`tests/automated/run-stepper-native.py` exercises INT_MIN/INT_MAX
wrapping with an INT_MAX step through the public API.

## Stepper state API inputs

The public Stepper range, value, and step APIs now reject a null widget before
invoking the Xt subclass check. Invalid optional handles return the existing
neutral value or no-op result; valid normalization, redraw, and callback
behavior is unchanged.

The focused regression check is `tests/automated/check-stepper-inputs.py`.

## Stepper layout conversions

Stepper calculates geometry as signed working values after bounding the widget
allocation to the X11 coordinate range.  It now converts those proven values
explicitly at the Xt `Position` and `Dimension` boundary, preserving small
layout behavior without implicit signed/unsigned narrowing.

NumberBox applies the same rule to editor, arrow, and error-label allocation;
the error label cannot receive a negative origin on a very small allocation.

## String-to-entity converter input validation

The widget/window name converter now returns `False` for missing or malformed
Xt argument, source, destination, or widget context instead of dereferencing
them. Valid `self` and descendant-name conversions retain their existing
behavior.

The focused regression check is
`tests/automated/check-string-entity-inputs.py`.

## Compound-string rendition copy guards

Rendition-tag arrays returned by `_XmStringGetSegment` are now built through a
single checked helper. Array allocation and each copied tag are validated; a
failure frees earlier tags and returns no partial rendition output for either
copying or borrowed-tag mode.

## Compound-string segment copy guards

`_XmStringGetSegment` now checks allocations made when copying a segment’s tag
and text. A failed copy returns `FALSE`, clears the affected outputs, and
releases a partially copied tag so callers do not receive invalid pointers or
leak temporary data.

## String-table resource conversion allocation

The string-table resource converter now checks its initial table, growth and
final reallocations, and every item string allocation. It releases partial
strings and the table on any failure, including an output-buffer size failure,
while preserving successful parsing and destroy ownership.

## String-table resource conversion bounds

The string-to-string-table resource converter grows a pointer table while it
parses comma-separated values.  Both capacity paths now check the `int`
limit, release strings already parsed, and return conversion failure before
the capacity can wrap and cause an undersized reallocation.

## String-table conversion rejects invalid multibyte input

`CvtStringToStringTable` now treats a negative `mblen` result as a conversion
failure. Previously it could stop at the invalid byte, publish the valid
prefix, and silently discard the remainder. The failure path releases the
already duplicated entries and leaves the destination conversion unset;
valid comma-separated and escaped-comma values retain their existing behavior.

The focused source-contract check is
`tests/automated/check-string-table-converter-allocation.py`.

## Compound-string wide-text conversion guard

`XmStringGetNextSegment` now checks its multibyte conversion buffer before
calling `wcstombs`. Allocation failure returns `FALSE`, clears the text and
tag outputs, and does not pass a null buffer to the conversion routine.

## XPM symbolic color detection

ImageCache now returns true when `SymbolicColorUsed` finds the requested
symbolic color, while absent inputs and unmatched colors return false. This
restores the cache’s intended color-sensitivity policy for XPM images without
changing ordinary color handling.

The focused regression is `tests/automated/check-symbolic-color-used.py`.

## Synthetic-resource allocation

Synthetic-resource class tables retain their initialized `XrmQuark` names in
the legacy `String` member.  The internal conversion now routes that tagged
value through `uintptr_t`, preserving the historical representation while
avoiding width-dependent direct pointer/integer casts on 64-bit builds.

`_XmBuildResources` now validates signed superclass/subclass counts, source
list pointers, and the complete byte-size calculation before allocating the
combined synthetic-resource table.  Failed allocation leaves the caller’s
existing list unchanged instead of copying through a null pointer.

The focused source-level regression is
`tests/automated/check-synthetic-resource-allocation.py`.

## Tab allocation guards

`XmTabCreate` and the internal `_XmTabCopy` helper now check both the tab
record and its optional decimal string allocation. A failed copy releases its
record and returns `NULL`; a failed resource-created tab is rejected before it
is linked into a tab list. Existing callers still receive the same tab data on
successful allocation, while allocation failures no longer become null
dereferences.

## TabBox label coordinate bounds

TabBox clamps visible canvas tab labels, including the insensitive shadow
pass, before passing coordinates to Motif's signed `Position` drawing
arguments.  Oversized tab layouts therefore cannot wrap a label to an
unrelated screen coordinate.

`tests/automated/check-tab-box-label-coordinate-bounds.py` protects this
drawing contract.

## Tab-list widget constructor guards

The private tab-list widget constructors now return `NULL` for a missing
parent or failed list allocation. `_XmCreateTab` also stops before accessing a
null parent list. Existing successful construction and resource handling are
unchanged.

## Tab-group mutator input validation

`XmAddTabGroup` and `XmRemoveTabGroup` now return safely when passed a missing
widget instead of forwarding it to `XtSetValues`. Valid tab-group resource
updates remain unchanged.

The focused regression check is
`tests/automated/check-tab-group-mutators.py`.

## TabBox canvas allocation

`XmTabBox` renders through an internal canvas child. If Xt cannot create that
canvas, initialization now releases the filtered argument list and leaves the
TabBox in a safe, uninitialized state. Resize and tab drawing treat the
missing canvas as an unavailable rendering surface instead of dereferencing a
null child. Normal canvas creation and rendering are unchanged.

The contract is protected by
`tests/automated/check-tabbox-child-allocation.py`.

## TabBox geometry allocation

`XmTabBox` keeps private arrays for actual tab rectangles, wanted tab
rectangles, and rendered image-cache entries. Growth now validates the count
and both host `size_t` and Xt `Cardinal` byte-size limits, retaining the
existing geometry arrays when
`XtRealloc` fails, and returns before the layout code can dereference a null
replacement. The bounded geometry byte counts are passed explicitly as Xt
`Cardinal` values. Image-cache replacement allocates and checks the new cache
before publishing it; an unavailable cache is represented as an empty cache.

The public tab ordering, geometry resources, and cache behavior on successful
allocation are unchanged.  Invalid or non-positive list counts now produce an
empty layout rather than indexing through an invalid allocation.

## Tab-box image rotation failure handling

The tab-box image rotation helper now checks its pixel-buffer allocation and
both `XCreateImage` calls. On failure it releases the buffer and returns
without attempting pixel writes, preventing low-memory conditions from
becoming NULL dereferences.

The focused regression is
`tests/automated/check-tabbox-image-failures.py`.

## TabBox image-rotation size bounds

TabBox rotates XImages used by tab visuals.  The old helper accumulated the
required storage in signed `int` and passed the result directly to Xt, so
large or malformed image dimensions could wrap into an undersized allocation.
The helper now validates positive dimensions, computes packed and rotated
sizes in `size_t`, and rejects values that cannot fit Xt's `Cardinal` size.

`tests/automated/check-tabbox-image-size-bounds.py` keeps the checked
allocation path in place.

## Complete inactive-tab outlines

`XmTabBox` now draws the outer outline for every inactive tab, including tabs
on later rows of a stacked layout.  The old renderer omitted those edges when
the tab was not on row zero, leaving labels such as **Source** or **Built-in
Icons** visually detached from the tab control.  The existing Motif shadow
GCs remain responsible for the subdued outline, and selected tabs retain
their existing selected-state fill and bevel.

## TabBox label bitmap graphics contexts

The horizontal and vertical TabBox label renderers now verify both lazily
created bitmap graphics contexts before clearing or drawing the label bitmap.
Under X resource pressure, a failed GC allocation therefore releases any
partial pair and returns safely instead of issuing `XFillRectangle` or text
drawing requests with a null GC. A later expose can retry allocation normally.

The focused source contract is checked by
`tests/automated/check-tabbox-label-gc.py`.

## TabBox stack-count bounds

`XmNnumStacks` is an integer resource and applications can set it to zero or
a negative value.  Stacked layout divides the tab count by this value, so an
invalid resource could terminate the process with a divide-by-zero signal.
`CalcStackedGeometry` now treats values below one as one before doing layout
arithmetic, preserving the normal layout contract for valid values.

`tests/automated/check-tabbox-num-stacks.py` keeps the guard in the layout
path.

## TabBox rendering graphics context

`XmTabBox` may create its drawing graphics context lazily during redisplay.
Redisplay now stops safely when that allocation fails and releases any damage
region it created, rather than entering tab drawing with a null GC.

The focused source contract is checked by
`tests/automated/check-tabbox-render-gc.py`.

## TabBox partial repaint and bitmap lifetime

TabBox label rendering can repaint a complete tab even when Xt supplies only a
small damage region. Redisplay now expands damage to intersected tab bounds,
restores the TabBox clip before each tab draw, and releases the temporary
region on every exit path. Cached Xft pictures are evicted before the reusable
label bitmap is freed, preventing stale drawable references during resize,
rotation, and destruction.

## TabBox right-to-left placement

Horizontal right-to-left TabBox layout now computes the initial tab origin in
signed arithmetic.  If the first tab is wider than a collapsed tab box, the
origin remains a valid negative/off-edge coordinate instead of wrapping to a
large positive position.

The source-level regression is `tests/automated/check-tabbox-rtl-placement.py`.

## Table converter input validation

String-table, `XmString`-table, and cardinal-list converters now reject missing
source or destination `XrmValue` structures before parsing or allocation.
Valid table parsing and output-size negotiation remain unchanged.

The focused regression check is
`tests/automated/check-table-converter-inputs.py`.

## TabList argument guards

`XmTabbedStackListInsert` and `XmTabbedStackListModify` now reject a `NULL`
attribute pointer when the caller supplies a nonzero attribute mask.  The
existing list is left unchanged and insertion returns `XmTAB_NOT_FOUND`; valid
zero-mask operations remain compatible.

The focused regression is `tests/automated/check-tablist-argument-guards.py`.

## TabList attribute cleanup

`XmTabAttributesFree` now accepts a `NULL` attribute record and returns without
dereferencing it.  This makes cleanup paths safe when an optional query did not
produce attributes.

The focused regression is `tests/automated/check-tablist-attributes-free.py`.

## TabList comparison with absent lists

`XmTabbedStackListCompare` now treats two `NULL` lists as equal and a single
`NULL` list as a size mismatch.  It returns before reading list fields, so
callers can compare optional tab-list values safely.

The focused regression is `tests/automated/check-tablist-compare-null.py`.

## TabList constructors

`XmTabbedStackListCreate` and `XmTabbedStackListCopy` now check their Xt
allocations before initializing or copying through the returned pointers.
Copying also rejects malformed or oversized source lists and releases the
partially created list before returning `NULL`.

The focused regression is `tests/automated/check-tablist-constructors.py`.

## Tab-list resource conversion allocation

The tab-list resource converter now checks each `XmTabCreate` result. A failed
tab allocation releases any partial tab list and returns `False` instead of
passing a null tab into `XmTabListInsertTabs`.

## Tab-list converter validation and rollback

The tab-list parser now validates its pointer inputs and uses checked finite
`strtof` conversion for offsets, rejecting overflow and malformed units. The
converter treats
`XmTabListInsertTabs` allocation failure transactionally: the temporary tab and
partial list are released and conversion returns `False`. Valid tab parsing
and output negotiation remain unchanged.

The focused regression check is
`tests/automated/check-tablist-converter-inputs.py`.

## Tab-list copy allocation

`XmTabListCopy` now checks its result-list allocation, rejects an empty source
range, and checks the first copied tab before initializing the list. Failures
release the partial list and unlock process state instead of dereferencing null
tab/list pointers.

## Tab-list copy offset validation

`XmTabListCopy` now validates the signed offset before deriving the default
copy count. Out-of-range offsets return `NULL` without allocation or traversal,
and magnitude calculation handles the most-negative `int` without invoking
undefined signed overflow. The same bounded magnitude helper is used by tab
lookup.

## Tab-list copy rollback

When a later tab cannot be copied, `XmTabListCopy` now closes the partial ring,
records the number of successfully copied tabs, unlocks process state, and
frees the partial list before returning `NULL`. Existing marked-tab ownership
rules remain unchanged.

## Tab-list proposal count bounds

`XmStringTableProposeTablist` now uses an unsigned index matching the internal
tab-list count and rejects exhaustion before adding a tab or incrementing the
per-line index.  This prevents counter wraparound while preserving ordinary
tab-list measurement.

`tests/automated/check-tablist-count-bounds.py` protects both guards.

## Empty tab-list cleanup

`XmTabListFree` now releases zero-count lists, including lists returned by the
tab-list widget constructor before their first tab is added. It returns early
before traversing the tab ring, preserving the normal cleanup path for
non-empty lists.

## TabList growth

`XmTabbedStackListInsert` now grows its attribute array through a temporary
pointer.  Slot-count and byte-size overflow are rejected, and an Xt allocation
failure returns `XmTAB_NOT_FOUND` while preserving the existing list and its
counts.

`XmTabListInsertTabs` likewise rejects a `Cardinal` tab-count addition that
would wrap before copying the old list. `XmTabListFree` uses a `Cardinal`
iteration variable, so a valid full-width tab count cannot wrap a signed loop
counter during cleanup. The internal tab-constructor argument scan also uses a
`Cardinal` index for the full Xt argument-count range.

The focused regression is `tests/automated/check-tablist-growth.py`.

## Tab-list insertion allocation

`XmTabListInsertTabs` now checks allocation of a new list and its first copied
tab in both insertion modes. It releases the partially created list, unlocks
the process state, and returns `NULL` on failure instead of dereferencing a
null tab or list. It also rejects a copied list with a missing insertion
neighbor before splicing the detached chain.

## Transactional tab-list insertion

`XmTabListInsertTabs` now builds copied tabs as a detached chain. The existing
list is changed only after all requested tabs have been copied successfully.
If a later allocation fails, the partial chain and copied list are released and
the caller receives `NULL`; successful insertion keeps the existing ring and
marked-tab ownership behavior.

## Tab-list modification rollback

`XmTabbedStackListModify` now copies a replacement label before releasing a
copied label.  A failed allocation leaves both the existing label and its
value mode unchanged, including when a shared label is being changed to copy
ownership.  NULL remains a supported explicit clear value.

The focused source regression is
`tests/automated/check-tablist-modify-rollback.py`.

## TabList query guards

`XmTabbedStackListQuery` now ignores a `NULL` output structure and malformed
lists before copying fields.  `XmTabbedStackListSimpleQuery` validates its
list and position before using the temporary result, returning `NULL` for an
invalid query instead of reading uninitialized data.

The focused regression is `tests/automated/check-tablist-query-guards.py`.

## Transactional tab-list removal

`XmTabListRemoveTabs` now performs its mark-and-sweep operation on a copied
list. If copying fails, it returns the original list unchanged. Successful
removal frees the old list only after the new list has been prepared, and the
all-tabs-removed case releases both lists before returning `NULL`. The removal
position loop uses a `Cardinal` index matching its public position-count
argument, so a large valid position list cannot wrap a signed counter.

## Transactional tab-list replacement

`XmTabListReplacePositions` now copies the source list before applying
replacements. A failed replacement-tab allocation releases the partial copy
and returns the original list unchanged. Successful replacement keeps the
existing ring structure and replaces the original list only after all requested
positions have been processed.

## Tab-list sensitivity mask

`XmTabbedStackListInsert` now tests `XmTAB_SENSITIVE` with a bitwise mask,
matching the other tab-list attribute paths. The previous multiplication test
could treat unrelated mask bits as a request to update sensitivity, so a tab
insert could silently copy `attributes->sensitive` when the caller had not
selected that attribute. The public mask and attribute contracts are unchanged.

The focused regression is
`tests/automated/check-tablist-sensitive-mask.py`.

## TabList simple modify

`XmTabbedStackListSimpleModify` now initializes the attribute record from its
`label_string` argument before calling the general modify operation.  This
restores the helper’s documented behavior and avoids reading an uninitialized
label value.

The focused regression is `tests/automated/check-tablist-simple-modify.py`.

## TabStack child allocation

`XmTabStack` creates an internal `XmTabBox` that owns tab selection and
geometry. If Xt cannot allocate that child, initialization now skips callback
registration, geometry returns a stable answer, and later layout/resource
updates avoid dereferencing the missing TabBox. Managed tab children remain
owned by the stack and can be recovered by a later resource or lifecycle
operation without changing normal TabBox behavior.

The contract is protected by
`tests/automated/check-tabstack-child-allocation.py`.

## TabStack conversion allocation

`TabConvertProc` now checks the temporary child-pointer allocation before
writing and publishing the conversion result.  If allocation fails, the
conversion refuses the request cleanly and leaves the drag/drop callback state
unchanged.

The focused source-level regression is
`tests/automated/check-tabstack-conversion-allocation.py`.

## TabStack default-size bounds

When no explicit geometry is supplied, `XmTabStack` derives each dimension
from its shadow thickness, margin, and a fixed tab-area allowance.  The
calculation now uses widened arithmetic and clamps to the X11 `Dimension`
range before assignment, preventing large resource values from wrapping to a
small default widget.

The focused source regression is
`tests/automated/check-tabstack-default-size-bounds.py`.

## TabStack deferred selection rendering

`XmTabStackSelectTab` records a selected page while the TabStack is unrealized
and restores it when the widget gains its X window.  The restore can repaint a
page with an explicit background.  TabStack now creates its private drawing GC
before that restore runs.

If the X server cannot allocate that GC, the selection and its public callback
still complete; only the optional immediate background repaint is deferred to a
later normal expose.  This prevents null-GC drawing requests without changing
the public TabStack API or selection semantics.

## TabStack drag action guards

`XmTabBoxDragTab` now validates its invoking widget, button event, parent
chain, and TabStack before accessing drag state. It also resolves the dragged
tab child on every build configuration, preventing an uninitialized client
pointer when `TEAR_OFF_TABS` is disabled. Tear-off-only eligibility checks
remain conditional; valid drag setup is otherwise unchanged.

The focused source-contract check is
`python3 tests/automated/check-tabstack-drag-action-guards.py`.

## TabStack drag callback guards

TabStack’s drag callback now validates the callback widget, client TabStack,
and callback data before inspecting drag reasons or updating cursor icons.
Malformed callback invocations return safely; valid drag/drop cursor behavior
is unchanged.

The focused source-contract check is
`python3 tests/automated/check-tabstack-drag-callback-guards.py`.

## TabStack drag-start failure guard

TabStack drag setup now checks the result of `XmDragStart` before registering
drag callbacks. A failed drag creation returns cleanly instead of passing a
null drag widget to `XtAddCallback`; successful drag setup is unchanged.

The focused source-contract check is
`python3 tests/automated/check-tabstack-drag-start-failure.py`.

## TabStack graphics-context guard

TabStack creates a private graphics context after realization and uses it from
resize, redisplay, and tab-selection rendering.  Those paths now skip visual
operations when GC creation fails, while preserving widget layout and
selection state.  This avoids issuing X drawing requests with a null GC under
server/resource exhaustion.

## TabStack navigation API inputs

The public TabStack navigation helpers now reject null widget handles before
walking parent relationships or entering the application lock. Invalid
optional handles return the existing no-op or `NULL` result; valid tab
selection and child lookup behavior is unchanged.

The focused regression check is `tests/automated/check-tabstack-inputs.py`.

## TabStack label getter guard

The TabStack constraint-resource getter now validates its widget and output
storage before copying a child tab label. Invalid synthetic-resource callback
inputs return a deterministic null value instead of dereferencing a missing
constraint record or output slot. Normal valid child-resource reads still use
the existing copied `XmString` contract.

The focused source-contract check is
`python3 tests/automated/check-tabstack-label-guard.py`.

## TabStack tear-off label initialization rollback

When tear-off tabs are enabled, `XmTabStack` now checks the initial
`XmStringCopy` for `XmNtearOffLabelString`.  If the copy cannot be allocated,
the widget installs an empty compound string instead of propagating a NULL
label into the tear-off button and later geometry or destruction paths.

The focused regression is
`tests/automated/check-tabstack-label-init-rollback.py`.

## TabStack label rollback

`XmTabStack` now copies changed tab and tear-off labels before releasing the
current `XmString`. Failed copies preserve the existing label, while NULL
continues to mean an explicit clear.

The focused source regression is
`tests/automated/check-tabstack-label-rollback.py`.

## TabStack menu action guards

The TabStack menu action now validates its invoking widget, event, parent, and
TabStack ancestor before translating coordinates or updating tear-off state.
Malformed action invocations return safely; valid menu positioning remains
unchanged.

The focused source-contract check is
`python3 tests/automated/check-tabstack-menu-action-guards.py`.

## TabStack menu construction

The TabStack overflow menu creates push-button children for valid tabs. Menu
item creation can fail under memory pressure; the builder now stops cleanly,
guards callback registration, and checks the refreshed child count before
assigning labels or managing a button. Existing valid menu entries remain
usable and no child-array overrun is possible.

The contract is protected by
`tests/automated/check-tabstack-menu-allocation.py`.

## TabStack tear-off menu selection guard

The optional tear-off menu callback now rejects a missing callback widget or
missing user-data target before selecting a tab. Valid menu entries continue to
call `XmTabStackSelectTab` exactly as before.

The focused source-contract check is
`python3 tests/automated/check-tabstack-menu-selection-guard.py`.

## TabStack selection callback guards

TabStack’s selection callback now validates the callback widget/data and parent
TabStack before reading child state. It also stops safely when no active child
exists to replace. Valid tab selection continues through the existing active
child swap and redraw path.

The focused source-contract check is
`python3 tests/automated/check-tabstack-selection-callback-guards.py`.

## TabStack tear-off callback guard

The optional tear-off callback now rejects a missing callback widget before
querying user data. Valid tear-off callbacks retain their existing behavior.

The focused source-contract check is
`python3 tests/automated/check-tabstack-tearoff-callback-guard.py`.

## TeachingTip event guards

TeachingTip validates its callback closure and event pointer before processing
owner or popup events. Invalid teardown-time dispatch is ignored safely while
normal dismissal behavior remains unchanged.

## TeachingTip visibility API inputs

The public TeachingTip show, hide, and visibility APIs now reject a null
widget before invoking the Xt subclass check. Invalid optional handles return
the existing no-op or false result; valid delay, popup, positioning, and
dismissal callback behavior is unchanged.

The focused regression check is
`tests/automated/check-teaching-tip-inputs.py`.

## TeachingTip popup lifecycle

TeachingTip checks its transient shell, content container, and both label
children before using them.  Allocation failure destroys the partial popup and
returns cleanly.

The popup shell also clears all stored widget pointers and the visible state
when destroyed externally, so the next show can build a fresh tip.

## TearOff child-count bounds

TearOff menu handler installation/removal and submenu redraw walk Xt composite
child arrays whose count is a `Cardinal`. Those loops now use `Cardinal`
indices, preserving handler and redraw behavior while avoiding signed-index
wraparound for large menu child lists. The separate excluded-pane list keeps
its existing signed index because its legacy count is explicitly a `short`.

## Complete tear-off WM hints

Tear-off creation now checks the X property request status and requires all
five `_MWM_HINTS` items before copying the property into the outgoing hints
structure.  A short or failed property is discarded instead of silently
replacing valid window-manager function and decoration fields with zeros.

Regression: `tests/automated/check-tearoff-hints-property.py`

## Tear-off shell allocation

Tear-off menu creation now checks the transient shell returned by
`XtCreatePopupShell` before setting its title, interning window-manager atoms,
or registering callbacks.  Allocation failure returns cleanly and leaves the
original menu hierarchy intact.

## Tear-off snapshot resources

When a menu is torn off, Motif snapshots the submenu into a background
pixmap.  The snapshot path now checks the temporary graphics context and
background pixmap before copying.  If either resource cannot be created, the
operation returns after releasing the GC instead of issuing an invalid copy or
publishing a `None` background pixmap.

## Text 16-bit conversion counts

The 16-bit `_XmTextCharactersToBytes` path now stops before its legacy
integer byte count can wrap when a character emits multiple bytes. The
conversion returns the largest representable count instead of corrupting the
result length.

Regression coverage: `tests/automated/check-text-16bit-byte-count.py`.

## Text byte-conversion allocation

The two-byte Text source conversion path uses a small temporary byte buffer
while it expands each `BITS16` value.  If that allocation fails, the converter
now writes an empty result and returns instead of dereferencing a null pointer.
The normal conversion and terminator behavior is unchanged.

`tests/automated/check-text-byte-conversion-allocation.py` protects the guard
and its empty-result cleanup path.

## Text byte-conversion widths

`_XmTextBytesToCharacters` now passes validated positive lengths explicitly to
size-typed C conversion APIs and checks the `mbstowcs` result before narrowing
it to the public `int` return value. Invalid multibyte input retains the
existing `-1` result.

Regression: `tests/automated/check-text-byte-conversion-width.py`

## Text callback conversion widths

Modify-verify callback buffers now use explicit `size_t` copy lengths and pass
the checked wide-character buffer size through Xt's `Cardinal` boundary.
`mbstowcs` results are validated before narrowing to the legacy callback block
length, with invalid conversion retaining the existing empty-result behavior.

Regression: `tests/automated/check-text-callback-conversion-width.py`

## Text character-to-byte conversion counts

`_XmTextCharactersToBytes` now checks the accumulated `wctomb` byte count
before adding each converted character. An unrepresentable result stops the
conversion without wrapping the legacy integer return value.

Regression coverage: `tests/automated/check-text-character-byte-count.py`.

## Text-control preferred geometry bounds

Prompt, Link, and Badge preferred-width calculations now perform text-length
multiplication in a wide type and clamp to the platform `Dimension` maximum.
Long labels no longer wrap into unexpectedly narrow geometry requests; normal
labels retain their existing sizing.

`tests/automated/check-text-control-geometry-bounds.py` protects these paths.

## Text conversion callback inputs

The Text and TextField conversion callback wrappers now reject a null widget
or callback structure before encoding lookup, display access, or selection
field access. This keeps malformed owner requests and teardown callbacks from
crashing before the lower-level converter guards can run.

The regression check is
`tests/automated/check-text-convert-callback-inputs.py`.

## Text converter inputs

Text and TextField selection converters now reject missing widgets, selection
atoms, output pointers, and format/length storage before deriving display or
encoding state. The TextField converter also delays `XmeGetEncodingAtom` until
after validation, preventing malformed owner requests from dereferencing a
null widget.

The regression check is
`tests/automated/check-text-converter-inputs.py`.

## Text destination callback events

Text and TextField destination callbacks now reject a missing event pointer
before storing it in the shared insertion context. If a transfer ID exists,
the malformed transfer is explicitly failed rather than left pending.

Regression coverage: `tests/automated/check-text-destination-event.py`.

## Text drop callback inputs

Text drop handling now fails closed when callback structures, transfer IDs,
drag contexts, or non-empty export-target lists are missing. Valid drops keep
the existing target negotiation and transfer lifecycle.

Regression coverage: `tests/automated/check-text-drop-inputs.py`.

## Text drop transfer IDs

Text and TextField drop-transfer callbacks now reject callbacks without a
transfer ID before invoking transfer completion or requesting another target.
This keeps malformed and teardown-time callbacks from reaching the transfer
API with a null context.

Regression coverage: `tests/automated/check-text-drop-transfer-id.py`.

## Text-field drop location lifetime

The text-field destination callback exposes drop coordinates through
`XmDestinationCallbackStruct.location_data`. Those coordinates are now stored
in transfer-owned memory and released by the transfer done callback instead of
pointing at a stack-local `XPoint`. This keeps coordinates valid for the
asynchronous drop transfer and fails cleanly if the allocation cannot be made.

The focused regression is
`tests/automated/check-text-field-drop-location-lifetime.py`.

## Text search length conversion

Forward and backward Text searches now reject null or oversized search strings
before passing their byte length into the legacy `int` character-count API.
This prevents truncation from turning a large search term into an unrelated
short query.

Regression coverage: `tests/automated/check-text-find-length.py`.

## Text search conversion allocations

Forward and backward Text searches now check converted search-string sizes
against Xt's allocation width and handle allocation failure before converting
or dereferencing the temporary 16-bit and wide-character buffers.

Regression coverage: `tests/automated/check-text-find-search-allocation.py`.

## Wide-string text search bounds

`XmTextFindStringWcs` now counts the search string in `size_t`, checks the
wide-to-multibyte allocation against both `size_t` and Xt's `Cardinal` limit,
and handles a null search string or allocation failure without calling
`wcstombs`. The normal search and public return contract are unchanged.

The source contract is checked by:

```sh
python3 tests/automated/check-text-find-wcs-bounds.py
```

## Text wide search conversion

`XmTextFindStringWcs` now retains the `wcstombs` result as `size_t` until it
has validated the conversion and legacy search-length limit. This avoids
truncating a large converted byte count before the ordinary search guards run.

Regression coverage: `tests/automated/check-text-find-wcs-conversion.py`.

## Text gap-span validation

Text replacement now validates the gapped-buffer pointers and ordering before
converting their signed pointer difference to `size_t`. A malformed gap state
is rejected before editing continues, preventing a wrapped span from driving
later pointer arithmetic.

Regression: `tests/automated/check-text-gap-span.py`

## Text help-drop cleanup

Text destination handling now releases the temporary drop location allocated
for `XmDROP_HELP` actions. The help path does not start a transfer, so retaining
that allocation leaked per-drop state until the destination callback ended.

Regression coverage: `tests/automated/check-text-help-drop-cleanup.py`.

## Text highlight capacity bounds

`AddWidget` no longer performs impossible negative or host-`SIZE_MAX` checks on
the unsigned Xt `Cardinal` highlight-capacity field. The meaningful Xt
`UINT_MAX` bound remains in place before the highlight array is resized.

Regression: `tests/automated/check-text-highlight-capacity.py`

## Text insert-target inputs

Text and TextField insert-target callbacks now validate widget, closure,
target-list, and transfer context inputs before iterating targets or requesting
the selected target. This prevents malformed secondary-selection responses
from reaching the transfer API.

Regression coverage: `tests/automated/check-text-insert-target-inputs.py`.

## Text key-selection parameters

The `key-select` action now initializes its cursor and selection endpoints to
the current insertion position before parsing an action parameter.  An
unknown direction therefore becomes a harmless no-op instead of allowing an
uninitialized position or range to reach selection scanning and cursor
updates.  Valid horizontal and vertical action parameters retain their
existing behavior.

`tests/automated/check-text-key-selection.c` exercises the invalid-parameter
case against the installed library.

## Text line look-pointer guard

Text line selection uses `Look()` to obtain a character pointer. The end-of-
line guard now checks that pointer against `NULL` before dereferencing it,
instead of comparing the pointer itself with the character constant `NUL`.
This preserves normal line selection and makes end-of-buffer handling defined.

The focused regression is `tests/automated/check-text-line-look-pointer.py`.

## Unsupported Text LINK operations

Text and TextField do not implement internal `XmLINK` destination operations.
Those callbacks now explicitly complete such transfers with failure instead of
returning while the transfer remains pending.

Regression coverage: `tests/automated/check-text-link-operation.py`.

## Locale text aggregation allocation

`_XmTextToLocaleText` joins the strings returned by Xlib's text-property
conversion. The aggregate length now uses `size_t` with overflow checks,
rejects unexpected null entries, and checks the final allocation before
writing. Every failure releases Xlib's string list and reports conversion
failure through the existing `success` output.

Successful locale conversion retains the same concatenated text and ownership
contract.

## Locale-text conversion failure state

`_XmTextToLocaleText` now initializes its optional `success` result to
`False` and validates the widget and source value before obtaining atoms from
the display.  Unsupported encodings, null inputs, and conversion failures
therefore cannot leave a caller with stale success state or trigger a display
access through a null widget.

The focused regression is
`tests/automated/check-text-locale-conversion.py`.

## Locale-text property formats

STRING, COMPOUND_TEXT, and UTF8_STRING selection values are byte-oriented X
text properties.  `_XmTextToLocaleText` now rejects 16- and 32-bit format
values before passing them to `XmbTextPropertyToTextList`, preventing malformed
selection data from being interpreted with the wrong element width.

The focused regression is `tests/automated/check-text-locale-format.py`.

## Locale-text input length

Selection text lengths arrive as `unsigned long`, but the legacy Motif text
conversion path and its allocation contract are `INT_MAX`-bounded.  The
converter now rejects larger lengths before calling
`XmbTextPropertyToTextList`, preventing an oversized remote selection from
driving an unbounded Xlib decode.

The focused regression is `tests/automated/check-text-locale-length.py`.

## Text location cleanup transfer IDs

Text and TextField location-data cleanup callbacks now validate the transfer
callback ID before looking up the destination callback state. A malformed
completion callback therefore exits without querying the transfer subsystem.

Regression coverage: `tests/automated/check-text-location-cleanup-transfer-id.py`.

## Text output allocation

TextOut backward scans now validate the character count before calculating the
temporary wide-buffer size. The previous signed `(num_chars + 1)` arithmetic
could overflow before reaching the allocator; the checked `size_t` calculation
now fails the scan safely instead.

Regression coverage:

```text
python3 tests/automated/check-text-output-allocation.py
```

## Text output conversion

TextOut backward-scanning paths now preserve `mbstowcs` results as `size_t`,
reject conversion failure or an unrepresentable character count, and only
then use the checked `int` count for scanning and measuring the converted
text.

Regression coverage:

```text
python3 tests/automated/check-text-output-conversion.py
```

## Text preedit completion conversion bounds

When Text completes an overstrike preedit, the saved multibyte tail is now
copied only after each `mblen()` step succeeds.  Invalid locale sequences
produce an empty recovery fragment instead of a negative byte count or an
invalid source offset.  Valid preedit completion and callback behavior remain
unchanged.

The warning-enabled Text source build is the native smoke check for this
cleanup path.

## XmText PreeditDraw abort cleanup

When preedit verification rejects an update, or the text source rejects the
replacement, `XmText` now releases the temporary converted string and runs
the normal reset cleanup before returning.  The source-replace failure path
also restores insertion-point painting.

Regression coverage: `tests/automated/check-text-preedit-draw-cleanup.py`.

## XIM preedit feedback length

`PreeditSetRendition` iterates the feedback array using
`XIMText.length`, whose type is `int`.  Its former `unsigned short` counter
could wrap for long feedback arrays and repeat or mis-index the loop.  The
counter now matches the callback width, and empty or negative lengths are
ignored before indexing the feedback array.

`tests/automated/check-text-preedit-feedback-width.py` protects this behavior.

## XIM preedit length bounds

`XIMPreeditDrawCallbackStruct.text->length` is an `int`.  The text widget
previously copied it into an `unsigned short` before checking the 512-byte
preedit limit, allowing large values to wrap and bypass that limit.  The
callback now retains the source width, rejects negative lengths, and applies
the existing limit before using the value in geometry or allocation arithmetic.

`tests/automated/check-text-preedit-length-width.py` protects this callback
contract.

## Text preedit multibyte capacity

Wide-character preedit conversion now checks the `size_t` multiplication used
to size its multibyte buffer before calculating the capacity. Extremely large
or malformed preedit lengths therefore fail closed instead of wrapping into a
small allocation; the existing Xt `Cardinal` bound remains enforced afterward.

Regression: `tests/automated/check-text-preedit-wide-conversion.py`

## XIM preedit position bounds

The XIM preedit callback supplies caret, change-start, and change-length
positions as signed protocol fields.  Negative values are malformed and could
otherwise be added to `XmTextPosition` state before the normal range clamp.
`PreeditDraw` now rejects all three before highlighting or updating the text
source, preserving the existing behavior for valid callbacks.

`tests/automated/check-text-preedit-position-bounds.py` protects this guard.

## Text inline preedit recovery bounds

Text widget overstrike preedit recovery now validates every multibyte step
before advancing through saved text or appending recovered bytes.  Invalid
locale sequences stop recovery safely instead of moving the scan pointer
backward or producing an invalid append length.  Valid composition behavior,
callbacks, and public widget interfaces remain unchanged.

The warning-enabled Text source build is the native smoke check for this
recovery path; the XIM preedit-range regression remains available for the
shared preedit state contract.

## XIM preedit overstrike recovery width

Overstrike recovery derives its replacement and recovery ranges from
`XmTextPosition`, but the legacy callback stored them in `int` and accumulated
the recovered multibyte byte count in another `int`. Large text positions or
long recovery spans could therefore truncate before allocation. Recovery now
keeps positions in `XmTextPosition` and byte totals in `size_t`, while retaining
the existing allocation limits.

`tests/automated/check-text-preedit-recovery-width.py` protects the conversion
and accumulation widths.

## XmText preedit-start snapshot handling

Overstrike input-method startup now validates the source scan range and checks
the saved text snapshot before activating the preedit.  If the range is
invalid or the snapshot cannot be obtained, the preedit state is cleared and
startup is rejected safely.

Regression coverage: `tests/automated/check-text-preedit-start-snapshot.py`.

## XmText preedit verification cleanup

When a verify-preedit callback ends an overstrike composition early, `XmText`
now releases any temporary replacement buffer and the saved overstrike text,
then clears the preedit positions and state before returning.

Regression coverage: `tests/automated/check-text-preedit-verify-cleanup.py`.

## Text primary-selection completion widgets

Text and TextField primary-selection completion callbacks now validate the
callback widget before deriving widget state. A completion arriving during
widget teardown therefore returns safely instead of dereferencing a null
widget.

Regression coverage: `tests/automated/check-text-primary-selection-done-widget.py`.

## Text primary-selection reference counts

Text and TextField primary-selection completion paths now treat a non-positive
reference count as terminal before decrementing it. Late or duplicate
completion callbacks therefore cannot underflow the shared selection state.

Regression coverage: `tests/automated/check-text-primary-selection-refcount.py`.

## Text property conversion bounds

`XmCvtXmStringTableToTextProperty` now rejects negative counts and non-empty
tables without a table pointer. Its compound-string conversion writes the final
terminator within the allocated buffer instead of advancing past the allocation.
The source contract is covered by
`tests/automated/check-text-property-bounds.py`.

Compound-text conversion also checks the pointer-array multiplication and each
string’s contribution to the signed `nitems` total before allocating the output.
Overflow cleans up the partially converted entries and returns `BadAlloc`; the
aggregate sizing contract is covered by
`tests/automated/check-text-property-size.py`.

The compound-string (ASN.1) branch applies the same checked aggregate sizing,
rejects totals outside Xt's `Cardinal` byte range, passes an explicit
Xt-sized allocation, allocates space for its final terminator, and verifies
each conversion result before copying it. The branch contract is covered by
`tests/automated/check-compound-string-bounds.py`.

A zero-length serialization result denotes failure for a non-null compound
string, including when its byte-stream allocation fails. The property converter
returns `BadAlloc` and clears/frees its partial value; it no longer reports
success with missing serialized elements and an overstated `nitems`. Missing
payloads and results exceeding the remaining allocation are also rejected.
Serializer lengths retain their unsigned type until checked against the signed
aggregate limit. Successful ASN.1 output and the public API remain unchanged.

Locale, text, and string conversion applies the same pointer-array and signed
aggregate checks, passes explicit Xt `Cardinal` byte sizes for its pointer and
final buffers, and includes cleanup of all temporary converted strings when the
total cannot be represented. The branch contract is covered by
`tests/automated/check-locale-text-property-size.py`.

Locale/string conversion releases its temporary pointer array when any item
conversion fails, including before strict-mode compound-text fallback. Aggregate
sizing occurs after all items have been converted, so overflow releases every
item, including those after the item that exceeded the size limit. Successful
output bytes, encoding choices and fallback behavior remain unchanged.

The executable ownership regression extracts the production property import/export
functions and exercises 116 cases across the locale/string and ASN.1 styles: every
allocation failure, per-item conversion errors, oversized aggregate lengths,
strict fallback, serializer failures and successful output bytes. Allocation, length
and conversion test doubles allow overflow and rollback checks without large
buffers or an X server. Outstanding allocations and application-lock balance
must return to zero after failures; success retains only the caller's result.

```sh
python3 tests/automated/run-text-property-ownership-unit.py --out /absolute/fresh/results
```

The runner uses strict compiler warnings and ASan/UBSan and records the tested
source hashes, commands and results in `receipt.json`. This unit scope does not
qualify the locale provider's actual character conversion or X11 transfers.

ASN.1 import rollback destroys each completed `XmString` with `XmStringFree`,
including after a later decoder failure or post-decode length rejection. A raw
`XtFree` only releases the outer record and leaks nested compound-string
storage. The executable test models separately owned records and payloads,
injects failures at all seven allocations of a three-element import, rejects
each decoder position and late length check, and verifies successful ownership
transfer and cleanup. Valid imported strings and public APIs are unchanged.

An empty compound-text property still imports as one empty `XmString`. If
creating that string fails, the converter frees its temporary table and returns
`XConverterNotFound`, matching failed element conversion in the non-empty path.
It no longer returns `Success` with a null table element. The unit test covers
failure at each of the table/string/payload allocations and successful cleanup
of the valid empty result.

The reverse conversion validates output pointers, property lengths, and required
value storage before reading an X property. Compound-text element counting also
rejects `int` counter overflow and pointer-table allocation overflow. The input
contract is covered by `tests/automated/check-text-property-inputs.py`.

Compound-text import now finds segment boundaries with `memchr` limited to the
declared property length, checks Xt-sized table and segment allocations, copies
each segment into a terminated temporary string, and retains the historical
trailing empty element. Missing terminators therefore cannot make the converter
read beyond Xlib’s payload. The parser contract is covered by
`tests/automated/check-compound-text-import.py`.

Motif compound-string (ASN.1) import now verifies every encoded element length
against the property’s remaining bytes before advancing or converting it. Zero
or truncated element lengths return `XConverterNotFound` rather than looping or
reading outside the Xlib payload. The contract is covered by
`tests/automated/check-compound-string-import.py`.

An empty Motif compound-string property now returns an empty table without
dereferencing a null payload. The empty-property contract is covered by
`tests/automated/check-compound-string-empty.py`.

ASN.1 header decoding also verifies the short or long length header is fully
present before calling the legacy length helper, preventing malformed short
payloads from being read past their X property buffer. The contract is covered
by `tests/automated/check-compound-string-header.py`.

Locale, STRING, and UTF-8 import handles an empty property as one empty
`XmString` and uses non-underflowing bounds for non-empty payloads. Oversized
property lengths are rejected before the `int` element counter is used. The
contract is covered by `tests/automated/check-text-property-empty.py`.

These imports also bound each element by `nitems`, copying it into terminated
temporary storage before calling `XmStringGenerate`. A caller-supplied property
whose final item has no null terminator is therefore read only within its
declared payload. Embedded separators, trailing empty items, item order and
valid conversion behavior remain unchanged. Temporary-segment or string
allocation failure releases every previously generated item and the table.

The executable ownership regression reproduced the previous heap-buffer-overread
under ASan with an exact-sized, unterminated four-byte property. It now checks
five payload layouts in each of the locale, STRING and UTF-8 branches, verifies
the source bytes remain unchanged and returned items match, and injects failure
at all seven allocations of a two-item import. Character-conversion providers
remain test doubles; this is parsing/ownership coverage rather than live locale
or X11 service qualification.

ASN.1 property tests also execute the production `XmStringByteStreamLength`
and its header readers from `XmString.c`, alongside the importer from
`TxtPropCv.c`. Exact-sized truncated identifiers, short/long headers and missing
payloads must be rejected before the element decoder runs, including a malformed
tail after a valid element. Valid single and concatenated long-form encodings
must produce the expected items. Both production files are fingerprinted.
Restoring the old inferred-header calculation in a scratch copy reproduced an
ASan heap-buffer-overread in this integrated test. Element creation and locale
conversion remain ownership-aware test doubles; this does not qualify a whole
installed toolkit build.

## Compound-text property concatenation

Compound-text conversion now copies each segment at its known destination
offset. The text-list and fallback property paths no longer rescan the growing
result with `strcat`, so large multi-segment values retain the same output with
linear copy work.

The source contract is checked by:

```sh
python3 tests/automated/check-text-property-concat.py
```

## Ordinary Text replacement lengths

The generic Text replacement path now checks the `size_t` result of
`strlen()` before storing it in the legacy `int` block length.  Oversized input
is rejected before source state changes and the insertion point is repainted.

Regression coverage: `tests/automated/check-text-replacement-length.py`.

## Text secondary-selection target counts

Secondary-selection target negotiation now rejects external target counts that
cannot be safely processed by the legacy transfer path, frees the received
target list, and fails the transfer before scanning it.

Regression coverage: `tests/automated/check-text-secondary-target-count.py`.

## Text secondary-selection transfer failure

Malformed secondary-selection callbacks now fail an active transfer when a
transfer ID is available, instead of returning silently and leaving the
transfer pending. Null callback state remains safe when no ID exists.

Regression coverage: `tests/automated/check-text-secondary-transfer-failure.py`.

## Text secondary callback widget guards

Secondary target callbacks now validate the widget and transfer inputs before
querying widget-dependent encoding atoms. Malformed callbacks therefore fail
without entering the Xt/Xm atom lookup path.

Regression coverage: `tests/automated/check-text-secondary-widget-guard.py`.

## Text secondary-selection wrapper inputs

Text and TextField secondary-selection wrappers now validate the widget,
closure, selection callback, and transfer ID before accessing the display or
forwarding the callback. This makes malformed and teardown-time callbacks
safe at the outer transfer boundary.

Regression coverage: `tests/automated/check-text-secondary-wrapper-inputs.py`.

## Text selection-array bounds

Text input widgets copy the configurable selection scan-type array into
widget-owned storage. The copy now checks the element-count multiplication
against both host `size_t` and Xt's `Cardinal` limits and handles allocation
failure before copying. If a count is invalid or storage cannot be allocated,
the widget falls back to Motif's built-in scan-type table, preserving
traversal behavior and the existing resource/API contract. Destruction leaves
that static fallback untouched instead of passing it to `XtFree`. Input-state
creation also stops cleanly if either state allocation fails, without
dereferencing partial state. Destination-context, delayed-transfer, and
paste-location allocations likewise return safely on failure and release any
partial state.

The warning-enabled TextIn source build verifies the guarded allocation path.

## Text selection callback inputs

The Text and TextField selection-transfer callbacks now validate their widget,
selection callback, and transfer closure inputs before deriving display or
widget state. Malformed or teardown-time callbacks therefore return safely
instead of dereferencing invalid state.

Regression coverage: `tests/automated/check-text-selection-callback-inputs.py`.

## Text selection export bounds

XA_STRING, COMPOUND_TEXT, and UTF-8 selection exports now check that the Xlib
property length fits the allocator’s `size_t` and allocate one byte for an
empty value. Copies are skipped when the length is zero, avoiding null-source
`memcpy` calls while preserving the reported protocol length.

The source contract is covered by
`tests/automated/check-text-selection-export.py`.

## Text selection import bounds

Incoming selection lengths are Xlib `unsigned long` values, while Motif’s text
source block length is an `int`. Primary, clipboard, and drop imports now reject
lengths that cannot be represented before narrowing, free the transfer payload,
and report failure through the existing transfer lifecycle.
For primary imports, the existing `num_chars` hand-off is cleared so a
rejected payload cannot trigger the move/delete follow-up.

The source contract is covered by
`tests/automated/check-text-selection-import.py`.

## Text selection-loss callback inputs

The Text and TextField selection-loss callbacks now reject a null widget, a
missing selection atom, or the `None` atom before deriving display state or
changing selection ownership. This makes owner replacement and destruction
callbacks safe during incomplete teardown.

The regression check is
`tests/automated/check-text-selection-loss-inputs.py`.

## Text selection property sizes

STRING, COMPOUND_TEXT, and UTF-8 selection exports now reject X text-property
payloads larger than Xt’s `Cardinal` allocation width before copying them.
This prevents large external/property data from truncating the allocation size.

Regression coverage: `tests/automated/check-text-selection-property-size.py`.

## Text selection target counts

The external target-list callback now rejects counts larger than the legacy
selection-processing limit before iterating the supplied atom array. Normal
target negotiation is unchanged, while oversized external metadata cannot wrap
the iteration index.

Regression coverage: `tests/automated/check-text-selection-target-count.py`.

## Text selection target-list inputs

Text and TextField target-list callbacks now reject a missing target-list
buffer even when the reported target count is non-zero. This prevents a
malformed or teardown-time selection callback from dereferencing a null list.

Regression coverage: `tests/automated/check-text-selection-target-list-inputs.py`.

## Text SelectScan pointer ownership

`SelectScan` now uses its existing character pointer in the byte-oriented word
selection branch instead of redeclaring a shadowing local. This keeps the
control flow unambiguous and allows strict `-Wshadow` builds to validate the
function without suppression.

Regression: `tests/automated/check-text-selectscan-shadow.py`

## XmTextSetStringWcs conversion termination

`XmTextSetStringWcs` now explicitly terminates successful multibyte
conversions.  A conversion that fills the complete destination capacity is
treated as non-terminated and replaced with an empty string rather than being
passed to string consumers without a NUL terminator.

Regression coverage: `tests/automated/check-text-set-string-wcs-termination.py`.

## FormSection and Skeleton text updates

FormSection header/footer strings and Skeleton labels are now copied before
their old values are released. Allocation failure preserves the currently
visible text instead of replacing it with a null or empty value.

The focused source contract is checked by
`tests/automated/check-text-setvalues-transaction.py`.

## Text-source accessor guards

Text-source value replacement and the editable, selection, and maximum-length
accessors now tolerate a null or partially destroyed source. Replacement
returns without changing state; boolean getters return `False`, the maximum
length getter returns zero, and setters become no-ops. Valid source behavior
is unchanged.

The focused source contract is checked by
`tests/automated/check-text-source-set-value-bounds.py`.

## Text source block length bounds

`_XmStringSourceReadString` now checks the gap-prefix byte count before
converting it to the legacy `int` `XmTextBlock` length. An unrepresentable
prefix returns an empty block instead of truncating the pointer difference.

Regression: `tests/automated/check-text-source-block-length.py`

## Text source byte-offset bounds

The legacy gap-buffer source converts text positions into byte offsets for
multibyte and wide-character storage.  That conversion is now centralized and
checks the position, character width, source capacity, and `size_t` product
before pointer arithmetic.  Reads, gap movement, and character lookup all use
the checked helper. Replacement resizing and gap expansion also use `size_t`
byte offsets, while the public source callbacks and Motif ABI remain
unchanged. Initial and compacted gap pointers use the same widened offset
calculation.

`tests/automated/check-text-source-byte-offset.py` is the focused regression
for the checked conversion and widened gap distances.

Gap movement and source-block reads also validate their source/widget storage
before dereferencing it. During teardown or partial initialization they now
return without moving the gap, or return an empty block, rather than touching
freed or missing state.

Character lookup applies the same widget-entry check and returns an empty
result when the source is partially torn down.

## Text source Cardinal allocation bounds

`ReadSource` now rejects a block whose byte count cannot fit Xt's `Cardinal`
allocation size, after checking host-size multiplication. The validated size
is passed explicitly to `XtMalloc` and `XtRealloc`, preventing truncation on
platforms where `size_t` is wider than Xt allocation lengths.

Regression: `tests/automated/check-text-source-cardinal-allocation.py`

## Text source constructor allocation

`_XmStringSourceCreate` now treats source construction as a transaction.  A
failed source record, data record, text buffer, wide-input conversion buffer,
newline buffer, or widget array releases every earlier allocation and returns
`NULL`.  A null wide input is treated as an empty value, matching the existing
narrow-input behavior instead of being dereferenced.

The source regression in `tests/automated/check-text-source-constructor-allocation.py`
protects the allocation checks, cleanup label, and null-input handling.

## Text source conversion cursor bounds

`_XmStringSourceGetString` converts source blocks into either a byte string or
a wide-character string.  Its destination cursor now uses `size_t`, checks
the remaining allocation before each copy or conversion, and rejects invalid
wide-character conversion results.  Large representable ranges therefore
fail cleanly instead of overflowing an `int` cursor or writing beyond the
allocated result.

`tests/automated/check-text-source-cursor-bounds.py` protects these contracts.

## Text-source grapheme-step inputs

The Unicode grapheme-step helper now validates its source callback, private
data, and attached widget before dispatching or building a Unicode map. Missing
or partially torn-down state preserves the requested position instead of
dereferencing invalid storage. Custom source callbacks retain their existing
dispatch behavior.

The focused source contract is checked by
`tests/automated/check-text-source-grapheme-inputs.py`.

## Unicode text-source line access guards

Unicode line scanning now validates source storage, gap ordering, widget
state, character width, and pointer-offset arithmetic before reading a native
character. Invalid or partially destroyed text-source state returns a neutral
character instead of performing undefined pointer arithmetic.


UTF-8 locales can report `MB_CUR_MAX` greater than three; those sources use
`wchar_t` storage and must not be rejected for that width. Gap validation checks
that the gap end is inside the allocation and that its size does not exceed
its end offset. Comparing the gap size with the data capacity after the gap
incorrectly rejects ordinary sources, particularly when the gap is at the end.
`tests/automated/check-text-source-lines.c` verifies native hard-line scans and
counts before and after gap edits in the current UTF-8 locale.

## Text source range allocation

`_XmStringSourceGetString` converts a requested text range into either native
bytes or `wchar_t` elements.  The range helper now sizes the terminator in the
same units as the requested element type, so wide strings receive a complete
`wchar_t` terminator instead of one extra byte.  Both conversion paths also
return `NULL` when their allocation fails before writing through the result.

The existing position bound remains in force because the legacy source reader
uses an `int` position.  `tests/automated/check-text-source-range-allocation.py`
protects the checked size calculation and both allocation guards.
The replacement path also rejects a range whose character span cannot be
multiplied by the source character width before expanding the gap, preventing
overflow in deletion geometry.

Range extraction also rejects a missing widget or source before reading its
character width, preserving the existing `NULL` failure contract during
teardown.

The replacement callback now rejects a missing initiator, source, range, or
block before reading character storage and returns `EditError`.

## Text source set-value bounds

`_XmStringSourceSetValue` accepts a C string and stores its length in the
legacy `XmTextBlock` integer field.  It now treats a null value as an empty
string and rejects a string whose length cannot be represented by that field,
without changing selection or source state.  Representable values continue
through the existing replacement and callback path.

`tests/automated/check-text-source-set-value-bounds.py` protects the checked
conversion and null-input behavior.

## Text source value conversion

`_XmStringSourceGetValue` now uses the checked range-size helper for both byte
and wide-character result buffers.  Allocation failures return `NULL` before
copying into the result, including the empty wide-string case.  The wide
conversion also passes the source block length to `mbstowcs` as a `size_t`,
without truncating a text-position difference to `int`, and rejects a
non-monotonic source callback safely.

`tests/automated/check-text-source-value-allocation.py` protects these checks.

## Text source value cleanup

When `_XmStringSourceGetValue` cannot read the complete source range, the
byte-string branch now frees its partially filled temporary buffer before
returning failure.  Successful retrieval and wide-character handling are
unchanged.

Regression: `tests/automated/check-text-source-value-cleanup.py`

## Text-source wide conversion

The string-source wide-value initializer now rejects `wcstombs` failure and
capacity-filling output before using the temporary narrow buffer to establish
the source length. Successful output is explicitly terminated first.

Regression coverage:

```text
python3 tests/automated/check-text-source-wide-conversion.py
```

## Text string allocation widths

`_XmStringSourceGetString` now passes its already range-checked allocation
through an explicit Xt `Cardinal` conversion and uses `size_t` for block-byte
copies and accumulation. The byte conversion helper likewise makes its
positive copy length and temporary allocation widths explicit.

Regression: `tests/automated/check-text-string-allocation-width.py`

## Generic Text substring bounds

`_XmTextGetSubstring()` now rejects null buffers, negative positions/counts,
overflowing position ranges, and non-positive capacities before reading the source.  Its output checks
reserve the terminating NUL and measure wide output in destination elements
rather than bytes.

Regression coverage: `tests/automated/check-text-substring-bounds.py`.

## Text substring conversion

The shared Text substring helper now preserves the native `size_t` result from
`mbstowcs`, rejects conversion failure, and only converts the result to the
signed destination offset after bounds checking. Invalid multibyte input now
fails the copy instead of silently advancing by a wrapped or zero length.

Regression coverage:

```text
python3 tests/automated/check-text-substring-conversion.py
```

## Text value conversion

Text widget initialization now treats the `wcstombs` result as `size_t` and
rejects conversion failure before indexing the temporary buffer. This avoids
the ineffective signed `>= 0` check and only publishes a NUL-terminated value.

Regression coverage:

```text
python3 tests/automated/check-text-value-conversion.py
```

## XmText valueWcs initialization termination

When an existing `XmText` source is initialized from `XmNvalueWcs`, the
multibyte conversion now verifies that its result leaves room for the NUL
terminator before writing it.  A full-capacity conversion is rejected instead
of writing beyond the allocation.

Regression coverage: `tests/automated/check-text-value-wcs-init-termination.py`.

## Wide Text replacement allocation

The generic Text replacement path now checks the wide-string-to-multibyte byte
count before calculating the allocation size, then checks its temporary
multibyte buffer before calling `wcstombs()`.  Arithmetic or allocation
failure restores the insertion-point painting and returns without changing the
document.

Regression coverage: `tests/automated/check-text-wide-replacement-allocation.py`.

## TextField action insertion conversion

Keyboard insertion now preserves the `size_t` result from `mbstowcs`, rejects
conversion failure or lengths too large for Motif's signed replacement API,
and reserves space for the terminating NUL in the input-method lookup buffer.

Regression coverage:

```text
python3 tests/automated/check-textfield-action-insertion-conversion.py
```

## TextField activation callback lifetime

A TextField activation callback may destroy the field, its parent or its shell.
The action must not subsequently dereference the destroyed field to propagate
activation to its parent. This also applies when an application invokes
`XtCallActionProc(field, "activate", ...)` outside normal event dispatch, where
Xt may destroy the widgets immediately rather than deferring destruction.

The action observes destruction while dispatching callbacks, removes the
stack-backed observer before returning if the field survives, and skips parent
propagation after immediate or deferred destruction. Surviving fields retain
normal default-button activation through their parent manager.

`run-textfield-activation-lifetime.py` covers all three destruction levels and
normal parent dispatch through direct action calls and keyboard events. All
eight cases pass in both native font configurations. The NumberBox editing
fixture additionally covers destruction through a composite editor's numeric
and invalid-value callbacks. A pre-fix native backtrace confirmed the stale
parent dereference in TextField Activate / _XmParentProcess.

## TextField byte initial-storage bounds

TextField validation now checks the byte-string length before assigning it to
the signed `string_length` field or adding the initial growth reserve. The
allocation is narrowed to Xt's `Cardinal` only after the range checks, matching
the wide-character initialization path and preventing oversized input from
wrapping into a small buffer.

Regression coverage: `tests/automated/check-textfield-byte-initial-storage-bounds.py`.

## TextField clear-selection bounds

`clear-selection` now validates the selected span before converting it to the
existing `int` insertion-length contract.  This prevents `span + 1` from
wrapping when a malformed or externally supplied selection range is extremely
large, and protects the wide-character replacement allocation before its
element-size multiplication.  Ordinary selections keep the same replacement,
callback, and cursor behavior.

The TextField source build is compiled with warnings enabled; the previous
out-of-bounds warning in `ClearSelection` is no longer emitted.

## TextField clear-selection allocation

Clearing a multibyte TextField selection now checks its temporary wide-space
buffer before filling it.  Allocation failure repaints the insertion point and
returns without dereferencing a null buffer.

Regression coverage: `tests/automated/check-textfield-clear-selection.py`.

## TextField copy conversion

TextField substring APIs now return the actual number of characters produced
by `wcstombs`/`mbstowcs`, rather than leaving requested lengths in place when
the source contains an earlier terminator. The wide-string setter also rejects
capacity-filling conversion output and explicitly terminates valid output
before handing it to the normal setter.

Regression coverage:

```text
python3 tests/automated/check-textfield-copy-conversion.py
```

## TextField destination context allocation

TextField destination-context setup now checks both its context record and
destination record allocations before initialization, callback registration,
or `XSaveContext()`.  A partial allocation is released and setup returns
`NULL` cleanly.

Regression coverage: `tests/automated/check-textfield-destination-context.py`.

## TextField selection conversion lengths

The TextField selection callback now bounds external lengths before its legacy
integer and wide-character conversions, and fails the transfer cleanly when
temporary byte or wide-character buffers cannot be allocated.

Regression coverage: `tests/automated/check-textfield-dostuff-length.py`.

## TextField drawing conversion

Wide TextField drawing and measurement now calculate conversion capacities with
checked `size_t` arithmetic, reject `wcstombs` failure, and only cast byte
lengths to the legacy Xlib/Xft `int` APIs after bounds checking. Invalid text
is skipped instead of being rendered with a wrapped length.

Regression coverage:

```text
python3 tests/automated/check-textfield-drawing-conversion.py
```

## TextField drop byte lengths

The TextField drop path now rejects a converted or external byte length larger
than `INT_MAX` before passing it to character-counting or the legacy
replacement API.  The transfer value is released and the transfer is failed
cleanly instead of truncating the length during a conversion to `int`.

Regression coverage: `tests/automated/check-textfield-drop-byte-length.py`.

## TextField drag/drop point allocation

TextField drag/drop setup now checks the optional XPoint allocation before
writing event coordinates. If allocation fails, the transfer sink receives
the existing null location pointer instead of triggering a null dereference.

Regression coverage: tests/automated/check-textfield-drop-point-allocation.py.

## TextField drop wide-character buffers

TextField drop conversion now rejects invalid character counts before wide
buffer sizing and fails cleanly when the wide-character buffer cannot be
allocated.  Pending transfer ownership and the insertion-point repaint are
restored on those failure paths.

Regression coverage: `tests/automated/check-textfield-drop-wide-buffer.py`.

## TextField highlight-list growth

TextField highlight insertion now bounds the reallocation size and preserves
the existing highlight count/capacity if `XtRealloc()` fails.  The new record
is not written until the resized list is available.

Regression coverage: `tests/automated/check-textfield-highlight-growth.py`.

## TextField initialization storage

TextField resource validation now checks its initial highlight and input-method
storage before initialization. Allocation failure reports through the Xt error
mechanism and returns without dereferencing a null record.

Regression coverage: tests/automated/check-textfield-initial-storage.py.

## TextField input-method conversion

Input-method reset now treats `wcstombs` and `mbstowcs` results as `size_t`,
rejects failed or capacity-filling conversions, and only passes a checked
`int` character count to the replacement path. Invalid preedit text is
discarded as an empty insertion instead of being copied with a wrapped length.

Regression coverage:

```text
python3 tests/automated/check-textfield-input-method-conversion.py
```

## TextField insertion conversion lengths

External TextField insertion now rejects lengths that cannot fit its legacy
integer path or wide-character allocation, and handles failed temporary
allocations before copying or converting selection data.

Regression coverage: `tests/automated/check-textfield-insert-length.py`.

## TextField locale-converted insertion lengths

TextField insertion now checks the size_t length returned by locale
conversion before converting it to the legacy int character count. The
wide-character allocation path also checks the length before adding its
terminator, preventing truncation and size arithmetic overflow.

Regression coverage: tests/automated/check-textfield-insert-locale-length.py.

## TextField modify-verify result allocations

When a modify-verify callback changes the proposed text, `TextField` allocates
the callback result in either multibyte or wide-character form.  Allocation
failure now rejects that edit and releases the temporary callback buffers
without dereferencing or returning a null result.

Regression coverage: `tests/automated/check-textfield-modify-verify-allocations.py`.
## TextField modify-verify result allocations

Modify-verify callback results now validate narrow-string lengths before
adding the terminating byte and narrowing the size for `XtMalloc`. This
matches the existing wide-string guards and keeps oversized callback output
on the failure path without truncating the allocation size.

Regression coverage: `tests/automated/check-textfield-modify-verify-allocations.py`.

## TextField modify-verify conversion

The TextField modify-verify bridge now preserves the native `size_t` results
from `wcstombs` and `mbstowcs`. Failed conversions and lengths that cannot fit
Motif's signed callback fields are rejected or converted to an empty callback
payload instead of being tested as negative `int` values and wrapped.

Regression coverage:

```text
python3 tests/automated/check-textfield-modify-verify-conversion.py
```

## TextField modify-verify wide allocation

The TextField modify-verify bridge now checks callback-provided wide-character
lengths before calculating temporary buffer sizes. Oversized lengths fail via
the existing allocation-failure path instead of wrapping `(length + 1)` or the
wide-character byte multiplication.

Regression check:

```text
python3 tests/automated/check-textfield-modifyverify-wide-allocation.py
```

## TextField narrow-value validation

When wide input is converted to single-byte TextField storage, validation now
checks the destination allocation before calling `wcstombs()`.  Failure clears
the value state, releases temporary storage, and returns safely.

Regression coverage: `tests/automated/check-textfield-narrow-validation.py`.

## TextField overstrike wide-buffer limit

The overstrike preedit setup now checks the wide suffix size against both
`size_t` and Xt's `Cardinal` limit before allocating the saved text. This
prevents a valid-looking position range from narrowing to an undersized Xt
allocation on platforms where `Cardinal` is smaller than `size_t`.

Regression check:

```text
python3 tests/automated/check-textfield-overstrike-wide-limit.py
```

## TextField portability warnings

TextField passed a null pointer through an integer parameter cast when
registering its input method, and initialized a wide-character value by
casting a pointer constant to `wchar_t`.  Those expressions are diagnostic-
prone on 64-bit builds even though they produced zero on common targets.

The calls now pass integer zero to the unsigned `XmImRegister` parameter and
wide-character zero to the `wchar_t` storage.  Runtime behavior and the public
ABI are unchanged, while strict 64-bit compiler diagnostics are removed.

`tests/automated/check-textfield-portability-warnings.py` protects these
type-correct constants.

## TextField preedit allocation failures

TextField's XIM preedit path creates temporary multibyte, wide-character, and
tab buffers.  The path now checks every allocation before writing into it and
releases the already-created buffer if the second empty-preedit allocation
fails.  Valid preedit rendering is unchanged; low-memory callbacks return
cleanly instead of dereferencing NULL.

`tests/automated/check-textfield-preedit-allocations.py` protects these guards.

## TextField preedit append allocation

Narrow overstrike preedit appends now validate the existing string length and
the appended suffix before calling `XtRealloc`. Both the `size_t` addition and
the Xt `Cardinal` narrowing are checked, so malformed preedit state cannot
wrap an append buffer size.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-append-allocation.py
```

## TextField preedit conversion

Input-method preedit updates now validate all multibyte/wide conversions before
using their buffers. Converted strings are explicitly terminated, and invalid
recovery text is discarded instead of being concatenated from uninitialized
storage or being accepted through a signed `< 0` check on a `size_t` result.

Regression coverage:

```text
python3 tests/automated/check-textfield-preedit-conversion.py
```

## TextField PreeditDraw caret restoration

`PreeditDraw` hides the insertion point while it updates the preedit.  Early
allocation failures in the font-metric and empty-preedit paths now restore the
insertion point before returning, so a transient allocation failure cannot
leave the TextField caret invisible.

Regression coverage: `tests/automated/check-textfield-preedit-draw-caret.py`.

## TextField empty preedit overstrike

The wide-character preedit path now creates an owned empty string when an XIM
draw callback has no text payload but overstrike recovery data still needs to
be appended. This prevents `wcslen` from dereferencing a null buffer while
preserving the existing recovery behavior.

The callback also rejects null XIM client or draw records before reading their
fields.

Regression coverage: `tests/automated/check-textfield-preedit-empty-overstrike.py`.

## TextField multibyte preedit allocation

The XIM preedit path now sizes narrow buffers from the multibyte string's
actual byte length. It no longer treats the advertised character count as a
byte count before copying with `strcpy`, preventing overflow for multibyte
locales while retaining the existing callback behavior.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-multibyte-allocation.py
```

## TextField wide-preedit multibyte capacity

When a wide XIM preedit is rendered through a single-byte TextField path, a
character can require multiple locale bytes.  The old temporary buffer used
only `insert_length + 1` bytes and could truncate the conversion.  The buffer
now reserves `MB_CUR_MAX` bytes per input character, checks the `Cardinal`
limit, and handles conversion failure before continuing.

`tests/automated/check-textfield-preedit-multibyte-capacity.py` protects this
Unicode conversion contract.

## TextField overstrike preedit allocations

The TextField overstrike path now checks its remaining-byte and
remaining-wide-character allocations, as well as replacement buffers, before
copying or converting into them. On failure it releases existing temporary
preedit storage and returns without dereferencing NULL.

`tests/automated/check-textfield-preedit-overstrike-allocations.py` protects
the allocation guards.

## TextField XIM preedit position bounds

The TextField XIM callback now rejects negative caret, change-start, and
change-length fields before updating its preedit range.  Negative or empty
feedback lengths are also ignored by the rendition helper, and negative text
lengths are rejected before the existing preedit-size limit.  Valid callbacks
retain the existing Motif behavior.

`tests/automated/check-textfield-preedit-position-bounds.py` protects these
guards.

## TextField preedit reallocation safety

The overstrike preedit path grows byte and wide-character buffers for the
remaining text and recovered text. It now reallocates through temporary
pointers and checks failure before replacing the live buffer. A failed growth
therefore releases the old buffer and other temporary storage cleanly instead
of leaving a lost pointer followed by a NULL write.

`tests/automated/check-textfield-preedit-realloc.py` protects the transactional
reallocation pattern.

## TextField preedit recovery allocations

The wide-character overstrike recovery path now checks both its temporary
multibyte conversion buffer and its recovered wide-character buffer before
writing into them.  Failed recovery releases the current wide buffer and
returns without continuing the preedit update.

`tests/automated/check-textfield-preedit-recovery-allocations.py` protects these
guards.

## TextField XIM overstrike recovery width

TextField overstrike recovery derives its span from `XmTextPosition`, but the
legacy callback stored that span in `int` and used an `int` index while
assembling the restored text.  The recovery span now retains the position
width and the local index uses `size_t`, preventing truncation for large text
fields while preserving normal preedit behavior.

`tests/automated/check-textfield-preedit-recovery-width.py` protects this
contract.

## TextField inline preedit recovery

When overstrike preedit recovery converts the saved wide-character tail back
to the locale encoding, the conversion buffer is now retained separately from
the cursor used to scan past completed multibyte characters.  This prevents an
invalid interior-pointer free during composition updates.  Invalid conversion
or multibyte boundaries stop recovery cleanly while preserving the active
preedit buffer and widget ABI.

The TextField source is compiled as part of the native library build.  The
portable preedit-state regression remains the smoke check for composition
state ownership on builds that provide the optional font backends.

## TextField preedit repaint on failure

The TextField replacement and preedit replacement paths now repaint the
insertion point before returning from allocation-size or reallocation
failures.  A failed update no longer leaves the caret hidden after the routine
temporarily disables it.

Regression coverage: `tests/automated/check-textfield-preedit-repaint.py`.

## TextField preedit-start overstrike allocations

Starting an input-method preedit in overstrike mode now validates the snapshot
length and checks both narrow and wide snapshot allocations before copying
text.  An allocation failure leaves the preedit inactive instead of writing
through a null pointer.

Regression coverage: `tests/automated/check-textfield-preedit-start-allocations.py`.

## TextField overstrike preedit suffix allocation

The overstrike preedit path now validates its suffix range before allocating
the preserved text. Negative position differences and counts that exceed Xt's
`Cardinal` or `size_t` limits are rejected before either narrow- or
wide-character buffer arithmetic is evaluated.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-suffix-allocation.py
```

## TextField wide preedit append allocation

Wide overstrike preedit appends now validate the combined character count and
byte size before `XtRealloc`. This prevents wrapped sums or an oversized
wide-character allocation from narrowing into an invalid Xt `Cardinal`.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-wide-append-allocation.py
```

## TextField wide preedit copy

The XIM wide-preedit path now copies exactly the character count supplied by
the input method and appends its own terminator. It no longer relies on
`wcscpy` finding a NUL within an externally supplied payload.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-wide-copy.py
```

## TextField replacement growth bounds

TextField replacement buffers now use one checked growth helper that rejects
signed-length overflow, doubling overflow, and wide-character byte conversion
overflow before calling Xt. This preserves the current text and returns
failure rather than wrapping an allocation size.

`tests/automated/check-textfield-replacement-growth-bounds.py` protects both
normal and preedit replacement paths.

## TextField replacement reallocation safety

Both normal and preedit TextField replacement paths now grow byte and
wide-character storage through temporary pointers.  Allocation failure returns
`False` before the field publishes a lost or NULL buffer; successful growth
publishes the new capacity and pointer together.

`tests/automated/check-textfield-replacement-realloc.py` protects both
replacement paths.

## TextField input-method reset allocations

Resetting an active preedit now handles allocation failures and invalid
multibyte conversions without dereferencing null buffers or indexing with a
negative conversion result.  The input-method state and insertion-point
painting are restored on each failure path. The multibyte length remains a
`size_t` until it has been checked against the preedit limit before narrowing
to the legacy `int` insertion count.

Regression coverage: `tests/automated/check-textfield-reset-ic-allocations.py`.

## TextField preedit reset allocation

`TextFieldResetIC` now validates the preedit range before allocating its
temporary wide-character buffers. The length and byte-size calculations are
checked for signed-range, `size_t`, and Xt `Cardinal` limits, preventing an
invalid preedit state from wrapping an allocation size.

Regression check:

```text
python3 tests/automated/check-textfield-resetic-allocation.py
```

## TextField selection export buffers

TextField selection conversion now uses one bounded allocation helper for
temporary byte strings and checks every allocation before copying, converting,
or terminating the buffer.  A length that cannot fit the toolkit allocation
type, or an allocation failure, returns `False` without dereferencing an
invalid buffer.

Regression coverage: `tests/automated/check-textfield-selection-buffers.py`.

## TextField selection export bounds

TextField narrow selection export now treats zero or negative `mblen()` results
as conversion failure instead of allowing a non-progressing loop or unsigned
length underflow.  Wide selection export checks the element count before
multiplying by `sizeof(wchar_t)`.  Valid narrow and wide selections preserve
their existing contents and ownership behavior.

The selection timestamp regression now covers both TextField export formats.
