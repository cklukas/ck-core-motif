---
nav_exclude: true
---

# Badge count API inputs

`XmBadgeGetDisplayedCount` now rejects a null or non-Badge widget before
casting it to the private Badge instance. It keeps the existing `-1` neutral
result for invalid handles; valid count and maximum handling is unchanged.

The focused regression check is `tests/automated/check-badge-inputs.py`.
