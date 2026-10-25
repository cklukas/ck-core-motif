# Native text and external-IME qualification matrix

This matrix records which native text boundaries are evidence-backed. A
provider row is only a pass when the test observes the provider's real XIM
callbacks; a deterministic interposed lookup or local Compose table is a
separate toolkit-contract test.

| Boundary | AArch64 Linux (`uconsole`) | x86_64 Linux | Evidence / remaining gap |
| --- | --- | --- | --- |
| UTF-8 scalar and UAX #29 grapheme movement/deletion | Pass | Bounded pass | Installed scalar/map/preedit/widget fixtures and the 1 MiB large-text stress fixture pass on x86_64 with the recorded fallback corpus. |
| Combining, emoji, RTL, CJK and fallback-font corpus | Pass for recorded corpus | Bounded pass | The recorded x86_64 fontconfig corpus passes layout, line-context, Unicode-widget, font-resize and large-text fixtures, including flags, ZWJ families and color-glyph transport. |
| Non-text Unicode labels, compound strings, List search, mnemonics, ComboBox, Menu and FileSelection values | Focused pass | Focused pass | `run-nontext-unicode-regressions.py` covers combining, non-BMP and Han values, a two-segment compound `XmString` with separator checks, a non-ASCII `XmNmnemonic` round trip, and List existence/position/match search through public Label, List, ComboBox, Menu and FileSelection APIs; physical-keyboard and application journeys remain open. |
| Local Compose/dead key | Pass with private `@im=none` Xvfb | Pending | XTest fixture proves local XIM/XIC registration and exact commits; it is not a physical-keyboard test. |
| IBus XIM composition, commit and cancellation | Focused pass | Pending | Real IBus/m17n fixture; standalone combining-mark commit and candidate panels remain gaps. |
| Provider restart and transport loss | Focused pass | Pending | Same-process service loss/reconnect fixture; broader engines/providers remain unqualified. |
| Clipboard UTF-8 round trip and interrupted INCR lifetime | Pass for the bounded AArch64 cohort with pinned corrected Xt | Bounded pass with pinned corrected Xt | Multi-megabyte UTF-8 bytes, direct Motif-to-Motif Text/TextField round trips, cancellation, a fresh transfer from the surviving owner, and the installed 8/16/32-bit transfer-storage/owner-replacement cohort pass; x86_64 now has the bounded large round trip, storage modes and cancelled-transfer lifetime with the pinned Xt. Broader DnD remains separate. |
| Motif drag/drop transfer, receiver/source cancellation and initiator cancellation | Focused pass | Independent-client modes pass | Installed XTEST gestures cover same-process and independent-client copy transfer, same-process and independent-client two-target transfers, independent-client 4 MiB incremental assembly, receiver-side delayed/active `XmDropTransfer` destruction, request cancellation and requestor-shell teardown, source-side owner replacement during an active drag and post-drop incremental transfer, drop admission, source conversion, transfer-entry client data, Xt-owned selection values and `XmDragCancel` finish notification; x86_64 evidence is limited to three fresh runs of the independent-client 14/22/116-check modes. Complete application workflows remain open. See `docs/dnd-transfer-lifecycle.md`. |
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

The current matrix deliberately does not claim complete x86_64 text/IME or
accessibility qualification, complete desktop input-method interoperability,
physical keyboard layouts, candidate panels, screen-reader usability, or a
matrix-wide clean sanitizer exit. The bounded x86_64 results are recorded in
`docs/status.md`; they do not substitute for the missing journeys. Detailed provider
behavior and the recorded ARM64 versions are in `docs/unicode.md`; aggregate
admission limits are in `docs/status.md`.

The installed `nontext-unicode` fixture is a deliberately bounded public-API
consumer check. It round-trips combining, non-BMP and Han `XmString` values
through Label, List, ComboBox, Menu and FileSelection resources, verifies a
two-segment compound string, List selection/search, and checks the menu label
and file-selection directory-spec paths. It does not substitute for an
application journey or for the remaining physical-keyboard matrix.

On 2026-10-25, the x86_64 WSL2 cohort passed the installed `abi`,
`incremental-map`, `preedit`, `layout`, `line-context`, `unicode-widgets`,
`font-resize` and `large-text` fixtures. The fresh receipt is
`/mnt/v/tmp/ckmotif-x86-20261025/results/text-pass-r5/receipt.json`; it records
the x86_64 ELF checks, installed-header/library fingerprints and the color-
enabled fontconfig file hash
`524b8206b678c48169cf9a7310303934a43618b2bafd7be8adbf231151d2fda3`.
The task-local corpus uses Noto CJK `1:20220127+repack1-1`, Noto core
`20201225-1build1`, Symbola `2.60-1.1` and Noto Color Emoji
`2.047-0ubuntu0.22.04.1`. The layout capture now groups glyph clusters that
belong to one extended grapheme, including regional-indicator flags in both LTR
and RTL context, and drawing batches Xft glyph requests to stay
inside the X11 Render request limit. The large fixture passes with
1,073,152 bytes, 761,856 scalars and 696,320 graphemes per widget. This is
bounded Xvfb evidence; physical displays, external IME services and broader
architecture acceptance remain separate gates. Full receipt and asset
fingerprints are in `docs/x86-text-qualification.md`.

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

On 2026-10-25, the x86_64 installed cohort passed the 11-check large UTF-8
clipboard round trip and the four transfer-storage modes (`clipboard`, `merge`,
`bounds` and `lifetime`) under the ordinary Xvfb profile. The strict storage
rerun initially reported 104 and 416 bytes rooted in system Xt for the
clipboard and lifetime modes. Repeating those modes with the pinned libXt
1.3.1 correction passed all four with ASan/UBSan leak detection enabled. The
native cancelled-transfer observer then passed its ABI and lifetime checks
with the same pinned Xt, using the fingerprinted color-font configuration to
avoid unrelated text-layout warnings. The x86 toolkit fingerprint is
`58bc39a032d63a600942e90554ab67655dd08eed4b3303e878b59e3f135ccf94`; the
pinned Xt library fingerprint is
`0588577bc669adc2c71a0a09b061970c7a944fe9f1f61a699cef05346bf8e538`.
Receipts are retained under
`/mnt/v/tmp/ckmotif-x86-20261025/results/storage-x86-xt-asan-r1/` and
`/mnt/v/tmp/ckmotif-x86-20261025/results/clipboard-native-x86-xt-r2/`.
This closes only the bounded x86 clipboard/selection-lifetime cohort; broader
application workflows, other providers and matrix-wide sanitizer ownership
remain open.
