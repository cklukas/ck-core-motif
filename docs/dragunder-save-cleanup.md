---
nav_exclude: true
---

# Drag-under backing-store cleanup

`SaveAll` and `SaveSegments` now reset `numSavedPixmaps` when their backing
store record allocation fails. Later animation cleanup therefore skips the
missing array instead of treating a null pointer as a saved-pixmap list.

The focused source contract is checked by
`tests/automated/check-dragunder-save-cleanup.py`.
