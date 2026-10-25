---
nav_exclude: true
---

# Info-bar text geometry bounds

`XmInfoBar` now bounds prefix positioning and text lengths before passing them
to Xlib. Normal messages retain their existing layout; unusually long strings
no longer wrap signed coordinates or length arguments.

The focused regression check is
`tests/automated/check-info-bar-text-bounds.py`.
