---
nav_exclude: true
---

# XPM extension error status

Malformed extension metadata is caller input, not an allocation failure.  The
array and buffer serializers now report `XpmFileInvalid` when extension names
or line arrays are missing or inconsistent; genuine allocation failures retain
their existing `XpmNoMemory` status.

Regression: `tests/automated/check-xpm-extension-error-status.py`
