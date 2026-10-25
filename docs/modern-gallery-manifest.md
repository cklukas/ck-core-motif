# Modern gallery component manifest

This document is the WP-05.0 inventory for the planned `ckmotif-gallery`.
The machine-readable source is
[`modern-gallery-manifest.json`](modern-gallery-manifest.json).  It is not a
second widget API and it does not claim that the gallery examples exist yet.
It records the public classes that must receive an example, together with the
explicit exclusions that are not constructible visual components.

## Inventory contract

The inventory source is `lib/Xm/XmAll.h`, the public aggregate header shipped
by this tree.  `tests/automated/check-gallery-manifest.py` parses its public
header includes and class declarations and then checks that the JSON contains
exactly one record for every declaration.  A missing header, stale class
symbol, duplicate identifier, invalid category, or incomplete planned record
fails the check.  This makes adding a public widget a deliberate gallery
maintenance event rather than an easy-to-miss documentation omission.

Each record has:

- a stable `id`, header and class symbol;
- the class kind and whether it is constructible;
- one of the gallery navigation categories;
- owners for construction, source-fragment extraction, interaction testing,
  documentation and screenshots; and
- a disposition (`planned-example` or an explicit non-constructible
  exclusion).

The `gallery.*` references are stable acceptance identifiers until the gallery
implementation supplies the corresponding source fragments, tests, anchors and
captures.  They intentionally do not point at invented files or pretend that
planned work has passed.

## Current baseline

The inventory currently contains 94 public class declarations: 85
constructible widgets or gadgets and nine explicit base/protocol/object
exclusions.  The excluded records cover Xt/Motif infrastructure whose class
records are not standalone visual examples, such as the display, screen,
manager and primitive bases, drag/slide contexts, and Motif object classes.

Run the baseline check from the repository root:

```sh
python3 tests/automated/check-gallery-manifest.py
```

This package is complete only as an inventory baseline.  The later gallery
packages must replace each `planned-example` disposition with a verified live
example, native interaction coverage, a source fragment, documentation anchor
and provenance-checked screenshot.
