# Fork history

The original Motif manuals and release notes describe the inherited toolkit.
The root `CHANGES` file records earlier fork work, including DPI-aware default
sizes for unlabeled toggle indicators, correct drag hotspot snap-back, explicit
X library linkage for tools and demos, and compiler prototype corrections.

## Maintenance foundation

The following changes precede the 2.4 development cohort:

| Source commit | Correction |
| --- | --- |
| `5b2ef86` | Generate Xm string definitions once under parallel make. |
| `8a03ed6` | Attach generated WML header dependencies to the object that consumes them. |
| `1f52842` | Declare terminating window-manager handlers with the Xt noreturn contract. |
| `791240f` | Enable Xft only after successful dependency probes. |
| `2b63cf1` | Correct selection timestamp item counts and release DataField encoding-probe storage. |

Selection lengths are protocol item counts: a format-32 timestamp is one
item, rather than `sizeof(Time)` items. This matters on platforms where native
`long` and the X protocol element size differ.

## Development 2.4 cohort

The next cohort introduces bounds-checked XIM preedit replacement, ICU Unicode
coordinate maps, coherent ICU/HarfBuzz text captures, native accessibility
providers and an explicit native ABI epoch. The [status page](status.md) lists
the validation limits. A source commit or a successful standalone fixture does
not establish complete widget or input-method acceptance.

## Widget and resource-compiler correctness

The focused corrections in `d73af27`, `fe44815` and `a285086` address
SpinBox/SimpleSpinBox behavior, List quick navigation, existing DPI defaults
and WML/UIL database handling. The repository's `WIDGET_REVIEW.md` retains the
full source review and API caveats; this page summarizes its scope.

- SpinBox uses exact integer/fixed-point formatting and parsing, bounded signed
  arithmetic and defined constraint offsets. It checks large increments,
  exceptional numeric input and deletion positions, preserves selected items
  during string insertion, clears emptied controls and cancels repeat timers
  when their widgets are destroyed.
- List quick navigation checks multibyte conversion results and uses independent
  conversion state, avoiding uninitialized input from malformed or truncated
  commits while preserving valid multibyte navigation.
- DPI defaults require finite, positive, completely parsed values and saturate
  dimensions before conversion. Default SpinBox arrows follow the existing
  Xft.dpi / 75 convention; explicitly supplied arrow and indicator sizes remain
  authoritative. This adds neither live per-monitor scaling nor automatic
  application-pixmap scaling.
- WML emits deterministic zero padding instead of reading beyond string storage,
  bounds debug-table iteration, accepts long output paths safely and reports
  missing output arguments and write/close failures. UIL copies owned database
  paths correctly, guards short suffixes and does not free environment storage.
  Existing WMD framing and length fields remain unchanged; the
  architecture-dependent file format is not redesigned.

The widget regression runner covers numeric limits, decimal/exponent input and
an independent arithmetic corpus, locale radix handling, indexed positions,
boundary callbacks, timer destruction, string-table mutations, List navigation
and DPI defaults. `tests/automated/check-wmd.py` covers generation, long/short
paths, error handling and UIL round trips. The List fixture interposes the XIM
lookup boundary; it does not qualify a real input-method service.

Focused GCC and ASan/UBSan checks passed in a separately built **ABI 4** cohort.
Those corrections do not themselves change public resource types, signatures or
instance layouts. The combined published **ABI 5** cohort subsequently passed
the installed native feature, widget, WMD/UIL and legacy GUI suites. Those
results qualify that exact cohort, not subsequent epoch6 source changes. Full sanitizer coverage requires instrumenting both the toolkit
library and its consumers; consumer-only instrumentation has narrower scope.


## Bounded text contexts and numeric accessibility

Native text widgets now reuse bounded whole-grapheme font resolution and
HarfBuzz shaping leases. Captures retain independent leases across eviction and
context destruction, with explicit configuration-change notification. The large
mixed-script fixture exposes remaining single-line latency rather than hiding
it. See the [text contract and measurements](unicode.md).

Xft Text, TextField and DataField column geometry now follows the documented
em-space, numeral-zero, maximum-width fallback order. The old maximum-glyph
metric could make a seven-column field request 194 pixels instead of 110. Actual
allocated/preferred geometry tests cover both proportional/monospace fonts and
two sizes. The shared typed helper also removes an unsafe integer-pointer cast.

ScrollBars now expose native numeric Value, actions, focus/orientation and
change events through accessibility. Continuous custom providers have their own
value setter contract; native integral controls validate range/step semantics.
See [accessibility](accessibility.md). New public provider/info fields and native
text records require deliberate [ABI epoch6 rebuilding](abi.md).

## String-generator metadata

The `makestrs` generator now parses paired dollar-delimited source metadata
with explicit bounds, trims trailing whitespace without discarding a valid last
character, ignores empty metadata, and releases its owned metadata after output.
The previous parser could index before an empty delimiter pair and leaked the
copied comment. Seven focused cases cover empty, single-character, unspaced,
spaced, whitespace-only, later-valid and unpaired input under AddressSanitizer
and UndefinedBehaviorSanitizer. This focused generator result does not qualify
the full toolkit sanitizer build.

WML also releases its default resource-literal string before replacing it with
an explicit literal. The full instrumented generator build exposed this
ownership leak; the corrected generation and installation complete without
disabling leak checks.

## Interrupted clipboard transfers

The independent Unicode/INCR peer now flushes before inspecting Xlib's event
queue; flushing may otherwise consume a socket event into that queue before a
socket wait. Separately, Xt's final-segment cleanup retained a freed selection
pointer which its timeout could free again. The dependency patch corrects that
ownership directly. A peer-disconnect regression waits through the actual
selection timeout, verifies that the native owner survives, and requires a new
exact-byte transfer from the same owner for both Text widgets. The
[pinned dependency build](build.md) records the selected source and patch.

## Primary-window modality across window managers

Primary-application-modal dialogs no longer depend on detecting the Motif
window manager. The toolkit's Xt grab cascade blocks the dialog's ancestor
windows while permitting independent branches. Nested or parallel primary
dialogs retain each other's blocked ancestors; an enclosing full-application
modal remains authoritative. A newly realized window or unrelated modal popup
cannot escape an existing full-modal scope simply by appending another grab.

Virtual grab insertion preserves blocked windows for later admission. Removal
compacts retained records explicitly, restores the surviving Xt suffix once,
releases associated cousin callbacks and refreshes permitted branches. Raw
widget grabs use their containing vendor shell for primary ancestry checks.
These changes do not add a window-manager dependency or change the public
`XmNdialogStyle` values.

The unattended `primary-modality` fixture exercises actual XTEST pointer and
keyboard input on an isolated X server, including independent windows, nested
and parallel dialogs, raw grabs, windows and modal popups created during a
modal, popdown and recursive destruction. The final fixture observes XmDisplay
retirement instead of reading it after the last VendorShell is destroyed.
Native installed-prefix and focused ASan/UBSan results are recorded in
[current status](status.md); broader desktop and final x86_64 gates remain open.


## Exact text changes and native menu accessibility

The optional AT-SPI transport now emits scalar-indexed TextChanged deletion and
insertion payloads from validated current live text, with changes ordered as one
replacement batch. Caret-only/unchanged observations no longer fabricate content
changes. Bounded reads, provider removal/reentrancy, invalid observations and
password-role suppression preserve explicit observation limits. Message construction
checks libdbus failures transactionally; a failed batch invalidates its baseline.

Native menu bars, panes, cascade widgets/gadgets and menu entries expose standard
roles, native actions, expansion ownership and posted MenuShell focus. Applications
can compose a single input-row tab group for fields and adjacent buttons using
existing navigation resources; the compound traversal fixture checks that contract.
No public struct layout or SONAME changes; the added role constants retain ABI6.

The strict ARM64 native build passes all 15 default features, including exact
Unicode text events, 20 targeted libdbus failure cases, menu semantics and compound
traversal. Clean builds take 116.040/120.184 seconds with two jobs, comparable to
the earlier 115.427-second profile. This is native protocol/control qualification;
sanitizer and actual screen-reader checks for these additions remain pending.


## Toolkit keyboard delivery to assistive listeners

Registered widgets now forward native key press/release events before their
translations run. Synchronous non-global AT-SPI listeners can query accessible
state and consume an event. Bounded waits fail open; nested dispatch, late replies,
disconnect and bridge destruction preserve native editing. The registry-input
wire encoding follows the actual pinned parser; its hardware/modifier types differ
from registry listener output despite the advertised signature.

The full16-fixture native ARM64 suite passes, including strict listener type and
ordering checks and consumption of the same action between two accepted runs.
Actual screen-reader input echo, IME and sanitizer checks of this new path remain
separate qualification gates. See [accessibility](accessibility.md).

## Native list identities and selection (ABI7)

List items now own non-reused identities, and native semantic projection exposes
item names, selection, cursor focus and exact-target actions. Selection methods
work for native lists and typed custom providers. Stale rows reject after
delete/replacement, repeated labels remain distinct, and optional projection
allocation failures close semantic actions until recovery. Native/private and
public provider layouts advance the suite to ABI7. The fresh installed-prefix
build and 20 native ARM64 fixtures pass, including allocation-failure recovery,
12 generic-provider reentrancy cases and eight native callback-destruction cases.
Provider snapshots validate the hierarchy and semantic epoch at every callback
boundary; native selection publication safely handles callback destruction.
Nine focused ASan/UBSan fixtures also pass, including List protocol/lifetime
and provider reentrancy; system/Xt libraries and leak checking are outside scope.
Actual-application adoption remains a separate gate. Repeated real
IBus/m17n service validation has an intermittent Hindi preedit failure under
investigation. Previous ABI6 results remain scoped to that cohort.

## Accessibility modal input admission

Accessibility actions, scalar/Selection writes and focus requests now honor the
same live Xt grab admission and Motif modal cascade as native input. Native list
items and semantic providers cannot activate a primary dialog's blocked parent;
independent window branches remain usable. Mutable semantic providers require a
live native widget ancestor instead of an implicit first-window owner. Provider
reads that reenter the toolkit are followed by fresh admission before mutation.
Xt's private input headers are isolated from Motif's distinct private callback
types through a narrow internal C boundary; no private layouts are copied.

The new regression is part of the 21-fixture installed native suite. Its 272
checks pass, including native focus callback destruction and modal changes
during provider queries/selection snapshots. The earlier library fails 87
checks, while the independent native input modality regression retains all 397
assertions. The complete normal native ARM64 suite passes. These results do not
establish complete assistive-technology, input-method or cross-platform coverage.

Ten focused native ARM64 fixtures also pass with the toolkit and consumers
instrumented by ASan/UBSan, including the new modal-admission regression.
Xt/system dependencies are uninstrumented, leak checking is disabled, and
allocator-fault interposition remains exclusive to the normal native suite.


### Complete semantic button callback data

Accessibility activation supplies the complete native callback structure for
PushButton, PushButtonGadget and ArrowButton, including click count 1 and a NULL
native event. Previously, callbacks reading beyond the common reason/event
prefix could read outside the provided stack storage. The same instrumented
fixture reproduces that read against the previous library and passes against
the rebuilt library, including callback-driven destruction and stale IDs.
This focused ASan/UBSan result does not establish unrelated application or
input-method behavior; process-global leak acceptance is not claimed.

## Empty compound-string segments

Compound-string construction, conversion, concatenation and component traversal
skip zero-byte payload copies, preserving valid empty components without passing
null text storage to byte primitives or pointer arithmetic. Comparisons and
separator-table matching also skip zero-byte comparisons after their existing
type, tag, direction and length checks. Metadata, ownership, counts and copied
terminators remain intact.

Public regressions cover empty and nonempty neighbors, tagged segments, direction
metadata, separators, legacy left-to-right construction, typed NULL/zero-length
construction, copy/traversal/unparse and counts-only/allocated separator tables.
Nonzero-length NULL input remains refused. Strict runtime qualification is
recorded separately from these source changes.
