# Implementation and validation status

The current WP-05.2 increment provides a deterministic per-display scale
override and shared UI-thread scale transaction through `<Xm/Display.h>`.  The
installed AArch64 `uconsole` consumer passes three fresh live Xvfb runs and a
consumer-only ASan/UBSan run while preserving Xrm identity, toggle state and
explicit indicator and SpinBox arrow resources, and reflowing unlabelled
realized `ToggleButton` and toolkit-owned `SpinBox` defaults.  Complete
historical-widget coverage, live appearance
transactions and real multi-output XLibre reflow remain open.

## Native hard-line and geometry qualification

The focused native AArch64 installed ABI 8 cohort passes all 33 selected feature
fixtures. The initial run's private accessibility socket placement failure is
retained; a suffix run with an explicit short runtime path completes the remaining
checks against identical installed library/header fingerprints. This is aggregated
fixture coverage, not one uninterrupted green run. Clipboard, clipboard lifetime
and the opt-in external XIM service remain excluded from this aggregate cohort.

The new native retained-output regression passes 12,890,100 assertions and 417
native/fresh line comparisons with zero layout-failure warnings. It covers shared
sources, complete mandatory separators, CRLF-forming edits, distant first-strong
bidi context, wrap/font changes, fixed dimensions, omitted-dimension sizing and
capture lifetime. Unaffected paragraph captures retain identity; edited hard
paragraphs may require full-context work. This does not claim incremental shaping
inside an edited paragraph or complete live editor/input acceptance.

The earlier strict ASan/UBSan profile reported 208 bytes in two Xt
selection-context allocations. The pinned Xt dependency patch now retires both
the destroyed-widget context and the final INCR value safely. A fresh helper-built
Xt prefix passed strict installed-prefix `abi`, `incremental-layout` and
`clipboard-lifetime` fixtures with no sanitizer diagnostics; see the dependency
receipt recorded by the native matrix. Separate strict font-lifetime
qualification passes 79 assertions on two independent displays with zero X errors
and no leaks. An additional strict legacy layout probe reports 8,839 bytes in 128
Fontconfig allocations; broad matrix-wide sanitizer acceptance is not claimed.
The strict installed FontSelector consumer also reports one 160-byte shared
Xt/Xlib GC-cache allocation at process exit after all observed Motif label
destroy paths release their GCs. This is retained as an external ownership
boundary, not suppressed or counted as a toolkit pass; see
`docs/fontselector-sanitizer-boundary.md`.
No warning suppression, removed focus action or disabled leak detection is used.
Native record layout and ABI epoch 8 remain unchanged. A fresh x86_64
installed cohort was qualified on 2026-10-25 from an out-of-tree build with
`--enable-xft --enable-unicode-text --enable-atspi --enable-jpeg
--enable-png --enable-svg --enable-xlibre-dpi --disable-printing`. The
installed `libXm.so.8.0.0` fingerprint is
`deae1ad2d0067bffde77be1fbb452802e2fe95f113057e49c2eab4127a38f5c7`; the
cohort also installed `libMrm`, `libUil`, the public headers and the UIL
compiler. WP-00 baseline, WML token bounds, the 25-check non-text Unicode
fixture, the accessibility-menu fixture, the XLibre RandR-DPI contract and
fallback, independent-client DnD (14 single-target, 22 multiple-target and
116 incremental checks), and the C++ support fixture (including ASan/UBSan)
pass against that exact prefix. The cross-process DnD modes passed in three
fresh runs each; the source fixture records hash
`053a76d5d7c8d44447fd320e8b6da19d9e5fe0de85cafe87a5ee09deb402f0f1`.

This is bounded x86_64 evidence, not complete architecture acceptance. The
full optional demo tree now also compiles in that cohort after removing the
case-insensitive `String.h`/`<string.h>` collision in the Exm demonstration
library; the build still reports the historical demo warnings. Full
ICU/HarfBuzz text readback, external IME services, real AT-SPI/screen-reader
journeys, clipboard workflows, application DnD, physical display/input and
matrix-wide sanitizer ownership remain explicit gates.

The XLibre observer lifetime follow-up was rebuilt separately on x86_64 with
the adapter enabled. Its installed `libXm.so.8.0.0` fingerprint is
`58bc39a032d63a600942e90554ab67655dd08eed4b3303e878b59e3f135ccf94`; the
parser/fallback, deterministic output-selection and ordinary-server observer
fixtures pass under Xvfb, together with the source lifetime guard. The
multi-output XLibre server scenario remains unqualified and is not inferred
from this fallback run.

The same x86_64 cohort now also passes the bounded large UTF-8 clipboard
round-trip and transfer-storage clipboard/merge/bounds/lifetime modes. The
strict rerun passes with the pinned corrected libXt 1.3.1 prefix and leak
detection enabled; the unpinned run's system-Xt allocations are retained as
the external ownership boundary rather than suppressed. The cancelled-transfer
observer passes ABI and lifetime checks with the same dependency and the
fingerprinted color-font configuration. This does not qualify application
clipboard managers, broader DnD workflows or matrix-wide sanitizer ownership.
The strict native observer rerun also passes with leak detection enabled; its
receipt is retained at
`/mnt/v/tmp/ckmotif-x86-20261025/results/clipboard-native-x86-xt-asan-r1/`.

The same installed x86_64 prefix now passes the bounded text cohort
(`incremental-map`, `preedit`, `layout`, `line-context`, `unicode-widgets`,
`font-resize` and `large-text`) with a task-local CJK/Unicode/color-emoji
fallback corpus. The runner accepts and fingerprints an explicit
`--fontconfig-file`; the fresh receipt is
`/mnt/v/tmp/ckmotif-x86-20261025/results/text-pass-r5/receipt.json`, with
fontconfig hash
`524b8206b678c48169cf9a7310303934a43618b2bafd7be8adbf231151d2fda3`.
The text-layout fix keeps multi-cluster extended graphemes together for caret
geometry and bounds Xft glyph requests to avoid X11 Render request overflow.
The large fixture reports 1,073,152 bytes, 761,856 scalars and 696,320
graphemes per widget with no layout warning or X error. This remains bounded
Xvfb evidence, not complete physical-display, external-IME or architecture
acceptance. The complete receipt and staged-font fingerprints are retained in
`docs/x86-text-qualification.md`.

An isolated AArch64 `uconsole` rebuild of commit `1c2e4379` with Xft, Unicode
text and AT-SPI enabled also passes the installed `abi`, `layout` and
`large-text` fixtures in 10.44 seconds. Its 1 MiB run reports the same
1,073,152-byte, 761,856-scalar and 696,320-grapheme-per-widget contract. The
host lacks Xrandr headers, so this focused check intentionally omits the
optional XLibre-DPI adapter; that host capability boundary does not affect the
text-layout qualification.

## Earlier incremental map qualification

Native AArch64 installed ABI 8 passes 32 selected feature fixtures, including
immutable incremental maps and realized shared Text/TextField lifecycle checks.
The native widget fixture performs 69,207,073 assertions. The source-map unit
passes 19,681,967 assertions against fresh construction and an independent ICU
oracle. Its allocator-fault companion passes 70,564 assertions across 22 creation
and 17 replacement failure points; both direct-source fixtures also pass strict
ASan/UBSan with leak detection enabled. A Unicode-disabled direct-source probe
checks the unavailable API contract without ICU linkage; this is not a complete
optional-toolkit configuration build.

The selected aggregate suite excludes clipboard, clipboard lifetime and the
opt-in external XIM service. The bounded strict dependency-cohort fixtures cover
the two clipboard/selection lifetime paths separately; matrix-wide leak
acceptance remains open for Fontconfig and other unqualified ownership paths.
No suppression or disabled leak gate is used for this qualification. The
source-map result does not establish incremental shaping, complete interactive
editor projection or x86 acceptance.


This is a development fork. Separate source availability, focused verification
and admission of an installed cohort. Optional features also depend on the
configuration used to build that cohort.

## Maintenance foundation

The maintenance source through `2b63cf1` has been built and exercised with
native Linux fixtures covering legacy behavior, selection timestamp counts,
parallel string generation and basic widget operation. This does not validate
features introduced afterward.

## Earlier installed ABI 5 cohort

| Area | Source and verification boundary |
| --- | --- |
| ABI cohort | Versioned headers and runtime epoch query are implemented. A clean rebuild and loaded-library proof are required for every consumer. |
| Unicode maps | Exact UTF-8/scalar mappings and ICU extended-grapheme boundaries have focused utility tests. |
| Shaped captures | Native ARM Linux strict compilation and Xvfb readback fixtures cover Arabic, Hebrew, Latin, combining marks, ligatures, CJK fallback and tabs. |
| Line context | Focused native fixtures cover full-paragraph bidi/isolate context, actual-line Arabic shaping, retained ownership and mixed-script wrapping. |
| Text/TextField integration | Shared capture paths for paint, geometry, hits, selections and movement are implemented. Installed ABI 5 native ARM64 fixtures pass for actual Text/TextField paint, hit/selection geometry, grapheme movement and constrained wrapping. |
| XIM | Preedit range and conversion validation are implemented. Real input-method service composition, cancellation, dead keys and restart behavior require service tests. |
| Accessibility | Semantic providers and optional transport are implemented. Installed native ARM64 D-Bus observer fixtures pass for service absence, connection, events, actions and multiple shells under one application root. Real screen-reader usability remains unqualified. |
| Disclosure | Installed native ARM64 interaction, geometry, focus, child-state retention, callback/destruction and real accessibility observer fixtures pass. |
| Clipboard | Timestamp fixes are covered by a dedicated fixture. Large INCR transfers and full Unicode clipboard round trips remain separate acceptance cases. |

The focused layout fixtures do not establish complete Unicode conformance,
all-font correctness, input-method interoperability or screen-reader usability.
The native ARM64 cohort also passes the legacy/basic GUI fixture. These results
cover the tested fonts, strings and interactions; they do not establish exhaustive
supplementary-plane coverage, long-line performance or intended-consumer usability.
All x86_64 compilation and runtime validation remain pending.

## Development 2.4 / ABI 6 candidate

ABI 6 changes native text records and the public accessibility provider/info
layouts. Rebuild every consumer against one exact prefix; do not alias an older
SONAME. The initial native ARM64 candidate passes installed feature, widget,
resource-compiler, legacy interaction and real IBus/m17n service fixtures.
The later `b726c1d` cohort has now been cleanly built with the corrected pinned
Xt dependency and exercised as one installed prefix on native ARM64. Its
11 default native feature fixtures, widget regressions, WMD/UIL checks, real
IBus/m17n service suite, and independent C11/C++ SDK consumer checks pass.
The clean combined build took 115.508 seconds, including building Xt; this is
the first measured profile with that dependency and is not compared as an
equivalent to a Motif-only build.

Bounded font contexts reduce the measured mixed-script large-text fixture from
51.557 to 2.309 seconds for wrapped Text loading. Single-line mutation still
takes 2.344 seconds and is not qualified as instantaneous. Retained captures,
font leases, configuration changes and documented column metrics have focused
native checks; see [Unicode text](unicode.md) for exact workload and limits.

The live accessibility tree now has ordered child indexes, bounded live-object
hash tables and iterative subtree removal. Source-linked checks exercise
10,000 nodes, 20,000-level teardown, reentrancy, exact real D-Bus Cache and
structural signals, native focus and authoritative wrapped visual lines.
These lifecycle, geometry and real D-Bus tree checks also pass against the
installed `b726c1d` library, with matching private fixture headers where required.
Actual Orca/eSpeak diagnostics against that prefix speak the covered native
button/toggle/text, list/list-item, menu/menu-item, selection, Disclosure and
ScrollBar controls. Precise editing notifications, complete desktop/application
journeys and human screen-reader usability remain unqualified. Initial
registration state and first-collapse behavior pass in the complete rebuilt
suite. The latest bounded Orca receipt records 113 utterances and 2,436,594
bytes of synthesized PCM with no reported errors; it explicitly leaves the
listed semantic and usability gaps open.

The independent Xlib clipboard fixture exercises exact Unicode bytes and both
directions of large INCR transfer. Diagnostics identified a peer event-queue
wait error and a separate Xt timeout ownership defect. Correcting the peer's
flush-before-queue-check ordering passes repeated instrumented transfers. The
interrupted-transfer regression fails with system Xt and passes for Text and
TextField with the pinned corrected Xt dependency, including a fresh transfer
from the same surviving owner. Direct Motif-to-Motif Text/TextField owner/import
round trips also pass for mixed-script and multi-megabyte payloads, with an
independent Xlib re-export check. Both regressions also pass in the combined
installed-prefix suite. See the [dependency build](build.md). This verifies the
recorded native toolkit/dependency/consumer scope. Installed same-process and
independent-client Motif drag gestures now pass conversion, valid-drop
admission, transfer-entry client-data propagation and Xt-owned selection-value
checks in three repeated Xvfb runs each. The companion initiator-side
`XmDragCancel` probe also passes its finish notification contract. The
receiver-side `XmDropTransfer` lifetime probe now passes delayed-start,
active-incremental request cancellation and requestor-shell destruction,
including destruction from the first transfer callback, in three repeated runs
each. The source-side probe now
passes owner replacement during an active drag and post-drop incremental
transfer cancellation in three repeated runs each. The same-process
multiple-target probe also passes its two-target client-data/payload contract
in three repeated runs. The cross-process fixture also passes 4 MiB
incremental payload assembly in three repeated runs. Broader application
drag-and-drop workflows, physical display/input qualification and other
architectures remain separate admission gates. See
`docs/dnd-transfer-lifecycle.md`.

The newer ABI 6 candidate, its consumers and documentation deployment remain
distinct gates. Final x86_64 validation of this candidate remains pending.

## Primary-window modality native checkpoint

The window-manager-independent primary-modal correction passes native ARM64
installed-prefix validation. The strict C11 fixture uses real XTEST pointer and
keyboard dispatch and passes 397 assertions with no X errors or Xt warnings. It
covers raw grab ancestry and duplicate destruction, full-modal barriers, newly
created windows and deferred popups, nested/parallel primary dialogs, reentrant
menu-popdown callbacks that reallocate the grab array, and repeated teardown.
The final fixture tracks retirement of XmDisplay after the last VendorShell.
The same fixture fails against the preceding library when an independent window
should accept input.

All 12 default native feature fixtures and the separate real IBus service fixture
pass against the new coherent ABI6 prefix with corrected pinned Xt1.3.1. The ABI
and modality consumers also pass with the toolkit and consumers instrumented by
ASan/UBSan. Xt and other system dependencies remain uninstrumented in that
profile; leak detection is disabled and no leak-clean claim is made.

These results cover the recorded native client input contract. Other window
managers, complete assistive/application usability and final x86_64 validation
remain separate gates. No new hosted documentation deployment is claimed.

## Evidence to retain

For an admitted build, retain source commit and dirty-state proof, configure
options, dependency/compiler versions, start/end/elapsed time, exact prefix,
header and library hashes, SONAMEs, runtime-loaded paths and fixture outputs.
Record a measured duration baseline only when comparable evidence exists.
Documentation publication has its own [source-pinned CI receipt](documentation.md);
a site build does not validate the native widget library.


## Text events, menus and compound traversal checkpoint

The native ARM64 default feature suite now has 15 passing fixtures. New exact text
change checks include Unicode scalar payloads, no-op/caret suppression, provider
lifetime/reentrancy, password-role refusal and 20 libdbus construction/send fault
cases with baseline recovery. Menu roles/actions/expansion and posted-shell focus
are covered alongside compound-row traversal. The installed non-text Unicode
consumer also validates compound segment payloads/separators and a non-ASCII
mnemonic resource. These source/API and native results do not establish
complete list/selection semantics, physical-keyboard layouts, all editing
notifications, actual screen-reader usability, x86_64 or sanitizer acceptance
for the additions.

## Accessibility modal admission checkpoint

The current native ARM64 default suite contains 21 passing fixtures. The added
accessibility-modality fixture passes 272 checks, while the native pointer/key
modality fixture retains 397 assertions. Blocked native and semantic targets
reject action/focus/value/Selection mutations; independent primary-dialog
branches remain usable. Reentrant query/snapshot callbacks and native focus
target/bridge destruction are covered. This checkpoint qualifies the generic
installed-library contract; actual application/assistive-technology journeys,
other display/input environments and final x86_64 validation remain separate.

Ten focused native ARM64 fixtures also pass with the toolkit and consumers
instrumented by ASan/UBSan, including the new modal-admission regression.
Xt/system dependencies are uninstrumented, leak checking is disabled, and
allocator-fault interposition remains exclusive to the normal native suite.

## Native clipping and label-query qualification

Native ARM64 fixtures verify 34 semantic queries and 267 assertions covering
ancestor clipping, edge contact, nested common intersection, managed gadgets,
independent popup mapping, automatic scroll reveal and repeated Disclosure/header
names. Normal and ASan/UBSan profiles pass with leak detection enabled. A real
private registry observes exactly one target SHOWING transition per ancestor
reveal/hide, with matching query states and normal fixture exit.

These checks do not qualify occlusion by unrelated windows, physical scale,
screen-reader operation or every consumer cohort. Existing application/toolkit
pins require their own integration admission. The native name-query correction
releases Label/LabelGadget getter copies while preserving Disclosure's borrowed
resource ownership; the former leak remains recorded in the qualification data.
