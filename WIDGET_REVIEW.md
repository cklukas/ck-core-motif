# Widget correctness, Unicode and DPI review

Review date: 2026-10-23. Scope: SpinBox/SimpleSpinBox API and lifetime,
list quick navigation and selection conversion, ComboBox convenience functions,
the existing Xft.dpi defaults, and WML/UIL database generation and loading.
Findings below have a generic toolkit rationale.
This is a focused source and regression review, not a claim that every widget,
input method, graphics configuration or historical application is qualified.

## Suggested fixes implemented in this change

| Finding | Consequence | Correction |
| --- | --- | --- |
| SimpleSpinBox reads an int increment into a short temporary | Stack overwrite and truncation: 70000 reads as 4464 | Use the resource's actual int type |
| SpinBox formats integers through float and abs(int) | Incorrect values above 2^24, INT_MIN undefined behavior, a possible one-byte heap overrun after rounding | Format the unsigned magnitude exactly and insert the locale radix |
| Position validation uses float, misses scanf EOF, leaks rejected strings and casts out-of-range/NaN values | 0.29 becomes 28 at two decimal places; empty/huge input is unsafe | Parse decimal fixed-point values exactly; release text on every path; bound exceptional values before integer conversion |
| Step, increment-grid and index arithmetic overflow or use unsigned promotion | Incorrect boundary callbacks and negative/indexed positions | Use widened signed arithmetic before range decisions |
| SimpleSpinBox decrements an unchecked deletion position | Deleting count+1 copies count strings into count-1 slots; INT_MIN decrement also overflows | Normalize one-based positions before subtracting; retain historical last-item fallback |
| Inserting at the selected item fails to move its index | Selection unexpectedly changes to the inserted string | Preserve the previously selected item, including multibyte labels |
| Removing the last string retains old text/storage | An empty control displays an item that no longer exists | Release the old table and clear the textual child |
| Spin repeat timers survive destruction or can be duplicated | Timer callback dereferences a destroyed widget | Own exactly one timer; clear fired/removed IDs and cancel on destruction |
| DPI parsing accepts NaN/infinity/trailing garbage, and conversion can exceed Dimension | Undefined floating-to-integer conversions and invalid control defaults | Require a finite positive complete DPI value and saturate computed dimensions |
| SpinBox default arrows remain 16 physical pixels on dense screens | Small click targets beside enlarged text/indicators | Scale the default using the existing 75 DPI convention; preserve explicit sizes |
| List navigation ignores failed multibyte decoding | Uninitialized comparison input and unsafe handling of malformed/truncated commits | Use per-conversion state and check decoding results; preserve valid multibyte navigation |
| WML generation reads one extra byte from every string and debug byte table | Sanitizer failure and arbitrary padding bytes copied from adjacent storage | Emit the existing WMD padding byte explicitly as zero and bound debug iteration |
| WML output names use an unchecked 80-byte buffer and write errors are ignored | Long-path corruption, missing-argument crash or successful exit with partial output | Retain the argument pointer, check missing arguments and report stream/close failures |
| UIL WMD path handling underallocates absolute paths, underreads short names and frees getenv storage | Heap corruption or invalid free during ordinary database loading | Copy owned path strings correctly, guard the suffix length and release only owned allocations |

## Compatibility

No public resource types, function signatures, instance layouts or shared-library
version are changed by this patch. It can be applied to the existing 2.3.8-derived
ABI independently of larger text-layout changes. Corrected numeric values and
selection preservation intentionally replace erroneous results.

Numeric input retains locale decimal separators, scientific notation, truncation
of extra fractional places, and the historical numeric-prefix acceptance policy.
NaN/invalid/empty input returns XmCURRENT_VALUE. Positive/negative unbounded input
returns XmMAXIMUM_VALUE/XmMINIMUM_VALUE. Legacy hexadecimal floating input is
retained. The int position resource still limits the representable range.

The existing scale convention is Xft.dpi / 75, with no downscaling below the
historical default. XmNarrowSize and XmNindicatorSize explicitly supplied by an
application remain authoritative. DPI defaults are chosen at widget creation;
this does not add live monitor-change handling, per-monitor fractional scaling,
automatic scaling of application pixmaps or a new graphics backend.

WMD table framing and length fields remain unchanged, including the historical
extra byte following a terminated string. That byte is now deterministic zero
padding rather than an out-of-bounds read. Existing WMD readers retain the same
layout. This does not redesign the architecture-dependent WMD file format.

## Verification

`tests/automated/run-widget-regressions.py PREFIX FRESH_ABSOLUTE_RESULTS` builds
consumers against the installed library, checks their dynamic library origin,
and runs them on isolated Xvfb servers. It tests integer limits, exact decimal
and exponent input, a generated German numeric locale, index conversion,
boundary callbacks, timer destruction, UTF-8 string mutation, valid/malformed
list commits, and DPI values at 1x/2x/4x plus explicit-size preservation.
The list test interposes only the XIM lookup boundary; it does not establish
live XIM preedit/commit/cancel behavior. Sanitizer coverage requires both the
library and consumers to be instrumented; the runner's --sanitize flag alone
instruments consumers.

`tests/automated/check-wmd.py BUILD_ROOT FRESH_ABSOLUTE_RESULTS` checks WML
debug generation, output names longer than 80 bytes, missing arguments, write
failure, UIL compilation using the generated database, absolute/short names and
WMDPATH. Optional `--legacy-uil /absolute/old/bin/uil` tests an earlier reader.

## Broader Unicode and graphics qualification

Unicode scalar positions, grapheme deletion/motion, bidi caret affinity,
paragraph/line shaping, font fallback and XIM ownership require a coordinated
text-layout implementation and its separate compatibility review. Merely
accepting UTF-8 strings or drawing with Xft is insufficient. Tests of combining
marks, emoji sequences, Arabic/Hebrew, Indic scripts, wrapped text and selections
must accompany that work. This patch does not claim those capabilities.

Likewise, DPI-aware control defaults do not establish rendering quality across
all widgets or desktops. Mixed-scale monitors, large coordinate/extents,
pixmap assets, XRender compositing, physical high-resolution displays and
Xwayland need their own measured, actual-window qualification. No speculative
replacement of Motif's application geometry contract is included here.
