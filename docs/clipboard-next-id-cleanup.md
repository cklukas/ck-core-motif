---
nav_exclude: true
---

# Clipboard next-item ID cleanup

`ClipboardGetNewItemId` receives a temporary property buffer from
`ClipboardFindItem`, increments and persists the next ID, then now releases
that buffer before returning.  Invalid-property handling is unchanged.

Regression: `tests/automated/check-clipboard-next-id-cleanup.py`
