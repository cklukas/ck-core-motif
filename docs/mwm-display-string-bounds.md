---
nav_exclude: true
---

# MWM display-string bounds

Per-screen MWM display strings are derived from the X display name and screen
number. Construction now checks the allocation-size arithmetic and uses
`snprintf`, rejecting truncation before publishing a partial display string.
The private display-name copy uses the checked Xt `Cardinal` width explicitly
after the representability test, and the final per-screen copy uses that same
validated width.
Normal display names and screen formatting remain unchanged.

The focused regression check is
`tests/automated/check-mwm-display-string-bounds.py`.
