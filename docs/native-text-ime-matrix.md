# Native text and external-IME qualification matrix

This matrix records which native text boundaries are evidence-backed. A
provider row is only a pass when the test observes the provider's real XIM
callbacks; a deterministic interposed lookup or local Compose table is a
separate toolkit-contract test.

| Boundary | AArch64 Linux (`uconsole`) | x86_64 Linux | Evidence / remaining gap |
| --- | --- | --- | --- |
| UTF-8 scalar and UAX #29 grapheme movement/deletion | Pass | Pending | Installed native text and map fixtures; x86_64 admission remains open. |
| Combining, emoji, RTL, CJK and fallback-font corpus | Pass for recorded corpus | Pending | Native layout/readback corpus; not exhaustive font coverage. |
| Non-text Unicode labels, compound strings, List search, ComboBox, Menu and FileSelection values | Focused pass | Pending | `run-nontext-unicode-regressions.py` covers combining, non-BMP and Han values, a two-segment compound `XmString`, and List existence/position/match search through public Label, List, ComboBox, Menu and FileSelection APIs; physical-keyboard and application journeys remain open. |
| Local Compose/dead key | Pass with private `@im=none` Xvfb | Pending | XTest fixture proves local XIM/XIC registration and exact commits; it is not a physical-keyboard test. |
| IBus XIM composition, commit and cancellation | Focused pass | Pending | Real IBus/m17n fixture; standalone combining-mark commit and candidate panels remain gaps. |
| Provider restart and transport loss | Focused pass | Pending | Same-process service loss/reconnect fixture; broader engines/providers remain unqualified. |
| Clipboard UTF-8 round trip and interrupted INCR lifetime | Pass for the bounded AArch64 cohort with pinned corrected Xt | Pending | Multi-megabyte UTF-8 bytes, direct Motif-to-Motif Text/TextField round trips, cancellation, a fresh transfer from the surviving owner, and the installed 8/16/32-bit transfer-storage/owner-replacement cohort pass; system-Xt failure, broader DnD, and x86_64 remain separate. |
| Same-process Motif drag/drop transfer and initiator cancellation | Focused pass | Pending | Installed XTEST gestures cover drop admission, source conversion, transfer-entry client data, Xt-owned selection values and `XmDragCancel` finish notification; cross-process transfer, selection-transfer cancellation, broader application workflows and x86_64 remain open. See `docs/dnd-transfer-lifecycle.md`. |
| Strict ASan/UBSan | Bounded pass with pinned Xt; matrix-wide gate pending | Pending | A fresh helper-built Xt cohort passes ABI, incremental-layout and clipboard-lifetime with leak detection enabled. The strict FontSelector consumer still reports one 160-byte shared Xt/Xlib GC-cache allocation at process exit; Fontconfig and broader ownership paths remain separate gates. See `docs/fontselector-sanitizer-boundary.md`. |

Run the focused installed service fixture with:

```sh
python3 tests/automated/check-native-features.py \
  --prefix /absolute/installed/prefix \
  --out /absolute/fresh/results --only xim-service
```

The default native suite is useful for the non-provider rows. It must be run
against one clean installed cohort with matching headers and libraries. Missing
services or fonts are recorded as unavailable matrix entries; they are not
replaced by synthetic callbacks or treated as successful provider coverage.

The current matrix deliberately does not claim x86_64 qualification, complete
desktop input-method interoperability, physical keyboard layouts, candidate
panels, screen-reader usability, or a matrix-wide clean sanitizer exit. Detailed provider
behavior and the recorded ARM64 versions are in `docs/unicode.md`; aggregate
admission limits are in `docs/status.md`.

The installed `nontext-unicode` fixture is a deliberately bounded public-API
consumer check. It round-trips combining, non-BMP and Han `XmString` values
through Label, List, ComboBox, Menu and FileSelection resources, verifies a
two-segment compound string, List selection/search, and checks the menu label
and file-selection directory-spec paths. It does not substitute for an
application journey or for the remaining physical-keyboard matrix.

The native runner accepts repeated `--runtime-library-dir` arguments for a
known dependency cohort. The selected Motif library directory remains first in
the runtime and link search path, while the additional directories supply
matching Xt/X11 dependencies. The receipt records every directory so a result
cannot be mistaken for a system-library run. For example, the interrupted
clipboard transfer fixture must be run with the pinned corrected Xt directory;
the system Xt on `uconsole` exits the owner during cancelled-transfer cleanup,
while the pinned dependency cohort keeps the owner alive and completes the
fresh transfer. This is a dependency qualification boundary, not a suppressed
toolkit failure.

On 2026-10-25, the official dependency build helper produced a fresh pinned Xt
prefix, and the consumer `--sanitize` run passed ABI, `incremental-layout` and
`clipboard-lifetime` with leak detection enabled. This closes the bounded Xt
selection-context and interrupted-INCR ownership regressions; it does not close
the matrix-wide leak gate for Fontconfig, the strict FontSelector Xt/Xlib GC
cache boundary, or other unqualified ownership paths. The FontSelector result
is retained as a failure rather than suppressed; see
`docs/fontselector-sanitizer-boundary.md`.

The clipboard observer now additionally runs direct Motif-to-Motif Text and
TextField owner/import workflows for both the mixed-script and large payloads.
The destination re-exports the received value to an independent Xlib
requestor, so the application path is checked without trusting only the paste
return value; large transfers must still use multiple INCR chunks.

The transfer-storage fixture also passes its clipboard, merge, bounds and
lifetime modes against the pinned corrected Xt cohort. It exercises 8-, 16-
and 32-bit format assembly across large split copies, replacement of named,
primary and secondary selection owners, repeated replacement, and destruction
of the active owner. Run the focused cohort with the repeatable mode selector:

```sh
python3 tests/automated/run-storage-regressions.py \
  /absolute/installed/prefix /absolute/fresh/results \
  --runtime-library-dir /absolute/pinned-xt/lib \
  --only clipboard --only merge --only bounds --only lifetime
```

The optional `--runtime-library-dir` is recorded in the receipt and keeps this
focused result distinct from the separate Xt callback-lifetime fixture and
from application-level drag-and-drop workflows.

The same dependency-directory contract reaches the local Compose observer. A
normal installed-prefix run passed the local XIM/XIC and dead-key fixture while
loading the pinned Xt directory; its receipt records both the Motif and
dependency paths. A strict attempt reaches the fixture body but remains
bounded by the external Fontconfig ownership report, so that result is kept as
a dependency-sanitizer boundary rather than suppressed.
