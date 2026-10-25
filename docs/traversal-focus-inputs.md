---
nav_exclude: true
---

# Traversal focus-data input validation

`_XmDestroyFocusData` and `_XmSetActiveTabGroup` now treat a missing focus
data record as a safe no-op. Existing focus-data destruction and active-tab
updates are unchanged when a valid record is supplied.

The focused regression check is
`tests/automated/check-traversal-focus-inputs.py`.
