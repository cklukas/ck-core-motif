---
nav_exclude: true
---

# Standard WM property status handling

The standard conversion paths for `WM_CLASS` and `WM_NAME` now require a
successful, complete `XGetWindowProperty` response.  Failed or truncated
properties are released and the ancestor search continues, preventing
partial X data from entering the text or class conversion paths.

Regression: `tests/automated/check-standard-convert-property-status.py`
