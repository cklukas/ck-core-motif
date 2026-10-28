---
nav_exclude: true
---

# Clipboard named-conversion result width

`ClipboardGetByNameItem` now stores the result of
`ClipboardRequestDataAndWait` in an `int`, matching the callee’s return type
instead of narrowing it to `short` before testing success.

Regression: `tests/automated/check-clipboard-by-name-result-width.py`
