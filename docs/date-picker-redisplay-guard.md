---
nav_exclude: true
---
# DatePicker redisplay guard

`XmDatePicker` now skips redisplay safely when its drawing GC could not be
allocated. The widget remains usable for date state and callbacks, while Xlib
is never called with a null GC. Callback routing compares resource names by
content, so open-change notifications are not dependent on string interning.

The focused source contract is checked by
`tests/automated/check-date-picker-redisplay-guard.py`.
