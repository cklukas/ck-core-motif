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
