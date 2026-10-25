# Native text and external-IME qualification matrix

This matrix records which native text boundaries are evidence-backed. A
provider row is only a pass when the test observes the provider's real XIM
callbacks; a deterministic interposed lookup or local Compose table is a
separate toolkit-contract test.

| Boundary | AArch64 Linux (`uconsole`) | x86_64 Linux | Evidence / remaining gap |
| --- | --- | --- | --- |
| UTF-8 scalar and UAX #29 grapheme movement/deletion | Pass | Pending | Installed native text and map fixtures; x86_64 admission remains open. |
| Combining, emoji, RTL, CJK and fallback-font corpus | Pass for recorded corpus | Pending | Native layout/readback corpus; not exhaustive font coverage. |
| Local Compose/dead key | Pass with private `@im=none` Xvfb | Pending | XTest fixture proves local XIM/XIC registration and exact commits; it is not a physical-keyboard test. |
| IBus XIM composition, commit and cancellation | Focused pass | Pending | Real IBus/m17n fixture; standalone combining-mark commit and candidate panels remain gaps. |
| Provider restart and transport loss | Focused pass | Pending | Same-process service loss/reconnect fixture; broader engines/providers remain unqualified. |
| Clipboard UTF-8 round trip and interrupted INCR lifetime | Pass for the bounded AArch64 cohort with pinned corrected Xt | Pending | Multi-megabyte UTF-8 bytes, cancellation, and a fresh transfer from the surviving owner pass for Text and TextField; system-Xt failure, application workflows, broader DnD, and x86_64 remain separate. |
| Strict ASan/UBSan | Body completes; exit leak gate open | Pending | Xt selection-context and Fontconfig ownership reports are retained, not suppressed. |

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
panels, screen-reader usability, or a clean sanitizer exit. Detailed provider
behavior and the recorded ARM64 versions are in `docs/unicode.md`; aggregate
admission limits are in `docs/status.md`.

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

The same pinned cohort passed the consumer `--sanitize` run for the
interrupted-clipboard fixture on 2026-10-25. That result covers the transfer
body and its fixture process only; it does not close the matrix-wide leak gate
for Xt selection-context or Fontconfig ownership.
