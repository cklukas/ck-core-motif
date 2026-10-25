---
nav_exclude: true
---
# WSM WM_WINDOWS property allocation failure

WSM `WM_WINDOWS` request unpacking now checks both the window-property and
match-attribute counts against Xt's `Cardinal` allocation width before copying
wire values. A failed or unrepresentable allocation clears the corresponding
state and returns before writing through a null pointer.

`tests/automated/check-wsm-wm-windows-oom.py` protects this failure path.
