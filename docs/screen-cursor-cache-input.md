---
nav_exclude: true
---

# Screen cursor-cache removal input validation

`_XmScreenRemoveFromCursorCache` now returns safely for a missing drag icon or
screen object before querying the cursor cache. Valid cache removal remains
unchanged.

The focused regression check is
`tests/automated/check-screen-cursor-cache-input.py`.
