---
nav_exclude: true
---

# Clipboard record-type length guard

`ClipboardFindItem` now verifies that a returned property contains at least
the record-type field before checking its value.  Short or empty properties
are rejected and cleaned up without dereferencing beyond the returned X
buffer.

Regression: `tests/automated/check-clipboard-record-type-length.py`
