---
nav_exclude: true
---

# Clipboard conversion format cleanup

The selection conversion helper temporarily owns the format record returned by
`ClipboardFindFormat`.  It now releases that record after any cut-by-name
callback request completes, preserving the callback result while avoiding a
leak on every conversion.

Regression: `tests/automated/check-clipboard-convert-format-cleanup.py`
