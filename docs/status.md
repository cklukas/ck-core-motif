# Implementation and validation status

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
Actual Orca/eSpeak diagnostics against that prefix speak the covered native button/toggle/text,
selection, Disclosure and ScrollBar controls. Precise editing notifications,
native list/menu item semantics, complete application journeys and human
screen-reader usability remain unqualified. Initial registration state and
first-collapse behavior pass in the complete rebuilt suite. The Orca baseline
records 98 utterances and 2,123,786 bytes of synthesized PCM with no reported
errors; it explicitly leaves the listed semantic and usability gaps open.

The independent Xlib clipboard fixture exercises exact Unicode bytes and both
directions of large INCR transfer. Diagnostics identified a peer event-queue
wait error and a separate Xt timeout ownership defect. Correcting the peer's
flush-before-queue-check ordering passes repeated instrumented transfers. The
interrupted-transfer regression fails with system Xt and passes for Text and
TextField with the pinned corrected Xt dependency, including a fresh transfer
from the same surviving owner. Both regressions also pass in the combined
installed-prefix suite. See the [dependency build](build.md). This verifies the
recorded native toolkit/dependency/consumer scope; application workflows,
physical display/input qualification and other architectures remain separate
admission gates.

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
are covered alongside compound-row traversal. These source/API and native results
do not establish complete list/selection semantics, all editing notifications,
actual screen-reader usability, x86_64 or sanitizer acceptance for the additions.
