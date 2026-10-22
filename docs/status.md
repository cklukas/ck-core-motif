# Implementation and validation status

This is a development fork. Separate source availability, focused verification
and admission of an installed cohort. Optional features also depend on the
configuration used to build that cohort.

## Maintenance foundation

The maintenance source through `2b63cf1` has been built and exercised with
native Linux fixtures covering legacy behavior, selection timestamp counts,
parallel string generation and basic widget operation. This does not validate
features introduced afterward.

## Development 2.4 / ABI 5

| Area | Source and verification boundary |
| --- | --- |
| ABI cohort | Versioned headers and runtime epoch query are implemented. A clean rebuild and loaded-library proof are required for every consumer. |
| Unicode maps | Exact UTF-8/scalar mappings and ICU extended-grapheme boundaries have focused utility tests. |
| Shaped captures | Native ARM Linux strict compilation and Xvfb readback fixtures cover Arabic, Hebrew, Latin, combining marks, ligatures, CJK fallback and tabs. |
| Line context | Focused native fixtures cover full-paragraph bidi/isolate context, actual-line Arabic shaping, retained ownership and mixed-script wrapping. |
| Text/TextField integration | Shared capture paths for paint, geometry, hits, selections and movement are implemented. Full new-cohort widget fixture admission is pending. |
| XIM | Preedit range and conversion validation are implemented. Real input-method service composition, cancellation, dead keys and restart behavior require service tests. |
| Accessibility | Semantic providers and optional transport are implemented. Service absence, connection, events, multiple shells and assistive-client behavior must be checked for the selected cohort. |
| Disclosure | Native section API and implementation are under development; integrated build, interaction and lifecycle fixtures are pending. |
| Clipboard | Timestamp fixes are covered by a dedicated fixture. Large INCR transfers and full Unicode clipboard round trips remain separate acceptance cases. |

The focused layout fixtures do not establish complete Unicode conformance,
all-font correctness, input-method interoperability or screen-reader usability.
Wrapped widget behavior, visual selection, supplementary-plane glyphs, keyboard
navigation, font fallback, long-line performance and destruction/recreation all
need to be exercised in the actual installed toolkit and its intended consumers.

## Evidence to retain

For an admitted build, retain source commit and dirty-state proof, configure
options, dependency/compiler versions, start/end/elapsed time, exact prefix,
header and library hashes, SONAMEs, runtime-loaded paths and fixture outputs.
Record a measured duration baseline only when comparable evidence exists.
Documentation publication has its own [source-pinned CI receipt](documentation.md);
a site build does not validate the native widget library.
