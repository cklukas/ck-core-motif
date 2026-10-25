---
nav_exclude: true
---
# Screen widget index scan

`XmGetXmScreen` first checks the existing child widgets of the Motif display.
That loop leaves its index at the child count.  The implied-creation path then
needs to scan the X display's screens, whose indexes are independent of the
child count.

The function now resets the index to zero before calling `ScreenOfDisplay`.
This preserves the existing screen lookup behavior and avoids skipping screens
or using an out-of-range display index when the display has several Motif
children.  The source regression in
`tests/automated/check-screen-index-scan.py` protects the reset.
