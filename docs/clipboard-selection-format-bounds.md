---
nav_exclude: true
---

# Clipboard selection format bounds

`ClipboardGetSelection` now validates the stored X selection format against
the only supported values (8, 16, and 32) before narrowing it to the legacy
`int *format` output.

Regression: `tests/automated/check-clipboard-selection-format-bounds.py`
