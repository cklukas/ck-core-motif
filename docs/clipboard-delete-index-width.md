---
nav_exclude: true
---

# Clipboard deletion index width

`ClipboardDeleteItem` now uses an unsigned-long traversal counter matching
`header->currItems`, while keeping a separate signed index for the older/newer
candidate search that intentionally uses `-1`. Large item lists no longer
depend on signed-counter wraparound.

Regression: `tests/automated/check-clipboard-delete-index-width.py`
