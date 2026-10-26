---
nav_exclude: true
---

# DataField picture-source ownership

DataField `SetValues` now duplicates a changed picture source before releasing
the old owned string. If `XtNewString` fails, the old source remains owned and
the new widget state reuses it instead of losing the picture definition or
freeing storage that is still needed.

The focused source contract is checked by
`tests/automated/check-datafield-picture-source.py`.
