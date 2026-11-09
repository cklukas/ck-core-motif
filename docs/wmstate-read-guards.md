---
nav_exclude: true
---
# WM_STATE read guards

`GetWMState` now requires the expected 32-bit format, exact item count,
complete X11 result, and non-null payload before returning a WM_STATE property.
Invalid properties are freed and reported as absent.
