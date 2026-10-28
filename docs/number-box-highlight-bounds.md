---
nav_exclude: true
---

# NumberBox highlight bounds

NumberBox now draws its focus/highlight frame only when both widget dimensions
exceed twice the highlight thickness.  Explicitly tiny controls therefore do
not wrap the frame dimensions passed to Xlib.

`tests/automated/check-number-box-highlight-bounds.py` protects this guard.
