---
nav_exclude: true
---
# TimePicker redisplay and callback guards

`XmTimePicker` now skips drawing safely when its X11 GC is unavailable. Callback
routing compares resource names by content rather than pointer identity, so
activate and value-change callbacks remain correctly separated across callers.

The focused source contract is checked by
`tests/automated/check-time-picker-redisplay-guard.py`.
