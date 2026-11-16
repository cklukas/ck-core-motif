---
nav_exclude: true
---
# Archived Bugfix and Hardening Notes (Part 7)

Consolidated archive of standalone bugfix and hardening validation notes.

## TextField selection insertion conversion

Selection insertion now preserves `mbstowcs` results as `size_t` in both
locale-text paths. Conversion failure or an unrepresentable wide-character
count fails the replacement instead of relying on a signed cast and a
negative-result check.

Regression coverage:

```text
python3 tests/automated/check-textfield-selection-insert-conversion.py
```

## TextField wide selection conversion

`XmTextFieldGetSelectionWcs` now preserves the native `size_t` result from
`mbstowcs`, rejects conversion failure, and returns exactly the number of wide
characters converted. This avoids signed-result wraparound and prevents
uninitialized trailing characters from being exposed when the selected text
contains a terminator before the requested bound.

Regression coverage:

```text
python3 tests/automated/check-textfield-selection-wcs-conversion.py
```

## TextField value callback lifetime

A value-change callback may destroy the text field, its parent or its shell.
`XmTextFieldSetString` must then stop without repainting or reading the freed
field. This also applies to the wide-character setter that delegates to it.

With multiple callbacks, immediate destruction can free Xt's compiled callback
list while `XtCallCallbackList` is still traversing it. TextField activation and
SetString value notifications now dispatch an owned snapshot obtained through
the callback resource. Dispatch stops when the field is destroyed or marked for
deferred destruction. Surviving fields retain callback order and normal default
button propagation. Changes to the registered list affect subsequent dispatches.

`run-textfield-setstring-lifetime.py` covers narrow/wide setters, immediate and
event-deferred destruction, and field/parent/shell destruction with callbacks
before and after the destructive callback. `run-textfield-activation-lifetime.py`
also checks multiple callbacks and default-button propagation. Both font configurations pass 17 setter/registration and eight activation cases. The
NumberBox editing fixture also passes all eight light/dark/scale configurations
under Valgrind with zero invalid-access errors (undefined-value and leak checks
were disabled). This does not complete the wider text-editing callback audit.


The dispatcher is the private `_XmCallCallbacksUntilDestroyed` utility in
CallbackI. It applies to callback resources of real Xt objects; it does not
replace the separate extension-record callback mechanism. It holds the
application lock, owns the callback records for the dispatch and returns False
when callers must stop accessing the object. NumberBox's value and invalid-input
notifications share this implementation. A registration-mutation case verifies
that callback removal/addition changes the next dispatch, not the current one.

## TextField SetValues empty wide value

When a multibyte TextField receives a null wide value through `SetValues()`,
the replacement terminator allocation is now checked before initialization.
Failure rejects the update after restoring insertion-point painting.

Regression coverage: `tests/automated/check-textfield-setvalues-empty.py`.

## TextField string and context callback guards

The TextField synthetic-resource callbacks now validate their inputs before
calling public string getters. Narrow and wide exports return a deterministic
null value when the callback receives a missing widget or output slot instead
of dereferencing invalid state.

The display-destroy cleanup callback also validates its context record and
screen before asking X11 to inspect or delete a context. A screenless record is
freed directly, which keeps teardown safe for partially initialized display
objects while preserving the normal X11 cleanup path.

The focused source-contract check is
`python3 tests/automated/check-textfield-string-getter-guards.py`.

## TextField target counts

TextField target negotiation now rejects callbacks without a transfer ID or
with target counts beyond the legacy processing limit before scanning external
atom data. The companion target-list scan uses the callback's unsigned count
type for its index as well, avoiding a signed comparison when an external
owner supplies the target list.

Regression coverage: `tests/automated/check-textfield-target-count.py`.

## TextField TARGETS allocation

The TextField TARGETS selection target now checks the result of
`XmeStandardTargets()` before appending widget-specific atoms.  If the target
list cannot be allocated, conversion returns `False` rather than writing
through a null pointer.

Regression coverage: `tests/automated/check-textfield-targets-allocation.py`.

## TextField timestamp allocation

The TextField timestamp selection target now checks its `Time` allocation
before writing the timestamp.  Allocation failure returns `False` instead of
dereferencing a null pointer.

Regression coverage: `tests/automated/check-textfield-timestamp-allocation.py`.

## TextField transfer-action allocation

Button-drag transfer setup now allocates the action record and event as a
transaction.  If either allocation fails, partial state is released and the
original action is invoked instead of copying through a null event pointer.

Regression coverage: `tests/automated/check-textfield-transfer-action.py`.

## TextField transfer conversion

TextField transfer and paste paths now preserve `mbstowcs` results as
`size_t`, reject failed or unrepresentable conversions, and explicitly
terminate successful wide buffers before replacement. Invalid transfer text
therefore fails safely instead of relying on a signed cast or exposing
unterminated storage.

Regression coverage:

```text
python3 tests/automated/check-textfield-transfer-conversion.py
```

## TextField selection and drop inputs

TextField selection and drop callbacks now reject malformed transfer metadata,
missing drag target arrays, and allocation failures before dereferencing their
temporary selection or drop records.

Regression coverage: `tests/automated/check-textfield-transfer-inputs.py`.

## TextField validation warnings

Unsupported-character diagnostics during TextField validation now tolerate
failure to allocate their temporary escaped string.  The warning is skipped,
but validation continues and no null buffer is formatted or freed.

Regression coverage: `tests/automated/check-textfield-validation-warning.py`.

## TextField value conversion

TextField value validation now preserves native conversion results and checks
allocation bounds before publishing either wide or narrow storage. Failed
`mbstowcs` conversions reset the wide value safely; failed or
capacity-filling `wcstombs` conversions release the narrow buffer instead of
exposing unterminated text.

Regression coverage:

```text
python3 tests/automated/check-textfield-value-conversion.py
```

## TextField unsupported-character warning allocation

The diagnostic path used when a TextField encounters an unsupported
multibyte character now checks the escaped-warning buffer size before
computing `4 * num_conv` or passing it to `XtMalloc`.  This prevents signed
arithmetic overflow and truncation of the allocator's `Cardinal` size while
preserving the normal warning for representable input.

Regression coverage: `tests/automated/check-textfield-warning-allocation.py`.

## TextField wide insertion conversion

Compound-text and UTF-8 insertion now checks the converted length against
legacy integer and Xt allocation limits, and handles wide-buffer allocation
failure before calling `mbstowcs`.

Regression coverage: `tests/automated/check-textfield-wcs-insertion.py`.

## TextField wide-value validation

Validation of multibyte TextField values now checks the wide-character
allocation before calling `mbstowcs()`.  Allocation failure clears the value
state, releases the temporary validation buffer, and returns without a null
dereference.

Regression coverage: `tests/automated/check-textfield-wide-validation.py`.

## TextField wide-value warnings

Unsupported-wide-character diagnostics now tolerate failure to allocate their
temporary escaped strings.  Warning formatting is skipped when necessary while
the validation loop continues without null-buffer access.

Regression coverage: `tests/automated/check-textfield-wide-warning.py`.

## XmTextFindStringWcs conversion termination

The wide-character search API now rejects a conversion that fills the entire
temporary multibyte buffer and explicitly terminates successful conversions
before passing them to the narrow search implementation.

Regression coverage: `tests/automated/check-textfind-wcs-termination.py`.

## TextField malformed transfer completion

`DoStuff` now explicitly fails a transfer when callback state is incomplete,
provided a transfer ID is available. Teardown and cancellation paths therefore
cannot leave an external selection request pending indefinitely.

Regression coverage: `tests/automated/check-textfsel-dostuff-failure.py`.

## File-selection drop format validation

File-selection text drops now reject external 16-bit and 32-bit formats, and
lengths that cannot be represented by the text conversion path, before locale
conversion or direct insertion.

Regression coverage: `tests/automated/check-textfsel-drop-format.py`.

## TextField export-target conversion allocation

Selection conversion now checks the `Atom` array used for Motif export-target
responses.  If it cannot be allocated, the transfer completes with an empty
failed response instead of writing through a null pointer.

Regression coverage: `tests/automated/check-textfsel-export-targets-allocation.py`.

## TextField target-discovery failure completion

TextField target discovery now completes transfers with failure when target
data is malformed, the insertion overlaps the current selection, or follow-up
state cannot be allocated. These paths no longer leave a request pending after
freeing its callback data.

Regression coverage: `tests/automated/check-textfsel-handle-targets-failure.py`.

## TextField primary-selection wide conversion

Primary-selection insertion now bounds the converted length before narrowing it
to `int` and checks the wide conversion buffer before calling `mbstowcs`.
Allocation or size failure safely rejects the replacement instead of
dereferencing a null buffer.

Regression coverage: `tests/automated/check-textfsel-primary-wide-allocation.py`.
The check follows the explicit bounded conversion result used by the current
implementation.

## TextField raw transfer format validation

The TextField raw-target transfer path now rejects non-byte selection formats
before copying or converting the payload. This keeps 16-bit and 32-bit
external data from being reinterpreted as locale bytes.

Regression coverage: `tests/automated/check-textfsel-raw-format.py`.

## TextIn selection-array ownership

The input method keeps a static built-in selection array when a caller has not
provided one or an allocation fails.  `InputSetValues` now frees the previous
array only when it is dynamically owned, avoiding an invalid free of the static
fallback during resource updates.

Regression coverage: `tests/automated/check-textin-selection-array-ownership.py`.

## TextOut allocation guards

Text output initialization, Unicode paragraph caching, and baseline-array
retrieval now handle allocation failure explicitly.  Empty baseline results
are represented without allocating, while failed non-empty results return
failure instead of dereferencing null storage.

Line-wrap metadata is optional: if its allocation fails, layout computation
continues without the extra record.

Regression coverage: `tests/automated/check-textout-allocation-guards.py`.

## TextOut line-wrap source validation

Line-end discovery now rejects non-advancing or malformed text-source blocks
before continuing its wrapping loops. This prevents malformed source data from
causing an infinite loop or reaching width/height measurement with invalid
memory.

Regression coverage: `tests/automated/check-textout-line-wrap-source-progress.py`.

## TextOut measurement block validation

The width and height measurement entry points now reject null, empty, or
invalid text blocks before dispatching to font, Xft, or multibyte routines.
This makes malformed source data harmless at the common measurement boundary.

Regression coverage: `tests/automated/check-textout-measurement-blocks.py`.

## TextOut pixmap-name bounds

Text output derives internal I-beam and add-mode cursor pixmap names from
integer geometry values. The fixed name buffers now use `snprintf`, preventing
an unexpectedly large geometry value from overrunning the name storage.

The source contract is covered by
`tests/automated/check-textout-pixmap-name-bounds.py`.

## TextOut position-to-coordinate source validation

`_XmTextPosToXY` now rejects a text-source block that does not advance the
position or reports bytes without a source pointer. This avoids an infinite
loop and prevents invalid blocks from reaching width measurement.

Regression coverage: `tests/automated/check-textout-postoxy-source-progress.py`.

The same check also protects `MakePositionVisible()` from narrowing
line-table `XmTextPosition` values to `int` while clamping the target line.

## TextOut resize-measurement source validation

Resize-time width and height measurement now rejects non-advancing or
malformed text-source blocks before continuing. On malformed input it returns
the conservative zero measurement instead of looping or measuring invalid
memory.

Regression coverage: `tests/automated/check-textout-resize-source-progress.py`.

## Unicode paragraph source-pointer validation

The Unicode paragraph cache now rejects a text-source read that reports
positive progress and bytes but supplies no source pointer. This prevents a
null pointer from reaching the paragraph snapshot `memcpy` path.

Regression coverage: `tests/automated/check-textout-unicode-paragraph-pointer.py`.

## TextOut viewport bounds

TextOut recalculates horizontal and vertical offsets after measuring a line.
During a transient resize, highlight, shadow, and text margins can consume the
entire widget allocation.  Treating the remaining viewport as a negative
dimension makes the offset calculation move in the wrong direction and can
leave a tiny text widget scrolled away from its content.

The offset paths now widen the allocation and margin arithmetic and clamp a
margin-only viewport to zero.  Normal viewports keep the same dimensions and
scroll behavior.  The contract is protected by
`tests/automated/check-textout-viewport-bounds.py`.

## TextOut visibility-edge bounds

`MakePositionVisible` compares the caret against the inner widget's right or
bottom edge before changing the scroll offset.  The edge is the allocation
minus a margin; converting that subtraction to `Position` first can wrap when
the margin is larger than a transient allocation.

The comparisons now widen the allocation and margin before subtraction and keep
the result signed.  Normal scrolling is unchanged, while tiny widgets use the
correct negative edge instead of an unrelated positive coordinate.  The
contract is protected by `tests/automated/check-textout-visible-edge-bounds.py`.

## TextOut coordinate hit-testing source validation

`XYToPos` now rejects non-advancing or malformed text-source blocks before
character measurement. This keeps coordinate hit testing bounded when a
source implementation fails to provide a valid block.

Regression coverage: `tests/automated/check-textout-xytopos-source-progress.py`.

## Text malformed transfer completion

`DoStuff` now explicitly fails a transfer when callback state is incomplete,
provided a transfer ID is available. Teardown and cancellation paths therefore
cannot leave an external selection request pending indefinitely.

Regression coverage: `tests/automated/check-textsel-dostuff-failure.py`.

## Text selection drop format validation

Text drops accepted by `DropTransferProc` are byte-oriented targets. The
callback now rejects external 16-bit and 32-bit formats, and lengths that
cannot be represented by the text conversion path, before conversion or
insertion.

Regression coverage: `tests/automated/check-textsel-drop-format.py`.

## Text target-discovery failure completion

`HandleTargets` now completes transfers with failure when target data is
malformed, the insertion overlaps the current selection, or follow-up state
cannot be allocated. These paths no longer leave an external selection request
pending after freeing its callback data.

Regression coverage: `tests/automated/check-textsel-handle-targets-failure.py`.

## TextStrSo wide callback conversion

Wide modify-verify callbacks may replace both their text pointer and length.
The source now copies that callback text into bounded, owned storage before
adding a terminator and converting it, avoiding writes into callback-owned
buffers.

Regression coverage: `tests/automated/check-textstrso-wide-callback-copy.py`.

## TimePicker coordinate bounds

TimePicker clamps its formatted time label before passing it to Motif's signed
`Position` drawing arguments.  Large picker heights or font metrics therefore
cannot wrap the label to an unrelated screen coordinate.

`tests/automated/check-time-picker-coordinate-bounds.py` protects this drawing
contract.

## TimePicker event guards

TimePicker validates its widget type and event pointer before handling field
selection, keyboard navigation, or activation. Invalid dispatch is ignored;
normal live-picker behavior is unchanged.

## Toast action-position bounds

`XmToast` places its optional action button from the toast width and the
button width. The old direct cast to Motif `Position` could wrap the action
origin on unusually wide layouts.

The origin now uses a signed wide calculation and clamps to the representable
Position range. Normal toast placement is unchanged.

The focused source regression is
`tests/automated/check-toast-action-position-bounds.py`.

## Toast callback guards

Toast action and timeout callbacks validate their widget data before reading
private state. Foreign, stale, or teardown-time callback data is ignored
safely while normal action and timeout dismissal remain unchanged.

## Toast child allocation

Toast creates message and action children during initialization.  Geometry,
callback registration, and resource updates now guard missing children so an
allocation failure cannot crash a later redraw or `SetValues` operation.

The contract is protected by
`tests/automated/check-toast-child-failure-guards.py`.

## Toast event guards

Toast validates its widget type and event pointer before handling Escape
dismissal. Invalid dispatch is ignored safely while normal live-toast behavior
is unchanged.

## Toast SetValues transactions

`XmToast` now copies replacement message and action strings before releasing
the old values. Allocation failure preserves the previous content and avoids
publishing invalid child labels; layout negotiation only treats successful
replacements as changes.

The focused source check is
`tests/automated/check-toast-setvalues-redraw.py`.

## ToggleButton default-size bounds

`XmToggleButton` derives its default geometry from label extents, margins,
indicator size, and highlight/shadow thickness.  Those sums now use widened
arithmetic and clamp to the X11 `Dimension` range before assignment.  Normal
toggle sizing is unchanged, while large resource values no longer wrap into
unrelated small dimensions.

The focused source regression is
`tests/automated/check-toggle-button-default-size-bounds.py`.

## ToggleButton geometry bounds

The ToggleButton shadow and fill paths now check the widget dimensions before
subtracting highlight and shadow margins.  Core dimensions are unsigned
`Dimension` values, so subtracting margins first could wrap a tiny widget into
a large positive rectangle after conversion to `int`.  Undersized widgets now
skip that draw operation, and the shadow path also skips calls with missing
shadow GCs.

Normal-sized toggles keep their existing margins, colors, and shadow ordering.
The focused regression is
`tests/automated/check-toggle-button-geometry-bounds.py`.

## ToggleButton menu shadow bounds

The ToggleButton menu enter and border-highlight callbacks now draw shadows
only when the widget has a positive interior after its highlight frame and both
shadow GCs are available.  Core dimensions are converted before subtraction,
preventing invalid rectangles for undersized or partially initialized menu
entries.

Normal-sized menu entries retain their existing shadow type and callback order.
The focused regression is
`tests/automated/check-toggle-button-menu-shadows.py`.

## ToggleButton ComputeSpace geometry

`XmToggleButton` recomputes label positions when the widget is smaller than
its preferred content. The old centering and right-to-left placement paths
subtracted unsigned dimensions before assigning signed text positions, so a
collapsed button could wrap its label origin.

The calculations now convert each dimension before subtraction. Normal label
placement is unchanged while undersized buttons keep deterministic positions.

## ToggleButton indicator margin bounds

ToggleButton grows label margins to reserve space for its indicator. The
requested increase can exceed a legacy `Dimension`; the widget now saturates
each affected margin and records the actual increase rather than the
unrepresentable requested delta. Later layout changes can therefore remove
exactly the reserved space without wrapping an origin or margin.

The focused regression is
`tests/automated/check-toggle-indicator-margin-bounds.py`.

## ToggleButton layout geometry

ToggleButton centers its label and indicator by subtracting margins, insets,
and indicator dimensions from unsigned widget dimensions. Small or temporarily
collapsed buttons could wrap those calculations before converting to a signed
position, moving text or indicators far outside the button.

The layout and indicator drawing paths now convert each dimension before
subtraction. Normal button geometry is preserved while tiny controls remain
stable during resize and realization.

## ToggleButton pixmap redisplay bounds

ToggleButton temporarily uses its active pixmap dimensions as Label text
rectangle geometry while redrawing. The pixmap API supplies unsigned X11
dimensions wider than Motif's legacy rectangle fields. Redisplay now clamps
the temporary origin and dimensions at those boundaries, preventing a large
pixmap from wrapping the label draw geometry. Normal pixmaps retain the same
layout and drawing behavior.

The focused regression is
`tests/automated/check-toggle-pixmap-geometry-bounds.py`.

## ToggleButton redisplay bounds

ToggleButton and ToggleButtonGadget redisplay now convert their unsigned widget
dimensions to `int` before subtracting margins and offsets.  The existing
non-negative clamp therefore handles collapsed widgets instead of receiving a
wrapped large value for the clear rectangle.

The focused regression is `tests/automated/check-toggle-redisplay-bounds.py`.

## ToggleButton gadget shadow bounds

ToggleButton gadget menu-arming and redisplay paths now check that the gadget
is wider and taller than twice its highlight thickness before drawing a shadow.
Collapsed gadgets therefore skip the shadow instead of converting a negative
inset span to a huge unsigned rectangle.

The source-level regression is `tests/automated/check-toggle-shadow-bounds.py`.

## ToggleButton pixmap sizing dimensions

`SetToggleSize` now clears its temporary width and height outputs before each
pixmap metadata lookup.  A failed or incomplete lookup therefore contributes
zero dimensions instead of reusing the dimensions of a preceding pixmap.
Valid pixmap metadata continues to participate in the existing maximum-size
calculation.

The focused regression is
`tests/automated/check-toggle-size-pixmap-dimensions.py`.

## Toggle-switch knob centering

The toggle knob is vertically centered from the track and knob dimensions.
This keeps the control symmetric when its geometry changes and removes the
previous hard-coded offset that placed the 18-pixel knob one pixel too low in
the 24-pixel track.

The native pixel contract is checked by
`tests/automated/run-toggle-switch-pixels.py`.

The native runner requires `--build` pointing to the configured build directory
and `--out` pointing to a fresh absolute results directory. It executes the real
widget rather than checking implementation text.

## ToggleSwitch graphics-context setup

`XmToggleSwitch` owns one mutable graphics context for its track, knob and label.
If allocation fails, expose skips drawing and destruction checks the handle
before freeing it. There is no partial set of drawing GCs to retain or release.

`tests/automated/run-toggle-switch-failures.py` injects a real `XCreateGC`
failure during construction and exercises expose and destruction. Pass `--build`
with the configured build directory and `--out` with a fresh absolute results
directory.

## ToggleSwitch state API inputs

`XmToggleSwitchGetState` and `XmToggleSwitchSetState` now reject a null
widget before invoking the Xt subclass check. Invalid optional handles return
the neutral false/no-op result; valid switch state and callback behavior are
unchanged.

The focused regression check is
`tests/automated/check-toggle-switch-inputs.py`.

## ToggleSwitch label rollback

`XmToggleSwitch` now copies a changed label before releasing the current
`XmString`. Allocation failure preserves the existing label, while NULL still
explicitly clears it.

The native allocation-failure regression is
`tests/automated/run-toggle-switch-failures.py`.

The native runner requires `--build` pointing to the configured build directory
and `--out` pointing to a fresh absolute results directory. It executes the real
widget rather than checking implementation text.

## ToggleSwitch rounded drawing

ToggleSwitch tracks and knobs now use X11 filled arcs with a centered
rectangle, rather than a hand-rasterized row loop. The geometry is symmetric
for both ends and remains correct when the control dimensions change, avoiding
the one-pixel protrusions and uneven circles produced by the old approximation.

The focused source contract is checked by
`tests/automated/check-toggle-switch-rounding.py`.

## ToggleButton text-origin bounds

ToggleButton computes text and accelerator origins from core size, indicator
space, margins, highlights, and shadows. Those sums can exceed the signed
legacy `Position` field even when every individual resource is valid. The
initial layout and later `ComputeSpace` alignment now retain wide arithmetic
and clamp only when storing the final text rectangle origin.

The focused regression is
`tests/automated/check-toggle-text-origin-bounds.py`.

## ToggleButton shadow bounds

The ToggleButton menu redisplay path now checks the widget's inner width and
height and both shadow GCs before drawing.  Collapsed or partially initialized
widgets therefore skip the shadow instead of passing wrapped inset dimensions
to Xlib.

The source-level regression is `tests/automated/check-toggleb-shadow-bounds.py`.

## ToggleButtonGadget box bounds

`DrawBox` now skips shadow and fill drawing when its computed edge is
non-positive or either shadow GC is unavailable.  This prevents invalid X draw
requests for collapsed gadget geometry while preserving the existing fill
margin behavior for valid boxes.

The focused regression is `tests/automated/check-togglebg-box-bounds.py`.

## ToggleButton fill origin bounds

The ToggleButton label and etched-menu fill paths derive their rectangle from
the gadget position plus highlight and shadow margins. Those resources are
unsigned `Dimension` values, while the position is a signed X11 coordinate.
The old locals could wrap the margin sum or the origin before the existing
width and height checks ran.

Both helpers now use signed `int` locals for the margin and fill origins. This
keeps off-edge and narrow gadgets deterministic without changing resources,
callbacks, the Motif ABI, or the fill dimensions. The source regression
`tests/automated/check-togglebg-fill-origins.py` covers both paths.

## ToggleButtonGadget shadow geometry bounds

The ToggleButtonGadget shadow path now skips drawing when the gadget is no
larger than twice its highlight thickness or when either shadow GC is absent.
It also converts the unsigned rectangle dimensions before subtracting the
highlight margin.  Tiny gadgets therefore cannot wrap into oversized X draw
requests.

The focused regression is `tests/automated/check-togglebg-geometry-bounds.py`.

## ToggleButton gadget highlight bounds

ToggleButton gadget menu-leave highlighting now passes signed inset dimensions
to `XmeDrawHighlight`.  A collapsed gadget therefore reaches the shared
zero-area guard instead of passing a wrapped `Dimension` span.

The source-level regression is `tests/automated/check-togglebg-highlight-bounds.py`.

## ToggleButtonGadget hook allocation handling

ToggleButtonGadget's secondary-object, set-values, and get-values hooks now
check extension-object and extension-data allocations.  Partial objects are
released before returning.  Initialize, set-values, and get-values posthooks
also tolerate missing extension records when a prehook could not establish
temporary state, preventing cleanup-time null dereferences.

The regression check is `tests/automated/check-togglebg-hook-allocation.py`.

## ToggleButtonGadget layout geometry

ToggleButtonGadget has the same undersized-control cases as the widget: pixmap
centering, accelerator placement, and label centering subtract unsigned
dimensions before storing signed positions. A collapsed gadget could therefore
wrap its content origin.

The affected paths now perform signed subtraction before assigning positions,
preserving normal menu and gadget layout while keeping tiny entries bounded.

## ToggleButton gadget draw origins

ToggleButton gadget shadow and highlight calls now add highlight thickness to
`Position` coordinates in signed arithmetic.  Gadgets positioned partly
outside a parent therefore retain their negative/off-edge origin instead of
wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-togglebg-origins.py`.

## ToggleButtonGadget pixmap redisplay bounds

ToggleButtonGadget temporarily maps its selected X pixmap dimensions into the
LabelGadget text rectangle during redisplay. The pixmap values are wider than
the legacy rectangle coordinate and dimension fields, so the values are now
explicitly clamped at that boundary. This preserves ordinary pixmap layout
while avoiding wrapped redraw geometry for very large pixmaps.

The focused regression is
`tests/automated/check-togglebg-pixmap-geometry-bounds.py`.

## ToggleButton shadow origin bounds

`DrawToggleShadow` already rejects gadgets whose highlight consumes the
available drawing area, but its origin calculation added an unsigned
highlight thickness to the X11 `Position` before casting the result to `int`.
That could wrap an off-edge or negative origin before `XmeDrawShadows` saw it.

The helper now promotes both operands before addition. The fit checks,
dimensions, Motif resources, and public ABI remain unchanged. The regression
`tests/automated/check-togglebg-shadow-origins.py` keeps this helper on the
signed-arithmetic path.

## ToggleButtonGadget pixmap sizing dimensions

`SetToggleSize` now clears its temporary width and height outputs before each
of the six selected and insensitive pixmap metadata lookups.  A failed or
incomplete lookup therefore contributes zero dimensions instead of reusing a
preceding pixmap's size.

The focused regression is
`tests/automated/check-togglebg-size-pixmap-dimensions.py`.

## ToggleButtonGadget text-origin bounds

ToggleButtonGadget combines indicator, margin, highlight, shadow, and
rectangle dimensions to place text and accelerator rectangles. Those valid
inputs can exceed the signed legacy coordinate field when combined. Initial
layout and `ComputeSpace` now use wide arithmetic and clamp only at the final
LabelGadget `Position` storage boundary.

Indicator margin deltas and the automatically recomputed rectangle dimensions
are also saturated. The stored delta now records the actual bounded margin
increase, so later layout updates can remove exactly the space that was added.

The focused regression is
`tests/automated/check-togglebg-text-origin-bounds.py`.

## ToggleButton color-conversion fallback

ToggleButton SetValues paths now validate the payload returned by the
select-color converter. A missing or wrong-sized payload falls back to the
widget foreground; valid converter results retain their existing color.

The regression check is
`tests/automated/check-togglebutton-color-conversion.py`.

## ToggleButtonGadget resource-list allocation

ToggleButtonGadget class initialization merges the LabelGadget and
ToggleButtonGadget resource tables into one cache-object list.  The merged
count and Xt byte size are now checked before allocation, and initialization
returns without writing through a null list when memory is unavailable.

The focused source-level regression is
`tests/automated/check-togglebutton-gadget-resource-list.py`.

## ToggleButton indicator fallback

ToggleButton and ToggleButtonGadget now tolerate an unnormalized
`XmONE_OF_MANY` indicator state by skipping that draw operation. The normalized
diamond, round, and other documented indicator styles are unchanged.

The regression check is
`tests/automated/check-togglebutton-indicator-fallback.py`.

## ToggleButton visual-state fallback

ToggleButton and ToggleButtonGadget painting now treats an invalid visual state
as a no-paint fallback. The normal unset, set, and indeterminate states retain
their existing rendering; malformed state does not terminate the application.

The regression check is
`tests/automated/check-togglebutton-visual-state.py`.

## ToggleSwitch label initialization rollback

ToggleSwitch now checks the private copy of its initial label.  If
`XmStringCopy` fails, it installs an empty compound string instead of passing a
NULL label to geometry measurement and drawing.  This keeps construction safe
and preserves a usable control under allocation pressure. If allocating the
empty string also fails, measurement and rendering tolerate the absent label.

The focused regression is
`tests/automated/run-toggle-switch-failures.py`.

The native runner requires `--build` pointing to the configured build directory
and `--out` pointing to a fresh absolute results directory. It executes the real
widget rather than checking implementation text.

## Toolbar action-label allocation guards

`XmToolbar` now handles conversion and allocation failures while copying item
strings and composing action labels with shortcuts. Length arithmetic is checked
before allocation, and the existing label formatting is unchanged for valid
inputs.

The focused regression check is
`tests/automated/check-toolbar-action-label-guards.py`.

## Toolbar callback guards

Toolbar validates its action callback closure before walking private action
storage. Foreign or stale callback data is ignored safely while normal
toolbar and menu-button activation remains unchanged.

## Toolbar layout allocation failures

Toolbar layout uses temporary arrays for action selection and preferred
dimensions.  A failed allocation must not reach `choose_actions`, because that
routine writes all three arrays.  The layout now hides the existing action and
overflow presentation, reports every action as overflow, and returns cleanly
when scratch storage is unavailable.  The toolbar can be laid out again after
memory pressure is relieved.

The contract is protected by
`tests/automated/check-toolbar-layout-allocation.py`.

Changing `XmNtoolbarOverflowLabel` also updates the already-created overflow
button immediately; the resource no longer requires destroying and recreating
the toolbar to become visible.

Action callback registration and icon updates also tolerate an internal button
creation failure, allowing the remaining model and layout state to survive
allocation pressure.

## Toolbar natural width bounds

Toolbar natural-width accumulation now saturates at the representable
`Dimension` limit before narrowing. Large action sets can no longer wrap the
intermediate width and produce an unexpectedly tiny toolbar.

## Tooltip label teardown guard

The optional tooltip label-destroy callback now ignores a missing callback
widget and avoids destroying a missing parent shell. Normal tooltip label and
shell teardown remains unchanged.

The focused source-contract check is
`python3 tests/automated/check-tooltip-destroy-guard.py`.

## ToolTip string rollback

`XmSetToolTipString` now copies a non-NULL replacement before releasing the
current tooltip.  Allocation failure leaves the existing tooltip intact;
passing NULL still intentionally clears it.

The focused regression is `tests/automated/check-tooltip-string-rollback.py`.

## Tooltip timer initialization

The VendorS tooltip trait stores timeout handles as `XtIntervalId`.  Its
initialization used pointer-to-`int` null casts, which produced diagnostics and
could truncate a sentinel on platforms where timeout IDs are wider than an
integer.

The fields now initialize with typed zero timeout IDs, matching the rest of the
tooltip lifecycle and preserving the existing no-timer state.

`tests/automated/check-tooltip-timer-types.py` protects the declaration and
initialization contract.

## Tooltip trait allocation

`XmSetToolTipString` now checks the allocation of a missing tooltip trait
before registering or using it. If allocation fails, the setter safely leaves
the existing widget state unchanged; successful tooltip creation and updates
are unchanged.

## Trait replacement ownership

`XmeTraitSet` now replaces an existing `(object, name)` binding instead of
stacking duplicate hash entries. The superseded key is released after the new
binding succeeds; if allocation fails, the old binding is restored when
possible. Removing a trait still releases its key as before.

The focused regression check is
`tests/automated/check-trait-replacement.py`.

## Transfer argument-count types

`XmeDragSource` and `XmeDropSink` assemble Xt argument lists whose public
count type is `Cardinal`. Their temporary counters now use that same type,
with explicit bounded allocations, avoiding signed narrowing and signed/unsigned
loop comparisons on 64-bit builds.

The focused regressions are
`tests/automated/check-transfer-arg-count-types.py` and
`tests/automated/check-drag-source-allocation.py`.

## Drag/drop argument-array bounds

`XmeDragSource` and `XmeDropSink` append toolkit-owned arguments to an
application-provided `ArgList`. Their count checks now bound the complete
byte size against both host `size_t` and Xt's `Cardinal` allocation width
before calling `XtMalloc`, preventing truncation on wide-size platforms.

The focused regressions are
`tests/automated/check-drag-source-allocation.py` and
`tests/automated/check-drop-sink-allocation.py`.

## Transfer request-block allocation

`XmTransferValue` adds each requested target to a linked transfer-block list.
The block allocator now reports `NULL` on failure, and the caller unlocks and
returns before mutating request counts or dereferencing the block.

Successful requests retain their ordering, counters, and selection behavior;
failed allocation leaves the transfer context unchanged.

## Deferred clipboard callback inputs

Deferred clipboard callbacks can arrive through cancellation and destruction
paths as well as normal data delivery.  `ClipboardCallback` now rejects a
missing reason, and rejects a missing target for non-delete callbacks before
dereferencing either pointer.  It also rejects duplicate or already-completed
data IDs and removes the hash entry using the data-ID value, matching the key
used when the request was registered.  This prevents callback-time crashes,
counter underflow, and stale snapshot entries.

The focused regression is
`tests/automated/check-transfer-callback-inputs.py`.

## transfer-callback-payload-guards
Normalize malformed transfer callback results
==============================================

The internal converter path also releases a converter-owned payload when its
format, type, length, or pointer combination is malformed, or when conversion
does not complete.  Rejected output is cleared before callers observe the
failure, preventing a leak while preserving the existing failure contract.

`SelectionCallbackWrapper` now validates selection-owner results before
publishing them through `XmSelectionCallbackStruct`.  A missing selection atom
and invalid 8/16/32-bit
formats and non-empty replies without both a type atom and payload pointer are
released, converted to an empty result, and reported with `XmCR_PROTOCOLS`
instead of `XmCR_OK`.  Valid results retain their original type, format,
length, and callback reason. A zero-length reply may carry either a null pointer
or an Xt-allocated empty value. Both are valid selection results, including the
empty acknowledgement used by WSM SET_STATE. The receiving callback retains
ownership of any delivered allocation and releases it normally.

The focused regression is:

```text
python3 tests/automated/check-transfer-callback-payload.py
```

The native MWM/Wsm acceptance runner exercises empty SET_STATE acknowledgements
through the real selection-transfer callback path.

## Client-window conversion safety

The standard transfer converter now handles a widget with no shell ancestor
when serving the client-window target. It frees the temporary result and
reports an unavailable conversion instead of dereferencing a null ancestor
during teardown or unusual widget hierarchies.

The focused regression check is
`tests/automated/check-transfer-client-window.py`.

## Transfer-context API inputs

Transfer completion, parameter, value, and MULTIPLE-request APIs now reject a
null transfer context or context with no widget before dereferencing it or
looking up its application context. Invalid callback handles are no-ops; valid
transfer queue ownership and cleanup behavior is unchanged.

The focused regression check is
`tests/automated/check-transfer-context-inputs.py`.

## Transfer context lookup

Transfer conversion contexts are keyed by display and selection atom. Their
lookup and insertion now recheck the key while holding `_XmProcessLock`, so
concurrent callers share one context instead of leaking a duplicate. Hash
table allocation and insertion failure also return cleanly without probing a
null table.

The regression check is
`tests/automated/check-transfer-context-lookup.py`.

## transfer-convert-result-guards
Guard typeless conversion results
=================================

`_XmConvertHandler` now rejects a conversion result that reports a non-empty
payload with `type == None`.  X selection data must identify its property type;
otherwise consumers may interpret an otherwise valid pointer using an
undefined atom.  The handler still accepts empty results and preserves the
existing 8/16/32-bit format checks.  Rejected results are returned as an
unsuccessful conversion with no published payload.

The focused regression check is:

```text
python3 tests/automated/check-transfer-convert-result.py
```

## Clipboard copy-status bookkeeping

Clipboard transfer setup can reject a format or length after the transfer
code has requested the copy.  The transfer code now increments its successful
count only when `XmClipboardCopy` returns `ClipboardSuccess`; deferred
snapshot requests are added to the callback dictionary only after the same
check.  Failed requests therefore cannot trigger a false successful MOVE or
leave an unresolvable callback entry behind.  Invalid selection formats are
also rejected before byte-length conversion.

The focused regression is
`tests/automated/check-transfer-copy-status.py`.

## Transfer-counter bounds

Transfer contexts keep request totals in signed `int` fields for ABI
compatibility.  `XmTransferValue` now refuses a new request when either
`count` or `outstanding` has reached `INT_MAX`, before allocating or linking a
transfer block.  This prevents signed wraparound from making a live transfer
appear complete or leaving cleanup counters negative.

The focused regression is
`tests/automated/check-transfer-counter-bounds.py`.

## Drop destination argument count

The drop-destination setup passes its argument count to Xt APIs, whose public
type is `Cardinal`. The local counter now uses that type, avoiding a signed
narrowing diagnostic while preserving the same one-argument setup.

The focused regression is
`tests/automated/check-transfer-destination-arg-count.py`.

## Transfer completion callback snapshot

Transfer completion now copies the callback list and operation while holding
the transfer process lock, then invokes the private snapshot after unlocking.
Concurrent cleanup or reentrant completion can no longer reclaim the array
while it is being traversed.

Regression coverage: `tests/automated/check-transfer-done-callback-snapshot.py`.

## Transfer completion callbacks

Transfer completion callbacks are invoked during cancellation, replacement,
and normal destruction. `XmeTransferAddDoneProc` now rejects a null callback
before it can enter the completion list, and rejects registration after the
transfer is flushed because that callback can no longer be delivered. Teardown
therefore does not retain unreachable callback allocations or dereference an
invalid function pointer.

The focused regression is
`tests/automated/check-transfer-done-proc-null.py`.

The flushed-state regression is
`tests/automated/check-transfer-done-proc-flushed.py`.

## Transfer completion-list allocation width

`XmeTransferAddDoneProc` bounds its completion-list byte count to Xt's
`Cardinal` range before reallocating. The checked `size_t` value is now passed
with an explicit `Cardinal` conversion, documenting the allocator contract on
all supported pointer widths.

The focused regression is
`tests/automated/check-transfer-done-proc-allocation-width.py`.

## Transfer completion callback operation

`CallDoneProcs` snapshots completion callbacks when possible.  Under
allocation pressure it falls back to fetching and invoking one callback at a
time so re-entrant callbacks remain safe.  The transfer operation is now
captured before that allocation attempt, ensuring the fallback receives the
same `XmTransferOp` instead of an uninitialized value.

Regression: `tests/automated/check-transfer-done-proc-fallback-op.py`

## Transfer done-proc inputs

Transfer completion dispatch now validates the transfer context, completion
record, callback count, and callback list before iterating done procedures.
Malformed internal completion paths therefore return safely.

Regression coverage: `tests/automated/check-transfer-done-proc-inputs.py`.

## Transfer done-procedure locking

Transfer-context done-procedure reclamation now occurs under the same process
lock used when registering callbacks. A finishing transfer can therefore not
free the callback list concurrently with `XmeTransferAddDoneProc`.

Regression coverage: `tests/automated/check-transfer-done-proc-locking.py`.

## Transfer completion and application locking

When a failed drop has no queued requests, `XmTransferDone` must finalize the
transfer immediately. Finalization invokes completion callbacks, which may
legitimately call back into Motif. The application lock is now released before
that finalization, avoiding a reentrant deadlock while retaining the existing
drop failure status updates.

Regression coverage: `tests/automated/check-transfer-done-unlocks-before-finish.py`.

## Transfer completion-callback allocation

`XmeTransferAddDoneProc` grows the callback array while a transfer context is
locked. The count and pointer now change only after the new allocation
succeeds, and null or unrepresentable contexts are ignored safely. A failed
growth therefore leaves all previously registered callbacks intact and avoids
a null write during transfer teardown.

Successful registrations retain their original order and callback semantics.

## Transfer encoding lookup inputs

`XmeGetEncodingAtom` now rejects a null widget before deriving its application
context or querying the display. Invalid optional handles return `None`; valid
encoding-atom discovery is unchanged.

The focused regression check is
`tests/automated/check-transfer-encoding-inputs.py`.

## Transfer source and sink entry inputs

Selection and drag-transfer entry points now reject null widget handles before
application-context or display lookup. Named sinks also reject an absent
selection atom, and standard conversion rejects a missing callback structure.
Valid source, sink, drag, and conversion behavior is unchanged.

The focused regression check is
`tests/automated/check-transfer-entry-inputs.py`.

## Transfer completion reentrancy

Transfer contexts now record completion under the process lock before done
callbacks run. Reentrant or concurrent completion attempts return immediately,
so callback delivery and context reclamation occur only once.

Regression coverage: `tests/automated/check-transfer-finish-reentrancy.py`.

## Transfer-id list locking

Active transfer IDs and reusable transfer IDs share process-global linked
lists. `FreeTransferID` now unlinks an ID and adds it to the free list while
holding `_XmProcessLock`, matching `GetTransferID` and preventing concurrent
completion callbacks from corrupting the list. A null ID is ignored safely.

The regression check is
`tests/automated/check-transfer-id-locking.py`.

## Transfer context reuse

Transfer contexts are now fully initialized before they are published on the
global context chain. Recycled contexts clear callback, widget, selection,
and drag/drop state after the old list is freed, preventing concurrent or
reused transfers from observing stale state.

Regression coverage: `tests/automated/check-transfer-id-reuse.py`.

## Locale-text allocation width

The locale-text conversion path bounds its accumulated string size below
Xt's `Cardinal` maximum before allocating. The allocation now passes that
validated size explicitly as `Cardinal`, preserving the checked contract on
32-bit and 64-bit builds.

The focused regression is
`tests/automated/check-transfer-locale-allocation-width.py`.

## Transfer location cleanup

Container, Text, and TextField register completion callbacks to release
temporary drag-location data. Those callbacks now tolerate a missing
completion structure, a stale transfer ID, or an already-cleared location.
This keeps cancellation, destruction, and reentrant completion paths from
turning cleanup into a null dereference or repeated free.

The regression check is
`tests/automated/check-transfer-location-cleanup.py`.

## Transfer merge allocation

`XmeConvertMerge` builds a new buffer before replacing the existing conversion
value. If that allocation fails, it now reports the existing conversion error,
releases the process lock, and leaves the original value and length unchanged.

Successful merges retain the same byte ordering and ownership transfer. The
failure path avoids dereferencing a null merged buffer or losing valid source
data under memory pressure.

## Transfer parameter formats

`XmTransferSetParameters` stores parameters for the next selection request.
The X selection protocol supports only 8-, 16-, and 32-bit formats; the API
continues to treat zero as the existing 8-bit default and now ignores other
values before touching the transfer context.  This prevents malformed callers
from creating parameter requests whose length and payload interpretation do
not match the protocol; a non-null buffer with a `None` type is rejected as
well.

The regression check is
`tests/automated/check-transfer-parameter-format.py`.

## Transfer parameter input validation

`XmTransferSetParameters` now rejects a nonzero parameter length when the
parameter buffer is `NULL`, before forwarding the request to Xt. Zero-length
parameters and non-null buffers retain their existing format and atom
validation behavior.

The focused source-contract check is
`tests/automated/check-transfer-context-inputs.py`.

## transfer-skipped-value-cleanup
Release skipped transfer-selection values
==========================================

`SelectionCallbackWrapper` now tracks whether a selection result was delivered
to an application callback.  Results arriving after cancellation, for ignored
transfer blocks, without a transfer block, or without a callback procedure are
released by the wrapper instead of leaking the Xt-owned selection buffer.
Delivered results retain the existing callback ownership contract and are not
freed by the wrapper.

The focused regression is:

```text
python3 tests/automated/check-transfer-skipped-value-cleanup.py
```

## Transfer status input validation

`XmTransferDone` now rejects values outside the four public
`XmTransferStatus` enumerators before changing queue state or sending a
selection request. Valid `SUCCEED`, `FAIL`, `CONTINUE`, and `DEFAULT` calls
retain their existing semantics; malformed enum values fail closed.

The focused source-contract check is
`tests/automated/check-transfer-context-inputs.py`.

## Clipboard transfer status scope

The clipboard-manager ownership branch now uses a distinct
`selection_status` variable. This avoids shadowing the enclosing transfer
status and keeps the ownership result separate from the Motif clipboard
operation result.

The focused regression is
`tests/automated/check-transfer-status-scope.py`.

## Selection text concatenation width

`_XmTextToLocaleText` now rejects a converted string total that cannot fit
the `Cardinal` size accepted by `XtMalloc`, including the terminator byte.
Normal text conversion is unchanged; oversized external selection data is
reported as a failed conversion instead of being truncated at the allocator
boundary.

The focused regression check is
`tests/automated/check-transfer-text-concat-width.py`.

## Text selection conversion concatenation

`_XmTextToLocaleText` already measures every converted segment before
allocating its result. It now uses that size to copy each segment at a tracked
offset, rather than repeatedly scanning the growing result with `strcat`.
Large multi-segment selections therefore retain the same concatenated bytes
with linear copy work.

The source contract is checked by:

```sh
python3 tests/automated/check-transfer-text-concat.py
```

## Transfer-value target validation

`XmTransferValue` now rejects the `None` atom before creating a transfer
block or incrementing outstanding request counts.  This avoids queuing an
invalid selection request and keeps cancellation/cleanup bookkeeping
balanced.  A null callback remains permitted for internal delete requests.

The focused regression is `tests/automated/check-transfer-value-target.py`.

## Transient focus and stacking guards

Focus repair and transient-tree normalization can run after a client has been
removed.  `FindTransientFocus` now falls back to the current keyboard focus,
while `BumpPrimaryToBottom` and `NormalizeTransientTreeStacking` report no
change for a missing tree.  Valid modal focus and stacking behavior is
unchanged.

When modal focus search reaches a sibling list, it now tests each sibling's
modal state while advancing.  The first focusable sibling is selected instead
of being skipped because an earlier sibling was modalized.

## Transient list guards

Transient windows can be withdrawn while their workspace or parent tree is
being detached.  `AddTransient` now validates workspace, screen, leader, and
tree-top state before linking or restacking.  `DeleteTransient` safely ignores
an already-detached client.  Existing modal-count and stacking behavior is
unchanged for valid transient trees.

## Transient sibling guards

Transient sibling ordering can run while a parent or sibling is being
withdrawn.  `PutTransientOnTop` now stops when the requested client is not
found in its parent’s chain, and `PutTransientBelowSiblings` handles an empty
child list.  Both helpers also reject a null client, preserving valid
restacking behavior while preventing malformed-chain dereferences.

## Transient traversal guards

Transient-tree traversal can be requested after a client has been detached.
`FindTransientTreeLeader` and `FindTransientOnTop` now return `NULL` for a
missing client, while `CountTransientChildren` returns zero.  Callers can
therefore skip restacking safely instead of dereferencing an invalid tree.
Valid traversal results are unchanged.

## Transient window-list guards

Restacking builds a temporary window list from the transient tree.  The list
builder now returns its current output pointer when the output buffer or
client tree is missing, avoiding writes through a null buffer during failed
allocation or teardown.  Valid list construction is unchanged.

## Traversal active-item and shell validation

`_XmGetActiveItem` now returns `NULL` for a missing widget, and focus-moved
dispatch returns `False` when no top-level shell can be resolved. Valid focus
lookups and vendor-shell callback dispatch remain unchanged.

The focused regression check is
`tests/automated/check-traversal-active-item-shell.py`.

## Traversal focus-entry input validation

Focus validation, destruction, manager traversal, and focus-moved callback
helpers now return safely when required widgets are absent. Normal traversal
and callback dispatch are unchanged for valid focus transitions.

The focused regression check is
`tests/automated/check-traversal-focus-entry-inputs.py`.

## Traversal focus-data input validation

`_XmDestroyFocusData` and `_XmSetActiveTabGroup` now treat a missing focus
data record as a safe no-op. Existing focus-data destruction and active-tab
updates are unchanged when a valid record is supplied.

The focused regression check is
`tests/automated/check-traversal-focus-inputs.py`.

## Traversal geometry input validation

Visibility and rectangle utility helpers now handle missing widgets or
rectangle pointers safely: visibility reports `False`, intersection reports no
intersection, empty-rectangle checks treat null as empty, and void helpers
return without writing. Valid geometry calculations are unchanged.

The focused regression check is
`tests/automated/check-traversal-geometry-inputs.py`.

## Traversal public-input validation

Traversal predicates, focus helpers, visibility, and tab-group queries now
return safe defaults when passed a missing widget. Valid widgets continue to
use the existing application-context and focus-data behavior.

The focused regression check is
`tests/automated/check-traversal-public-inputs.py`.

## Traversal query input validation

Obscured-traversal callbacks, focus-path clearing, and focus-policy lookup now
return safely when their required widget or ancestry is absent. Valid traversal
queries and focus-path updates remain unchanged.

The focused regression check is
`tests/automated/check-traversal-query-inputs.py`.

## Traversal redirect-cycle fallback

Traversal-control traits can redirect focus repeatedly. Once the bounded
redirect pass limit is reached, `Xm` now keeps the current requested focus
widget instead of asserting and returning `NULL`. This preserves focus
navigation for malformed or mutually redirecting traits while retaining the
existing recursion bound.

## Traversal graph shrink bounds

Traversal graph pruning now validates the reduced node count against both
`size_t` and Xt's `Cardinal` allocation width before calling `XtRealloc`.
The existing transactional behavior remains intact when the shrink cannot be
represented or cannot be allocated.

Row-item and exclusion-tab list resizing uses the same dual-width checks, so
all traversal arrays reject byte counts that do not fit Xt's `Cardinal`.

The focused regression check is
`tests/automated/check-traversal-shrink-allocation.py`.

## Traversal widget input validation

Traversal lifecycle hooks now return safely when their widget argument is
missing, before invoking Xt widget macros or application-context helpers.
Valid navigation initialization, set-values, managed-change, and resize paths
remain unchanged.

The focused regression check is
`tests/automated/check-traversal-widget-inputs.py`.

## Tree centering arithmetic

`XmTree` centers each node and its disclosure button inside the computed
bounding box.  The box, child, and disclosure dimensions are Xt `Dimension`
values, so subtracting them directly can wrap when a child is larger than its
available space.  The centering differences now use signed `int` arithmetic
in both horizontal and vertical tree orientations; oversized children remain
near the box instead of receiving a wrapped coordinate.

Compressed-child width accumulation uses the same signed conversion when
subtracting the horizontal compression delta, keeping parent layout widths
valid when the delta exceeds a child’s width.

The source contract is checked by
`tests/automated/check-tree-centering-arithmetic.py`.

## Tree child and argument count bounds

Tree constraint updates and layout traversal use `Cardinal` indices for Xt
argument counts and composite-child counts. This keeps the internal loops
within the full range of their public/count fields instead of allowing a
signed `int` index to wrap on a large, valid widget tree.

The focused source regression is
`tests/automated/check-tree-count-bounds.py`.

## Tree extra-space geometry

Tree layout derives unused space by subtracting its bounding-box dimensions
from the widget dimensions.  The vertical and horizontal calculations now
convert both operands to signed integers before subtraction, so an oversized
or temporarily clipped tree reports the configured margin rather than a
wrapped near-maximum space value.

## Tree initialization allocation handling

Tree initialization now checks the resized top-node constraint record and the
child-operation list allocation before using either. Allocation failure raises
an Xt error rather than leaving a partially initialized widget whose later
callbacks could dereference null constraint or list storage; successful
hierarchy setup is unchanged.

The regression check is `tests/automated/check-tree-initialization-allocation.py`.

## TreeView coordinate bounds

TreeView keeps its row and indentation calculations in signed `int` values,
then clamps label coordinates before passing them to Motif's signed `Position`
arguments.  Large trees, indentation, or font metrics therefore cannot wrap a
label to an unrelated screen coordinate.

`tests/automated/check-tree-view-coordinate-bounds.py` protects this drawing
contract.

## TreeView event guards

TreeView validates its widget type and event pointer before processing pointer
and keyboard navigation. Invalid dispatch is ignored safely while normal
selection, scrolling, and expansion remain unchanged.

## TreeView model and state API inputs

The public TreeView model, selection, expansion, and top-row APIs now reject a
null widget before invoking the Xt subclass check. Invalid optional handles
return the existing neutral values or no-op result; valid model replacement,
selection callbacks, expansion, and scrolling behavior are unchanged.

The focused regression check is `tests/automated/check-tree-view-inputs.py`.

## TreeView model allocation

`XmTreeViewSetNodes` now builds the replacement item model privately and
publishes it only after the item array and every copied label have been
allocated successfully.  An unavailable item array or label leaves the
existing tree, expansion state, and selection untouched; partial labels are
released before returning.

Successful node replacement and the public TreeView API are unchanged.  This
prevents allocation pressure from turning model updates into null dereferences
or partially replaced trees.

## TreeView row-height bounds

`XmNtreeViewRowHeight` is a public `Dimension` resource.  A zero value left
the widget with zero-height rows after initialization or `SetValues`, making
the view effectively unusable even though `visibleRows` was already guarded.
Both lifecycle paths now clamp row height to one pixel before geometry and
input calculations.

`tests/automated/check-tree-view-row-height.py` keeps the invariant in both
paths.

## Tree-view RTL label bounds

`XmTreeView` now bounds depth offsets, RTL label-width, and X drawing arithmetic
before converting geometry to signed integers. Normal labels retain their
existing placement; deeply indented or unusually long labels no longer wrap the
calculated position.

The focused regression check is
`tests/automated/check-tree-view-rtl-label-bounds.py`.

## TreeView geometry bounds

TreeView preferred height now computes row-height multiplication in a wider
type and clamps it to the platform `Dimension` maximum. Large but valid
`XmNtreeViewRowHeight` and `XmNtreeViewVisibleRows` values no longer wrap into
an unexpectedly small geometry request; ordinary sizes are unchanged.

`tests/automated/check-treeview-geometry-bounds.py` protects this calculation.

## TreeView geometry negotiation

`XmTreeView` now tracks automatic sizing independently on each axis.  A zero
constructor or `XtSetValues` dimension requests content-derived sizing for that
axis.  A nonzero application-supplied width or height is retained in
`QueryGeometry`, so a geometry manager cannot replace it with the TreeView's
natural label or row metrics.

## Zero-sized compound-string conversions

COMPOUND_STRING export now rejects a zero-sized
`XmCvtXmStringToByteStream()` result. A valid XmString always includes an
ASN.1 header, so zero indicates conversion or allocation failure and must not
be emitted as an empty property.

The focused regression check is
`tests/automated/check-txtpropcv-asn1-zero.py`.

## Compound-text export failures

When exporting an XmString table as COMPOUND_TEXT, a failed per-string
conversion is now reported as `BadAlloc` and previously converted strings are
released. The converter no longer turns a failed conversion into an empty
property element.

The focused regression check is
`tests/automated/check-txtpropcv-ct-failure.py`.

## Empty plain-text import

When an empty plain-text property is converted, the generated empty
`XmString` is now checked before the result table is returned. Allocation
failure releases the table and returns `BadAlloc` instead of reporting success
with a NULL entry.

The focused regression check is
`tests/automated/check-txtpropcv-empty-generation.py`.

## Empty text-property results

An empty X text property is a valid conversion result. The single-item helper
now returns an allocated empty string when Xlib reports zero items, rather than
leaving the caller's output pointer uninitialized while reporting success.

The focused regression check is
`tests/automated/check-txtpropcv-empty-result.py`.

## Text-property export table inputs

All non-empty XmString tables passed to
`XmCvtXmStringTableToTextProperty` must contain valid entries. The exporter
now rejects a NULL entry with `BadValue` before any format-specific conversion,
avoiding silent empty elements and keeping all encoding styles consistent.

The focused regression check is
`tests/automated/check-txtpropcv-table-inputs.py`.

## UIL argument dump pointer widths

Argument diagnostics now print argument-name and argument-value pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-argument-dump-pointers.py`

## UIL byte-table bounds

The UIL database reader allocates byte tables from the record's advertised
`table_size` and then reads the advertised item count.  Each byte-table record
now verifies that `num_items` fits within `table_size` before reading.  A
malformed UID record is rejected with the normal bad-database diagnostic,
without changing valid database compatibility.

## UIL callback dump pointer widths

Callback diagnostics now print reason-name, procedure-reference, and
procedure-reference-list pointers with `%p` and `void *`, preserving complete
addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-callback-dump-pointers.py`

## UIL case-sensitivity tag width

The module case-sensitivity flag is a boolean value stored in the symbol
header's 8-bit tag field. The conversion is now explicit, documenting the
intent and keeping strict warning builds free of an incidental narrowing
diagnostic without changing the stored `0`/`1` values.

Regression: `tests/automated/check-uil-case-sensitivity-tag-width.py`

## UIL color-item dump pointer widths

Color-item diagnostics now print the entry, color, and next-item pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-color-dump-pointers.py`

## UIL object-comment allocation bounds

Object comments are accumulated as C strings but Xt allocation sizes use
`Cardinal`. The semantic action now rejects a comment length that cannot fit
that allocator width before adding the terminator and narrowing the size.

Regression: `tests/automated/check-uil-comment-allocation-bounds.py`

## UIL comment-buffer growth

The UIL lexer preserves adjacent source comments in a dynamically grown
buffer. Growth now uses a temporary pointer and updates the global buffer only
after the Xt allocation succeeds. If allocation fails, the lexer reports the
failure and returns without exposing a null or partially updated buffer to the
comment concatenation path.

Object comment association applies the same failure rule: a failed symbol
comment allocation leaves the destination pointer null and clears the pending
comment, preventing a later symbol from receiving stale parser state.

## UIL compound-string component length bounds

`XmStringComponentCreate` accepts an unsigned-int length, while UIL obtains
component lengths from `strlen`. Both locale and text component paths now
check that conversion before calling the public Motif API.

Regression: `tests/automated/check-uil-component-length-bounds.py`

## UIL compound-string length

Compound-string records store their assembled length in an unsigned 16-bit
field. The semantic evaluator now checks the toolkit-reported length before
storing it, preventing an overlong value from wrapping and describing a
different resource size.

Regression: `tests/automated/check-uil-compound-length.py`

## UIL control dump pointer widths

Control diagnostics now print the controlled-object pointer with `%p` and
`void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-control-dump-pointers.py`

## UIL cycle-mask validation

The semantic validator records cycle state in a bit mask. The already-checked
path now tests whether the mask is nonzero, rather than comparing it with the
literal value `1`; this preserves cycle detection when the mask bit is not the
lowest bit.

Regression: `tests/automated/check-uil-cycle-mask.py`

## UIL database allocation guards

The UIL compiler rejects a malformed or oversized database cleanly when a
nonempty allocation for table pointers, length records, strings, or enum
values fails.  It no longer passes a null allocation result into the database
reader, and releases temporary length records when later string allocation
fails.

Empty serialized sections are still accepted: no pointer is required when the
corresponding read has zero bytes and no consumer can dereference it.

## UIL database character-table bounds

Allowed-character tables allocate `num_items * num_bits` bytes and store one
pointer per item. The loader now checks the pointer-table capacity, rejects
invalid counts, checks the multiplication and Xt allocation width, and only
then reads and assigns the table pointers.

Regression: `tests/automated/check-uil-db-char-table-bounds.py`

## UIL database-header bounds

Compiled UIL database headers and global limits carry counts and byte sizes as
signed integers, but those fields feed allocation, lookup, and `fread` sizes.
Database loading rejects negative table fields before entering table dispatch
and negative global limits before publishing them. This prevents a corrupt
database from turning a signed underflow into a huge allocation or a poisoned
global loop bound. The loader returns after the diagnostic so this remains true
when an embedding application supplies a non-fatal diagnostic handler.

Regression: `tests/automated/check-uil-db-header-bounds.py`

## UIL database integer-table bounds

Enumeration-set records contain a variable number of unsigned-short values.
The loader validates the descriptor-table capacity, rejects negative counts,
and accumulates the value count in `size_t` with an Xt allocation bound.  It
then computes the integer-table byte count once and uses that exact value for
both allocation and input.  This keeps the allocation unit explicit and avoids
repeating unchecked element-to-byte arithmetic before reading or rebasing the
values.

Regression: `tests/automated/check-uil-db-int-table-bounds.py`

## UIL database name-length-table bounds

Name tables carry a separate signed length array. The loader now validates the
pointer-table capacity and `num_items + 1` arithmetic, rejects negative string
lengths, and bounds the accumulated serialized string bytes before allocation
and pointer rebasing.

Regression: `tests/automated/check-uil-db-length-table-bounds.py`

## UIL raw database table allocation

Several WMD records contain raw byte, `unsigned short`, or `int` tables.
Their loader now routes each allocation through a shared helper before passing
the result to `fread`.  An empty serialized table gets a one-byte object, so
the input API always receives a valid destination; an allocation failure is
diagnosed and terminates database loading.  In particular, the loader does not
mistake the unread record payload for a subsequent table header.

This does not alter the serialized database format or valid table contents.
`tests/automated/check-uil-db-raw-table-allocation.py` covers the loading
contract for every raw-table record type.

## UIL database reader failures

The compound WMD table readers now return success only after consuming their
entire record.  The outer loader closes the database immediately when a helper
rejects a count, cannot allocate, or encounters a short read.  It therefore
cannot interpret unread record bytes as another database header.

The same rule applies to the globals and record-header framing: normal end of
file is detected before attempting a header read; a partial header, invalid
header, unknown table ID, or record/count mismatch stops loading immediately.

Zero-byte table payloads are handled without calling `fread` with an empty
element size.  This preserves valid empty records while making incomplete
records fail deterministically.

`tests/automated/check-uil-db-reader-status.py` protects the reader/loader
contract.

## UIL database scalar widths

Database reads return `size_t` counts while several UIL globals and generated
identifiers use compact legacy types. The loader now makes those bounded
conversions explicit at the `fread`, charset-count, and user-defined-object
boundaries.

Regression: `tests/automated/check-uil-db-scalar-widths.py`

## UIL database string-table bounds

Key-table database records contain a serialized table followed by variable
length names. The loader now validates the record capacity, accumulates name
storage in `size_t` with an Xt-`Cardinal` bound, and checks allocation before
reading or rebasing string pointers.

Regression: `tests/automated/check-uil-db-string-bounds.py`

## UIL debug listing allocation failure

The debug listing formatter first measures a variadic message and then allocates
storage for its complete result.  If that allocation fails, `lst_debug_output`
now abandons that message and returns through the normal `va_end` path.  The
compiler therefore keeps its existing listing state and can report the source
of the failure instead of passing a null buffer to `vsnprintf`.  The measured
length is also checked before narrowing it to Xt's `Cardinal` allocation size.
The second formatting pass is checked as well; a conversion failure or length
mismatch frees the temporary buffer and suppresses the incomplete listing line.

The source regression in `tests/automated/check-uil-debug-allocation.py` keeps
the null-allocation guard next to the formatter call.  The existing long-output
listing regression continues to exercise the normal formatting path.

## UIL debug-fragment flushing

UIL debug output may be assembled from several calls and the final call does
not always include a newline.  The listing writer keeps those fragments in a
bounded 132-byte buffer so long messages can be emitted in chunks.  Listing
cleanup now emits the remaining fragment and resets the buffer when a new
listing starts; debug text is therefore not silently lost or carried into a
later listing. The fragment writer also flushes immediately when a previous
call filled the bounded buffer exactly, preventing a zero-capacity retry from
looping forever on the next fragment.

The focused source regression is
`tests/automated/check-uil-debug-flush.py`.

## UIL default output filename

When no `-o` option is supplied, UIL uses the conventional `a.uid` output
filename. That fixed default is now held in internal static storage rather than
being heap allocated and copied during command decoding. Normal command-line
behavior is unchanged, while allocation pressure can no longer turn the
default-output path into a null-pointer copy.

The focused regression is
`tests/automated/check-uil-default-output-name.py`.

## UIL diagnostic and numeric widths

The diagnostic text helpers accept the historical signed `int` type while
symbol-table type codes are unsigned. Their call sites now make the API
boundary explicit. Long integer values converted to floating-point expression
results likewise use an explicit conversion, documenting the intentional
precision boundary.

Regression: `tests/automated/check-uil-diagnostic-widths.py`

## UIL diagnostic text

UIL diagnostics no longer silently truncate formatted messages, source
locations, listing entries, or resource-manager context at the former
132-character temporary-buffer limit.  The compiler measures and allocates
each message safely, then retains the existing diagnostic callback and listing
contracts.

Source-column carets are clamped to the retrieved source line, so an invalid
column cannot overrun the diagnostic pointer buffer.

## UIL enumeration-code width

UIL keyword enumeration codes are held in a compact 16-bit value-entry field.
The semantic assignment now makes that representation boundary explicit while
retaining the existing range assertion and value lookup.

Regression: `tests/automated/check-uil-enum-code-width.py`

## UIL enum-width boundaries

UIL parser enum values are wider in local temporaries than the historical
symbol-table fields and diagnostic APIs. The conversions are now explicit at
those boundaries, preserving the existing ABI-sized fields and diagnostic
arguments while making the intended representation visible to strict builds.

Regression: `tests/automated/check-uil-enum-widths.py`

## UIL expression-frame widths

Parser expression operators and flags are held in compact frame fields. The
semantic-action code now narrows the known operator and flag values explicitly
when constructing binary and unary expression frames, preserving the legacy
representation without implicit signedness or width conversions.

Regression: `tests/automated/check-uil-expression-frame-widths.py`

## UIL expression type widths

Expression type codes are stored in compact character fields, and forward
reference flags are stored in an unsigned mask. The evaluator now makes those
legacy-width boundaries explicit when receiving integer helper results and
when clearing a flag, preserving the existing bounded type-code values without
signedness drift.

Regression: `tests/automated/check-uil-expression-type-widths.py`

## UIL external-definition dump pointer widths

External-definition diagnostics now print the entry, next-object, and name
pointers with `%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-external-dump-pointers.py`

## UIL source filename capacity

UIL symbol records preserve both the source spelling and the expanded path.
The source spelling fields were 255 bytes even though callers can provide a
255-byte name, leaving no room for the terminating NUL during the existing
copies.

The root and include `file_name` fields now use 256 bytes, matching the
expanded-name capacity and preserving maximum-length input safely. The
regression `tests/automated/check-uil-file-name-capacity.py` keeps both fields
aligned.

## UIL file-summary bounds

The listing file summary prints every expanded source path into a 132-byte
line buffer.  Formatting the path directly with `snprintf` was memory-safe but
still produced a compiler truncation warning and left the available capacity
implicit.

The summary now measures the prefix, computes the remaining path capacity, and
copies only that many bytes before adding the terminator.  The existing
one-line summary format remains unchanged for normal paths, while unusually
long paths are visibly truncated at the line boundary without a diagnostic or
buffer overrun.

`tests/automated/check-uil-file-summary-bounds.py` protects this contract.

## UIL listing fill helper

The private `_fill` helper now uses a uniquely named `size_t` counter and a
zero-based decrement loop. This avoids colliding with callers' local `i`
variables and keeps fixed-buffer fill lengths unsigned without changing the
generated listing layout.

Regression: `tests/automated/check-uil-fill-macro.py`

## UIL forward-reference dump pointer widths

Forward-reference diagnostics now print the entry, links, update location,
parent, and name pointers with `%p` and `void *`, preserving complete
addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-forward-ref-dump-pointers.py`

## UIL parser frame-flag width

Semantic value entries keep object flags in a 32-bit field, while parser
frames expose a 16-bit flag field. The transfers now make that legacy width
boundary explicit without changing the existing flag values.

Regression: `tests/automated/check-uil-frame-flag-width.py`

## UIL expanded filename capacity

UIL source control blocks accept paths up to 255 bytes plus a terminating NUL.
The symbol-table root and include records previously declared their copied
`full_file_name` fields as 255 bytes, so the maximum accepted path could
overflow those fields when copied with the existing API.

Both internal fields now have 256 bytes, matching the source control buffer.
Existing filenames remain valid and the symbol-table API shape is unchanged;
the regression `tests/automated/check-uil-full-name-capacity.py` keeps the
capacities aligned.

## UIL gadget-count bounds

Control-list records store their gadget count in an unsigned 16-bit field.
The semantic validator now rejects a negative or over-wide accumulated count
before narrowing it, so an unusually large control list cannot wrap and leave
the resource record describing the wrong number of gadgets.

Regression: `tests/automated/check-uil-gadget-count-bounds.py`

## UIL icon payload sizing

UIL icon payload sizing previously shifted width and multiplied by height in
`int` arithmetic. Large or malformed dimensions could wrap the computed byte
count before the resource context was resized.

Icon row and payload sizes now use `size_t`, then pass through the checked
resource-size helper. Color-table descriptor additions use the same guard.
Valid icon layouts are unchanged; unrepresentable payloads stop emission via
the existing internal error path. The regression
`tests/automated/check-uil-image-size-bounds.py` covers the checked calculation.

## UIL include-file dump pointer width

Include-file symbol diagnostics now print their entry address with `%p` and
`void *`, preserving full pointer values on 64-bit hosts.

Regression: `tests/automated/check-uil-include-dump-pointer.py`

## UIL include filename metadata bounds

Include symbol records retain a fixed 256-byte filename field for compatibility.
The parser now bounds and terminates the stored include name before copying it;
the separate temporary buffer still carries the complete name used to open the
file.

Regression: `tests/automated/check-uil-include-filename-bounds.py`

## UIL numeric-to-integer conversion range

Expression evaluation stores integer results in a C `int`, while parsed UIL
integer values use `long` and floating values use `double` or `float`. The
conversion now rejects values outside the destination range and NaN instead
of relying on an overflowing or implementation-defined cast.

Regression: `tests/automated/check-uil-integer-conversion-range.py`

## UIL integer-to-keysym range

Private integer values coerced to a keysym are serialized as one character.
The validator now accepts only values in the unsigned-byte range before
allocating and copying that character; out-of-range values remain invalid
instead of silently truncating through a `char` conversion.

Regression: `tests/automated/check-uil-keysym-range.py`

## UIL key-table scope

Argument validation uses one key-table pointer for both the support/duplicate
checks and the later value-type check. The redundant inner declaration was
removed so a future edit cannot accidentally update one lookup while using a
shadowed binding for the other.

Regression: `tests/automated/check-uil-keytable-scope.py`

## UIL keyword-length validation

Keyword lookup receives an unsigned source length while the generated maximum
keyword length remains an `int` for ABI compatibility. Lookup now rejects a
negative configuration and compares the values only after an explicit width
conversion, avoiding signedness-dependent acceptance of oversized names.

Regression: `tests/automated/check-uil-keyword-length.py`

## UIL machine-list formatting bounds

The UIL machine-code listing uses fixed-size temporary lines while rendering
resource names, class names, tags, and decoded values.  Those values can be
longer than the line, and the old `sprintf`/`strcat` calls could overwrite
adjacent compiler state.

All machine-list formatting now goes through a size-aware `snprintf` helper;
the few incremental additions use a capacity-aware append helper.  Normal
lines retain their existing text, while oversized descriptions are truncated
at the listing line boundary without corrupting the compiler.

The machine-code writer has a separate fixed line buffer.  Its text field
starts at the one-based `TEXT_COL`, so its terminating NUL is placed at
`TEXT_COL - 1 + text_len`.  This keeps a maximum-width resource description
inside the final valid byte rather than writing one byte past the buffer.

`tests/automated/check-uil-machine-list-bounds.py` protects the formatting
contract, and `tests/automated/check-uil-machine-text-terminator.py` protects
the machine-code field terminator.

## UIL machine-listing string widths

Machine-listing value and module records now reject index strings that cannot
fit the compact source-record length, and make integer stride and offset-stack
copy widths explicit before passing them to legacy APIs.

Regression: `tests/automated/check-uil-machine-string-widths.py`

## UIL machine-code listing bounds

Machine-code listing records store their display-text length in a 16-bit
field, while the source text is measured with `strlen()`. The listing path now
clamps the `size_t` result to the fixed output buffer before narrowing it,
preventing a long embedded text field from wrapping and bypassing the output
limit. Machine-code vector allocation sizing is explicit as well.

Regression: `tests/automated/check-uil-machine-text-bounds.py`

## UIL machine-listing unload bounds

Machine-listing offset records use 16-bit lengths and offsets. The unload path
now rejects oversized embedded strings, descending offsets, and record spans
that cannot fit the compact length field before passing them to source-record
construction.

Regression: `tests/automated/check-uil-machine-unload-bounds.py`

## UIL diagnostic message bounds

UIL diagnostic text is copied into a fixed 132-byte listing line.  The
abbreviation and message text can both come from localized or source-provided
strings, so the old `sprintf` call could overrun the listing buffer.

The message line now uses bounded `snprintf`.  Normal diagnostics retain their
format, while unusually long text is truncated at the listing boundary without
corrupting compiler state.

`tests/automated/check-uil-message-bounds.py` protects this contract.

## UIL module-entry dump pointer widths

Module-entry diagnostics now print the entry, name, and version pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-module-dump-pointers.py`

## UIL name-entry dump pointer widths

Name-entry diagnostics now print the entry, next-name, and object pointers
with `%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-name-dump-pointers.py`

## UIL name-hash width

On 64-bit-long platforms, the name hash combines machine words with masks up
to all 64 bits. The mask and accumulator storage now use `unsigned long`, so
the all-ones mask is represented without a signed overflow conversion while
preserving the hash algorithm and its public behavior.

Regression: `tests/automated/check-uil-name-hash-width.py`

## UIL name-length bounds

Name entries use a legacy unsigned-short allocation-size API and a compact
character subtype field. Name lookup and insertion now reject null, negative,
or over-wide lengths before hashing or allocation, and the validated lengths
are converted explicitly for copying and record storage.

Regression: `tests/automated/check-uil-name-length-bounds.py`

## UIL object-header dump pointers

The common symbol-object dumper reports names, references, and list links.
Those diagnostics now use `%p` and `void *`, avoiding 32-bit truncation when
debugging symbol tables on 64-bit hosts.

Regression: `tests/automated/check-uil-object-dump-pointers.py`

## UIL object enum-width boundaries

Object semantic values and definition flags use compact historical symbol
fields, while parser tables and diagnostic APIs use wider integer types. The
object semantic actions now make those allocation, enum, diagnostic, and flag
conversions explicit without changing their stored values.

Regression: `tests/automated/check-uil-object-enum-widths.py`

## UIL object-variant dump pointer width

Default-object-variant diagnostics now print the entry and next-link pointers
with `%p` and `void *`, preserving complete addresses on 64-bit systems.

Regression: `tests/automated/check-uil-object-variant-dump-pointer.py`

## UIL output path storage

The UIL compiler opens the requested UID output file and reports that same
caller-owned path in diagnostics and status callbacks.  It no longer routes
the path through a fixed 256-byte local result buffer.  Long, valid output
paths therefore retain their complete spelling without risking a local-buffer
overwrite or a truncated status filename.

The low-level MRMs resolved-name output is optional for this invocation, so
the compiler passes `NULL` and consistently uses the command's resource-file
path for both diagnostics and `Uil_current_file`.

`tests/automated/check-uil-output-path-buffer.py` verifies that the emitter
does not reintroduce the fixed result buffer.

## UIL parent-list dump pointer widths

Parent-list diagnostics now print the entry, parent, and next-link pointers
with `%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-parent-dump-pointers.py`

## uil-path-check
Parameterize the UIL path regression
====================================

`check-uil-paths.py` now performs a clean static-only skip when a generic
no-argument sweep does not provide a UIL binary and results directory.  With
both absolute paths supplied, it retains the existing strict runtime test for
overlong include paths.

## UIL WMD path formatting

The UIL database search path expands several `%s` components from environment
variables and installation directories.  The old allocation counted the
template bytes and substituted strings but omitted the terminating byte; the
subsequent `sprintf` could therefore write one byte past the allocation, with
larger path arithmetic also vulnerable to overflow.

`init_wmd_path` now measures the expanded format with `snprintf`, allocates the
exact result plus its terminator, and performs the bounded write.  Normal WMD
and application-resource search order is unchanged, including the absolute
database path and environment overrides.

`tests/automated/check-uil-path-formatting.py` protects the sizing and bounded
formatting contract.

## UIL procedure-definition dump pointer widths

Procedure-definition diagnostics now print the entry and name pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-proc-def-dump-pointers.py`

## UIL procedure-reference dump pointer widths

Procedure-reference diagnostics now print the entry, procedure-definition,
and argument-value pointers with `%p` and `void *`, preserving complete
addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-proc-ref-dump-pointers.py`

## UIL procedure widget-type limit

Procedure symbol records retain the widget-type sentinel in an 8-bit field.
Procedure creation now rejects a database object limit that cannot represent
`uil_max_object + 1`, instead of silently wrapping the metadata value.

Regression: `tests/automated/check-uil-procedure-widget-limit.py`

## UIL real literals

UIL real-literal conversion now requires `strtod` to consume the complete
literal and produce a finite value without range errors. Overflow, NaN,
Infinity, malformed text, and trailing characters produce the existing real
literal diagnostic and are stored as `0.0` rather than propagating an unsafe
semantic value.

## UIL root-entry dump pointer widths

Root-entry diagnostics now print the root, module, and section pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-root-dump-pointers.py`

## UIL root filename metadata bounds

The UIL symbol-table root has fixed 256-byte filename fields for ABI
compatibility. Root creation now copies command-line and expanded filenames
with a terminating bound, preventing an unusually long path from overwriting
adjacent symbol data. Only the stored metadata is truncated; source opening
continues to use the original path.

Regression: `tests/automated/check-uil-root-filename-bounds.py`

## UIL section-dump pointer widths

Section diagnostics now print the section, previous/next links, and entry-list
pointer with `%p` and `void *`, preserving full addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-section-dump-pointers.py`

## UIL source-path length bounds

Source-file lookup composes the main-file directory, include directories, and
the requested filename into a fixed path buffer. The composition checks each
length with subtraction-based bounds before adding lengths, so an unusually
large include-directory string cannot wrap the arithmetic and bypass the
buffer check. Existing absolute-path and include-search ordering is unchanged.

The contract is covered by
`tests/automated/check-uil-source-path-bounds.py`.

## UIL source-record allocation bounds

UIL diagnostic and machine-code records use flexible trailing text/data. Their
lengths now stay in `size_t`, reject negative machine-code lengths, and check
the complete struct-plus-payload size before calling the allocator. Allocation
failure also returns without publishing a partially initialized record.

The source contract is checked by:

```sh
python3 tests/automated/check-uil-source-record-bounds.py
```

## UIL source-record width validation

Source diagnostics and machine-code records retain compact offsets, lengths,
and source positions for compatibility. The append paths now reject values
outside those field widths and reject allocations that cannot fit Xt's
`Cardinal` size before narrowing or copying.
File-position keys are also range-checked before crossing the legacy unsigned
key/`fseek` signed-offset boundary, and `ftell` failures no longer become huge
unsigned positions.

Regression: `tests/automated/check-uil-source-record-widths.py`

## UIL string-concatenation bounds

UIL simple-string values store their length in a 16-bit symbol-entry field.
Concatenation now rejects a pair whose combined length cannot fit that field
before allocating or copying, preventing signed arithmetic and narrowing from
turning a large valid pair into a wrapped allocation or length.

Regression: `tests/automated/check-uil-string-concat-bounds.py`

## UIL table-count bounds

UIL value entries store table counts in an 8-bit field. The semantic table
builder now rejects a count above `UCHAR_MAX` before narrowing, preventing a
large table from silently wrapping its recorded count.

Regression: `tests/automated/check-uil-table-count-bounds.py`

## UIL listing title bounds

The listing title stores the module name and optional version in a fixed
132-byte buffer.  Module identifiers are not limited to the buffer's size, so
the old `sprintf` calls could overwrite adjacent compiler state while opening
or updating a listing.

Both title updates now use the buffer size with `snprintf`.  Long names and
versions are truncated in the listing while parsing and compilation continue
with the original symbol values and diagnostics.

`tests/automated/check-uil-title-bounds.py` protects the bounded formatting
contract.

## UIL listing title-one bounds

The first listing title combines the compiler name, version, and timestamp in
a fixed 132-byte buffer.  These values are normally short, but build variants
or localized time strings can make the old `sprintf` write past the buffer.

The title now uses bounded `snprintf`; normal title output is unchanged and
oversized metadata is truncated at the listing boundary.

`tests/automated/check-uil-title1-bounds.py` protects this contract.

## UIL unknown-entry dump pointer width

The fallback symbol-entry diagnostic now prints its entry pointer with `%p`
and `void *`, while retaining the legacy-width byte value formatting.

Regression: `tests/automated/check-uil-unknown-dump-pointer.py`

## UIL validation-table bounds

The semantic validator allocates one entry for every known argument and
callback reason. These counts originate in the parsed UIL symbol tables and
must not be used directly in a multiplication or narrowed to Xt's `Cardinal`
allocation size. The validator now rejects negative or overflowing counts,
checks the allocation size against `UINT_MAX`, and reports allocation failure
before walking the parse tree.

Regression: `tests/automated/check-uil-validation-table-bounds.py`

## UIL value-entry dump pointer widths

Value-entry diagnostics now print the entry and name pointers with `%p` and
`void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-value-dump-pointers.py`

## UIL value-frame field narrowing

UIL parser frames retain several compact compatibility fields (`b_type`,
`b_flags`, and `b_charset`). The value-construction paths now make each
narrowing conversion explicit at the field boundary. This documents the
existing ABI-sized storage without changing the values produced for valid UIL
input. Compound-string component and direction values likewise narrow only at
the Motif API boundary, after UIL enum validation. This keeps strict compiler
diagnostics useful for new conversions. Compound-string lengths are checked
before entering the ABI-sized `w_length` field so oversized values cannot
silently wrap. Name-binding masks are widened explicitly before combining with
the unsigned object flags field.

Font-table growth is likewise bounded by the 8-bit table-count field before a
new entry is linked.

Regression: `tests/automated/check-uil-value-frame-narrowing.py`

## UIL value-payload dump pointer widths

Value-payload diagnostics now print table links, user-defined charset values,
icon tables, and color entries with `%p` and `void *`, preserving complete
addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-value-payload-dump-pointers.py`

## UIL value-size arithmetic

UIL resource emission accumulates string-table, font-table, integer-table, and
translation-table payload sizes in an `int` before resizing the resource
context. Large or malformed tables could wrap that counter and cause an
undersized allocation followed by out-of-bounds writes.

`UilP2Out.c` now uses a checked size-add helper for those accumulations. An
unrepresentable resource reports the existing internal error and stops
emission, while valid resources retain the same layout and offsets. The source
regression `tests/automated/check-uil-value-size-bounds.py` covers the helper
and its table call sites.

## UIL resource-vector allocation bounds

UIL initializes several unsigned-short resource tables from limits read from
the database. Their byte counts now pass through one checked helper before
conversion to Xt's `Cardinal` allocation width. Negative or unrepresentable
limits stop through Xt's normal fatal-error path instead of wrapping an
allocation size.

Regression: `tests/automated/check-uil-vector-allocation-bounds.py`

## UIL widget-dump pointer widths

The symbol-table widget dumper is also used when diagnosing compiler input on
64-bit systems. Its pointer fields now use `%p` and `void *`, so debug output
does not truncate addresses to 32 bits.

Regression: `tests/automated/check-uil-widget-dump-pointers.py`

## Unit-converter input validation

Horizontal and vertical position/dimension converters now validate their Xt
argument, widget, source string, and destination before resolving screen units.
Valid unit conversion and warning behavior remain unchanged.

The focused regression check is
`tests/automated/check-unit-converter-inputs.py`.

## Unit-string parser inputs

The internal unit parser now validates its specification and output pointers
before scanning the string.  A malformed resource conversion returns
`XmPARSE_ERROR` instead of dereferencing a null pointer.

## Unit-string byte classification

Motif unit parsing now converts each input byte to `unsigned char` before
calling `isascii` and `isspace`. This keeps whitespace handling defined for
high-bit bytes in localized resource strings without changing valid numeric
or unit syntax.

## Usable text concatenation bounds

Converting a compound string to locale or string text joins each valid segment
into one buffer.  The running byte count now uses `size_t` and checks the next
segment before adding it.  If the total cannot be represented, the conversion
frees the current segment and accumulated buffer, releases its context, and
returns `XNoMemory` instead of wrapping an `int` and under-allocating.

## UTF-8 resource conversion length accounting

`XmString` resource conversion may expand a source string when converting it
to UTF-8.  The output buffer is appended using the converted byte count, and
the running length must advance by that same count.  Advancing by the source
length leaves the output cursor inside the converted data, so a following
segment can overwrite bytes or produce a truncated text property.

The conversion path now records the converted length once, checks the
`unsigned int` and aggregate-length bounds, and uses that value for both the
append and the cursor update.

## UTF-8 to UCS-2 allocation

`_XmUtf8ToUcs2` validates the requested buffer size against both host
`size_t` and Xt's `Cardinal` width before allocating the temporary `XChar2b`
array. It also handles an allocation failure by
returning `NULL` and resetting the output length, rather than dereferencing a
null buffer while converting input bytes.

Successful conversion and invalid-byte replacement remain unchanged.

## UTM destination cleanup

If the destination callback arrives without an
`XmDestinationCallbackStruct`, the UTM bridge cannot start the transfer. It
now dequeues and frees the pending package in that case. This prevents a later
request from consuming stale parameters and keeps the queue from leaking.

`tests/automated/check-utm-destination-cleanup.py` protects this lifecycle
behavior.

## UTM send allocation guards

The WSM UTM bridge now checks both the transfer-package allocation and the
queue-node allocation before dereferencing either object. A failed allocation
logs the existing UTM error style and drops the unsent request cleanly while
retaining the public `void UTMSendMessage` API.

`tests/automated/check-utm-send-allocations.py` protects the failure paths.

## UTM sink failure cleanup

When `XmeNamedSink` rejects a request, its package is now removed by identity
from the pending UTM queue and released. Removing the specific node preserves
other queued requests and prevents a later destination callback from consuming
stale parameters.

`tests/automated/check-utm-sink-failure.py` protects this queue lifecycle.

## Varargs SimpleMenu parent guards

The varargs SimpleMenu constructors now return `NULL` for a null parent before
application-context lookup and argument processing:
`XmVaCreateSimpleMenuBar`, `XmVaCreateSimplePulldownMenu`,
`XmVaCreateSimplePopupMenu`, and `XmVaCreateSimpleOptionMenu`. Valid parent
widgets retain the existing varargs behavior.

The focused regression is
`tests/automated/check-va-simple-parent-inputs.py`.

## Vendor-shell WM property status handling

The vendor-shell `_MWM_HINTS` reader now checks the result of
`XGetWindowProperty` before examining the returned type, format, and item
count.  X errors therefore use the configured vendor fallback instead of
depending on uninitialized property metadata.

Regression: `tests/automated/check-vendor-mwm-property-status.py`

## VendorS destroy-grab list growth

Vendor shell extensions keep a small registry of shells that need destroy
callbacks.  The old growth path incremented its 16-bit capacity and assigned
`XtRealloc`'s result without checking it, so a failed allocation could lose
the registry and write through a null pointer.

Growth now checks both the counter and byte arithmetic, then commits the new
pointer and capacity only after successful allocation.

## VendorShell composite-extension allocation

VendorShell class initialization now checks the inherited composite-extension
lookups and the `XtNew` allocation before copying the superclass record.  A
missing or unallocatable extension causes class initialization to return
without dereferencing null state; successful initialization retains the
existing inheritance and trait behavior.

The focused source-level regression is
`tests/automated/check-vendorshell-composite-extension-allocation.py`.

## VendorShell count bounds

VendorShell `SetValues` and `GetValuesHook` now iterate Xt argument counts
with `Cardinal` counters, and `Resize` uses a `Cardinal` child index for the
composite child array. This preserves resource and child processing while
avoiding signed-counter wraparound at large valid counts.

## VendorShell extension-data allocation

The VendorShell extension-object post-hook now allocates and initializes its
private `XmWidgetExtData` record before publishing it through
`_XmPushWidgetExtData`.  Both the extension record and its request-widget copy
are checked; failed allocation releases the partial record and returns without
exposing a null or incomplete extension to later hooks.

Successful extension setup and the existing VendorShell resource conversion
path are unchanged.

## VendorShell extension-object allocation

VendorShell initialization creates a secondary extension object, a request
copy, and an extension-data record before importing resources. The initializer
now checks each allocation and releases any partial objects before returning,
so allocation pressure cannot cause a null dereference or leaked request
storage.

## VendorShell MWM menu rollback

VendorShell now copies a changed `mwm_menu` resource before releasing the
previous owned copy.  When allocation fails during `SetValues`, the previous
menu remains installed; initialization safely leaves the menu unset if its
copy cannot be made.  This preserves MWM menu behavior without exposing a
dangling resource pointer.

The focused regression is
`tests/automated/check-vendorshell-mwm-menu-rollback.py`.

## VendorShell resize bounds

VendorShell resize now returns safely when its shell extension data is absent
or incomplete.  It also computes the child height with signed operands and
clamps it to zero when input-method height exceeds the shell height, avoiding a
wrapped oversized child configuration.

The focused regression is
`tests/automated/check-vendorshell-resize-bounds.py`.

## VendorShell resource-list allocation

VendorShell class initialization merges ShellExt and VendorShellExt resource
tables into one list. The merged count and Xt byte size are now checked before
allocation, and a failed allocation releases the process lock and leaves the
class tables unchanged.

## VendorShell set-values extension data

VendorShell set-values processing clones extension metadata when an existing
shell extension record is present. The clone allocation is now checked before
copying or pushing the record, so failure leaves the existing extension data
unchanged.

## VendorShell SetValues allocation

The VendorShell `SetValues` prehook now allocates both secondary-object
snapshots before publishing its extension-data record.  Failed `_XmExtObjAlloc`
calls release any partial snapshots and the record, avoiding null `memcpy`
targets and leaving the hook unregistered.  Successful updates retain the
existing old/request snapshot and resource-conversion behavior.

## VendorShell tooltip-trait allocation

The VendorShell initialization prehook creates a tooltip configuration trait
when the class does not provide one. The trait allocation is now checked before
its default fields are initialized; failure returns from the prehook without a
null dereference.

## VendorShell trait guards

VendorShell’s direction and unit-type trait callbacks now handle null widgets
and incomplete shell-extension records before dereferencing extension state.
The established fallbacks remain unchanged: `XmRIGHT_TO_LEFT` for unavailable
direction state and `XmPIXELS` for unavailable unit state.

The focused source-contract check is
`python3 tests/automated/check-vendorshell-trait-guards.py`.

## VendorShell warning buffers

`MotifWarningHandler` builds a combined header/message string and an indented
copy for `XtWarning`. Both buffers now validate their byte sizes against the Xt
allocation limit and check allocation results before formatting or copying.
Oversized or allocation-limited warnings return without dereferencing null
storage.

## Virtual-key result allocation

`XmeVirtualToActualKeysyms` returns a newly allocated array of concrete
keysyms for a virtual key. It now rejects a null output pointer and checks the
result allocation before filling it, returning zero matches on failure.

Successful mappings preserve their ordering and count.

## Virtual-key binding-file allocation

`_XmVirtKeysLoadFileBindings` builds a NUL-terminated binding string from
possibly long lines. Growth and final trimming now use temporary pointers and
check allocation results; invalid input pointers are rejected before file
access.

Failure closes the file and returns `False` without publishing a null or
partially resized buffer. Successful binding text remains unchanged.

## Virtual-key binding file growth

`_XmVirtKeysLoadFileBindings` reads a binding file in 255-byte chunks and
accumulates the result for the display. The old byte counter used `int` and
added only one fixed growth block, which could wrap or leave the destination too
small for a large valid file.

The loader now uses `size_t` arithmetic, grows repeatedly until the complete
line fits, limits allocations to the Xt allocator's representable size, and
returns failure cleanly when the input cannot be represented. Existing comment
handling and binding text are unchanged.

`tests/automated/check-virtkeys-binding-growth.py` guards the growth contract.

## Virtual-key binding database input

Virtual-key initialization normally obtains bindings from the Motif properties
or its built-in fallback files. If every source is unavailable, allocation of
the fallback string can still fail. In that case Motif now skips database
population safely instead of passing a null string/database into the resource
parser and binding walker. Existing valid bindings are unchanged.

## Atomic virtual-key file loading

When a user virtual-key binding file cannot be read completely, the loader
now releases any partially assembled string and clears the output pointer
before returning failure.  This ensures `LoadVendorBindings` proceeds to its
normal alias or built-in fallback instead of publishing truncated bindings.
The same cleanup applies if trimming the successfully read buffer cannot be
allocated.

Regression: `tests/automated/check-virtkeys-file-failure-cleanup.py`

## Virtual-key initialization inputs

`_XmVirtKeysInitialize` now validates that its handle is a non-null XmDisplay
before casting it or requesting its X display. Invalid internal calls return
without dereferencing the handle; normal display initialization is unchanged.

The focused regression check is `tests/automated/check-virtkeys-inputs.py`.

## Complete virtual-key binding properties

Virtual-key initialization now checks the return status from
`XGetWindowProperty` and rejects `_MOTIF_BINDINGS` or
`_MOTIF_DEFAULT_BINDINGS` data when the server reports remaining bytes.  A
truncated property is discarded and the normal fallback binding selection is
used, avoiding silent parsing of incomplete input.

Regression: `tests/automated/check-virtkeys-property-complete.py`

## Virtual-binding converter allocation

The `XmRVirtualBinding` string converter allocates the output
`XmKeyBindingRec` array when the Xt conversion caller does not provide one.
The allocation is now checked before the array is populated.  On failure the
temporary keysyms, modifiers, and event-type buffers are released, the output
size is reset to zero, and conversion returns `False` instead of dereferencing
a null result.  Successful conversion and the caller-provided output-buffer
contract are unchanged.

## Virtual-key binding-file assembly

The virtual-key binding loader measures each line and grows its destination
before copying. It now writes each line at the tracked byte offset and updates
the terminator directly, avoiding repeated `strcat` scans for long binding
files while retaining the original concatenated text.

The source contract is checked by:

```sh
python3 tests/automated/check-virtual-key-binding-copy.py
```

## Virtual-key vendor-string bounds

Virtual-key alias loading now checks the X server vendor-string length before
converting it to Xt's `Cardinal` allocation size. It reserves explicit space
for the release suffix and uses `snprintf`, rejecting a formatting failure
without leaking the temporary string.

The source contract is checked by:

```sh
python3 tests/automated/check-virtual-key-vendor-bounds.py
```

## Visibility region allocation handling

`XmGetVisibility` now checks all sibling-obscuration region allocations before
using them. If Xlib cannot provide a required region, partially allocated
regions and the child list are released and the function returns the existing
conservative fully-obscured result.

The focused regression check is
`tests/automated/check-visibility-region-allocation.py`.

## Shared varargs widget-creation parent guard

`XmeVLCreateWidget` now returns `NULL` when its parent widget is null, before
application-context lookup or varargs conversion. This protects all public
varargs constructors that use the shared helper while preserving valid widget
creation.

The focused regression is
`tests/automated/check-vl-create-parent-input.py`.

## Warning output safety

Motif warning handlers receive text from the error database and substitution
parameters supplied by widgets. The handler now measures the formatted header
and message before allocating them, then allocates enough space for indentation
after embedded newlines. Long diagnostics remain readable without overwriting
the neighboring warning state.

`tests/automated/check-warning-buffer.py` checks the source contract, and the
native ARM64 library build verifies the implementation compiles cleanly.

## Warning parameter handling

The Motif warning formatter receives Xt-style parameter metadata.  It now
requires both a positive parameter count and a non-NULL parameter array before
copying arguments for `snprintf`.  A malformed warning call with a missing
array therefore falls back to the unparameterized message instead of
dereferencing NULL; normal warning formatting is unchanged.

`tests/automated/check-warning-params-null.py` protects both formatting passes.

## Window-configuration occupancy sentinel

The icon move occupancy check now skips `NO_ICON_PLACE` before indexing the
placement list.  Invalid converted coordinates therefore take the existing
no-placement path without a negative array access.

`tests/automated/check-winconf-occupancy-sentinel.py` protects the contract.

## Window-configuration placement sentinel guard

The icon move path in `WmWinConf.c` now converts negative, oversized, or
storage-free placement results to `NO_ICON_PLACE` before checking occupancy.
This prevents invalid coordinates from becoming a placement-list index.

`tests/automated/check-winconf-place-sentinel.py` protects the contract.

## Window-configuration widget move guard

The icon-box move path now validates placement storage, the current index, and
the target widget before calling `XtMoveWidget`.  Valid icon moves retain their
existing behavior.

`tests/automated/check-winconf-widget-move-guard.py` protects the contract.

## Window-state placement cleanup guards

State cleanup now validates workspace records, placement storage, and stored
icon indices before clearing entries in WSM and non-WSM paths.  Valid cleanup
continues to remove the client from its placement slot.

`tests/automated/check-winstate-removal-place-guards.py` protects the contract.

## Withdrawal workspace guards

WSM client withdrawal can race workspace membership teardown.  The withdrawal
paths now verify that the client still has screen and workspace-list state,
and only pass a workspace to `TakeClientOutOfWorkspace` when the lookup
succeeds.  This keeps normal membership removal unchanged while avoiding a
null workspace dereference during partial teardown.

## WmGraphics size-limit header

`clients/mwm/WmGraphics.c` bounds rectangle-list allocation with `INT_MAX` and
`UINT_MAX`.  The source now includes `<limits.h>` directly so Mwm compiles
without relying on an incidental header to provide those constants.

The allocation behavior and public interfaces are unchanged.  The automated
check is `tests/automated/check-wm-graphics-size-limit-header.py`.

## WmIconBox creation failure

The icon-box insertion path calls `MWarning(char *, char *)`, but its
allocation-failure branch supplied only one argument and returned `NULL` from
a `Boolean` function.  The branch now passes the translated diagnostic as a
`%s` message and returns `False`, preserving the failure behavior while
compiling correctly with current C diagnostics.

The regression check is
`tests/automated/check-wm-iconbox-failure-return.py`.

## WmMenu sentinel values

Mwm menu construction used pointer-valued `NULL` casts for KeySym, KeyCode and
CARD32 sentinel arguments.  Those casts trigger 64-bit pointer-to-integer
diagnostics even though the intended value is zero.

The calls now use explicit zero constants of the destination types.  Menu
construction behavior and public interfaces are unchanged.  The regression
check is `tests/automated/check-wm-menu-sentinels.py`.

## Mwm window-management request termination

Mwm reads `_DT_WM_REQUEST` as a sequence of NUL-separated byte strings. The
request extractor now treats an unterminated final property as a valid final
request, copies only the bytes returned by X, and appends its own terminator.
It no longer indexes one byte beyond the property or relies on `strncpy` to
read a terminator that may not exist.

`tests/automated/check-wm-request-termination.py` guards this behavior.

## WmResParse table sentinels

The Mwm resource parser uses sentinel entries in modifier and event tables.
Those entries previously cast the pointer-valued `NULL` macro into integer and
function-pointer fields, which triggers 64-bit compiler diagnostics and can
obscure real parser warnings.

The sentinels now use integer zero and a null function pointer explicitly.
Table layout, parsing behavior and public interfaces are unchanged.  The
regression check is `tests/automated/check-wm-resparse-sentinels.py`.

## MWM screen-name allocation bounds

`InitWmScreen` now keeps the screen-name length in `size_t` until it has been
validated against the `Cardinal` size accepted by `XtRealloc`. An unrepresentable
length is rejected before the allocation, instead of being narrowed through an
`int` and potentially requesting the wrong buffer size.

The source contract is checked by:

```sh
python3 tests/automated/check-wm-screen-name-bounds.py
```

## WM save-hint property validation

`ProcessWmSaveHint` reads `_DT_WMSAVE_HINT`, whose payload is one 32-bit
save-hint flag word. X permits a property to have the expected type and
format while containing no items or a truncated suffix. The reader requests
and requires exactly one complete item before dereferencing the returned
buffer, and continues to clear the saved flags when the property is absent or
malformed.

This preserves the existing behavior for valid clients while preventing a
malformed empty, oversized, or truncated property from influencing `mwm`.

## Workspace information property allocation

Mwm now checks both temporary buffers used to publish a workspace information
property.  If either allocation fails, any successful sibling allocation is
released and the property update returns without dereferencing invalid state.
The normal property encoding is unchanged.

`tests/automated/check-workspace-info-allocation.py` protects this path.

## Workspace-info serialization guards

`SetWorkspaceInfo` now rejects a missing data buffer for nonzero record counts
and checks the record-size multiplication before publishing the property.
Zero-record updates remain valid and publish an empty value.

## Mwm workspace-info title conversion

`SetWorkspaceInfoProperty` converts the workspace title before passing the
string vector to Xlib.  A missing or malformed `XmString` can make that
conversion return `NULL`; the property update now logs the failure, releases
its temporary buffers, and returns before Xlib sees a null list entry.  This
preserves the existing property format while avoiding a crash in the Xmb
conversion path.

`tests/automated/check-workspace-info-title.py` protects this failure path.
