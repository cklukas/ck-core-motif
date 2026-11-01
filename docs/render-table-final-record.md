---
nav_exclude: true
---

# Render-table final-record handling

`XmRenderTableCvtFromProp` accepts property strings supplied by other X11
clients. A valid final rendition no longer needs a trailing newline: once the
last field has been parsed, the importer records that rendition before acting
on end-of-input. This matches the documented property example and preserves
the normal newline-terminated output from `XmRenderTableCvtToProp`.

If rendition-array growth cannot be completed, the newly created rendition is
released before the importer returns its existing partial-result form. Public
syntax and successful conversion results are otherwise unchanged.

The focused regression check is
`tests/automated/check-render-table-final-record.py`.
