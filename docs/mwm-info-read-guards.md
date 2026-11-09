---
nav_exclude: true
---
# MWM info read guards

`GetMwmInfo` now accepts the root `_MOTIF_WM_INFO` property only when its
format is 32-bit, its item count matches the expected structure, X11 reports a
complete result, and the payload is non-null. Invalid properties are released
and treated as absent.
