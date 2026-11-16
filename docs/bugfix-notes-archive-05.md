---
nav_exclude: true
---
# Archived Bugfix and Hardening Notes (Part 5)

Consolidated archive of standalone bugfix and hardening validation notes.

## MWM gadget-state input validation

`PushGadgetIn` and `PopGadgetOut` return without changing state when passed a
null `ClientData` pointer.  This guard runs before decoration flags, display
lists, or global gadget state are touched; valid gadget interactions are
unchanged.

The source-contract regression is
`tests/automated/check-mwm-gadget-state-inputs.py`.

## MWM help-argument bounds

`WmDtGetHelpArgs` keeps its existing ABI and `MAXWMPATH + 1` output-buffer
contract.  It now validates all required pointers, initializes successful
outputs deterministically, and accepts a parsed topic or volume only when it
fits that established limit.  Oversized tokens reject the complete parsed
argument set with empty outputs instead of overflowing the caller's buffer or
leaving a misleading partial argument count.

Well-formed help arguments retain their original topic, volume, and argument
count. The focused source-level regression is
`tests/automated/check-mwm-help-argument-bounds.py`.

## MWM icon-box widget creation

MWM now checks icon-box item widget creation before applying the placeholder
label, callbacks, or window-dependent state. If allocation fails, icon-box
initialization returns through its existing failure path instead of using a
null child.

`tests/automated/check-mwm-icon-widget-creation.py` protects this path.

## MWM `IncludeCommand` message lengths

MWM now validates the received IPC length before unpacking the command header
and before reading the advertised window list. Truncated messages are rejected
without reading beyond the received buffer; complete messages retain the
existing include-command behavior.

## MWM invoke-command input

`F_InvokeCommand` now parses its command ID, client window, and selection
fields individually with overflow and trailing-input checks. Values must fit
their protocol types; malformed input is rejected before any narrowing or
`SendInvokeMessage` call.

## MWM key-substitution bounds

Localized MWM key substitutions now validate table growth against Xt's
allocation width and retain parsed token lengths in `size_t` until their
documented `int` representation is proven safe.

The replacement token is also limited so that its trailing NUL fits in Xt's
`Cardinal` allocation width. Invalid or unrepresentable replacements discard
the pending source token and leave the already completed substitutions intact.

## MWM menu-button count bounds

MWM derives popup-menu button storage from configured menu-item lists. The
count now rejects an unrepresentable item before incrementing and verifies that
the final allocation fits Xt's `Cardinal` byte count. A pathological resource
fails cleanly without wrapping the count;
ordinary menu construction is unchanged.

`tests/automated/check-mwm-menu-button-count-bounds.py` protects this
boundary.

## MWM menu label conversion

Menu construction now checks localized label and accelerator-text conversion
before passing results to Motif widgets. Failed conversions omit only the
affected presentation string while preserving the menu item and accelerator
action; successful labels, accelerators, and menu behavior are unchanged.
Menu renaming also skips the widget update if `XmStringGenerate` fails after
the internal name has been updated.

`tests/automated/check-mwm-menu-label-conversion.py` protects this path.

## MWM duplicated menu-name rollback

MWM now allocates duplicated system-menu names before releasing the existing
name. If allocation fails, the duplicate keeps its original name and remains
usable instead of carrying a NULL menu name.

The focused source regression is
`tests/automated/check-mwm-menu-name-rollback.py`.

## MWM menu-widget recreation bounds

When a client command changes, MWM collects affected menu specifications,
destroys their old widgets, and recreates them. The temporary list now tracks
its count in `size_t`, rejects an impossible count before it can wrap, and
checks native and Xt `Cardinal` allocation widths. Valid menu recreation order
and callback behavior are unchanged.

`tests/automated/check-mwm-menu-recreation-bounds.py` protects this boundary.

## MWM menu-command whitespace

The menu-command matcher now casts label and command bytes to `unsigned char`
before calling `isspace`. This keeps matching defined for high-bit bytes in
localized menu strings while preserving the existing whitespace rules.

## MWM menu widget creation

MWM now checks label, separator, cascade, and push-button gadget creation
before applying sensitivity, bookkeeping, or callbacks. If widget allocation
fails, menu construction returns through its existing failure path instead of
dereferencing a null child. Successful menu creation is unchanged.

`tests/automated/check-mwm-menu-widget-creation.py` protects this path.

## Mwm numeric KeySym parsing

Numeric KeySym details in Mwm event bindings are parsed through the same
checked `StrToNum` helper used for numeric window-manager function arguments.
`StrToNum` reports success separately from the converted value, so
`ParseKeySym` now passes its output pointer explicitly and assigns the parsed
value only after conversion succeeds. Invalid or overflowing numeric details
continue to be rejected as `NoSymbol`.

The parser also passes the first token byte to `isdigit` as an unsigned
character, as required by the C character-classification interface. The
focused regression check is
`tests/automated/check-wm-resparse-keysym-numbers.py`; an x86 Mwm rebuild
compiles the real parser.

## MWM Panelist function-argument validation

The exported Panelist `ParseWmFunctionArg` helper now clears a supplied output
argument and rejects missing parser/output pointers, negative function indexes,
indexes beyond the fixed MWM function table, and table entries without a parser.
Valid function indexes and their parsing behavior are unchanged.

The focused regression is
`tests/automated/check-mwm-panelist-function-arguments.py`.

## Motif WM property status handling

`XmIsMotifWMRunning` now checks the status returned by
`XGetWindowProperty` before examining the returned property metadata.  An X
error therefore produces the normal `False` result instead of allowing
uninitialized type, format, or length values to influence WM detection.

The query also requires a complete non-null property value, and initializes the
optional `XQueryTree` child-list result before cleanup. Truncated properties and
failed tree queries therefore produce the normal `False` result without
dereferencing or freeing invalid storage.

Regression: `tests/automated/check-mwm-property-status.py`

## MWM `RemoveCommand` message lengths

`RemoveCommand` now validates its command ID, window count, and window-list
bytes before processing the request. Truncated IPC payloads are rejected
without reading beyond the received message; complete requests are unchanged.

## MWM `RenameCommand` strings

`RenameCommand` now bounds its length-prefixed name and validates the remaining
window-list bytes before unpacking them. Truncated or malformed requests are
rejected without tree mutation, and the decoded temporary name is released on
all exits.

## MWM request-copy allocation

`mwm` copies each received desktop request before using the shared parser
buffer. If that copy cannot be allocated, it now reports the failure, releases
the received request, and skips only that request. It does not pass a null
string to the parser or disturb subsequent requests.

```sh
python3 tests/automated/check-mwm-request-copy-allocation.py
```

## MWM screen-function arguments

The numeric form of MWM's `f.screen` function now uses complete, range-checked
decimal parsing. Invalid, overflowing, or trailing-text arguments are rejected
with a warning instead of being silently converted to screen zero. The named
forms `next`, `prev`, and `back` retain their existing behavior.

## MWM selection conversion defaults

`WMiConvert` now rejects null widget or output arguments before using them and
initializes `outputType` to `None` alongside the existing empty-buffer
defaults. Target-specific conversion branches can then set their documented
types, while invalid or unsupported requests return deterministic state.

The focused regression check is
`tests/automated/check-mwm-selection-conversion-guards.py`.

## MWM session item counts

The session-file record count is now parsed as a complete, positive integer
within the representable range before `GetSessionHintsInfo` allocates storage.
Malformed, overflowing, zero, and negative counts are rejected with a warning,
so they cannot wrap into an unrelated allocation size.

## MWM session database-name classification

When MWM increments a numeric session database suffix, it now casts each
filename byte to `unsigned char` before calling `isdigit`. This keeps the
character classification defined for filenames containing bytes above 127
while preserving the existing suffix behavior.

## MWM session-resource integers

Restored MWM session state contains coordinates, dimensions, and icon
positions supplied by a resource database. These values are now parsed as
complete, range-checked integers. Malformed or overflowing values are ignored
without setting the corresponding restore flag, so a bad session file cannot
silently reposition or resize a client to zero.

## MWM session persistence input boundaries

MWM session save and restore helpers can run while session shutdown or client
teardown has left only a partial manager state.  The public callback and
database-entry paths now fail closed when their top-level shell, screen array,
client record, client screen, or workspace-list state is absent.

A valid client still serializes the same geometry, state, identity, and
workspace/icon data.  If a workspace list is absent, the valid client record
is retained but its unavailable workspace-specific icon entries are omitted.
`FindClientDBMatch` now initializes its optional workspace-name output to null,
and a failed database close is reported as a failed checkpoint instead of
loading a potentially incomplete file.

Focused check:

```
python3 tests/automated/check-mwm-session-save-inputs.py
```

## MWM system-menu placement input validation

`GetSystemMenuPosition` is a legacy `void` helper.  It now returns without
writing coordinates when its client record or either output pointer is null.
Normal minimized, icon-box, and framed-client placement calculations remain
unchanged for valid calls.

The source-contract regression is
`tests/automated/check-mwm-system-menu-position-inputs.py`.

## MWM TARGETS conversion allocation guard

MWM’s selection conversion now checks its fixed TARGETS atom-array allocation
before writing protocol atoms. Allocation failure returns a failed conversion
with the already-initialized empty output fields.

The focused regression is
`tests/automated/check-mwm-targets-conversion-allocation.py`.

## MWM title-text rectangle input validation

`GetTextBox` now returns without writing when passed a null `ClientData` or
`XRectangle` pointer.  This guard precedes title-area lookup and its fallback
rectangle writes; valid title-text geometry is unchanged.

The source-contract regression is
`tests/automated/check-mwm-text-box-inputs.py`.

## MWM title fallback

When a managed client has no readable `WM_NAME`, MWM builds a title from its
client name. If that `XmString` allocation fails, it now falls back to the
existing shared default title before later measurement or drawing. The normal
localized title path is unchanged.

`tests/automated/check-mwm-title-fallback.py` protects this path.

## MWM title-metric input validation

`TitleTextHeight` and `InitTitleBarHeight` return zero for a null
`ClientData` pointer.  This avoids reading appearance or decoration state from
an absent client; valid title and title-bar metrics are unchanged.

The source-contract regression is
`tests/automated/check-mwm-title-metrics-inputs.py`.

## MWM title-width bounds

`GetFramePartInfo` checks the total horizontal space required by frame borders
and title-bar gadgets before calculating title, system-menu, minimize, or
maximize geometry.  If the frame is too narrow, the function returns `FALSE`
without writing wrapped unsigned widths or x-coordinates.  Valid frames retain
the previous geometry calculations.

Resize-corner spans use the same checked subtraction: when twice the corner
size exceeds the frame dimension, the interior span is zero rather than a
wrapped unsigned value.

East and south edge placement also rejects frames smaller than the lower
border, preventing wrapped edge coordinates.

Northeast and southeast corner coordinates clamp to zero when their computed
span exceeds the corresponding frame dimension.

The title child itself now rejects an initial frame narrower than both upper
borders, and later reconfiguration skips that invalid subtraction.

The source-contract regression is
`tests/automated/check-mwm-title-width-bounds.py`.

## MWM virtual-key binding lifetime

The virtual-key repair path copies X11 binding properties into Xt-owned,
null-terminated storage before parsing them. MWM now releases that storage only
with `XtFree` and exactly once on each path, avoiding allocator mismatches and
a fallback-path double free.

```sh
python3 tests/automated/check-mwm-virtkeys-binding-lifetime.py
```

## MWM virtual-screen request bounds

The WSM PAN request contains two 32-bit coordinates and one byte; GOTO contains
two 32-bit coordinates. `WMiConvert` now checks the 8-bit protocol format and
minimum payload lengths before unpacking either request. Invalid requests are
ignored safely, while valid virtual-screen operations retain their protocol
behavior.

The focused regression check is
`tests/automated/check-mwm-virtual-screen-request-guards.py`.

## MWM workspace-resource build guards

Workspace resource tables only exist in MWM builds that include workspace
management. Non-WSM builds now skip those lookups entirely instead of applying
`XtNumber` to their null placeholder pointers. This preserves the WSM resource
behavior and prevents incorrect pointer-size counts in ordinary MWM builds.

```sh
python3 tests/automated/check-mwm-workspace-resource-build-guards.py
```

## Resource-name comparison input validation

`XmeNamesAreEqual` now returns `False` when either input string is missing,
avoiding an invalid dereference. Existing Motif-prefix and case-insensitive
comparisons remain unchanged for valid strings.

The focused regression check is
`tests/automated/check-name-compare-inputs.py`.

## Named-table converter input validation

Button-type, keysym-table, and charset-table converters now validate source and
destination values (and required displays) before parsing. Their cleanup
callbacks also tolerate missing destination addresses. Valid table conversion
and cleanup remain unchanged.

The focused regression check is
`tests/automated/check-named-table-converter-inputs.py`.

## Navigability trait guards

Manager, Primitive, and Gadget navigability callbacks now validate their
widget class before reading traversal and sensitivity state. Invalid inputs
return `XmNOT_NAVIGABLE`; valid widgets retain the existing tab/control
navigability decisions.

The focused source-contract check is
`python3 tests/automated/check-navigability-trait-guards.py`.

## Navigation mutator teardown guards

TreeView top-row selection, BreadcrumbBar current-index selection, and the
Sidebar toggle entry point now reject widgets once Xt destruction begins.
This keeps late key, pointer, and application calls from recalculating layout,
invalidating a drawable, or reading navigation state during teardown.

`tests/automated/check-navigation-mutator-destroy-guards.py` checks the three
public entry points.

## Parse callback input validation

`XmeGetNextCharacter` now treats missing input/output pointers and negative
pattern lengths as a no-op (`XmINSERT`). Valid callbacks retain the existing
component parsing behavior, while malformed application parse callbacks no
longer trigger an assertion or pointer arithmetic on invalid data.

The focused regression check is
`tests/automated/check-next-character-inputs.py`.

## Icon-place lookup guards

`GetNextIconPlace` now returns `NO_ICON_PLACE` when placement storage is
missing or empty.  The forward position conversion also rejects indices past
the placement list, avoiding invalid reads while preserving its zero-position
fallback.

`tests/automated/check-next-place-guards.py` protects these bounds.

## Notebook scroller allocation

Notebook creates tab-scroller arrows and a default page-scroller pair as
internal children. Creation can fail under resource pressure or a custom Xt
class configuration. The notebook now checks each child before trait callbacks,
manage/unmanage operations, and layout use. A missing page-scroller child
removes the partial scroller and disables default-scroller updates; tabs and
pages remain usable without navigation arrows.

The contract is protected by
`tests/automated/check-notebook-child-allocation.py`.

## Notebook child-count traversal

Notebook redisplay, join-side updates, page-number assignment, active-child
tracking, and child lookup use `Cardinal` indices for the composite child
count. The page-number helper accepts the same full-width index, avoiding
signed-index wraparound when a valid notebook has many children.

The focused source regression is
`tests/automated/check-notebook-child-count-bounds.py`.

## Notebook child lookup constraint guard

The internal Notebook child lookup now skips null composite children and
children without constraint storage before reading page/type metadata. This
keeps malformed child state from crashing tab and page discovery while leaving
valid lookup behavior unchanged.

The source contract is checked by
`tests/automated/check-notebook-child-widget-constraint-guard.py`.

## Notebook drawing bounds

Notebook binding and frame drawing subtracts borders from the current frame
allocation.  During a transient tiny allocation those signed extents can be
zero or negative; keeping them in `Dimension` would wrap them into enormous
X11 rectangles.  The drawing paths now use signed extents and return without
drawing when no positive area remains.

The source contract is checked by
`tests/automated/check-notebook-drawing-bounds.py`.

## Notebook layout constraint guards

Page, major-tab, and minor-tab layout scans now skip null composite children
and children without constraint storage before reading Notebook metadata. This
prevents malformed child state from crashing relayout while preserving normal
geometry and tab behavior.

The source contract is checked by
`tests/automated/check-notebook-layout-constraint-guards.py`.

## Notebook negative-dimension bounds

`XmNotebook` normalizes geometry requests that use negative widths or heights
by moving the origin and taking the dimension's absolute value. The old macro
performed both operations in `int`, making `INT_MIN` undefined and allowing
the resulting `Position` or `Dimension` to wrap.

The normalization now uses signed wide arithmetic and clamps the resulting
origin and dimension to Motif's representable ranges. Valid geometry requests
retain their existing placement and size.

The focused source regression is
`tests/automated/check-notebook-negative-dimension-bounds.py`.

## Notebook page constraint guard

`XmNotebookGetPageInfo` now skips a missing child or child constraint record
while scanning page metadata. This keeps malformed composite state from being
dereferenced during lookup; valid Notebook page and tab metadata remains
unchanged.

The source contract is checked by
`tests/automated/check-notebook-page-constraint-guard.py`.

## Notebook page and scroller coordinates

`XmNotebook` positions the page scroller relative to the active page.  The
scroller may be wider than a temporarily narrow page, so the position
calculation can legitimately be negative.  Layout coordinates now use signed
`int` values instead of `Dimension`; this prevents unsigned subtraction from
wrapping a negative position into a far-away coordinate.

The source contract is checked by
`tests/automated/check-notebook-page-coordinates.py`.

## Notebook page-number constraint guard

Automatic page-number allocation now skips null children and children without
constraint storage before reading their page numbers. Valid page allocation
is unchanged, while malformed composite state no longer reaches an unchecked
constraint dereference.

The source contract is checked by
`tests/automated/check-notebook-page-number-constraint-guard.py`.

## Notebook orientation hook guards

Notebook's horizontal/vertical pixel conversion hooks now validate the widget
class and callback value storage before reading orientation state. Invalid
exports return deterministic zero output; invalid imports return
`XmSYNTHETIC_NONE`. Valid Notebook subclasses retain their existing orientation
dependent conversion behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-notebook-pixel-hook-guards.py
```

## Notebook pixmap binding metadata

Notebook pixmap bindings now distinguish a valid uncached metadata fetch from
an unresolved pixmap.  `XmeGetPixmapData` may return `False` after fetching a
valid pixmap into the cache, so binding drawing now validates the initialized
depth instead of using the Boolean cache-status result as an error signal.

The focused regression is `tests/automated/check-notebook-pixmap-binding.py`.

## Notebook binding pixmap metadata

Notebook binding rendering now checks the result of `XmeGetPixmapData` before
using the returned pixmap depth.  If metadata cannot be resolved, the binding
draw is skipped safely instead of branching on an uninitialized depth value.

Successful lookups retain the existing stippled-versus-tiled GC behavior.  The
focused regression is `tests/automated/check-notebook-pixmap-metadata.py`.

## Notebook redisplay constraint guards

Notebook redisplay now skips null composite children and children without
constraint storage while counting major and minor tabs. Expose handling no
longer dereferences malformed child state; normal tab-count and scroller
behavior is unchanged.

The source contract is checked by
`tests/automated/check-notebook-redisplay-constraint-guards.py`.

## Notebook scroll-frame allocation handling

Notebook’s scroll-frame trait initializer now checks its private
`XmScrollFrameData` allocation before initializing fields.  Allocation failure
leaves the trait uninitialized and returns cleanly, allowing the caller to
report an unavailable scroll frame instead of dereferencing null state.

The regression check is `tests/automated/check-notebook-scrollframe-allocation.py`.

## Notebook shrinking geometry

Notebook layout reduces its remaining width or height as page, scroller,
binding, and tab regions are minimized.  Those values are toolkit
`Dimension`s; subtracting a region larger than the remaining space used to
wrap to a huge value and could make later child sizes grow during a shrink.

The shrinking path now saturates each subtraction at zero. Normal layouts keep
their existing sizes, while very small notebooks reduce regions in a
deterministic order without unsigned underflow.

## Notebook spiral pixmap creation

Notebook creates a server-side pixmap for its binding spiral and immediately
renders several arcs and lines into it.  The creation result is now checked
before rendering.  On failure the cached spiral is reset to
`XmUNSPECIFIED_PIXMAP`, so later expose handling can retry or omit the optional
decoration without issuing drawing requests against `None`.

## Notebook status-area geometry

Notebook status width now subtracts margins and scrollers in signed arithmetic
before clamping.  Collapsed pages therefore keep zero-sized status geometry
instead of wrapping to a large unsigned dimension; existing status/scroller
height selection is retained.

The source-level regression is
`tests/automated/check-notebook-status-geometry.py`.

## Notebook unhighlight constraint guard

The Notebook unhighlight callback now requires both a non-null child and
constraint storage before selecting the appropriate background GC. Malformed
child state safely returns no GC instead of dereferencing an absent Notebook
constraint; normal focus and tab painting are unchanged.

The source contract is checked by
`tests/automated/check-notebook-unhighlight-constraint-guard.py`.

## Notebook widget-class guard

`XmNotebookGetPageInfo` now rejects `NULL` and non-Notebook widget handles
before looking up the application context or reading child constraints. A
malformed caller therefore receives `XmPAGE_INVALID` instead of risking a
read through an unrelated widget's private layout state. Valid Notebook page
queries are unchanged.

The source contract is checked by
`tests/automated/check-notebook-widget-class-guard.py`.

## NumberBox state API inputs

The public NumberBox range, value, step, and invalid-state APIs now reject a
null widget before invoking the Xt subclass check. Invalid optional handles
return the existing neutral values or no-op result; valid normalization,
formatting, redraw, and callback behavior is unchanged.

The focused regression check is
`tests/automated/check-number-box-inputs.py`.

## Number-box text-length bounds

`XmNumberBox` now bounds editable text lengths before passing them to
`XDrawString`. Normal input rendering is unchanged; unusually long text no
longer wraps the signed Xlib length argument.

The focused regression check is
`tests/automated/check-number-box-text-bounds.py`.

## NumberBox arithmetic bounds

NumberBox range and step resources are `int`, while the old step operation
used `long` for its intermediate.  On platforms where `long` is 32-bit,
stepping at an extreme value could overflow before the range or wrap clamp
ran.  The operation now uses `int64_t`, preserving behavior across 32-bit and
64-bit platforms.

`tests/automated/check-numberbox-arithmetic.py` keeps the fixed-width
arithmetic in the step path.

## Numeric resource-converter input validation

Boolean-dimension, cardinal, text-position, and top-item-position converters
now reject missing source/destination values (and missing widget context for
unit conversion) before parsing or dereferencing them. Valid numeric and
boolean conversions retain their existing behavior.

The focused regression check is
`tests/automated/check-numeric-converter-inputs.py`.

## Option-menu gadget lookup guards

`XmOptionLabelGadget` and `XmOptionButtonGadget` now return `NULL` when their
menu widget argument is null, before application-context lookup. Valid option
menus retain their existing child-discovery behavior.

The focused regression is
`tests/automated/check-option-gadget-inputs.py`.

## Motif path-resolution inputs

`_XmOSAbsolutePathName` now rejects null path, output-pointer, and scratch
buffer arguments before accessing them. Valid absolute and relative paths keep
their existing behavior; malformed icon/resource lookup inputs return failure
instead of dereferencing invalid pointers.

## Outline count bounds

Outline constraint argument scanning and recursive node-child traversal now
use Xt `Cardinal` counters matching their `num_args` and hierarchy child
counts. Constraint updates and hierarchy layout behavior are unchanged while
large valid lists avoid signed-counter wraparound.

## Outline child-width geometry

Outline negotiates child widths from the parent width minus each child’s
horizontal position.  The available width is now calculated as a signed
integer and clamped to one pixel before it is used in Xt geometry requests.
Deeply indented or temporarily out-of-bounds children therefore receive a
small valid request instead of a wrapped near-maximum width.

## Icon-box packing guards

`PackIconBox` now returns when its widgets, icon increments, or placement list
are unavailable.  This prevents invalid Xt access and divide-by-zero during
packing while preserving normal behavior for initialized icon boxes.

`tests/automated/check-pack-icon-box-guards.py` protects the contract.

## Paned sash action parameters

The legacy `XmPaned` sash action accepts its keyboard increment as text from
the translation table.  That value is stored in a signed `short`; the old
`atoi` conversion could wrap an oversized or malformed parameter and move the
sash in the wrong direction or by an unexpected amount.

The action now uses a checked `strtol` conversion.  Values outside the
representable `short` range, missing values, and other malformed strings use
the existing default behavior: `LargeIncr` selects ten pixels and every other
zero/default value selects one pixel.  Valid signed values retain their prior
behavior and the public action/resource ABI is unchanged.

`tests/automated/check-paned-action-parameters.py` guards the conversion
contract and prevents the unchecked `atoi` path from returning.

## Paned child spans

Paned now computes child width and height through a signed, bounded span
helper.  When margins and borders exceed a collapsed Paned container, child
geometry remains one pixel instead of wrapping an unsigned dimension
subtraction into a huge size.

The source-level regression is `tests/automated/check-paned-child-spans.py`.

## Legacy PanedWindow managed-child growth

The legacy `XmPanedWidget` managed-child rebuild now grows its array through a
temporary pointer. Capacity addition and byte-size conversion are checked
against both `size_t` and Xt's `Cardinal` allocator width; failed growth leaves
the existing array and capacity unchanged and stops the rebuild safely.

The focused source regression is
`tests/automated/check-paned-legacy-managed-children.py`.

## PanedWindow managed-child allocation handling

PanedWindow now treats its managed-child list allocation as optional during
initialization and checks growth allocations in `ReManageChildren`. Capacity
arithmetic is bounded against host `size_t`, the `Cardinal` slot count, and
Xt's allocation width. A failed growth leaves the existing list intact and
stops rebuilding safely instead of indexing null or partially resized storage.

The regression check is `tests/automated/check-paned-managed-children-allocation.py`.

## Paned margin geometry bounds

Changing an `XmPanedWindow` horizontal margin adjusts the container width by
the difference between the old and new doubled margins.  That delta is now
computed in signed wide arithmetic and clamped to the X11 `Dimension` range
before assignment.  Large increases no longer wrap to a small width, and
large decreases no longer wrap to a huge width.

The focused source regression is
`tests/automated/check-paned-margin-bounds.py`.

## Paned pixel hook guards

Paned's parent, child, and opposite-orientation pixel converters now validate
their widget/value inputs and, for child resources, the Paned parent class
before reading orientation state. Invalid exports return safely; invalid
imports return `XmSYNTHETIC_NONE` without changing widget state.

The focused source-contract check is:

```sh
python3 tests/automated/check-paned-pixel-hook-guards.py
```

## XmPaned preferred-size bounds

`XmPaned` computes its preferred major-axis size by adding pane dimensions,
borders, sash spacing, and margins. Those values are `Dimension`-sized, so
performing each addition directly could wrap a large layout before the final
window-size calculation.

The accumulation now saturates every addition at the maximum X11
`Dimension`. Normal layouts retain their existing geometry; oversized layouts
remain bounded instead of becoming a small or otherwise invalid request.

The focused source regression is
`tests/automated/check-paned-preferred-size-bounds.py`.

## Paned refigure and margin safety

XmPaned now keeps narrow geometry negotiation in the representable Dimension
range in both directions. Adding margins saturates, while removing them floors
at zero instead of wrapping into a very large size. This applies to parent
negotiation, returned geometry, preferred geometry, reset, and set-values
refigure paths.

The no-unused-space fallback also handles an all-pane refigure (NO_INDEX)
without indexing before the pane array. It uses the last pane for a global
refigure and the first pane when a specified first pane has no predecessor.

The focused regression check is
tests/automated/check-paned-refigure-margin-safety.py.

## Paned right-to-left pane origins

Right-to-left horizontal Paned layout now computes each pane origin from
signed container-width and accumulated-position values before converting to
`Position`.  Overfull or collapsed containers therefore keep an off-edge
origin instead of wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-paned-rtl-origin.py`.

## Paned negative sash indents

Paned sash placement now computes negative-indent coordinates in signed
arithmetic before converting to `Position`.  Narrow containers therefore keep
the intended off-edge sash location instead of wrapping to a distant positive
coordinate.

The source-level regression is `tests/automated/check-paned-sash-indent.py`.

## Paned regular sash positions

Paned now computes regular horizontal and vertical sash coordinates in signed
arithmetic before converting to `Position`.  Narrow panes and negative
off-edge placements no longer risk mixed signed and unsigned wraparound.

The source-level regression is `tests/automated/check-paned-sash-positions.py`.

## Paned separator positions

Paned separator x and y coordinates now use signed intermediates before
conversion to `Position`.  Overfull or collapsed panes therefore keep valid
off-edge separator positions instead of wrapping mixed signed and unsigned
dimension arithmetic.

The source-level regression is
`tests/automated/check-paned-separator-positions.py`.

## PanedWindow pane-stack allocation handling

PanedWindow’s private pane stack now checks its node allocation before linking
and initializing the node.  Allocation failure leaves the existing stack
unchanged and returns from the optional stack push safely.

The regression check is `tests/automated/check-paned-stack-allocation.py`.

## PanedWindow child-count traversal

The maintained `XmPanedWindow` implementation now uses full-width `Cardinal`
indices when updating pane positions, clipping sash regions, and rebuilding
managed-child lists. This matches Xt's composite child-count field and avoids
signed-index wraparound in large PanedWindow hierarchies.

The focused source regression is
`tests/automated/check-paned-window-child-count-bounds.py`.

## PanedWindow separator allocation

When `XmPanedWindow` toggles `XmNseparatorOn`, it collects the separator
gadgets for all pane children before managing or unmanaging them.  The
temporary list now uses the full child count as its upper bound, checks the
allocation-size arithmetic against host `size_t` and Xt's `Cardinal` limits,
and verifies the allocation before collecting entries.

This fixes both the old `num_children / 3` under-allocation (which could write
past the list for valid pane layouts) and the null dereference on allocation
failure.  If the temporary list cannot be allocated, separator visibility is
left unchanged while other `SetValues` processing continues.

## Parse-mapping ArgList validation

`XmParseMappingGetValues` now rejects a missing `ArgList` when entries were
requested and skips entries without a name or destination pointer. Valid
mapping fields retain their existing behavior, while malformed application
metadata no longer causes a null dereference.

The focused regression check is
`tests/automated/check-parse-mapping-args.py`.

## Parse-table cleanup

`XmParseTableFree` treats a `NULL` parse-table pointer as an empty table and
returns safely, even when a nonzero count is supplied. Existing tables still
have every mapping freed and are released as before.

The focused regression check is
`tests/automated/check-parse-table-free-null.py`.

## XmeParseUnits input validation

`XmeParseUnits` now rejects a null unit specification or output pointer with
`XmPARSE_ERROR` before inspecting the string.  Callers can therefore pass
malformed resource data through the normal parse-failure path safely.

## Picker and popup geometry replies

Calendar, DatePicker, TimePicker, and TeachingTip retain their existing
preferred dimensions.  Their geometry query methods now delegate the reply to
`XmeReplyToQueryGeometry`, which evaluates only the width/height fields a
parent actually requested.  This makes partial and query-only Xt negotiations
standards-compliant without changing normal picker layout.

## DatePicker and TimePicker render-table updates

DatePicker and TimePicker now treat render-table replacement transactionally.
When copying a requested table fails, the existing table remains owned and in
use, and the setter reports no render-table change. The previous implementation
freed the valid table before checking the replacement result.

The focused source contract is checked by
`tests/automated/check-picker-render-table-transaction.py`.

## Picture accepted-text buffer growth

XmPicture parsing appends accepted characters to a state-owned string buffer.
The previous path assigned `XtRealloc`'s result directly and doubled the
capacity without checking the actual required terminator space.

Growth now computes the required length, expands until it fits, and updates
the buffer and append pointer only after allocation succeeds.  Failed growth
returns a parse failure while preserving the existing state allocation.

## Picture matcher character classification

Picture matching classifies and uppercases input bytes with the C ctype
functions.  Every such call now receives an `unsigned char` value, so bytes
outside ASCII no longer invoke undefined behavior when a picture is matched.

## Picture parser null input

`XmParsePicture` now returns `NULL` when given no specification, before it
attempts to copy or parse the input string.  This makes malformed caller input
fail safely.

## Picture state allocation

Picture parsing and state construction now treat allocation as a transaction.
`XmParsePicture` checks its picture, source-copy, and initial node-table
allocations before entering the parser, releasing earlier allocations on
failure.  `XmGetNewPictureState` now treats state construction as a
transaction.  It
rejects a null picture and node counts that cannot be represented by the
ABI-preserved `int statesize` field, and checks every allocation before the
state is initialized or published.  If a later allocation fails, all earlier
allocations are released and the function returns `NULL`.

The state-vector size is computed in `size_t` before conversion to `int`, so a
large or malformed picture cannot overflow the old `num_nodes * sizeof(char)`
expression.  Existing callers that receive a valid state retain the same
layout and behavior; callers must continue to handle the documented `NULL`
result from allocation failure.  The public processing, deletion, query, and
auto-fill helpers also reject null handles, making cleanup and optional
processing paths safe without changing valid-handle behavior.  Auto-fill also
stops and returns the current partial string when character processing cannot
grow its buffer, instead of retrying indefinitely. `XmPictureProcessCharacter`
also initializes its valid `is_finished` output to `False` before processing,
so a later growth failure cannot leave callers with a stale completion value.
Growth is also limited to Xt's `Cardinal` byte range before the reallocation,
so a large `size_t` capacity cannot be truncated at the allocator boundary.

The source-level regression check is
`tests/automated/check-picture-state-allocation.py`.

## PixConv pixmap metadata guard

`_XmGetPixmapBasedGC` now configures a tile or stipple GC only when
`XmeGetPixmapData` successfully resolves the pixmap metadata.  A lookup failure
leaves the existing foreground/background GC in place, avoiding an
uninitialized depth decision and an invalid pixmap resource in the GC.

The focused regression is
`tests/automated/check-pixconv-pixmap-metadata.py`.

## Pixel converter input validation

Rendition-pixel converters now validate displays, argument counts, source and
destination values before conversion or warning dispatch. The shared integer
parser also safely rejects missing strings or result pointers. Valid pixel and
special-value conversions remain unchanged.

The direct `XmRPixel` converter now copies a complete `Pixel` object after
checking the supplied source width. It no longer reads the resource through an
`int`, which could truncate a 64-bit pixel value or reinterpret a high-bit
color as a negative signed integer.

The focused regression check is
`tests/automated/check-pixel-converter-inputs.py`.

## Scaled pixmap dual-table insertion

Scaled pixmap entries are published in both the pixmap and pixmap-data hash
tables. Publication is now transactional: if either bucket insertion fails,
the first insertion is removed, the X11 pixmap is released, and the pending
entry is cleaned up instead of leaving inconsistent cache state.

The focused regression is `tests/automated/check-pixmap-dual-insertion.py`.

## Pixmap wrapper input guards

The public `XmGetScaledPixmap`, `XmGetPixmapByDepth`, and `XmGetPixmap`
wrappers now reject a null widget or screen, and a null image name, before
accessing Xt or X11 state. Invalid input returns `XmUNSPECIFIED_PIXMAP`;
valid calls retain their existing cache and locking behavior.

The focused regression is `tests/automated/check-xm-pixmap-wrapper-inputs.py`.

## Icon placement conversion guards

Icon placement conversion now validates placement dimensions, cell sizes, and
root-window coordinate arrays before doing division or indexing.  Invalid
state produces `(0, 0)` for the position conversion or `NO_ICON_PLACE` for the
reverse conversion.

`tests/automated/check-placement-conversion-guards.py` protects these bounds.

## PNG conversion failure cleanup

The PNG converter now treats 8-bit indexed visuals as an explicit unsupported
format instead of returning success with unwritten pixels.  Whenever pixel
conversion fails, the newly allocated `XImage` is destroyed and the caller's
pointer is cleared before the error is returned.  Normal 16/24/32-bit output
is unchanged, while failed loads no longer leak or expose invalid image data.

The regression check is
`tests/automated/check-png-error-image-cleanup.py`.

## PNG `SCREEN_GAMMA` input

Motif's PNG decoder accepts the optional `SCREEN_GAMMA` environment variable
for applications that need to override the display exponent. The value is
accepted only when it is a complete, finite, strictly positive decimal number.
Malformed, overflowing, NaN, infinity, zero, and negative values are ignored
and the decoder keeps its platform-derived default exponent. This prevents a
process environment from sending invalid gamma values to libpng.

## Anchored popup position arithmetic

`XmAutoSuggestBox`, `XmFlyout`, and `XmTeachingTip` now calculate anchor,
offset, and screen-edge coordinates in `long long` values.  They clamp the result before converting to
Xt's signed `Position` type, so large virtual screens and resource offsets no
longer wrap a popup around the display.

`tests/automated/check-popup-position-arithmetic.py` protects the source-level
contract.  The installed-prefix AutoSuggestBox and Flyout Xvfb fixtures remain
the runtime coverage for popup creation, placement, and dismissal.

## Popup post-list input guards

`XmAddToPostFromList` and `XmRemoveFromPostFromList` now return immediately
for a null menu widget, before casting or application-context lookup. Valid
popup and pulldown menus retain their existing post-from-list behavior.

The focused regression is
`tests/automated/check-popup-post-list-inputs.py`.

## Primitive class-extension allocation

`XmPrimitive` class initialization creates a private class-extension record
when a derived widget does not provide one.  The initializer now checks the
`XtCalloc` result and returns without dereferencing a missing extension.  This
keeps class registration fail-closed under allocation pressure while
preserving the existing extension inheritance and public widget ABI when the
allocation succeeds.

The guard applies to the shared Primitive class path, so all widgets derived
from `XmPrimitive` receive the same failure behavior.

## Primitive string-converter input validation

Character and keysym converters now reject missing source/output values, and
the character converter also rejects an empty source buffer before reading its
first byte. Valid conversions and destination-size handling remain unchanged.

The focused regression check is
`tests/automated/check-primitive-converter-inputs.py`.

## Primitive coordinate getter guards

Primitive's synthetic `x` and `y` exports now validate widget and output
storage before querying parent/shell geometry. Invalid calls return a
deterministic zero value; normal coordinate conversion remains unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-primitive-coordinate-getter-guards.py
```

## Primitive tooltip hook guards

Primitive's tooltip synthetic import/export hooks now validate widget and value
storage before calling tooltip APIs. Invalid exports return a deterministic
null string; invalid imports return `XmSYNTHETIC_NONE` without changing state.

The focused source-contract check is:

```sh
python3 tests/automated/check-primitive-tooltip-hook-guards.py
```

## Primitive trait getter guards

Primitive’s direction, access-colors, and unit-type trait callbacks now
validate their widget class and color-output structure before reading instance
state. Invalid inputs return safely (`XmDEFAULT_DIRECTION` or `XmPIXELS`, or
no color write); valid primitive subclasses retain their configured values.

The focused source-contract check is
`python3 tests/automated/check-primitive-trait-getter-guards.py`.

## PrintShell callback setup

`XmPrintToFile` allocates and initializes its parent status callback before
forking the document writer. This prevents callback allocation failure from
returning failure while a child continues writing. Fork failure releases the
callback, closes both pipe ends, and removes the output file.

## PrintShell child failure status

The `XmPrintToFile` child now reports setup failures through the existing
status pipe. Allocation or display-open failures close and remove the partial
file before reporting `XPGetDocError`. The parent also treats a truncated or
interrupted status read as an error rather than consuming an uninitialized
status value.

## PrintShell output-file inputs

`XmPrintToFile` now rejects a null display, null path, or empty path before
opening the output. If child creation fails after opening a valid path, the
empty file is removed before returning failure. Valid output paths retain the
existing print workflow and callback behavior.

## PrintShell file-output status

`XmPrintToFile` now checks every child-side `fwrite` and the final `fclose`.
When either operation fails, the finish callback reports `XPGetDocError` and
removes the partial output file. Successful X Print completion keeps the
existing file and status behavior.

## Print setup lifecycle

`XmPrintSetup` now rejects null inputs before walking the video widget's
ancestor chain. It also checks application-shell and print-shell allocation,
destroying the temporary application shell when print-shell creation fails.

Successful print setup retains the existing callback, realization, and mapped
state behavior.

## PrintS child descriptor allocation

The child side of `XmPrintToFile` creates a descriptor before opening its
application display.  The descriptor and copied file name are now checked;
allocation failure releases any partial state and exits the child cleanly
instead of dereferencing a null descriptor or passing a null name into the
printing callbacks.

The regression check is `tests/automated/check-prints-child-allocation.py`.

## Private highlight segment bounds

The historical `_XmDrawHighlight` renderer is still used by legacy List and
IconG paths.  It creates X11 `XSegment` records, whose endpoints are signed
16-bit values.  Thickness is now limited to the supplied rectangle and every
derived endpoint is saturated before storage.  Normal highlight rendering is
unchanged; extreme geometry can no longer wrap a segment across the drawable.

## ProgressBar coordinate bounds

ProgressBar keeps its bar origin in signed `int` arithmetic and clamps label
and percentage text coordinates before converting them to Motif's signed
`Position` type.  Very large spacing or font metrics are therefore clipped
safely instead of wrapping to an unrelated screen coordinate.

`tests/automated/check-progress-bar-coordinate-bounds.py` protects this
coordinate contract.

## ProgressBar graphics-context setup

ProgressBar requires its track and fill graphics contexts as a pair. Its
initialization now rolls back every partially created GC when either required
allocation fails, preventing leaked X resources and leaving redisplay in a
consistent no-drawing state. The optional text GC remains an independent
fallback: text rendering can use the track GC when it is unavailable.

The focused source contract is checked by
`tests/automated/check-progress-bar-gc-setup.py`.

## ProgressBar state API inputs

The public ProgressBar range, value, and indeterminate-state APIs now reject a
null widget before invoking the Xt subclass check. Invalid optional handles
return the existing neutral values or no-op result; valid range normalization,
redraw, and callback behavior are unchanged.

The focused regression check is
`tests/automated/check-progress-bar-inputs.py`.

## Progress-bar text allocation guard

`XmProgressBar` now checks the optional percentage `XmString` before measuring,
drawing, or freeing it. If allocation fails, the bar remains rendered and only
the percentage text is omitted for that redisplay.

The focused regression check is
`tests/automated/check-progress-bar-text-allocation.py`.

## ProgressRing coordinate bounds

ProgressRing now keeps its arc center in signed `int` arithmetic until the X
arc is drawn.  Text and label positions are clamped before conversion to
Motif's signed `Position` type.  Large spacing or DPI-derived dimensions can
therefore be clipped safely instead of wrapping to an unrelated screen
coordinate.

`tests/automated/check-progress-ring-coordinate-bounds.py` protects this
coordinate contract.

## ProgressRing graphics-context setup

ProgressRing now releases any graphics contexts already created when one of its
three GC allocations fails, then resets all GC fields. Expose and destroy paths
can therefore handle partial server-resource setup without stale handles.

The focused source contract is checked by
`tests/automated/check-progress-ring-gc-setup.py`.

## Progress-ring geometry bounds

`XmProgressRing` now clamps default and queried width/height calculations after
combining the ring diameter, label extent, and spacing. Normal layouts are
unchanged; large labels or spacing values no longer wrap geometry requests.

The focused regression check is
`tests/automated/check-progress-ring-geometry-bounds.py`.

## ProgressRing geometry with an empty label

ProgressRing geometry negotiation and redisplay now measure label metrics only
when both the font and XmString label are available. Clearing a label or an
allocation failure therefore yields the normal ring geometry without a null
XmString query.

The focused source contract is checked by
`tests/automated/check-progress-ring-geometry-label.py`.

## ProgressRing state API inputs

The public ProgressRing range, value, and indeterminate-state APIs now reject
a null widget before invoking the Xt subclass check. Invalid optional handles
return the existing neutral values or no-op result; valid normalization,
redraw, and callback behavior are unchanged.

The focused regression check is
`tests/automated/check-progress-ring-inputs.py`.

## ProgressRing label centering

ProgressRing now computes label Y offsets in signed arithmetic. If a label is
taller than the ring, its negative centering offset remains negative instead
of underflowing the unsigned `Dimension` subtraction and placing the label far
outside the widget.

## ProgressRing label initialization

ProgressRing now falls back to an empty XmString when copying an initial label
fails. The widget remains safely renderable instead of carrying a null label
into metric and draw paths.

The focused source contract is checked by
`tests/automated/check-progress-ring-label-init.py`.

## ProgressRing render bounds

ProgressRing keeps the ring center and circular text-opening calculations in
signed working coordinates.  It only converts text origins through the
existing bounded `Position` helper, and squares an explicitly positive opening
width when checking whether the label fits.

## ProgressRing resource failure handling

ProgressRing now keeps its previous label when a replacement XmString cannot
be copied, and uses the track GC for text whenever the sensitive text GC is
unavailable. A partial resource failure therefore preserves visible text
instead of freeing the old label or issuing a draw with a null GC.

The focused source contract is checked by
`tests/automated/check-progress-ring-resources.py`.

## Progress-ring text allocation guard

`XmProgressRing` now checks the optional percentage `XmString` before measuring
or drawing it. If allocation fails, the ring and track remain rendered and only
the percentage text is omitted for that redisplay.

The focused regression check is
`tests/automated/check-progress-ring-text-allocation.py`.

## ProgressBar label initialization rollback

ProgressBar now checks the private copy of its initial label.  If
`XmStringCopy` fails, it installs an empty compound string and avoids
measuring a NULL label, keeping construction and geometry calculation safe
under allocation pressure.

The focused regression is
`tests/automated/check-progressbar-label-init-rollback.py`.

## ProgressBar label rollback

`XmProgressBar` now copies a changed label before releasing the current
`XmString`. Allocation failure preserves the existing label, while NULL still
explicitly clears it.

The focused source regression is
`tests/automated/check-progressbar-label-rollback.py`.

## ProgressBar geometry negotiation

`XmProgressBar` computes preferred dimensions from its label, style, and
spacing.  Those dimensions are a fallback for an unallocated widget; an
explicit application allocation is now retained by `QueryGeometry` and
negotiated through `XmeReplyToQueryGeometry`.

This avoids a parent geometry query silently reverting a deliberately narrow,
wide, short, or tall progress indicator to its content-derived default.

The modern percentage-text overlay also clamps its filled clip segment to the
actual progress track and converts clip coordinates through the toolkit's
bounded X11 `Position` helper.  Large but representable widget allocations
therefore cannot wrap the signed clip origin or extend its unfilled segment.

## ProgressBar preferred-size bounds

ProgressBar's preferred width and height include label metrics and twice the
spacing resource.  The old additions were performed in `Dimension` and could
wrap for large spacing or font metrics.  Initialization and `QueryGeometry`
now compute in `size_t` and saturate at the maximum representable
`Dimension`.

`tests/automated/check-progressbar-size-bounds.py` keeps the bounded sizing
helper in both paths.

## XmPrompt live color updates

`XmPrompt` now updates its drawing GC when foreground or background resources
change through `SetValues`, and requests a repaint for that transition. Live
theme or resource changes therefore take effect immediately instead of leaving
stale text colors in an already-created prompt.

The focused source contract is checked by
`tests/automated/check-prompt-color-updates.py`.

## Prompt coordinate bounds

Prompt clamps its text origin before passing it to Motif's signed `Position`
drawing arguments.  Large prompt heights or font metrics therefore cannot
wrap the text to an unrelated screen coordinate.

`tests/automated/check-prompt-coordinate-bounds.py` protects this drawing
contract.

## XmPrompt geometry

`XmPrompt` now reports a preferred height based on its XmString render-table
metrics, with the same padding used during initialization. This prevents
larger fonts and complex-script text from being clipped by the old fixed
24-pixel geometry; the 24-pixel default remains for an empty prompt.

The focused source contract is checked by
`tests/automated/check-prompt-geometry.py`.

## Prompt visibility API inputs

The public Prompt visibility setter and getter now reject a null widget before
invoking the Xt subclass check. Invalid optional handles return the existing
no-op or false result; valid visibility and redraw behavior is unchanged.

The focused regression check is `tests/automated/check-prompt-inputs.py`.

## Prompt padded extents

Prompt derives its preferred size from rendered text plus layout padding.
Initialization and geometry queries now use one saturating helper for both
width and height, preventing a very large rendered height from wrapping before
the padding is added.

The contract is protected by
`tests/automated/check-prompt-padded-extents.py`.

## XmPrompt render-table rollback

When a prompt receives a new font list, it now releases the old render table
only after `XmFontListCopy` succeeds. A failed replacement keeps the previous
valid table and does not request a font-driven redraw.

The focused source contract is checked by
`tests/automated/check-prompt-render-table-rollback.py`.

## Prompt resource and GC safety

`XmPrompt` now skips drawing when its private `GC` cannot be created, avoiding
a null-GC Xlib request. Prompt text replacements release the previous owned
`XmString` only after a replacement succeeds, retaining the old value if
copying fails. Successful rendering and resource updates are unchanged.

`tests/automated/check-prompt-resource-safety.py` protects these paths.

## Prompt text-length bounds

`XmPrompt` now bounds user-provided prompt text lengths before passing them to
`XDrawString`. Normal prompt rendering is unchanged; unusually long text no
longer wraps the signed Xlib length argument.

The focused regression check is
`tests/automated/check-prompt-text-bounds.py`.

## Property buffer guards

Counted property serializers now reject a missing data buffer when the count
is nonzero. Empty counts remain valid and continue to replace the property
with an empty value, preserving normal workspace and embedded-client behavior.

## Protocol registration allocation

Adding shell protocols allocates one per-protocol record before publishing the
new count. If a record allocation fails partway through a batch, the partially
created records are released and the manager keeps its original protocol
count, so later registration can retry without a leaked or half-initialized
entry.

The source-level regression is
`tests/automated/check-protocol-allocation.py`.


Batch growth now rejects `Cardinal` count and byte-size overflow and publishes
the expanded capacity only after `XtRealloc` succeeds.

## Protocol callback registration

`XmAddProtocolCallback` now checks the protocol lookup again after attempting
to create a missing protocol entry. Allocation failure therefore returns
cleanly instead of dereferencing a null entry; successful callback
registration is unchanged.

The focused regression check is
`tests/automated/check-protocol-callback-allocation.py`.

## Protocol lookup null slots

Protocol lookup now skips null entries in a partially initialized protocol
array. Valid entries continue to match normally, while malformed internal
state no longer causes a null dereference during lookup or dispatch.

The focused regression check is
`tests/automated/check-protocol-lookup-null-slots.py`.

## Protocol manager allocation

Protocol registration now treats manager creation and manager-list growth as
transactional.  A failed manager allocation or list resize leaves the
existing per-shell protocol state unchanged, and callers stop cleanly instead
of dereferencing a null manager.  The initial per-shell manager allocation is
also checked before installing callbacks or saving the X context.

Valid protocol registration and callback behavior are unchanged.  The focused
source-level regression is
`tests/automated/check-protocol-manager-allocation.py`.

## Protocol-manager removal index safety

When a vendor shell was destroyed with more than one protocol manager,
`RemoveProtocolMgr` reused one loop variable for both the protocol entries and
the manager list.  The manager compaction therefore started at the number of
protocols instead of the removed manager's index, leaving a freed manager in
the live list and crashing during destruction.

The cleanup now keeps independent manager and protocol indices.  This retains
the public protocol API and makes multi-manager shell destruction safe.  The
focused source check is
`tests/automated/check-protocol-manager-removal-index.py`; the installed C++
consumer lifecycle fixture exercises the destruction path.

## Protocol-manager teardown

Protocol-manager removal now validates membership before freeing a manager,
compacts the owner list, and decrements `num_protocol_mgrs`. Destroy-time
teardown removes the first remaining manager until the list is empty, avoiding
stale pointers and skipped entries.

The focused regression check is
`tests/automated/check-protocol-manager-removal.py`.

## Protocol null-slot handling

Protocol-manager teardown now skips null protocol slots. This keeps partially
initialized arrays from causing null dereferences while preserving normal
cleanup for valid protocol entries.

The focused regression check is
`tests/automated/check-protocol-null-slots.py`.

## Protocol property publication

Protocol property updates now skip null manager and protocol slots and safely
return when a nonempty manager has no backing protocol array. Valid active
protocols are published exactly as before.

The focused regression check is
`tests/automated/check-protocol-publication-null-slots.py`.

## Public mutator teardown guards

Public state-changing helpers must be safe when applications receive a late
event or callback while Xt is destroying the target widget.  Disclosure
expansion, RowActions reveal, TeachingTip hide, and Flyout hide now reject
that state before changing child layout, popup visibility, timers, or callback
delivery.  Query APIs retain their established, conservative teardown values.

`tests/automated/check-public-mutator-destroy-guards.py` keeps this contract
explicit for the affected entry points.

## Public query teardown guards

Public widget queries return their existing neutral values while Xt is
destroying the target: counts and generations return zero, indexes return
`-1`, booleans return `False`, and child/string handles return `NULL`.
AutoSuggestBox dismissal follows the same rule.  The remaining composite
queries—including Toast, Prompt, FormSection, and EmptyState—use the same
contract.  Deferred Toast and AutoSuggestBox timer callbacks check teardown
before clearing timer state or dismissing their popups.  This prevents late
application callbacks from observing or forwarding partially disposed child
and model state.

`tests/automated/check-public-query-destroy-guards.py` records the affected
component APIs.

## PushButton tear-off release shadow bounds

The PushButton tear-off release path can redraw a depressed shadow after menu
focus and geometry have changed.  Its `BtnUp` shadow call now requires positive
interior dimensions and both shadow GCs, converting the core dimensions before
subtracting the highlight frame.

Normal tear-off release feedback keeps its existing shadow type and focus
behavior.  The focused regression is
`tests/automated/check-push-button-btnup-shadow.py`.

## PushButton default shadow bounds

`XmPushButton` chooses either the external highlight thickness or the internal
3D enhancement as the default-button shadow delta.  Its previous bounds check
allowed the internal delta to exceed a small widget before subtracting it from
`Dimension` values.

The draw path now rejects dimensions no larger than the selected delta before
computing the inner rectangle.  Valid default-button shadows keep their
existing emphasis and colors.

## PushButtonGadget default shadow bounds

`XmPushButtonGadget` uses either the external highlight thickness or the
internal 3D enhancement as the default-button shadow delta.  The previous
check only compared dimensions with the highlight thickness, so the internal
delta could exceed a small gadget and produce invalid subtraction results.

The draw and clear paths now reject rectangles no larger than the selected
delta before computing the inner shadow rectangle.  Valid default-button
shadows retain their existing colors and emphasis behavior.

## PushButton highlight bounds

`XmPushButton` reduces its highlight thickness for default-button shadows and
uses an internal focus delta for the highlight rectangle.  Small thickness or
widget values previously underflowed unsigned arithmetic and could pass an
invalid rectangle or missing GC to `XmeDrawHighlight`.

The focus path now rejects underflow-prone thickness, missing highlight GCs,
and rectangles consumed by the internal delta.  Valid focus highlighting is
unchanged.

## PushButton menu shadow bounds

The PushButton menu border-highlight and enter callbacks now draw shadows only
when the highlight frame leaves a positive interior and both shadow GCs exist.
The core dimensions are converted before subtraction, so undersized or
partially initialized menu entries cannot pass invalid rectangles to X11.

Normal-sized menu entries retain their existing shadow type and callback order.
The focused regression is
`tests/automated/check-push-button-menu-shadows.py`.

## PushButton menu unhighlight guards

Menu `XmPushButton` unhighlighting either fills the whole widget for etched-in
menus or clears the inner border.  The path previously assumed a background GC
and subtracted highlight margins even during small geometry transitions.

The fill and clear requests now validate their respective resources and
geometry.  Label redraw and disarm callbacks still run in the same order.

## PushButton timeout shadow bounds

The PushButton menu arm timeout can redraw after a menu entry has been resized
or partially initialized.  Its shadow redraw now requires positive interior
geometry and both shadow GCs, with signed conversion before subtracting the
highlight frame.

Normal timeout feedback keeps its existing shadow type and focus checks.  The
focused regression is
`tests/automated/check-push-button-timeout-shadow.py`.

## PushButton unhighlight bounds

When an unarmed `XmPushButton` uses internal default-button emphasis, its
unhighlight path adds the enhancement and default-shadow thickness to form an
inner clearing delta.  Small widgets previously subtracted that delta from
`Dimension` values without checking the result.

The path now leaves the border unchanged when the delta consumes either
axis.  Valid unhighlight clearing retains its existing geometry and callback
behavior.

## PushButton default-shadow erase bounds

`EraseDefaultButtonShadow` now skips border clearing for buttons no larger than
twice the highlight delta and performs the remaining subtraction with signed
operands.  Collapsed buttons cannot wrap into oversized clear rectangles.

The focused regression is `tests/automated/check-pushb-erase-shadow-bounds.py`.

## PushButton default-glyph bounds

Optional default-glyph drawing and erasure now validate the available right
margin and content height before placement.  Signed calculations prevent tiny
buttons or glyphs taller than the label area from wrapping into negative or
oversized X coordinates and dimensions.

The focused regression is `tests/automated/check-pushb-glyph-bounds.py`.

## PushButtonGadget default-glyph bounds

Optional default-glyph drawing and erasure now validate the available right
margin and content height before placement.  Signed calculations prevent tiny
gadgets or tall glyphs from wrapping into invalid X coordinates and
dimensions.

The focused regression is `tests/automated/check-pushbg-glyph-bounds.py`.

## PushButton gadget highlight bounds

PushButton gadget menu-disarm highlighting now passes signed inset dimensions
to `XmeDrawHighlight`.  Collapsed gadgets therefore reach the shared guard
instead of passing wrapped `Dimension` spans.

The source-level regression is `tests/automated/check-pushbg-menu-highlight-bounds.py`.

## PushButtonGadget hook allocation handling

PushButtonGadget's secondary-object, set-values, and get-values hooks now
check every extension-object and extension-data allocation.  Partial objects
are released before returning, and get-values cleanup tolerates a missing
extension record when temporary state could not be created.  Successful cache
merging and resource-hook behavior is unchanged.

The regression check is `tests/automated/check-pushbg-hook-allocation.py`.

## PushButtonGadget label-area guard

`ComputePBLabelArea` now validates both the gadget and output rectangle before
reading gadget colors or geometry. Invalid internal calls return `False`
without dereferencing state; valid layout calculations retain their existing
geometry and non-negative sizing behavior.

The focused source-contract check is
`python3 tests/automated/check-pushbg-label-area-guard.py`.

## PushButton gadget draw origins

PushButton gadget shadow and highlight calls now add highlight thickness to
`Position` coordinates in signed arithmetic.  Gadgets positioned partly
outside a parent therefore retain their negative/off-edge origin instead of
wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-pushbg-origins.py`.

## PushButton default-shadow geometry

When erasing the default-button shadow, `XmPushButton` derives the fill area
from the widget size and an emphasis adjustment.  A zero highlight thickness
could make that adjustment negative, and a tiny widget could then pass wrapped
width or height values to the fill helper.

The adjustment is now clamped to zero and the fill is skipped unless both
dimensions contain the requested inset.  Normal default-button rendering is
unchanged while collapsed buttons avoid oversized erase operations.

## PushButton emphasis fallback

PushButton and PushButtonGadget shadow/highlight routines now tolerate an
invalid `default_button_emphasis` value by returning without drawing. The
documented external and internal emphasis modes retain their existing behavior;
only malformed values avoid terminating the application.

The regression check is
`tests/automated/check-pushbutton-emphasis-fallback.py`.

## PushButtonGadget resource-list allocation

PushButtonGadget class initialization merges the LabelGadget and PushButton
resource tables into one cache-object list. The merged count and Xt byte size
are now checked before allocation, and initialization returns without writing
through a null list when memory is unavailable.

## PushButton label geometry

PushButton computes the label rectangle after accounting for highlight and
shadow thickness.  Those values are stored in unsigned `XRectangle` dimensions,
so a button smaller than its decorations could previously wrap a negative
content size to a near-maximum rectangle.  The dimensions now clamp to zero
when the decorations consume the available space; normal-sized buttons keep
the same geometry.

## PushButton default-frame reservation bounds

`XmPushButton` and `XmPushButtonGadget` reserve geometry for classic default
button emphasis. That reservation, the corresponding margin adjustments, text
origins, and recomputed widget sizes now use wide intermediates and saturate
at Motif's legacy `Position` and `Dimension` limits. This prevents a large
requested default shadow from wrapping a button or gadget into unrelated
geometry while preserving normal classic and modern profile behavior.

The focused source regression is
`tests/automated/check-pushbutton-reservation-bounds.py`.

## PushButton synthetic hook guards

PushButton and PushButtonGadget synthetic pixel conversion hooks now validate
the callback widget and `XtArgVal` storage before reading or converting values.
Invalid imports return `XmSYNTHETIC_NONE`; invalid exports return without
touching caller storage. Valid button subclasses retain their existing unit
conversion and default-shadow behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-pushbutton-synthetic-hook-guards.py
```

## PushButtonGadget default-shadow geometry

The gadget default-shadow erase paths subtracted twice the emphasis inset from
unsigned rectangle dimensions before checking whether the result was usable.
Tiny menu entries could therefore wrap to a large clear region, and the later
positive-width check could not detect the wrapped value.

Both erase paths now reject rectangles that cannot contain the inset before
subtraction. Normal default-button rendering is unchanged while collapsed
gadgets avoid oversized clear operations.

## PushButtonGadget highlight geometry

The internal PushButtonGadget focus highlight subtracts the 3-D enhancement
pixel from the configured highlight thickness and then subtracts the resulting
inset from the gadget rectangle.  The path now performs the first subtraction
in a signed integer, rejects a non-positive highlight width, and skips drawing
when the inset consumes the gadget.  This prevents unsigned wraparound and
negative rectangles on small controls while preserving normal focus visuals.

## PushButtonGadget label geometry

PushButtonGadget stores its label rectangle in the shared integer
`LRectangle`, but the source rectangle dimensions are unsigned.  When the
highlight and shadow decorations consume a tiny gadget, direct subtraction
could produce a negative rectangle that later drawing code interpreted as a
large region.  The computed label width and height now clamp to zero while
normal gadget geometry remains unchanged.

## PushButtonGadget unhighlight geometry

When a PushButtonGadget loses focus, its menu and internal highlight paths
clear an inset border.  The clear operation now checks that the gadget is
larger than twice the inset before subtracting from its unsigned rectangle
dimensions.  Very small gadgets leave the border untouched instead of passing
wrapped dimensions to the X drawing helper.

## RatingControl value API inputs

The public RatingControl value getter and setter now reject a null widget
before invoking the Xt subclass check. Invalid optional handles return the
existing neutral value or no-op result; valid normalization, redraw, and
callback behavior is unchanged.

The focused regression check is
`tests/automated/check-rating-control-inputs.py`.

## RatingControl coordinate bounds

RatingControl now clamps each calculated star vertex to X11's signed 16-bit
`XPoint` range before drawing.  Wide controls remain clipped predictably
instead of wrapping individual star points.

The star-field clip rectangle now applies the same bounds before converting its
inset, width, and height to X11's 16-bit fields.

`tests/automated/check-rating-coordinate-bounds.py` protects this drawing
boundary.

## RCMenu callback-closure snapshot allocation

Entry activation snapshots callback closures before invoking application
callbacks, because later Xt calls can invalidate the callback list. The
snapshot allocation is now checked before copying closures or dispatching
them. Both host-size arithmetic and Xt's `Cardinal` allocation width are
validated; allocation failure leaves the menu state intact and returns safely.

## RowColumn menu child-count traversal

RowColumn menu activation, recursive menu processing, menu search, and widget
membership checks now use `Cardinal` indices when traversing composite
children. This matches Xt's child-count field and avoids signed-index
wraparound for large valid menu hierarchies.

The focused source regression is
`tests/automated/check-rcmenu-child-count-bounds.py`.

## RCMenu excluded-parent-pane growth

Menu popdown temporarily records the active parent panes while restoring
shared or torn-off menus. The ABI stores the pane count and capacity as signed
shorts.

The traversal now uses a checked integer depth and grows the pane array through
a temporary pointer. Capacity and byte-size limits are validated before Xt is
called, and the replacement is published only after success. An allocation
failure stops the traversal with all previously recorded panes intact.

## RCMenu keyboard-list growth

Row-column menus keep keyboard accelerator and mnemonic entries in a compact
list whose capacity is stored as a signed `short` for ABI compatibility.
Adding an entry grows that list in ten-entry increments.

Growth now validates the signed capacity and byte count before calling Xt, and
publishes the new pointer and capacity only after the allocation succeeds. A
failed or unrepresentable growth leaves all existing entries intact and skips
the new entry safely.

## RCMenu post-from-list growth

Shared popup menus keep their attach widgets in a dynamically allocated
`postFromList`. The list uses signed `int` counters for ABI compatibility.

The growth path now treats a count at or beyond capacity as a growth request,
checks signed and byte-size limits, and publishes the replacement pointer and
capacity only after `XtRealloc` succeeds. Existing attach widgets remain valid
when allocation is unavailable or the counters are malformed.

## RCPopup list growth failures

The row-column popup registry grows a per-widget `WidgetList` whenever a
menu is posted.  The old path assigned `XtRealloc`'s result directly and
then stored the new parent widget, so an allocation failure discarded the
registry and caused a null-pointer write.

Growth now checks both host `size_t` and Xt's `Cardinal` byte limits before
multiplication, passes an explicit Xt-sized allocation, and commits the new
list only after allocation succeeds. Existing popup registrations stay valid
when the list cannot grow.

## Generated real translation bounds

The legacy `_XmGetRealXlations` helper expands virtual key bindings into real
translation text. It previously concatenated key names and arbitrary action
strings into a 1000-byte stack buffer and asserted only after writing it.

The helper now appends into Xt-owned growable storage with checked size
arithmetic and allocation limits. Long valid action strings are preserved;
allocation overflow returns `NULL` cleanly. The translation text and modifier
format for normal inputs are unchanged.

`tests/automated/check-real-xlations-growth.py` guards the storage contract.

## Icon-list realignment guards

`RealignIconList` now rejects missing placement storage, non-positive old or
new dimensions, invalid cell sizes, and incomplete root coordinate arrays
before converting or moving entries.  Valid icon-box realignment is unchanged.

`tests/automated/check-realign-icon-list-guards.py` protects the contract.

## XmRegion API null guards

`_XmRegionDestroy` now accepts a `NULL` region.  `_XmRegionGetRectangles`
validates both output pointers and treats a `NULL` region as an empty result,
so optional geometry cleanup and output collection cannot dereference absent
state.

The focused regression is `tests/automated/check-region-api-null-guards.py`.

## XmRegion copy growth

`miRegionCopy` now grows a destination rectangle array through a temporary
pointer and checks the element-count multiplication against both `size_t` and
Xt's `Cardinal` byte-size range.  If `XtRealloc` fails,
the destination keeps its existing storage and the copy returns without
corrupting the region.

The copy path also allocates storage when a destination region starts with a
NULL rectangle pointer, so empty or partially initialized regions cannot reach
a NULL `memmove`.

The focused regression is `tests/automated/check-region-copy-growth.py`.

## Empty Region shrink allocation

Region operations may shrink their rectangle storage after an intersection or
subtraction.  The non-empty shrink path now checks the rectangle-byte count
before narrowing it to Xt's `Cardinal` allocation size.  When the result is
empty, the replacement one-record allocation is obtained before the old array
is released.  If allocation fails or the size is unrepresentable, the region
keeps its existing (possibly larger) array and remains valid for later
operations instead of exposing a null rectangle pointer.

The focused source-level regression is
`tests/automated/check-region-empty-shrink-allocation.py`.

## XmRegion GC input guards

`_XmRegionSetGCRegion` now returns when its display or graphics-context handle
is absent, before rectangle collection or the Xlib clip operation. This keeps
optional region cleanup safe while preserving the existing behavior for valid
X11 handles and regions.

The focused regression is `tests/automated/check-region-gc-input-guards.py`.

## XmRegion operation input guards

Private region operations now handle optional geometry state consistently:
null sources are treated as empty, null destinations are left untouched, and
null rectangle inputs or shrink targets are ignored. Valid region operations
retain their existing intersection, union, subtraction and shrink behavior.

The focused regression is
`tests/automated/check-region-operation-input-guards.py`.

## XmRegion query and mutation null guards

The private `XmRegion` geometry helpers now handle absent inputs safely:
clearing and offsetting a null region are no-ops, an absent region is empty,
and equality treats two absent regions as equal. Point queries return false,
rectangle extents remain untouched when either input is absent, and the
rectangle count for an absent region is zero.

These rules preserve the existing behavior for valid regions while allowing
optional geometry state to be inspected or cleaned up without a crash. The
focused regression is `tests/automated/check-region-query-null-guards.py`.

## Region shadow segment growth

The region shadow renderer keeps two segment arrays while it walks the
rectangle bands. Each shadow helper doubles its array when the current index
reaches the capacity.

Growth is transactional: the helper computes and validates the doubled count,
asks Xt for a replacement buffer, and updates the caller's pointer and capacity
only after the allocation succeeds. If the count cannot be represented or the
allocation fails, the existing buffer remains owned by the caller and its
capacity is reduced to the number of valid segments. Subsequent helpers retry
growth instead of overwriting valid segments or treating uninitialized entries
as drawable data. This preserves the normal Motif rendering API while making
allocation pressure safe.

## Region rectangle-shrink metadata

Region operations periodically shrink their rectangle array.  The previous
path changed `size` before `XtRealloc`; if the allocation failed it restored
the pointer but left the smaller capacity recorded, so a later insertion
could overrun the allocation.

The pointer and capacity are now committed together only after a successful
shrink.  Failed shrink attempts leave the original region storage unchanged.

## Render-table conversion array bounds

Render-table property conversion stores generated arguments and temporary
strings in fixed arrays.  Each insertion now checks the corresponding array
capacity and routes oversized input through the existing cleanup path, avoiding
out-of-bounds writes for unusually large property data.

## Render-table converter inputs

`XmRenderTableCvtFromProp` now rejects a missing widget or property string
before selecting the application context or invoking the lexer.  Malformed
conversion calls therefore return `NULL` instead of dereferencing invalid
input.

## Empty render-table string tokens

The render-table lexer now emits an allocated empty string for `""` and
advances beyond both quote characters.  Empty quoted values therefore no
longer leave the lexer at the same position and repeatedly parse the token.

## Long render-table tokens

Alphabetic render-table tokens are now allocated to their actual length.  Long
font or tag names are preserved as one token instead of being truncated at 79
bytes and misparsed as multiple tokens.

## Render-table numeric token parsing

The render-table lexer now handles two malformed numeric cases safely.  An
unrecognized character is consumed as a one-character token instead of leaving
the lexer at the same position, and a `strtod` result is cast to `int` only
after checking the full integer range.  Out-of-range values remain floating
tokens without invoking an undefined conversion. `strtod` range errors and
non-finite results are emitted as ordinary character tokens instead of being
stored as render-table floating values.

## Render-table property-name bounds

Render-table property conversion uses a fixed table for the supported property
names.  The converter now checks the property index before indexing that table;
oversized input follows the existing error cleanup path instead of reading past
the array.

## Render-table AS_IS sentinel parsing

Render-table property import now compares the parsed integer for background,
foreground, underline, and strikethrough fields against the `-1` AS_IS
sentinel.  The previous code compared the token-type enum to `-1`, so the
sentinel was never recognized and could be passed through as a normal resource
value.  Other valid integer values retain their existing behavior.

The regression check is
`tests/automated/check-render-table-as-is-sentinel.py`.

## Bounded render-table import

`XmRenderTableCvtFromProp` now honors its `len` argument by copying exactly
that many bytes into a NUL-terminated parser buffer. This prevents the token
scanner from reading beyond a property that is not NUL-terminated while
preserving the public conversion API and its existing token grammar.

The focused regression check is
`tests/automated/check-render-table-bounded-input.py`.

## Render-table property conversion allocation

`XmRenderTableCvtToProp` now checks its initial conversion-buffer allocation
before copying the transfer vector.  Allocation failure unlocks the app
context and returns failure without dereferencing a null buffer; successful
conversion behavior is unchanged.

The regression check is
`tests/automated/check-render-table-conversion-allocation.py`.

## Render-table conversion results

The standard conversion target for a render table serializes the table into a
property buffer.  If serialization reports nonzero bytes without returning a
buffer, the conversion now publishes an empty `None`-typed result instead of
claiming a valid `XA_STRING` payload.  Valid empty and non-empty buffers keep
the existing format and type.

The regression check is
`tests/automated/check-render-table-conversion-result.py`.

## Render-table converter input validation

Render-table conversion now validates display, widget, source, destination, and
converter-argument inputs before creating renditions. Valid default, named,
and fallback font-list conversions remain unchanged.

The focused regression check is
`tests/automated/check-render-table-converter-inputs.py`.

## Default render-table extents

`XmRenderTableGetDefaultFontExtents` now handles a null render table without
dereferencing it. Any supplied height, ascent, and descent outputs are set to
zero, matching the function's existing no-font result behavior.

## Render-table property EOF handling

The initial item-list scan in `XmRenderTableCvtFromProp` now stops on either a
newline or end-of-input token.  A malformed property without a trailing
newline can no longer leave the scan index unchanged and loop forever.

The regression check is `tests/automated/check-render-table-eof-scan.py`.

## Render-table property export bounds

`XmRenderTableCvtToProp` serializes tags, font names, tab lists, and rendition
attributes into an X11 property string. Its old append macro doubled the output
buffer only once, so a single long field could still overflow it. It also used
fixed 2048-byte temporary buffers and appended a newline using the previous
field's length, which could read beyond the newline literal.

The exporter now grows its output until the complete field fits with overflow
checks, uses bounded formatting for temporary records and tab lists, and appends
the newline with its actual length. Oversized or unrepresentable output fails
cleanly instead of corrupting memory.

`tests/automated/check-render-table-export-bounds.py` guards these contracts.

## Render-table input-copy allocation

Render-table resource conversion tokenizes a mutable copy of the input
string. The copy is now checked before tokenization; allocation failure
returns `False` without passing a null pointer into the parser.

## MRM render-table construction

`_XmCreateRenderTable` now checks its table body and handle allocations. A
failed handle allocation releases the body and returns `NULL` before any
render-table fields are initialized.

A null parent is rejected before display lookup for the same reason.

## MRM rendition construction

`_XmCreateRendition` now rejects a null parent/table handle and a failed
rendition allocation before duplicate lookup or table mutation. Invalid MRM
construction therefore returns `NULL` without dereferencing partial state.

Table resizing and the copied table entry are checked as well; a failed
mutation releases the newly created rendition instead of publishing a null
slot.

The resized body count is also checked with `size_t` arithmetic before the
reallocation request.

## Render-table property bounds

`XmRenderTableCvtFromProp` keeps parser offsets in `int` for compatibility
with the existing token reader.  The converter now rejects property lengths
that cannot be represented by those offsets before parsing begins.  Normal
render-table resources are unchanged; oversized malformed properties fail
closed instead of truncating a pointer difference.

`tests/automated/check-render-table-property-bounds.py` is the focused source
regression for the boundary.

## Render-table property conversion

Property conversion now rejects a failed `XmRenditionCreate` instead of
inserting a null entry into the rendition array. The temporary array is also
released after conversion, including malformed-input error paths.

## Render-table removal

Shared render tables are copied before removal so other handles remain
unchanged. The copy now checks the table body, handle, and each rendition
allocation; a failure releases partial state and returns the original table.

## Render-table rendition arrays

`XmRenderTableGetRenditions` now follows its documented result layout: the
returned array has one slot for every requested tag, and an unmatched tag
leaves its corresponding slot as `NULL`. The previous implementation compacted
matches and resized the allocation, which made the public positional contract
unreliable for callers that requested more than one tag.

The allocation size is checked against both host `size_t` and Xt `Cardinal`
byte limits before multiplication. Allocation failure is reported as `NULL`,
and a partially copied result is released before returning.
Successful matching and rendition ownership remain unchanged; callers still
release each non-`NULL` rendition with `XmRenditionFree` and the array with
`XtFree`.

Display-less tables are protected by the process lock. Allocation-failure
cleanup releases copied renditions through the internal lock-held helpers, so
it does not recursively acquire the process lock.

`XmRenderTableAddRenditions` checks the new-table body, handle, and initial
rendition copies before publishing the table. If any allocation fails, already
copied renditions are released and the call returns `NULL`.

Merge modes also retain the existing rendition when a replacement or clone
cannot be allocated; no merge mode publishes a null rendition slot.

When copying a shared table for a merge, body/handle/rendition failures now
roll back the temporary table and leave the original shared table unchanged.

Appending unmatched renditions follows the same transactional rule: failed
body, handle, or entry copies are released without publishing a partial table.

Count narrowing and allocation-size arithmetic are checked before creating the
temporary match array or a new table body.

`XmRenderTableCopy` applies the same body-size bound before allocating a copied
table for a large tag selection or full-table copy.

`XmRenderTableCopy` uses the internal rendition lookup while its table lock is
held, and rolls back copied handles if an allocation fails. Tagged copies
therefore avoid recursive locking and never return a partially initialized
table.

`XmRenderTableAddRenditions` also checks its temporary merge-match array
before clearing it, returning the original table if that allocation fails.

## Rendition creation

`_XmRenditionCreate` now checks both its body and handle allocations. If
either allocation fails, it releases any partial body and returns `NULL`
without entering resource lookup or validation with an invalid handle.

## Render-table resize

When removal compacts a render table, the `XtRealloc` result is now checked
before replacing the table body. A failed resize or replacement handle leaves
the current table intact rather than publishing a null body.

## Render resource paths

Rendition resource lookup now bounds the widget-ancestor path and reserves
space for the resource name, optional rendition tag, and terminating quarks.
Very deep widget hierarchies no longer overrun the fixed lookup buffers.

## Remaining label/font SetValues ownership

BreadcrumbBar, Divider, and DropHighlight now validate replacement font or
label allocations before releasing the current resource. Allocation failure
keeps the existing value in place, avoiding a partially updated widget and
preserving the established Motif ownership contract.

The focused source contract is checked by
`tests/automated/check-render-table-setvalues-transaction.py`.

## Null render-table lookup

`XmRenderTableGetRendition` now returns `NULL` when passed a null table. This
matches the failure behavior of the other render-table lookup helpers and
prevents the private display lookup from dereferencing an invalid handle.
Valid lookups and rendition ownership are unchanged.

Display-less tables are also supported: the lookup uses the process lock when
there is no display-backed application context.

## Render-table tab-list EOF handling

Render-table tab-list import now rejects EOF or a newline before the closing
bracket or separator.  Malformed properties can no longer leave either token
loop rereading EOF indefinitely; valid tab lists retain their existing parsing
and resource behavior.

The regression check is
`tests/automated/check-render-table-tablist-eof.py`.

## Render-table tag arrays

`XmRenderTableGetTags` now validates its output pointer and handles every
allocation failure. A failed tag allocation releases any tags already copied,
clears the caller's output, and returns zero. The successful result still
contains one independently allocated tag for every rendition and retains the
documented `XtFree` ownership rules. Display-less render tables use the
process lock instead of attempting to derive an application context from a
null display.

## Render-table token bounds

The render-table parser keeps its public position interface as an `int` for
compatibility, but token scanning now rejects positions and token lengths that
would overflow that type.  This prevents signed-overflow undefined behavior
for malformed or extremely large render-table strings while preserving normal
tokenization and the existing end-of-input behavior.

`tests/automated/check-render-token-bounds.py` protects the overflow guards.

## Render-table token classification

The render-table lexer skips whitespace and recognizes alphabetic tokens with
C ctype functions.  These calls now receive unsigned byte values, preventing
undefined behavior when a malformed or non-ASCII render-table string is
parsed.

## Unterminated render-table strings

The render-table lexer now distinguishes a closing quote from the terminating
NUL.  A string without a closing quote consumes through the NUL and leaves the
cursor at the end of input instead of advancing one byte beyond it.

## Representation-type action parameters

Action parameter conversion now rejects a missing parameter or result pointer
before parsing.  Its whitespace and digit checks also pass unsigned byte
values to the C ctype functions, so malformed or non-ASCII input fails safely.

## Representation-type converter input validation

The forward and reverse representation-type converters now reject missing Xt
argument/value structures and unresolved record IDs before dereferencing them.
Valid conversions and warning behavior for known records remain unchanged.

The focused regression check is
`tests/automated/check-reptype-converter-inputs.py`.

## Representation-type name case conversion

When representation type names are formatted with the `Xm` prefix, Motif
uppercases the remaining label.  The case-conversion loop now casts each byte
to `unsigned char` before calling the C ctype functions, avoiding undefined
behavior for locale strings containing bytes above ASCII.

## `XmRepTypeGetId` null-name validation

`XmRepTypeGetId` now returns `XmREP_TYPE_INVALID` when its type-name argument
is `NULL`, avoiding an invalid `strcmp` call. Normal static and dynamic
representation-type lookup remains unchanged.

The focused regression check is
`tests/automated/check-reptype-getid-null.py`.

## Representation-type identifier bounds

`XmRepTypeId` reserves `XmREP_TYPE_INVALID` as its invalid sentinel. Dynamic
registration now refuses a record whose identifier would reach that sentinel,
and wide-prototype public entry points reject negative or out-of-range integer
identifiers before narrowing them to `XmRepTypeId`. Wide-prototype registration
also rejects value counts that cannot fit the public `unsigned char` field.

This prevents an invalid identifier from aliasing a valid registry record while
preserving all existing valid registration and lookup behavior. The focused
source regression is `tests/automated/check-reptype-id-inputs.py`; the native
RepType consumer also verifies that the reserved sentinel has no record, name
list, or valid value.

## Numeric representation action parameters

Motif actions that accept a representation type may retain the historical
numeric form (for example, `"2"`) in addition to a named value. Numeric input
is parsed with a range-checked conversion before it is validated against the
representation type. Inputs that exceed the range of an `int` are rejected
without invoking undefined integer conversion behavior; the accepted legacy
prefix syntax remains available.

The regression is run with:

```sh
python3 tests/automated/run-reptype-number-regressions.py /path/to/prefix /path/to/results
```

## Representation-type registration allocation

`XmRepTypeRegister` grows the process-wide dynamic representation-type table
before filling the new record. The resize byte size is checked against Xt's
`Cardinal` range and passed explicitly; the result is checked before the
global pointer is replaced or the new record is accessed. An allocation
failure returns `XmREP_TYPE_INVALID` while preserving the existing registered
types and converter state. `XmRepTypeGetRegistered` likewise returns `NULL`
when its result list cannot be allocated, without touching the process-wide
registry.

`XmRepTypeGetRecord` also checks its individual record allocation and returns
`NULL` without leaving the process lock held when memory is unavailable.

The shared copy helper now checks every string and value allocation, including
the pointer-table multiplication and Xt's `Cardinal` byte-size range for long
names, and passes explicit Xt-sized byte counts. Partial
records are released before a failure is returned, and list/record queries
clean up entries copied before the failed one.

Reverse conversion warnings also return `False` if their temporary formatted
message cannot be allocated, avoiding a null pointer passed to Xt's warning
path.

This keeps the public registration API and successful registration behavior
unchanged while preventing a null-table dereference under memory pressure.

## RepType conversion warning bounds

When a representation-type conversion receives an unknown value, Motif builds
a warning from the localized message and the signed integer value. The old
allocation added a fixed ten bytes for the number, which could be too small for
the sign, all digits, separator, and terminator.

The warning is now sized with `snprintf` before allocation, preserving the full
diagnostic for every representable integer.

`tests/automated/check-reptype-warning-bounds.py` guards the formatting path.

## Resource converter portability

The resource converters now make their established positive, bounded integer
values explicit at byte-count and copy-size boundaries. String spans, cardinal
table capacities, default-render-table cache growth and atom-list copies retain
their native unsigned widths until Xt's checked `Cardinal` allocation boundary.
Charset-table conversion also checks its pointer-table multiplication before
evaluating it: a maximum-width comma-separated value can reach an entry count
that would otherwise overflow the table byte calculation before the later
allocation-width checks run.

The focused source contract is covered by
`tests/automated/check-resconvert-portability.py`.

## resencod-ctext-concat
Compound-text concatenation failure handling
=============================================

`ResEncod.c` compound-text concatenation now checks the combined byte count
before growth and keeps the original buffer when `XtRealloc` cannot extend it.
It no longer overwrites that buffer with `NULL` before copying, preventing a
null dereference and preserving already-encoded output under memory pressure.

## Compound-text resource-converter output

The compound-text resource converter now initializes `XrmValue.addr` and
`XrmValue.size` before parsing. Empty input and allocation/conversion failures
therefore leave a deterministic empty output instead of exposing stale fields
from the caller's `XrmValue`.

The focused regression check is
`tests/automated/check-resencod-output-contract.py`.

## Icon-box maximum-size reset guards

`ResetIconBoxMaxSize` now returns when its client or bulletin-board widget is
missing, avoiding invalid Xt queries and geometry writes.  Valid resets retain
the existing sizing and placement behavior.

`tests/automated/check-reset-icon-box-max-guards.py` protects the contract.

## Public part-offset resolution guards

`XmeResolvePartOffsets` is a public compatibility entry point used while
widget classes are assembled. It now rejects null inputs, checks the
superclass-count multiplication against both host `size_t` and Xt's
`Cardinal` width, and releases a successfully allocated normal table if the
optional constraint table cannot be allocated. Resource offsets are not
updated from a partial table.

The focused regression check is
`tests/automated/check-resolve-part-offset-allocation.py`.

## Public part-offset resolution requires a superclass

`XmeResolvePartOffsets` now rejects a widget-class record whose superclass is
`NULL` before inspecting superclass sizes or class traits. A root or
partially initialized class is not a valid input for part-offset resolution;
returning with both output tables unset keeps malformed class records from
causing a null dereference while preserving the existing valid-class path.
The resolver also checks alignment rounding and the inherited-size addition
before changing either class record, so a representationally oversized class
layout fails without leaving a partially updated size.

The focused regression check is
`tests/automated/check-resolve-part-offset-allocation.py`.

## RowActions item-array allocation failure

`XmRowActionsSetItems` validates the item-count multiplication before entering
its replacement phase. An oversized request therefore leaves the current
action strip intact instead of erasing it first. Allocation failures after
replacement begins retain the existing behavior: the strip remains valid and
can be populated by a later call.

The focused source-contract check is
`tests/automated/check-row-actions-input-preservation.py`.

## Row-actions button-text allocation bounds

`XmRowActions` assembles an action label and optional shortcut directly as
compound `XmString` segments. This avoids flattening multilingual labels into
a temporary byte buffer and preserves charset information in the visible
button.

The focused regression check is
`tests/automated/check-row-actions-button-text-bounds.py`.

## RowActions callback guards

RowActions validates its activation callback closure before walking private
action storage. Foreign or stale callback data is ignored safely while normal
action invocation remains unchanged.

## RowActions child allocation

RowActions creates one push button per configured action.  Its preferred-size
and layout paths now tolerate a missing button, preserving safe redraw and
reveal behavior even while a failed child allocation is being recovered.

The contract is protected by
`tests/automated/check-row-actions-child-failure-guards.py`.

## RowActions event guards

RowActions validates its widget type and event pointer before processing
keyboard reveal events. Invalid dispatch is ignored safely while normal
keyboard interaction remains unchanged.

## RowActions item construction

`XmRowActionsSetItems` now validates all item and button-label allocations and
tracks the partial count while constructing buttons.  If an allocation or
widget creation fails, the partial list is destroyed and the widget remains
empty instead of dereferencing a null buffer or retaining an incomplete row.
Valid item arrays and button layout are unchanged.

The setter also ignores a replacement request after Xt begins destruction,
avoiding button construction and child management against teardown state.

`tests/automated/check-row-actions-item-transaction.py` protects this path.

## RowActions label assembly

`XmRowActions` now combines labels and shortcuts with `XmStringConcat`,
preserving compound-string segments in the displayed action text instead of
flattening the values into a temporary character buffer.

The source contract is checked by:

```sh
python3 tests/automated/check-row-actions-linear-copy.py
```

## Row-actions string-copy allocation guards

`XmRowActions` now copies retained item labels and shortcuts directly as
`XmString` values, preserving compound-string segments. Allocation failure
returns the empty-string fallback or `NULL` as appropriate without a text
conversion round trip.

The focused regression check is
`tests/automated/check-row-actions-string-copy-guards.py`.

## RowColumn accelerator expansion

RowColumn expands virtual menu-bar and menu key bindings into real Xt
translations. The expansion now grows its temporary storage as needed and
checks size arithmetic instead of writing into a fixed 1000-byte stack buffer.
The resulting accelerator string and public menu behavior remain unchanged.

## RowColumn managed-child search bounds

`FindFirstManagedChild` now keeps both its scan index and copied composite
child count in `Cardinal` variables. This avoids truncating Xt's child count or
wrapping a signed index while finding the first traversable managed child.

The focused source regression is
`tests/automated/check-rowcolumn-child-search-bounds.py`.

## RowColumn menu-post rollback

RowColumn now allocates a private copy of a changed `XmNmenuPost` string
before releasing the old value.  If the copy fails, the old menu-post string
and its parsed button/modifier mapping remain installed, and passive grabs are
left unchanged.  Clearing the resource still follows the existing update
path.

The focused regression is
`tests/automated/check-rowcolumn-menupost-rollback.py`.

## RowColumn creation parent guards

The non-varargs RowColumn creation wrappers now return `NULL` when passed a
null parent, before application-context lookup. This covers row columns,
work areas, radio boxes, option menus, menu bars, popup menus, and pulldown
menus. Valid parent widgets retain the existing creation behavior.

The focused regression is
`tests/automated/check-rowcolumn-parent-inputs.py`.

## RowColumn popup allocation

RowColumn menu creation now rejects null parent or name inputs and checks the
popup-shell result before marking the shell private.  A failed popup-shell
allocation returns cleanly instead of dereferencing a null menu-shell widget.

## RowColumn synthetic getter guards

RowColumn's synthetic mnemonic, menu, option-label, and tear-off resources
now validate the callback output pointer and the source widget before reading
private RowColumn state. Invalid calls return a deterministic null value;
valid RowColumn subclasses retain their existing copied-string behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-rowcolumn-synthetic-getter-guards.py
```

## RowColumn synthetic string export guards

Menu-post, label-string, and tear-off-title synthetic exports now validate the
widget and output pointers before reading RowColumn state or copying strings.
Malformed resource callbacks return safely without changing normal exported
values.

## RowColumn tear-off title initialization rollback

`XmRowColumn` now checks the initial `XmStringCopy` of its tear-off title.
When that copy fails, initialization installs an empty compound string rather
than propagating a NULL title into menu construction and later title handling.
The existing `SetValues` rollback behavior remains unchanged.

The focused regression is
`tests/automated/check-rowcolumn-tearoff-title-init-rollback.py`.

## RowColumn tear-off title rollback

`XmRowColumn` now copies a non-NULL tear-off title before releasing the
current title during `SetValues`.  Failed copies keep the existing menu title;
setting the title to NULL still clears it intentionally.

The focused regression is
`tests/automated/check-rowcolumn-tearoff-title-rollback.py`.

## RowColumn traversal child-list bounds

`TraversalChildren` now initializes its output parameters, checks the
`(num_children + 1) * sizeof(Widget)` size against both `size_t` and Xt's
`Cardinal` allocator width, and handles allocation failure without writing
through a null result. The bounded byte count is passed explicitly as an Xt
`Cardinal`. The tear-off control remains the first entry, followed
by the original composite children.

The focused source regression is
`tests/automated/check-rowcolumn-traversal-children.py`.

## Safe atom-name fallback

Transfer diagnostics must continue safely when an invalid X selection atom is
reported and the fallback allocation cannot be satisfied. The atom-name helper
now returns a non-owned empty string in that case, and its cleanup path leaves
that sentinel untouched.

Regression coverage: `tests/automated/check-safe-atom-name-fallback.py`.

## Sash cursor allocation

Sash instances share a per-display crosshair cursor.  When the X server cannot
allocate that cursor, Sash now leaves the cache empty, avoids registering a
destruction callback for an invalid cursor, and creates its window without the
`CWCursor` attribute.  X therefore retains the inherited cursor rather than
receiving an invalid cursor resource.

Later Sash instances can retry the normal shared-cursor allocation.  No public
resources or Sash behavior change when the allocation succeeds.

## Sash focus redraw lifecycle

Sash focus hooks can be requested by application traversal setup before the
widget is realized.  The highlight and unhighlight paths now defer their X
drawing until a window exists.  Once realized, their focus geometry and
appearance are unchanged.

## Sash highlight geometry

Sash focus painting fills or clears the widget interior after applying twice
the primitive shadow thickness.  The highlight and unhighlight paths now skip
the operation when that inset consumes the widget, preventing unsigned width
or height wraparound on very small sashes.  Normal sash focus painting is
unchanged.

## Scale available trough geometry

Scale computes the trough area after subtracting leading and trailing
padding.  Horizontal and vertical layout now perform those subtractions in a
signed `int` before comparing with the scrollbar minimum and converting back to
`Dimension`.  Small controls therefore take the existing minimum-size path
instead of wrapping to a huge trough dimension.

## Scale child-count bounds

`XmScale` keeps its child count in the Xt `Cardinal` type. Label measurement
and horizontal/vertical label layout now use `Cardinal` indices when walking
the composite child array, including the right-to-left layout path. This
preserves the existing child ordering and placement behavior while avoiding
signed-index wraparound for large child lists.

## Scale value-label formatting

Scale computes the trough, ascent, descent, and label dimensions by formatting
the configured minimum and maximum values into a small temporary buffer. The
four non-Xft layout paths now use `snprintf` with the buffer size for both
integer-only and decimal-label forms. This keeps the existing label metrics and
resource behavior while making the buffer contract explicit if value ranges or
locale formatting change.

`tests/automated/check-scale-label-bounds.py` verifies that all four paths use
bounded formatting and that the former unbounded calls are absent.

## Scale label placement

Scale tick labels are placed from the scrollbar’s slider area.  The available
interval can be smaller than a label, especially during a narrow or short
allocation.  Placement now keeps interval endpoints and the single-label
centering difference in signed arithmetic for horizontal and vertical scales;
an oversized label is clipped near the control instead of wrapping to an
extreme coordinate.

The source contract is checked by
`tests/automated/check-scale-label-placement.py`.

## Scale label-spacing arithmetic

When `XmScale` derives a fallback size from several managed labels, it adds
space for the tick/slider allowance only when that allowance exceeds half the
largest label extent.  The allowance and extent are Motif `Dimension` values,
which are unsigned.  The subtraction now uses signed `int` intermediates in
both orientations; a label that is taller or wider than the allowance no
longer wraps to a huge positive gap and inflate the requested Scale geometry.

The source contract is checked by
`tests/automated/check-scale-label-spacing.py`.

## Scale tick-array allocation failure

`XmScaleSetTicks` now checks the local array returned by `ALLOCATE_LOCAL` and
unlocks the application context before returning if allocation fails. This
prevents a low-memory condition from becoming a NULL write while preserving
normal tick generation.

The focused regression is
`tests/automated/check-scale-ticks-allocation.py`.

## XmScaleSetTicks input and allocation bounds

`XmScaleSetTicks` now rejects a non-positive `big_every` interval before it can
divide by zero or generate invalid tick counts.  The scale range is promoted
before subtraction, so using the full signed `int` range cannot overflow.

The medium- and small-tick counts are checked against `INT_MAX`, and the final
separator count is checked against both the host `size_t` and Motif's
`Cardinal` limits before the temporary separator array is allocated.  Normal
tick generation is unchanged; only invalid or unrepresentable requests return
without creating ticks.

`tests/automated/check-scale-ticks-bounds.py` protects this contract.

## Scale tick child-creation failures

`XmScaleSetTicks` now increments its managed-child count only when a separator
gadget was created successfully. A failed child creation is therefore omitted
from `XtManageChildren` instead of passing a NULL widget through the batch.

The focused regression is
`tests/automated/check-scale-ticks-child-failures.py`.

## Scale tick-generation input guard

`XmScaleSetTicks` now returns immediately when passed a null scale widget,
before application-context lookup. Valid scales retain the existing tick
generation and range validation behavior.

The focused regression is
`tests/automated/check-scale-ticks-input.py`.

## Scale title getter guard

The Scale synthetic title getter now validates the widget class and output
slot before reading title state or querying the label child. Invalid callback
inputs return a deterministic null value; valid title and child-label reads
retain their existing behavior.

The focused source-contract check is
`python3 tests/automated/check-scale-title-getter-guard.py`.

## Scale trailing tick bounds

Scale’s trailing tick coordinates now calculate widget and slider extents in
signed arithmetic.  Collapsed or temporarily over-constrained scales no
longer wrap a trailing tick position through unsigned `Dimension` subtraction.

The source-level regression is
`tests/automated/check-scale-trailing-ticks.py`.

## Scaled pixmap color input validation

`_XmGetScaledPixmap` now rejects a missing `XmAccessColorData` record before
cache lookup or image construction. The helper later copies the record and
uses its color fields, so accepting a null pointer previously caused a crash
through a private toolkit boundary. Existing callers that provide color data
retain their behavior; invalid input now returns `XmUNSPECIFIED_PIXMAP`.

The focused regression is
`tests/automated/check-xm-scaled-pixmap-color-input.py`.

## XmScreen child-array growth

`XmScreen` keeps its desktop children in a growable array. Insertion now
checks slot arithmetic and both host `size_t` and Xt `Cardinal` byte limits,
and grows through a temporary pointer, so an Xt allocation failure leaves the
existing children and slot count valid and skips only the new insertion.

The focused regression is `tests/automated/check-screen-child-growth.py`.

## Screen cursor-cache removal input validation

`_XmScreenRemoveFromCursorCache` now returns safely for a missing drag icon or
screen object before querying the cursor cache. Valid cache removal remains
unchanged.

The focused regression check is
`tests/automated/check-screen-cursor-cache-input.py`.

## Default-glyph lookup input validation

`_XmGetDefaultGlyphPixmap` now returns `None` and clears requested dimensions
when its screen is missing or unresolved. Valid glyph pixmap and dimension
lookups remain unchanged.

The focused regression check is
`tests/automated/check-screen-default-glyph-input.py`.

## Screen child deletion input validation

Screen child deletion now returns safely for a missing widget, parent, or
backing child list. Valid child removal and list compaction remain unchanged.

The focused regression check is
`tests/automated/check-screen-delete-child-input.py`.

## Screen font-unit input validation

`XmSetFontUnits` now ignores a missing display or unresolved screen, and
`_XmGetFontUnit` returns zero for a missing screen. Valid font-unit updates
and reads remain unchanged.

The focused regression check is
`tests/automated/check-screen-font-unit-input.py`.

## `XmGetXmScreen` null-screen validation

`XmGetXmScreen` now returns `NULL` when called without a `Screen`. This keeps
the compatibility entry point safe before it evaluates `DisplayOfScreen`;
normal screen lookup and implied-screen creation are unchanged.

The focused regression check is
`tests/automated/check-screen-getxm-null.py`.

## Screen drag-icon helper input validation

Operation, state, and source icon helpers now return no icon when given a
missing widget or unresolved screen object. Normal icon lookup and lazy
creation remain unchanged.

The focused regression check is
`tests/automated/check-screen-icon-inputs.py`.

## Screen child insertion input validation

Screen child insertion now validates the child, parent capacity, and backing
list before growing or writing the child array. Valid insertion and growth
behavior remain unchanged.

The focused regression check is
`tests/automated/check-screen-insert-child-input.py`.

## Screen menu-cursor input validation

Menu-cursor setters and screen-based getters now safely handle missing
displays, screens, or `XmScreen` objects. Valid cursor updates and reads are
unchanged.

The focused regression check is
`tests/automated/check-screen-menu-cursor-input.py`.

## XmScreen generated name bounds

When `XmGetScreen` implicitly creates a screen widget, it derives a name from
the screen index. The fixed name buffer now uses `snprintf` with its capacity,
so the generated resource name cannot overrun the stack if the display exposes
an unusually large screen index.

`tests/automated/check-screen-name-bounds.py` guards this formatting contract.

## Null-cursor input validation

`XmeGetNullCursor` now returns `None` for a missing widget or unresolved
screen, before accessing Xt or screen state. Normal cursor creation and cache
reuse remain unchanged.

The focused regression check is
`tests/automated/check-screen-null-cursor-input.py`.

## Screen per-screen getter validation

Color calculation/allocation getters now return `NULL` for a missing screen;
bitmap conversion returns `XmMATCH_DEPTH`; and insensitive-stipple lookup
returns `None` for a missing widget or screen. Valid screen values remain
unchanged.

The focused regression check is
`tests/automated/check-screen-per-screen-getters.py`.

## Screen pixmap/cache input validation

Scratch-pixmap release now safely ignores missing screens, `None` pixmaps, or
uninitialized cache tables. Cursor-cache lookup likewise returns `NULL` for a
missing screen. Valid cache allocation and release remain unchanged.

The focused regression check is
`tests/automated/check-screen-pixmap-cache-input.py`.

## XmScreen scratch-pixmap cache failures

Scratch-pixmap checkout and return now check `_XmAddHashEntry`.  If the in-use
or scratch cache cannot allocate a hash bucket, the pixmap and ownership key
are released instead of becoming unreachable and leaking.  A failed checkout
returns `None`.

The focused regression is
`tests/automated/check-screen-scratch-cache-failure.py`.

## Screen unpost-behavior input validation

`_XmGetUnpostBehavior` now returns Motif’s default
`XmUNPOST_AND_REPLAY` behavior when its widget or screen is missing. Valid
screen-specific behavior remains unchanged.

The focused regression check is
`tests/automated/check-screen-unpost-behavior-input.py`.

## Scroll-visible callback guards

`IconScrollVisibleCallback` now ignores missing callback data, active workspace
or icon-box state, and missing traversal destinations before calling
`XmScrollVisible`.  Valid traversal callbacks remain unchanged.

`tests/automated/check-scroll-visible-callback-guards.py` protects the contract.

## Scroll-visible input guards

`XmScrollVisible` now returns immediately when either the scrolled-window
widget or the descendant widget is null. The validation happens before the
widget cast and application-context lookup, preventing invalid traversal
requests from crashing the caller. Existing warning behavior for non-null
widgets that are not suitable scrolled windows is unchanged.

The focused regression is
`tests/automated/check-scroll-visible-inputs.py`.

## ScrollBar far-side arrow positions

Far-side arrow coordinates now use signed intermediates before conversion to
the X toolkit `Position` type.  When a collapsed scrollbar cannot fit its
insets and arrow extent, the coordinate remains a valid negative/off-edge
position instead of wrapping to a distant positive location.

The source-level regression is
`tests/automated/check-scrollbar-arrow-positions.py`.

## ScrollBar redisplay shadow bounds

ScrollBar redisplay now clamps the shadow drawing width and height to one pixel
when the widget is smaller than twice its highlight thickness.  Collapsed
controls no longer pass wrapped unsigned dimensions to the shadow renderer.

The source-level regression is
`tests/automated/check-scrollbar-redisplay-bounds.py`.
