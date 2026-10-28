---
nav_exclude: true
---

# Clipboard next-item ID setter cleanup

`ClipboardSetNextItemId` receives a temporary `XM_NEXT_ID` property buffer,
persists the newly selected ID, and now releases that buffer before returning.
The ID selection and invalid-property behavior are unchanged.

Regression: `tests/automated/check-clipboard-set-next-id-cleanup.py`
