---
nav_exclude: true
---
# Xm pixmap-data validation

`_XmGetPixmapData` now rejects a null screen, unspecified pixmap, missing
output pointers, or an unavailable cache before dereferencing them. Valid
outputs are cleared before lookup, so a failed lookup cannot expose stale
metadata. Successful lookups retain the existing values and internal name
pointer contract.

The source-level regression is
`tests/automated/check-xm-get-pixmap-data-validation.py`.
