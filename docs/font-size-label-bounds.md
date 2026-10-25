---
nav_exclude: true
---
# Font selector size-label formatting

The font selector builds the list of available size labels from the global
size table.  Those values are numeric and were formatted with `sprintf` into
a 10-byte buffer, leaving no margin for a larger signed value or a future
table entry.

The label buffer now has 32 bytes and uses `snprintf`.  Existing size labels
remain identical, while a larger value cannot overwrite adjacent stack data.
The source regression in `tests/automated/check-font-size-name-bounds.py`
protects this bounded path.
