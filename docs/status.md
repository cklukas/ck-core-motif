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

## Evidence to retain

For an admitted build, retain source commit and dirty-state proof, configure
options, dependency/compiler versions, start/end/elapsed time, exact prefix,
header and library hashes, SONAMEs, runtime-loaded paths and fixture outputs.
Record a measured duration baseline only when comparable evidence exists.
Documentation publication has its own [source-pinned CI receipt](documentation.md);
a site build does not validate the native widget library.
